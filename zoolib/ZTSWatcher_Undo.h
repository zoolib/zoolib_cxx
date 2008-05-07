/* ------------------------------------------------------------
Copyright (c) 2008 Andrew Green
http://www.zoolib.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
COPYRIGHT HOLDER(S) BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
------------------------------------------------------------ */

#ifndef __ZTSWatcher_Undo__
#define __ZTSWatcher_Undo__
#include "zconfig.h"

#include "ZTSWatcher.h"

#ifndef ZCONFIG_TSWatcher_Undo_Debug
#	define ZCONFIG_TSWatcher_Undo_Debug 1
#endif

// =================================================================================================
#pragma mark -
#pragma mark * ZTSWatcher_Undo

class ZTSWatcher_Undo : public ZTSWatcher
	{
public:
	enum { kDebug = ZCONFIG_TSWatcher_Undo_Debug };

	ZTSWatcher_Undo(ZRef<ZTSWatcher> iWatcher);
	virtual ~ZTSWatcher_Undo();

// From ZTSWatcher
	virtual void AllocateIDs(size_t iCount, uint64& oBaseID, size_t& oCountIssued);

	virtual void DoIt(
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
	int64 Open(bool iWaitForDoIt);
	void Commit(int64 iMarker, bool iWaitForDoIt);
	void GoTo(int64 iMarker, bool iWaitForDoIt);

private:
	class PQuery;

	class PTuple;

	PTuple& pGetPTuple(uint64 iID);
	PTuple* pGetPTupleExtant(uint64 iID);

	void pRegisterAQC(AddedQueryCombo& ioAQC);

	void Callback();
	static void sCallback(void* iRefcon);

	ZRef<ZTSWatcher> fWatcher;
	Callback_t fCallback;
	void* fRefcon;
	ZMutexNR fMutex_CallDoIt;
	ZMutexNR fMutex_Save;
	ZCondition fCondition_Save;

	enum { eMode_Normal, eMode_Save, eMode_Discard } fMode;

	bool* fHasChangesPtr;

	std::map<int64, PQuery> fPQueries;
	std::map<uint64, PTuple> fPTuples;
	};

#endif // __ZTSWatcher_Undo__
