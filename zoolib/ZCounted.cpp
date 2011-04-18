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

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * ZCountedBase

ZCountedBase::ZCountedBase()
:	fRefCount(0)
	{}

ZCountedBase::~ZCountedBase()
	{
	ZAssertStop(1, !fWeakRefProxy);
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
	if (this->FinishFinalize())
		delete this;
	}

bool ZCountedBase::FinishFinalize()
	{
	if (!ZAtomic_DecAndTest(&fRefCount))
		return false;
	
	if (fWeakRefProxy)
		{
		fWeakRefProxy->Clear();
		fWeakRefProxy.Clear();
		}

	return true;
	}

void ZCountedBase::Retain()
	{
	if (0 == ZAtomic_Add(&fRefCount, 1))
		this->Initialize();
	}

void ZCountedBase::Release()
	{
	for (;;)
		{
		const int oldRefCount = ZAtomic_Get(&fRefCount);
		if (oldRefCount == 1)
			{
			this->Finalize();
			return;
			}

		if (ZAtomic_CompareAndSwap(&fRefCount, oldRefCount, oldRefCount - 1))
			return;
		}
	}

bool ZCountedBase::IsShared() const
	{ return 1 < ZAtomic_Get(&fRefCount); }

bool ZCountedBase::IsReferenced() const
	{ return 0 != ZAtomic_Get(&fRefCount); }

ZRef<ZCountedBase::WeakRefProxy> ZCountedBase::GetWeakRefProxy()
	{
	if (!fWeakRefProxy)
		{
		ZRef<WeakRefProxy> theWeakRefProxy = new WeakRefProxy(this);
		if (!fWeakRefProxy.AtomicSetIfNull(theWeakRefProxy.Get()))
			{
			// We lost the race, so (efficiently) clear theWeakRefProxy's reference
			// to us, or we'll trip an asssertion in WeakRefProxy::~WeakRefProxy.
			theWeakRefProxy->Clear();
			}
		}
	return fWeakRefProxy;
	}

int ZCountedBase::pCOMAddRef()
	{
	const int oldRefCount = ZAtomic_Add(&fRefCount, 1);
	if (oldRefCount == 0)
		this->Initialize();
	return oldRefCount + 1;
	}

int ZCountedBase::pCOMRelease()
	{
	for (;;)
		{
		const int oldRefCount = ZAtomic_Get(&fRefCount);
		if (oldRefCount == 1)
			{
			this->Finalize();
			// Hmm. At this point we cannot know if we've been destroyed.
			// Return zero as a sensible value.
			return 0;
			}
		else if (ZAtomic_CompareAndSwap(&fRefCount, oldRefCount, oldRefCount - 1))
			{
			return oldRefCount - 1;
			}
		}
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZCountedBase::WeakRefProxy

ZCountedBase::WeakRefProxy::WeakRefProxy(ZCountedBase* iCountedBase)
:	fCountedBase(iCountedBase)
	{}

ZCountedBase::WeakRefProxy::~WeakRefProxy()
	{ ZAssertStop(1, !fCountedBase); }

ZRef<ZCountedBase> ZCountedBase::WeakRefProxy::GetCountedBase()
	{
	ZAcqMtx acq(fMtx);
	return fCountedBase;
	}

void ZCountedBase::WeakRefProxy::Clear()
	{
	ZAcqMtx acq(fMtx);
	fCountedBase = nullptr;
	}

} // namespace ZooLib
