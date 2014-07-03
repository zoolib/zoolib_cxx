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

#ifndef __ZooLib_Dataspace_Relater_Searcher_h__
#define __ZooLib_Dataspace_Relater_Searcher_h__ 1
#include "zconfig.h"

#include "zoolib/dataspace/Relater.h"
#include "zoolib/dataspace/Searcher.h"

#include "zoolib/QueryEngine/Walker.h"

namespace ZooLib {
namespace Dataspace {

// =================================================================================================
// MARK: - Relater_Searcher

class Relater_Searcher
:	public Relater
	{
public:
	enum { kDebug = 1 };

	Relater_Searcher(ZRef<Searcher> iSearcher);
	virtual ~Relater_Searcher();

// From ZCounted via Relater
	virtual void Initialize();

// From Source
	virtual bool Intersects(const RelHead& iRelHead);

	virtual void ModifyRegistrations(
		const AddedQuery* iAdded, size_t iAddedCount,
		const int64* iRemoved, size_t iRemovedCount);

	virtual void CollectResults(std::vector<QueryResult>& oChanged);

// Our protocol
	void ForceUpdate();

protected:
	bool pCollectResultsFromSearcher();
	void pSearcherResultsAvailable(ZRef<Searcher>);

	ZMtxR fMtxR;
	ZCnd fCnd;

	ZRef<Searcher> fSearcher;

	class PQuery;

	// -----

	class Visitor_DoMakeWalker;
	friend class Visitor_DoMakeWalker;

	ZRef<QueryEngine::Walker> pMakeWalker_Concrete(PQuery* iPQuery,
		const ConcreteHead& iConcreteHead);

	// -----

	class DLink_ClientQuery_InPQuery;
	class DLink_ClientQuery_NeedsWork;
	class ClientQuery;
	std::map<int64, ClientQuery> fMap_Refcon_ClientQuery;
	DListHead<DLink_ClientQuery_NeedsWork> fClientQuery_NeedsWork;

	// -----

	class DLink_PQuery_NeedsWork;
	typedef std::map<
			ZRef<RelationalAlgebra::Expr_Rel>,
			PQuery,
			Less_Compare_T<ZRef<RelationalAlgebra::Expr_Rel> > >
		Map_Rel_PQuery;

	Map_Rel_PQuery fMap_Rel_PQuery;

	DListHead<DLink_PQuery_NeedsWork> fPQuery_NeedsWork;

	// -----

	class DLink_PRegSearch_NeedsWork;
	class PRegSearch;

	int64 fNextRefcon;
	typedef std::map<int64,PRegSearch> Map_Refcon_PRegSearch;
	Map_Refcon_PRegSearch fMap_Refcon_PRegSearch;

	typedef std::map<ConcreteHead,PRegSearch*> Map_ConcreteHead_PRegSearch;
	Map_ConcreteHead_PRegSearch fMap_ConcreteHead_PRegSearch;

	DListHead<DLink_PRegSearch_NeedsWork> fPRegSearch_NeedsWork;
	};

} // namespace Dataspace
} // namespace ZooLib

#endif // __ZooLib_Dataspace_Relater_Searcher_h__
