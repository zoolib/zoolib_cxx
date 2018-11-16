/* -------------------------------------------------------------------------------------------------
Copyright (c) 2011 Andrew Green
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
	Relater_Asyncify(ZRef<Relater> iRelater);
	virtual ~Relater_Asyncify();

// From ZCounted via Relater
	virtual void Initialize();
	virtual void Finalize();

// From Relater
	virtual bool Intersects(const RelHead& iRelHead);

	virtual void ModifyRegistrations(
		const AddedQuery* iAdded, size_t iAddedCount,
		const int64* iRemoved, size_t iRemovedCount);

	virtual void CollectResults(std::vector<QueryResult>& oChanged);

// Our protocol
	void CrankIt();
	void Shutdown();

private:
	void pTrigger_Update();
	void pUpdate();

	void pResultsAvailable(ZRef<Relater> iRelater);

	ZMtx fMtx;
	ZCnd fCnd;
	ZRef<Relater> fRelater;

	bool fTriggered_Update;
	bool fNeeds_RelaterCollectResults;

	std::map<int64,ZRef<RelationalAlgebra::Expr_Rel> > fPendingAdds;
	std::set<int64> fPendingRemoves;
	std::map<int64,QueryResult> fPendingResults;
	};

} // namespace Dataspace
} // namespace ZooLib

#endif // __ZooLib_Dataspace_Relater_Asyncify_h__
