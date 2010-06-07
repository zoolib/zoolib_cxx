/* -------------------------------------------------------------------------------------------------
Copyright (c) 2009 Andrew Green
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

#include "zoolib/ZDebug.h"
#include "zoolib/ZRef_Counted.h"
#include "zoolib/ZWeakRef.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * ZWeakRefereeProxy

class ZWeakRefereeProxy : public ZCounted
	{
public:
	ZWeakRefereeProxy(ZWeakReferee* iReferee);
	virtual ~ZWeakRefereeProxy();

// From ZCounted
	virtual void Finalize();

private:
	void pDetachReferee(ZWeakReferee* iReferee);

	ZWeakReferee* pLockUse();
	void pUnlock();

	ZMtx fMtx;
	ZWeakReferee* fReferee;
	friend class ZWeakReferee;
	friend class ZWeakRefBase;
	};

ZWeakRefereeProxy::ZWeakRefereeProxy(ZWeakReferee* iReferee)
:	fReferee(iReferee)
	{}

ZWeakRefereeProxy::~ZWeakRefereeProxy()
	{
	ZAssert(!fReferee);
	}

void ZWeakRefereeProxy::Finalize()
	{
	ZGuardRMtx guard(fMtx);
	this->FinalizationComplete();
	if (!fReferee && !this->GetRefCount())
		{
		guard.Release();
		delete this;
		}
	}

void ZWeakRefereeProxy::pDetachReferee(ZWeakReferee* iReferee)
	{
	ZGuardRMtx guard(fMtx);
	ZAssert(iReferee == fReferee);
	fReferee = nullptr;
	if (!this->GetRefCount())
		{
		guard.Release();
		delete this;
		}
	}

ZWeakReferee* ZWeakRefereeProxy::pLockUse()
	{
	// Do not use a guard here -- we want to hold the lock
	// after we've returned if fReferee is non-null.
	fMtx.Acquire();
	if (fReferee)
		return fReferee;
	fMtx.Release();
	return nullptr;
	}

void ZWeakRefereeProxy::pUnlock()
	{ fMtx.Release(); }

// =================================================================================================
#pragma mark -
#pragma mark * ZWeakReferee

ZWeakReferee::ZWeakReferee()
:	fWRP(nullptr)
	{}

ZWeakReferee::~ZWeakReferee()
	{ this->pDetachProxy(); }

void ZWeakReferee::pDetachProxy()
	{
	ZAcqMtx acq(fMtx);
	if (ZWeakRefereeProxy* theWRP = fWRP)
		{
		fWRP = nullptr;
		theWRP->pDetachReferee(this);
		}
	}

ZWeakRefereeProxy* ZWeakReferee::pGetWeakRefereeProxy()
	{
	ZAcqMtx acq(fMtx);
	if (!fWRP)
		fWRP = new ZWeakRefereeProxy(this);
	return fWRP;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZWeakRefBase

ZWeakRefBase::ZWeakRefBase()
	{}

ZWeakRefBase::ZWeakRefBase(const ZWeakRefBase& iOther)
:	fWRP(iOther.fWRP)
	{}

ZWeakRefBase::ZWeakRefBase(ZWeakReferee* iWeakReferee)
	{
	if (iWeakReferee)
		fWRP = iWeakReferee->pGetWeakRefereeProxy();
	}

ZWeakRefBase::~ZWeakRefBase()
	{}

void ZWeakRefBase::pAssignFrom(const ZWeakRefBase& iOther)
	{ fWRP = iOther.fWRP; }

void ZWeakRefBase::pAssignFrom(ZWeakReferee* iWeakReferee)
	{
	if (iWeakReferee)
		fWRP = iWeakReferee->pGetWeakRefereeProxy();
	else
		fWRP.Clear();
	}

void ZWeakRefBase::pClear()
	{ fWRP.Clear(); }

ZWeakReferee* ZWeakRefBase::pLockUse() const
	{
	if (fWRP)
		return fWRP->pLockUse();
	return nullptr;
	}

void ZWeakRefBase::pUnlock() const
	{ fWRP->pUnlock(); }

} // namespace ZooLib
