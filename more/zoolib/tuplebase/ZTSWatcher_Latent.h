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

#ifndef __ZTSWatcher_Latent__
#define __ZTSWatcher_Latent__
#include "zconfig.h"

#include "zoolib/ZThreadOld.h"
#include "zoolib/tuplebase/ZTSWatcher.h"

#ifndef ZCONFIG_TSWatcher_Latent_Debug
#	define ZCONFIG_TSWatcher_Latent_Debug 1
#endif

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * ZTSWatcher_Latent

class ZTSWatcher_Latent : public ZTSWatcher
	{
public:
	enum { kDebug = ZCONFIG_TSWatcher_Latent_Debug };

	ZTSWatcher_Latent(ZRef<ZTSWatcher> iTSWatcher);
	virtual ~ZTSWatcher_Latent();

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
	void Save(bool iWaitForSync);
	void Discard(bool iWaitForSync);

	bool HasChanges(bool iWaitForSync);

private:
	class PQuery;

	class PTuple;

	PTuple& pGetPTuple(uint64 iID);
	PTuple* pGetPTupleExtant(uint64 iID);

	void pRegisterAQC(AddedQueryCombo& ioAQC);

	void pCallback();
	static void spCallback(void* iRefcon);

	ZRef<ZTSWatcher> fTSWatcher;
	Callback_t fCallback;
	void* fRefcon;
	ZMutex fMutex_CallSync;
	ZMutex fMutex_Save;
	ZCondition fCondition_Save;

	enum { eMode_Normal, eMode_Save, eMode_Discard } fMode;

	bool* fHasChangesPtr;

	std::map<int64, PQuery> fPQueries;
	std::map<uint64, PTuple> fPTuples;
	};

} // namespace ZooLib

#endif // __ZTSWatcher_Latent__
