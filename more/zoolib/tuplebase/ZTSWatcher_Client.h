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

#ifndef __ZTSWatcher_Client__
#define __ZTSWatcher_Client__
#include "zconfig.h"

#include "zoolib/ZThreadOld.h"
#include "zoolib/ZStreamer.h"

#include "zoolib/tuplebase/ZTSWatcher.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * ZTSWatcher_Client

class ZTSWatcher_Client : public ZTSWatcher
	{
public:
	ZTSWatcher_Client(
		bool iSupports2, bool iSupports3, bool iSupports4, ZRef<ZStreamerRW> iStreamerRW);

	virtual ~ZTSWatcher_Client();

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

private:
	void pSync1(
		const ZStreamR& iStreamR, const ZStreamW& iStreamW,
		const uint64* iRemovedIDs, size_t iRemovedIDsCount,
		const uint64* iAddedIDs, size_t iAddedIDsCount,
		const int64* iRemovedQueries, size_t iRemovedQueriesCount,
		const AddedQueryCombo* iAddedQueries, size_t iAddedQueriesCount,
		std::vector<uint64>& oAddedIDs,
		std::vector<uint64>& oChangedTupleIDs, std::vector<ZTuple>& oChangedTuples,
		const uint64* iWrittenTupleIDs, const ZTuple* iWrittenTuples, size_t iWrittenTuplesCount,
		std::map<int64, std::vector<uint64> >& oChangedQueries);

	void pSync2(
		const ZStreamR& iStreamR, const ZStreamW& iStreamW,
		const uint64* iRemovedIDs, size_t iRemovedIDsCount,
		const uint64* iAddedIDs, size_t iAddedIDsCount,
		const int64* iRemovedQueries, size_t iRemovedQueriesCount,
		const AddedQueryCombo* iAddedQueries, size_t iAddedQueriesCount,
		std::vector<uint64>& oChangedTupleIDs, std::vector<ZTuple>& oChangedTuples,
		const uint64* iWrittenTupleIDs, const ZTuple* iWrittenTuples, size_t iWrittenTuplesCount,
		std::map<int64, std::vector<uint64> >& oChangedQueries);

	void pSync3(
		const ZStreamR& iStreamR, const ZStreamW& iStreamW,
		const uint64* iRemovedIDs, size_t iRemovedIDsCount,
		const uint64* iAddedIDs, size_t iAddedIDsCount,
		const int64* iRemovedQueries, size_t iRemovedQueriesCount,
		const AddedQueryCombo* iAddedQueries, size_t iAddedQueriesCount,
		std::vector<uint64>& oChangedTupleIDs, std::vector<ZTuple>& oChangedTuples,
		const uint64* iWrittenTupleIDs, const ZTuple* iWrittenTuples, size_t iWrittenTuplesCount,
		std::map<int64, std::vector<uint64> >& oChangedQueries);

	void pSync4(
		const ZStreamR& iStreamR, const ZStreamW& iStreamW,
		const uint64* iRemovedIDs, size_t iRemovedIDsCount,
		const uint64* iAddedIDs, size_t iAddedIDsCount,
		const int64* iRemovedQueries, size_t iRemovedQueriesCount,
		const AddedQueryCombo* iAddedQueries, size_t iAddedQueriesCount,
		std::vector<uint64>& oAddedIDs,
		std::vector<uint64>& oChangedTupleIDs, std::vector<ZTuple>& oChangedTuples,
		const uint64* iWrittenTupleIDs, const ZTuple* iWrittenTuples, size_t iWrittenTuplesCount,
		std::map<int64, std::vector<uint64> >& oChangedQueries);

	ZMutex fMutex;
	bool const fSupports2;
	bool const fSupports3;
	bool const fSupports4;
	ZRef<ZStreamerRW> fStreamerRW;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZTSWatcherFactory_Client

class ZTSWatcherFactory_Client : public ZTSWatcherFactory
	{
public:
	ZTSWatcherFactory_Client(ZRef<ZStreamerRWFactory> iStreamerRWFactory);
	virtual ~ZTSWatcherFactory_Client();

	virtual ZRef<ZTSWatcher> MakeTSWatcher();

private:
	ZRef<ZStreamerRWFactory> fStreamerRWFactory;
	};

typedef ZTSWatcherFactory_Client ZTSWatcherFactory_StreamerRWFactory;

} // namespace ZooLib

#endif // __ZTSWatcher_Client__
