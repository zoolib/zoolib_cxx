/* -------------------------------------------------------------------------------------------------
Copyright (c) 2005 Andrew Green and Learning in Motion, Inc.
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

#ifndef __ZTS_Umbrella_h__
#define __ZTS_Umbrella_h__
#include "zconfig.h"

#include "zoolib/ZTS.h"

#include "zoolib/ZMutexComposite.h"

#include <map>

namespace ZooLib {

// =================================================================================================
// MARK: - ZTS_Umbrella

class ZTS_Umbrella : public ZTS
	{
public:
	ZTS_Umbrella(const std::vector<ZRef<ZTS> >& iTSes);
	virtual ~ZTS_Umbrella();

	virtual void AllocateIDs(size_t iCount, uint64& oBaseID, size_t& oCount);
	virtual void SetTuples(size_t iCount, const uint64* iIDs, const ZTuple* iTuples);
	virtual void GetTuples(size_t iCount, const uint64* iIDs, ZTuple* oTuples);
	virtual void Search(const ZTBSpec& iSpec, const std::set<uint64>& iSkipIDs, std::set<uint64>& ioIDs);
	virtual ZMutexBase& GetReadLock();
	virtual ZMutexBase& GetWriteLock();

private:
	typedef std::map<uint64, std::pair<size_t, uint64> > GlobalToLocal_t;
	typedef std::map<uint64, uint64> LocalToGlobal_t;
	class Child
		{
	public:
		ZRef<ZTS> fTS;
		LocalToGlobal_t fLocalToGlobal;
		};

	uint64 pAllocateID();
	uint64 pAllocateID(ZRef<ZTS> iTS);

	void pTranslate_GlobalToLocal(size_t iChildIndex, ZMap_ZooLib& ioTuple);
	void pTranslate_LocalToGlobal(size_t iChildIndex, ZMap_ZooLib& ioTuple);

	uint64 pGlobalToLocal(size_t iChildIndex, uint64 iGlobalID);
	uint64 pLocalToGlobal(size_t iChildIndex, uint64 iLocalID);

	std::vector<Child> fChildren;
	GlobalToLocal_t fGlobalToLocal;
	ZMutexComposite fReadComposite;
	};

} // namespace ZooLib

#endif // __ZTS_Umbrella_h__
