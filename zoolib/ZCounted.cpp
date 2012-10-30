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
// MARK: - ZCountedBase

ZCountedBase::ZCountedBase()
:	fRefCount(0)
	{}

ZCountedBase::~ZCountedBase()
	{
	ZAssertStop(1, not fWeakRefProxy);
	if (ZCONFIG_Debug >= 1)
		{
		const int old = sAtomic_Get(&fRefCount);
		if (0 != old)
			ZDebugStopf(1, "Non-zero refcount at destruction, it is %d", old);
		}
	}

void ZCountedBase::Initialize()
	{
	if (ZCONFIG_Debug >= 1)
		{
		const int old = sAtomic_Get(&fRefCount);
		if (1 != old)
			ZDebugStopf(1, "Refcount is not 1, it is %d", old);
		}
	}

void ZCountedBase::Finalize()
	{
	if (this->FinishFinalize())
		delete this;
	}

bool ZCountedBase::FinishFinalize()
	{
	if (not sAtomic_DecAndTest(&fRefCount))
		return false;

	if (fWeakRefProxy)
		{
		fWeakRefProxy->pClear();
		fWeakRefProxy.Clear();
		}

	return true;
	}

void ZCountedBase::Retain()
	{
	if (0 == sAtomic_Add(&fRefCount, 1))
		{
		ZAssertStop(1, not fWeakRefProxy);
		this->Initialize();
		}
	}

void ZCountedBase::Release()
	{
	for (;;)
		{
		const int oldRefCount = sAtomic_Get(&fRefCount);
		if (oldRefCount == 1)
			{
			this->Finalize();
			return;
			}

		if (sAtomic_CAS(&fRefCount, oldRefCount, oldRefCount - 1))
			return;
		}
	}

bool ZCountedBase::IsShared() const
	{ return sAtomic_Get(&fRefCount) > 1; }

bool ZCountedBase::IsReferenced() const
	{ return sAtomic_Get(&fRefCount) > 0; }

ZRef<ZCountedBase::WeakRefProxy> ZCountedBase::GetWeakRefProxy()
	{
	// It is not legal to take a weak reference from an un-initialized object.
	ZAssert(sAtomic_Get(&fRefCount));

	if (not fWeakRefProxy)
		{
		ZRef<WeakRefProxy> theWeakRefProxy = new WeakRefProxy(this);
		if (not fWeakRefProxy.AtomicCAS(nullptr, theWeakRefProxy.Get()))
			{
			// We lost the race, so clear theWeakRefProxy's reference
			// to us, or we'll trip an asssertion in WeakRefProxy::~WeakRefProxy.
			theWeakRefProxy->pClear();
			}
		}
	return fWeakRefProxy;
	}

int ZCountedBase::pCOMAddRef()
	{
	const int oldRefCount = sAtomic_Add(&fRefCount, 1);
	if (oldRefCount == 0)
		this->Initialize();
	return oldRefCount + 1;
	}

int ZCountedBase::pCOMRelease()
	{
	for (;;)
		{
		const int oldRefCount = sAtomic_Get(&fRefCount);
		if (oldRefCount == 1)
			{
			this->Finalize();
			// Hmm. At this point we cannot know if we've been destroyed.
			// Return zero as a sensible value.
			return 0;
			}
		else if (sAtomic_CAS(&fRefCount, oldRefCount, oldRefCount - 1))
			{
			return oldRefCount - 1;
			}
		}
	}

// =================================================================================================
// MARK: - ZCountedBase::WeakRefProxy

ZCountedBase::WeakRefProxy::WeakRefProxy(ZCountedBase* iCountedBase)
:	fCountedBase(iCountedBase)
	{}

ZCountedBase::WeakRefProxy::~WeakRefProxy()
	{ ZAssertStop(1, not fCountedBase); }

ZRef<ZCountedBase> ZCountedBase::WeakRefProxy::pGetCountedBase()
	{
	// This looks pretty innocuous, but we are incrementing the refcount
	// of fCountedBase under the protection of fBen.
	ZAcqBen acq(fBen);
	return fCountedBase;
	}

void ZCountedBase::WeakRefProxy::pClear()
	{
	// And here we're clearing it, but are locked out until
	// any call to pGetCountedBase has returned.
	ZAcqBen acq(fBen);
	fCountedBase = nullptr;
	}

// =================================================================================================
// MARK: - ZWeakRefBase

ZWeakRefBase::ZWeakRefBase()
	{}

ZWeakRefBase::~ZWeakRefBase()
	{}

ZWeakRefBase::ZWeakRefBase(const ZWeakRefBase& iOther)
:	fWeakRefProxy(iOther.fWeakRefProxy)
	{}

ZWeakRefBase& ZWeakRefBase::operator=(const ZWeakRefBase& iOther)
	{
	fWeakRefProxy = iOther.fWeakRefProxy;
	return *this;
	}

ZWeakRefBase::ZWeakRefBase(const ZRef<ZCountedBase::WeakRefProxy>& iWeakRefProxy)
:	fWeakRefProxy(iWeakRefProxy)
	{}

void ZWeakRefBase::pAssign(const ZRef<ZCountedBase::WeakRefProxy>& iWeakRefProxy)
	{ fWeakRefProxy = iWeakRefProxy; }

void ZWeakRefBase::pClear()
	{ fWeakRefProxy.Clear(); }

ZRef<ZCountedBase> ZWeakRefBase::pGet() const
	{
	if (fWeakRefProxy)
		return fWeakRefProxy->pGetCountedBase();
	return null;
	}

ZRef<ZCountedBase::WeakRefProxy> ZWeakRefBase::pGetWeakRefProxy() const
	{ return fWeakRefProxy; }

} // namespace ZooLib
