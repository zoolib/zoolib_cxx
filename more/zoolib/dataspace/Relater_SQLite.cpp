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

#include "zoolib/dataspace/Relater_SQLite.h"

#include "zoolib/RelationalAlgebra/AsSQL.h"
#include "zoolib/RelationalAlgebra/GetRelHead.h"

namespace ZooLib {
namespace ZDataspace {

using std::make_pair;
using std::map;
using std::pair;
using std::set;
using std::vector;

using namespace ZSQLite;
using namespace ZUtil_STL;

namespace RA = RelationalAlgebra;

// =================================================================================================
// MARK: - Relater_SQLite::ClientQuery

class Relater_SQLite::DLink_ClientQuery_InPQuery
:	public DListLink<ClientQuery, DLink_ClientQuery_InPQuery, kDebug>
	{};

class Relater_SQLite::ClientQuery
:	public Relater_SQLite::DLink_ClientQuery_InPQuery
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
// MARK: - Relater_SQLite::PQuery

class Relater_SQLite::PQuery
	{
public:
	PQuery(ZRef<RA::Expr_Rel> iRel)
	:	fRel(iRel)
		{}

	ZRef<RA::Expr_Rel> fRel;
	RelHead fRelHead;
	string8 fSQL;
	DListHead<DLink_ClientQuery_InPQuery> fClientQueries;
	};

// =================================================================================================
// MARK: - Relater_SQLite

Relater_SQLite::Relater_SQLite(ZRef<ZSQLite::DB> iDB, ZRef<Clock> iClock)
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
			{ theRelHead |= iterTable->Get(1).Get<string8>(); }
		theRelHead |= RA::ColName("oid");

		if (sNotEmpty(theRelHead))
			sInsertMust(kDebug, fMap_Tables, theTableName, theRelHead);
		}
	}

Relater_SQLite::~Relater_SQLite()
	{}

bool Relater_SQLite::Intersects(const RelHead& iRelHead)
	{
	foreachi (iterTables, fMap_Tables)
		{
		if (sNotEmpty(RA::sPrefixInserted(iterTables->first + "_", iterTables->second) & iRelHead))
			return true;
		}
	return false;
	}

void Relater_SQLite::ModifyRegistrations(
	const AddedQuery* iAdded, size_t iAddedCount,
	const int64* iRemoved, size_t iRemovedCount)
	{
	const bool trigger = iAddedCount || iRemovedCount;

	while (iAddedCount--)
		{
		ZRef<RA::Expr_Rel> theRel = iAdded->GetRel();

		pair<Map_Rel_PQuery::iterator,bool> iterPQueryPair =
			fMap_Rel_PQuery.insert(make_pair(theRel, PQuery(theRel)));

		PQuery* thePQuery = &iterPQueryPair.first->second;

		if (iterPQueryPair.second)
			{
			RA::sWriteAsSQL(fMap_Tables, theRel, ZStrimW_String<string8>(thePQuery->fSQL));
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

	if (trigger)
		Source::pTriggerResultsAvailable();
	}

void Relater_SQLite::CollectResults(std::vector<QueryResult>& oChanged)
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
			for (size_t xx = 0; xx < theCount; ++xx)
				thePackedRows.push_back(theIter->Get(xx));
			}

		ZRef<QueryEngine::Result> theResult =
			new QueryEngine::Result(thePQuery->fRelHead, &thePackedRows);

		for (DListIterator<ClientQuery, DLink_ClientQuery_InPQuery>
			iterCS = thePQuery->fClientQueries; iterCS; iterCS.Advance())
			{
			oChanged.push_back(QueryResult(iterCS.Current()->fRefcon, theResult, theEvent));
			}
		}
	}

} // namespace ZDataspace
} // namespace ZooLib
