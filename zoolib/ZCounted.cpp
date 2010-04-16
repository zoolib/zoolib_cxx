/* -------------------------------------------------------------------------------------------------
Copyright (c) 2010 Andrew Green
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

#include "zoolib/ZCounted.h"
#include "zoolib/ZDebug.h"

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * ZCountedBase

ZCountedBase::ZCountedBase()
:	fRefCount(0)
	{}

ZCountedBase::~ZCountedBase()
	{
	ZAssertStopf(1, 0 == ZAtomic_Get(&fRefCount),
		("Non-zero refcount at destruction, it is %d", ZAtomic_Get(&fRefCount)));
	}

void ZCountedBase::Initialize()
	{
	ZAssertStopf(1, 1 == ZAtomic_Get(&fRefCount),
		("Refcount is not 1, it is %d", ZAtomic_Get(&fRefCount)));
	}

void ZCountedBase::Finalize()
	{
	ZAssertStopf(1, 1 == ZAtomic_Get(&fRefCount),
		("Refcount is not 1, it is %d", ZAtomic_Get(&fRefCount)));
	this->FinalizationComplete();
	delete this;
	}

void ZCountedBase::FinalizationComplete()
	{ ZAtomic_Dec(&fRefCount); }

void ZCountedBase::Retain()
	{
	if (0 == ZAtomic_Add(&fRefCount, 1))
		this->Initialize();
	}

void ZCountedBase::Release()
	{
	for (;;)
		{
		int oldRefCount = ZAtomic_Get(&fRefCount);
		if (oldRefCount == 1)
			{
			this->Finalize();
			break;
			}
		else
			{
			if (ZAtomic_CompareAndSwap(&fRefCount, oldRefCount, oldRefCount - 1))
				break;
			}
		}
	}

int ZCountedBase::GetRefCount() const
	{ return ZAtomic_Get(&fRefCount); }

int ZCountedBase::pCOMAddRef()
	{
	int oldRefCount = ZAtomic_Add(&fRefCount, 1);
	if (oldRefCount == 0)
		this->Initialize();
	return oldRefCount + 1;
	}

int ZCountedBase::pCOMRelease()
	{
	for (;;)
		{
		int oldRefCount = ZAtomic_Get(&fRefCount);
		if (oldRefCount == 1)
			{
			this->Finalize();
			// Hmm. At this point we cannot know if we've been destroyed.
			// Return zero as a sensible value.
			return 0;
			}
		else
			{
			if (ZAtomic_CompareAndSwap(&fRefCount, oldRefCount, oldRefCount - 1))
				return oldRefCount - 1;
			}
		}
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZCountedWithoutFinalize

ZCountedWithoutFinalize::ZCountedWithoutFinalize()
:	fRefCount(0)
	{}

ZCountedWithoutFinalize::~ZCountedWithoutFinalize()
	{
	ZAssertStopf(1, ZThreadSafe_Get(fRefCount) == 0,
		("Non-zero refcount at destruction, it is %d", ZThreadSafe_Get(fRefCount)));
	}

void ZCountedWithoutFinalize::Release()
	{
	if (ZThreadSafe_DecAndTest(fRefCount))
		delete this;
	}

int ZCountedWithoutFinalize::GetRefCount() const
	{ return ZThreadSafe_Get(fRefCount); }

NAMESPACE_ZOOLIB_END
