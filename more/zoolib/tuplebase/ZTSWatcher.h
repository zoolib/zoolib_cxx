/* -------------------------------------------------------------------------------------------------
Copyright (c) 2005 Andrew Green and Learning in Motion, Inc.
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

#ifndef __ZTSWatcher__
#define __ZTSWatcher__
#include "zconfig.h"

#include "zoolib/ZCompat_NonCopyable.h"
#include "zoolib/ZData_Any.h"
#include "zoolib/ZStream_Data_T.h"

#include "zoolib/tuplebase/ZTBQuery.h"

#include <map>
#include <set>
#include <vector>

namespace ZooLib {

typedef ZData_Any ZMemoryBlock;
typedef ZStreamRPos_Data_T<ZData_Any> ZStreamRPos_MemoryBlock;
typedef ZStreamRWPos_Data_T<ZData_Any> ZStreamRWPos_MemoryBlock;

// =================================================================================================
#pragma mark -
#pragma mark * ZTSWatcher

class ZTSWatcher : public ZCounted, NonCopyable
	{
protected:
	ZTSWatcher();

public:
	virtual bool AllocateIDs(size_t iCount, uint64& oBaseID, size_t& oCountIssued) = 0;

	class AddedQueryCombo;

	virtual bool Sync(
		const uint64* iRemovedIDs, size_t iRemovedIDsCount,
		const uint64* iAddedIDs, size_t iAddedIDsCount,
		const int64* iRemovedQueries, size_t iRemovedQueriesCount,
		const AddedQueryCombo* iAddedQueries, size_t iAddedQueriesCount,
		std::vector<uint64>& oAddedIDs,
		std::vector<uint64>& oChangedTupleIDs, std::vector<ZTuple>& oChangedTuples,
		const uint64* iWrittenTupleIDs, const ZTuple* iWrittenTuples, size_t iWrittenTuplesCount,
		std::map<int64, std::vector<uint64> >& oChangedQueries) = 0;

	typedef void (*Callback_t)(void* iRefcon);
	virtual void SetCallback(Callback_t iCallback, void* iRefcon) = 0;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZTSWatcher

class ZTSWatcher::AddedQueryCombo
	{
public:
	AddedQueryCombo()
		{}

	AddedQueryCombo(size_t iSize)
	:	fMemoryBlock(iSize)
		{}

	inline bool operator<(const AddedQueryCombo& iOther) const
		{ return fRefcon < iOther.fRefcon; }

	int64 fRefcon;
	bool fPrefetch;
	ZData_Any fMemoryBlock;
	ZTBQuery fTBQuery;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZTSWatcherFactory

class ZTSWatcherFactory : public ZCounted
	{
protected:
	ZTSWatcherFactory();
	virtual ~ZTSWatcherFactory();

public:
	virtual ZRef<ZTSWatcher> MakeTSWatcher() = 0;
	};

} // namespace ZooLib

#endif // __ZTSWatcher__
