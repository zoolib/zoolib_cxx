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
#include "zoolib/ZStrimU_StreamUTF8Buffered.h"
#include "zoolib/ZStrimmer_Streamer.h"
#include "zoolib/ZUtil_STL_map.h"
#include "zoolib/ZYad_Any.h"
#include "zoolib/ZYad_ZooLibStrim.h"

#include "zoolib/dataspace/ZDataspace_Source_Dataset.h"

#include "zoolib/zqe/ZQE_Search.h"
#include "zoolib/zqe/ZQE_Visitor_DoMakeWalker.h"

#include "zoolib/zra/ZRA_Expr_Rel_Concrete.h"
#include "zoolib/zra/ZRA_Transform_ConsolidateRenames.h"
#include "zoolib/zra/ZRA_Transform_PushDownRestricts.h"
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

		ZRef<ZStrimmerU> theStrimmerU =
			new ZStrimmerU_Streamer_T<ZStrimU_StreamUTF8Buffered>(1024, theStreamerR);

		ZRef<ZYadR> theYadR = ZYad_ZooLibStrim::sMakeYadR(theStrimmerU);

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
		ZStrimW_StreamUTF8(MakeStreamRWPos_Data_T(theData)));
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

	virtual ZRef<ZQE::Walker> Prime(const map<string8,size_t>& iOffsets,
		map<string8,size_t>& oOffsets,
		size_t& ioBaseOffset)
		{
		fSource->pPrime(this, iOffsets, oOffsets, ioBaseOffset);
		return this;
		}

	virtual bool ReadInc(
		ZVal_Any* ioResults,
		set<ZRef<ZCounted> >* oAnnotations)
		{ return fSource->pReadInc(this, ioResults, oAnnotations); }

	const ZRef<Source_Dataset> fSource;
	const vector<string8> fNames;
	size_t fBaseOffset;
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
	:	fRefcon(iRefcon)
	,	fPSearch(iPSearch)
		{}

	int64 const fRefcon;
	PSearch* const fPSearch;
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

	const ZRef<ZRA::Expr_Rel> fRel;
	DListHead<DLink_ClientSearch_InPSearch> fClientSearches;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Source_Dataset

void Source_Dataset::ForceUpdate()
	{
	ZAcqMtxR acq(fMtxR);
	if (this->pPull())
		{
		fStackChanged = true;
		this->pInvokeCallable_ResultsAvailable();
		}

//	fStackChanged = true;
//	this->pInvokeCallable_ResultsAvailable();
	}

Source_Dataset::Source_Dataset(ZRef<Dataset> iDataset)
:	fDataset(iDataset)
,	fEvent(Event::sZero())
,	fStackChanged(false)
	{}

Source_Dataset::~Source_Dataset()
	{}

bool Source_Dataset::Intersects(const RelHead& iRelHead)
	{ return true; }

void Source_Dataset::ModifyRegistrations(
	const AddedSearch* iAdded, size_t iAddedCount,
	const int64* iRemoved, size_t iRemovedCount)
	{
	ZAcqMtxR acq(fMtxR);
	if (iAddedCount || iRemovedCount)
		{
		fStackChanged = true;
		this->pInvokeCallable_ResultsAvailable();
		}

	for (/*no init*/; iAddedCount--; ++iAdded)
		{
		ZRef<ZRA::Expr_Rel> theRel = iAdded->GetRel();

		theRel = ZRA::Transform_PushDownRestricts().Do(theRel);
		theRel = ZRA::Transform_ConsolidateRenames().Do(theRel);

		pair<Map_Rel_PSearch::iterator,bool> iterPSearchPair =
			fMap_Rel_PSearch.insert(make_pair(theRel, PSearch(theRel)));

		if (!iterPSearchPair.second)
			{
			if (ZLOGF(s, eDebug))
				s << "Reusing exisiting PSearch";
			}

		const Map_Rel_PSearch::iterator& iterPSearch = iterPSearchPair.first;
		PSearch* thePSearch = &iterPSearch->second;

		const int64 theRefcon = iAdded->GetRefcon();

		std::map<int64, ClientSearch>::iterator iterClientSearch =
			fMap_Refcon_ClientSearch.insert(
			make_pair(theRefcon, ClientSearch(theRefcon, thePSearch))).first;

		thePSearch->fClientSearches.Insert(&iterClientSearch->second);
		}

	while (iRemovedCount--)
		{
		int64 theRefcon = *iRemoved++;

		std::map<int64, ClientSearch>::iterator iterClientSearch =
			fMap_Refcon_ClientSearch.find(theRefcon);

		ZAssertStop(kDebug, iterClientSearch != fMap_Refcon_ClientSearch.end());
		
		ClientSearch* theClientSearch = &iterClientSearch->second;
		
		PSearch* thePSearch = theClientSearch->fPSearch;
		thePSearch->fClientSearches.Erase(theClientSearch);
		if (thePSearch->fClientSearches.Empty())
			ZUtil_STL::sEraseMustContain(kDebug, fMap_Rel_PSearch, thePSearch->fRel);
		
		fMap_Refcon_ClientSearch.erase(iterClientSearch);
		}
	}

void Source_Dataset::CollectResults(vector<SearchResult>& oChanged)
	{
	ZAcqMtxR acq(fMtxR);

	oChanged.clear();

	bool anyChanges = fStackChanged;
	fStackChanged = false;

	// Pick up (and index) values from dataset
	if (this->pPull())
		anyChanges = true;
	
	if (anyChanges)
		{
		for (Map_Rel_PSearch::iterator iterPSearch = fMap_Rel_PSearch.begin();
			iterPSearch != fMap_Rel_PSearch.end(); ++iterPSearch)
			{
			PSearch* thePSearch = &iterPSearch->second;

			fWalkerCount = 0;
			fReadCount = 0;
			fStepCount = 0;

			ZRef<ZQE::Walker> theWalker = Visitor_DoMakeWalker(this).Do(thePSearch->fRel);

			if (ZLOGPF(s, eDebug + 1))
				{
				s << "\n";
				ZRA::Util_Strim_Rel::sToStrim(thePSearch->fRel, s);
				}

			ZRef<ZQE::Result> theResult = sSearch(theWalker);

			if (ZLOGPF(s, eDebug + 1))
				{
				s	<< "Walkers: " << fWalkerCount
					<< ", reads: " << fReadCount
					<< ", steps: " << fStepCount;
				}

			for (DListIterator<ClientSearch, DLink_ClientSearch_InPSearch>
				iterCS = thePSearch->fClientSearches; iterCS; iterCS.Advance())
				{
				oChanged.push_back(SearchResult(iterCS.Current()->fRefcon, theResult, fEvent));
				}
			}
		}

	if (!ZMACRO_IPhone_Device)
		ZThread::sSleep(1);
	}

ZRef<ZDataset::Dataset> Source_Dataset::GetDataset()
	{ return fDataset; }

void Source_Dataset::Insert(const Daton& iDaton)
	{
	ZAcqMtxR acq(fMtxR);
	this->pModify(iDaton, sAsVal(iDaton), true);
	this->pConditionalPushDown();
	}

void Source_Dataset::Erase(const Daton& iDaton)
	{
	ZAcqMtxR acq(fMtxR);
	this->pModify(iDaton, sAsVal(iDaton), false);
	this->pConditionalPushDown();
	}

size_t Source_Dataset::OpenTransaction()
	{
	ZAcqMtxR acq(fMtxR);
	fStackChanged = true;
	fStack.push_back(fMap_Pending);
	return fStack.size();
	}

void Source_Dataset::ClearTransaction(size_t iIndex)
	{
	ZAcqMtxR acq(fMtxR);
	fStackChanged = true;
	ZAssert(iIndex == fStack.size());
	fMap_Pending = fStack.back();

	this->pInvokeCallable_ResultsAvailable();
	}

void Source_Dataset::CloseTransaction(size_t iIndex)
	{
	ZAcqMtxR acq(fMtxR);
	fStackChanged = true;
	ZAssert(iIndex == fStack.size());
	fStack.pop_back();
	this->pConditionalPushDown();
	this->pInvokeCallable_ResultsAvailable();
	}

void Source_Dataset::pConditionalPushDown()
	{
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
	{
	++fWalkerCount;
	return new Walker(this, vector<string8>(iRelHead.begin(), iRelHead.end()));
	}

void Source_Dataset::pRewind(ZRef<Walker> iWalker)
	{
	iWalker->fCurrent_Main = fMap.begin();
	iWalker->fCurrent_Pending = fMap_Pending.begin();
	}

void Source_Dataset::pPrime(ZRef<Walker> iWalker,
	const map<string8,size_t>& iOffsets,
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
	ZVal_Any* ioResults,
	set<ZRef<ZCounted> >* oAnnotations)
	{
	++fReadCount;

	const vector<string8>& theNames = iWalker->fNames;
	const string8* theNamesPtr = &theNames[0];
	const size_t theCount = theNames.size();

	while (iWalker->fCurrent_Main != fMap.end())
		{
		++fStepCount;
		// Ignore anything that's in pending (for now).
		if (fMap_Pending.end() == fMap_Pending.find(iWalker->fCurrent_Main->first))
			{
			if (const ZMap_Any* theMap = iWalker->fCurrent_Main->second.second.PGet<ZMap_Any>())
				{
				bool gotAll = true;
				for (size_t x = 0; x < theCount; ++x)
					{
					if (const ZVal_Any* theVal = theMap->PGet(theNamesPtr[x]))
						ioResults[iWalker->fBaseOffset + x] = *theVal;
					else
						gotAll = false;
					}

				if (gotAll)
					{
					if (ZLOGF(s, eDebug + 2))
						ZYad_ZooLibStrim::sToStrim(sMakeYadR(*theMap), s);

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
		++fStepCount;
		if (iWalker->fCurrent_Pending->second.second)
			{
			if (const ZMap_Any* theMap = iWalker->fCurrent_Pending->second.first.PGet<ZMap_Any>())
				{
				bool gotAll = true;
				for (size_t x = 0; x < theCount; ++x)
					{
					if (const ZVal_Any* theVal = theMap->PGet(theNamesPtr[x]))
						ioResults[iWalker->fBaseOffset + x] = *theVal;
					else
						gotAll = false;
					}

				if (gotAll)
					{
					if (ZLOGF(s, eDebug + 2))
						ZYad_ZooLibStrim::sToStrim(sMakeYadR(*theMap), s);

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
	if (s)
		s << "\ntheMNED.size()=" << theMNED.size();
	bool anyChanges = false;
	for (Map_NamedEvent_Delta_t::const_iterator
		iterMNED = theMNED.begin(), endMNED = theMNED.end();
		iterMNED != endMNED; ++iterMNED)
		{
		const NamedEvent& theNamedEvent = iterMNED->first;
		const map<Daton, bool>& theStatements = iterMNED->second->GetStatements();
		if (s)
			s << "\ntheStatements.size()=" << theStatements.size();
		for (map<Daton, bool>::const_iterator
			iterStmts = theStatements.begin(), endStmts = theStatements.end();
			iterStmts != endStmts; ++iterStmts)
			{
			const Daton& theDaton = iterStmts->first;

			if (s)
				{
				const ZData_Any& theData = theDaton.GetData();
				s << "\n" << (iterStmts->second ? "+" : "-") << ": ";
				s.Write(static_cast<const UTF8*>(theData.GetData()), theData.GetSize());
				}

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
