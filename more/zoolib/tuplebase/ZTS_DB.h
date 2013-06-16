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

#ifndef __ZTS_DB__
#define __ZTS_DB__
#include "zconfig.h"
#include "zoolib/ZCONFIG_API.h"
#include "zoolib/ZCONFIG_SPI.h"

#ifndef ZCONFIG_API_Avail__TS_DB
#	define ZCONFIG_API_Avail__TS_DB ZCONFIG_SPI_Enabled(BerkeleyDB)
#endif

#ifndef ZCONFIG_API_Desired__TS_DB
#	define ZCONFIG_API_Desired__TS_DB 1
#endif

#include "zoolib/tuplebase/ZTS.h"

#if ZCONFIG_API_Enabled(TS_DB)

#include "zoolib/ZFile.h"
#include "zoolib/tuplebase/ZTupleIndex.h"

#include <string>
#include <vector>

#include <db.h>

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * ZTS_DB

class ZTS_DB : public ZTS
	{
public:
	ZTS_DB(const ZFileSpec& iFileSpec);
	ZTS_DB(const ZFileSpec& iFileSpec, const std::vector<std::vector<std::string> >& iIndexSpecs);
	virtual ~ZTS_DB();

// From ZTS
	virtual void AllocateIDs(size_t iCount, uint64& oBaseID, size_t& oCount);
	virtual void SetTuples(size_t iCount, const uint64* iIDs, const ZTuple* iTuples);
	virtual void GetTuples(size_t iCount, const uint64* iIDs, ZTuple* oTuples);
	virtual void Search(
		const ZTBSpec& iSpec, const std::set<uint64>& iSkipIDs, std::set<uint64>& ioIDs);
	virtual ZMutexBase& GetReadLock();
	virtual ZMutexBase& GetWriteLock();

// Our protocol
	void Flush();

// Uggh
	void SetNextUnusedID(uint64 iID) { fNextUnusedID = iID; }

private:
	class Index;
	friend class Index;

	bool pGetTuple(uint64 iID, ZTuple& oTuple);
	void pFlush();

	static int spBTree_Compare(const DBT* iLeft, const DBT* iRight);
	static size_t spBTree_Prefix(const DBT* iLeft, const DBT* iRight);

	ZMutex fMutex;
	ZMutex fMutex_ID;
	uint64 fNextUnusedID;
	ZFileSpec fFileSpec;
	bool fChanged;
	DB* fDB;
	std::vector<ZTupleIndex*> fIndices;
	};

} // namespace ZooLib

#endif // ZCONFIG_API_Enabled(TS_DB)

#endif // __ZTS_DB__
