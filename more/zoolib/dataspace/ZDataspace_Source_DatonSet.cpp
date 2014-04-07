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
#include "zoolib/ZMACRO_foreach.h"
#include "zoolib/ZStream_Data_T.h"
#include "zoolib/ZStrim_Stream.h"
#include "zoolib/ZStrimU_StreamUTF8Buffered.h"
#include "zoolib/ZStrimmer_Streamer.h"
#include "zoolib/ZStringf.h"
#include "zoolib/ZUtil_STL_map.h"
#include "zoolib/ZUtil_Strim_IntervalTreeClock.h"
#include "zoolib/ZVisitor_Expr_Bool_ValPred_Do_GetNames.h"
#include "zoolib/ZYad_Any.h"
#include "zoolib/ZYad_ZooLibStrim.h"

#include "zoolib/dataspace/ZDataspace_Source_DatonSet.h"

#include "zoolib/QueryEngine/DoQuery.h"
#include "zoolib/QueryEngine/Transform_Search.h"
#include "zoolib/QueryEngine/Visitor_DoMakeWalker.h"
#include "zoolib/QueryEngine/Walker_Project.h"
#include "zoolib/QueryEngine/Walker_Result.h"
#include "zoolib/QueryEngine/Walker_Rename.h"
#include "zoolib/QueryEngine/Walker_Restrict.h"

#include "zoolib/RelationalAlgebra/Expr_Rel_Concrete.h"
#include "zoolib/RelationalAlgebra/Util_Strim_Rel.h"
#include "zoolib/RelationalAlgebra/Util_Strim_RelHead.h"

namespace ZooLib {
namespace ZDataspace {

using ZDatonSet::DatonSet;
using ZDatonSet::Daton;
using ZDatonSet::Deltas;
using ZDatonSet::Map_NamedEvent_Delta_t;
using ZDatonSet::NamedEvent;

using namespace ZUtil_STL;

using std::make_pair;
using std::map;
using std::pair;
using std::set;
using std::vector;

namespace QE = QueryEngine;
namespace RA = RelationalAlgebra;

/*
Minimally, keep an index of property names in contents -- that way we can efficiently
identify those entities that satisfy a RelHead.
*/

static const ZStrimW& operator<<(const ZStrimW& w, const Daton& iDaton)
	{
	const ZData_Any& theData = iDaton.GetData();
	w.Write(static_cast<const UTF8*>(theData.GetPtr()), theData.GetSize());
	return w;
	}

// =================================================================================================
// MARK: - spAsVal (anonymous)

namespace { // anonymous

ZVal_Any spAsVal(const ZData_Any& iData)
	{
	try
		{
		ZRef<ZStreamerR> theStreamerR =
			new ZStreamerRPos_T<ZStreamRPos_Data_T<ZData_Any> >(iData);

		ZRef<ZStrimmerU> theStrimmerU =
			new ZStrimmerU_Streamer_T<ZStrimU_StreamUTF8Buffered>(1024, theStreamerR);

		ZRef<ZYadR> theYadR = ZYad_ZooLibStrim::sYadR(theStrimmerU);

		return sFromYadR(ZVal_Any(), theYadR);
		}
	catch (...)
		{
		return ZVal_Any();
		}
	}

} // anonymous namespace

// =================================================================================================
// MARK: - Daton/Val conversion.

ZVal_Any sAsVal(const Daton& iDaton)
	{ return spAsVal(iDaton.GetData()); }

Daton sAsDaton(const ZVal_Any& iVal)
	{
	ZData_Any theData;
	ZYad_ZooLibStrim::sToStrim(sYadR(iVal),
		ZStrimW_StreamUTF8(sStreamRWPos_Data_T(theData)));
	return theData;
	}

// =================================================================================================
// MARK: - Source_DatonSet::Visitor_DoMakeWalker

class Source_DatonSet::Visitor_DoMakeWalker
:	public virtual QE::Visitor_DoMakeWalker
,	public virtual RA::Visitor_Expr_Rel_Concrete
,	public virtual QE::Visitor_Expr_Rel_Search
	{
	typedef QE::Visitor_DoMakeWalker inherited;
public:
	Visitor_DoMakeWalker(ZRef<Source_DatonSet> iSource, PQuery* iPQuery)
	:	fSource(iSource)
	,	fPQuery(iPQuery)
		{}

	virtual void Visit_Expr_Rel_Concrete(const ZRef<RA::Expr_Rel_Concrete>& iExpr)

	virtual void Visit_Expr_Rel_Search(const ZRef<QE::Expr_Rel_Search>& iExpr)
		{ this->pSetResult(fSource->pMakeWalker_Search(fPQuery, iExpr)); }

	ZRef<Source_DatonSet> const fSource;
	PQuery* const fPQuery;
	};

// =================================================================================================
// MARK: - Source_DatonSet::Walker_Concrete

class Source_DatonSet::Walker_Concrete : public QE::Walker
	{
public:
	Walker_Concrete(ZRef<Source_DatonSet> iSource, const vector<string8>& iNames)
	:	fSource(iSource)
	,	fNames(iNames)
		{}

	virtual ~Walker_Concrete()
		{}

// From QE::Walker
	virtual void Rewind()
		{ fSource->pRewind_Concrete(this); }

	virtual ZRef<QE::Walker> Prime(const map<string8,size_t>& iOffsets,
		map<string8,size_t>& oOffsets,
		size_t& ioBaseOffset)
		{
		fSource->pPrime_Concrete(this, iOffsets, oOffsets, ioBaseOffset);
		return this;
		}

	virtual bool QReadInc(ZVal_Any* ioResults)
		{ return fSource->pReadInc_Concrete(this, ioResults); }

	const ZRef<Source_DatonSet> fSource;
	const vector<string8> fNames;
	size_t fBaseOffset;
	Map_Main::const_iterator fCurrent_Main;
	Map_Pending::const_iterator fCurrent_Pending;
	std::set<std::vector<ZVal_Any> > fPriors;
	};

// =================================================================================================
// MARK: - Source_DatonSet::ClientQuery

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
// MARK: - Source_DatonSet::PQuery

class Source_DatonSet::DLink_PQuery_NeedsWork
:	public DListLink<PQuery, DLink_PQuery_NeedsWork, kDebug>
	{};

class Source_DatonSet::PQuery
:	public DLink_PQuery_NeedsWork
	{
public:
	PQuery(ZRef<RA::Expr_Rel> iRel)
	:	fRel(iRel)
		{}

	const ZRef<RA::Expr_Rel> fRel;
	DListHead<DLink_ClientQuery_InPQuery> fClientQuery_InPQuery;
	set<PSearch*> fPSearch_Used;
	ZRef<QE::Result> fResult;
	};

// =================================================================================================
// MARK: - Source_DatonSet::PSearch

class Source_DatonSet::DLink_PSearch_NeedsWork
:	public DListLink<PSearch, DLink_PSearch_NeedsWork, kDebug>
	{};

class Source_DatonSet::PSearch
:	public DLink_PSearch_NeedsWork
	{
public:
	PSearch() {}

	RelHead fRelHead;
	set<PQuery*> fPQuery_Using;
	ZRef<QE::Result> fResult;
	};

// =================================================================================================
// MARK: - Source_DatonSet

void Source_DatonSet::ForceUpdate()
	{ Source::pTriggerResultsAvailable(); }

Source_DatonSet::Source_DatonSet(ZRef<DatonSet> iDatonSet)
:	fDatonSet(iDatonSet)
,	fEvent(Event::sZero())
	{}

Source_DatonSet::~Source_DatonSet()
	{
	for (DListEraser<PSearch,DLink_PSearch_NeedsWork> eraser = fPSearch_NeedsWork;
		eraser; eraser.Advance())
		{}
	for (DListEraser<ClientQuery,DLink_ClientQuery_NeedsWork> eraser = fClientQuery_NeedsWork;
		eraser; eraser.Advance())
		{}
	}

bool Source_DatonSet::Intersects(const RelHead& iRelHead)
	{ return true; }

void Source_DatonSet::ModifyRegistrations(
	const AddedQuery* iAdded, size_t iAddedCount,
	const int64* iRemoved, size_t iRemovedCount)
	{
	ZGuardMtxR guard(fMtxR);

	for (/*no init*/; iAddedCount--; ++iAdded)
		{
		if (ZLOGPF(s, eDebug + 1))
			s << "\nDatonSet Raw:\n" << iAdded->GetRel();

		ZRef<RA::Expr_Rel> theRel = iAdded->GetRel();
// FIXME: With the transform, restrictions in an embed don't find names in the owning rel
//###		theRel = QE::sTransform_Search(theRel);

		if (ZLOGPF(s, eDebug + 1))
			s << "\nDatonSet Cooked:\n" << theRel;

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
		sInsertBackMust(thePQuery->fClientQuery_InPQuery, theClientQuery);

		if (iterPQueryPair.second)
			{
			// It's a new PQuery, so we'll need to work on it.
			sInsertBackMust(fPQuery_NeedsWork, thePQuery);
			}
		else
			{
			// It's an existing PQuery, so the ClientQuery will need to be worked on.
			sInsertBackMust(fClientQuery_NeedsWork, theClientQuery);
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
		sEraseMust(thePQuery->fClientQuery_InPQuery, theClientQuery);
		if (sIsEmpty(thePQuery->fClientQuery_InPQuery))
			{
			// Detach from any depended-upon PSearch
			for (set<PSearch*>::iterator iterPSearch = thePQuery->fPSearch_Used.begin();
				iterPSearch != thePQuery->fPSearch_Used.end(); ++iterPSearch)
				{
				PSearch* thePSearch = *iterPSearch;
				sEraseMust(kDebug, thePSearch->fPQuery_Using, thePQuery);
				if (thePSearch->fPQuery_Using.empty())
					sQInsertBack(fPSearch_NeedsWork, thePSearch);
				}
			thePQuery->fPSearch_Used.clear();

			sQErase(fPQuery_NeedsWork, thePQuery);
			sEraseMust(kDebug, fMap_Rel_PQuery, thePQuery->fRel);
			}

		sQErase(fClientQuery_NeedsWork, theClientQuery);
		fMap_Refcon_ClientQuery.erase(iterClientQuery);
		}

	if (sNotEmpty(fClientQuery_NeedsWork) || sNotEmpty(fPQuery_NeedsWork))
		{
		guard.Release();
		Source::pTriggerResultsAvailable();
		}
	}

void Source_DatonSet::CollectResults(vector<QueryResult>& oChanged)
	{
	this->pCollectResultsCalled();

	ZAcqMtxR acq(fMtxR);

	oChanged.clear();

	this->pPull();

	ZLOGPF(s, eDebug + 1);

	for (DListEraser<PQuery,DLink_PQuery_NeedsWork> eraser = fPQuery_NeedsWork;
		eraser; eraser.Advance())
		{
		PQuery* thePQuery = eraser.Current();

		fWalkerCount = 0;
		fReadCount = 0;
		fStepCount = 0;

		const ZTime start = ZTime::sNow();

		ZRef<QE::Walker> theWalker = Visitor_DoMakeWalker(this, thePQuery).Do(thePQuery->fRel);
		const ZTime afterMakeWalker = ZTime::sNow();

		if (s)
			{
			s << "\n" << thePQuery->fRel;
			s.Emit();
			}

		thePQuery->fResult = QE::sDoQuery(theWalker);
		const ZTime afterDoQuery = ZTime::sNow();

		if (s && 0)
			{
			s
				<< "\nWalkerCount: " << fWalkerCount
				<< "\nReadCount: " << fReadCount
				<< "\nStepCount: " << fStepCount
				<< "\nMakeWalker: " << sStringf("%gms", 1000*(afterMakeWalker-start))
				<< "\nDoQuery: " << sStringf("%gms", 1000*(afterDoQuery-afterMakeWalker));
			}

		for (DListIterator<ClientQuery, DLink_ClientQuery_InPQuery>
			iter = thePQuery->fClientQuery_InPQuery; iter; iter.Advance())
			{ sQInsertBack(fClientQuery_NeedsWork, iter.Current()); }
		}

	ZRef<Event> theEvent = fDatonSet->GetEvent();
	if (fDatonSet_Temp)
		theEvent = theEvent->Joined(fDatonSet_Temp->GetEvent());

	for (DListEraser<ClientQuery,DLink_ClientQuery_NeedsWork> eraser = fClientQuery_NeedsWork;
		eraser; eraser.Advance())
		{
		ClientQuery* theClientQuery = eraser.Current();
		PQuery* thePQuery = theClientQuery->fPQuery;
		oChanged.push_back(QueryResult(theClientQuery->fRefcon, thePQuery->fResult, theEvent));
		}

	// Remove any unused PSearches
	for (DListEraser<PSearch,DLink_PSearch_NeedsWork> eraser = fPSearch_NeedsWork;
		eraser; eraser.Advance())
		{
		PSearch* thePSearch = eraser.Current();
		if (thePSearch->fPQuery_Using.empty())
			sEraseMust(kDebug, fMap_PSearch, thePSearch->fRelHead);
		}
	}

ZRef<ZDatonSet::DatonSet> Source_DatonSet::GetDatonSet()
	{ return fDatonSet; }

ZRef<Event> Source_DatonSet::Insert(const Daton& iDaton)
	{
	ZGuardMtxR guard(fMtxR);
	this->pModify(iDaton, sAsVal(iDaton), true);
	ZRef<Event> result = this->pConditionalPushDown();
	guard.Release();

	Source::pTriggerResultsAvailable();

	return result;
	}

ZRef<Event> Source_DatonSet::Erase(const Daton& iDaton)
	{
	ZGuardMtxR guard(fMtxR);
	this->pModify(iDaton, sAsVal(iDaton), false);
	ZRef<Event> result = this->pConditionalPushDown();
	guard.Release();

	Source::pTriggerResultsAvailable();

	return result;
	}

ZRef<Event> Source_DatonSet::Replace(const ZDatonSet::Daton& iOld, const ZDatonSet::Daton& iNew)
	{
	ZGuardMtxR guard(fMtxR);
	this->pModify(iOld, sAsVal(iOld), false);
	this->pModify(iNew, sAsVal(iNew), true);
	ZRef<Event> result = this->pConditionalPushDown();
	guard.Release();

	Source::pTriggerResultsAvailable();

	return result;
	}

size_t Source_DatonSet::OpenTransaction()
	{
	ZAcqMtxR acq(fMtxR);
	if (fStack_Map_Pending.empty())
		fDatonSet_Temp = fDatonSet->Fork();

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
	ZGuardMtxR guard(fMtxR);

	ZAssert(iIndex == fStack_Map_Pending.size());

	this->pChangedAll();
	fStack_Map_Pending.pop_back();
	this->pConditionalPushDown();
	if (fStack_Map_Pending.empty())
		{
		ZAssert(fDatonSet_Temp);
		fDatonSet->Join(fDatonSet_Temp);
		fDatonSet_Temp.Clear();
		}
	guard.Release();

	Source::pTriggerResultsAvailable();
	}

void Source_DatonSet::pPull()
	{
	// Get our map in sync with fDatonSet
	ZLOGPF(s, eDebug);
	ZRef<Deltas> theDeltas;
	fDatonSet->GetDeltas(fEvent, fEvent, theDeltas);
	const Map_NamedEvent_Delta_t& theMNED = theDeltas->GetMap();
	if (s)
		s << ", theMNED.size()=" << theMNED.size();

	foreachi (iterMNED, theMNED)
		{
		const NamedEvent& theNamedEvent = iterMNED->first;
		const map<Daton, bool>& theStatements = iterMNED->second->GetStatements();
		if (s)
			{
			s << ", theStatements.size()=" << theStatements.size();
			s << ", clk:" << theNamedEvent.GetEvent();
			}

		foreachi (iterStmts, theStatements)
			{
			const Daton& theDaton = iterStmts->first;

			if (s)
				s << "\n" << (iterStmts->second ? "+" : "-") << ":";

			map<Daton, pair<NamedEvent, ZVal_Any> >::iterator iterMap = fMap.lower_bound(theDaton);
			if (iterMap == fMap.end() || iterMap->first != theDaton)
				{
				s << " NFo";
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
					s << " " << iterMap->second.first.GetEvent() << (alb?" alb":"") << (bla?" blb":"");

				if (alb)
					{
					// theNamedEvent is more recent than what we've got and thus supersedes it.
					if (s)
						s << " MRc";

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
						s << " Old";
					}
				}

			if (s)
				s << " " << theDaton;
			}
		}
	}

ZRef<Event> Source_DatonSet::pConditionalPushDown()
	{
	if (not fStack_Map_Pending.empty())
		{
		ZAssert(fDatonSet_Temp);
		return fDatonSet->GetEvent()->Joined(fDatonSet_Temp->TickleClock());
		}
	else
		{
		ZRef<DatonSet> theDS = fDatonSet_Temp ? fDatonSet_Temp : fDatonSet;
		foreachi (ii, fMap_Pending)
			{
			if (ii->second.second)
				theDS->Insert(ii->first);
			else
				theDS->Erase(ii->first);
			}
		fMap_Pending.clear();
		return theDS->GetEvent();
		}
	}

void Source_DatonSet::pModify(const ZDatonSet::Daton& iDaton, const ZVal_Any& iVal, bool iSense)
	{
	Map_Pending::iterator ii = fMap_Pending.find(iDaton);
	if (fMap_Pending.end() == ii)
		{
		fMap_Pending.insert(make_pair(iDaton, make_pair(iVal, iSense)));
		}
	else if (ii->second.second == iSense)
		{
		if (ZLOGF(s, eDebug))
			s << "\nDaton: " << iDaton;
		ZDebugStop(0);
		}
	else
		{
		fMap_Pending.erase(ii);
		}
	this->pChanged(iVal);
	}


void Source_DatonSet::pChanged(const ZVal_Any& iVal)
	{
	const ZMap_Any theMap = iVal.Get<ZMap_Any>();
	RelHead theRH;
	for (ZMap_Any::Index_t i = theMap.Begin(); i != theMap.End(); ++i)
		theRH |= RA::ColName(theMap.NameOf(i));

	for (Map_PSearch::iterator iterPSearch = fMap_PSearch.begin();
		iterPSearch != fMap_PSearch.end(); ++iterPSearch)
		{
		PSearch* thePSearch = &iterPSearch->second;
		if (sIncludes(theRH, thePSearch->fRelHead))
			{
			for (set<PQuery*>::iterator iterPQuery = thePSearch->fPQuery_Using.begin();
				iterPQuery != thePSearch->fPQuery_Using.end(); ++iterPQuery)
				{
				PQuery* thePQuery = *iterPQuery;
				sQInsertBack(fPQuery_NeedsWork, thePQuery);
				for (set<PSearch*>::iterator iterPSearch_Used =
					thePQuery->fPSearch_Used.begin();
					iterPSearch_Used != thePQuery->fPSearch_Used.end();
					++iterPSearch_Used)
					{
					PSearch* thePSearch_Used = *iterPSearch_Used;
					if (thePSearch_Used != thePSearch)
						{
						sEraseMust(kDebug, thePSearch_Used->fPQuery_Using, thePQuery);

						if (thePSearch_Used->fPQuery_Using.empty())
							sQInsertBack(fPSearch_NeedsWork, thePSearch_Used);
						}
					}
				thePQuery->fPSearch_Used.clear();
				}
			thePSearch->fPQuery_Using.clear();
			thePSearch->fResult.Clear();
			sQInsertBack(fPSearch_NeedsWork, thePSearch);
			}
		}
	}

void Source_DatonSet::pChangedAll()
	{
	for (Map_PSearch::iterator iterPSearch = fMap_PSearch.begin();
		iterPSearch != fMap_PSearch.end(); ++iterPSearch)
		{
		PSearch* thePSearch = &iterPSearch->second;
		for (set<PQuery*>::iterator iterPQuery = thePSearch->fPQuery_Using.begin();
			iterPQuery != thePSearch->fPQuery_Using.end(); ++iterPQuery)
			{
			PQuery* thePQuery = *iterPQuery;
			sQInsertBack(fPQuery_NeedsWork, thePQuery);
			for (set<PSearch*>::iterator iterPSearch_Used =
				thePQuery->fPSearch_Used.begin();
				iterPSearch_Used != thePQuery->fPSearch_Used.end();
				++iterPSearch_Used)
				{
				PSearch* thePSearch_Used = *iterPSearch_Used;
				if (thePSearch_Used != thePSearch)
					{
					sEraseMust(kDebug, thePSearch_Used->fPQuery_Using, thePQuery);

					if (thePSearch_Used->fPQuery_Using.empty())
						sQInsertBack(fPSearch_NeedsWork, thePSearch_Used);
					}
				}
			thePQuery->fPSearch_Used.clear();
			}
		thePSearch->fPQuery_Using.clear();
		thePSearch->fResult.Clear();
		sQInsertBack(fPSearch_NeedsWork, thePSearch);
		}
	}

ZRef<QueryEngine::Walker> Source_DatonSet::pMakeWalker_Concrete(PQuery* iPQuery, const RelHead& iRelHead)
	{
	++fWalkerCount;

	PSearch* thePSearch;
	Map_PSearch::iterator iterPSearch = fMap_PSearch.find(iRelHead);
	if (iterPSearch != fMap_PSearch.end())
		{
		thePSearch = &iterPSearch->second;
		}
	else
		{
		pair<Map_PSearch::iterator,bool> inPSearch =
			fMap_PSearch.insert(make_pair(iRelHead, PSearch()));

		thePSearch = &inPSearch.first->second;
		thePSearch->fRelHead = iRelHead;
		}

	sInsertMust(kDebug, iPQuery->fPSearch_Used, thePSearch);
	sInsertMust(kDebug, thePSearch->fPQuery_Using, iPQuery);

	if (not thePSearch->fResult)
		{
		ZRef<QueryEngine::Walker> theWalker =
			new Walker_Concrete(this, vector<string8>(iRelHead.begin(), iRelHead.end()));
		thePSearch->fResult = sDoQuery(theWalker);
		}

	return new QE::Walker_Result(thePSearch->fResult);
	}

ZRef<QE::Walker> Source_DatonSet::pMakeWalker_Search(
	PQuery* iPQuery, const ZRef<QE::Expr_Rel_Search>& iRel)
	{
	// This is where we would be able to take advantage of indices. For the moment
	// just do it the dumb way.

	const RelationalAlgebra::Rename& theRename = iRel->GetRename();
	RelationalAlgebra::RelHead theRelHead;
	for (RelationalAlgebra::Rename::const_iterator ii = theRename.begin(); ii != theRename.end(); ++ii)
		theRelHead |= ii->first;

	ZRef<QueryEngine::Walker> theWalker;
	const ZRef<ZExpr_Bool>& theExpr_Bool = iRel->GetExpr_Bool();
	if (theExpr_Bool && theExpr_Bool != sTrue())
		{
		const RelationalAlgebra::RelHead augmented = theRelHead | sGetNames(theExpr_Bool);
		if (augmented.size() != theRelHead.size())
			{
			theWalker = this->pMakeWalker_Concrete(iPQuery, augmented);
			theWalker = new QE::Walker_Restrict(theWalker, theExpr_Bool);
			theWalker = new QE::Walker_Project(theWalker, theRelHead);
			}
		else
			{
			theWalker = this->pMakeWalker_Concrete(iPQuery, theRelHead);
			theWalker = new QE::Walker_Restrict(theWalker, theExpr_Bool);
			}
		}
	else
		{
		theWalker = this->pMakeWalker_Concrete(iPQuery, theRelHead);
		}

	for (RA::Rename::const_iterator iterRename = theRename.begin();
		iterRename != theRename.end(); ++iterRename)
		{
		if (iterRename->first != iterRename->second)
			theWalker = new QE::Walker_Rename(theWalker, iterRename->second, iterRename->first);
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
	for (size_t xx = 0; xx < iWalker->fNames.size(); ++xx)
		oOffsets[iWalker->fNames[xx]] = ioBaseOffset++;
	}

bool Source_DatonSet::pReadInc_Concrete(ZRef<Walker_Concrete> iWalker, ZVal_Any* ioResults)
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
				vector<ZVal_Any> subset;
				subset.reserve(theCount);
				for (size_t xx = 0; xx < theCount; ++xx)
					{
					// Empty name indicates that we want the Daton itself.
					if (theNamesPtr[xx].empty())
						{
						const ZVal_Any& theVal = iWalker->fCurrent_Main->first;
						ioResults[iWalker->fBaseOffset + xx] = theVal;
						subset.push_back(theVal);
						}
					else if (const ZVal_Any* theVal = sPGet(*theMap, theNamesPtr[xx]))
						{
						ioResults[iWalker->fBaseOffset + xx] = *theVal;
						subset.push_back(*theVal);
						}
					else
						{
						gotAll = false;
						break;
						}
					}

				if (gotAll && sQInsert(iWalker->fPriors, subset))
					{
					if (ZLOGF(s, eDebug + 2))
						ZYad_ZooLibStrim::sToStrim(sYadR(*theMap), s);
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
				vector<ZVal_Any> subset;
				subset.reserve(theCount);
				bool gotAll = true;
				for (size_t xx = 0; xx < theCount; ++xx)
					{
					if (theNamesPtr[xx].empty())
						{
						const ZVal_Any& theVal = iWalker->fCurrent_Pending->first;
						ioResults[iWalker->fBaseOffset + xx] = theVal;
						subset.push_back(theVal);
						}
					else if (const ZVal_Any* theVal = theMap->PGet(theNamesPtr[xx]))
						{
						ioResults[iWalker->fBaseOffset + xx] = *theVal;
						subset.push_back(*theVal);
						}
					else
						{
						gotAll = false;
						break;
						}
					}

				if (gotAll && sQInsert(iWalker->fPriors, subset))
					{
					if (ZLOGF(s, eDebug + 2))
						ZYad_ZooLibStrim::sToStrim(sYadR(*theMap), s);

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
