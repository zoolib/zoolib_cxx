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

#ifndef __ZCounted__
#define __ZCounted__ 1
#include "zconfig.h"

#include "zoolib/ZAtomic.h"
#include "zoolib/ZCountedWithoutFinalize.h"
#include "zoolib/ZDebug.h"
#include "zoolib/ZRef.h"
#include "zoolib/ZThread.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * ZCountedBase

class ZCountedBase
	{
public:
	ZCountedBase();
	virtual ~ZCountedBase();

	virtual void Initialize();
	virtual void Finalize();

	bool FinishFinalize();

	void Retain();
	void Release();
	bool IsShared() const;
	bool IsReferenced() const;

	class WeakRefProxy;
	ZRef<WeakRefProxy> GetWeakRefProxy();

protected:
	int pCOMAddRef();
	int pCOMRelease();

private:
	ZAtomic_t fRefCount;
	ZRef<WeakRefProxy> fWeakRefProxy;
	};

// =================================================================================================
#pragma mark -
#pragma mark * sRetain/sRelase for ZCountedBase derivatives (ie ZCounted)

inline void sRetain(ZCountedBase& iObject)
	{ iObject.Retain(); }

inline void sRelease(ZCountedBase& iObject)
	{ iObject.Release(); }

inline void sCheck(ZCountedBase* iP)
	{ ZAssertStop(1, iP); }

// =================================================================================================
#pragma mark -
#pragma mark * ZCounted

class ZCounted : public virtual ZCountedBase
	{};

// =================================================================================================
#pragma mark -
#pragma mark * ZCountedBase::WeakRefProxy

class ZCountedBase::WeakRefProxy
:	public ZCountedWithoutFinalize
	{
private:
	WeakRefProxy(ZCountedBase* iCountedBase);
	virtual ~WeakRefProxy();

	ZRef<ZCountedBase> pGetCountedBase();
	void pClear();

	ZMtx fMtx;
	ZCountedBase* fCountedBase;

	friend class ZCountedBase;
	friend class ZWeakRefBase;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZWeakRefBase

class ZWeakRefBase
	{
protected:
	ZWeakRefBase();
	~ZWeakRefBase();
	ZWeakRefBase(const ZWeakRefBase& iOther);
	ZWeakRefBase& operator=(const ZWeakRefBase& iOther);

	ZWeakRefBase(const ZRef<ZCountedBase::WeakRefProxy>& iWeakRefProxy);

	void pAssign(const ZRef<ZCountedBase::WeakRefProxy>& iWeakRefProxy);
	void pClear();

	ZRef<ZCountedBase> pGet() const;
	ZRef<ZCountedBase::WeakRefProxy> pGetWeakRefProxy() const;

private:
	ZRef<ZCountedBase::WeakRefProxy> fWeakRefProxy;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZWeakRef

template <class T>
class ZWeakRef
:	protected ZWeakRefBase
	{
public:
	ZWeakRef()
		{}

	~ZWeakRef()
		{}

	ZWeakRef(const ZWeakRef& iOther)
	:	ZWeakRefBase(iOther)
		{}

	ZWeakRef& operator=(const ZWeakRef& iOther)
		{
		ZWeakRefBase::operator=(iOther);
		return *this;
		}

	ZWeakRef(const null_t&)
		{}

	template <class O>
	ZWeakRef(const ZWeakRef<O>& iOther)
	:	ZWeakRefBase(iOther)
		{
		// Ensure that T is a supertype of O
		(void)static_cast<T*>(static_cast<O*>(0));
		}

	template <class O>
	ZWeakRef& operator=(const ZWeakRef<O>& iOther)
		{
		(void)static_cast<T*>(static_cast<O*>(0));
		ZWeakRefBase::operator=(iOther);
		return *this;
		}

	ZWeakRef(const ZRef<ZCountedBase::WeakRefProxy>& iWeakRefProxy)
	:	ZWeakRefBase(iWeakRefProxy)
		{}

	ZWeakRef& operator=(const ZRef<ZCountedBase::WeakRefProxy>& iWeakRefProxy)
		{
		ZWeakRefBase::pAssign(iWeakRefProxy);
		return *this;
		}

	ZWeakRef(ZCountedBase::WeakRefProxy* iWeakRefProxy)
	:	ZWeakRefBase(iWeakRefProxy)
		{}

	ZWeakRef& operator=(ZCountedBase::WeakRefProxy* iWeakRefProxy)
		{
		ZWeakRefBase::pAssign(iWeakRefProxy);
		return *this;
		}

	template <class O>
	ZWeakRef(const ZRef<O>& iRef)
	:	ZWeakRefBase(iRef ? iRef->GetWeakRefProxy() : null)
		{ (void)static_cast<T*>(static_cast<O*>(0)); }

	template <class O>
	ZWeakRef& operator=(const ZRef<O>& iRef)
		{
		(void)static_cast<T*>(static_cast<O*>(0));
		ZWeakRefBase::pAssign(iRef ? iRef->GetWeakRefProxy() : null);
		return *this;
		}

	void Clear()
		{ ZWeakRefBase::pClear(); }

	ZRef<ZCountedBase::WeakRefProxy> GetWeakRefProxy() const
		{ return ZWeakRefBase::pGetWeakRefProxy(); }

	ZRef<T> Get() const
		{
		ZRef<ZCountedBase> theCB = ZWeakRefBase::pGet();
		return theCB.DynamicCast<T>();
		}

	template <class O>
	operator ZRef<O,false>() const
		{
		(void)static_cast<T*>(static_cast<O*>(0));
		ZRef<ZCountedBase> theCB = ZWeakRefBase::pGet();
		return theCB.DynamicCast<O>();
		}

	template <class O>
	operator ZRef<O,true>() const
		{
		(void)static_cast<T*>(static_cast<O*>(0));
		ZRef<ZCountedBase> theCB = ZWeakRefBase::pGet();
		return theCB.DynamicCast<O>();
		}
	};

// =================================================================================================
#pragma mark -
#pragma mark * MakeWeakRef

template <class T>
ZWeakRef<T> MakeWeakRef(T* iP)
	{
	if (iP)
		return ZWeakRef<T>(iP->GetWeakRefProxy());
	return null;
	}

} // namespace ZooLib

#endif // __ZCounted__
