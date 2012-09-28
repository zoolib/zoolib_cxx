/* -------------------------------------------------------------------------------------------------
Copyright (c) 2006 Andrew Green and Learning in Motion, Inc.
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

#ifndef __ZTSWatcherMUX__
#define __ZTSWatcherMUX__
#include "zconfig.h"

#include "zoolib/ZDList.h"
#include "zoolib/ZThreadOld.h"
#include "zoolib/tuplebase/ZTSWatcher.h"

#ifndef ZCONFIG_TSWatcherMUX_Debug
#	define ZCONFIG_TSWatcherMUX_Debug 1
#endif

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * ZTSWatcherMUX

class ZTSWatcherMUX : public ZTSWatcherFactory
	{
public:
	enum { kDebug = ZCONFIG_TSWatcherMUX_Debug };

	ZTSWatcherMUX(ZRef<ZTSWatcher> iTSWatcher, size_t iCacheSize_Queries, size_t iCacheSize_Tuples);
	~ZTSWatcherMUX();

// From ZTSWatcherFactory
	virtual ZRef<ZTSWatcher> MakeTSWatcher();

// Our protocol
	ZRef<ZTSWatcher> NewWatcher(bool iAlwaysForceSync);

private:
	class Watcher;

	class PQuery;
	class DLink_PQuery_Sync;
	class DLink_PQuery_Cached;

	class WQuery;
	class DLink_WQuery_PQuery;
	class DLink_WQuery_Tripped;

	class PTuple;
	class DLink_PTuple_Sync;
	class DLink_PTuple_Cached;

	class WTuple;
	class DLink_WTuple_PTuple;
	class DLink_WTuple_Tripped;

	void Watcher_Finalize(Watcher* iWatcher);

	bool Watcher_AllocateIDs(Watcher* iWatcher,
		size_t iCount, uint64& oBaseID, size_t& oCountIssued);

	bool Watcher_Sync(Watcher* iWatcher,
		const uint64* iRemovedIDs, size_t iRemovedIDsCount,
		const uint64* iAddedIDs, size_t iAddedIDsCount,
		const int64* iRemovedQueries, size_t iRemovedQueriesCount,
		const ZTSWatcher::AddedQueryCombo* iAddedQueries, size_t iAddedQueriesCount,
		std::vector<uint64>& oAddedIDs,
		std::vector<uint64>& oChangedTupleIDs, std::vector<ZTuple>& oChangedTuples,
		const uint64* iWrittenTupleIDs, const ZTuple* iWrittenTuples, size_t iWrittenTuplesCount,
		std::map<int64, std::vector<uint64> >& oChangedQueries);

	void Watcher_SetCallback(Watcher* iWatcher, ZTSWatcher::Callback_t iCallback, void* iRefcon);

	bool pSyncAll(bool iWaitForSync, Watcher* iWatcher);

	PQuery* pGetPQuery(const ZTSWatcher::AddedQueryCombo& iAQC);
	PQuery* pGetPQueryIfExtant(int64 iRefcon);

	PTuple* pGetPTuple(uint64 iID);
	PTuple* pGetPTupleMustExist(uint64 iID);
	PTuple* pGetPTupleIfExtant(uint64 iID);

	void pCallback();
	static void spCallback(void* iRefcon);

	ZMutex fMutex_Structure;
	ZCondition fCondition;

	int64 fCurrentSyncNumber;
	int64 fNextSyncNumber;
	bool fForceNextSync;

	ZRef<ZTSWatcher> fTSWatcher;

	std::set<Watcher*> fWatchers;

	int64 fNextQueryRefcon;
	std::map<ZMemoryBlock, PQuery> fMB_To_PQuery;
	std::map<int64, PQuery*> fRefcon_To_PQuery;
	DListHead<DLink_PQuery_Sync> fPQueries_Sync;
	DListHead<DLink_PQuery_Cached> fPQueries_Cached;
	size_t fCacheSize_Queries;

	std::map<uint64, PTuple> fPTuples;
	DListHead<DLink_PTuple_Sync> fPTuples_Sync;
	DListHead<DLink_PTuple_Cached> fPTuples_Cached;
	size_t fCacheSize_Tuples;

	friend class Watcher;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZTSWatcherFactory_TSWatcherMUX

class ZTSWatcherFactory_TSWatcherMUX : public ZTSWatcherFactory
	{
public:
	ZTSWatcherFactory_TSWatcherMUX(ZRef<ZTSWatcherMUX> iMUX, bool iAlwaysForceSync);

// From ZTSWatcherFactory
	virtual ZRef<ZTSWatcher> MakeTSWatcher();

private:
	ZRef<ZTSWatcherMUX> fMUX;
	bool fAlwaysForceSync;
	};

} // namespace ZooLib

#endif // __ZTSWatcherMUX__
