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

#include "zoolib/ZUtil_STL.h"
#include "zoolib/dataspace/ZDataspace_Source_SQLite.h"
#include "zoolib/zqe/ZQE_Result_Any.h"
#include "zoolib/zra/ZRA_SQL.h"

namespace ZooLib {
namespace ZDataspace {

using namespace ZSQLite;

// =================================================================================================
#pragma mark -
#pragma mark * Source_SQLite::PQuery

class Source_SQLite::PQuery
	{
public:
	PQuery(int64 iRefcon);

	const int64 fRefcon;
	string fSQL;
	};

Source_SQLite::PQuery::PQuery(int64 iRefcon)
:	fRefcon(iRefcon)
	{}

// =================================================================================================
#pragma mark -
#pragma mark * Source_SQLite

Source_SQLite::Source_SQLite(ZRef<ZSQLite::DB> iDB)
:	fDB(iDB)
	{
	fChangeCount = ::sqlite3_total_changes(fDB->GetDB());
	}

Source_SQLite::~Source_SQLite()
	{}

set<RelHead> Source_SQLite::GetRelHeads()
	{
	set<RelHead> result;

	for (ZRef<Iter> iterTables = new Iter(fDB, "select name from sqlite_master;");
		iterTables->HasValue(); iterTables->Advance())
		{
		const string8 theTableName = iterTables->Get(0).Get_T<string>();
		RelHead theRelHead;
		for (ZRef<Iter> iterTable = new Iter(fDB, "pragma table_info(" + theTableName + ");");
			iterTable->HasValue(); iterTable->Advance())
			{
			theRelHead.Insert(theTableName + "." + iterTable->Get(1).Get_T<string>());
			}
		result.insert(theRelHead);
		}
	return result;
	}

void Source_SQLite::Update(
	bool iLocalOnly,
	AddedSearch* iAdded, size_t iAddedCount,
	int64* iRemoved, size_t iRemovedCount,
	vector<SearchResult>& oChanged,
	Clock& oClock)
	{
	oChanged.clear();

	while (iRemovedCount--)
		{
		PQuery* thePQuery = ZUtil_STL::sEraseAndReturn(kDebug, fMap_RefconToPQuery, *iRemoved++);
		delete thePQuery;
		}

	while (iAddedCount--)
		{
		PQuery* thePQuery = new PQuery(iAdded->fRefcon);
		ZRef<ZRA::SQL::Expr_Rel_SFW> theSFW = ZRA::SQL::sConvert(iAdded->fRel);
		thePQuery->fSQL = sAsSQL(theSFW);
		ZUtil_STL::sInsertMustNotContain(kDebug,
			fMap_RefconToPQuery, thePQuery->fRefcon, thePQuery);
		++iAdded;
		}

	for (map<int64, PQuery*>::iterator i = fMap_RefconToPQuery.begin();
		i != fMap_RefconToPQuery.end(); ++i)
		{
		PQuery* thePQuery = (*i).second;

		SearchResult theSearchResult;
		theSearchResult.fRefcon = thePQuery->fRefcon;

		for (ZRef<Iter> theIter = new Iter(fDB, thePQuery->fSQL);
			theIter->HasValue(); theIter->Advance())
			{
			ZMap_Any theMap;
			for (size_t x = 0; x < theIter->Count(); ++x)
				theMap.Set(theIter->NameOf(x), theIter->Get(x));

			theSearchResult.fResults.push_back(new ZQE::Result_Any(theMap));
			}

		oChanged.push_back(theSearchResult);
		}

	fClock.Event();

	oClock = fClock;
	}

} // namespace ZDataspace
} // namespace ZooLib
