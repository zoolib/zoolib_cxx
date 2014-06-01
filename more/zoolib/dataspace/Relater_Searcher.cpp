/* -------------------------------------------------------------------------------------------------
Copyright (c) 2014 Andrew Green
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
#include "zoolib/ZUtil_STL_map.h"
#include "zoolib/ZUtil_STL_set.h"

#include "zoolib/dataspace/Relater_Searcher.h"

#include "zoolib/QueryEngine/ResultFromWalker.h"
#include "zoolib/QueryEngine/Transform_Search.h"
#include "zoolib/QueryEngine/Visitor_DoMakeWalker.h"
#include "zoolib/QueryEngine/Walker_Project.h"
#include "zoolib/QueryEngine/Walker_Result.h"
#include "zoolib/QueryEngine/Walker_Rename.h"
#include "zoolib/QueryEngine/Walker_Restrict.h"

#include "zoolib/RelationalAlgebra/GetRelHead.h"
#include "zoolib/RelationalAlgebra/Expr_Rel_Concrete.h"
#include "zoolib/RelationalAlgebra/Util_Strim_Rel.h"
#include "zoolib/RelationalAlgebra/Util_Strim_RelHead.h"

namespace ZooLib {
namespace ZDataspace {

using std::make_pair;
using std::map;
using std::pair;
using std::set;
using std::vector;

namespace QE = QueryEngine;
namespace RA = RelationalAlgebra;

using namespace ZUtil_STL;

// =================================================================================================
// MARK: - Relater_Searcher::Walker_Concrete

class Relater_Searcher::Walker_Concrete : public QE::Walker
	{
public:
	Walker_Concrete(ZRef<Relater_Searcher> iSource, const ConcreteHead& iConcreteHead)
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

	const ZRef<Relater_Searcher> fSource;
	const ConcreteHead fConcreteHead;
	size_t fBaseOffset;
	Map_Main::const_iterator fCurrent_Main;
	Map_Pending::const_iterator fCurrent_Pending;
	std::set<std::vector<ZVal_Any> > fPriors;
	};

// =================================================================================================
// MARK: - Relater_Searcher::ClientQuery

class Relater_Searcher::DLink_ClientQuery_InPQuery
:	public DListLink<ClientQuery, DLink_ClientQuery_InPQuery, kDebug>
	{};

class Relater_Searcher::DLink_ClientQuery_NeedsWork
:	public DListLink<ClientQuery, DLink_ClientQuery_NeedsWork, kDebug>
	{};

class Relater_Searcher::ClientQuery
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
// MARK: - Relater_Searcher::PQuery

class Relater_Searcher::DLink_PQuery_NeedsWork
:	public DListLink<PQuery, DLink_PQuery_NeedsWork, kDebug>
	{};

class Relater_Searcher::PQuery
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
// MARK: - Relater_Searcher::PSearch

class Relater_Searcher::DLink_PSearch_NeedsWork
:	public DListLink<PSearch, DLink_PSearch_NeedsWork, kDebug>
	{};

class Relater_Searcher::PSearch
:	public DLink_PSearch_NeedsWork
	{
public:
	PSearch() {}

	RelHead fRelHead;
	set<PQuery*> fPQuery_Using;
	ZRef<QE::Result> fResult;
	};

// =================================================================================================
// MARK: - Relater_Searcher

void Relater_Searcher::ForceUpdate()
	{ Source::pTriggerResultsAvailable(); }

Relater_Searcher::Relater_Searcher(ZRef<Searcher> iSearcher)
:	fSearcher(iSearcher)
	{}

Relater_Searcher::~Relater_Searcher()
	{
	for (DListEraser<PSearch,DLink_PSearch_NeedsWork> eraser = fPSearch_NeedsWork;
		eraser; eraser.Advance())
		{}

	for (DListEraser<ClientQuery,DLink_ClientQuery_NeedsWork> eraser = fClientQuery_NeedsWork;
		eraser; eraser.Advance())
		{}
	}

bool Relater_Searcher::Intersects(const RelHead& iRelHead)
	{ return true; }

void Relater_Searcher::ModifyRegistrations(
	const AddedQuery* iAdded, size_t iAddedCount,
	const int64* iRemoved, size_t iRemovedCount)
	{
	ZGuardMtxR guard(fMtxR);

	for (/*no init*/; iAddedCount--; ++iAdded)
		{
		ZRef<RA::Expr_Rel> theRel = iAdded->GetRel();

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

void Relater_Searcher::CollectResults(vector<QueryResult>& oChanged)
	{
	this->pCollectResultsCalled();

	ZAcqMtxR acq(fMtxR);

	oChanged.clear();

//	this->pPull();

	for (DListEraser<PQuery,DLink_PQuery_NeedsWork> eraser = fPQuery_NeedsWork;
		eraser; eraser.Advance())
		{
		PQuery* thePQuery = eraser.Current();

		ZRef<QE::Walker> theWalker = Visitor_DoMakeWalker(this, thePQuery).Do(thePQuery->fRel);

		thePQuery->fResult = QE::sDoQuery(theWalker);

		for (DListIterator<ClientQuery, DLink_ClientQuery_InPQuery>
			iter = thePQuery->fClientQuery_InPQuery; iter; iter.Advance())
			{ sQInsertBack(fClientQuery_NeedsWork, iter.Current()); }
		}

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
//##		if (thePSearch->fPQuery_Using.empty())
//##			sEraseMust(kDebug, fMap_PSearch, thePSearch->fRelHead);
		}
	}

void Relater_Searcher::pChanged(const ZVal_Any& iVal)
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

void Relater_Searcher::pChangedAll()
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

ZRef<QueryEngine::Walker> Relater_Searcher::pMakeWalker_Concrete(
	PQuery* iPQuery, const ConcreteHead& iConcreteHead)
	{
	const RelHead theRelHead_Required = RelationalAlgebra::sRelHead_Required(iConcreteHead);

	PSearch* thePSearch;
	Map_PSearch::iterator iterPSearch = fMap_PSearch.find(theRelHead_Required);
	if (iterPSearch != fMap_PSearch.end())
		{
		thePSearch = &iterPSearch->second;
		}
	else
		{
		pair<Map_PSearch::iterator,bool> inPSearch =
			fMap_PSearch.insert(make_pair(theRelHead_Required, PSearch()));

		thePSearch = &inPSearch.first->second;
		thePSearch->fRelHead = theRelHead_Required;
		}

	sInsertMust(kDebug, iPQuery->fPSearch_Used, thePSearch);
	sInsertMust(kDebug, thePSearch->fPQuery_Using, iPQuery);

	if (not thePSearch->fResult)
		{
		ZRef<QueryEngine::Walker> theWalker = new Walker_Concrete(this, iConcreteHead);
		thePSearch->fResult = sDoQuery(theWalker);
		}

	return new QE::Walker_Result(thePSearch->fResult);
	}

//ZRef<QE::Walker> Relater_Searcher::pMakeWalker_Search(
//	PQuery* iPQuery, const ZRef<QE::Expr_Rel_Search>& iRel)
//	{
//	// This is where we would be able to take advantage of indices. For the moment
//	// just do it the dumb way.
//
//	const RA::Rename& theRename = iRel->GetRename();
//	RA::RelHead theRH_Required;
//	for (RA::Rename::const_iterator ii = theRename.begin(); ii != theRename.end(); ++ii)
//		theRH_Required |= ii->first;
//
//	const RA::RelHead& theRH_Optional = iRel->GetRelHead_Optional();
//
//	ZRef<QE::Walker> theWalker;
//	const ZRef<ZExpr_Bool>& theExpr_Bool = iRel->GetExpr_Bool();
//	if (theExpr_Bool && theExpr_Bool != sTrue())
//		{
//		const RelHead augmented = theRH_Required | sGetNames(theExpr_Bool);
//		theWalker = this->pMakeWalker_Concrete(iPQuery, RA::sConcreteHead(augmented, theRH_Optional));
//		theWalker = new QE::Walker_Restrict(theWalker, theExpr_Bool);
//
//		if (augmented.size() != theRH_Required.size())
//			theWalker = new QE::Walker_Project(theWalker, theRH_Required);
//		}
//	else
//		{
//		theWalker = this->pMakeWalker_Concrete(iPQuery, RA::sConcreteHead(theRH_Required, theRH_Optional));
//		}
//
//	for (RA::Rename::const_iterator iterRename = theRename.begin();
//		iterRename != theRename.end(); ++iterRename)
//		{
//		if (iterRename->first != iterRename->second)
//			theWalker = new QE::Walker_Rename(theWalker, iterRename->second, iterRename->first);
//		}
//
//	return theWalker;
//	}
//
void Relater_Searcher::pRewind_Concrete(ZRef<Walker_Concrete> iWalker)
	{
//##	iWalker->fCurrent_Main = fMap.begin();
//##	iWalker->fCurrent_Pending = fMap_Pending.begin();
	}

void Relater_Searcher::pPrime_Concrete(ZRef<Walker_Concrete> iWalker,
	const map<string8,size_t>& iOffsets,
	map<string8,size_t>& oOffsets,
	size_t& ioBaseOffset)
	{
//##	iWalker->fCurrent_Main = fMap.begin();
//##	iWalker->fCurrent_Pending = fMap_Pending.begin();
	iWalker->fBaseOffset = ioBaseOffset;
	const ConcreteHead& theConcreteHead = iWalker->fConcreteHead;
	for (ConcreteHead::const_iterator ii =
		theConcreteHead.begin(), end = theConcreteHead.end();
		ii != end; ++ii)
		{ oOffsets[ii->first] = ioBaseOffset++; }
	}

bool Relater_Searcher::pReadInc_Concrete(ZRef<Walker_Concrete> iWalker, ZVal_Any* ioResults)
	{
	const ConcreteHead& theConcreteHead = iWalker->fConcreteHead;

	while (iWalker->fCurrent_Main != fMap.end())
		{
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
