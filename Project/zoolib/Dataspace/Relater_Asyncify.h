// Copyright (c) 2011 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Dataspace_Relater_Asyncify_h__
#define __ZooLib_Dataspace_Relater_Asyncify_h__ 1
#include "zconfig.h"

#include "zoolib/Dataspace/Relater.h"

#include <map>
#include <set>
#include <vector>

namespace ZooLib {
namespace Dataspace {

// =================================================================================================
#pragma mark - Relater_Asyncify

class Relater_Asyncify : public Relater
	{
public:
	Relater_Asyncify(ZP<Relater> iRelater);
	virtual ~Relater_Asyncify();

// From Counted via Relater
	virtual void Initialize();
	virtual void Finalize();

// From Relater
	virtual bool Intersects(const RelHead& iRelHead);

	virtual void ModifyRegistrations(
		const AddedQuery* iAdded, size_t iAddedCount,
		const int64* iRemoved, size_t iRemovedCount);

	virtual void CollectResults(std::vector<QueryResult>& oChanged, int64& oChangeCount);

// Our protocol
	void CrankIt();
	void Shutdown();

private:
	void pTrigger_Update();
	void pUpdate();

	void pResultsAvailable(ZP<Relater> iRelater);

	ZMtx fMtx;
	ZCnd fCnd;
	ZP<Relater> fRelater;

	bool fTriggered_Update;
	bool fNeeds_RelaterCollectResults;

	std::map<int64,ZP<RelationalAlgebra::Expr_Rel>> fPendingAdds;
	std::set<int64> fPendingRemoves;
	std::map<int64,QueryResult> fPendingResults;
	};

} // namespace Dataspace
} // namespace ZooLib

#endif // __ZooLib_Dataspace_Relater_Asyncify_h__
