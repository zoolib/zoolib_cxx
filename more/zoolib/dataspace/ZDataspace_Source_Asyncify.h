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

#ifndef __ZDataspace_Source_Asyncify__
#define __ZDataspace_Source_Asyncify__ 1
#include "zconfig.h"

#include "zoolib/ZStreamer.h"
#include "zoolib/dataspace/ZDataspace_Source.h"

#include <map>
#include <set>
#include <vector>

namespace ZooLib {
namespace ZDataspace {

// =================================================================================================
#pragma mark -
#pragma mark * Source_Asyncify

class Source_Asyncify : public Source
	{
public:
	Source_Asyncify(ZRef<Source> iSource);
	virtual ~Source_Asyncify();

// From ZCounted via Source
	virtual void Initialize();
	virtual void Finalize();

// From Source
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

	void pResultsAvailable(ZRef<Source> iSource);

	ZMtxR fMtxR;
	ZCnd fCnd;
	ZRef<Source> fSource;

	bool fTriggered_Update;
	bool fNeeds_SourceCollectResults;

	std::map<int64,ZRef<ZRA::Expr_Rel> > fPendingAdds;
	std::set<int64> fPendingRemoves;
	std::map<int64,QueryResult> fPendingResults;
	};

} // namespace ZDataspace
} // namespace ZooLib

#endif // __ZDataspace_Source_Asyncify__
