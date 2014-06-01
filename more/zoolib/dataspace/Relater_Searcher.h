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

#include "zoolib/dataspace/ZDataspace_Source.h"
#include "zoolib/dataspace/Searcher.h"

#include "zoolib/QueryEngine/Walker.h"

namespace ZooLib {
namespace ZDataspace {

// =================================================================================================
// MARK: - Relater_Searcher

class Relater_Searcher
:	public Source
	{
public:
	enum { kDebug = 1 };

	Relater_Searcher(ZRef<Searcher> iSearcher);
	virtual ~Relater_Searcher();

	void ForceUpdate();

// From Source
	virtual bool Intersects(const RelHead& iRelHead);

	virtual void ModifyRegistrations(
		const AddedQuery* iAdded, size_t iAddedCount,
		const int64* iRemoved, size_t iRemovedCount);

	virtual void CollectResults(std::vector<QueryResult>& oChanged);

protected:
	void pChanged(const ZVal_Any& iVal);
	void pChangedAll();

	ZMtxR fMtxR;

	ZRef<Searcher> fSearcher;

	class PQuery;

	// -----

	class Walker_Searcher;
	friend class Walker_Searcher;

	ZRef<QueryEngine::Walker> pMakeWalker_Searcher(PQuery* iPQuery,
		const ConcreteHead& iConcreteHead);

	void pRewind_Searcher(ZRef<Walker_Searcher> iWalker);

	void pPrime_Searcher(ZRef<Walker_Searcher> iWalker,
		const std::map<string8,size_t>& iOffsets,
		std::map<string8,size_t>& oOffsets,
		size_t& ioBaseOffset);

	bool pReadInc_Searcher(ZRef<Walker_Searcher> iWalker, ZVal_Any* ioResults);

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

	class DLink_PSearch_NeedsWork;
	class PSearch;

	typedef std::map<RelHead,PSearch> Map_PSearch;
	Map_PSearch fMap_PSearch;

	DListHead<DLink_PSearch_NeedsWork> fPSearch_NeedsWork;
	};

} // namespace ZDataspace
} // namespace ZooLib

#endif // __ZooLib_Dataspace_Relater_Searcher_h__
