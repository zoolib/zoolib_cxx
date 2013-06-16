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

#include "zoolib/tuplebase/ZTBRep_Client.h"

#include "zoolib/ZLog.h"
#include "zoolib/ZMACRO_foreach.h"
#include "zoolib/ZUtil_STL_vector.h"

#include "zoolib/tuplebase/ZTupleIndex.h"

namespace ZooLib {

using namespace ZUtil_STL;

using std::deque;
using std::map;
using std::min;
using std::pair;
using std::set;
using std::string;
using std::vector;

#define kDebug_TBRep_Client 1

#ifdef ZCONFIG_TBServer_ShowCommsTuples
#	define kDebug_ShowCommsTuples ZCONFIG_TBServer_ShowCommsTuples
#else
#	define kDebug_ShowCommsTuples 0
#endif

// =================================================================================================
#pragma mark -
#pragma mark * ZTBRep_Client::TransTuple

class ZTBRep_Client::TransTuple
	{
public:
	TransTuple(uint64 iID);

	uint64 fID;
	ZTuple fValue;

	bool fRequestSent;
	bool fValueReturned;

	bool fWritten;
	bool fWriteSent;

	ZTBRepTransaction::Callback_GetTuple_t fCallback_GetTuple;
	void* fRefcon;
	};

ZTBRep_Client::TransTuple::TransTuple(uint64 iID)
	{
	fID = iID;

	fRequestSent = false;
	fValueReturned = false;

	fWritten = false;
	fWriteSent = false;

	fCallback_GetTuple = nullptr;
	fRefcon = nullptr;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZTBRep_Client::TransSearch_t

class ZTBRep_Client::TransSearch_t
	{
public:
	Transaction* fTransaction;
	ZTBRepTransaction::Callback_Search_t fCallback;
	void* fRefcon;
	ZTuple fQueryAsTuple;
	vector<uint64> fResults;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZTBRep_Client::TransCount_t

class ZTBRep_Client::TransCount_t
	{
public:
	Transaction* fTransaction;
	ZTBRepTransaction::Callback_Count_t fCallback;
	void* fRefcon;
	ZTuple fQueryAsTuple;
	size_t fResult;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZTBRep_Client::Transaction

class ZTBRep_Client::Transaction : public ZTBRepTransaction
	{
public:
	Transaction(ZRef<ZTBRep_Client> iTBRep, bool iLive);
	~Transaction();

	virtual ZRef<ZTBRep> GetTBRep();

	virtual void Search(const ZTBQuery& iQuery, Callback_Search_t iCallback, void* iRefcon);

	virtual void Count(const ZTBQuery& iQuery, Callback_Count_t iCallback, void* iRefcon);

	virtual void GetTuples(size_t iCount, const uint64* iIDs,
		Callback_GetTuple_t iCallback, void* iRefcon);

	virtual void SetTuple(uint64 iID, const ZTuple& iTuple);

	virtual void AbortPreValidate();

	virtual void Validate(Callback_Validate_t iCallback_Validate, void* iRefcon);

	virtual void AcceptFailure();

	virtual void CancelPostValidate();

	virtual void Commit(Callback_Commit_t iCallback_Commit, void* iRefcon);

private:
	ZRef<ZTBRep_Client> fTBRep;
	uint64 fServerID;
	bool fLive;

	map<uint64, TransTuple*> fTransTuples;

	set<TransTuple*> fTransTuples_NeedWork;

	vector<TransSearch_t*> fSearches_Unsent;
	set<TransSearch_t*> fSearches_Waiting;

	vector<TransCount_t*> fCounts_Unsent;
	set<TransCount_t*> fCounts_Waiting;

	Callback_Validate_t fCallback_Validate;
	Callback_Commit_t fCallback_Commit;
	void* fRefcon;

	friend class ZTBRep_Client;
	friend class TransTuple;
	};

ZTBRep_Client::Transaction::Transaction(ZRef<ZTBRep_Client> iTBRep, bool iLive)
:	fTBRep(iTBRep),
	fServerID(0),
	fLive(iLive)
	{}

ZTBRep_Client::Transaction::~Transaction()
	{
	for (map<uint64, TransTuple*>::iterator i = fTransTuples.begin(); i != fTransTuples.end(); ++i)
		delete (*i).second;
	}

ZRef<ZTBRep> ZTBRep_Client::Transaction::GetTBRep()
	{ return fTBRep; }

void ZTBRep_Client::Transaction::Search(
	const ZTBQuery& iQuery, Callback_Search_t iCallback, void* iRefcon)
	{ fTBRep->Trans_Search(this, iQuery, iCallback, iRefcon); }

void ZTBRep_Client::Transaction::Count(
	const ZTBQuery& iQuery, Callback_Count_t iCallback, void* iRefcon)
	{ fTBRep->Trans_Count(this, iQuery, iCallback, iRefcon); }

void ZTBRep_Client::Transaction::GetTuples(size_t iCount, const uint64* iIDs,
					Callback_GetTuple_t iCallback, void* iRefcon)
	{ fTBRep->Trans_GetTuples(this, iCount, iIDs, iCallback, iRefcon); }

void ZTBRep_Client::Transaction::SetTuple(uint64 iID, const ZTuple& iTuple)
	{ fTBRep->Trans_SetTuple(this, iID, iTuple); }

void ZTBRep_Client::Transaction::AbortPreValidate()
	{ fTBRep->Trans_AbortPreValidate(this); }

void ZTBRep_Client::Transaction::Validate(Callback_Validate_t iCallback_Validate, void* iRefcon)
	{ fTBRep->Trans_Validate(this, iCallback_Validate, iRefcon); }

void ZTBRep_Client::Transaction::CancelPostValidate()
	{ fTBRep->Trans_CancelPostValidate(this); }

void ZTBRep_Client::Transaction::AcceptFailure()
	{ fTBRep->Trans_AcceptFailure(this); }

void ZTBRep_Client::Transaction::Commit(Callback_Commit_t iCallback_Commit, void* iRefcon)
	{ fTBRep->Trans_Commit(this, iCallback_Commit, iRefcon); }

// =================================================================================================
#pragma mark -
#pragma mark * ZTBRep_Client

ZTBRep_Client::ZTBRep_Client()
:	fLive(false),
	fNextFakeID(~0),
	fTime_LastRead(ZTime::sSystem()),
	fPingRequested(false),
	fPingSent(false),
	fIDsNeeded(0)
	{}

ZTBRep_Client::~ZTBRep_Client()
	{}

void ZTBRep_Client::AllocateIDs(
	size_t iCount, Callback_AllocateIDs_t iCallback, void* iRefcon)
	{
	ZMutexLocker lock(fMutex_Structure);
	while (iCount)
		{
		if (fIDs.empty())
			break;
		uint64 baseID = fIDs.back().first;
		size_t countToIssue = iCount;
		if (countToIssue < fIDs.back().second)
			{
			fIDs.back().first += countToIssue;
			fIDs.back().second -= countToIssue;
			}
		else
			{
			countToIssue = fIDs.back().second;
			fIDs.pop_back();
			}
		iCount -= countToIssue;
		iCallback(iRefcon, baseID, countToIssue);
		}

	if (iCount)
		{
		AllocateIDRequest_t theRequest;
		theRequest.fCount = iCount;
		theRequest.fCallback = iCallback;
		theRequest.fRefcon = iRefcon;
		fAllocateIDPending.push_back(theRequest);
		fIDsNeeded += iCount;
		fCondition_Sender.Broadcast();
		}

	this->pDoStuff();
	}

ZTBRepTransaction* ZTBRep_Client::CreateTransaction()
	{
	ZMutexLocker lock(fMutex_Structure);

	Transaction* theTransaction = new Transaction(this, fLive);
	sInsertSortedMust(kDebug_TBRep_Client, fTransactions_Create_Unsent, theTransaction);

	fCondition_Sender.Broadcast();

	this->pDoStuff();

	while (!theTransaction->fServerID)
		fCondition_Transaction.Wait(fMutex_Structure);

	return theTransaction;
	}

bool ZTBRep_Client::Reader(const ZStreamR& iStreamR)
	{
	try
		{
		this->pReader(iStreamR);
		return true;
		}
	catch (...)
		{}
	if (ZLOG(s, eDebug, "ZTBRep_Client"))
		s << "Returning false from reader";

	ZMutexLocker locker(fMutex_Structure);
	// Wake writer
	fCondition_Sender.Broadcast();

	return false;
	}

bool ZTBRep_Client::Writer(const ZStreamW& iStreamW)
	{
	try
		{
		this->pWriter(iStreamW);
		if (!fLive)
			{
			if (ZLOG(s, eDebug, "ZTBRep_Client"))
				s << "Returning false from writer, fLive is false";
			}
		return fLive;
		}
	catch (...)
		{}

	if (ZLOG(s, eDebug, "ZTBRep_Client"))
		s << "Returning false from writer, caught exception";
	return false;
	}

void ZTBRep_Client::Live()
	{
	ZMutexLocker lock(fMutex_Structure);
	ZAssertStop(kDebug_TBRep_Client, !fLive);
	fLive = true;
	}

void ZTBRep_Client::Fail()
	{
	ZMutexLocker lock(fMutex_Structure);
	ZAssertStop(kDebug_TBRep_Client, fLive);

	fLive = false;

	// Mark all extant transactions as dead.
	this->pFailTransactions(fTransactions_Create_Unsent);
	this->pFailTransactions(fTransactions_Create_Unacked);
	this->pFailTransactions(fTransactions_Created);

	this->pFailTransactions(fTransactions_Validate_Unsent);
	this->pFailTransactions(fTransactions_Validate_Unacked);
	this->pFailTransactions(fTransactions_Validated);
	this->pFailTransactions(fTransactions_Failed);

	this->pFailTransactions(fTransactions_Aborted_Unsent);
	this->pFailTransactions(fTransactions_Commit_Unsent);
	// This is the problem one. Has the server done the work?
	this->pFailTransactions(fTransactions_Commit_Unacked);

	this->pDoStuff();
	}

void ZTBRep_Client::Trans_Search(Transaction* iTransaction, const ZTBQuery& iQuery,
	ZTBRepTransaction::Callback_Search_t iCallback, void* iRefcon)
	{
	ZMutexLocker locker(fMutex_Structure);
	TransSearch_t* theTransSearch = new TransSearch_t;
	theTransSearch->fTransaction = iTransaction;
	theTransSearch->fRefcon = iRefcon;
	theTransSearch->fCallback = iCallback;
	theTransSearch->fQueryAsTuple = iQuery.AsTuple();
	iTransaction->fSearches_Unsent.push_back(theTransSearch);
	fTransactions_NeedWork.insert(iTransaction);
	fCondition_Sender.Broadcast();

	this->pDoStuff();
	}

void ZTBRep_Client::Trans_Count(Transaction* iTransaction, const ZTBQuery& iQuery,
	ZTBRepTransaction::Callback_Count_t iCallback, void* iRefcon)
	{
	ZMutexLocker locker(fMutex_Structure);
	TransCount_t* theTransCount = new TransCount_t;
	theTransCount->fTransaction = iTransaction;
	theTransCount->fRefcon = iRefcon;
	theTransCount->fCallback = iCallback;
	theTransCount->fQueryAsTuple = iQuery.AsTuple();
	iTransaction->fCounts_Unsent.push_back(theTransCount);
	fTransactions_NeedWork.insert(iTransaction);
	fCondition_Sender.Broadcast();

	this->pDoStuff();
	}

void ZTBRep_Client::Trans_GetTuples(Transaction* iTransaction, size_t iCount, const uint64* iIDs,
	ZTBRepTransaction::Callback_GetTuple_t iCallback, void* iRefcon)
	{
	ZMutexLocker locker(fMutex_Structure);
	while (iCount--)
		{
		uint64 theID = *iIDs++;
		TransTuple* theTransTuple = this->pGetTransTuple(iTransaction, theID);
		if (theTransTuple->fWritten)
			{
			// It's been written to, so we know its value.
			locker.Release();
			if (iCallback)
				iCallback(iRefcon, 1, &theID, &theTransTuple->fValue);
			}
		else
			{
			if (theTransTuple->fValueReturned)
				{
				locker.Release();
				if (iCallback)
					iCallback(iRefcon, 1, &theID, &theTransTuple->fValue);
				}
			else
				{
				if (theTransTuple->fRequestSent)
					{
					if (iCallback)
						{
						ZAssertStop(kDebug_TBRep_Client, !theTransTuple->fCallback_GetTuple);
						theTransTuple->fCallback_GetTuple = iCallback;
						theTransTuple->fRefcon = iRefcon;
						}
					}
				else
					{
					ZAssertStop(kDebug_TBRep_Client, !theTransTuple->fCallback_GetTuple);
					theTransTuple->fCallback_GetTuple = iCallback;
					theTransTuple->fRefcon = iRefcon;
					iTransaction->fTransTuples_NeedWork.insert(theTransTuple);
					fTransactions_NeedWork.insert(iTransaction);
					fCondition_Sender.Broadcast();
					}
				}
			}
		locker.Acquire();
		}

	this->pDoStuff();
	}

void ZTBRep_Client::Trans_SetTuple(Transaction* iTransaction, uint64 iID, const ZTuple& iTuple)
	{
	ZMutexLocker locker(fMutex_Structure);
	TransTuple* theTransTuple = this->pGetTransTuple(iTransaction, iID);
	theTransTuple->fWritten = true;
	theTransTuple->fWriteSent = false;
	theTransTuple->fValue = iTuple;
	iTransaction->fTransTuples_NeedWork.insert(theTransTuple);
	fTransactions_NeedWork.insert(iTransaction);
	fCondition_Sender.Broadcast();

	this->pDoStuff();
	}

void ZTBRep_Client::Trans_AbortPreValidate(Transaction* iTransaction)
	{
	ZMutexLocker lock(fMutex_Structure);

	this->pDoStuff();

	// Wait till a pending create has finished.
	while (!iTransaction->fServerID)
		fCondition_Transaction.Wait(fMutex_Structure);

	// iTransaction must be created.
	if (!sQEraseSorted(fTransactions_Created, iTransaction))
		{
		ZDebugStopf(kDebug_TBRep_Client, ("Aborting a transaction that's not created"));
		}

	sInsertSortedMust(kDebug_TBRep_Client, fTransactions_Aborted_Unsent, iTransaction);
	fCondition_Sender.Broadcast();

	this->pDoStuff();
	}

void ZTBRep_Client::Trans_Validate(Transaction* iTransaction,
	ZTBRepTransaction::Callback_Validate_t iCallback_Validate, void* iRefcon)
	{
	ZMutexLocker lock(fMutex_Structure);

	sEraseSortedMust(kDebug_TBRep_Client, fTransactions_Created, iTransaction);
	sInsertSortedMust(kDebug_TBRep_Client, fTransactions_Validate_Unsent, iTransaction);

	iTransaction->fCallback_Validate = iCallback_Validate;
	iTransaction->fRefcon = iRefcon;

	fCondition_Sender.Broadcast();

	this->pDoStuff();
	}

void ZTBRep_Client::Trans_CancelPostValidate(Transaction* iTransaction)
	{
	ZMutexLocker lock(fMutex_Structure);

	this->pDoStuff();

	// Wait till a pending create has finished.
	while (!iTransaction->fServerID)
		fCondition_Transaction.Wait(fMutex_Structure);

	// iTransaction must either be validated
	if (!sQEraseSorted(fTransactions_Validated, iTransaction))
		{
		ZDebugStopf(kDebug_TBRep_Client, ("Canceling a transaction that's not validated"));
		}

	sInsertSortedMust(kDebug_TBRep_Client, fTransactions_Aborted_Unsent, iTransaction);
	fCondition_Sender.Broadcast();
	this->pDoStuff();
	}

void ZTBRep_Client::Trans_AcceptFailure(Transaction* iTransaction)
	{
	ZMutexLocker lock(fMutex_Structure);
	sEraseSortedMust(kDebug_TBRep_Client, fTransactions_Failed, iTransaction);
	delete iTransaction;
	}

void ZTBRep_Client::Trans_Commit(Transaction* iTransaction,
	ZTBRepTransaction::Callback_Commit_t iCallback_Commit, void* iRefcon)
	{
	ZMutexLocker lock(fMutex_Structure);

	sEraseSortedMust(kDebug_TBRep_Client, fTransactions_Validated, iTransaction);
	sInsertSortedMust(kDebug_TBRep_Client, fTransactions_Commit_Unsent, iTransaction);

	iTransaction->fCallback_Commit = iCallback_Commit;
	iTransaction->fRefcon = iRefcon;
	fCondition_Sender.Broadcast();

	this->pDoStuff();
	}

ZTBRep_Client::TransTuple* ZTBRep_Client::pGetTransTuple(Transaction* iTransaction, uint64 iID)
	{
	map<uint64, TransTuple*>::iterator iter = iTransaction->fTransTuples.find(iID);
	if (iter != iTransaction->fTransTuples.end())
		return (*iter).second;

	TransTuple* newTransTuple = new TransTuple(iID);

	iTransaction->fTransTuples.insert(map<uint64, TransTuple*>::value_type(iID, newTransTuple));

	return newTransTuple;
	}

// =================================================================================================

void ZTBRep_Client::pFailTransactions(vector<Transaction*>& iTransactions)
	{
	for (vector<Transaction*>::iterator i = iTransactions.begin();
		i != iTransactions.end(); ++i)
		{
		Transaction* theTransaction = *i;
		theTransaction->fLive = false;
		fTransactions_NeedWork.insert(theTransaction);
		}
	}

// Use all 1's for the dummy ID.
static const uint64 kDummyServerID = uint64(-1);

void ZTBRep_Client::pDoStuff()
	{
	ZAssertStop(kDebug_TBRep_Client, fMutex_Structure.IsLocked());

	if (!fLive)
		{
		// Issue fake IDs, which start at MAXUINT64 and work downwards.
		// These will never be usable in a live transaction, because
		// we don't issue them if we're live, and a transaction can
		// go from live to dead, but not back again.
		while (!fAllocateIDPending.empty())
			{
			AllocateIDRequest_t& theAIR = fAllocateIDPending.front();

			size_t theCount = theAIR.fCount;
			theAIR.fCount = 0;
			fNextFakeID -= theCount;

			theAIR.fCallback(theAIR.fRefcon, fNextFakeID, theCount);

			fAllocateIDPending.pop_front();
			}
		}

	// For transactions that are dead, we basically do what pReader/pWriter
	// do, but with bogus results obviously.

	if (!fTransactions_Create_Unsent.empty())
		{
		for (vector<Transaction*>::iterator i = fTransactions_Create_Unsent.begin();
			i != fTransactions_Create_Unsent.end(); /*no inc*/)
			{
			Transaction* theTransaction = *i;
			if (theTransaction->fLive)
				{
				++i;
				}
			else
				{
				i = fTransactions_Create_Unsent.erase(i);
				// Move it directly into fTransactions_Created
				sInsertSortedMust(
					kDebug_TBRep_Client, fTransactions_Created, theTransaction);
				// Give it a fake server ID, to wake Create/Abort etc.
				theTransaction->fServerID = kDummyServerID;
				}
			}
		}

	if (!fTransactions_Create_Unacked.empty())
		{
		for (vector<Transaction*>::iterator i = fTransactions_Create_Unacked.begin();
			i != fTransactions_Create_Unacked.end(); /*no inc*/)
			{
			Transaction* theTransaction = *i;
			if (theTransaction->fLive)
				{
				++i;
				}
			else
				{
				i = fTransactions_Create_Unacked.erase(i);
				// Move it into fTransactions_Created
				sInsertSortedMust(
					kDebug_TBRep_Client, fTransactions_Created, theTransaction);
				// Give it a fake server ID, to wake Create/Abort etc.
				theTransaction->fServerID = kDummyServerID;
				}
			}
		}

	if (!fTransactions_Validate_Unsent.empty())
		{
		vector<Transaction*> toCall;
		for (vector<Transaction*>::iterator i = fTransactions_Validate_Unsent.begin();
			i != fTransactions_Validate_Unsent.end(); /*no inc*/)
			{
			Transaction* theTransaction = *i;
			if (theTransaction->fLive)
				{
				++i;
				}
			else
				{
				i = fTransactions_Validate_Unsent.erase(i);
				// Move it directly into fTransactions_Failed
				sInsertSortedMust(
					kDebug_TBRep_Client, fTransactions_Failed, theTransaction);
				toCall.push_back(theTransaction);
				}
			}

		for (vector<Transaction*>::iterator i = toCall.begin(); i != toCall.end(); ++i)
			{
			Transaction* theTransaction = *i;
			if (theTransaction->fCallback_Validate)
				theTransaction->fCallback_Validate(false, theTransaction->fRefcon);
			}
		}

	if (!fTransactions_Validate_Unacked.empty())
		{
		vector<Transaction*> toCall;
		for (vector<Transaction*>::iterator i = fTransactions_Validate_Unacked.begin();
			i != fTransactions_Validate_Unacked.end(); /*no inc*/)
			{
			Transaction* theTransaction = *i;
			if (theTransaction->fLive)
				{
				++i;
				}
			else
				{
				i = fTransactions_Validate_Unacked.erase(i);
				// Move it directly into fTransactions_Failed
				sInsertSortedMust(
					kDebug_TBRep_Client, fTransactions_Failed, theTransaction);
				toCall.push_back(theTransaction);
				}
			}

		for (vector<Transaction*>::iterator i = toCall.begin(); i != toCall.end(); ++i)
			{
			Transaction* theTransaction = *i;
			if (theTransaction->fCallback_Validate)
				theTransaction->fCallback_Validate(false, theTransaction->fRefcon);
			}
		}

	if (!fTransactions_Aborted_Unsent.empty())
		{
		for (vector<Transaction*>::iterator i = fTransactions_Aborted_Unsent.begin();
			i != fTransactions_Aborted_Unsent.end(); /*no inc*/)
			{
			Transaction* theTransaction = *i;
			if (theTransaction->fLive)
				{
				++i;
				}
			else
				{
				i = fTransactions_Aborted_Unsent.erase(i);
				delete theTransaction;
				}
			}
		}

	if (!fTransactions_Commit_Unsent.empty())
		{
		vector<Transaction*> toCall;
		for (vector<Transaction*>::iterator i = fTransactions_Commit_Unsent.begin();
			i != fTransactions_Commit_Unsent.end(); /*no inc*/)
			{
			Transaction* theTransaction = *i;
			if (theTransaction->fLive)
				{
				++i;
				}
			else
				{
				i = fTransactions_Commit_Unsent.erase(i);
				toCall.push_back(theTransaction);
				}
			}

		for (vector<Transaction*>::iterator i = toCall.begin(); i != toCall.end(); ++i)
			{
			Transaction* theTransaction = *i;
			// Need to extend API, to allow indication of late failure.
			if (theTransaction->fCallback_Commit)
				theTransaction->fCallback_Commit(theTransaction->fRefcon);
			delete theTransaction;
			}
		}

	if (!fTransactions_Commit_Unacked.empty())
		{
		vector<Transaction*> toCall;
		for (vector<Transaction*>::iterator i = fTransactions_Commit_Unacked.begin();
			i != fTransactions_Commit_Unacked.end(); /*no inc*/)
			{
			Transaction* theTransaction = *i;
			if (theTransaction->fLive)
				{
				++i;
				}
			else
				{
				i = fTransactions_Commit_Unacked.erase(i);
				toCall.push_back(theTransaction);
				}
			}

		for (vector<Transaction*>::iterator i = toCall.begin(); i != toCall.end(); ++i)
			{
			Transaction* theTransaction = *i;
			// Need to extend API, to allow indication of late failure.
			if (theTransaction->fCallback_Commit)
				theTransaction->fCallback_Commit(theTransaction->fRefcon);
			delete theTransaction;
			}
		}

	if (!fTransactions_NeedWork.empty())
		{
		for (set<Transaction*>::iterator i = fTransactions_NeedWork.begin();
			i != fTransactions_NeedWork.end(); /*no inc*/)
			{
			Transaction* theTransaction = *i;
			if (theTransaction->fLive)
				{
				++i;
				continue;
				}

			fTransactions_NeedWork.erase(i);
			i = fTransactions_NeedWork.lower_bound(theTransaction);

			if (!theTransaction->fTransTuples_NeedWork.empty())
				{
				for (set<TransTuple*>::iterator i = theTransaction->fTransTuples_NeedWork.begin();
					i != theTransaction->fTransTuples_NeedWork.end(); ++i)
					{
					TransTuple* theTransTuple = *i;
					if (!theTransTuple->fWritten)
						{
						if (theTransTuple->fCallback_GetTuple)
							{
							ZTBRepTransaction::Callback_GetTuple_t theCallback =
								theTransTuple->fCallback_GetTuple;
							void* theRefcon = theTransTuple->fRefcon;
							theTransTuple->fCallback_GetTuple = nullptr;
							theTransTuple->fRefcon = nullptr;
							const uint64 theID = theTransTuple->fID;
							const ZTuple theValue = theTransTuple->fValue;
							theCallback(theRefcon, 1, &theID, &theValue);
							}
						}
					}
				theTransaction->fTransTuples_NeedWork.clear();
				}

			for (vector<TransSearch_t*>::iterator i = theTransaction->fSearches_Unsent.begin();
				i != theTransaction->fSearches_Unsent.end(); ++i)
				{
				TransSearch_t* theSearch = *i;
				vector<uint64> emptyVectorIDs;
				theSearch->fCallback(theSearch->fRefcon, emptyVectorIDs);
				delete theSearch;
				}
			theTransaction->fSearches_Unsent.clear();

			for (set<TransSearch_t*>::iterator i = theTransaction->fSearches_Waiting.begin();
				i != theTransaction->fSearches_Waiting.end(); ++i)
				{
				TransSearch_t* theSearch = *i;
				vector<uint64> emptyVectorIDs;
				theSearch->fCallback(theSearch->fRefcon, emptyVectorIDs);
				delete theSearch;
				}
			theTransaction->fSearches_Waiting.clear();

			for (vector<TransCount_t*>::iterator i = theTransaction->fCounts_Unsent.begin();
				i != theTransaction->fCounts_Unsent.end(); ++i)
				{
				TransCount_t* theCount = *i;
				theCount->fCallback(theCount->fRefcon, 0);
				delete theCount;
				}
			theTransaction->fCounts_Unsent.clear();

			for (set<TransCount_t*>::iterator i = theTransaction->fCounts_Waiting.begin();
				i != theTransaction->fCounts_Waiting.end(); ++i)
				{
				TransCount_t* theCount = *i;
				theCount->fCallback(theCount->fRefcon, 0);
				delete theCount;
				}
			theTransaction->fCounts_Waiting.clear();
			}
		}
	fCondition_Transaction.Broadcast();
	}

void ZTBRep_Client::pReader(const ZStreamR& iStream)
	{
	ZTuple theResp(iStream);

	ZMutexLocker locker(fMutex_Structure);
	fTime_LastRead = ZTime::sSystem();

	const string theWhat = theResp.GetString("What");

	if (kDebug_ShowCommsTuples)
		{
//##		if (ZLOG(s, eDebug, "ZTBRep_Client"))
//##			s << "<< " << theResp;
		}

	if (theWhat == "Ping")
		{
		fPingRequested = true;
		fCondition_Sender.Broadcast();
		}
	else if (theWhat == "Pong")
		{
		fPingSent = false;
		}
	else if (theWhat == "AllocateIDs_Ack")
		{
		uint64 baseID = theResp.Get<int64>("BaseID");
		size_t count = theResp.GetInt32("Count");
		// Distribute them to waiting requests.
		while (count && !fAllocateIDPending.empty())
			{
			size_t countToIssue = min(count, fAllocateIDPending.front().fCount);
			fAllocateIDPending.front().fCallback(
				fAllocateIDPending.front().fRefcon, baseID, countToIssue);
			baseID += countToIssue;
			count -= countToIssue;
			fAllocateIDPending.front().fCount -= countToIssue;
			if (fAllocateIDPending.front().fCount == 0)
				fAllocateIDPending.pop_front();
			}
		if (count)
			fIDs.push_back(pair<uint64, size_t>(baseID, count));
		}
	else if (theWhat == "Create_Ack")
		{
		foreachi (i, theResp.Get<ZSeq_Any>("IDs"))
			{
			ZTuple theTuple = (*i).Get<ZMap_Any>();
			Transaction* theTransaction =
				reinterpret_cast<Transaction*>(theTuple.Get<int64>("ClientID"));

			sEraseSortedMust(
				kDebug_TBRep_Client, fTransactions_Create_Unacked, theTransaction);

			sInsertSortedMust(
				kDebug_TBRep_Client, fTransactions_Created, theTransaction);

			ZAssertStop(kDebug_TBRep_Client, theTransaction->fServerID == 0);

			theTransaction->fServerID = theTuple.Get<int64>("ServerID");
			fCondition_Transaction.Broadcast();
			}
		}
	else if (theWhat == "Validate_Succeeded")
		{
		foreachi (i, theResp.Get<ZSeq_Any>("ClientIDs"))
			{
			Transaction* theTransaction = reinterpret_cast<Transaction*>((*i).Get<int64>());

			sEraseSortedMust(
				kDebug_TBRep_Client, fTransactions_Validate_Unacked, theTransaction);
			sInsertSortedMust(
				kDebug_TBRep_Client, fTransactions_Validated, theTransaction);

			if (theTransaction->fCallback_Validate)
				theTransaction->fCallback_Validate(true, theTransaction->fRefcon);
			}
		}
	else if (theWhat == "Validate_Failed")
		{
		foreachi (i, theResp.Get<ZSeq_Any>("ClientIDs"))
			{
			Transaction* theTransaction = reinterpret_cast<Transaction*>((*i).Get<int64>());

			sEraseSortedMust(
				kDebug_TBRep_Client, fTransactions_Validate_Unacked, theTransaction);

			sInsertSortedMust(kDebug_TBRep_Client, fTransactions_Failed, theTransaction);

			if (theTransaction->fCallback_Validate)
				theTransaction->fCallback_Validate(false, theTransaction->fRefcon);
			}
		}
	else if (theWhat == "Commit_Ack")
		{
		foreachi (i, theResp.Get<ZSeq_Any>("ClientIDs"))
			{
			Transaction* theTransaction = reinterpret_cast<Transaction*>((*i).Get<int64>());

			sEraseSortedMust(
				kDebug_TBRep_Client, fTransactions_Commit_Unacked, theTransaction);

			if (theTransaction->fCallback_Commit)
				theTransaction->fCallback_Commit(theTransaction->fRefcon);
			delete theTransaction;
			}
		}
	else if (theWhat == "GetTuples_Ack")
		{
		foreachi (i, theResp.Get<ZSeq_Any>("Transactions"))
			{
			ZTuple theTransactionTuple = (*i).Get<ZMap_Any>();
			Transaction* theTransaction =
				reinterpret_cast<Transaction*>(theTransactionTuple.Get<int64>("ClientID"));

			foreachi (i, theResp.Get<ZSeq_Any>("IDValues"))
				{
				ZTuple theTuple = (*i).Get<ZMap_Any>();
				uint64 theID = theTuple.GetID("ID");
				TransTuple* theTransTuple = this->pGetTransTuple(theTransaction, theID);
				if (!theTransTuple->fWritten)
					{
					theTransTuple->fValueReturned = true;
					theTransTuple->fValue = theTuple.Get<ZMap_Any>("Value");
					if (theTransTuple->fCallback_GetTuple)
						{
						ZTBRepTransaction::Callback_GetTuple_t theCallback =
							theTransTuple->fCallback_GetTuple;
						void* theRefcon = theTransTuple->fRefcon;
						theTransTuple->fCallback_GetTuple = nullptr;
						theTransTuple->fRefcon = nullptr;
						ZTuple theValue = theTransTuple->fValue;
						theCallback(theRefcon, 1, &theID, &theValue);
						}
					}
				}
			}
		}
	else if (theWhat == "Search_Ack")
		{
		foreachi (i, theResp.Get<ZSeq_Any>("Searches"))
			{
			ZTuple theSearchTuple = (*i).Get<ZMap_Any>();
			TransSearch_t* theSearch =
				reinterpret_cast<TransSearch_t*>(theSearchTuple.Get<int64>("SearchID"));
			Transaction* theTransaction = theSearch->fTransaction;
			theTransaction->fSearches_Waiting.erase(theSearch);

			vector<uint64> vectorResultIDs;
			foreachi (i, theResp.Get<ZSeq_Any>("Results"))
				vectorResultIDs.push_back(i->Get<uint64>());

			theSearch->fCallback(theSearch->fRefcon, vectorResultIDs);
			delete theSearch;
			}
		}
	else if (theWhat == "Count_Ack")
		{
		foreachi (i, theResp.Get<ZSeq_Any>("Counts"))
			{
			ZTuple theCountTuple = (*i).Get<ZMap_Any>();
			TransCount_t* theCount =
				reinterpret_cast<TransCount_t*>(theCountTuple.Get<int64>("CountID"));
			Transaction* theTransaction = theCount->fTransaction;
			theTransaction->fCounts_Waiting.erase(theCount);

			theCount->fCallback(theCount->fRefcon, theCountTuple.Get<int64>("Result"));
			delete theCount;
			}
		}
	else
		{
		if (ZLOG(s, eInfo, "ZTBRep_Client"))
			s << "Unrecognized response";
		}
	}

static void spSend(ZMutexLocker& iLocker, const ZStreamW& iStream, const ZTuple& iTuple)
	{
	iLocker.Release();

	if (kDebug_ShowCommsTuples)
		{
//##		if (ZLOG(s, eDebug, "ZTBRep_Client"))
//##			s << ">> " << iTuple;
		}

	iTuple.ToStream(iStream);
	iLocker.Acquire();
	}

void ZTBRep_Client::pWriter(const ZStreamW& iStream)
	{
	ZMutexLocker locker(fMutex_Structure);
	bool didAnything = false;

	if (fPingRequested)
		{
		fPingRequested = false;
		ZTuple response;
		response.SetString("What", "Pong");
		spSend(locker, iStream, response);
		didAnything = true;
		}

	if (!fPingSent && fTime_LastRead + 10.0 < ZTime::sSystem())
		{
		fPingSent = true;
		ZTuple req;
		req.SetString("What", "Ping");
		spSend(locker, iStream, req);
		didAnything = true;
		}

	if (fIDsNeeded)
		{
		ZTuple reqTuple;
		reqTuple.SetString("What", "AllocateIDs");
		reqTuple.SetInt32("Count", fIDsNeeded);
		fIDsNeeded = 0;
		spSend(locker, iStream, reqTuple);
		didAnything = true;
		}

	if (!fTransactions_Create_Unsent.empty())
		{
		ZTuple reqTuple;
		reqTuple.SetString("What", "Create");

		ZSeq_Any theSeq;
		for (vector<Transaction*>::iterator i = fTransactions_Create_Unsent.begin();
			i != fTransactions_Create_Unsent.end(); /*no inc*/)
			{
			Transaction* theTransaction = *i;
			if (!theTransaction->fLive)
				{
				++i;
				}
			else
				{
				i = fTransactions_Create_Unsent.erase(i);

				theSeq.Append(reinterpret_cast<int64>(theTransaction));
				sInsertSortedMust(kDebug_TBRep_Client,
					fTransactions_Create_Unacked, theTransaction);
				}
			}

		if (theSeq.Count())
			{
			reqTuple.Set("ClientIDs", theSeq);
			spSend(locker, iStream, reqTuple);
			didAnything = true;
			}
		}

	if (!fTransactions_Aborted_Unsent.empty())
		{
		ZTuple reqTuple;
		reqTuple.SetString("What", "Abort");

		ZSeq_Any theSeq;
		for (vector<Transaction*>::iterator i = fTransactions_Aborted_Unsent.begin();
			i != fTransactions_Aborted_Unsent.end(); /*no inc*/)
			{
			Transaction* theTransaction = *i;
			if (!theTransaction->fLive)
				{
				++i;
				}
			else
				{
				i = fTransactions_Aborted_Unsent.erase(i);

				theSeq.Append(int64(theTransaction->fServerID));
				delete theTransaction;
				}
			}

		if (theSeq.Count())
			{
			reqTuple.Set("ServerIDs", theSeq);
			spSend(locker, iStream, reqTuple);
			didAnything = true;
			}
		}

	if (!fTransactions_Validate_Unsent.empty())
		{
		if (fTransactions_NeedWork.empty())
			{
			// We don't send the validate till we've disposed of pending work.
			ZTuple reqTuple;
			reqTuple.SetString("What", "Validate");

			ZSeq_Any theSeq;
			for (vector<Transaction*>::iterator i = fTransactions_Validate_Unsent.begin();
				i != fTransactions_Validate_Unsent.end(); /*no inc*/)
				{
				Transaction* theTransaction = *i;
				if (!theTransaction->fLive)
					{
					++i;
					}
				else
					{
					i = fTransactions_Validate_Unsent.erase(i);

					theSeq.Append(int64(theTransaction->fServerID));
					sInsertSortedMust(kDebug_TBRep_Client,
						fTransactions_Validate_Unacked, theTransaction);
					}
				}

			reqTuple.Set("ServerIDs", theSeq);
			spSend(locker, iStream, reqTuple);
			}
		didAnything = true;
		}

	if (!fTransactions_Commit_Unsent.empty())
		{
		ZTuple reqTuple;
		reqTuple.SetString("What", "Commit");

		ZSeq_Any theSeq;
		for (vector<Transaction*>::iterator i = fTransactions_Commit_Unsent.begin();
			i != fTransactions_Commit_Unsent.end(); /*no inc*/)
			{
			Transaction* theTransaction = *i;
			if (!theTransaction->fLive)
				{
				++i;
				}
			else
				{
				i = fTransactions_Commit_Unsent.erase(i);

				theSeq.Append(int64(theTransaction->fServerID));
				sInsertSortedMust(kDebug_TBRep_Client,
					fTransactions_Commit_Unacked, theTransaction);
				}
			}

		if (theSeq.Count())
			{
			reqTuple.Set("ServerIDs", theSeq);
			spSend(locker, iStream, reqTuple);
			didAnything = true;
			}
		}

	if (!fTransactions_NeedWork.empty())
		{
		for (set<Transaction*>::iterator i = fTransactions_NeedWork.begin();
			i != fTransactions_NeedWork.end(); /*no inc*/)
			{
			Transaction* theTransaction = *i;
			if (!theTransaction->fLive)
				{
				++i;
				continue;
				}
			else
				{
				fTransactions_NeedWork.erase(i);
				}

			ZTuple actionsTuple;
			if (!theTransaction->fTransTuples_NeedWork.empty())
				{
				ZSeq_Any valWrites, valGets;

				for (set<TransTuple*>::iterator i = theTransaction->fTransTuples_NeedWork.begin();
					i != theTransaction->fTransTuples_NeedWork.end(); ++i)
					{
					TransTuple* theTT = *i;
					if (theTT->fWritten)
						{
						if (!theTT->fWriteSent)
							{
							theTT->fWriteSent = true;
							// tell the server about the write.
							ZTuple theWriteTuple;
							theWriteTuple.SetID("ID", theTT->fID);
							theWriteTuple.SetTuple("Value", theTT->fValue);
							valWrites.Append(theWriteTuple);
							}
						}
					else
						{
						if (!theTT->fRequestSent)
							{
							theTT->fRequestSent = true;
							// Ask the server to send the tuple.
							valGets.Append(theTT->fID);
							}
						}
					}

				actionsTuple.SetString("What", "Actions");
				actionsTuple.SetInt64("ServerID", theTransaction->fServerID);
				if (valGets.Count())
					actionsTuple.Set("Gets", valGets);
				if (valWrites.Count())
					actionsTuple.Set("Writes", valWrites);

				theTransaction->fTransTuples_NeedWork.clear();
				}

			ZTuple searchesTuple;
			if (!theTransaction->fSearches_Unsent.empty())
				{
				searchesTuple.SetString("What", "Search");
				searchesTuple.SetInt64("ServerID", theTransaction->fServerID);

				ZSeq_Any theSeq;
				for (vector<TransSearch_t*>::iterator i = theTransaction->fSearches_Unsent.begin();
					i != theTransaction->fSearches_Unsent.end(); ++i)
					{
					TransSearch_t* theSearch = *i;
					ZTuple theTuple;
					theTuple.SetInt64("SearchID", reinterpret_cast<int64>(theSearch));
					theTuple.SetTuple("QueryAsTuple", theSearch->fQueryAsTuple);
					theSeq.Append(theTuple);
					theTransaction->fSearches_Waiting.insert(theSearch);
					}
				searchesTuple.Set("Searches", theSeq);
				theTransaction->fSearches_Unsent.clear();
				}

			ZTuple countsTuple;
			if (!theTransaction->fCounts_Unsent.empty())
				{
				countsTuple.SetString("What", "Count");
				countsTuple.SetInt64("ServerID", theTransaction->fServerID);

				ZSeq_Any theSeq;
				for (vector<TransCount_t*>::iterator i = theTransaction->fCounts_Unsent.begin();
					i != theTransaction->fCounts_Unsent.end(); ++i)
					{
					TransCount_t* theCount = *i;
					ZTuple theTuple;
					theTuple.SetInt64("CountID", reinterpret_cast<int64>(theCount));
					theTuple.SetTuple("QueryAsTuple", theCount->fQueryAsTuple);
					theSeq.Append(theTuple);
					theTransaction->fCounts_Waiting.insert(theCount);
					}
				countsTuple.Set("Counts", theSeq);
				theTransaction->fCounts_Unsent.clear();
				}

			if (not actionsTuple.IsEmpty())
				{
				spSend(locker, iStream, actionsTuple);
				didAnything = true;
				}

			if (not searchesTuple.IsEmpty())
				{
				spSend(locker, iStream, searchesTuple);
				didAnything = true;
				}

			if (not countsTuple.IsEmpty())
				{
				spSend(locker, iStream, countsTuple);
				didAnything = true;
				}

			i = fTransactions_NeedWork.lower_bound(theTransaction);
			}
		}

	if (!didAnything)
		{
		iStream.Flush();
		// Wait for up to two seconds before going round again.
		fCondition_Sender.WaitFor(fMutex_Structure, 2);
		}
	}

} // namespace ZooLib
