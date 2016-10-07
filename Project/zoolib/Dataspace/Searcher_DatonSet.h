/* -------------------------------------------------------------------------------------------------
Copyright (c) 2014 Andrew Green
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

#ifndef __ZooLib_Dataspace_Searcher_DatonSet_h__
#define __ZooLib_Dataspace_Searcher_DatonSet_h__ 1
#include "zconfig.h"

#include "zooLib/Dataspace/Searcher_Datons.h"

#include "zoolib/DatonSet/DatonSet.h"

#include <set>

namespace ZooLib {
namespace Dataspace {

typedef std::vector<ColName> IndexSpec;

// =================================================================================================
#pragma mark -
#pragma mark Searcher_DatonSet

class Searcher_DatonSet
:	public Searcher
	{
public:
	enum { kDebug = 1 };

	Searcher_DatonSet(const std::vector<IndexSpec>& iIndexSpecs);
	virtual ~Searcher_DatonSet();

// From ZCounted
	virtual void Initialize();

// From Searcher
	virtual bool Intersects(const RelHead& iRelHead);

	virtual void ModifyRegistrations(
		const AddedSearch* iAdded, size_t iAddedCount,
		const int64* iRemoved, size_t iRemovedCount);

	virtual void CollectResults(std::vector<SearchResult>& oChanged);

// Our protocol
	ZRef<DatonSet::Callable_PullSuggested> GetCallable_PullSuggested();

private:
	ZMtxR fMtxR;

	void pSearcherResultsAvailable(ZRef<Searcher>);

	void pPullSuggested(const ZRef<DatonSet::Callable_PullFrom>& iCallable_PullFrom);
	ZRef<DatonSet::Callable_PullSuggested> fCallable_PullSuggested_Self;

	void pPull();

	std::set<ZRef<DatonSet::Callable_PullFrom> > fCallables_PullFrom;

	typedef DatonSet::Event Event;
	typedef std::map<DatonSet::Daton,ZRef<Event> > Map_Assert;

	// -----

	ZRef<Event> fEvent;
	ZRef<Searcher_Datons> fSearcher_Datons;

	Map_Assert fMap_Assert;

	typedef std::map<DatonSet::Daton,ZRef<Event> > Map_Retract;
	Map_Retract fMap_Retract;
	};

} // namespace Dataspace
} // namespace ZooLib

#endif // __ZooLib_Dataspace_Searcher_DatonSet_h__
