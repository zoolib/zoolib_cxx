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

#include "zoolib/Dataspace/Relater.h"
#include "zoolib/Dataspace/Searcher.h"

#include "zoolib/QueryEngine/Walker.h"

namespace ZooLib {
namespace Dataspace {

// =================================================================================================
#pragma mark - Relater_Searcher

class Relater_Searcher
:	public Relater
	{
public:
	enum { kDebug = 1 };

	Relater_Searcher(ZP<Searcher> iSearcher);
	virtual ~Relater_Searcher();

// From Counted via Relater
	virtual void Initialize();

// From Relater
	virtual bool Intersects(const RelHead& iRelHead);

	virtual void ModifyRegistrations(
		const AddedQuery* iAdded, size_t iAddedCount,
		const int64* iRemoved, size_t iRemovedCount);

	virtual void CollectResults(std::vector<QueryResult>& oChanged, int64& oChangeCount);

// Our protocol
	void ForceUpdate();

protected:
	bool pCollectResultsFromSearcher();
	void pSearcherResultsAvailable(ZP<Searcher>);

	ZMtx fMtx;
	ZCnd fCnd;

	ZP<Searcher> fSearcher;

	int64 fChangeCount;

	class PQuery;

	// -----

	class Walker_Bingo;
	friend class Walker_Bingo;

	void pFinalize(Walker_Bingo* iWalker_Bingo);

	void pRewind(ZP<Walker_Bingo> iWalker_Bingo);

	ZP<QueryEngine::Walker> pPrime(ZP<Walker_Bingo> iWalker_Bingo,
		const std::map<string8,size_t>& iOffsets,
		std::map<string8,size_t>& oOffsets,
		size_t& ioBaseOffset);

	bool pQReadInc(ZP<Walker_Bingo> iWalker, Val_Any* ioResults);

	// -----

	class Visitor_DoMakeWalker;
	friend class Visitor_DoMakeWalker;

	ZP<QueryEngine::Walker> pMakeWalker(PQuery* iPQuery,
		const RelHead& iRelHead_Bound,
		const SearchSpec& iSearchSpec);

	// -----

	class DLink_ClientQuery_InPQuery;
	class DLink_ClientQuery_NeedsWork;
	class ClientQuery;
	std::map<int64, ClientQuery> fMap_Refcon_ClientQuery;
	DListHead<DLink_ClientQuery_NeedsWork> fClientQuery_NeedsWork;

	// -----

	class DLink_PQuery_NeedsWork;
	typedef std::map<
			ZP<RelationalAlgebra::Expr_Rel>,
			PQuery,
			Less_Compare_T<ZP<RelationalAlgebra::Expr_Rel>>>
		Map_Rel_PQuery;

	Map_Rel_PQuery fMap_Rel_PQuery;

	DListHead<DLink_PQuery_NeedsWork> fPQuery_NeedsWork;

	// -----

	class DLink_PRegSearch_NeedsWork;
	class PRegSearch;

	int64 fNextRefcon;
	typedef std::map<int64,PRegSearch> Map_Refcon_PRegSearch;
	Map_Refcon_PRegSearch fMap_Refcon_PRegSearch;

	typedef std::map<SearchSpec,PRegSearch*> Map_SearchSpec_PRegSearchStar;
	Map_SearchSpec_PRegSearchStar fMap_SearchSpec_PRegSearchStar;

	DListHead<DLink_PRegSearch_NeedsWork> fPRegSearch_NeedsWork;
	};

// =================================================================================================
#pragma mark - sTransform_PushDownRestricts_IntoSearch

ZP<Expr_Rel> sTransform_PushDownRestricts_IntoSearch(const ZP<Expr_Rel>& iRel);

} // namespace Dataspace
} // namespace ZooLib

#endif // __ZooLib_Dataspace_Relater_Searcher_h__
