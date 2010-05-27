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

#ifndef __ZTS_Watchable__
#define __ZTS_Watchable__
#include "zconfig.h"

#include "zoolib/tuplebase/ZTS.h"
#include "zoolib/tuplebase/ZTSWatcher.h"

#ifndef ZCONFIG_TS_Watchable_Debug
#	define ZCONFIG_TS_Watchable_Debug 0
#endif

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * ZTS_Watchable

class ZTS_Watchable : public ZTS
	{
public:
	enum { kDebug = ZCONFIG_TS_Watchable_Debug };

	ZTS_Watchable(ZRef<ZTS> iTS);
	virtual ~ZTS_Watchable();

// From ZTS
	virtual void AllocateIDs(size_t iCount, uint64& oBaseID, size_t& oCount);
	virtual void SetTuples(size_t iCount, const uint64* iIDs, const ZTuple* iTuples);
	virtual void GetTuples(size_t iCount, const uint64* iIDs, ZTuple* oTuples);
	virtual void Search(const ZTBSpec& iTBSpec,
		const std::set<uint64>& iSkipIDs, std::set<uint64>& oIDs);
	virtual ZMutexBase& GetReadLock();
	virtual ZMutexBase& GetWriteLock();

// Our protocol
	ZRef<ZTSWatcher> NewWatcher();

private:
	class PQuery;
	class PSpec;
	class PTuple;
	class WatcherQuery;
	class WatcherQueryTripped;
	class WatcherQueryUsing;

	class Watcher;
	friend class Watcher;

	void Watcher_Finalize(Watcher* iWatcher);

	void Watcher_AllocateIDs(Watcher* iWatcher,
		size_t iCount, uint64& oBaseID, size_t& oCountIssued);

	void Watcher_Sync(Watcher* iWatcher,
		const uint64* iRemovedIDs, size_t iRemovedIDsCount,
		const uint64* iAddedIDs, size_t iAddedIDsCount,
		const int64* iRemovedQueries, size_t iRemovedQueriesCount,
		const ZTSWatcher::AddedQueryCombo* iAddedQueries, size_t iAddedQueriesCount,
		std::vector<uint64>& oAddedIDs,
		std::vector<uint64>& oChangedTupleIDs, std::vector<ZTuple>& oChangedTuples,
		const uint64* iWrittenTupleIDs, const ZTuple* iWrittenTuples, size_t iWrittenTuplesCount,
		std::map<int64, std::vector<uint64> >& oChangedQueries);

	void Watcher_SetCallback(Watcher* iWatcher, ZTSWatcher::Callback_t iCallback, void* iRefcon);

	class Quisitioner;
	friend class Quisitioner;

	void Quisitioner_Search(PQuery* iPQuery, const ZTBSpec& iTBSpec, std::set<uint64>& ioIDs);
	void Quisitioner_Search(PQuery* iPQuery, const ZTBSpec& iTBSpec, std::vector<uint64>& oIDs);

	void Quisitioner_FetchTuples(size_t iCount, const uint64* iIDs, ZTuple* oTuples);
	ZTuple Quisitioner_FetchTuple(uint64 iID);


	void pSetTuples(size_t iCount, const uint64* iIDs, const ZTuple* iTuples);

	void pSetTuples(size_t iCount, const uint64* iIDs, const ZTuple* iTuples,
		std::set<Watcher*>& ioTouchedWatchers);

	void pInvalidatePSpecs(const ZTuple& iOld, const ZTuple& iNew,
		std::set<Watcher*>& ioTouchedWatchers);

	void pInvalidateTuple(uint64 iID, std::set<Watcher*>& ioTouchedWatchers);

	PTuple* pGetPTupleExtant(uint64 iID);
	PTuple* pGetPTuple(uint64 iID);
	void pReleasePTuple(PTuple* iPTuple);

	PSpec* pGetPSpec(const ZTBSpec& iTBSpec);
	void pReleasePSpec(PSpec* iPSpec);

	void pReleasePQuery(PQuery* iPQuery);

	void pUpdateQueryResults(PQuery* iPQuery);

	ZRef<ZTS> fTS;
	ZMutex fMutex_Structure;

	std::map<ZData_Any, PQuery*> fMB_To_PQuery;

	std::map<ZTName, std::set<PSpec*> > fPropName_To_PSpec;
	std::map<ZTBSpec, PSpec*> fTBSpec_To_PSpec;

	std::map<uint64, PTuple*> fID_To_PTuple;

	std::set<Watcher*> fWatchers;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZTSWatcherFactory_TS_Watchable

class ZTSWatcherFactory_TS_Watchable : public ZTSWatcherFactory
	{
public:
	ZTSWatcherFactory_TS_Watchable(ZRef<ZTS_Watchable> iTS_Watchable);

// From ZTSWatcherFactory
	virtual ZRef<ZTSWatcher> MakeTSWatcher();

private:
	ZRef<ZTS_Watchable> fTS_Watchable;
	};

} // namespace ZooLib

#endif // __ZTS_Watchable__
