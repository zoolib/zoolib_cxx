/* -------------------------------------------------------------------------------------------------
Copyright (c) 2000 Andrew Green and Learning in Motion, Inc.
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

#include "ZRefCount.h"

/**
\defgroup RefCount Reference Counting and Smart Pointers

\todo Finish this
*/

// =================================================================================================
#pragma mark -
#pragma mark * ZRefCounted

ZRefCounted::~ZRefCounted()
	{
	ZAssertStopf(1, ZThreadSafe_Get(fRefCount) == 0,
		("Non-zero refcount at destruction, it is %d", ZThreadSafe_Get(fRefCount)));
	}

void ZRefCounted::sCheckAccess(ZRefCounted* iObject)
	{
	ZAssertStopf(0, iObject, ("ZRef accessed with nil object"));
	ZAssertStopf(0, ZThreadSafe_Get(iObject->fRefCount) >= 0, ("ZRef accessed with invalid refcount"));
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZRefCountedWithFinalization

ZRefCountedWithFinalization::~ZRefCountedWithFinalization()
	{
	ZAssertStopf(1, 0 == ZThreadSafe_Get(fRefCount),
		("Non-zero refcount at destruction, it is %d", ZThreadSafe_Get(fRefCount)));
	}

void ZRefCountedWithFinalization::Initialize()
	{
	ZAssertStopf(1, 1 == ZThreadSafe_Get(fRefCount),
		("Refcount is not 1, it is %d", ZThreadSafe_Get(fRefCount)));
	}

void ZRefCountedWithFinalization::Finalize()
	{
	ZAssertStopf(1, 1 == ZThreadSafe_Get(fRefCount),
		("Refcount is not 1, it is %d", ZThreadSafe_Get(fRefCount)));
	this->FinalizationComplete();
	delete this;
	}

void ZRefCountedWithFinalization::FinalizationComplete()
	{
	ZThreadSafe_Dec(fRefCount);
	}

void ZRefCountedWithFinalization::sDecRefCount(ZRefCountedWithFinalization* iObject)
	{
	if (!iObject)
		return;

	#if ZCONFIG_Thread_Preemptive

		for (;;)
			{
			int oldRefCount = ZAtomic_Get(&iObject->fRefCount);
			if (oldRefCount == 1)
				{
				iObject->Finalize();
				break;
				}
			else
				{
				if (ZAtomic_CompareAndSwap(&iObject->fRefCount, oldRefCount, oldRefCount - 1))
					break;
				// We'll only have to loop if we were preempted between the call to
				// ZAtomic_Get and the call to ZAtomic_CompareAndSwap, so we'll very
				// likely succeed next time round the loop.
				}
			}
	#else

		if (ZThreadSafe_DecAndTest(iObject->fRefCount))
			{
			ZThreadSafe_Set(iObject->fRefCount, 1);
			iObject->Finalize();
			}

	#endif
	}

int ZRefCountedWithFinalization::AddRef()
	{
	size_t newCount = ZThreadSafe_IncReturnNew(fRefCount);
	if (newCount == 1)
		this->Initialize();
	return newCount;
	}

int ZRefCountedWithFinalization::Release()
	{
	for (;;)
		{
		int oldRefCount = ZAtomic_Get(&fRefCount);
		if (oldRefCount == 1)
			{
			this->Finalize();
			// We return the refcount we ended up with, which may be non-zero
			// depending on what happened in Finalize.
			return ZAtomic_Get(&fRefCount);
			}
		else
			{
			if (ZAtomic_CompareAndSwap(&fRefCount, oldRefCount, oldRefCount - 1))
				return oldRefCount - 1;
			}
		}
	}

void ZRefCountedWithFinalization::sCheckAccess(ZRefCountedWithFinalization* iObject)
	{
	ZAssertStopf(0, iObject, ("ZRef accessed with nil object"));
	ZAssertStopf(0, ZThreadSafe_Get(iObject->fRefCount) >= 0, ("ZRef accessed with invalid refcount"));
	}
