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
#include "zoolib/dataspace/ZDataspace_Source_Any.h"
#include "zoolib/dataspace/ZDataspace_Util_Strim.h"
#include "zoolib/zqe/ZQE_Result_Any.h"

#include "zoolib/zra/ZRA_SQL.h"
#include "zoolib/zra/ZRA_Util_Strim_RelHead.h"

#include "zoolib/ZLog.h"

namespace ZooLib {
namespace ZDataspace {

using ZRA::RelName;

// =================================================================================================
#pragma mark -
#pragma mark * Source_Any::PQuery

class Source_Any::PQuery
	{
public:
	PQuery(int64 iRefcon, const SearchThing& iSearchThing);

	const int64 fRefcon;
	const SearchThing fSearchThing;
	};

Source_Any::PQuery::PQuery(int64 iRefcon, const SearchThing& iSearchThing)
:	fRefcon(iRefcon)
,	fSearchThing(iSearchThing)
	{}

// =================================================================================================
#pragma mark -
#pragma mark * Source_Any

Source_Any::Source_Any()
	{}

Source_Any::~Source_Any()
	{}

set<RelHead> Source_Any::GetRelHeads()
	{ return set<RelHead>(); }

void Source_Any::Update(
	bool iLocalOnly,
	AddedSearch* iAdded, size_t iAddedCount,
	int64* iRemoved, size_t iRemovedCount,
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
			s << "\n" << iAdded->fSearchThing;
		
		PQuery* thePQuery = new PQuery(iAdded->fRefcon, iAdded->fSearchThing);
		ZUtil_STL::sInsertMustNotContain(kDebug,
			fMap_RefconToPQuery, thePQuery->fRefcon, thePQuery);
		++iAdded;
		}

	// For now, just regenerate everything every time. Later we'll only generate
	// newly added queries, or everything if the Val change count increments.
	for (map<int64, PQuery*>::iterator i = fMap_RefconToPQuery.begin();
		i != fMap_RefconToPQuery.end(); ++i)
		{
		PQuery* thePQuery = i->second;

		SearchResult theSearchResult;
		theSearchResult.fRefcon = thePQuery->fRefcon;

#if 0
		for (ZRef<Iter> theIter = new Iter(fDB, thePQuery->fSQL);
			theIter->HasValue(); theIter->Advance())
			{
			ZMap_Any theMap;
			for (size_t x = 0; x < theIter->Count(); ++x)
				theMap.Set(theIter->NameOf(x), theIter->Get(x));

			theSearchResult.fResults.push_back(new ZQE::Result_Any(theMap));
			}
#endif
		oChanged.push_back(theSearchResult);
		}

	sEvent(fStamp);

	oEvent = fStamp->GetEvent();
	}

} // namespace ZDataspace
} // namespace ZooLib
