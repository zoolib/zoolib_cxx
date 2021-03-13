// Copyright (c) 2010 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Dataspace_Relater_Union_h__
#define __ZooLib_Dataspace_Relater_Union_h__ 1
#include "zconfig.h"

#include "zoolib/DList.h"

#include "zoolib/Dataspace/Relater.h"
#include "zoolib/QueryEngine/Walker.h"

namespace ZooLib {
namespace Dataspace {

// =================================================================================================
#pragma mark - Relater_Union

class Relater_Union
:	public Relater
	{
public:
	enum { kDebug = 0 };

	Relater_Union();
	virtual ~Relater_Union();

// From Counted via Relater
	virtual void Initialize();

// From Relater
	virtual bool Intersects(const RelHead& iRelHead);

	virtual void ModifyRegistrations(
		const AddedQuery* iAdded, size_t iAddedCount,
		const int64* iRemoved, size_t iRemovedCount);

	virtual void CollectResults(std::vector<QueryResult>& oChanged, int64& oChangeCount);

// Our protocol
	void InsertRelater(ZP<Relater> iRelater, const string8& iPrefix);
	void EraseRelater(ZP<Relater> iRelater);

private:
	ZMtx fMtx;

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

	typedef std::map<ZP<RelationalAlgebra::Expr_Rel>,Proxy*> Map_Rel_ProxyX;
	Map_Rel_ProxyX fMap_Rel_ProxyX;

	std::set<PRelater*> pIdentifyPRelaters(const RelHead& iRelHead);
	ZP<RelationalAlgebra::Expr_Rel> pGetProxy(PQuery* iPQuery,
		const std::set<PRelater*>& iPRelaters,
		const RelHead& iRelHead,
		ZP<RelationalAlgebra::Expr_Rel> iRel);
	void pFinalizeProxy(Proxy* iProxy);

	// -----

	class Walker_Proxy;

	ZP<Walker_Proxy> pMakeWalker(ZP<Proxy> iProxy);

	void pRewind(ZP<Walker_Proxy> iWalker);

	void pPrime(ZP<Walker_Proxy> iWalker,
		const std::map<string8,size_t>& iOffsets,
		std::map<string8,size_t>& oOffsets,
		size_t& ioBaseOffset);

	bool pReadInc(ZP<Walker_Proxy> iWalker, Val_DB* ioResults);

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

	typedef std::map<
			ZP<RelationalAlgebra::Expr_Rel>,
			PQuery,
			RelationalAlgebra::Less_Rel>
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

	typedef std::map<ZP<Relater>, PRelater> Map_Relater_PRelater;
	Map_Relater_PRelater fMap_Relater_PRelater;

	// -----

	ZP<Relater::Callable_RelaterResultsAvailable> fCallable_RelaterResultsAvailable;

	void pCollectFrom(PRelater* iPRelater);
	void pResultsAvailable(ZP<Relater> iRelater);
	};

} // namespace Dataspace
} // namespace ZooLib

#endif // __ZooLib_Dataspace_Relater_Union_h__
