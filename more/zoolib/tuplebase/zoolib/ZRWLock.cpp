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

#include "zoolib/ZCompat_algorithm.h"
#include "zoolib/ZRWLock.h"

namespace ZooLib {

using std::find;
using std::vector;

// =================================================================================================
#pragma mark -
#pragma mark * kDebug

#define kDebug_RWLock 3

// =================================================================================================
#pragma mark -
#pragma mark * ZRWLock

ZRWLock::ZRWLock(const char* iName)
:	fSem_Access(1, "ZRWLock::fSem_Access"),
	fSem_SubsequentReaders(0, "ZRWLock::fSem_SubsequentReaders"),
	fCount_WaitingWriters(0),
	fCount_CurrentWriter(0),
	fThreadID_CurrentWriter(0),
	fCount_WaitingReaders(0),
	fCount_CurrentReaders(0),
	fRead(this, iName),
	fWrite(this, iName)
	{}

ZRWLock::~ZRWLock()
	{
	ZAssertStop(kDebug_RWLock, fCount_WaitingWriters == 0);
	ZAssertStop(kDebug_RWLock, fCount_CurrentWriter == 0);
	ZAssertStop(kDebug_RWLock, fCount_WaitingReaders == 0);
	ZAssertStop(kDebug_RWLock, fCount_CurrentReaders == 0);
	ZAssertStop(kDebug_RWLock, fVector_CurrentReaders.size() == 0);
	ZAssertStop(kDebug_RWLock, fThreadID_CurrentWriter == 0);
	}

void ZRWLock::DemoteWriteToRead()
	{
	fMutex_Structure.Acquire();

	const ZThread::ID currentID = ZThread::sID();

	ZAssert(fThreadID_CurrentWriter == currentID);
	fThreadID_CurrentWriter = 0;

	ZAssert(fCount_CurrentWriter);
	fCount_CurrentWriter = 0;

	ZAssert(fCount_CurrentReaders == 0);
	fCount_CurrentReaders = 1;
	fVector_CurrentReaders.push_back(currentID);

	fSem_SubsequentReaders.Signal(fCount_WaitingReaders);
	fCount_CurrentReaders += fCount_WaitingReaders;
	fCount_WaitingReaders = 0;

	fMutex_Structure.Release();
	}

void ZRWLock::AcquireRead()
	{
	fMutex_Structure.Acquire();

	ZThread::ID currentID = ZThread::sID();

	if (fThreadID_CurrentWriter == currentID)
		{
		// We're the current writer, just reacquire the write lock (this is fair, cause we've
		// already locked out other readers by the fact that we posess the write lock. And anyway,
		// if we were to fail to acquire the read lock, we'd just deadlock the entire system)
		++fCount_CurrentWriter;
		}
	else
		{
		// The following check on fCount_CurrentReaders short circuits the call to find(); if
		// fCount_CurrentReaders is zero then obviously we can't find ourselves in the vector.
		if (fCount_CurrentReaders
			&& find(fVector_CurrentReaders.begin(), fVector_CurrentReaders.end(), currentID)
			!= fVector_CurrentReaders.end())
			{
			// We're a current reader, so just stick ourselves on the list again.
			++fCount_CurrentReaders;
			fVector_CurrentReaders.push_back(currentID);

			ZDebugLogf(kDebug_RWLock,
				"Thread %04X, Got it read recursive, total: %d", ZThread::sID(),
				fCount_CurrentReaders);
			}
		else
			{
			if (fCount_WaitingWriters != 0 || fCount_CurrentReaders == 0)
				{
				// There are waiting writers, or there are no current
				// readers, so we have to ensure the access lock is grabbed
				++fCount_WaitingReaders;
				if (fCount_WaitingReaders == 1)
					{
					// We're the first reader to come along, acquire the access lock.					ZDebugLogf(kDebug_RWLock, "Thread %04X, Waiting read", ZThread::sID()));
					fMutex_Structure.Release();

					fSem_Access.Wait(1);

					fMutex_Structure.Acquire();

					ZDebugLogf(kDebug_RWLock, "Thread %04X, Got it read", ZThread::sID());
					ZAssertStop(kDebug_RWLock,
						fThreadID_CurrentWriter == 0 && fCount_CurrentWriter == 0);
					ZDebugLogf(kDebug_RWLock,
						"Thread %04X, Signal subs: %d", ZThread::sID(),
						fCount_WaitingReaders - 1);

					// We've acquired the access lock, so let the subsequent readers through.
					fSem_SubsequentReaders.Signal(fCount_WaitingReaders - 1);
					fCount_CurrentReaders += fCount_WaitingReaders;
					fCount_WaitingReaders = 0;
					fVector_CurrentReaders.push_back(currentID);
					}
				else
					{
					// We're a subsequent reader. Wait for the read barrier to be opened
					ZDebugLogf(kDebug_RWLock,
						"Thread %04X, Waiting subsequent", ZThread::sID());

					fMutex_Structure.Release();

					fSem_SubsequentReaders.Wait(1);

					fMutex_Structure.Acquire();

					ZDebugLogf(kDebug_RWLock,
						"Thread %04X, Got it subsequent, total: %d",
							ZThread::sID(), fCount_CurrentReaders);
					ZAssertStop(kDebug_RWLock,
						fThreadID_CurrentWriter == 0 && fCount_CurrentWriter == 0);

					fVector_CurrentReaders.push_back(currentID);
					}
				}
			else
				{
				// There were no waiting writers and there are current readers.
				ZAssertStop(kDebug_RWLock,
					fThreadID_CurrentWriter == 0 && fCount_CurrentWriter == 0);
				ZDebugLogf(kDebug_RWLock, "Thread %04X, Got it other", ZThread::sID());

				++fCount_CurrentReaders;
				fVector_CurrentReaders.push_back(currentID);
				}
			}
		}
	fMutex_Structure.Release();
	}

void ZRWLock::ReturnLock()
	{
	fMutex_Structure.Acquire();
	// Current writer must be positive
	ZAssertStop(kDebug_RWLock, fCount_CurrentWriter >= 0);
	// And reader or writer count must be non zero
	ZAssertStop(kDebug_RWLock, fCount_CurrentWriter != 0 || fVector_CurrentReaders.size() != 0);
	// *And* they can't both be non zero
	ZAssertStop(kDebug_RWLock, !(fCount_CurrentWriter != 0 && fVector_CurrentReaders.size() != 0));

	ZThread::ID currentID = ZThread::sID();

	if (fThreadID_CurrentWriter == currentID)
		{
		// We're returning the the write lock.
		if (--fCount_CurrentWriter == 0)
			{
			// We must also release the access lock
			fThreadID_CurrentWriter = 0;

			ZDebugLogf(kDebug_RWLock, "Thread %04X, Returning write", ZThread::sID());
			fSem_Access.Signal(1);
			}

		}
	else
		{
		// We're not the current writer, so we're returning the read lock. In which case
		// there can't be a current writer
		ZAssertStop(kDebug_RWLock, fThreadID_CurrentWriter == 0);

		vector<ZThread::ID>::iterator theIter =
			find(fVector_CurrentReaders.begin(), fVector_CurrentReaders.end(), currentID);
		ZAssertStop(kDebug_RWLock, theIter != fVector_CurrentReaders.end());
		fVector_CurrentReaders.erase(theIter);

		if (--fCount_CurrentReaders == 0)
			{
			ZAssertStop(kDebug_RWLock, fVector_CurrentReaders.size() == 0);
			ZDebugLogf(kDebug_RWLock, "Thread %04X, Returning read", ZThread::sID());

			fSem_Access.Signal(1);
			}
		}

	fMutex_Structure.Release();
	}

void ZRWLock::AcquireWrite()
	{
	fMutex_Structure.Acquire();
	ZThread::ID currentID = ZThread::sID();

	// Trip an assertion if this is a call on a lock we've already
	// locked for reading, which means we're gonna deadlock.
	ZAssertLogf(kDebug_RWLock,
		find(fVector_CurrentReaders.begin(), fVector_CurrentReaders.end(), currentID)
		== fVector_CurrentReaders.end(), ("DEADLOCK"));

	if (fThreadID_CurrentWriter == currentID)
		{
		// We're the current writer so this is a recursive
		// acquisition and we can just increment the count.
		++fCount_CurrentWriter;
		ZAssertStop(kDebug_RWLock, fCount_CurrentReaders == 0);
		ZDebugLogf(kDebug_RWLock,
			"Thread %04X, Got it write recursive %d", ZThread::sID(),
			fCount_CurrentWriter);
		fMutex_Structure.Release();
		return;
		}

	// Indicate that we're waiting to write
	++fCount_WaitingWriters;
	ZDebugLogf(kDebug_RWLock, "Thread %04X, Waiting write", ZThread::sID());
	fMutex_Structure.Release();

	fSem_Access.Wait(1);

	fMutex_Structure.Acquire();
	ZDebugLogf(kDebug_RWLock, "Thread %04X, Got it write", ZThread::sID());
	// We're no longer waiting
	--fCount_WaitingWriters;

	ZAssertStop(kDebug_RWLock,
		fThreadID_CurrentWriter == 0 && fCount_CurrentWriter == 0);
	ZAssertStop(kDebug_RWLock, fCount_CurrentReaders == 0);
	fThreadID_CurrentWriter = currentID;

	fCount_CurrentWriter = 1;
	fMutex_Structure.Release();
	}

bool ZRWLock::CanRead()
	{
	fMutex_Structure.Acquire();
	ZThread::ID currentID = ZThread::sID();
	if (fCount_CurrentWriter != 0 && fThreadID_CurrentWriter == currentID)
		{
		// We're the current writer, so we can also read.
		fMutex_Structure.Release();
		return true;
		}

	if (find(fVector_CurrentReaders.begin(), fVector_CurrentReaders.end(), currentID)
		!= fVector_CurrentReaders.end())
		{
		// We're a current reader.
		fMutex_Structure.Release();
		return true;
		}

	fMutex_Structure.Release();
	return false;
	}

bool ZRWLock::CanWrite()
	{
	fMutex_Structure.Acquire();
	if (fCount_CurrentWriter != 0 && fThreadID_CurrentWriter == ZThread::sID())
		{
		// We're the current writer.
		fMutex_Structure.Release();
		return true;
		}
	fMutex_Structure.Release();
	return false;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZRWLock::Read

void ZRWLock::Read::Acquire()
	{ fRWLock->AcquireRead(); }

void ZRWLock::Read::Release()
	{ fRWLock->ReturnLock(); }

bool ZRWLock::Read::IsLocked()
	{ return fRWLock->CanRead(); }

// =================================================================================================
#pragma mark -
#pragma mark * ZRWLock::Write

void ZRWLock::Write::Acquire()
	{ fRWLock->AcquireWrite(); }

void ZRWLock::Write::Release()
	{ fRWLock->ReturnLock(); }

bool ZRWLock::Write::IsLocked()
	{ return fRWLock->CanWrite(); }

} // namespace ZooLib
