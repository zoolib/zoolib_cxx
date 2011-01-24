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
#pragma mark * Source_SQLite::ClientSearch

class Source_SQLite::DLink_ClientSearch_InPSearch
:	public DListLink<ClientSearch, DLink_ClientSearch_InPSearch, kDebug>
	{};

class Source_SQLite::ClientSearch
:	public Source_SQLite::DLink_ClientSearch_InPSearch
	{
public:
	ClientSearch(int64 iRefcon, PSearch* iPSearch)
	:	fRefcon(iRefcon),
		fPSearch(iPSearch)
		{}

	int64 fRefcon;
	PSearch* fPSearch;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Source_SQLite::PSearch

class Source_SQLite::PSearch
	{
public:
	PSearch(ZRef<ZRA::Expr_Rel> iRel)
	:	fRel(iRel)
		{}

	ZRef<ZRA::Expr_Rel> fRel;
	RelHead fRelHead;
	string8 fSQL;
	DListHead<DLink_ClientSearch_InPSearch> fClientSearches;
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
		if (!(ZRA::sPrefixInsert(iterTables->first + "_", iterTables->second) & iRelHead).empty())
			return true;
		}
	return false;
	}

void Source_SQLite::ModifyRegistrations(
	const AddedSearch* iAdded, size_t iAddedCount,
	const int64* iRemoved, size_t iRemovedCount)
	{
	if (iAddedCount || iRemovedCount)
		{
//		fStackChanged = true;
		this->pInvokeCallable_ResultsAvailable();
		}

	while (iAddedCount--)
		{
		ZRef<ZRA::Expr_Rel> theRel = iAdded->GetRel();

		pair<Map_RelToPSearch::iterator,bool> inPSearch =
			fMap_RelToPSearch.insert(make_pair(theRel, PSearch(theRel)));

		PSearch* thePSearch = &inPSearch.first->second;
		
		if (inPSearch.second)
			{
			string8 asSQL;
			ZRA::sWriteAsSQL(fMap_Tables, theRel, ZStrimW_String(asSQL));
			if (ZLOGF(s, eDebug))
				s << asSQL;
			thePSearch->fSQL = asSQL;
			thePSearch->fRelHead = sGetRelHead(theRel);
			}

		const int64 theRefcon = iAdded->GetRefcon();

		std::map<int64, ClientSearch>::iterator iterClientSearch =
			fMap_RefconToClientSearch.insert(
			make_pair(theRefcon, ClientSearch(theRefcon, thePSearch))).first;

		thePSearch->fClientSearches.Insert(&iterClientSearch->second);

		++iAdded;
		}

	while (iRemovedCount--)
		{
		int64 theRefcon = *iRemoved++;

		std::map<int64, ClientSearch>::iterator iterClientSearch =
			fMap_RefconToClientSearch.find(theRefcon);
		
		ClientSearch* theClientSearch = &iterClientSearch->second;
		
		PSearch* thePSearch = theClientSearch->fPSearch;
		thePSearch->fClientSearches.Erase(theClientSearch);
		if (thePSearch->fClientSearches.Empty())
			ZUtil_STL::sEraseMustContain(kDebug, fMap_RelToPSearch, thePSearch->fRel);
		
		fMap_RefconToClientSearch.erase(iterClientSearch);
		}
	}

void Source_SQLite::CollectResults(std::vector<SearchResult>& oChanged)
	{
	oChanged.clear();

	for (Map_RelToPSearch::iterator iterPSearch = fMap_RelToPSearch.begin();
		iterPSearch != fMap_RelToPSearch.end(); ++iterPSearch)
		{
		PSearch* thePSearch = &iterPSearch->second;
		vector<ZVal_Any> thePackedRows;
		for (ZRef<Iter> theIter = new Iter(fDB, thePSearch->fSQL);
			theIter->HasValue(); theIter->Advance())
			{
			const size_t theCount = theIter->Count();
			for (size_t x = 0; x < theCount; ++x)
				thePackedRows.push_back(theIter->Get(x));
			}

		ZRef<ZQE::Result> theResult =
			new ZQE::Result(thePSearch->fRelHead, &thePackedRows, nullptr);
		
		for (DListIterator<ClientSearch, DLink_ClientSearch_InPSearch>
			iterCS = thePSearch->fClientSearches; iterCS; iterCS.Advance())
			{
			oChanged.push_back(SearchResult(iterCS.Current()->fRefcon, theResult, null));
			}
		}
	}

} // namespace ZDataspace
} // namespace ZooLib
