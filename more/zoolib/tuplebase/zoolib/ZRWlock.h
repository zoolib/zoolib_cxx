/* -------------------------------------------------------------------------------------------------
Copyright (c) 2008 Andrew Green
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

#ifndef __ZRWLock__
#define __ZRWLock__ 1
#include "zconfig.h"

#include "zoolib/ZThreadOld.h"

#include <vector>

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * ZRWLock

class ZRWLock : NonCopyable
	{
public:
	ZRWLock(const char* iName = nullptr);
	~ZRWLock();

	ZMutexBase& GetRead() { return fRead; }
	ZMutexBase& GetWrite() { return fWrite; }

	bool CanRead();
	bool CanWrite();

	void DemoteWriteToRead();

protected:
	void AcquireRead();
	void AcquireWrite();
	void ReturnLock();

private:
	// Mutex to provide mutual exclusion while fiddling with internal data
	ZMutexNR fMutex_Structure;

	// Used to queue up a reader and multiple writers.
	ZSemaphore fSem_Access;

	// Used to block subsequent readers
	ZSemaphore fSem_SubsequentReaders;

	// Writers waiting for the lock
	int32 fCount_WaitingWriters;

	// Writers holding the lock (the same writer recursively, of course)
	int32 fCount_CurrentWriter;
	ZThread::ID fThreadID_CurrentWriter;

	// Readers waiting for the lock
	int32 fCount_WaitingReaders;

	// Readers holding the lock (to allow recursive calls to AcquireRead)
	int32 fCount_CurrentReaders;
	std::vector<ZThread::ID> fVector_CurrentReaders;

	class Read : public ZMutexBase
		{
	public:
		Read(ZRWLock* iRWLock, const char* iName) : fRWLock(iRWLock) {}
		~Read() {}

		virtual void Acquire();
		virtual void Release();
		virtual bool IsLocked();

	private:
		ZRWLock* fRWLock;
		};

	Read fRead;
	friend class Read;

public:
	class Write : public ZMutexBase
		{
	public:
		Write(ZRWLock* iRWLock, const char* iName) : fRWLock(iRWLock) {}
		~Write() {}

		virtual void Acquire();
		virtual void Release();
		virtual bool IsLocked();

		ZRWLock& GetRWLock() { return *fRWLock; }
	private:
		ZRWLock* fRWLock;
		};
private:

	Write fWrite;
	friend class Write;
	};

} // namespace ZooLib

#endif // __ZRWLock__
