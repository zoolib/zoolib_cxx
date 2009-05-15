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

#ifndef __ZTSWatcherServerAsync__
#define __ZTSWatcherServerAsync__
#include "zconfig.h"

#include "zoolib/ZCommer.h"
#include "zoolib/ZTSWatcher.h"

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * ZTSWatcherServer

class ZTSWatcherServerAsync : public ZCommer
	{
public:
	ZTSWatcherServerAsync(ZRef<ZTSWatcher> iTSWatcher);
	~ZTSWatcherServerAsync();

// From ZCommer
	virtual bool Read(const ZStreamR& iStreamR);
	virtual bool Write(const ZStreamW& iStreamW);

	virtual void Attached();
	virtual void Detached();

private:
	void pCallback();
	static void spCallback(void* iRefcon);

	ZMutex fMutex;
	ZRef<ZTSWatcher> fTSWatcher;
	bool fReceivedClose;
	bool fCallbackNeeded;
	bool fSyncNeeded;
	size_t fIDsNeeded;

	std::vector<uint64> fRemovedIDs;
	std::vector<uint64> fAddedIDs;
	std::vector<int64> fRemovedQueries;
	std::vector<ZTSWatcher::AddedQueryCombo> fAddedQueries;
	std::vector<uint64> fWrittenTupleIDs;
	std::vector<ZTuple> fWrittenTuples;
	};

NAMESPACE_ZOOLIB_END

#endif // __ZTSWatcherServerAsync__
