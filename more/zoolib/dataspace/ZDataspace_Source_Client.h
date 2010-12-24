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

#ifndef __ZDataspace_Source_Client__
#define __ZDataspace_Source_Client__ 1
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
#pragma mark * Source_Client

class Source_Client : public Source
	{
public:
	Source_Client(ZRef<ZStreamerR> iStreamerR, ZRef<ZStreamerW> iStreamerW);
	virtual ~Source_Client();

// From ZCounted via Source
	virtual void Initialize();

// From Source
	virtual RelHead GetRelHead();

	virtual void ModifyRegistrations(
		const AddedSearch* iAdded, size_t iAddedCount,
		const int64* iRemoved, size_t iRemovedCount);

	virtual void CollectResults(std::vector<SearchResult>& oChanged);

private:
	bool pRead(ZRef<ZWorker> iWorker);
	void pWrite();

	ZRef<ZStreamerR> fStreamerR;
	ZRef<ZStreamerW> fStreamerW;

	ZMtxR fMtxR;
	bool fNeedsWrite;
	std::map<int64, ZRef<ZRA::Expr_Rel> > fAdds;
	std::set<int64> fRemoves;
	std::vector<SearchResult> fResults;
	};

} // namespace ZDataspace
} // namespace ZooLib

#endif // __ZDataspace_Source_Client__
