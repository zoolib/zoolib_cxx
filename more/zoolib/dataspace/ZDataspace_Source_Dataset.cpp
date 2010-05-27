/* -------------------------------------------------------------------------------------------------
Copyright (c) 2010 Andrew Green
http://www.zoolib.org

Permission is hereby granted, free of charge, to any person obtaining a copy of this software
and associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software
is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE COPYRIGHT HOLDER(S) BE LIABLE FOR ANY CLAIM, DAMAGES
OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
------------------------------------------------------------------------------------------------- */

#include "zoolib/ZStream_Data_T.h"
#include "zoolib/ZStrim_Stream.h"
#include "zoolib/ZStrimU_Unreader.h"
#include "zoolib/ZStrimmer_Streamer.h"
#include "zoolib/ZUtil_STL.h"
#include "zoolib/ZYad_Any.h"
#include "zoolib/ZYad_ZooLibStrim.h"

#include "zoolib/dataspace/ZDataspace_Source_Dataset.h"

#include "zoolib/zqe/ZQE_Iterator_Any.h"
#include "zoolib/zqe/ZQE_Result_Any.h"
#include "zoolib/zqe/ZQE_Iterator_Any.h"
#include "zoolib/zra/ZRA_Expr_Rel_Concrete.h"

namespace ZooLib {
namespace ZDataspace {

using ZDataset::ClockedDeltas;
using ZDataset::Daton;
using ZDataset::Dataset;
using ZDataset::Map_NamedClock_Delta_t;
using ZDataset::NamedClock;

/*
Minimally, keep an index of property names in contents -- that way we can efficiently
identify those entities that satisfy a RelHead.
*/

// =================================================================================================
#pragma mark -
#pragma mark * spAsVal (anonymous)

namespace { // anonymous

ZVal_Any spAsVal(const Daton& iDaton)
	{
	try
		{
		ZRef<ZStreamerR> theStreamerR =
			new ZStreamerRPos_T<ZStreamRPos_Data_T<ZData_Any> >(iDaton.GetData());
		
		ZRef<ZStrimmerR> theStrimmerR_StreamUTF8 =
			new ZStrimmerR_Streamer_T<ZStrimR_StreamUTF8>(theStreamerR);

		ZRef<ZStrimmerU> theStrimmerU_Unreader =
			new ZStrimmerU_FT<ZStrimU_Unreader>(theStrimmerR_StreamUTF8);

		ZRef<ZYadR> theYadR = ZYad_ZooLibStrim::sMakeYadR(theStrimmerU_Unreader);

		return sFromYadR(ZVal_Any(), theYadR);
		}
	catch (...)
		{
		return ZVal_Any();
		}
	}

} // anonymous namespace

// =================================================================================================
#pragma mark -
#pragma mark * Iterator (anonymous)

namespace { // anonymous

class Iterator : public ZQE::Iterator
	{
public:
	typedef map<Daton, pair<NamedClock, ZVal_Any> > Map_t;

	Iterator(const RelHead& iRelHead, Map_t::const_iterator iCurrent, Map_t::const_iterator iEnd);

	virtual ~Iterator();
	
	virtual ZRef<ZQE::Iterator> Clone();
	virtual ZRef<ZQE::Result> ReadInc();

private:
	const RelHead fRelHead;
	Map_t::const_iterator fCurrent;
	const Map_t::const_iterator fEnd;
	};

Iterator::Iterator(
	const RelHead& iRelHead, Map_t::const_iterator iCurrent, Map_t::const_iterator iEnd)
:	fRelHead(iRelHead)
,	fCurrent(iCurrent)
,	fEnd(iEnd)
	{}

Iterator::~Iterator()
	{}

ZRef<ZQE::Iterator> Iterator::Clone()
	{ return new Iterator(fRelHead, fCurrent, fEnd); }

ZRef<ZQE::Result> Iterator::ReadInc()
	{
	while (fCurrent != fEnd)
		{
		const ZVal_Any& theVal = (*fCurrent).second.second;
		if (const ZMap_Any* theMap = theVal.PGet_T<ZMap_Any>())
			{
			ZRA::RelHead sourceRelHead;
			ZMap_Any newMap;
			for (ZMap_Any::Index_t i = theMap->Begin(); i != theMap->End(); ++i)
				{
				const string theName = theMap->NameOf(i);
				sourceRelHead |= theName;
				if (fRelHead.Contains(theName))
					newMap.Set(theName, theMap->Get(i));
				}

			if (sourceRelHead.Contains(fRelHead))
				{
				ZRef<ZQE::Result_Any> result = new ZQE::Result_Any(newMap);
				result->AddAnnotation(new Annotation_Daton((*fCurrent).first));
				++fCurrent;
				return result;
				}
			}
		++fCurrent;
		}
	return nullref;
	}

} // anonymous namespace

// =================================================================================================
#pragma mark -
#pragma mark * Visitor_DoMakeIterator (anonymous)

namespace { // anonymous

class Visitor_DoMakeIterator
:	public virtual ZQE::Visitor_DoMakeIterator_Any
,	public virtual ZRA::Visitor_Expr_Rel_Concrete
	{
public:
	Visitor_DoMakeIterator(Source_Dataset* iSource);

	virtual void Visit_Expr_Rel_Concrete(ZRef<ZRA::Expr_Rel_Concrete> iExpr);	

private:
	Source_Dataset* fSource;
	};

Visitor_DoMakeIterator::Visitor_DoMakeIterator(Source_Dataset* iSource)
:	fSource(iSource)
	{}

void Visitor_DoMakeIterator::Visit_Expr_Rel_Concrete(ZRef<ZRA::Expr_Rel_Concrete> iExpr)
	{
	this->pSetResult(fSource->pMakeIterator(iExpr->GetRelHead()));
	}

} // anonymous namespace

// =================================================================================================
#pragma mark -
#pragma mark * Source_Dataset::Query

class Source_Dataset::Query
	{
public:
	int64 fRefcon;
	ZRef<ZRA::Expr_Rel> fRel;
	ZRef<ZQE::Result> fResults;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Source_Dataset

Source_Dataset::Source_Dataset(ZRef<Dataset> iDataset)
:	fDataset(iDataset)
	{}

Source_Dataset::~Source_Dataset()
	{}

set<RelHead> Source_Dataset::GetRelHeads()
	{ return set<RelHead>(); }

void Source_Dataset::Update(
	bool iLocalOnly,
	AddedSearch* iAdded, size_t iAddedCount,
	int64* iRemoved, size_t iRemovedCount,
	vector<SearchResult>& oChanged,
	Clock& oClock)
	{
	oChanged.clear();

	while (iAddedCount--)
		{
		Query* theQuery = new Query;
		theQuery->fRefcon = iAdded->fRefcon;
		theQuery->fRel = iAdded->fRel;
		fRels[theQuery->fRefcon] = theQuery;
		++iAdded;
		}
	
	while (iRemovedCount--)
		{
		map<int64, Query*>::iterator i = fRels.find(*iRemoved++);
		delete (*i).second;
		}

	// Pick up (and index) values from dataset
	this->pPull();

	for (map<int64, Query*>::iterator i = fRels.begin(); i != fRels.end(); ++i)
		{
		SearchResult theSearchResult;
		theSearchResult.fRefcon = (*i).first;

		ZRef<ZQE::Iterator> theIterator = Visitor_DoMakeIterator(this).Do((*i).second->fRel);
		for (;;)
			{
			if (ZRef<ZQE::Result> theResult = theIterator->ReadInc())
				theSearchResult.fResults.push_back(theResult);
			else
				break;
			}
		oChanged.push_back(theSearchResult);
		}

	oClock = fClock;
	}

void Source_Dataset::Dump(const ZStrimW& w)
	{
	for (map<Daton, pair<NamedClock, ZVal_Any> >::iterator i = fMap.begin(); i != fMap.end(); ++i)
		{
		ZYad_ZooLibStrim::sToStrim(sMakeYadR((*i).second.second), w);
		w << "\n";
		}
	}

ZRef<ZQE::Iterator> Source_Dataset::pMakeIterator(const RelHead& iRelHead)
	{ return new Iterator(iRelHead, fMap.begin(), fMap.end()); }

void Source_Dataset::pPull()
	{
	Clock newClock;
	ZRef<ClockedDeltas> clockedDeltas;
	fDataset->GetClockedDeltas(fClock, newClock, clockedDeltas);
	fClock = newClock;

	const Map_NamedClock_Delta_t& theNCDM = clockedDeltas->GetMap();
	for (Map_NamedClock_Delta_t::const_iterator iterNCDM = theNCDM.begin(), endNCDM = theNCDM.end();
		iterNCDM != endNCDM; ++iterNCDM)
		{
		const NamedClock& theNamedClock = (*iterNCDM).first;
		const map<Daton, bool>& theStatements = (*iterNCDM).second->GetStatements();
		for (map<Daton, bool>::const_iterator iterStmts = theStatements.begin();
			iterStmts != theStatements.end(); ++iterStmts)
			{
			const Daton& theDaton = (*iterStmts).first;
			const pair<NamedClock, ZVal_Any> newPair(theNamedClock, spAsVal(theDaton));

			map<Daton, pair<NamedClock, ZVal_Any> >::iterator iterMap = fMap.find(theDaton);

			if (iterMap == fMap.end() || (*iterMap).first != theDaton)
				{
				if ((*iterStmts).second)
					fMap.insert(iterMap, make_pair(theDaton, newPair));
				}
			else if ((*iterMap).second.first < theNamedClock)
				{
				// Only change if theNamedClock is more recent than what we've got.
				if ((*iterStmts).second)
					(*iterMap).second = newPair;
				else
					fMap.erase(iterMap);
				}
			}
		}
	}

} // namespace ZDataspace
} // namespace ZooLib
