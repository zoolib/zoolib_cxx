/* -------------------------------------------------------------------------------------------------
Copyright (c) 2008 Andrew Green
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

#ifndef __ZTSWatcher_Reader__
#define __ZTSWatcher_Reader__
#include "zconfig.h"

#include "zoolib/ZThreadOld.h"
#include "zoolib/tuplebase/ZTSWatcher.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * ZTSWatcher_Reader

class ZTSWatcher_Reader
:	public ZTSWatcher,
	public ZStreamerReader
	{
public:
	ZTSWatcher_Reader(ZRef<ZStreamerR> iStreamerR, ZRef<ZStreamerW> iStreamerW);

	virtual ~ZTSWatcher_Reader();

// From ZTSWatcher
	virtual bool AllocateIDs(size_t iCount, uint64& oBaseID, size_t& oCountIssued);

	virtual bool Sync(
		const uint64* iRemovedIDs, size_t iRemovedIDsCount,
		const uint64* iAddedIDs, size_t iAddedIDsCount,
		const int64* iRemovedQueries, size_t iRemovedQueriesCount,
		const AddedQueryCombo* iAddedQueries, size_t iAddedQueriesCount,
		std::vector<uint64>& oWatcherAddedIDs,
		std::vector<uint64>& oChangedTupleIDs, std::vector<ZTuple>& oChangedTuples,
		const uint64* iWrittenTupleIDs, const ZTuple* iWrittenTuples, size_t iWrittenTuplesCount,
		std::map<int64, std::vector<uint64> >& oChangedQueries);

	virtual void SetCallback(Callback_t iCallback, void* iRefcon);

// From ZStreamerReader
	virtual bool Read(const ZStreamR& iStreamR);

	virtual void ReadStarted();
	virtual void ReadFinished();

private:
	bool pAllocateIDs(size_t iCount, uint64& oBaseID, size_t& oCountIssued);
	bool pSync(
		const uint64* iRemovedIDs, size_t iRemovedIDsCount,
		const uint64* iAddedIDs, size_t iAddedIDsCount,
		const int64* iRemovedQueries, size_t iRemovedQueriesCount,
		const AddedQueryCombo* iAddedQueries, size_t iAddedQueriesCount,
		std::vector<uint64>& oWatcherAddedIDs,
		std::vector<uint64>& oChangedTupleIDs, std::vector<ZTuple>& oChangedTuples,
		const uint64* iWrittenTupleIDs, const ZTuple* iWrittenTuples, size_t iWrittenTuplesCount,
		std::map<int64, std::vector<uint64> >& oChangedQueries);

	ZMutex fMutex_Structure;
	ZMutex fMutex_Stream;
	ZCondition fCondition;

	ZRef<ZStreamerW> fStreamerW;
	bool fStreamsOkay;
	bool fSentClose;

	uint64* fBaseID;
	size_t* fCountIssued;

	std::vector<uint64>* fWatcherAddedIDs;
	std::vector<uint64>* fChangedTupleIDs;
	std::vector<ZTuple>* fChangedTuples;
	std::map<int64, std::vector<uint64> >* fChangedQueries;

	Callback_t fCallback;
	void* fRefcon;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZTSWatcherFactory_Reader

class ZTSWatcherFactory_Reader : public ZTSWatcherFactory
	{
public:
	ZTSWatcherFactory_Reader(ZRef<ZStreamerRWFactory> iStreamerRWFactory);
	virtual ~ZTSWatcherFactory_Reader();

	virtual ZRef<ZTSWatcher> MakeTSWatcher();

private:
	ZRef<ZStreamerRWFactory> fStreamerRWFactory;
	};

} // namespace ZooLib

#endif // __ZTSWatcher_Reader__
