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
#include "zoolib/ZUtil_Strim_IntervalTreeClock.h"
#include "zoolib/ZYad_Any.h"
#include "zoolib/ZYad_ZooLibStrim.h"

#include "zoolib/dataspace/ZDataspace_Source_DatonSet.h"

#include "zoolib/zqe/ZQE_DoQuery.h"
#include "zoolib/zqe/ZQE_Transform_Search.h"
#include "zoolib/zqe/ZQE_Visitor_DoMakeWalker.h"
#include "zoolib/zqe/ZQE_Walker_Result.h"
#include "zoolib/zqe/ZQE_Walker_Rename.h"
#include "zoolib/zqe/ZQE_Walker_Restrict.h"

#include "zoolib/zra/ZRA_Expr_Rel_Concrete.h"
#include "zoolib/zra/ZRA_Transform_ConsolidateRenames.h"
#include "zoolib/zra/ZRA_Transform_DecomposeRestricts.h"
#include "zoolib/zra/ZRA_Transform_PushDownRestricts.h"
#include "zoolib/zra/ZRA_Util_Strim_Rel.h"
#include "zoolib/zra/ZRA_Util_Strim_RelHead.h"

namespace ZooLib {
namespace ZDataspace {

using ZDatonSet::DatonSet;
using ZDatonSet::Daton;
using ZDatonSet::Deltas;
using ZDatonSet::Map_NamedEvent_Delta_t;
using ZDatonSet::NamedEvent;

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
#pragma mark * Daton/Val conversion.

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
#pragma mark * Source_DatonSet::Visitor_DoMakeWalker

class Source_DatonSet::Visitor_DoMakeWalker
:	public virtual ZQE::Visitor_DoMakeWalker
,	public virtual ZRA::Visitor_Expr_Rel_Concrete
,	public virtual ZQE::Visitor_Expr_Rel_Search
	{
	typedef ZQE::Visitor_DoMakeWalker inherited;
public:
	Visitor_DoMakeWalker(ZRef<Source_DatonSet> iSource, PQuery* iPQuery)
	:	fSource(iSource)
	,	fPQuery(iPQuery)
		{}

	virtual void Visit_Expr_Rel_Concrete(const ZRef<ZRA::Expr_Rel_Concrete>& iExpr)
		{
		this->pSetResult(fSource->pMakeWalker_Concrete(iExpr->GetConcreteRelHead()));
		}

	virtual void Visit_Expr_Rel_Search(const ZRef<ZQE::Expr_Rel_Search>& iExpr)
		{
		this->pSetResult(fSource->pMakeWalker_Search(fPQuery, iExpr));
		}

	ZRef<Source_DatonSet> fSource;
	PQuery* fPQuery;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Source_DatonSet::Walker_Concrete

class Source_DatonSet::Walker_Concrete : public ZQE::Walker
	{
public:
	Walker_Concrete(ZRef<Source_DatonSet> iSource, const vector<string8>& iNames)
	:	fSource(iSource)
	,	fNames(iNames)
		{}

	virtual ~Walker_Concrete()
		{}

// From ZQE::Walker
	virtual void Rewind()
		{ fSource->pRewind_Concrete(this); }

	virtual ZRef<ZQE::Walker> Prime(const map<string8,size_t>& iOffsets,
		map<string8,size_t>& oOffsets,
		size_t& ioBaseOffset)
		{
		fSource->pPrime_Concrete(this, iOffsets, oOffsets, ioBaseOffset);
		return this;
		}

	virtual bool ReadInc(
		ZVal_Any* ioResults,
		set<ZRef<ZCounted> >* oAnnotations)
		{ return fSource->pReadInc_Concrete(this, ioResults, oAnnotations); }

	const ZRef<Source_DatonSet> fSource;
	const vector<string8> fNames;
	size_t fBaseOffset;
	Map_Main::const_iterator fCurrent_Main;
	Map_Pending::const_iterator fCurrent_Pending;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Source_DatonSet::ClientQuery

class Source_DatonSet::DLink_ClientQuery_InPQuery
:	public DListLink<ClientQuery, DLink_ClientQuery_InPQuery, kDebug>
	{};

class Source_DatonSet::DLink_ClientQuery_NeedsWork
:	public DListLink<ClientQuery, DLink_ClientQuery_NeedsWork, kDebug>
	{};

class Source_DatonSet::ClientQuery
:	public DLink_ClientQuery_InPQuery
,	public DLink_ClientQuery_NeedsWork
	{
public:
	ClientQuery(int64 iRefcon, PQuery* iPQuery)
	:	fRefcon(iRefcon)
	,	fPQuery(iPQuery)
		{}

	int64 const fRefcon;
	PQuery* const fPQuery;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Source_DatonSet::PQuery

class Source_DatonSet::DLink_PQuery_NeedsWork
:	public DListLink<PQuery, DLink_PQuery_NeedsWork, kDebug>
	{};

class Source_DatonSet::PQuery
:	public DLink_PQuery_NeedsWork
	{
public:
	PQuery(ZRef<ZRA::Expr_Rel> iRel)
	:	fRel(iRel)
		{}

	const ZRef<ZRA::Expr_Rel> fRel;
	DListHead<DLink_ClientQuery_InPQuery> fClientQueries;
	set<PSearch*> fDependingPSearches;
	ZRef<ZQE::Result> fResult;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Source_DatonSet::PSearch

class Source_DatonSet::DLink_PSearch_NeedsWork
:	public DListLink<PSearch, DLink_PSearch_NeedsWork, kDebug>
	{};

class Source_DatonSet::PSearch
:	public DLink_PSearch_NeedsWork
	{
public:
	PSearch() {}

	ZRA::Rename fRename;
	RelHead fRelHead;
	ZRef<ZExpr_Bool> fExpr_Bool;
	set<PQuery*> fDependentPQueries;
//##	ZRef<ZQE::Result> fResult;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Source_DatonSet

void Source_DatonSet::ForceUpdate()
	{ this->pInvokeCallable_ResultsAvailable(); }

Source_DatonSet::Source_DatonSet(ZRef<DatonSet> iDatonSet)
:	fDatonSet(iDatonSet)
,	fEvent(Event::sZero())
	{}

Source_DatonSet::~Source_DatonSet()
	{}

bool Source_DatonSet::Intersects(const RelHead& iRelHead)
	{ return true; }

void Source_DatonSet::ModifyRegistrations(
	const AddedQuery* iAdded, size_t iAddedCount,
	const int64* iRemoved, size_t iRemovedCount)
	{
	ZGuardRMtxR guard(fMtxR);

	for (/*no init*/; iAddedCount--; ++iAdded)
		{
		if (ZLOGPF(s, eDebug))
			{
			s << "\nDatonSet Raw:\n";
			ZRA::Util_Strim_Rel::sToStrim(iAdded->GetRel(), s);
			}

		ZRef<ZRA::Expr_Rel> theRel = iAdded->GetRel();
		theRel = ZQE::sTransform_Search(theRel);

		if (ZLOGPF(s, eDebug))
			{
			s << "\nDatonSet Cooked:\n";
			ZRA::Util_Strim_Rel::sToStrim(theRel, s);
			}

		const pair<Map_Rel_PQuery::iterator,bool> iterPQueryPair =
			fMap_Rel_PQuery.insert(make_pair(theRel, PQuery(theRel)));

		const Map_Rel_PQuery::iterator& iterPQuery = iterPQueryPair.first;
		PQuery* thePQuery = &iterPQuery->second;

		const int64 theRefcon = iAdded->GetRefcon();

		const pair<map<int64,ClientQuery>::iterator,bool> iterClientQueryPair =
			fMap_Refcon_ClientQuery.insert(
			make_pair(theRefcon, ClientQuery(theRefcon, thePQuery)));
		ZAssert(iterClientQueryPair.second);

		ClientQuery* theClientQuery = &iterClientQueryPair.first->second;
		thePQuery->fClientQueries.Insert(theClientQuery);

		if (iterPQueryPair.second)
			{
			// It's a new PQuery, so we'll need to work on it.
			fPQuery_NeedsWork.Insert(thePQuery);
			}
		else
			{
			// It's an existing PQuery, so the ClientQuery will need to be worked on.
			fClientQuery_NeedsWork.Insert(theClientQuery);
			}
		}

	while (iRemovedCount--)
		{
		int64 theRefcon = *iRemoved++;

		map<int64, ClientQuery>::iterator iterClientQuery =
			fMap_Refcon_ClientQuery.find(theRefcon);

		ZAssertStop(kDebug, iterClientQuery != fMap_Refcon_ClientQuery.end());
		
		ClientQuery* theClientQuery = &iterClientQuery->second;
		
		PQuery* thePQuery = theClientQuery->fPQuery;
		thePQuery->fClientQueries.Erase(theClientQuery);
		if (thePQuery->fClientQueries.Empty())
			{
			this->pDetachPQuery(thePQuery);
			fPQuery_NeedsWork.EraseIfContains(thePQuery);
			ZUtil_STL::sEraseMustContain(kDebug, fMap_Rel_PQuery, thePQuery->fRel);
			}

		fClientQuery_NeedsWork.EraseIfContains(theClientQuery);
		fMap_Refcon_ClientQuery.erase(iterClientQuery);
		}

	if (!fClientQuery_NeedsWork.Empty() || !fPQuery_NeedsWork.Empty())
		{
		guard.Release();
		this->pInvokeCallable_ResultsAvailable();
		}
	}

void Source_DatonSet::CollectResults(vector<QueryResult>& oChanged)
	{
	this->pCollectResultsCalled();
	
	ZAcqMtxR acq(fMtxR);

	oChanged.clear();

	// Pick up (and index) values from dataset
	this->pPull();
	
	for (DListEraser<PQuery,DLink_PQuery_NeedsWork> eraser = fPQuery_NeedsWork;
		eraser; eraser.Advance())
		{
		PQuery* thePQuery = eraser.Current();

		if (ZLOGPF(s, eDebug))
			{
			s << "\n";
			ZRA::Util_Strim_Rel::sToStrim(thePQuery->fRel, s);
			}

		ZRef<ZQE::Walker> theWalker = Visitor_DoMakeWalker(this, thePQuery).Do(thePQuery->fRel);


		ZRef<ZQE::Result> theResult = ZQE::sDoQuery(theWalker);
		thePQuery->fResult = theResult;
		for (DListIterator<ClientQuery, DLink_ClientQuery_InPQuery>
			iterCS = thePQuery->fClientQueries; iterCS; iterCS.Advance())
			{ fClientQuery_NeedsWork.InsertIfNotContains(iterCS.Current()); }
		}

	for (DListEraser<ClientQuery,DLink_ClientQuery_NeedsWork> eraser = fClientQuery_NeedsWork;
		eraser; eraser.Advance())
		{
		ClientQuery* theClientQuery = eraser.Current();
		PQuery* thePQuery = theClientQuery->fPQuery;
		oChanged.push_back(QueryResult(theClientQuery->fRefcon, thePQuery->fResult, fEvent));
		}

	// Remove any unused PSearches
	for (DListEraser<PSearch,DLink_PSearch_NeedsWork> eraser = fPSearch_NeedsWork;
		eraser; eraser.Advance())
		{
		PSearch* thePSearch = eraser.Current();
		if (thePSearch->fDependentPQueries.empty())
			{
			// Delete thePSearch
			const PSearchKey theKey(thePSearch->fRename, thePSearch->fExpr_Bool);
			ZUtil_STL::sEraseMustContain(1, fMap_Rel_PSearch, theKey);
			}
		}

//##	if (!ZMACRO_IPhone_Device)
//##		ZThread::sSleep(1);
	}

ZRef<ZDatonSet::DatonSet> Source_DatonSet::GetDatonSet()
	{ return fDatonSet; }

void Source_DatonSet::Insert(const Daton& iDaton)
	{
	ZGuardRMtxR guard(fMtxR);
	this->pModify(iDaton, sAsVal(iDaton), true);
	this->pConditionalPushDown();
	guard.Release();

	this->pInvokeCallable_ResultsAvailable();	
	}

void Source_DatonSet::Erase(const Daton& iDaton)
	{
	ZGuardRMtxR guard(fMtxR);
	this->pModify(iDaton, sAsVal(iDaton), false);
	this->pConditionalPushDown();
	guard.Release();

	this->pInvokeCallable_ResultsAvailable();	
	}

void Source_DatonSet::Replace(const ZDatonSet::Daton& iOld, const ZDatonSet::Daton& iNew)
	{
	ZGuardRMtxR guard(fMtxR);
	this->pModify(iOld, sAsVal(iOld), false);
	this->pModify(iNew, sAsVal(iNew), true);
	this->pConditionalPushDown();
	guard.Release();

	this->pInvokeCallable_ResultsAvailable();	
	}

size_t Source_DatonSet::OpenTransaction()
	{
	ZAcqMtxR acq(fMtxR);
	fStack_Map_Pending.push_back(fMap_Pending);
	return fStack_Map_Pending.size();
	}

void Source_DatonSet::ClearTransaction(size_t iIndex)
	{
	ZAcqMtxR acq(fMtxR);
	ZAssert(iIndex == fStack_Map_Pending.size());

	this->pChangedAll();

	fMap_Pending = fStack_Map_Pending.back();
	}

void Source_DatonSet::CloseTransaction(size_t iIndex)
	{
	ZGuardRMtxR guard(fMtxR);

	ZAssert(iIndex == fStack_Map_Pending.size());

	this->pChangedAll();
	fStack_Map_Pending.pop_back();
	this->pConditionalPushDown();
	guard.Release();

	this->pInvokeCallable_ResultsAvailable();	
	}

void Source_DatonSet::pDetachPQuery(PQuery* iPQuery)
	{
	// Detach from any depended-upon PSearch
	for (set<PSearch*>::iterator iterPSearch = iPQuery->fDependingPSearches.begin();
		iterPSearch != iPQuery->fDependingPSearches.end(); ++iterPSearch)
		{
		PSearch* thePSearch = *iterPSearch;
		ZUtil_STL::sEraseMustContain(1, thePSearch->fDependentPQueries, iPQuery);
		if (thePSearch->fDependentPQueries.empty())
			fPSearch_NeedsWork.InsertIfNotContains(thePSearch);
		}
	iPQuery->fDependingPSearches.clear();
	}

void Source_DatonSet::pPull()
	{
	// Get our map in sync with fDatonSet
	ZLOGF(s, eDebug);
	ZRef<Deltas> theDeltas;
	fEvent = fDatonSet->GetDeltas(theDeltas, fEvent);
	const Map_NamedEvent_Delta_t& theMNED = theDeltas->GetMap();
	if (s)
		s << "\ntheMNED.size()=" << theMNED.size();

	for (Map_NamedEvent_Delta_t::const_iterator
		iterMNED = theMNED.begin(), endMNED = theMNED.end();
		iterMNED != endMNED; ++iterMNED)
		{
		const NamedEvent& theNamedEvent = iterMNED->first;
		const map<Daton, bool>& theStatements = iterMNED->second->GetStatements();
		if (s)
			{
			s << "\ntheStatements.size()=" << theStatements.size();
			s << "\nclk:" << theNamedEvent.GetEvent();
			}

		for (map<Daton, bool>::const_iterator
			iterStmts = theStatements.begin(), endStmts = theStatements.end();
			iterStmts != endStmts; ++iterStmts)
			{
			const Daton& theDaton = iterStmts->first;

			if (s)
				s << "\n" << (iterStmts->second ? "+" : "-") << ": ";

			map<Daton, pair<NamedEvent, ZVal_Any> >::iterator iterMap = fMap.lower_bound(theDaton);
			if (iterMap == fMap.end() || iterMap->first != theDaton)
				{
				s << " NFo ";
				if (iterStmts->second)
					{
					const ZVal_Any theVal = sAsVal(theDaton);
					this->pChanged(theVal);

					fMap.insert(iterMap,
						make_pair(theDaton,
						pair<NamedEvent, ZVal_Any>(theNamedEvent, theVal)));
					}
				}
			else
				{
				const bool alb = iterMap->second.first < theNamedEvent;
				const bool bla = theNamedEvent < iterMap->second.first;

				if (s)
					s << iterMap->second.first.GetEvent() << (alb?"alb" :"") << (bla?"blb":"");

				if (alb)
					{
					// theNamedEvent is more recent than what we've got and thus supersedes it.
					if (s)
						s << " MRc ";

					const ZVal_Any theVal = sAsVal(theDaton);
					this->pChanged(theVal);

					if (iterStmts->second)
						iterMap->second = pair<NamedEvent, ZVal_Any>(theNamedEvent, theVal);
					else
						fMap.erase(iterMap);
					}
				else
					{
					if (s)
						s << " Old ";
					}
				}
				

			if (s)
				{
				const ZData_Any& theData = theDaton.GetData();
				s.Write(static_cast<const UTF8*>(theData.GetData()), theData.GetSize());
				}
			}
		}
	}

void Source_DatonSet::pConditionalPushDown()
	{
	if (fStack_Map_Pending.empty())
		{
		for (Map_Pending::iterator i = fMap_Pending.begin(), end = fMap_Pending.end();
			i != end; ++i)
			{
			if (i->second.second)
				fDatonSet->Insert(i->first);
			else
				fDatonSet->Erase(i->first);			
			}
		fMap_Pending.clear();
		}
	}

void Source_DatonSet::pModify(const ZDatonSet::Daton& iDaton, const ZVal_Any& iVal, bool iSense)
	{
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
	this->pChanged(iVal);
	}

void Source_DatonSet::pChanged(const ZVal_Any& iVal)
	{
	const ZMap_Any theMap = iVal.GetMap();
	RelHead theRH;
	for (ZMap_Any::Index_t i = theMap.Begin(); i != theMap.End(); ++i)
		theRH |= theMap.NameOf(i);

	for (Map_Rel_PSearch::iterator iterPSearch = fMap_Rel_PSearch.begin();
		iterPSearch != fMap_Rel_PSearch.end(); ++iterPSearch)
		{
		PSearch* thePSearch = &iterPSearch->second;
		if (!(thePSearch->fRelHead & theRH).empty())
			{
			for (set<PQuery*>::iterator iterPQuery = thePSearch->fDependentPQueries.begin();
				iterPQuery != thePSearch->fDependentPQueries.end(); ++iterPQuery)
				{
				PQuery* thePQuery = *iterPQuery;
				fPQuery_NeedsWork.InsertIfNotContains(thePQuery);
				for (set<PSearch*>::iterator iterDependingPSearch =
					thePQuery->fDependingPSearches.begin();
					iterDependingPSearch != thePQuery->fDependingPSearches.end();
					++iterDependingPSearch)
					{
					PSearch* theDependingPSearch = *iterDependingPSearch;
					if (theDependingPSearch != thePSearch)
						{
						ZUtil_STL::sEraseMustContain(
							1, theDependingPSearch->fDependentPQueries, thePQuery);

						if (theDependingPSearch->fDependentPQueries.empty())
							fPSearch_NeedsWork.InsertIfNotContains(theDependingPSearch);
						}
					}
				thePQuery->fDependingPSearches.clear();
				}
			thePSearch->fDependentPQueries.clear();
			fPSearch_NeedsWork.InsertIfNotContains(thePSearch);
//##			thePSearch->fResult.Clear();
			}
		}
	}

void Source_DatonSet::pChangedAll()
	{
	for (Map_Pending::iterator i = fMap_Pending.begin(), end = fMap_Pending.end();
		i != end; ++i)
		{ this->pChanged(i->second.first); }
	}

ZRef<ZQE::Walker> Source_DatonSet::pMakeWalker_Concrete(const RelHead& iRelHead)
	{
	++fWalkerCount;
	return new Walker_Concrete(this, vector<string8>(iRelHead.begin(), iRelHead.end()));
	}

ZRef<ZQE::Walker> Source_DatonSet::pMakeWalker_Search(
	PQuery* iPQuery, const ZRef<ZQE::Expr_Rel_Search>& iRel)
	{
	// Walker_Search knows the PQuery it's operating on behalf of, and so as it
	// gets PSearches from the source, they're registered against that PQuery.

	const ZRA::Rename& theRename = iRel->GetRename();
	const ZRef<ZExpr_Bool>& theExpr_Bool = iRel->GetExpr_Bool();

	const PSearchKey theKey(theRename, theExpr_Bool);

	PSearch* thePSearch;
	Map_Rel_PSearch::iterator iterPSearch = fMap_Rel_PSearch.find(theKey);
	if (iterPSearch != fMap_Rel_PSearch.end())
		{
		thePSearch = &iterPSearch->second;
		}
	else
		{
		pair<Map_Rel_PSearch::iterator,bool> inPSearch =
			fMap_Rel_PSearch.insert(make_pair(theKey, PSearch()));

		thePSearch = &inPSearch.first->second;
		thePSearch->fRename = theRename;
		thePSearch->fExpr_Bool = theExpr_Bool;
		for (ZRA::Rename::const_iterator i = theRename.begin(); i != theRename.end(); ++i)
			thePSearch->fRelHead |= i->first;
		}

	ZUtil_STL::sInsertMustNotContain(1, iPQuery->fDependingPSearches, thePSearch);
	ZUtil_STL::sInsertMustNotContain(1, thePSearch->fDependentPQueries, iPQuery);

	ZRef<ZQE::Walker> theWalker = this->pMakeWalker_Concrete(thePSearch->fRelHead);

	if (theExpr_Bool != sTrue())
		theWalker = new ZQE::Walker_Restrict(theWalker, theExpr_Bool);

	for (ZRA::Rename::const_iterator iterRename = theRename.begin();
		iterRename != theRename.end(); ++iterRename)
		{
		if (iterRename->first != iterRename->second)
			theWalker = new ZQE::Walker_Rename(theWalker, iterRename->first, iterRename->second);
		}

	return theWalker;
	}

void Source_DatonSet::pRewind_Concrete(ZRef<Walker_Concrete> iWalker)
	{
	iWalker->fCurrent_Main = fMap.begin();
	iWalker->fCurrent_Pending = fMap_Pending.begin();
	}

void Source_DatonSet::pPrime_Concrete(ZRef<Walker_Concrete> iWalker,
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

bool Source_DatonSet::pReadInc_Concrete(ZRef<Walker_Concrete> iWalker,
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

} // namespace ZDataspace
} // namespace ZooLib
