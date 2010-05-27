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

#ifndef __ZTBRep_Client__
#define __ZTBRep_Client__ 1
#include "zconfig.h"

#include "zoolib/tuplebase/ZTBRep.h"

#include "zoolib/ZStream.h"
#include "zoolib/ZTime.h"

#include <deque>
#include <set>
#include <vector>

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * ZTBRep_Client

class ZTBRep_Client : public ZTBRep
	{
protected:
	ZTBRep_Client();
	virtual ~ZTBRep_Client();

public:
// From ZTBRep
	virtual void AllocateIDs(size_t iCount,
		Callback_AllocateIDs_t iCallback_AllocateIDs, void* iRefcon);

	virtual ZTBRepTransaction* CreateTransaction();

// Our protocol
	bool Reader(const ZStreamR& iStream);
	bool Writer(const ZStreamW& iStream);

	void Live();
	void Fail();

private:
	class Transaction;
	friend class Transaction;

	class TransTuple;
	class TransSearch_t;
	class TransCount_t;

	void Trans_Search(Transaction* iTransaction, const ZTBQuery& iQuery,
		ZTBRepTransaction::Callback_Search_t iCallback, void* iRefcon);

	void Trans_Count(Transaction* iTransaction, const ZTBQuery& iQuery,
		ZTBRepTransaction::Callback_Count_t iCallback, void* iRefcon);

	void Trans_GetTuples(Transaction* iTransaction, size_t iCount, const uint64* iIDs,
		ZTBRepTransaction::Callback_GetTuple_t iCallback, void* iRefcon);

	void Trans_SetTuple(Transaction* iTransaction, uint64 iID, const ZTuple& iTuple);

	void Trans_AbortPreValidate(Transaction* iTransaction);

	void Trans_Validate(Transaction* iTransaction,
		ZTBRepTransaction::Callback_Validate_t iCallback_Validate, void* iRefcon);

	void Trans_AcceptFailure(Transaction* iTransaction);

	void Trans_CancelPostValidate(Transaction* iTransaction);

	void Trans_Commit(Transaction* iTransaction,
		ZTBRepTransaction::Callback_Commit_t iCallback_Commit, void* iRefcon);

	TransTuple* pGetTransTuple(Transaction* iTransaction, uint64 iID);

	void pFailTransactions(std::vector<Transaction*>& iTransactions);
	void pDoStuff();

	void pReader(const ZStreamR& iStream);
	void pWriter(const ZStreamW& iStream);

	ZMutex fMutex_Structure;
	ZCondition fCondition_Sender;
	ZCondition fCondition_Transaction;

	bool fLive;
	uint64 fNextFakeID;

	ZTime fTime_LastRead;
	bool fPingRequested;
	bool fPingSent;

	std::vector<std::pair<uint64, size_t> > fIDs;
	struct AllocateIDRequest_t
		{
		size_t fCount;
		Callback_AllocateIDs_t fCallback;
		void* fRefcon;
		};
	size_t fIDsNeeded;
	std::deque<AllocateIDRequest_t> fAllocateIDPending;

	std::vector<Transaction*> fTransactions_Create_Unsent;
	std::vector<Transaction*> fTransactions_Create_Unacked;
	std::vector<Transaction*> fTransactions_Created;

	std::vector<Transaction*> fTransactions_Validate_Unsent;
	std::vector<Transaction*> fTransactions_Validate_Unacked;
	std::vector<Transaction*> fTransactions_Validated;
	std::vector<Transaction*> fTransactions_Failed;

	std::vector<Transaction*> fTransactions_Aborted_Unsent;

	std::vector<Transaction*> fTransactions_Commit_Unsent;
	std::vector<Transaction*> fTransactions_Commit_Unacked;

	std::set<Transaction*> fTransactions_NeedWork;
	};

} // namespace ZooLib

#endif // __ZTBRep_Client__
