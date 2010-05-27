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

#include "zoolib/tuplebase/ZTBRep.h"

using std::map;

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * ZTBRep

ZTBRep::~ZTBRep()
	{}

ZTBRepTransaction* ZTBRep::FindOrCreateTransaction(const ZTxn& iTxn)
	{
	ZMutexLocker locker(fMutex_Transactions);
	int32 theID = iTxn.GetID();
	map<int32, ZTBRepTransaction*>::iterator iter = fTransactions.find(theID);
	if (iter != fTransactions.end())
		return (*iter).second;

	ZTBRepTransaction* newTransaction = this->CreateTransaction();
	fTransactions.insert(map<int32, ZTBRepTransaction*>::value_type(theID, newTransaction));
	this->RegisterWithTxn(iTxn);
	return newTransaction;
	}

void ZTBRep::TxnAbortPreValidate(int32 iTxnID)
	{
	ZMutexLocker locker(fMutex_Transactions);
	map<int32, ZTBRepTransaction*>::iterator iter = fTransactions.find(iTxnID);
	ZTBRepTransaction* theTBRepTransaction = (*iter).second;
	fTransactions.erase(iter);
	locker.Release();

	theTBRepTransaction->AbortPreValidate();
	}

void ZTBRep::TxnValidate(int32 iTxnID, ValidateCallbackProc iCallback, void* iRefcon)
	{
	ZMutexLocker locker(fMutex_Transactions);
	map<int32, ZTBRepTransaction*>::iterator iter = fTransactions.find(iTxnID);
	ZTBRepTransaction* theTBRepTransaction = (*iter).second;
	locker.Release();

	theTBRepTransaction->Validate(iCallback, iRefcon);
	}

void ZTBRep::TxnAbortPostValidate(int32 iTxnID, bool iValidationSucceeded)
	{
	ZMutexLocker locker(fMutex_Transactions);
	map<int32, ZTBRepTransaction*>::iterator iter = fTransactions.find(iTxnID);
	ZTBRepTransaction* theTBRepTransaction = (*iter).second;
	fTransactions.erase(iter);
	locker.Release();

	if (iValidationSucceeded)
		theTBRepTransaction->CancelPostValidate();
	else
		theTBRepTransaction->AcceptFailure();
	}

void ZTBRep::TxnCommit(int32 iTxnID, CommitCallbackProc iCallback, void* iRefcon)
	{
	ZMutexLocker locker(fMutex_Transactions);
	map<int32, ZTBRepTransaction*>::iterator iter = fTransactions.find(iTxnID);
	ZTBRepTransaction* theTBRepTransaction = (*iter).second;
	fTransactions.erase(iter);
	locker.Release();

	theTBRepTransaction->Commit(iCallback, iRefcon);
	}

} // namespace ZooLib
