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

#include "zoolib/tuplebase/ZTB.h"
#include "zoolib/tuplebase/ZTBServer.h"

#include "zoolib/ZLog.h"
#include "zoolib/ZMACRO_foreach.h"
#include "zoolib/ZStream.h"
#include "zoolib/ZTime.h"
#include "zoolib/ZUtil_STL.h"
#include "zoolib/ZUtil_STL_map.h"
#include "zoolib/ZUtil_STL_set.h"
#include "zoolib/ZUtil_STL_vector.h"

using std::map;
using std::pair;
using std::set;
using std::string;
using std::vector;

namespace ZooLib {

using namespace ZUtil_STL;

#define kDebug_TBServer 1

#ifdef ZCONFIG_TBServer_ShowCommsTuples
#	define kDebug_ShowCommsTuples ZCONFIG_TBServer_ShowCommsTuples
#else
#	define kDebug_ShowCommsTuples 0
#endif

// There should be a couple of side channels. One for handling broadcast messages outside the
// scope of the tuple store (like the admin wants to shut down). The other should be for bulk
// data transfer -- the KBBLOBStream stuff.

// =================================================================================================

static void spDumpRequest(const string& iLogFacility, ZTBServer* iServer, const ZTuple& iTuple)
	{
#if kDebug_ShowCommsTuples
	if (ZLOG(s, eDebug, iLogFacility))
		{
		s.Writef("<< Server: %08X ", iServer);
		s << iTuple;
		}
#endif
	}

static void spSend(void* iConnection, const string& iLogFacility,
	ZMutexLocker& locker, const ZStreamW& iStream, const ZTuple& iTuple)
	{
#if kDebug_ShowCommsTuples
	if (ZLOG(s, eDebug, iLogFacility))
		{
		s.Writef(">> Server: %08X ", iConnection);
		s << iTuple;
		}
#endif
	locker.Release();
	iTuple.ToStream(iStream);
	locker.Acquire();
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZTBServer

class ZTBServer::Transaction
	{
public:
	ZTBServer* fServer;

	uint64 fClientID;
	ZTBRepTransaction* fTBRepTransaction;
	map<uint64, ZTuple> fTuplesToSend_HighPriority;
	map<uint64, ZTuple> fTuplesToSend_LowPriority;
	set<uint64> fTuplesSent;
	};

class ZTBServer::Search_t
	{
public:
	Transaction* fTransaction;
	uint64 fClientSearchID;
	vector<uint64> fResults;
	};

class ZTBServer::Count_t
	{
public:
	Transaction* fTransaction;
	uint64 fClientCountID;
	size_t fResult;
	};

ZTBServer::ZTBServer(
	ZRef<ZTaskMaster> iTaskMaster,
	ZRef<ZStreamerR> iStreamerR, ZRef<ZStreamerW> iStreamerW,
	ZRef<ZTBRep> iTBRep, const std::string& iLogFacility)
:	ZTask(iTaskMaster),
	ZCommer(iStreamerR, iStreamerW),
	fTBRep(iTBRep),
	fSendClose(false),
	fPingRequested(false),
	fPingSent(false),
	fLogFacility(iLogFacility)
	{}

ZTBServer::~ZTBServer()
	{}

bool ZTBServer::Read(const ZStreamR& iStream)
	{
	ZTuple req(iStream);

	spDumpRequest(fLogFacility, this, req);

	ZMutexLocker locker(fMutex_Structure);
	fTime_LastRead = ZTime::sSystem();
	locker.Release();

	string theWhat = req.GetString("What");

	if (false) {}
	else if (theWhat == "AllocateIDs") this->Handle_AllocateIDs(req);
	else if (theWhat == "Create") this->Handle_Create(req);
	else if (theWhat == "Validate") this->Handle_Validate(req);
	else if (theWhat == "Abort") this->Handle_Abort(req);
	else if (theWhat == "Commit") this->Handle_Commit(req);
	else if (theWhat == "Actions") this->Handle_Actions(req);
	else if (theWhat == "Search") this->Handle_Search(req);
	else if (theWhat == "Count") this->Handle_Count(req);

	else if (theWhat == "Ping")
		{
		fPingRequested = true;
		ZStreamerWriter::Wake();
		}
	else if (theWhat == "Pong")
		{
		fPingSent = false;
		}
	else if (theWhat == "Close")
		{
		fSendClose = true;
		ZStreamerWriter::Wake();
		}
	else
		{
		if (ZLOG(s, eErr, fLogFacility))
			s << "Unrecognized request: " << theWhat;
		}
	return true;
	}

bool ZTBServer::Write(const ZStreamW& iStream)
	{
	ZMutexLocker locker(fMutex_Structure);

	bool didAnything = false;

	if (fPingRequested)
		{
		fPingRequested = false;
		ZTuple response;
		response.SetString("What", "Pong");
		spSend(this, fLogFacility, locker, iStream, response);
		didAnything = true;
		}


	if (!fPingSent && fTime_LastRead + 10.0 < ZTime::sSystem())
		{
		fPingSent = true;
		ZTuple response;
		response.SetString("What", "Ping");
		spSend(this, fLogFacility, locker, iStream, response);
		didAnything = true;
		}


	if (!fIDs.empty())
		{
		pair<uint64, size_t> thePair = fIDs.back();
		fIDs.pop_back();

		ZTuple response;
		response.SetString("What", "AllocateIDs_Ack");
		response.SetInt64("BaseID", thePair.first);
		response.SetInt32("Count", thePair.second);
		spSend(this, fLogFacility, locker, iStream, response);
		didAnything = true;
		}


	if (!fTransactions_Create_Unsent.empty())
		{
		ZTuple response;
		response.SetString("What", "Create_Ack");

		ZSeq_Any& vectorIDs = response.Mut<ZSeq_Any>("IDs");
		for (vector<Transaction*>::iterator i = fTransactions_Create_Unsent.begin();
			i != fTransactions_Create_Unsent.end(); ++i)
			{
			Transaction* theTransaction = *i;
			ZTuple theTuple;
			theTuple.SetInt64("ClientID", theTransaction->fClientID);
			theTuple.SetInt64("ServerID", reinterpret_cast<int64>(theTransaction));
			vectorIDs.Append(theTuple);
			sInsertSortedMust(kDebug_TBServer, fTransactions_Created, theTransaction);
			}
		fTransactions_Create_Unsent.clear();

		spSend(this, fLogFacility, locker, iStream, response);
		didAnything = true;
		}


	if (!fTransactions_Validate_Succeeded.empty())
		{
		ZTuple response;
		response.SetString("What", "Validate_Succeeded");

		ZSeq_Any& vectorClientIDs = response.Mut<ZSeq_Any>("ClientIDs");
		for (vector<Transaction*>::iterator i = fTransactions_Validate_Succeeded.begin();
			i != fTransactions_Validate_Succeeded.end(); ++i)
			{
			Transaction* theTransaction = *i;
			vectorClientIDs.Append(int64(theTransaction->fClientID));
			sInsertSortedMust(kDebug_TBServer, fTransactions_Validated, theTransaction);
			}
		fTransactions_Validate_Succeeded.clear();

		spSend(this, fLogFacility, locker, iStream, response);
		didAnything = true;
		}


	if (!fTransactions_Validate_Failed.empty())
		{
		ZTuple response;
		response.SetString("What", "Validate_Failed");

		ZSeq_Any& vectorClientIDs = response.Mut<ZSeq_Any>("ClientIDs");
		for (vector<Transaction*>::iterator i = fTransactions_Validate_Failed.begin();
			i != fTransactions_Validate_Failed.end(); ++i)
			{
			Transaction* theTransaction = *i;
			vectorClientIDs.Append(int64(theTransaction->fClientID));
			fTransactions_HaveTuplesToSend.erase(theTransaction);
			theTransaction->fTBRepTransaction->AcceptFailure();
			// AcceptFailure deletes ZTBRepTransaction
			}
		fTransactions_Validate_Failed.clear();

		spSend(this, fLogFacility, locker, iStream, response);
		didAnything = true;
		}


	if (!fTransactions_Commit_Acked.empty())
		{
		ZTuple response;
		response.SetString("What", "Commit_Ack");

		ZSeq_Any& vectorClientIDs = response.Mut<ZSeq_Any>("ClientIDs");
		for (vector<Transaction*>::iterator i = fTransactions_Commit_Acked.begin();
			i != fTransactions_Commit_Acked.end(); ++i)
			{
			Transaction* theTransaction = *i;
			vectorClientIDs.Append(int64(theTransaction->fClientID));
			fTransactions_HaveTuplesToSend.erase(theTransaction);
			delete theTransaction;
			}
		fTransactions_Commit_Acked.clear();

		spSend(this, fLogFacility, locker, iStream, response);
		didAnything = true;
		}


	if (!fTransactions_HaveTuplesToSend.empty())
		{
		// Do the high priority ones first.
		ZTuple response;
		response.SetString("What", "GetTuples_Ack");

		bool allEmptied = true;
		bool sentAny = false;
		ZSeq_Any& vectorTransactionTuples = response.Mut<ZSeq_Any>("Transactions");

		for (set<Transaction*>::iterator i = fTransactions_HaveTuplesToSend.begin();
			i != fTransactions_HaveTuplesToSend.end(); ++i)
			{
			Transaction* theTransaction = *i;
			if (!theTransaction->fTuplesToSend_HighPriority.empty())
				{
				ZTuple theTransactionTuple;
				theTransactionTuple.SetInt64("ClientID", theTransaction->fClientID);
				ZSeq_Any& vectorTuples =
					theTransactionTuple.Mut<ZSeq_Any>("IDValues");

				for (map<uint64, ZTuple>::iterator
					j = theTransaction->fTuplesToSend_HighPriority.begin();
					j != theTransaction->fTuplesToSend_HighPriority.end(); ++j)
					{
					ZTuple aTuple;
					aTuple.SetID("ID", (*j).first);
					aTuple.SetTuple("Value", (*j).second);
					vectorTuples.Append(aTuple);

					// Remove it from our low priority list too
					// Actually, this shouldn't be necessary. fTuplesSent should
					// stop this from happening. Probably should make this an assertion.
					sQErase(theTransaction->fTuplesToSend_LowPriority, (*j).first);
					}
				theTransaction->fTuplesToSend_HighPriority.clear();
				vectorTransactionTuples.Append(theTransactionTuple);
				sentAny = true;
				}
			if (!theTransaction->fTuplesToSend_LowPriority.empty())
				allEmptied = false;
			}
		if (allEmptied)
			fTransactions_HaveTuplesToSend.clear();

		if (sentAny)
			{
			spSend(this, fLogFacility, locker, iStream, response);
			didAnything = true;
			}
		}


	if (!fSearches_Unsent.empty())
		{
		ZTuple response;
		response.SetString("What", "Search_Ack");

		ZSeq_Any& vectorSearches = response.Mut<ZSeq_Any>("Searches");
		for (vector<Search_t*>::iterator i = fSearches_Unsent.begin();
			i != fSearches_Unsent.end(); ++i)
			{
			Search_t* theSearch = *i;
			ZTuple theTuple;
			theTuple.SetInt64("SearchID", theSearch->fClientSearchID);
			foreacha (aa, theSearch->fResults)
				theTuple.Mut<ZSeq_Any>("Results").Append(aa);

			vectorSearches.Append(theTuple);
			delete theSearch;
			}
		fSearches_Unsent.clear();

		spSend(this, fLogFacility, locker, iStream, response);
		didAnything = true;
		}


	if (!fCounts_Unsent.empty())
		{
		ZTuple response;
		response.SetString("What", "Count_Ack");

		ZSeq_Any& vectorCounts = response.Mut<ZSeq_Any>("Counts");
		for (vector<Count_t*>::iterator i = fCounts_Unsent.begin();
			i != fCounts_Unsent.end(); ++i)
			{
			Count_t* theCount = *i;
			ZTuple theTuple;
			theTuple.SetInt64("CountID", theCount->fClientCountID);
			theTuple.SetInt64("Result", theCount->fResult);
			vectorCounts.Append(theTuple);
			delete theCount;
			}
		fCounts_Unsent.clear();

		spSend(this, fLogFacility, locker, iStream, response);
		didAnything = true;
		}


	if (!didAnything && !fTransactions_HaveTuplesToSend.empty())
		{
		// We didn't do anything else on this loop, and we may have
		// low priority tuples to send.
		ZTuple response;
		response.SetString("What", "GetTuples_Ack");
		response.SetString("extra", "low priority");

		bool allEmptied = true;
		ZSeq_Any& vectorTransactionTuples = response.Mut<ZSeq_Any>("Transactions");

		for (set<Transaction*>::iterator i = fTransactions_HaveTuplesToSend.begin();
			i != fTransactions_HaveTuplesToSend.end(); ++i)
			{
			Transaction* theTransaction = *i;
			if (!theTransaction->fTuplesToSend_LowPriority.empty())
				{
				ZTuple theTransactionTuple;
				theTransactionTuple.SetInt64("ClientID", theTransaction->fClientID);
				ZSeq_Any& vectorTuples =
					theTransactionTuple.Mut<ZSeq_Any>("IDValues");

				for (map<uint64, ZTuple>::iterator
					i = theTransaction->fTuplesToSend_LowPriority.begin();
					i != theTransaction->fTuplesToSend_LowPriority.end(); ++i)
					{
					ZTuple aTuple;
					aTuple.SetID("ID", (*i).first);
					aTuple.SetTuple("Value", (*i).second);
					vectorTuples.Append(aTuple);
					}
				theTransaction->fTuplesToSend_LowPriority.clear();
				vectorTransactionTuples.Append(theTransactionTuple);
				}
			if (!theTransaction->fTuplesToSend_HighPriority.empty())
				allEmptied = false;
			}
		if (allEmptied)
			fTransactions_HaveTuplesToSend.clear();

		spSend(this, fLogFacility, locker, iStream, response);
		didAnything = true;
		}

	if (fSendClose)
		{
		ZTuple response;
		response.SetString("What", "Close");
		spSend(this, fLogFacility, locker, iStream, response);
		}

	if (!didAnything)
		iStream.Flush();
	else
		ZStreamerWriter::Wake();

	return !fSendClose;
	}

void ZTBServer::Finished()
	{
	ZTask::pFinished();
	}

void ZTBServer::spCallback_AllocateIDs(void* iRefcon, uint64 iBaseID, size_t iCount)
	{
	ZTBServer* theServer = static_cast<ZTBServer*>(iRefcon);
	ZMutexLocker locker(theServer->fMutex_Structure);
	theServer->fIDs.push_back(pair<uint64, size_t>(iBaseID, iCount));
	theServer->ZStreamerWriter::Wake();
	}

void ZTBServer::Handle_AllocateIDs(const ZTuple& iReq)
	{
	fTBRep->AllocateIDs(iReq.GetInt32("Count"), spCallback_AllocateIDs, this);
	}

void ZTBServer::Handle_Create(const ZTuple& iReq)
	{
	ZMutexLocker locker(fMutex_Structure);

	foreachi (i, iReq.Get<ZSeq_Any>("ClientIDs"))
		{
		locker.Release();

		Transaction* theTransaction = new Transaction;
		theTransaction->fServer = this;
		theTransaction->fClientID = (*i).Get<int64>();
		theTransaction->fTBRepTransaction = fTBRep->CreateTransaction();

		locker.Acquire();

		sInsertSortedMust(kDebug_TBServer, fTransactions_Create_Unsent, theTransaction);

		ZStreamerWriter::Wake();
		}
	}

void ZTBServer::spCallback_GetTupleForSearch(
	void* iRefcon, size_t iCount, const uint64* iIDs, const ZTuple* iTuples)
	{
	if (iCount)
		{
		Transaction* theTransaction = static_cast<Transaction*>(iRefcon);
		ZMutexLocker locker(theTransaction->fServer->fMutex_Structure);

		while (iCount--)
			{
			uint64 theID = *iIDs++;
			const ZTuple& theTuple = *iTuples++;
			if (not sContains(theTransaction->fTuplesSent, theID))
				{
				theTransaction->fTuplesSent.insert(theID);
				theTransaction->fTuplesToSend_LowPriority.insert(
					pair<uint64, ZTuple>(theID, theTuple));
				}
			}

		theTransaction->fServer->fTransactions_HaveTuplesToSend.insert(theTransaction);
		theTransaction->fServer->ZStreamerWriter::Wake();
		}
	}

void ZTBServer::spCallback_Search(void* iRefcon, vector<uint64>& ioResults)
	{
	Search_t* theSearch = static_cast<Search_t*>(iRefcon);
	Transaction* theTransaction = theSearch->fTransaction;

	ZMutexLocker locker(theTransaction->fServer->fMutex_Structure);

	// Just copy the results (rather than swapping them, as before), because we're going to be using
	// them below, outside the scope of the mutex and by which time the search could be disposed.
	theSearch->fResults = ioResults;

	sEraseSortedMust(kDebug_TBServer, theTransaction->fServer->fSearches_Waiting, theSearch);
	sInsertSortedMust(kDebug_TBServer,
		theTransaction->fServer->fSearches_Unsent, theSearch);
	theTransaction->fServer->ZStreamerWriter::Wake();

	locker.Release();

	if (ioResults.size())
		{
		theTransaction->fTBRepTransaction->GetTuples(ioResults.size(), &ioResults[0],
			spCallback_GetTupleForSearch, theTransaction);
		}
	}

void ZTBServer::Handle_Search(const ZTuple& iReq)
	{
	ZMutexLocker locker(fMutex_Structure);

	Transaction* theTransaction = reinterpret_cast<Transaction*>(iReq.Get<int64>("ServerID"));
	ZAssert(theTransaction->fServer == this);

	foreachi (i, iReq.Get<ZSeq_Any>("Searches"))
		{
		ZTuple t = (*i).Get<ZMap_Any>();

		Search_t* theSearch = new Search_t;
		theSearch->fTransaction = theTransaction;
		theSearch->fClientSearchID = t.Get<int64>("SearchID");
		ZTBQuery theQuery(t.Get<ZMap_Any>("QueryAsTuple"));
		sInsertSortedMust(kDebug_TBServer, fSearches_Waiting, theSearch);
		theTransaction->fTBRepTransaction->Search(theQuery, spCallback_Search, theSearch);
		}
	}

void ZTBServer::spCallback_Count(void* iRefcon, size_t iResult)
	{
	Count_t* theCount = static_cast<Count_t*>(iRefcon);
	Transaction* theTransaction = theCount->fTransaction;

	ZMutexLocker locker(theTransaction->fServer->fMutex_Structure);

	theCount->fResult = iResult;

	sEraseSortedMust(kDebug_TBServer, theTransaction->fServer->fCounts_Waiting, theCount);
	sInsertSortedMust(kDebug_TBServer, theTransaction->fServer->fCounts_Unsent, theCount);
	theTransaction->fServer->ZStreamerWriter::Wake();

	locker.Release();
	}

void ZTBServer::Handle_Count(const ZTuple& iReq)
	{
	ZMutexLocker locker(fMutex_Structure);

	Transaction* theTransaction = reinterpret_cast<Transaction*>(iReq.Get<int64>("ServerID"));
	ZAssert(theTransaction->fServer == this);

	foreachi (i, iReq.Get<ZSeq_Any>("Counts"))
		{
		ZTuple t = (*i).Get<ZMap_Any>();

		Count_t* theCount = new Count_t;
		theCount->fTransaction = theTransaction;
		theCount->fClientCountID = t.Get<int64>("CountID");
		ZTBQuery theQuery(t.Get<ZMap_Any>("QueryAsTuple"));
		sInsertSortedMust(kDebug_TBServer, fCounts_Waiting, theCount);
		theTransaction->fTBRepTransaction->Count(theQuery, spCallback_Count, theCount);
		}
	}

void ZTBServer::spCallback_Validate(bool iSucceeded, void* iRefcon)
	{
	Transaction* theTransaction = static_cast<Transaction*>(iRefcon);
	ZMutexLocker locker(theTransaction->fServer->fMutex_Structure);

	sEraseSortedMust(kDebug_TBServer,
		theTransaction->fServer->fTransactions_Validate_Waiting, theTransaction);

	if (iSucceeded)
		{
		sInsertSortedMust(kDebug_TBServer,
			theTransaction->fServer->fTransactions_Validate_Succeeded, theTransaction);
		}
	else
		{
		sInsertSortedMust(kDebug_TBServer,
			theTransaction->fServer->fTransactions_Validate_Failed, theTransaction);
		}

	theTransaction->fServer->ZStreamerWriter::Wake();
	}

void ZTBServer::Handle_Validate(const ZTuple& iReq)
	{
	ZMutexLocker locker(fMutex_Structure);

	foreachi (i, iReq.Get<ZSeq_Any>("ServerIDs"))
		{
		Transaction* theTransaction = reinterpret_cast<Transaction*>((*i).Get<int64>());

		sEraseSortedMust(kDebug_TBServer, fTransactions_Created, theTransaction);

		sInsertSortedMust(kDebug_TBServer,
			fTransactions_Validate_Waiting, theTransaction);

		locker.Release();
		theTransaction->fTBRepTransaction->Validate(spCallback_Validate, theTransaction);
		locker.Acquire();
		}
	}

void ZTBServer::Handle_Abort(const ZTuple& iReq)
	{
	ZMutexLocker locker(fMutex_Structure);

	foreachi (i, iReq.Get<ZSeq_Any>("ServerIDs"))
		{
		Transaction* theTransaction = reinterpret_cast<Transaction*>((*i).Get<int64>());
		// theTransaction must either be created or validated.
		if (sQEraseSorted(fTransactions_Created, theTransaction))
			{
			theTransaction->fTBRepTransaction->AbortPreValidate();
			}
		else if (sQEraseSorted(fTransactions_Validated, theTransaction))
			{
			theTransaction->fTBRepTransaction->CancelPostValidate();
			}
		else
			{
			ZDebugStop(kDebug_TBServer);
			}
		fTransactions_HaveTuplesToSend.erase(theTransaction);
		delete theTransaction;
		}
	}

void ZTBServer::spCallback_Commit(void* iRefcon)
	{
	Transaction* theTransaction = static_cast<Transaction*>(iRefcon);
	ZMutexLocker locker(theTransaction->fServer->fMutex_Structure);

	sEraseSortedMust(kDebug_TBServer,
		theTransaction->fServer->fTransactions_Commit_Waiting, theTransaction);

	sInsertSortedMust(kDebug_TBServer,
		theTransaction->fServer->fTransactions_Commit_Acked, theTransaction);

	theTransaction->fServer->ZStreamerWriter::Wake();
	}

void ZTBServer::Handle_Commit(const ZTuple& iReq)
	{
	ZMutexLocker locker(fMutex_Structure);

	foreachi (i, iReq.Get<ZSeq_Any>("ServerIDs"))
		{
		Transaction* theTransaction = reinterpret_cast<Transaction*>((*i).Get<int64>());
		sEraseSortedMust(kDebug_TBServer, fTransactions_Validated, theTransaction);
		sInsertSortedMust(kDebug_TBServer, fTransactions_Commit_Waiting, theTransaction);
		locker.Release();
		theTransaction->fTBRepTransaction->Commit(spCallback_Commit, theTransaction);
		locker.Acquire();
		}
	}

void ZTBServer::spCallback_GetTuple(
	void* iRefcon, size_t iCount, const uint64* iIDs, const ZTuple* iTuples)
	{
	if (iCount)
		{
		Transaction* theTransaction = static_cast<Transaction*>(iRefcon);
		ZMutexLocker locker(theTransaction->fServer->fMutex_Structure);

		while (iCount--)
			{
			uint64 theID = *iIDs++;
			if (not ZUtil_STL::sContains(theTransaction->fTuplesSent, theID))
				{
				theTransaction->fTuplesSent.insert(theID);
				const ZTuple& theTuple = *iTuples++;
				theTransaction->fTuplesToSend_HighPriority.insert(
					pair<uint64, ZTuple>(theID, theTuple));
				}
			}

		theTransaction->fServer->fTransactions_HaveTuplesToSend.insert(theTransaction);
		theTransaction->fServer->ZStreamerWriter::Wake();
		}
	}

void ZTBServer::Handle_Actions(const ZTuple& iReq)
	{
	Transaction* theTransaction = reinterpret_cast<Transaction*>(iReq.Get<int64>("ServerID"));

	vector<uint64> vectorGets;
	foreachi (i, iReq.Get<ZSeq_Any>("Gets"))
		vectorGets.push_back(i->Get<uint64>());

	theTransaction->fTBRepTransaction->GetTuples(
		vectorGets.size(), &vectorGets[0], spCallback_GetTuple, theTransaction);

	foreachi (i, iReq.Get<ZSeq_Any>("Writes"))
		{
		const ZTuple& t = (*i).Get<ZMap_Any>();
		theTransaction->fTBRepTransaction->SetTuple(t.GetID("ID"), t.Get<ZMap_Any>("Value"));
		}
	}

template <class T>
void sDeleteEraseAll(T& ioContainer)
	{
	ZUtil_STL::sDeleteAll(ioContainer.begin(), ioContainer.end());
	ioContainer.clear();
	}

void ZTBServer::TearDown()
	{
	ZAssertLocked(kDebug_TBServer, fMutex_Structure);

	// First we need to wait for all outstanding searches to complete and
	// be moved into fSearches_Unsent.
	while (!fSearches_Waiting.empty())
		fCondition_Sender.Wait(fMutex_Structure);

	// We can now delete everything in fSearches_Unsent.
	sDeleteEraseAll(fSearches_Unsent);

	// And the same again for counts
	while (!fCounts_Waiting.empty())
		fCondition_Sender.Wait(fMutex_Structure);

	// We can now delete everything in fCounts_Unsent.
	sDeleteEraseAll(fCounts_Unsent);

	// Abort created transactions that have not been sent to the client.
	for (vector<Transaction*>::iterator i = fTransactions_Create_Unsent.begin();
		i != fTransactions_Create_Unsent.end(); ++i)
		{
		(*i)->fTBRepTransaction->AbortPreValidate();
		delete *i;
		}
	fTransactions_Create_Unsent.clear();

	// Abort created transactions that have been sent to the client.
	for (vector<Transaction*>::iterator i = fTransactions_Created.begin();
		i != fTransactions_Created.end(); ++i)
		{
		(*i)->fTBRepTransaction->AbortPreValidate();
		delete *i;
		}
	fTransactions_Created.clear();

	// Wait for any validating transactions to complete and be moved to
	// fTransactions_Validate_Succeeded or fTransactions_Validate_Failed.
	while (!fTransactions_Validate_Waiting.empty())
		fCondition_Sender.Wait(fMutex_Structure);

	// Cancel validated transactions that succeeded but had not been sent to the client.
	for (vector<Transaction*>::iterator i = fTransactions_Validate_Succeeded.begin();
		i != fTransactions_Validate_Succeeded.end(); ++i)
		{
		(*i)->fTBRepTransaction->CancelPostValidate();
		delete *i;
		}
	fTransactions_Validate_Succeeded.clear();

	// Abort validated transactions that succeeded and had been sent to the client.
	for (vector<Transaction*>::iterator i = fTransactions_Validated.begin();
		i != fTransactions_Validated.end(); ++i)
		{
		(*i)->fTBRepTransaction->CancelPostValidate();
		delete *i;
		}
	fTransactions_Validated.clear();

	// Accept failure from validated transactions that failed but
	// had not been sent to the client.
	for (vector<Transaction*>::iterator i = fTransactions_Validate_Failed.begin();
		i != fTransactions_Validate_Failed.end(); ++i)
		{
		(*i)->fTBRepTransaction->AcceptFailure();
		delete *i;
		}
	fTransactions_Validate_Failed.clear();

	// We need to wait for outstanding calls to Commit to complete and be moved
	// into fTransactions_Commit_Acked.
	while (!fTransactions_Commit_Waiting.empty())
		fCondition_Sender.Wait(fMutex_Structure);

	// Delete committed transactions that had not been sent to the client.
	sDeleteEraseAll(fTransactions_Commit_Acked);
	}

} // namespace ZooLib
