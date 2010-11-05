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
#include "zoolib/dataspace/ZDataspace_Util_Strim.h"

#include "zoolib/zqe/ZQE_Visitor_DoMakeWalker_Any.h"
#include "zoolib/zqe/ZQE_Walker_Product.h"
#include "zoolib/zqe/ZQE_Walker_ValPredCompound.h"

namespace ZooLib {
namespace ZDataspace {

using ZDataset::Dataset;
using ZDataset::Daton;
using ZDataset::Deltas;
using ZDataset::Map_NamedEvent_Delta_t;
using ZDataset::NamedEvent;

using ZRA::NameMap;

using std::map;
using std::pair;
using std::set;
using std::vector;

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
#pragma mark * Walker (anonymous)

namespace { // anonymous

class Walker : public ZQE::Walker
	{
public:
	typedef map<Daton, pair<NamedEvent, ZVal_Any> > Map_t;

	Walker(const NameMap& iNameMap, Map_t::const_iterator iCurrent, Map_t::const_iterator iEnd);
	Walker(const vector<pair<string8, string8> >& iNames,
		Map_t::const_iterator iCurrent, Map_t::const_iterator iEnd);

	virtual ~Walker();

// From ZQE::Walker
	virtual size_t NameCount();
	virtual string8 NameAt(size_t iIndex);

	virtual ZRef<ZQE::Walker> Clone();
	virtual ZRef<ZQE::Row> ReadInc();

private:
	vector<pair<string8, string8> > fNames;
	Map_t::const_iterator fCurrent;
	const Map_t::const_iterator fEnd;
	};

Walker::Walker(const NameMap& iNameMap, Map_t::const_iterator iCurrent, Map_t::const_iterator iEnd)
:	fNames(iNameMap.GetElems().begin(), iNameMap.GetElems().end())
,	fCurrent(iCurrent)
,	fEnd(iEnd)
	{}

Walker::Walker(const vector<pair<string8, string8> >& iNames,
	Map_t::const_iterator iCurrent, Map_t::const_iterator iEnd)
:	fNames(iNames)
,	fCurrent(iCurrent)
,	fEnd(iEnd)
	{}

Walker::~Walker()
	{}

size_t Walker::NameCount()
	{ return fNames.size(); }

string8 Walker::NameAt(size_t iIndex)
	{
	if (iIndex < fNames.size())
		return fNames[iIndex].first;
	return string8();
	}

ZRef<ZQE::Walker> Walker::Clone()
	{ return new Walker(fNames, fCurrent, fEnd); }

ZRef<ZQE::Row> Walker::ReadInc()
	{
	while (fCurrent != fEnd)
		{
		const ZVal_Any& theVal = fCurrent->second.second;
		if (const ZMap_Any* theMap = theVal.PGet_T<ZMap_Any>())
			{
			vector<ZVal_Any> theVals;
			theVals.reserve(fNames.size());
			bool allGood = true;
			for (size_t x = 0; x < fNames.size(); ++x)
				{
				if (const ZVal_Any* theVal = theMap->PGet(fNames[x].second))
					{
					theVals[x] = *theVal;
					}
				else
					{
					allGood = false;
					break;
					}
				}

			if (allGood)
				{
				ZRef<ZQE::Row_Vector> theRow = new ZQE::Row_Vector(&theVals);
				theRow->AddAnnotation(new Annotation_Daton(fCurrent->first));
				++fCurrent;
				return theRow;
				}
			}
		++fCurrent;
		}
	return null;
	}

} // anonymous namespace

// =================================================================================================
#pragma mark -
#pragma mark * Source_Dataset::PQuery

class Source_Dataset::PQuery
	{
public:
	int64 fRefcon;
	SearchSpec fSearchSpec;
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
	const AddedSearch* iAdded, size_t iAddedCount,
	const int64* iRemoved, size_t iRemovedCount,
	vector<SearchResult>& oChanged,
	ZRef<Event>& oEvent)
	{
	oChanged.clear();

	while (iAddedCount--)
		{
		PQuery* thePQuery = new PQuery;
		thePQuery->fRefcon = iAdded->fRefcon;
		thePQuery->fSearchSpec = iAdded->fSearchSpec;
		ZUtil_STL::sInsertMustNotContain(kDebug,
			fMap_RefconToPQuery, thePQuery->fRefcon, thePQuery);

		++iAdded;
		}

	while (iRemovedCount--)
		{
		PQuery* thePQuery = ZUtil_STL::sEraseAndReturn(kDebug, fMap_RefconToPQuery, *iRemoved++);
		delete thePQuery;
		}

	// Pick up (and index) values from dataset
	this->pPull();

	for (map<int64, PQuery*>::iterator i = fMap_RefconToPQuery.begin();
		i != fMap_RefconToPQuery.end(); ++i)
		{
		SearchResult theSearchResult;
		theSearchResult.fRefcon = i->first;
		const vector<NameMap>& theNM = i->second->fSearchSpec.fNameMaps;

		ZRef<ZQE::Walker> theWalker;
		for (vector<NameMap>::const_iterator iterNM = theNM.begin();
			iterNM != theNM.end(); ++iterNM)
			{
			ZRef<ZQE::Walker> cur = new Walker(*iterNM, fMap.begin(), fMap.end());
			if (theWalker)
				theWalker = new ZQE::Walker_Product(theWalker, cur);
			else
				theWalker = cur;
			}

		theWalker =
			new ZQE::Walker_ValPredCompound(theWalker, i->second->fSearchSpec.fPredCompound);

		vector<string8> theRowHead;
		for (size_t x = 0; x < theWalker->NameCount(); ++x)
			theRowHead.push_back(theWalker->NameAt(x));

		vector<ZRef<ZQE::Row> > theRows;

		for (;;)
			{
			if (ZRef<ZQE::Row> theRow = theWalker->ReadInc())
				theRows.push_back(theRow);
			else
				break;
			}

		oChanged.push_back(theSearchResult);
		}

	oEvent = fEvent;
	}

void Source_Dataset::Dump(const ZStrimW& w)
	{
	for (map<Daton, pair<NamedEvent, ZVal_Any> >::iterator i = fMap.begin(); i != fMap.end(); ++i)
		{
		ZYad_ZooLibStrim::sToStrim(sMakeYadR(i->second.second), w);
		w << "\n";
		}
	}

ZRef<ZQE::Walker> Source_Dataset::pMakeWalker(const RelHead& iRelHead)
	{ return new Walker(iRelHead, fMap.begin(), fMap.end()); }

void Source_Dataset::pPull()
	{
	ZRef<Deltas> theDeltas;
	fEvent = fDataset->GetDeltas(theDeltas, fEvent);

	const Map_NamedEvent_Delta_t& theMNED = theDeltas->GetMap();
	for (Map_NamedEvent_Delta_t::const_iterator
		iterMNED = theMNED.begin(), endMNED = theMNED.end();
		iterMNED != endMNED; ++iterMNED)
		{
		const NamedEvent& theNamedEvent = iterMNED->first;
		const map<Daton, bool>& theStatements = iterMNED->second->GetStatements();
		for (map<Daton, bool>::const_iterator
			iterStmts = theStatements.begin(), endStmts = theStatements.end();
			iterStmts != endStmts; ++iterStmts)
			{
			const Daton& theDaton = iterStmts->first;
			const pair<NamedEvent, ZVal_Any> newPair(theNamedEvent, spAsVal(theDaton));

			map<Daton, pair<NamedEvent, ZVal_Any> >::iterator iterMap = fMap.find(theDaton);

			if (iterMap == fMap.end() || iterMap->first != theDaton)
				{
				if (iterStmts->second)
					fMap.insert(iterMap, make_pair(theDaton, newPair));
				}
			else if (iterMap->second.first < theNamedEvent)
				{
				// theNamedEvent is more recent than what we've got and thus supersedes it.
				if (iterStmts->second)
					iterMap->second = newPair;
				else
					fMap.erase(iterMap);
				}
			}
		}
	}

} // namespace ZDataspace
} // namespace ZooLib
