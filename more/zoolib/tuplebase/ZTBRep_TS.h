/* -------------------------------------------------------------------------------------------------
Copyright (c) 2003 Andrew Green and Learning in Motion, Inc.
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

#ifndef __ZTBRep_TS__
#define __ZTBRep_TS__ 1
#include "zconfig.h"

#include "zoolib/tuplebase/ZTBRep.h"
#include "zoolib/tuplebase/ZTS.h"
#include "zoolib/ZDList.h"

#include <map>

#ifndef ZCONFIG_TBRep_TS_Debug
#	define ZCONFIG_TBRep_TS_Debug 2
#endif

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * ZTBRep_TS

class ZTupleIndexFactory;

class ZTBRep_TS : public ZTBRep
	{
public:
	enum { kDebug = ZCONFIG_TBRep_TS_Debug };

	ZTBRep_TS(const std::vector<ZRef<ZTupleIndexFactory> >& iIndexFactories, ZRef<ZTS> iTS);
	virtual ~ZTBRep_TS();

// From ZTBRep
	virtual void AllocateIDs(size_t iCount,
			Callback_AllocateIDs_t iCallback_AllocateIDs, void* iRefcon);

	virtual ZTBRepTransaction* CreateTransaction();

// Our protocol
	void AddIndex(ZRef<ZTupleIndexFactory> iFactory);
	void AddIndices(const std::vector<ZRef<ZTupleIndexFactory> >& iFactories);

	ZRef<ZTS> GetTS();


	// The TransTuple name has to be public, so we can typedef
	// MapTransTuple_t outside this class declaration.
	class TransTuple;

private:
	class Transaction;
	friend class Transaction;

	class TransTupleUsing;

	class TupleInUse;
	typedef std::map<uint64, TupleInUse> MapTupleInUse_t;

// Trans
	ZTuple Trans_FetchTuple(Transaction* iTransaction, uint64 iID);
	void Trans_FetchTuples(Transaction* iTransaction,
			size_t iCount, const uint64* iIDs, ZTuple* oTuples);

	void Trans_Search(Transaction* iTransaction, const ZTBSpec& iSpec, std::set<uint64>& ioIDs);
	void Trans_Search(Transaction* iTransaction, const ZTBSpec& iSpec, std::vector<uint64>& oIDs);

	void Trans_SetTuple(Transaction* iTransaction, uint64 iID, const ZTuple& iTuple);

	void Trans_AbortPreValidate(Transaction* iTransaction);

	void Trans_Validate(Transaction* iTransaction,
			ZTBRepTransaction::Callback_Validate_t iCallback_Validate, void* iRefcon);

	void Trans_AcceptFailure(Transaction* iTransaction);

	void Trans_CancelPostValidate(Transaction* iTransaction);

	void Trans_Commit(Transaction* iTransaction);

// Private
	void pFetchTuples(Transaction* iTransaction,
		size_t iCount, const uint64* iIDs, ZTuple* oTuples);
	void pFetchTuples(Transaction* iTransaction, const std::set<uint64>& iIDs);

	void pTryValidation(Transaction* iTransaction);
	bool pAcquireLockOrQueue(Transaction* iTransaction, TupleInUse& iTIU, bool iWrite);
	void pReleaseLock(Transaction* iTransaction, TupleInUse& iTIU, bool iWrite);

	TransTuple& pGetTransTuple(Transaction* iTransaction, uint64 iID);
	TransTuple& pAllocateTransTuple(const void* iPosition,
			Transaction* iTransaction, TupleInUse& iTIU,
			const ZTuple& iValue, uint64 iClockStart, bool iWritten);
	void pDisposeTransTuple(TransTuple& iTransTuple);

	TupleInUse& pGetTupleInUse(uint64 iID);
	void pReleaseTupleInUse(TupleInUse& iTupleInUse);

	ZRef<ZTS> fTS;

	ZMutex fMutex_Structure;
	std::vector<ZRef<ZTupleIndexFactory> > fIndexFactories;

	uint64 fClock;

	MapTupleInUse_t fTuplesInUse;

	std::set<Transaction*> fTransactions;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZTBRep_TS::TupleInUse

class ZTBRep_TS::TupleInUse
	{
public:
	uint64 fID;

	DListHead<TransTupleUsing> fUsingTransTuples;

	Transaction* fTransaction_Writer;
	size_t fTransaction_ReaderCount;

	Transaction* fTransaction_Waiting;

	ZTuple fValue;

	uint64 fClock_LastWritten;
	};

} // namespace ZooLib

#endif // __ZTBRep_TS__
