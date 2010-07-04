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

#ifndef __ZDataspace_Source_Union__
#define __ZDataspace_Source_Union__ 1
#include "zconfig.h"

#include "zoolib/ZDList.h"

#include "zoolib/dataspace/ZDataspace_Source.h"

namespace ZooLib {
namespace ZDataspace {

// =================================================================================================
#pragma mark -
#pragma mark * Source_Union

class Source_Union : public Source
	{
public:
	enum { kDebug = 1 };

	Source_Union();
	virtual ~Source_Union();

// From Source
	virtual set<RelHead> GetRelHeads();

	virtual void Update(
		bool iLocalOnly,
		AddedSearch* iAdded, size_t iAddedCount,
		int64* iRemoved, size_t iRemovedCount,
		vector<SearchResult>& oChanged,
		Clock& oClock);

// Our protocol
	void InsertSource(Source* iSource, const string8& iPrefix);
	void EraseSource(Source* iSource);

private:
	class PQuery;
	class DLink_PQuery_Changed;

	int64 fNextRefcon;
	map<int64, PQuery*> fMap_RefconToPQuery;
	DListHead<DLink_PQuery_Changed> fPQuery_Changed;


	class PSourceProduct;
	class DLink_PSourceProduct_ToAdd;
	class DLink_PSourceProduct_InPQuery;

	class PSourceSearches;
	class DLink_PSourceSearches_ToAdd;
	class DLink_PSourceSearches_InPQuery;

	class PSource;
	set<PSource*> fPSources_ToAdd;
	set<PSource*> fPSources_ToRemove;

	map<Source*, PSource*> fMap_SourceToPSource;

	Clock fClock;

	void pUpdate(
		AddedSearch* iAdded, size_t iAddedCount,
		int64* iRemoved, size_t iRemovedCount,
		vector<SearchResult>& oChanged,
		Clock& oClock);

	void pDetachPQuery(PQuery* iPQuery);

	class Iterator_PSourceProduct;
	friend class Iterator_PSourceProduct;

	class Iterator_PSourceSearches;
	friend class Iterator_PSourceSearches;
	};

} // namespace ZDataspace
} // namespace ZooLib

#endif // __ZDataspace_Source_Union__
