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

#include "zoolib/ZString.h"
#include "zoolib/ZUtil_STL.h"
#include "zoolib/ZUtil_STL_set.h"
#include "zoolib/dataspace/ZDataspace_Source_SQLite.h"
#include "zoolib/zqe/ZQE_Result_Any.h"

#include "zoolib/zra/ZRA_SQL.h"

#include "zoolib/ZLog.h"

namespace ZooLib {
namespace ZDataspace {

using namespace ZSQLite;
using ZRA::RelName;

// =================================================================================================
#pragma mark -
#pragma mark * Source_SQLite::PQuery

class Source_SQLite::PQuery
	{
public:
	PQuery(int64 iRefcon, const string8& iSQL);

	const int64 fRefcon;
	const string8 fSQL;
	};

Source_SQLite::PQuery::PQuery(int64 iRefcon, const string8& iSQL)
:	fRefcon(iRefcon)
,	fSQL(iSQL)
	{}

// =================================================================================================
#pragma mark -
#pragma mark * Helpers

namespace { // anonymous

} // anonymous namespace

// =================================================================================================
#pragma mark -
#pragma mark * Source_SQLite

Source_SQLite::Source_SQLite(ZRef<ZSQLite::DB> iDB)
:	fDB(iDB)
	{
	fChangeCount = ::sqlite3_total_changes(fDB->GetDB());

	for (ZRef<Iter> iterTables = new Iter(fDB, "select name from sqlite_master;");
		iterTables->HasValue(); iterTables->Advance())
		{
		const string8 theTableName = iterTables->Get(0).Get_T<string>();
		ZAssert(!theTableName.empty());
		RelHead theRelHead;
		for (ZRef<Iter> iterTable = new Iter(fDB, "pragma table_info(" + theTableName + ");");
			iterTable->HasValue(); iterTable->Advance())
			{
			theRelHead.Insert(theTableName + "_" + iterTable->Get(1).Get_T<string>());
			}

		if (!theRelHead.empty())
			ZUtil_STL::sInsertMustNotContain(kDebug, fMap_NameToRelHead, theTableName, theRelHead);
		}
	}

Source_SQLite::~Source_SQLite()
	{}

set<RelHead> Source_SQLite::GetRelHeads()
	{
	set<RelHead> result;
	for (map<string8, RelHead>::const_iterator i = fMap_NameToRelHead.begin();
		i != fMap_NameToRelHead.end(); ++i)
		{
		result.insert(i->second);
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
		if (ZLOGF(s, eDebug))
			s << "\n" << iAdded->fSearchThing;
		
		const string8 theSQL = this->pAsSQL(iAdded->fSearchThing);
		PQuery* thePQuery = new PQuery(iAdded->fRefcon, theSQL);
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

string8 Source_SQLite::pAsSQL(const SearchThing& iSearchThing)
	{
	// Go through each NameMap. Use the from to identify which table it represents. Generate
	// a distinct name for the table, for use in an AS clause. Rewrite the from to reference
	// the distinct name.

	// We can then generate an SFW formed from the reworked NameMaps and the ValPredCompound, which
	// is written in terms of the Tos.

	map<string8, int> tableSuffixes;
	vector<NameMap> revisedNameMaps;
	string8 theFields;
	for (vector<NameMap>::const_iterator i = iSearchThing.fNameMaps.begin();
		i != iSearchThing.fNameMaps.end(); ++i)
		{
		const NameMap& sourceNameMap = *i;
		const RelHead& theRH_From = sourceNameMap.GetRelHead_From();
		if (ZLOGF(s, eDebug))
			s << theRH_From;
		for (map<string8, RelHead>::const_iterator i = fMap_NameToRelHead.begin();
			i != fMap_NameToRelHead.end(); ++i)
			{
			if (ZUtil_STL_set::sIncludes(i->second, theRH_From))
				{
				// Found a table.
				const RelName theTableName = i->first;
				const int theSuffix = tableSuffixes[theTableName]++;
				const RelName newPrefix = theTableName + ZStringf("%d", theSuffix) + ".";
				const RelName oldPrefix = theTableName + "_";

				NameMap newNameMap;
				for (set<NameMap::Elem_t>::const_iterator i = sourceNameMap.GetElems().begin();
					i != sourceNameMap.GetElems().end(); ++i)
					{
					const RelName oldFrom = i->second;
					RelName newFrom = ZRA::sPrefixRemove(theTableName + "_", oldFrom);
					newFrom = ZRA::sPrefixAdd(newPrefix, newFrom);
					newNameMap.InsertToFrom(i->first, newFrom);
					if (! theFields.empty())
						theFields += ", ";
					theFields += newFrom + " AS '" + i->first + "'";
					}
				revisedNameMaps.push_back(newNameMap);
				if (ZLOGF(s, eDebug))
					s << newNameMap;
				}
			}
		}

	string8 theTables;
	for (map<string8, int>::const_iterator i = tableSuffixes.begin();
		i != tableSuffixes.end(); ++i)
		{
		for (int x = 0; x < i->second; ++x)
			{
			if (! theTables.empty())
				theTables += ", ";
			theTables += i->first + " AS '" + i->first + ZStringf("%d", x) + "'";
			}
		}

	string8 result = "SELECT " + theFields + " FROM " + theTables + " WHERE ";
	
	result += ZRA::SQL::sAsSQL(sAsExpr_Logic(iSearchThing.fPredCompound));
	
	if (ZLOGF(s, eDebug))
		s << result;
	return result;
	}

} // namespace ZDataspace
} // namespace ZooLib
