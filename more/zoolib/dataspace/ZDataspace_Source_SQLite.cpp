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
#include "zoolib/dataspace/ZDataspace_Util_Strim.h"

#include "zoolib/zra/ZRA_SQL.h"
#include "zoolib/zra/ZRA_Util_Strim_RelHead.h"

#include "zoolib/ZLog.h"

#include "zoolib/ZCallable_PMF.h"
#include "zoolib/ZWorker_Callable.h"
#include "zoolib/ZWorkerRunner_CFRunLoop.h"

namespace ZooLib {
namespace ZDataspace {

using std::map;
using std::set;
using std::vector;

using namespace ZSQLite;

using ZRA::NameMap;
using ZRA::RelName;

const ZStrimW& operator<<(const ZStrimW& w, const Rename_t& iRename)
	{
	w << "(";
	bool isSubsequent = false;
	for (Rename_t::const_iterator i = iRename.begin(); i != iRename.end(); ++i)
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
#pragma mark * Source_SQLite

Source_SQLite::Source_SQLite(ZRef<ZSQLite::DB> iDB)
:	fDB(iDB)
,	fStamp(Stamp::sSeed())
	{
	fChangeCount = ::sqlite3_total_changes(fDB->GetDB());

	for (ZRef<Iter> iterTables = new Iter(fDB, "select name from sqlite_master;");
		iterTables->HasValue(); iterTables->Advance())
		{
		const string8 theTableName = iterTables->Get(0).Get_T<string8>();
		ZAssert(!theTableName.empty());
		RelHead theRelHead;
		for (ZRef<Iter> iterTable = new Iter(fDB, "pragma table_info(" + theTableName + ");");
			iterTable->HasValue(); iterTable->Advance())
			{
			theRelHead.Insert(theTableName + "_" + iterTable->Get(1).Get_T<string8>());
			}

		if (!theRelHead.empty())
			ZUtil_STL::sInsertMustNotContain(kDebug, fMap_NameToRelHead, theTableName, theRelHead);
		}

	ZRef<ZWorker> theWorker = MakeWorker(MakeCallable(this, &Source_SQLite::pCheck));
	ZWorkerRunner_CFRunLoop::sMain()->Add(theWorker);
	theWorker->Wake();
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
	const AddedSearch* iAdded, size_t iAddedCount,
	const int64* iRemoved, size_t iRemovedCount,
	vector<SearchResult>& oChanged,
	ZRef<Event>& oEvent)
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
			s << "\n" << iAdded->fSearchSpec;
		
		const string8 theSQL = this->pAsSQL(iAdded->fSearchSpec);
		PQuery* thePQuery = new PQuery(iAdded->fRefcon, theSQL);
		ZUtil_STL::sInsertMustNotContain(kDebug,
			fMap_RefconToPQuery, thePQuery->fRefcon, thePQuery);
		++iAdded;
		}

	// For now, just regenerate everything every time. Later we'll only generate
	// newly added queries, or everything if the sqlite change count increments.
	for (map<int64, PQuery*>::iterator i = fMap_RefconToPQuery.begin();
		i != fMap_RefconToPQuery.end(); ++i)
		{
		PQuery* thePQuery = i->second;

		ZRef<Iter> theIter = new Iter(fDB, thePQuery->fSQL);
		vector<string8> theRowHead;
		const size_t theCount = theIter->Count();
		for (size_t x = 0; x < theCount; ++x)
			theRowHead.push_back(theIter->NameOf(x));

		vector<ZRef<ZQE::Row> > theRows;
		for (/*no init*/;theIter->HasValue(); theIter->Advance())
			{
			vector<ZVal_Any> theVals;
			theVals.reserve(theCount);
			for (size_t x = 0; x < theCount; ++x)
				theVals.push_back(theIter->Get(x));
			ZRef<ZQE::Row> theRow = new ZQE::Row_Vector(&theVals);
			theRows.push_back(theRow);
			}

		ZRef<ZQE::RowVector> theRowVector = new ZQE::RowVector(&theRows);
		ZRef<SearchRows> theSearchRows = new SearchRows(&theRowHead, theRowVector);

		SearchResult theSearchResult;
		theSearchResult.fRefcon = thePQuery->fRefcon;
		theSearchResult.fSearchRows = theSearchRows;

		oChanged.push_back(theSearchResult);
		}

	sEvent(fStamp);

	oEvent = fStamp->GetEvent();
	}

bool Source_SQLite::pCheck(ZRef<ZWorker> iWorker)
	{
	this->pInvokeCallables();
	iWorker->WakeIn(10);
	return true;
	}

string8 Source_SQLite::pAsSQL(const SearchSpec& iSearchSpec)
	{
	// Go through each NameMap. Use the from to identify which table it represents. Generate
	// a distinct name for the table, for use in an AS clause. Rewrite the from to reference
	// the distinct name.

	// We can then generate an SFW formed from the reworked NameMaps and the ValPredCompound, which
	// is written in terms of the Tos.

	// We should rewrite the ValPredCompound and the AS clauses using unqiue
	// names ("zoolib_unique_1",...) so we don't get bitten by SQL's case-insentivity
	// and clashes with column names.
	// We'll live with it for now.

	map<string8, int> theTableSuffixes;
	map<string8, string8> theRename;
	string8 theFields;
	for (vector<NameMap>::const_iterator i = iSearchSpec.fNameMaps.begin();
		i != iSearchSpec.fNameMaps.end(); ++i)
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
				const int theSuffix = theTableSuffixes[theTableName]++;
				const RelName newPrefix = theTableName + ZStringf("%d", theSuffix) + ".";
				const RelName oldPrefix = theTableName + "_";

				for (set<NameMap::Elem_t>::const_iterator i = sourceNameMap.GetElems().begin();
					i != sourceNameMap.GetElems().end(); ++i)
					{
					const RelName oldFrom = i->second;
					RelName newFrom = ZRA::sPrefixRemove(theTableName + "_", oldFrom);
					newFrom = ZRA::sPrefixAdd(newPrefix, newFrom);
					if (! theFields.empty())
						theFields += ", ";
					theFields += newFrom + " AS '" + i->first + "'";
					ZUtil_STL::sInsertMustNotContain(1, theRename, i->first, newFrom);
					}
				}
			}
		}

	string8 theTables;
	for (map<string8, int>::const_iterator i = theTableSuffixes.begin();
		i != theTableSuffixes.end(); ++i)
		{
		for (int x = 0; x < i->second; ++x)
			{
			if (! theTables.empty())
				theTables += ", ";
			theTables += i->first + " AS '" + i->first + ZStringf("%d", x) + "'";
			}
		}

	string8 result = "SELECT " + theFields + " FROM " + theTables + " WHERE ";
	
	if (ZLOGF(s, eDebug))
		s << theRename;

	result += ZRA::SQL::sAsSQL(sAsExpr_Logic(iSearchSpec.fPredCompound.Renamed(theRename)));
	
	if (ZLOGF(s, eDebug))
		s << result;

	return result;
	}

} // namespace ZDataspace
} // namespace ZooLib
