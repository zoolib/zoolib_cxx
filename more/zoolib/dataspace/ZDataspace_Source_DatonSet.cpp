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
#include "zoolib/ZCompare.h"
#include "zoolib/ZMACRO_foreach.h"
#include "zoolib/ZStream_Data_T.h"
#include "zoolib/ZStrim_Stream.h"
#include "zoolib/ZStrimU_StreamUTF8Buffered.h"
#include "zoolib/ZStrimmer_Streamer.h"
#include "zoolib/ZStringf.h"
#include "zoolib/ZUtil_STL_map.h"
#include "zoolib/ZUtil_STL_vector.h"
#include "zoolib/ZUtil_Strim_IntervalTreeClock.h"
#include "zoolib/ZVisitor_Expr_Bool_ValPred_Do_GetNames.h"
#include "zoolib/ZYad_Any.h"
#include "zoolib/ZYad_ZooLibStrim.h"

#include "zoolib/dataspace/ZDataspace_Source_DatonSet.h"

#include "zoolib/QueryEngine/ResultFromWalker.h"
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

// =================================================================================================
// MARK: -

namespace ZDataspace {

using ZDatonSet::DatonSet;
using ZDatonSet::Daton;
using ZDatonSet::Deltas;
using ZDatonSet::Vector_Event_Delta_t;

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

//ZVal_Any sAsVal(const Daton& iDaton)
//	{ return spAsVal(iDaton.GetData()); }
//
//Daton sAsDaton(const ZVal_Any& iVal)
//	{
//	ZData_Any theData;
//	ZYad_ZooLibStrim::sToStrim(sYadR(iVal),
//		ZStrimW_StreamUTF8(sStreamRWPos_Data_T(theData)));
//	return theData;
//	}

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
		{ this->pSetResult(fSource->pMakeWalker_Concrete(fPQuery, iExpr->GetConcreteHead())); }

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
	Walker_Concrete(ZRef<Source_DatonSet> iSource, const ConcreteHead& iConcreteHead)
	:	fSource(iSource)
	,	fConcreteHead(iConcreteHead)
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
	const ConcreteHead fConcreteHead;
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
	set<PScan*> fPScan_Used;
	ZRef<QE::Result> fResult;
	};

// =================================================================================================
// MARK: - Source_DatonSet::PScan

class Source_DatonSet::DLink_PScan_NeedsWork
:	public DListLink<PScan, DLink_PScan_NeedsWork, kDebug>
	{};

class Source_DatonSet::PScan
:	public DLink_PScan_NeedsWork
	{
public:
	PScan() {}

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
	for (DListEraser<PScan,DLink_PScan_NeedsWork> eraser = fPScan_NeedsWork;
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
			// Detach from any depended-upon PScan
			for (set<PScan*>::iterator iterPScan = thePQuery->fPScan_Used.begin();
				iterPScan != thePQuery->fPScan_Used.end(); ++iterPScan)
				{
				PScan* thePScan = *iterPScan;
				sEraseMust(kDebug, thePScan->fPQuery_Using, thePQuery);
				if (thePScan->fPQuery_Using.empty())
					sQInsertBack(fPScan_NeedsWork, thePScan);
				}
			thePQuery->fPScan_Used.clear();

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

		thePQuery->fResult = sResultFromWalker(theWalker);
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

	// Remove any unused PScanes
	for (DListEraser<PScan,DLink_PScan_NeedsWork> eraser = fPScan_NeedsWork;
		eraser; eraser.Advance())
		{
		PScan* thePScan = eraser.Current();
		if (thePScan->fPQuery_Using.empty())
			sEraseMust(kDebug, fMap_PScan, thePScan->fRelHead);
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
	fDatonSet->GetDeltas(fEvent, theDeltas, fEvent);
	const Vector_Event_Delta_t& theVector = theDeltas->GetVector();
//##	if (sNotEmpty(theVector) && s)
	if (s)
		s << fDatonSet.Get() << ", theVector.size(): " << theVector.size();

	foreachi (iterVector, theVector)
		{
		const ZRef<Event>& theEvent = iterVector->first;
		const map<Daton, bool>& theStatements = iterVector->second->GetStatements();
		if (s and false)
			{
			s << ", theStatements.size()=" << theStatements.size();
			s << ", clk:" << theEvent;
			}

		foreachi (iterStmts, theStatements)
			{
			const Daton& theDaton = iterStmts->first;

			if (s and false)
				s << "\n" << (iterStmts->second ? "+" : "-") << ":";

			map<Daton, pair<ZRef<Event>, ZVal_Any> >::iterator iterMap = fMap.lower_bound(theDaton);
			if (iterMap == fMap.end() || iterMap->first != theDaton)
				{
				if (s and false)
					s << " NFo";
				if (iterStmts->second)
					{
					const ZVal_Any theVal = sAsVal(theDaton);
					this->pChanged(theVal);

					fMap.insert(iterMap,
						make_pair(theDaton,
						pair<ZRef<Event>, ZVal_Any>(theEvent, theVal)));
					}
				}
			else
				{
				const bool alb = sIsBefore(iterMap->second.first, theEvent);
				const bool bla = sIsBefore(theEvent, iterMap->second.first);

				if (s and false)
					s << " " << iterMap->second.first << (alb?" alb":"") << (bla?" blb":"");

				if (alb)
					{
					// theNamedEvent is more recent than what we've got and thus supersedes it.
					if (s)
						s << " MRc";

					const ZVal_Any theVal = sAsVal(theDaton);
					this->pChanged(theVal);

					if (iterStmts->second)
						iterMap->second = pair<ZRef<Event>, ZVal_Any>(theEvent, theVal);
					else
						fMap.erase(iterMap);
					}
				else
					{
					if (s and false)
						s << " Old";
					}
				}

			if (s and false)
				s << " " << theDaton;
			}
		}
	}

ZRef<Event> Source_DatonSet::pConditionalPushDown()
	{
	if (sNotEmpty(fStack_Map_Pending))
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

	for (Map_PScan::iterator iterPScan = fMap_PScan.begin();
		iterPScan != fMap_PScan.end(); ++iterPScan)
		{
		PScan* thePScan = &iterPScan->second;
		if (sIncludes(theRH, thePScan->fRelHead))
			{
			for (set<PQuery*>::iterator iterPQuery = thePScan->fPQuery_Using.begin();
				iterPQuery != thePScan->fPQuery_Using.end(); ++iterPQuery)
				{
				PQuery* thePQuery = *iterPQuery;
				sQInsertBack(fPQuery_NeedsWork, thePQuery);
				for (set<PScan*>::iterator iterPScan_Used =
					thePQuery->fPScan_Used.begin();
					iterPScan_Used != thePQuery->fPScan_Used.end();
					++iterPScan_Used)
					{
					PScan* thePScan_Used = *iterPScan_Used;
					if (thePScan_Used != thePScan)
						{
						sEraseMust(kDebug, thePScan_Used->fPQuery_Using, thePQuery);

						if (thePScan_Used->fPQuery_Using.empty())
							sQInsertBack(fPScan_NeedsWork, thePScan_Used);
						}
					}
				thePQuery->fPScan_Used.clear();
				}
			thePScan->fPQuery_Using.clear();
			thePScan->fResult.Clear();
			sQInsertBack(fPScan_NeedsWork, thePScan);
			}
		}
	}

void Source_DatonSet::pChangedAll()
	{
	for (Map_PScan::iterator iterPScan = fMap_PScan.begin();
		iterPScan != fMap_PScan.end(); ++iterPScan)
		{
		PScan* thePScan = &iterPScan->second;
		for (set<PQuery*>::iterator iterPQuery = thePScan->fPQuery_Using.begin();
			iterPQuery != thePScan->fPQuery_Using.end(); ++iterPQuery)
			{
			PQuery* thePQuery = *iterPQuery;
			sQInsertBack(fPQuery_NeedsWork, thePQuery);
			for (set<PScan*>::iterator iterPScan_Used =
				thePQuery->fPScan_Used.begin();
				iterPScan_Used != thePQuery->fPScan_Used.end();
				++iterPScan_Used)
				{
				PScan* thePScan_Used = *iterPScan_Used;
				if (thePScan_Used != thePScan)
					{
					sEraseMust(kDebug, thePScan_Used->fPQuery_Using, thePQuery);

					if (thePScan_Used->fPQuery_Using.empty())
						sQInsertBack(fPScan_NeedsWork, thePScan_Used);
					}
				}
			thePQuery->fPScan_Used.clear();
			}
		thePScan->fPQuery_Using.clear();
		thePScan->fResult.Clear();
		sQInsertBack(fPScan_NeedsWork, thePScan);
		}
	}

ZRef<QueryEngine::Walker> Source_DatonSet::pMakeWalker_Concrete(
	PQuery* iPQuery, const ConcreteHead& iConcreteHead)
	{
	++fWalkerCount;

	const RelHead theRelHead_Required = RelationalAlgebra::sRelHead_Required(iConcreteHead);

	PScan* thePScan;
	Map_PScan::iterator iterPScan = fMap_PScan.find(theRelHead_Required);
	if (iterPScan != fMap_PScan.end())
		{
		thePScan = &iterPScan->second;
		}
	else
		{
		pair<Map_PScan::iterator,bool> inPScan =
			fMap_PScan.insert(make_pair(theRelHead_Required, PScan()));

		thePScan = &inPScan.first->second;
		thePScan->fRelHead = theRelHead_Required;
		}

	sInsertMust(kDebug, iPQuery->fPScan_Used, thePScan);
	sInsertMust(kDebug, thePScan->fPQuery_Using, iPQuery);

	if (not thePScan->fResult)
		{
		ZRef<QueryEngine::Walker> theWalker = new Walker_Concrete(this, iConcreteHead);
		thePScan->fResult = sResultFromWalker(theWalker);
		}

	return new QE::Walker_Result(thePScan->fResult);
	}

ZRef<QE::Walker> Source_DatonSet::pMakeWalker_Search(
	PQuery* iPQuery, const ZRef<QE::Expr_Rel_Search>& iRel)
	{
	// This is where we would be able to take advantage of indices. For the moment
	// just do it the dumb way.

	ZUnimplemented();

	const RA::Rename& theRename = iRel->GetRename();
	RA::RelHead theRH_Required;
	for (RA::Rename::const_iterator ii = theRename.begin(); ii != theRename.end(); ++ii)
		theRH_Required |= ii->first;

	const RA::RelHead& theRH_Optional = iRel->GetRelHead_Optional();

	ZRef<QE::Walker> theWalker;
	const ZRef<ZExpr_Bool>& theExpr_Bool = iRel->GetExpr_Bool();
	if (theExpr_Bool && theExpr_Bool != sTrue())
		{
		const RelHead augmented = theRH_Required | sGetNames(theExpr_Bool);
		theWalker = this->pMakeWalker_Concrete(iPQuery, RA::sConcreteHead(augmented, theRH_Optional));
		theWalker = new QE::Walker_Restrict(theWalker, theExpr_Bool);

		if (augmented.size() != theRH_Required.size())
			theWalker = new QE::Walker_Project(theWalker, theRH_Required);
		}
	else
		{
		theWalker = this->pMakeWalker_Concrete(iPQuery, RA::sConcreteHead(theRH_Required, theRH_Optional));
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
	const ConcreteHead& theConcreteHead = iWalker->fConcreteHead;
	for (ConcreteHead::const_iterator ii =
		theConcreteHead.begin(), end = theConcreteHead.end();
		ii != end; ++ii)
		{ oOffsets[ii->first] = ioBaseOffset++; }
	}

bool Source_DatonSet::pReadInc_Concrete(ZRef<Walker_Concrete> iWalker, ZVal_Any* ioResults)
	{
	++fReadCount;

	const ConcreteHead& theConcreteHead = iWalker->fConcreteHead;

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
				subset.reserve(theConcreteHead.size());
				size_t offset = iWalker->fBaseOffset;
				for (ConcreteHead::const_iterator
					ii = theConcreteHead.begin(), end = theConcreteHead.end();
					ii != end; ++ii, ++offset)
					{
					// Empty name indicates that we want the Daton itself.
					const string8& theName = ii->first;
					if (theName.empty())
						{
						const ZVal_Any& theVal = iWalker->fCurrent_Main->first;
						ioResults[offset] = theVal;
						subset.push_back(theVal);
						}
					else if (const ZVal_Any* theVal = sPGet(*theMap, theName))
						{
						ioResults[offset] = *theVal;
						subset.push_back(*theVal);
						}
					else if (not ii->second)
						{
						ioResults[offset] = Default_t();
						subset.push_back(Default_t());
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
				bool gotAll = true;
				vector<ZVal_Any> subset;
				subset.reserve(theConcreteHead.size());
				size_t offset = iWalker->fBaseOffset;
				for (ConcreteHead::const_iterator
					ii = theConcreteHead.begin(), end = theConcreteHead.end();
					ii != end; ++ii, ++offset)
					{
					const string8& theName = ii->first;
					if (theName.empty())
						{
						const ZVal_Any& theVal = iWalker->fCurrent_Pending->first;
						ioResults[offset] = theVal;
						subset.push_back(theVal);
						}
					else if (const ZVal_Any* theVal = theMap->PGet(theName))
						{
						ioResults[offset] = *theVal;
						subset.push_back(*theVal);
						}
					else if (not ii->second)
						{
						ioResults[offset] = Default_t();
						subset.push_back(Default_t());
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
