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

#ifndef __ZooLib_Dataspace_Relater_Union_h__
#define __ZooLib_Dataspace_Relater_Union_h__ 1
#include "zconfig.h"

#include "zoolib/DList.h"

#include "zoolib/dataspace/Relater.h"
#include "zoolib/QueryEngine/Walker.h"

namespace ZooLib {
namespace Dataspace {

// =================================================================================================
// MARK: - Relater_Union

class Relater_Union
:	public Relater
	{
public:
	enum { kDebug = 0 };

	Relater_Union();
	virtual ~Relater_Union();

// From ZCounted via Relater
	virtual void Initialize();

// From Relater
	virtual bool Intersects(const RelHead& iRelHead);

	virtual void ModifyRegistrations(
		const AddedQuery* iAdded, size_t iAddedCount,
		const int64* iRemoved, size_t iRemovedCount);

	virtual void CollectResults(std::vector<QueryResult>& oChanged);

// Our protocol
	void InsertRelater(ZRef<Relater> iRelater, const string8& iPrefix);
	void EraseRelater(ZRef<Relater> iRelater);

private:
	ZMtxR fMtxR;

	class PQuery;

	// -----

	class PIP;
	class DLink_PIP_InProxy;
	class DLink_PIP_NeedsWork;
	typedef std::map<int64, PIP> Map_Refcon_PIP;

	// -----
	class PRelater;

	class Proxy;
	class Visitor_Proxy;

	typedef std::map<ZRef<RelationalAlgebra::Expr_Rel>,Proxy*> Map_Rel_ProxyX;
	Map_Rel_ProxyX fMap_Rel_ProxyX;

	std::set<PRelater*> pIdentifyPRelaters(const RelHead& iRelHead);
	ZRef<RelationalAlgebra::Expr_Rel> pGetProxy(PQuery* iPQuery,
		const std::set<PRelater*>& iPRelaters,
		const RelHead& iRelHead,
		ZRef<RelationalAlgebra::Expr_Rel> iRel);
	void pFinalizeProxy(Proxy* iProxy);

	// -----

	class Walker_Proxy;

	ZRef<Walker_Proxy> pMakeWalker(ZRef<Proxy> iProxy);

	void pRewind(ZRef<Walker_Proxy> iWalker);

	void pPrime(ZRef<Walker_Proxy> iWalker,
		const std::map<string8,size_t>& iOffsets,
		std::map<string8,size_t>& oOffsets,
		size_t& ioBaseOffset);

	bool pReadInc(ZRef<Walker_Proxy> iWalker, ZVal_Any* ioResults);

	size_t fWalkerCount;
	size_t fReadCount;
	size_t fStepCount;

	// -----

	class DLink_ClientQuery_NeedsWork;
	class DLink_ClientQuery_InPQuery;
	class ClientQuery;

	DListHead<DLink_ClientQuery_NeedsWork> fClientQuery_NeedsWork;

	typedef std::map<int64, ClientQuery> Map_Refcon_ClientQuery;
	Map_Refcon_ClientQuery fMap_Refcon_ClientQuery;

	// -----

	class DLink_PQuery_NeedsWork;
	DListHead<DLink_PQuery_NeedsWork> fPQuery_NeedsWork;

	typedef std::map<ZRef<RelationalAlgebra::Expr_Rel>, PQuery,
		Less_Compare_T<ZRef<RelationalAlgebra::Expr_Rel> > >
		Map_Rel_PQuery;
	Map_Rel_PQuery fMap_Rel_PQuery;

	// -----

	class Visitor_DoMakeWalker;

	// -----

	class Analyze;
	friend class Analyze;

	// -----

	class DLink_PRelater_NeedsWork;
	class DLink_PRelater_CollectFrom;
	DListHead<DLink_PRelater_CollectFrom> fPRelater_CollectFrom;
	DListHead<DLink_PRelater_NeedsWork> fPRelater_NeedsWork;

	typedef std::map<ZRef<Relater>, PRelater> Map_Relater_PRelater;
	Map_Relater_PRelater fMap_Relater_PRelater;

	// -----

	ZRef<Event> fEvent;
	ZRef<Relater::Callable_RelaterResultsAvailable> fCallable_RelaterResultsAvailable;

	void pCollectFrom(PRelater* iPRelater);
	void pResultsAvailable(ZRef<Relater> iRelater);
	};

} // namespace Dataspace
} // namespace ZooLib

#endif // __ZooLib_Dataspace_Relater_Union_h__
