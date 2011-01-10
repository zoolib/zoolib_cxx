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

#include "zoolib/ZLog.h"
#include "zoolib/ZStream_Data_T.h"
#include "zoolib/ZStrim_Stream.h"
#include "zoolib/ZStrimU_Unreader.h"
#include "zoolib/ZStrimmer_Streamer.h"
#include "zoolib/ZUtil_STL.h"
#include "zoolib/ZYad_Any.h"
#include "zoolib/ZYad_ZooLibStrim.h"

#include "zoolib/dataspace/ZDataspace_Source_Dataset.h"

#include "zoolib/zqe/ZQE_Search.h"
#include "zoolib/zqe/ZQE_Visitor_DoMakeWalker.h"

#include "zoolib/zra/ZRA_Expr_Rel_Concrete.h"
#include "zoolib/zra/ZRA_Util_Strim_Rel.h"

namespace ZooLib {
namespace ZDataspace {

using ZDataset::Dataset;
using ZDataset::Daton;
using ZDataset::Deltas;
using ZDataset::Map_NamedEvent_Delta_t;
using ZDataset::NamedEvent;

using std::make_pair;
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

ZVal_Any spAsVal(const ZData_Any& iData)
	{
	try
		{
		ZRef<ZStreamerR> theStreamerR =
			new ZStreamerRPos_T<ZStreamRPos_Data_T<ZData_Any> >(iData);

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
#pragma mark * Daton/val conversion.

ZVal_Any sAsVal(const Daton& iDaton)
	{ return spAsVal(iDaton.GetData()); }

Daton sAsDaton(const ZVal_Any& iVal)
	{
	ZData_Any theData;
	ZYad_ZooLibStrim::sToStrim(sMakeYadR(iVal),
		ZStrimW_StreamUTF8(ZStreamRWPos_Data_T<ZData_Any>(theData)));
	return theData;
	}

// =================================================================================================
#pragma mark -
#pragma mark * Visitor_DoMakeWalker

class Source_Dataset::Visitor_DoMakeWalker
:	public virtual ZQE::Visitor_DoMakeWalker
,	public virtual ZRA::Visitor_Expr_Rel_Concrete
	{
public:
	Visitor_DoMakeWalker(ZRef<Source_Dataset> iSource)
	:	fSource(iSource)
		{}

	virtual void Visit_Expr_Rel_Concrete(const ZRef<ZRA::Expr_Rel_Concrete>& iExpr)
		{ this->pSetResult(fSource->pMakeWalker(iExpr->GetConcreteRelHead())); }

	ZRef<Source_Dataset> fSource;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Source_Dataset::Walker

class Source_Dataset::Walker : public ZQE::Walker
	{
public:
	Walker(ZRef<Source_Dataset> iSource, const vector<string8>& iNames)
	:	fSource(iSource)
	,	fNames(iNames)
		{}

	virtual ~Walker()
		{}

// From ZQE::Walker
	virtual void Rewind()
		{ fSource->pRewind(this); }

	virtual void Prime(const map<string8,size_t>& iBindingOffsets, 
		map<string8,size_t>& oOffsets,
		size_t& ioBaseOffset)
		{ fSource->pPrime(this, iBindingOffsets, oOffsets, ioBaseOffset); }

	virtual bool ReadInc(const ZVal_Any* iBindings,
		ZVal_Any* oResults,
		set<ZRef<ZCounted> >* oAnnotations)
		{ return fSource->pReadInc(this, iBindings, oResults, oAnnotations); }

	ZRef<Source_Dataset> fSource;
	size_t fBaseOffset;
	vector<string8> fNames;
	Map_Main::const_iterator fCurrent_Main;
	Map_Pending::const_iterator fCurrent_Pending;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Source_Dataset::ClientSearch

class Source_Dataset::DLink_ClientSearch_InPSearch
:	public DListLink<ClientSearch, DLink_ClientSearch_InPSearch, kDebug>
	{};

class Source_Dataset::ClientSearch
:	public Source_Dataset::DLink_ClientSearch_InPSearch
	{
public:
	ClientSearch(int64 iRefcon, PSearch* iPSearch)
	:	fRefcon(iRefcon),
		fPSearch(iPSearch)
		{}

	int64 fRefcon;
	PSearch* fPSearch;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Source_Dataset::PSearch

class Source_Dataset::PSearch
	{
public:
	PSearch(ZRef<ZRA::Expr_Rel> iRel)
	:	fRel(iRel)
		{}

	ZRef<ZRA::Expr_Rel> fRel;
	DListHead<DLink_ClientSearch_InPSearch> fClientSearches;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Source_Dataset

void Source_Dataset::ForceUpdate()
	{
	fStackChanged = true;
	this->pInvokeCallable_ResultsAvailable();
	}

Source_Dataset::Source_Dataset(ZRef<Dataset> iDataset)
:	fDataset(iDataset)
,	fEvent(Event::sZero())
,	fStackChanged(false)
	{}

Source_Dataset::~Source_Dataset()
	{}

RelHead Source_Dataset::GetRelHead()
	{ return RelHead(); }

void Source_Dataset::ModifyRegistrations(
	const AddedSearch* iAdded, size_t iAddedCount,
	const int64* iRemoved, size_t iRemovedCount)
	{
	if (iAddedCount || iRemovedCount)
		{
		fStackChanged = true;
		this->pInvokeCallable_ResultsAvailable();
		}

	for (/*no init*/; iAddedCount--; ++iAdded)
		{
		ZRef<ZRA::Expr_Rel> theRel = iAdded->GetRel();
		pair<Map_RelToPSearch::iterator,bool> iterPSearchPair =
			fMap_RelToPSearch.insert(make_pair(theRel, PSearch(theRel)));

		if (!iterPSearchPair.second)
			{
			if (ZLOGF(s, eDebug))
				s << "Reusing exisiting PSearch";
			}

		const Map_RelToPSearch::iterator& iterPSearch = iterPSearchPair.first;
		PSearch* thePSearch = &iterPSearch->second;

		const int64 theRefcon = iAdded->GetRefcon();

		std::map<int64, ClientSearch>::iterator iterClientSearch =
			fMap_RefconToClientSearch.insert(
			make_pair(theRefcon, ClientSearch(theRefcon, thePSearch))).first;

		thePSearch->fClientSearches.Insert(&iterClientSearch->second);
		}

	while (iRemovedCount--)
		{
		int64 theRefcon = *iRemoved++;

		std::map<int64, ClientSearch>::iterator iterClientSearch =
			fMap_RefconToClientSearch.find(theRefcon);

		ZAssertStop(kDebug, iterClientSearch != fMap_RefconToClientSearch.end());
		
		ClientSearch* theClientSearch = &iterClientSearch->second;
		
		PSearch* thePSearch = theClientSearch->fPSearch;
		thePSearch->fClientSearches.Erase(theClientSearch);
		if (thePSearch->fClientSearches.Empty())
			ZUtil_STL::sEraseMustContain(kDebug, fMap_RelToPSearch, thePSearch->fRel);
		
		fMap_RefconToClientSearch.erase(iterClientSearch);
		}
	}

void Source_Dataset::CollectResults(vector<SearchResult>& oChanged)
	{
	oChanged.clear();

	bool anyChanges = fStackChanged;
	fStackChanged = false;

	// Pick up (and index) values from dataset
	if (this->pPull())
		anyChanges = true;
	
	if (anyChanges)
		{
		for (Map_RelToPSearch::iterator iterPSearch = fMap_RelToPSearch.begin();
			iterPSearch != fMap_RelToPSearch.end(); ++iterPSearch)
			{
			PSearch* thePSearch = &iterPSearch->second;
			ZRef<ZQE::Walker> theWalker = Visitor_DoMakeWalker(this).Do(thePSearch->fRel);

			if (!theWalker)
				{
				if (ZLOGF(s, eDebug))
					{
					s << "\n";
					ZRA::Util_Strim_Rel::sToStrim(thePSearch->fRel, s);
					}
				}

			ZRef<ZQE::Result> theResult = sSearch(theWalker);
			for (DListIterator<ClientSearch, DLink_ClientSearch_InPSearch>
				iterCS = thePSearch->fClientSearches; iterCS; iterCS.Advance())
				{
				oChanged.push_back(SearchResult(iterCS.Current()->fRefcon, theResult, fEvent));
				}
			}
		}
	}

ZRef<ZDataset::Dataset> Source_Dataset::GetDataset()
	{ return fDataset; }

void Source_Dataset::Insert(const Daton& iDaton)
	{ this->pModify(iDaton, sAsVal(iDaton), true); }

void Source_Dataset::Erase(const Daton& iDaton)
	{ this->pModify(iDaton, sAsVal(iDaton), false); }

size_t Source_Dataset::OpenTransaction()
	{
	fStackChanged = true;
	fStack.push_back(fMap_Pending);
	return fStack.size();
	}

void Source_Dataset::ClearTransaction(size_t iIndex)
	{
	fStackChanged = true;
	ZAssert(iIndex == fStack.size());
	fMap_Pending = fStack.back();

	this->pInvokeCallable_ResultsAvailable();
	}

void Source_Dataset::CloseTransaction(size_t iIndex)
	{
	fStackChanged = true;
	ZAssert(iIndex == fStack.size());
	fStack.pop_back();
	if (fStack.empty())
		{
		for (Map_Pending::iterator i = fMap_Pending.begin(), end = fMap_Pending.end();
			i != end; ++i)
			{
			if (i->second.second)
				fDataset->Insert(i->first);
			else
				fDataset->Erase(i->first);			
			}
		fMap_Pending.clear();
		}
	this->pInvokeCallable_ResultsAvailable();
	}

void Source_Dataset::pModify(const ZDataset::Daton& iDaton, const ZVal_Any& iVal, bool iSense)
	{
	fStackChanged = true;
	Map_Pending::iterator i = fMap_Pending.find(iDaton);
	if (fMap_Pending.end() == i)
		{
		fMap_Pending.insert(make_pair(iDaton, make_pair(iVal, iSense)));
		}
	else
		{
		ZAssert(i->second.second == !iSense);
		fMap_Pending.erase(i);
		}
	this->pInvokeCallable_ResultsAvailable();	
	}

ZRef<ZQE::Walker> Source_Dataset::pMakeWalker(const RelHead& iRelHead)
	{ return new Walker(this, vector<string8>(iRelHead.begin(), iRelHead.end())); }

void Source_Dataset::pRewind(ZRef<Walker> iWalker)
	{
	iWalker->fCurrent_Main = fMap.begin();
	iWalker->fCurrent_Pending = fMap_Pending.begin();
	}

void Source_Dataset::pPrime(ZRef<Walker> iWalker,
	const map<string8,size_t>& iBindingOffsets, 
	map<string8,size_t>& oOffsets,
	size_t& ioBaseOffset)
	{
	iWalker->fCurrent_Main = fMap.begin();
	iWalker->fCurrent_Pending = fMap_Pending.begin();
	iWalker->fBaseOffset = ioBaseOffset;
	for (size_t x = 0; x < iWalker->fNames.size(); ++x)
		oOffsets[iWalker->fNames[x]] = ioBaseOffset++;
	}

bool Source_Dataset::pReadInc(ZRef<Walker> iWalker,
	const ZVal_Any* iBindings,
	ZVal_Any* oResults,
	set<ZRef<ZCounted> >* oAnnotations)
	{
	while (iWalker->fCurrent_Main != fMap.end())
		{
		// Ignore anything that's in pending (for now).
		if (fMap_Pending.end() == fMap_Pending.find(iWalker->fCurrent_Main->first))
			{
			if (const ZMap_Any* theMap = iWalker->fCurrent_Main->second.second.PGet<ZMap_Any>())
				{
				bool gotAll = true;
				for (size_t x = 0; x < iWalker->fNames.size(); ++x)
					{
					if (const ZVal_Any* theVal = theMap->PGet(iWalker->fNames[x]))
						oResults[iWalker->fBaseOffset + x] = *theVal;
					else
						gotAll = false;
					}

				if (gotAll)
					{
					if (oAnnotations)
						oAnnotations->insert(new Annotation_Daton(iWalker->fCurrent_Main->first));
					++iWalker->fCurrent_Main;
					return true;
					}
				}
			}
		++iWalker->fCurrent_Main;
		}

	// Handle anything pending
	while (iWalker->fCurrent_Pending != fMap_Pending.end())
		{
		if (iWalker->fCurrent_Pending->second.second)
			{
			if (const ZMap_Any* theMap = iWalker->fCurrent_Pending->second.first.PGet<ZMap_Any>())
				{
				bool gotAll = true;
				for (size_t x = 0; x < iWalker->fNames.size(); ++x)
					{
					if (const ZVal_Any* theVal = theMap->PGet(iWalker->fNames[x]))
						oResults[iWalker->fBaseOffset + x] = *theVal;
					else
						gotAll = false;
					}

				if (gotAll)
					{
					if (oAnnotations)
						oAnnotations->insert(new Annotation_Daton(iWalker->fCurrent_Pending->first));
					++iWalker->fCurrent_Pending;
					return true;
					}
				}
			}
		++iWalker->fCurrent_Pending;
		}

	return false;
	}

bool Source_Dataset::pPull()
	{
	// Get our map in sync with fDataset
	ZLOGF(s, eDebug);
	ZRef<Deltas> theDeltas;
	fEvent = fDataset->GetDeltas(theDeltas, fEvent);
	const Map_NamedEvent_Delta_t& theMNED = theDeltas->GetMap();
	s << "theMNED.size()=" << theMNED.size();
	bool anyChanges = false;
	for (Map_NamedEvent_Delta_t::const_iterator
		iterMNED = theMNED.begin(), endMNED = theMNED.end();
		iterMNED != endMNED; ++iterMNED)
		{
		const NamedEvent& theNamedEvent = iterMNED->first;
		const map<Daton, bool>& theStatements = iterMNED->second->GetStatements();
		s << "\ntheStatements.size()=" << theStatements.size();
		for (map<Daton, bool>::const_iterator
			iterStmts = theStatements.begin(), endStmts = theStatements.end();
			iterStmts != endStmts; ++iterStmts)
			{
			const Daton& theDaton = iterStmts->first;
			map<Daton, pair<NamedEvent, ZVal_Any> >::iterator iterMap = fMap.lower_bound(theDaton);
			if (iterMap == fMap.end() || iterMap->first != theDaton)
				{
				if (iterStmts->second)
					{
					anyChanges = true;
					fMap.insert(iterMap,
						make_pair(theDaton,
						pair<NamedEvent, ZVal_Any>(theNamedEvent, sAsVal(theDaton))));
					}
				}
			else if (iterMap->second.first < theNamedEvent)
				{
				// theNamedEvent is more recent than what we've got and thus supersedes it.
				anyChanges = true;

				if (iterStmts->second)
					iterMap->second = pair<NamedEvent, ZVal_Any>(theNamedEvent, sAsVal(theDaton));
				else
					fMap.erase(iterMap);
				}
			}
		}
	return anyChanges;
	}

} // namespace ZDataspace
} // namespace ZooLib
