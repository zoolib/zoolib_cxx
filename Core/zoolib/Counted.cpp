// Copyright (c) 2010 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/Counted.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - CountedBase

CountedBase::CountedBase()
:	fRefCount(0)
	{}

CountedBase::~CountedBase()
	{
	ZAssertStop(1, not fWPProxy);
	if (ZCONFIG_Debug >= 1)
		{
		const int old = sAtomic_Get(&fRefCount);
		if (0 != old)
			ZDebugStopf(1, "Non-zero refcount at destruction, it is %d", old);
		}
	}

void CountedBase::Initialize()
	{
	if (ZCONFIG_Debug >= 1)
		{
		const int old = sAtomic_Get(&fRefCount);
		if (1 != old)
			ZDebugStopf(1, "Refcount is not 1, it is %d", old);
		}
	}

void CountedBase::Finalize()
	{
	if (this->FinishFinalize())
		delete this;
	}

bool CountedBase::FinishFinalize()
	{
	// Our weak proxy may be a way to get resurrected, so kill it before we
	// check our refcount, so we can be sure that value is valid.

	if (ZP<WPProxy> theWPProxy = fWPProxy)
		{
		if (fWPProxy.AtomicCAS(theWPProxy.Get(), nullptr))
			theWPProxy->pClear();
		}

	return sAtomic_DecAndTest(&fRefCount);
	}

void CountedBase::Retain()
	{
	if (0 == sAtomic_Add(&fRefCount, 1))
		{
		ZAssertStop(1, not fWPProxy);
		this->Initialize();
		}
	}

void CountedBase::Release()
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

bool CountedBase::IsShared() const
	{ return sAtomic_Get(&fRefCount) > 1; }

bool CountedBase::IsReferenced() const
	{ return sAtomic_Get(&fRefCount) > 0; }

ZP<CountedBase::WPProxy> CountedBase::GetWPProxy()
	{
	// It is not legal to take a weak reference from an un-initialized object.
	ZAssert(sAtomic_Get(&fRefCount));

	if (not fWPProxy)
		{
		ZP<WPProxy> theWPProxy = new WPProxy(this);
		if (not fWPProxy.AtomicCAS(nullptr, theWPProxy.Get()))
			{
			// We lost the race, so clear theWPProxy's reference
			// to us, or we'll trip an asssertion in WPProxy::~WPProxy.
			theWPProxy->pClear();
			}
		}
	return fWPProxy;
	}

int CountedBase::pCOMAddRef()
	{
	const int oldRefCount = sAtomic_Add(&fRefCount, 1);
	if (oldRefCount == 0)
		this->Initialize();
	return oldRefCount + 1;
	}

int CountedBase::pCOMRelease()
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
#pragma mark - CountedBase::WPProxy

CountedBase::WPProxy::WPProxy(CountedBase* iCountedBase)
:	fCountedBase(iCountedBase)
	{}

CountedBase::WPProxy::~WPProxy()
	{ ZAssertStop(1, not fCountedBase); }

ZP<CountedBase> CountedBase::WPProxy::pGetCountedBase()
	{
	// This looks pretty innocuous, but we are incrementing the refcount
	// of fCountedBase under the protection of fMtx.
	ZAcqMtx acq(fMtx);
	return fCountedBase;
	}

void CountedBase::WPProxy::pClear()
	{
	// And here we're clearing it, but are locked out until
	// any call to pGetCountedBase has returned.
	ZAcqMtx acq(fMtx);
	fCountedBase = nullptr;
	}

// =================================================================================================
#pragma mark - WPBase

WPBase::WPBase()
	{}

WPBase::~WPBase()
	{}

WPBase::WPBase(const WPBase& iOther)
:	fWPProxy(iOther.fWPProxy)
	{}

WPBase& WPBase::operator=(const WPBase& iOther)
	{
	fWPProxy = iOther.fWPProxy;
	return *this;
	}

WPBase::WPBase(const ZP<CountedBase::WPProxy>& iWPProxy)
:	fWPProxy(iWPProxy)
	{}

void WPBase::pAssign(const ZP<CountedBase::WPProxy>& iWPProxy)
	{ fWPProxy = iWPProxy; }

void WPBase::pClear()
	{ fWPProxy.Clear(); }

ZP<CountedBase> WPBase::pGet() const
	{
	if (fWPProxy)
		return fWPProxy->pGetCountedBase();
	return null;
	}

ZP<CountedBase::WPProxy> WPBase::pGetWPProxy() const
	{ return fWPProxy; }

} // namespace ZooLib
