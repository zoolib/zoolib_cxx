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

#ifndef __ZTBRep__
#define __ZTBRep__ 1
#include "zconfig.h"

#include "zoolib/ZCompat_NonCopyable.h"
#include "zoolib/tuplebase/ZTBQuery.h"
#include "zoolib/ZTuple.h"
#include "zoolib/ZTxn.h"

#include "zoolib/ZThreadOld.h"

namespace ZooLib {

class ZTBRepTransaction;

// =================================================================================================
#pragma mark -
#pragma mark * ZTBRep

class ZTBRep
:	public ZCounted,
	private ZTxnTarget,
	NonCopyable
	{
public:
	virtual ~ZTBRep();

// Our protocol
	ZTBRepTransaction* FindOrCreateTransaction(const ZTxn& iTxn);

	typedef void (*Callback_AllocateIDs_t)(void* iRefcon, uint64 iBaseID, size_t iCount);
	virtual void AllocateIDs(size_t iCount,
					Callback_AllocateIDs_t iCallback_AllocateIDs, void* iRefcon) = 0;

	virtual ZTBRepTransaction* CreateTransaction() = 0;

private:
// From ZTxnTarget
	virtual void TxnAbortPreValidate(int32 iTxnID);
	virtual void TxnValidate(int32 iTxnID, ValidateCallbackProc iCallback, void* iRefcon);
	virtual void TxnAbortPostValidate(int32 iTxnID, bool iValidationSucceeded);
	virtual void TxnCommit(int32 iTxnID, CommitCallbackProc iCallback, void* iRefcon);

	ZMutex fMutex_Transactions;
	std::map<int32, ZTBRepTransaction*> fTransactions;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZTBRepTransaction

class ZTBRepTransaction : NonCopyable
	{
protected:
	~ZTBRepTransaction() {}

public:
	virtual ZRef<ZTBRep> GetTBRep() = 0;

	typedef void (*Callback_Search_t)(void* iRefcon, std::vector<uint64>& ioResults);
	virtual void Search(const ZTBQuery& iQuery, Callback_Search_t iCallback, void* iRefcon) = 0;

	typedef void (*Callback_Count_t)(void* iRefcon, size_t iResult);
	virtual void Count(const ZTBQuery& iQuery, Callback_Count_t iCallback, void* iRefcon) = 0;

	typedef void (*Callback_GetTuple_t)(void* iRefcon,
		size_t iCount, const uint64* iIDs, const ZTuple* iTuples);
	virtual void GetTuples(size_t iCount, const uint64* iIDs,
					Callback_GetTuple_t iCallback_Get, void* iRefcon) = 0;

	virtual void SetTuple(uint64 iID, const ZTuple& iTuple) = 0;

	virtual void AbortPreValidate() = 0;

	typedef void (*Callback_Validate_t)(bool iSucceeded, void* iRefcon);
	virtual void Validate(Callback_Validate_t iCallback_Validate, void* iRefcon) = 0;

	virtual void AcceptFailure() = 0;

	virtual void CancelPostValidate() = 0;

	typedef void (*Callback_Commit_t)(void* iRefcon);
	virtual void Commit(Callback_Commit_t iCallback_Commit, void* iRefcon) = 0;
	};

} // namespace ZooLib

#endif // __ZTBRep__
