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

#include "zoolib/ZCallable_PMF.h"
#include "zoolib/ZLog.h"
#include "zoolib/ZString.h"
#include "zoolib/ZUtil_STL_map.h"
#include "zoolib/ZUtil_STL_set.h"
#include "zoolib/ZWorker_Callable.h"
#include "zoolib/ZWorkerRunner_CFRunLoop.h"

#include "zoolib/dataspace/ZDataspace_Source_SQLite.h"
#include "zoolib/dataspace/ZDataspace_Util_Strim.h"

#include "zoolib/zra/ZRA_AsSQL.h"
#include "zoolib/zra/ZRA_GetRelHead.h"
#include "zoolib/zra/ZRA_Util_Strim_RelHead.h"

namespace ZooLib {
namespace ZDataspace {

using std::map;
using std::pair;
using std::make_pair;
using std::set;
using std::vector;

using namespace ZSQLite;

using ZRA::RelName;

const ZStrimW& operator<<(const ZStrimW& w, const ZRA::Rename& iRename)
	{
	w << "(";
	bool isSubsequent = false;
	for (ZRA::Rename::const_iterator i = iRename.begin(); i != iRename.end(); ++i)
		{
		if (isSubsequent)
			w << ", ";
		isSubsequent = true;
		w << i->first << "<--" << i->second;
		}
	w << ")";
	return w;
	}

// =================================================================================================
#pragma mark -
#pragma mark * Source_SQLite::ClientQuery

class Source_SQLite::DLink_ClientQuery_InPQuery
:	public DListLink<ClientQuery, DLink_ClientQuery_InPQuery, kDebug>
	{};

class Source_SQLite::ClientQuery
:	public Source_SQLite::DLink_ClientQuery_InPQuery
	{
public:
	ClientQuery(int64 iRefcon, PQuery* iPQuery)
	:	fRefcon(iRefcon),
		fPQuery(iPQuery)
		{}

	int64 fRefcon;
	PQuery* fPQuery;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Source_SQLite::PQuery

class Source_SQLite::PQuery
	{
public:
	PQuery(ZRef<ZRA::Expr_Rel> iRel)
	:	fRel(iRel)
		{}

	ZRef<ZRA::Expr_Rel> fRel;
	RelHead fRelHead;
	string8 fSQL;
	DListHead<DLink_ClientQuery_InPQuery> fClientQueries;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Source_SQLite

Source_SQLite::Source_SQLite(ZRef<ZSQLite::DB> iDB)
:	fDB(iDB)
,	fChangeCount(0)
,	fClock(Clock::sSeed())
	{
	for (ZRef<Iter> iterTables = new Iter(fDB, "select name from sqlite_master;");
		iterTables->HasValue(); iterTables->Advance())
		{
		const string8 theTableName = iterTables->Get(0).Get<string8>();
		ZAssert(!theTableName.empty());
		RelHead theRelHead;
		for (ZRef<Iter> iterTable = new Iter(fDB, "pragma table_info(" + theTableName + ");");
			iterTable->HasValue(); iterTable->Advance())
			{
			theRelHead |= iterTable->Get(1).Get<string8>();
			}
		theRelHead |= "oid";

		if (!theRelHead.empty())
			ZUtil_STL::sInsertMustNotContain(kDebug, fMap_Tables, theTableName, theRelHead);
		}

//	ZRef<ZWorker> theWorker = MakeWorker(MakeCallable(this, &Source_SQLite::pCheck));
//	ZWorkerRunner_CFRunLoop::sMain()->Attach(theWorker);
//	theWorker->Wake();
	}

Source_SQLite::~Source_SQLite()
	{}

bool Source_SQLite::Intersects(const RelHead& iRelHead)
	{
	for (map<string8, RelHead>::const_iterator iterTables = fMap_Tables.begin();
		iterTables != fMap_Tables.end(); ++iterTables)
		{
		if (!(ZRA::sPrefixInserted(iterTables->first + "_", iterTables->second) & iRelHead).empty())
			return true;
		}
	return false;
	}

void Source_SQLite::ModifyRegistrations(
	const AddedQuery* iAdded, size_t iAddedCount,
	const int64* iRemoved, size_t iRemovedCount)
	{
	if (iAddedCount || iRemovedCount)
		{
		this->pInvokeCallable_ResultsAvailable();
		}

	while (iAddedCount--)
		{
		ZRef<ZRA::Expr_Rel> theRel = iAdded->GetRel();

		pair<Map_Rel_PQuery::iterator,bool> inPQuery =
			fMap_Rel_PQuery.insert(make_pair(theRel, PQuery(theRel)));

		PQuery* thePQuery = &inPQuery.first->second;
		
		if (inPQuery.second)
			{
			string8 asSQL;
			ZRA::sWriteAsSQL(fMap_Tables, theRel, ZStrimW_String(asSQL));
			if (ZLOGF(s, eDebug))
				s << asSQL;
			thePQuery->fSQL = asSQL;
			thePQuery->fRelHead = sGetRelHead(theRel);
			}

		const int64 theRefcon = iAdded->GetRefcon();

		std::map<int64, ClientQuery>::iterator iterClientQuery =
			fMap_RefconToClientQuery.insert(
			make_pair(theRefcon, ClientQuery(theRefcon, thePQuery))).first;

		thePQuery->fClientQueries.Insert(&iterClientQuery->second);

		++iAdded;
		}

	while (iRemovedCount--)
		{
		int64 theRefcon = *iRemoved++;

		std::map<int64, ClientQuery>::iterator iterClientQuery =
			fMap_RefconToClientQuery.find(theRefcon);
		
		ClientQuery* theClientQuery = &iterClientQuery->second;
		
		PQuery* thePQuery = theClientQuery->fPQuery;
		thePQuery->fClientQueries.Erase(theClientQuery);
		if (thePQuery->fClientQueries.Empty())
			ZUtil_STL::sEraseMustContain(kDebug, fMap_Rel_PQuery, thePQuery->fRel);
		
		fMap_RefconToClientQuery.erase(iterClientQuery);
		}
	}

void Source_SQLite::CollectResults(std::vector<QueryResult>& oChanged)
	{
	this->pCollectResultsCalled();
	oChanged.clear();

	for (Map_Rel_PQuery::iterator iterPQuery = fMap_Rel_PQuery.begin();
		iterPQuery != fMap_Rel_PQuery.end(); ++iterPQuery)
		{
		PQuery* thePQuery = &iterPQuery->second;
		vector<ZVal_Any> thePackedRows;
		for (ZRef<Iter> theIter = new Iter(fDB, thePQuery->fSQL);
			theIter->HasValue(); theIter->Advance())
			{
			const size_t theCount = theIter->Count();
			for (size_t x = 0; x < theCount; ++x)
				thePackedRows.push_back(theIter->Get(x));
			}

		ZRef<ZQE::Result> theResult =
			new ZQE::Result(thePQuery->fRelHead, &thePackedRows, nullptr);
		
		for (DListIterator<ClientQuery, DLink_ClientQuery_InPQuery>
			iterCS = thePQuery->fClientQueries; iterCS; iterCS.Advance())
			{
			oChanged.push_back(QueryResult(iterCS.Current()->fRefcon, theResult, null));
			}
		}
	}

} // namespace ZDataspace
} // namespace ZooLib
