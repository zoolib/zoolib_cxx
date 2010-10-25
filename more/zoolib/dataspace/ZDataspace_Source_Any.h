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

#ifndef __ZDataspace_Source_Any__
#define __ZDataspace_Source_Any__ 1
#include "zconfig.h"

#include "zoolib/dataspace/ZDataspace_Source.h"
#include "zoolib/ZVal_Any.h"

#include <map>
#include <set>

namespace ZooLib {
namespace ZDataspace {

// =================================================================================================
#pragma mark -
#pragma mark * Source_Any

class Source_Any : public Source
	{
public:
	enum { kDebug = 1 };

	Source_Any();
	virtual ~Source_Any();

// From Source
	virtual std::set<RelHead> GetRelHeads();

	virtual void Update(
		bool iLocalOnly,
		const AddedSearch* iAdded, size_t iAddedCount,
		const int64* iRemoved, size_t iRemovedCount,
		std::vector<SearchResult>& oChanged,
		ZRef<Event>& oEvent);

private:
	ZRef<Clock> fClock;

	std::map<uint64, ZVal_Any> fMap;

	class PQuery;
	std::map<int64, PQuery*> fMap_RefconToPQuery;
	};

} // namespace ZDataspace
} // namespace ZooLib

#endif // __ZDataspace_Source_Any__
