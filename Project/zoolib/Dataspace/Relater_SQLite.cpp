// Copyright (c) 2010 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/Util_STL_map.h"
#include "zoolib/Util_STL_set.h"

#include "zoolib/Chan_UTF_string.h"
#include "zoolib/Log.h"

#include "zoolib/Dataspace/Relater_SQLite.h"

#include "zoolib/RelationalAlgebra/AsSQL.h"
#include "zoolib/RelationalAlgebra/GetRelHead.h"

#include "zoolib/ZMACRO_foreach.h"

namespace ZooLib {
namespace Dataspace {

using std::make_pair;
using std::map;
using std::pair;
using std::set;
using std::vector;

using namespace SQLite;
using namespace Util_STL;

namespace RA = RelationalAlgebra;

// =================================================================================================
#pragma mark - Relater_SQLite::ClientQuery

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
#pragma mark - Relater_SQLite::PQuery

class Relater_SQLite::PQuery
	{
public:
	PQuery(ZP<RA::Expr_Rel> iRel)
	:	fRel(iRel)
		{}

	ZP<RA::Expr_Rel> fRel;
	RelHead fRelHead;
	string8 fSQL;
	DListHead<DLink_ClientQuery_InPQuery> fClientQueries;
	};

// =================================================================================================
#pragma mark - Relater_SQLite

Relater_SQLite::Relater_SQLite(ZP<SQLite::DB> iDB)
:	fDB(iDB)
	{
	for (ZP<Iter> iterTables = new Iter(fDB, "select name from sqlite_master;");
		iterTables->HasValue(); iterTables->Advance())
		{
		const string8 theTableName = iterTables->Get(0).Get<string8>();
		ZAssert(sNotEmpty(theTableName));
		RelHead theRelHead;
		for (ZP<Iter> iterTable = new Iter(fDB, "pragma table_info(" + theTableName + ");");
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
	foreacha (entry, fMap_Tables)
		{
		if (sNotEmpty(RA::sPrefixInserted(entry.first + "_", entry.second) & iRelHead))
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
		ZP<RA::Expr_Rel> theRel = iAdded->GetRel();

		pair<Map_Rel_PQuery::iterator,bool> iterPQueryPair =
			fMap_Rel_PQuery.insert(make_pair(theRel, PQuery(theRel)));

		PQuery* thePQuery = &iterPQueryPair.first->second;

		if (iterPQueryPair.second)
			{
			RA::sWriteAsSQL(fMap_Tables, theRel, ChanW_UTF_string8(&thePQuery->fSQL));
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
		Relater::pTrigger_RelaterResultsAvailable();
	}

void Relater_SQLite::CollectResults(std::vector<QueryResult>& oChanged)
	{
	Relater::pCalled_RelaterCollectResults();
	oChanged.clear();

	foreacha (entry, fMap_Rel_PQuery)
		{
		const PQuery* thePQuery = &entry.second;
		vector<Val_ZZ> thePackedRows;
		for (ZP<Iter> theIter = new Iter(fDB, thePQuery->fSQL);
			theIter->HasValue(); theIter->Advance())
			{
			const size_t theCount = theIter->Count();
			for (size_t xx = 0; xx < theCount; ++xx)
				thePackedRows.push_back(theIter->Get(xx).As<Val_ZZ>());
			}

		ZP<QueryEngine::Result> theResult =
			new QueryEngine::Result(thePQuery->fRelHead, &thePackedRows);

		for (DListIterator<ClientQuery, DLink_ClientQuery_InPQuery>
			iterCS = thePQuery->fClientQueries; iterCS; iterCS.Advance())
			{
			oChanged.push_back(QueryResult(iterCS.Current()->fRefcon, theResult));
			}
		}
	}

} // namespace Dataspace
} // namespace ZooLib
