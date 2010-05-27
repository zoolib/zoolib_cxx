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

#ifndef __ZTS_RAM__
#define __ZTS_RAM__
#include "zconfig.h"

#include "zoolib/ZRWlock.h"
#include "zoolib/tuplebase/ZTS.h"

#include <map>
#include <set>

namespace ZooLib {

class ZTupleIndex;
class ZTupleIndexFactory;

// =================================================================================================
#pragma mark -
#pragma mark * ZTS_RAM

class ZTS_RAM : public ZTS
	{
public:
	ZTS_RAM();
	ZTS_RAM(const std::vector<ZRef<ZTupleIndexFactory> >& iIndexFactories);
	ZTS_RAM(const std::vector<ZRef<ZTupleIndexFactory> >& iIndexFactories,
		uint64 iNextUnusedID, const std::map<uint64, ZTuple>& iTuples);
	ZTS_RAM(const std::vector<ZRef<ZTupleIndexFactory> >& iIndexFactories,
		uint64 iNextUnusedID, std::map<uint64, ZTuple>& ioTuples, bool iKnowWhatImDoing);
	virtual ~ZTS_RAM();

// From ZTS
	virtual void AllocateIDs(size_t iCount, uint64& oBaseID, size_t& oCount);
	virtual void SetTuples(size_t iCount, const uint64* iIDs, const ZTuple* iTuples);
	virtual void GetTuples(size_t iCount, const uint64* iIDs, ZTuple* oTuples);
	virtual void Search(const ZTBSpec& iSpec,
		const std::set<uint64>& iSkipIDs, std::set<uint64>& ioIDs);
	virtual ZMutexBase& GetReadLock();
	virtual ZMutexBase& GetWriteLock();

// Our protocol
	void AddIndex(ZRef<ZTupleIndexFactory> iIndexFactory);
	void AddIndices(const std::vector<ZRef<ZTupleIndexFactory> >& iIndexFactories);

	bool GetDataIfChanged(uint64& oNextUnusedID, std::map<uint64, ZTuple>& oTuples);
	bool GetDataIfChanged(uint64& oNextUnusedID, std::vector<std::pair<uint64, ZTuple> >& oTuples);
	void GetData(uint64& oNextUnusedID, std::map<uint64, ZTuple>& oTuples);

private:
	ZRWLock fRWLock;
	ZMutex fMutex_ID;
	uint64 fNextUnusedID;
	std::map<uint64, ZTuple> fTuples;
	std::vector<ZTupleIndex*> fIndices;
	bool fChanged;
	};

} // namespace ZooLib

#endif // __ZTS_RAM__
