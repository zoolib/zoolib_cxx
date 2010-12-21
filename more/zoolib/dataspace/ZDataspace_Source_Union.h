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
	virtual std::set<RelHead> GetRelHeads();

	virtual void ModifyRegistrations(
		const AddedSearch* iAdded, size_t iAddedCount,
		const int64* iRemoved, size_t iRemovedCount);

	virtual void CollectResults(std::vector<SearchResult>& oChanged);

// Our protocol
	void InsertSource(ZRef<Source> iSource, const string8& iPrefix);
	void EraseSource(ZRef<Source> iSource);

private:
	class PQuery;
	class DLink_PQuery_Changed;

	int64 fNextRefcon;
	std::map<int64, PQuery*> fMap_RefconToPQuery;
	DListHead<DLink_PQuery_Changed> fPQuery_Changed;

	class PSourceProduct;
	class DLink_PSourceProduct_ToAdd;
	class DLink_PSourceProduct_InPQuery;

	class PSourceSearches;
	class DLink_PSourceSearches_ToAdd;
	class DLink_PSourceSearches_InPQuery;

	class PSource;
	class DLink_PSource_ToUpdate;
	std::set<PSource*> fPSources_ToAdd;
	std::set<PSource*> fPSources_ToRemove;
	DListHead<DLink_PSource_ToUpdate> fPSource_ToUpdate;

	std::map<ZRef<Source>, PSource*> fMap_SourceToPSource;

	ZRef<Event> fEvent;
	ZRef<Source::Callable_ResultsAvailable> fCallable;

	void pCallback(ZRef<Source> iSource);

	void pUpdate(
		const AddedSearch* iAdded, size_t iAddedCount,
		const int64* iRemoved, size_t iRemovedCount,
		std::vector<SearchResult>& oChanged,
		ZRef<Event>& oEvent);

	void pDetachPQuery(PQuery* iPQuery);
	};

} // namespace ZDataspace
} // namespace ZooLib

#endif // __ZDataspace_Source_Union__
