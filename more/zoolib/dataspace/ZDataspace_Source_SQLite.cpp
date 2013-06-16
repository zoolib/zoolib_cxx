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
#include "zoolib/ZUtil_STL_map.h"
#include "zoolib/ZUtil_STL_set.h"

#include "zoolib/dataspace/ZDataspace_Source_SQLite.h"

#include "zoolib/zra/ZRA_AsSQL.h"
#include "zoolib/zra/ZRA_GetRelHead.h"

namespace ZooLib {
namespace ZDataspace {

using std::map;
using std::pair;
using std::make_pair;
using std::set;
using std::vector;

using namespace ZSQLite;
using namespace ZUtil_STL;

using ZRA::ColName;

// =================================================================================================
// MARK: - Source_SQLite::ClientQuery

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
// MARK: - Source_SQLite::PQuery

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
// MARK: - Source_SQLite

Source_SQLite::Source_SQLite(ZRef<ZSQLite::DB> iDB, ZRef<Clock> iClock)
:	fDB(iDB)
,	fClock(iClock)
	{
	for (ZRef<Iter> iterTables = new Iter(fDB, "select name from sqlite_master;");
		iterTables->HasValue(); iterTables->Advance())
		{
		const string8 theTableName = iterTables->Get(0).Get<string8>();
		ZAssert(sNotEmpty(theTableName));
		RelHead theRelHead;
		for (ZRef<Iter> iterTable = new Iter(fDB, "pragma table_info(" + theTableName + ");");
			iterTable->HasValue(); iterTable->Advance())
			{
			theRelHead |= iterTable->Get(1).Get<string8>();
			}
		theRelHead |= "oid";

		if (sNotEmpty(theRelHead))
			sInsertMust(kDebug, fMap_Tables, theTableName, theRelHead);
		}
	}

Source_SQLite::~Source_SQLite()
	{}

bool Source_SQLite::Intersects(const RelHead& iRelHead)
	{
	foreachi (iterTables, fMap_Tables)
		{
		if (sNotEmpty(
			ZRA::sPrefixInserted(iterTables->first + "_", iterTables->second) & iRelHead))
			{ return true; }
		}
	return false;
	}

void Source_SQLite::ModifyRegistrations(
	const AddedQuery* iAdded, size_t iAddedCount,
	const int64* iRemoved, size_t iRemovedCount)
	{
	if (iAddedCount)// || iRemovedCount)
		{
		Source::pTriggerResultsAvailable();
		}

	while (iAddedCount--)
		{
		ZRef<ZRA::Expr_Rel> theRel = iAdded->GetRel();

		pair<Map_Rel_PQuery::iterator,bool> iterPQueryPair =
			fMap_Rel_PQuery.insert(make_pair(theRel, PQuery(theRel)));

		PQuery* thePQuery = &iterPQueryPair.first->second;

		if (iterPQueryPair.second)
			{
			ZRA::sWriteAsSQL(fMap_Tables, theRel, ZStrimW_String<string8>(thePQuery->fSQL));
			thePQuery->fRelHead = sGetRelHead(theRel);
			}

		const int64 theRefcon = iAdded->GetRefcon();
		pair<std::map<int64, ClientQuery>::iterator,bool> iterClientQueryPair =
			fMap_RefconToClientQuery.insert(
			make_pair(theRefcon, ClientQuery(theRefcon, thePQuery)));
		ZAssert(iterClientQueryPair.second);

		sInsertBackMust(thePQuery->fClientQueries, &iterClientQueryPair.first->second);

		++iAdded;
		}

	while (iRemovedCount--)
		{
		int64 theRefcon = *iRemoved++;

		std::map<int64, ClientQuery>::iterator iterClientQuery =
			fMap_RefconToClientQuery.find(theRefcon);

		ClientQuery* theClientQuery = &iterClientQuery->second;

		PQuery* thePQuery = theClientQuery->fPQuery;
		sEraseMust(thePQuery->fClientQueries, theClientQuery);
		if (thePQuery->fClientQueries.IsEmpty())
			sEraseMust(kDebug, fMap_Rel_PQuery, thePQuery->fRel);

		fMap_RefconToClientQuery.erase(iterClientQuery);
		}
	}

void Source_SQLite::CollectResults(std::vector<QueryResult>& oChanged)
	{
	this->pCollectResultsCalled();
	oChanged.clear();

	ZRef<Event> theEvent = fClock->GetEvent();
	foreachi (iterPQuery, fMap_Rel_PQuery)
		{
		const PQuery* thePQuery = &iterPQuery->second;
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
			oChanged.push_back(QueryResult(iterCS.Current()->fRefcon, theResult, theEvent));
			}
		}
	}

} // namespace ZDataspace
} // namespace ZooLib
