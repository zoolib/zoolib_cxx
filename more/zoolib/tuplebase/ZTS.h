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

#ifndef __ZTS__
#define __ZTS__
#include "zconfig.h"

#include "zoolib/ZCompat_NonCopyable.h"
#include "zoolib/ZData_Any.h"
#include "zoolib/ZStream_Data_T.h"
#include "zoolib/ZTuple.h"
#include "zoolib/ZThreadOld.h"
#include "zoolib/tuplebase/ZTBSpec.h"

#include <set>

namespace ZooLib {

typedef ZData_Any ZMemoryBlock;
typedef ZStreamRPos_Data_T<ZData_Any> ZStreamRPos_MemoryBlock;
typedef ZStreamRWPos_Data_T<ZData_Any> ZStreamRWPos_MemoryBlock;

// =================================================================================================
#pragma mark -
#pragma mark * ZTS

class ZTS : public ZCountedWithoutFinalize, NonCopyable
	{
protected:
	ZTS();

public:
	virtual ~ZTS();

	virtual void AllocateIDs(size_t iCount, uint64& oBaseID, size_t& oCountIssued) = 0;
	virtual void SetTuples(size_t iCount, const uint64* iIDs, const ZTuple* iTuples) = 0;
	virtual void GetTuples(size_t iCount, const uint64* iIDs, ZTuple* oTuples) = 0;

	virtual void Search(
		const ZTBSpec& iSpec, const std::set<uint64>& iSkipIDs, std::set<uint64>& oIDs) = 0;

	virtual void Flush();
	virtual ZMutexBase& GetReadLock() = 0;
	virtual ZMutexBase& GetWriteLock() = 0;

	ZTuple GetTuple(uint64 iID);
	};

} // namespace ZooLib

#endif // __ZTS__
