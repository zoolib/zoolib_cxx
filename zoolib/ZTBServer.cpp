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

#include "zoolib/ZTBServer.h"

#include "zoolib/ZLog.h"
#include "zoolib/ZStream.h"
#include "zoolib/ZTB.h"
#include "zoolib/ZTime.h"

#include "zoolib/ZUtil_STL.h" // For sSortedEraseMustContain etc
using namespace ZUtil_STL;

#include "zoolib/ZUtil_Strim_Tuple.h"

using std::map;
using std::pair;
using std::set;
using std::string;
using std::vector;

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

ZTBServer::ZTBServer(ZTB iTB)
:	fTBRep(iTB.GetTBRep()),
	fReaderExited(false),
	fWriterExited(false),
	fPingRequested(false),
	fPingSent(false),
	fLogFacility("ZTBServer")
	{}

ZTBServer::ZTBServer(ZTB iTB, const string& iLogFacility)
:	fTBRep(iTB.GetTBRep()),
	fReaderExited(false),
	fWriterExited(false),
	fPingRequested(false),
	fPingSent(false),
	fLogFacility(iLogFacility)
	{}

ZTBServer::~ZTBServer()
	{
	fMutex_Structure.Acquire();
	while (!fWriterExited ||!fReaderExited)
		fCondition_Sender.Wait(fMutex_Structure);

	ZAssertStop(kDebug_TBServer, fTransactions_Create_Unsent.empty());
	ZAssertStop(kDebug_TBServer, fTransactions_Created.empty());

	ZAssertStop(kDebug_TBServer, fTransactions_Validate_Waiting.empty());
	ZAssertStop(kDebug_TBServer, fTransactions_Validate_Succeeded.empty());
	ZAssertStop(kDebug_TBServer, fTransactions_Validate_Failed.empty());
	ZAssertStop(kDebug_TBServer, fTransactions_Validated.empty());

	ZAssertStop(kDebug_TBServer, fTransactions_Commit_Waiting.empty());
	ZAssertStop(kDebug_TBServer, fTransactions_Commit_Acked.empty());
	}

void ZTBServer::RunReader(const ZStreamR& iStream)
	{
	try
		{
		this->Reader(iStream);
		}
	catch (...)
		{}

	fMutex_Structure.Acquire();
	if (fWriterExited)
		this->TearDown();

	fReaderExited = true;
	fCondition_Sender.Broadcast();
	fMutex_Structure.Release();
	}

void ZTBServer::RunWriter(const ZStreamW& iStreamW)
	{
	try
		{
		this->Writer(iStreamW);
		}
	catch (...)
		{}

	fMutex_Structure.Acquire();
	if (fReaderExited)
		this->TearDown();

	fWriterExited = true;
	fCondition_Sender.Broadcast();
	fMutex_Structure.Release();
	}

void ZTBServer::sCallback_AllocateIDs(void* iRefcon, uint64 iBaseID, size_t iCount)
	{
	ZTBServer* theServer = static_cast<ZTBServer*>(iRefcon);
	ZMutexLocker locker(theServer->fMutex_Structure);
	theServer->fIDs.push_back(pair<uint64, size_t>(iBaseID, iCount));
	theServer->fCondition_Sender.Broadcast();
	}

void ZTBServer::Handle_AllocateIDs(const ZTuple& iReq)
	{
	fTBRep->AllocateIDs(iReq.GetInt32("Count"), sCallback_AllocateIDs, this);
	}

void ZTBServer::Handle_Create(const ZTuple& iReq)
	{
	ZMutexLocker locker(fMutex_Structure);

	const vector<ZTValue>& vectorClientIDs = iReq.GetVector("ClientIDs");
	for (vector<ZTValue>::const_iterator i = vectorClientIDs.begin();
		i != vectorClientIDs.end(); ++i)
		{
		locker.Release();

		Transaction* theTransaction = new Transaction;
		theTransaction->fServer = this;
		theTransaction->fClientID = (*i).GetInt64();
		theTransaction->fTBRepTransaction = fTBRep->CreateTransaction();

		locker.Acquire();

		sSortedInsertMustNotContain(kDebug_TBServer, fTransactions_Create_Unsent, theTransaction);

		fCondition_Sender.Broadcast();
		}
	}

void ZTBServer::sCallback_GetTupleForSearch(
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
			if (!ZUtil_STL::sContains(theTransaction->fTuplesSent, theID))
				{
				theTransaction->fTuplesSent.insert(theID);
				theTransaction->fTuplesToSend_LowPriority.insert(
					pair<uint64, ZTuple>(theID, theTuple));
				}
			}

		theTransaction->fServer->fTransactions_HaveTuplesToSend.insert(theTransaction);
		theTransaction->fServer->fCondition_Sender.Broadcast();
		}
	}

void ZTBServer::sCallback_Search(void* iRefcon, vector<uint64>& ioResults)
	{
	Search_t* theSearch = static_cast<Search_t*>(iRefcon);
	Transaction* theTransaction = theSearch->fTransaction;

	ZMutexLocker locker(theTransaction->fServer->fMutex_Structure);

	// Just copy the results (rather than swapping them, as before), because we're going to be using
	// them below, outside the scope of the mutex and by which time the search could be disposed.
	theSearch->fResults = ioResults;

	sSortedEraseMustContain(kDebug_TBServer, theTransaction->fServer->fSearches_Waiting, theSearch);
	sSortedInsertMustNotContain(kDebug_TBServer,
		theTransaction->fServer->fSearches_Unsent, theSearch);
	theTransaction->fServer->fCondition_Sender.Broadcast();

	locker.Release();

	if (ioResults.size())
		{
		theTransaction->fTBRepTransaction->GetTuples(ioResults.size(), &ioResults[0],
			sCallback_GetTupleForSearch, theTransaction);
		}
	}

void ZTBServer::Handle_Search(const ZTuple& iReq)
	{
	ZMutexLocker locker(fMutex_Structure);

	Transaction* theTransaction = reinterpret_cast<Transaction*>(iReq.GetInt64("ServerID"));
	ZAssert(theTransaction->fServer == this);

	const vector<ZTValue>& vectorSearches = iReq.GetVector("Searches");
	for (vector<ZTValue>::const_iterator i = vectorSearches.begin();
		i != vectorSearches.end(); ++i)
		{
		ZTuple t = (*i).GetTuple();

		Search_t* theSearch = new Search_t;
		theSearch->fTransaction = theTransaction;
		theSearch->fClientSearchID = t.GetInt64("SearchID");
		ZTBQuery theQuery(t.GetTuple("QueryAsTuple"));
		sSortedInsertMustNotContain(kDebug_TBServer, fSearches_Waiting, theSearch);
		theTransaction->fTBRepTransaction->Search(theQuery, sCallback_Search, theSearch);
		}
	}

void ZTBServer::sCallback_Count(void* iRefcon, size_t iResult)
	{
	Count_t* theCount = static_cast<Count_t*>(iRefcon);
	Transaction* theTransaction = theCount->fTransaction;

	ZMutexLocker locker(theTransaction->fServer->fMutex_Structure);

	theCount->fResult = iResult;

	sSortedEraseMustContain(kDebug_TBServer, theTransaction->fServer->fCounts_Waiting, theCount);
	sSortedInsertMustNotContain(kDebug_TBServer, theTransaction->fServer->fCounts_Unsent, theCount);
	theTransaction->fServer->fCondition_Sender.Broadcast();

	locker.Release();
	}

void ZTBServer::Handle_Count(const ZTuple& iReq)
	{
	ZMutexLocker locker(fMutex_Structure);

	Transaction* theTransaction = reinterpret_cast<Transaction*>(iReq.GetInt64("ServerID"));
	ZAssert(theTransaction->fServer == this);

	const vector<ZTValue>& vectorCounts = iReq.GetVector("Counts");
	for (vector<ZTValue>::const_iterator i = vectorCounts.begin(); i != vectorCounts.end(); ++i)
		{
		ZTuple t = (*i).GetTuple();

		Count_t* theCount = new Count_t;
		theCount->fTransaction = theTransaction;
		theCount->fClientCountID = t.GetInt64("CountID");
		ZTBQuery theQuery(t.GetTuple("QueryAsTuple"));
		sSortedInsertMustNotContain(kDebug_TBServer, fCounts_Waiting, theCount);
		theTransaction->fTBRepTransaction->Count(theQuery, sCallback_Count, theCount);
		}
	}

void ZTBServer::sCallback_Validate(bool iSucceeded, void* iRefcon)
	{
	Transaction* theTransaction = static_cast<Transaction*>(iRefcon);
	ZMutexLocker locker(theTransaction->fServer->fMutex_Structure);

	sSortedEraseMustContain(kDebug_TBServer,
		theTransaction->fServer->fTransactions_Validate_Waiting, theTransaction);

	if (iSucceeded)
		{
		sSortedInsertMustNotContain(kDebug_TBServer,
			theTransaction->fServer->fTransactions_Validate_Succeeded, theTransaction);
		}
	else
		{
		sSortedInsertMustNotContain(kDebug_TBServer,
			theTransaction->fServer->fTransactions_Validate_Failed, theTransaction);
		}

	theTransaction->fServer->fCondition_Sender.Broadcast();
	}

void ZTBServer::Handle_Validate(const ZTuple& iReq)
	{
	ZMutexLocker locker(fMutex_Structure);

	const vector<ZTValue>& vectorServerIDs = iReq.GetVector("ServerIDs");
	for (vector<ZTValue>::const_iterator i = vectorServerIDs.begin();
		i != vectorServerIDs.end(); ++i)
		{
		Transaction* theTransaction = reinterpret_cast<Transaction*>((*i).GetInt64());

		sSortedEraseMustContain(kDebug_TBServer, fTransactions_Created, theTransaction);

		sSortedInsertMustNotContain(kDebug_TBServer,
			fTransactions_Validate_Waiting, theTransaction);

		locker.Release();
		theTransaction->fTBRepTransaction->Validate(sCallback_Validate, theTransaction);
		locker.Acquire();
		}
	}

void ZTBServer::Handle_Abort(const ZTuple& iReq)
	{
	ZMutexLocker locker(fMutex_Structure);

	const vector<ZTValue>& vectorServerIDs = iReq.GetVector("ServerIDs");
	for (vector<ZTValue>::const_iterator i = vectorServerIDs.begin();
		i != vectorServerIDs.end(); ++i)
		{
		Transaction* theTransaction = reinterpret_cast<Transaction*>((*i).GetInt64());
		// theTransaction must either be created or validated.
		if (sSortedEraseIfContains(fTransactions_Created, theTransaction))
			{
			theTransaction->fTBRepTransaction->AbortPreValidate();
			}
		else if (sSortedEraseIfContains(fTransactions_Validated, theTransaction))
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

void ZTBServer::sCallback_Commit(void* iRefcon)
	{
	Transaction* theTransaction = static_cast<Transaction*>(iRefcon);
	ZMutexLocker locker(theTransaction->fServer->fMutex_Structure);

	sSortedEraseMustContain(kDebug_TBServer,
		theTransaction->fServer->fTransactions_Commit_Waiting, theTransaction);

	sSortedInsertMustNotContain(kDebug_TBServer,
		theTransaction->fServer->fTransactions_Commit_Acked, theTransaction);

	theTransaction->fServer->fCondition_Sender.Broadcast();
	}

void ZTBServer::Handle_Commit(const ZTuple& iReq)
	{
	ZMutexLocker locker(fMutex_Structure);

	const vector<ZTValue>& vectorServerIDs = iReq.GetVector("ServerIDs");
	for (vector<ZTValue>::const_iterator i = vectorServerIDs.begin();
		i != vectorServerIDs.end(); ++i)
		{
		Transaction* theTransaction = reinterpret_cast<Transaction*>((*i).GetInt64());
		sSortedEraseMustContain(kDebug_TBServer, fTransactions_Validated, theTransaction);
		sSortedInsertMustNotContain(kDebug_TBServer, fTransactions_Commit_Waiting, theTransaction);
		locker.Release();
		theTransaction->fTBRepTransaction->Commit(sCallback_Commit, theTransaction);
		locker.Acquire();
		}
	}

void ZTBServer::sCallback_GetTuple(
	void* iRefcon, size_t iCount, const uint64* iIDs, const ZTuple* iTuples)
	{
	if (iCount)
		{
		Transaction* theTransaction = static_cast<Transaction*>(iRefcon);
		ZMutexLocker locker(theTransaction->fServer->fMutex_Structure);

		while (iCount--)
			{
			uint64 theID = *iIDs++;
			if (!ZUtil_STL::sContains(theTransaction->fTuplesSent, theID))
				{
				theTransaction->fTuplesSent.insert(theID);
				const ZTuple& theTuple = *iTuples++;
				theTransaction->fTuplesToSend_HighPriority.insert(
					pair<uint64, ZTuple>(theID, theTuple));
				}
			}

		theTransaction->fServer->fTransactions_HaveTuplesToSend.insert(theTransaction);
		theTransaction->fServer->fCondition_Sender.Broadcast();
		}
	}

void ZTBServer::Handle_Actions(const ZTuple& iReq)
	{
	Transaction* theTransaction = reinterpret_cast<Transaction*>(iReq.GetInt64("ServerID"));

	vector<uint64> vectorGets;
	iReq.GetVector_T("Gets", back_inserter(vectorGets), uint64());
	theTransaction->fTBRepTransaction->GetTuples(
		vectorGets.size(), &vectorGets[0], sCallback_GetTuple, theTransaction);

	const vector<ZTValue>& vectorWrites = iReq.GetVector("Writes");
	for (vector<ZTValue>::const_iterator i = vectorWrites.begin(); i != vectorWrites.end(); ++i)
		{
		const ZTuple& t = (*i).GetTuple();
		theTransaction->fTBRepTransaction->SetTuple(t.GetID("ID"), t.GetTuple("Value"));
		}
	}

static void sDumpRequest(const string& iLogFacility, ZTBServer* iServer, const ZTuple& iTuple)
	{
#if kDebug_ShowCommsTuples
	if (ZLOG(s, eDebug, iLogFacility))
		{
		s.Writef("<< Server: %08X ", iServer);
		ZUtil_Strim_Tuple::sToStrim(s, iTuple);
		}
#endif
	}

void ZTBServer::Reader(const ZStreamR& iStream)
	{
	for (;;)
		{
		ZTuple req(iStream);
		
		sDumpRequest(fLogFacility, this, req);

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
			fCondition_Sender.Broadcast();
			}
		else if (theWhat == "Pong")
			{
			fPingSent = false;
			}
		else
			{
			if (ZLOG(s, eErr, fLogFacility))
				s << "Unrecognized request: " << theWhat;
			return;
			}
		}
	}

static void sSend(void* iConnection, const string& iLogFacility,
	ZMutexLocker& locker, const ZStreamW& iStream, const ZTuple& iTuple)
	{
#if kDebug_ShowCommsTuples
	if (ZLOG(s, eDebug, iLogFacility))
		{
		s.Writef(">> Server: %08X ", iConnection);
		ZUtil_Strim_Tuple::sToStrim(s, iTuple);
		}
#endif
	locker.Release();
	iTuple.ToStream(iStream);
	locker.Acquire();
	}

void ZTBServer::Writer(const ZStreamW& iStream)
	{
	ZMutexLocker locker(fMutex_Structure);

	while (!fReaderExited)
		{
		bool didAnything = false;

		if (fPingRequested)
			{
			fPingRequested = false;
			ZTuple response;
			response.SetString("What", "Pong");
			sSend(this, fLogFacility, locker, iStream, response);
			didAnything = true;
			}


		if (!fPingSent && fTime_LastRead + 10.0 < ZTime::sSystem())
			{
			fPingSent = true;
			ZTuple response;
			response.SetString("What", "Ping");
			sSend(this, fLogFacility, locker, iStream, response);
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
			sSend(this, fLogFacility, locker, iStream, response);
			didAnything = true;
			}


		if (!fTransactions_Create_Unsent.empty())
			{
			ZTuple response;
			response.SetString("What", "Create_Ack");
	
			vector<ZTValue>& vectorIDs = response.SetMutableVector("IDs");
			for (vector<Transaction*>::iterator i = fTransactions_Create_Unsent.begin();
				i != fTransactions_Create_Unsent.end(); ++i)
				{
				Transaction* theTransaction = *i;
				ZTuple theTuple;
				theTuple.SetInt64("ClientID", theTransaction->fClientID);
				theTuple.SetInt64("ServerID", reinterpret_cast<int64>(theTransaction));
				vectorIDs.push_back(theTuple);
				sSortedInsertMustNotContain(kDebug_TBServer,
					fTransactions_Created, theTransaction);
				}
			fTransactions_Create_Unsent.clear();
	
			sSend(this, fLogFacility, locker, iStream, response);
			didAnything = true;
			}


		if (!fTransactions_Validate_Succeeded.empty())
			{
			ZTuple response;
			response.SetString("What", "Validate_Succeeded");
	
			vector<ZTValue>& vectorClientIDs = response.SetMutableVector("ClientIDs");
			for (vector<Transaction*>::iterator i = fTransactions_Validate_Succeeded.begin();
				i != fTransactions_Validate_Succeeded.end(); ++i)
				{
				Transaction* theTransaction = *i;
				vectorClientIDs.push_back(int64(theTransaction->fClientID));
				sSortedInsertMustNotContain(kDebug_TBServer,
					fTransactions_Validated, theTransaction);
				}
			fTransactions_Validate_Succeeded.clear();

			sSend(this, fLogFacility, locker, iStream, response);
			didAnything = true;
			}


		if (!fTransactions_Validate_Failed.empty())
			{
			ZTuple response;
			response.SetString("What", "Validate_Failed");
	
			vector<ZTValue>& vectorClientIDs = response.SetMutableVector("ClientIDs");
			for (vector<Transaction*>::iterator i = fTransactions_Validate_Failed.begin();
				i != fTransactions_Validate_Failed.end(); ++i)
				{
				Transaction* theTransaction = *i;
				vectorClientIDs.push_back(int64(theTransaction->fClientID));
				fTransactions_HaveTuplesToSend.erase(theTransaction);
				theTransaction->fTBRepTransaction->AcceptFailure();
				// AcceptFailure deletes ZTBRepTransaction
				}
			fTransactions_Validate_Failed.clear();

			sSend(this, fLogFacility, locker, iStream, response);
			didAnything = true;
			}


		if (!fTransactions_Commit_Acked.empty())
			{
			ZTuple response;
			response.SetString("What", "Commit_Ack");
	
			vector<ZTValue>& vectorClientIDs = response.SetMutableVector("ClientIDs");
			for (vector<Transaction*>::iterator i = fTransactions_Commit_Acked.begin();
				i != fTransactions_Commit_Acked.end(); ++i)
				{
				Transaction* theTransaction = *i;
				vectorClientIDs.push_back(int64(theTransaction->fClientID));
				fTransactions_HaveTuplesToSend.erase(theTransaction);
				delete theTransaction;
				}
			fTransactions_Commit_Acked.clear();

			sSend(this, fLogFacility, locker, iStream, response);
			didAnything = true;
			}


		if (!fTransactions_HaveTuplesToSend.empty())
			{
			// Do the high priority ones first.
			ZTuple response;
			response.SetString("What", "GetTuples_Ack");
			
			bool allEmptied = true;
			bool sentAny = false;
			vector<ZTValue>& vectorTransactionTuples
				= response.SetMutableVector("Transactions");

			for (set<Transaction*>::iterator i = fTransactions_HaveTuplesToSend.begin();
				i != fTransactions_HaveTuplesToSend.end(); ++i)
				{
				Transaction* theTransaction = *i;
				if (!theTransaction->fTuplesToSend_HighPriority.empty())
					{
					ZTuple theTransactionTuple;
					theTransactionTuple.SetInt64("ClientID", theTransaction->fClientID);
					vector<ZTValue>& vectorTuples =
						theTransactionTuple.SetMutableVector("IDValues");

					for (map<uint64, ZTuple>::iterator
						j = theTransaction->fTuplesToSend_HighPriority.begin();
						j != theTransaction->fTuplesToSend_HighPriority.end(); ++j)
						{
						ZTuple aTuple;
						aTuple.SetID("ID", (*j).first);
						aTuple.SetTuple("Value", (*j).second);
						vectorTuples.push_back(aTuple);

						// Remove it from our low priority list too
						// Actually, this shouldn't be necessary. fTuplesSent should
						// stop this from happening. Probably should make this an assertion.
						ZUtil_STL::sEraseIfContains(
							theTransaction->fTuplesToSend_LowPriority, (*j).first);
						}
					theTransaction->fTuplesToSend_HighPriority.clear();
					vectorTransactionTuples.push_back(theTransactionTuple);
					sentAny = true;
					}
				if (!theTransaction->fTuplesToSend_LowPriority.empty())
					allEmptied = false;
				}
			if (allEmptied)
				fTransactions_HaveTuplesToSend.clear();

			if (sentAny)
				{
				sSend(this, fLogFacility, locker, iStream, response);
				didAnything = true;
				}
			}


		if (!fSearches_Unsent.empty())
			{
			ZTuple response;
			response.SetString("What", "Search_Ack");
	
			vector<ZTValue>& vectorSearches = response.SetMutableVector("Searches");
			for (vector<Search_t*>::iterator i = fSearches_Unsent.begin();
				i != fSearches_Unsent.end(); ++i)
				{
				Search_t* theSearch = *i;
				ZTuple theTuple;
				theTuple.SetInt64("SearchID", theSearch->fClientSearchID);
				theTuple.SetVector_T("Results",
					theSearch->fResults.begin(), theSearch->fResults.end());

				vectorSearches.push_back(theTuple);
				delete theSearch;
				}
			fSearches_Unsent.clear();

			sSend(this, fLogFacility, locker, iStream, response);
			didAnything = true;
			}


		if (!fCounts_Unsent.empty())
			{
			ZTuple response;
			response.SetString("What", "Count_Ack");
	
			vector<ZTValue>& vectorCounts = response.SetMutableVector("Counts");
			for (vector<Count_t*>::iterator i = fCounts_Unsent.begin();
				i != fCounts_Unsent.end(); ++i)
				{
				Count_t* theCount = *i;
				ZTuple theTuple;
				theTuple.SetInt64("CountID", theCount->fClientCountID);
				theTuple.SetInt64("Result", theCount->fResult);
				vectorCounts.push_back(theTuple);
				delete theCount;
				}
			fCounts_Unsent.clear();

			sSend(this, fLogFacility, locker, iStream, response);
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
			vector<ZTValue>& vectorTransactionTuples
				= response.SetMutableVector("Transactions");

			for (set<Transaction*>::iterator i = fTransactions_HaveTuplesToSend.begin();
				i != fTransactions_HaveTuplesToSend.end(); ++i)
				{
				Transaction* theTransaction = *i;
				if (!theTransaction->fTuplesToSend_LowPriority.empty())
					{
					ZTuple theTransactionTuple;
					theTransactionTuple.SetInt64("ClientID", theTransaction->fClientID);
					vector<ZTValue>& vectorTuples =
						theTransactionTuple.SetMutableVector("IDValues");

					for (map<uint64, ZTuple>::iterator
						i = theTransaction->fTuplesToSend_LowPriority.begin();
						i != theTransaction->fTuplesToSend_LowPriority.end(); ++i)
						{
						ZTuple aTuple;
						aTuple.SetID("ID", (*i).first);
						aTuple.SetTuple("Value", (*i).second);
						vectorTuples.push_back(aTuple);
						}
					theTransaction->fTuplesToSend_LowPriority.clear();
					vectorTransactionTuples.push_back(theTransactionTuple);
					}
				if (!theTransaction->fTuplesToSend_HighPriority.empty())
					allEmptied = false;
				}
			if (allEmptied)
				fTransactions_HaveTuplesToSend.clear();

			sSend(this, fLogFacility, locker, iStream, response);
			didAnything = true;
			}

		if (!didAnything)
			{
			iStream.Flush();
			fCondition_Sender.Wait(fMutex_Structure, 1000000);
			}
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
