/* -------------------------------------------------------------------------------------------------
Copyright (c) 2007 Andrew Green and Learning in Motion, Inc.
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

#ifndef __ZTSWatcher_AutoReconnect__
#define __ZTSWatcher_AutoReconnect__
#include "zconfig.h"

#include "zoolib/ZCompat_NonCopyable.h"
#include "zoolib/ZThreadOld.h"
#include "zoolib/tuplebase/ZTSWatcher.h"

#ifndef ZCONFIG_ZTSWatcher_AutoReconnect_Debug
#	define ZCONFIG_ZTSWatcher_AutoReconnect_Debug 1
#endif

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * ZTSWatcher_AutoReconnect

class ZTSWatcher_AutoReconnect : public ZTSWatcher
	{
public:
	enum { kDebug = ZCONFIG_ZTSWatcher_AutoReconnect_Debug };

	ZTSWatcher_AutoReconnect(ZRef<ZTSWatcherFactory> iFactory);
	virtual ~ZTSWatcher_AutoReconnect();

// From ZTSWatcher
	virtual bool AllocateIDs(size_t iCount, uint64& oBaseID, size_t& oCountIssued);

	virtual bool Sync(
		const uint64* iRemovedIDs, size_t iRemovedIDsCount,
		const uint64* iAddedIDs, size_t iAddedIDsCount,
		const int64* iRemovedQueries, size_t iRemovedQueriesCount,
		const AddedQueryCombo* iAddedQueries, size_t iAddedQueriesCount,
		std::vector<uint64>& oAddedIDs,
		std::vector<uint64>& oChangedTupleIDs, std::vector<ZTuple>& oChangedTuples,
		const uint64* iWrittenTupleIDs, const ZTuple* iWrittenTuples, size_t iWrittenTuplesCount,
		std::map<int64, std::vector<uint64> >& oChangedQueries);

	virtual void SetCallback(Callback_t iCallback, void* iRefcon);

// Our protocol
	enum EStatus
		{
		eStatus_Dead,
		eStatus_Failed,
		eStatus_Established
		};

	virtual void NotifyStatusChange(EStatus iStatus);

private:
	bool pEnsureConnection();
	bool pReestablishConnection();
	bool pEstablishConnection();

	void pUpdateRegistrations(
		const uint64* iRemovedIDs, size_t iRemovedIDsCount,
		const uint64* iAddedIDs, size_t iAddedIDsCount,
		const int64* iRemovedQueries, size_t iRemovedQueriesCount,
		const AddedQueryCombo* iAddedQueries, size_t iAddedQueriesCount);

	bool pAllocateIDs(size_t iCount, uint64& oBaseID, size_t& oCountIssued);

	bool pSync(
		const uint64* iRemovedIDs, size_t iRemovedIDsCount,
		const uint64* iAddedIDs, size_t iAddedIDsCount,
		const int64* iRemovedQueries, size_t iRemovedQueriesCount,
		const AddedQueryCombo* iAddedQueries, size_t iAddedQueriesCount,
		std::vector<uint64>& oAddedIDs,
		std::vector<uint64>& oChangedTupleIDs, std::vector<ZTuple>& oChangedTuples,
		const uint64* iWrittenTupleIDs, const ZTuple* iWrittenTuples, size_t iWrittenTuplesCount,
		std::map<int64, std::vector<uint64> >& oChangedQueries);

	void pCallback();
	static void spCallback(void* iRefcon);

private:
	ZMutex fMutex;
	ZRef<ZTSWatcherFactory> fFactory;
	ZRef<ZTSWatcher> fTSWatcher;
	bool fConnectionFailed;
	Callback_t fCallback;
	void* fRefcon;
	std::set<uint64> fTupleIDs;
	std::set<AddedQueryCombo> fQueries;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZTSWatcherFactory_AutoReconnect

class ZTSWatcherFactory_AutoReconnect : public ZTSWatcherFactory
	{
public:
	ZTSWatcherFactory_AutoReconnect(ZRef<ZTSWatcherFactory> iFactory);
	virtual ~ZTSWatcherFactory_AutoReconnect();

	virtual ZRef<ZTSWatcher> MakeTSWatcher();

private:
	ZRef<ZTSWatcherFactory> fFactory;
	};

} // namespace ZooLib

#endif // __ZTSWatcher_AutoReconnect__
