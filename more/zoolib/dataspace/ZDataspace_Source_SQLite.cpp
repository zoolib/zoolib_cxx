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
#include "zoolib/ZUtil_STL.h"
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
#pragma mark * Source_SQLite::PSearch

class Source_SQLite::PSearch
	{
public:
	PSearch(int64 iRefcon,
		const ZRef<ZRA::Expr_Rel>& iRel, const RelHead& iRelHead, const string8& iSQL);

	const int64 fRefcon;
	const ZRef<ZRA::Expr_Rel> fRel;
	const RelHead fRelHead;
	const string8 fSQL;
	};

Source_SQLite::PSearch::PSearch(int64 iRefcon,
	const ZRef<ZRA::Expr_Rel>& iRel, const RelHead& iRelHead, const string8& iSQL)
:	fRefcon(iRefcon)
,	fRel(iRel)
,	fRelHead(iRelHead)
,	fSQL(iSQL)
	{}

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

RelHead Source_SQLite::GetRelHead()
	{
	RelHead result;
	for (map<string8, RelHead>::const_iterator iterTables = fMap_Tables.begin();
		iterTables != fMap_Tables.end(); ++iterTables)
		{
		const string8& tableName = iterTables->first;
		const RelHead& theRH = iterTables->second;
		for (RelHead::const_iterator iterRH = theRH.begin(); iterRH != theRH.end(); ++iterRH)
			result |= tableName + "_" + *iterRH;
		}
	return result;
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
		string8 asSQL;
		if (ZRA::sWriteAsSQL(fMap_Tables, theRel, ZStrimW_String(asSQL)))
			{
			if (ZLOGF(s, eDebug))
				s << asSQL;
			PSearch* thePSearch =
				new PSearch(iAdded->GetRefcon(), theRel, sGetRelHead(theRel), asSQL);
			ZUtil_STL::sInsertMustNotContain(kDebug,
				fMap_RefconToPSearch, thePSearch->fRefcon, thePSearch);
			}

		++iAdded;
		}

	while (iRemovedCount--)
		{
		if (ZQ<PSearch*> thePSearch =
			ZUtil_STL::sEraseAndReturnIfContains(fMap_RefconToPSearch, *iRemoved++))
			{
			delete thePSearch.Get();
			}
//		delete ZUtil_STL::sEraseAndReturn(kDebug, fMap_RefconToPSearch, *iRemoved++);
		}
	}

void Source_SQLite::CollectResults(std::vector<SearchResult>& oChanged)
	{
	oChanged.clear();

	for (map<int64, PSearch*>::iterator iter_RefconToPSearch = fMap_RefconToPSearch.begin();
		iter_RefconToPSearch != fMap_RefconToPSearch.end(); ++iter_RefconToPSearch)
		{
		vector<ZVal_Any> thePackedRows;
		PSearch* thePSearch = iter_RefconToPSearch->second;
		for (ZRef<Iter> theIter = new Iter(fDB, thePSearch->fSQL);
			theIter->HasValue(); theIter->Advance())
			{
			const size_t theCount = theIter->Count();
			for (size_t x = 0; x < theCount; ++x)
				thePackedRows.push_back(theIter->Get(x));
			}

		ZRef<ZQE::Result> theResult =
			new ZQE::Result(thePSearch->fRelHead, &thePackedRows, nullptr);
		SearchResult theSearchResult(thePSearch->fRefcon, theResult, null);
		oChanged.push_back(theSearchResult);
		}
	}

} // namespace ZDataspace
} // namespace ZooLib
