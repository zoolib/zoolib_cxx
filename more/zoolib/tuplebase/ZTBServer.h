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

#ifndef __ZTBServer__
#define __ZTBServer__ 1
#include "zconfig.h"

#include "zoolib/ZThreadOld.h"
#include "zoolib/ZTuple.h"

#include <set>

namespace ZooLib {

class ZStreamR;
class ZStreamW;
class ZTB;
class ZTBRep;

// =================================================================================================
#pragma mark -
#pragma mark * ZTBServer

class ZTBServer
:	public ZTask
,	public ZCommer
	{
private:
	class Transaction;
	class Search_t;
	class Count_t;

public:
	ZTBServer(
		ZRef<ZTaskMaster> iTaskMaster,
		ZRef<ZStreamerR> iStreamerR, ZRef<ZStreamerW> iStreamerW,
		ZRef<ZTBRep> iTBRep, const std::string& iLogFacility);

	~ZTBServer();

// From ZTask
	virtual void Kill();

// From ZCommer
	virtual bool Read(const ZStreamR& iStreamR);
	virtual bool Write(const ZStreamW& iStreamW);

	virtual void Finished();

private:
	static void spCallback_AllocateIDs(void* iRefcon, uint64 iBaseID, size_t iCount);
	void Handle_AllocateIDs(const ZTuple& iReq);

	void Handle_Create(const ZTuple& iReq);

	static void spCallback_GetTupleForSearch(
		void* iRefcon, size_t iCount, const uint64* iIDs, const ZTuple* iTuples);

	static void spCallback_Search(void* iRefcon, std::vector<uint64>& ioResults);
	void Handle_Search(const ZTuple& iReq);

	static void spCallback_Count(void* iRefcon, size_t iResult);
	void Handle_Count(const ZTuple& iReq);

	void Handle_Abort(const ZTuple& iReq);

	static void spCallback_Validate(bool iSucceeded, void* iRefcon);
	void Handle_Validate(const ZTuple& iReq);

	static void spCallback_Commit(void* iRefcon);
	void Handle_Commit(const ZTuple& iReq);

	static void spCallback_GetTuple(
		void* iRefcon, size_t iCount, const uint64* iIDs, const ZTuple* iTuples);

	void Handle_Actions(const ZTuple& iReq);

	void Reader(const ZStreamR& iStream);
	void Writer(const ZStreamW& iStream);

	void TearDown();

	ZRef<ZTBRep> fTBRep;

	ZMutex fMutex_Structure;
	ZCondition fCondition_Sender;
		ZTime fTime_LastRead;
		bool fSendClose;
		bool fPingRequested;
		bool fPingSent;

		std::vector<Transaction*> fTransactions_Create_Unsent;
		std::vector<Transaction*> fTransactions_Created;

		std::vector<Transaction*> fTransactions_Validate_Waiting;
		std::vector<Transaction*> fTransactions_Validate_Succeeded;
		std::vector<Transaction*> fTransactions_Validate_Failed;
		std::vector<Transaction*> fTransactions_Validated;

		std::vector<Transaction*> fTransactions_Commit_Waiting;
		std::vector<Transaction*> fTransactions_Commit_Acked;

		std::set<Transaction*> fTransactions_HaveTuplesToSend;

		std::vector<Search_t*> fSearches_Waiting;
		std::vector<Search_t*> fSearches_Unsent;

		std::vector<Count_t*> fCounts_Waiting;
		std::vector<Count_t*> fCounts_Unsent;

		std::vector<std::pair<uint64, size_t> > fIDs;

	std::string fLogFacility;
	};

} // namespace ZooLib

#endif // __ZTBServer__
