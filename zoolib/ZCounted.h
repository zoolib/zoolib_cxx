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

#ifndef __ZCounted_h__
#define __ZCounted_h__ 1
#include "zconfig.h"

#include "zoolib/ZAtomic.h"
#include "zoolib/ZCountedWithoutFinalize.h"
#include "zoolib/ZDebug.h"
#include "zoolib/ZRef.h"
#include "zoolib/ZThread.h"

namespace ZooLib {

// =================================================================================================
// MARK: - ZCountedBase

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
// MARK: - sRetain/sRelase for ZCountedBase derivatives (ie ZCounted)

inline void sRetain(ZCountedBase& iObject)
	{ iObject.Retain(); }

inline void sRelease(ZCountedBase& iObject)
	{ iObject.Release(); }

inline void sCheck(ZCountedBase* iP)
	{ ZAssertStop(1, iP); }

// =================================================================================================
// MARK: - ZCounted

class ZCounted : public virtual ZCountedBase
	{};

// =================================================================================================
// MARK: - ZCountedBase::WeakRefProxy

class ZCountedBase::WeakRefProxy
:	public ZCountedWithoutFinalize
	{
private:
	WeakRefProxy(ZCountedBase* iCountedBase);
	virtual ~WeakRefProxy();

	ZRef<ZCountedBase> pGetCountedBase();
	void pClear();

	ZBen fBen;
	ZCountedBase* fCountedBase;

	friend class ZCountedBase;
	friend class ZWeakRefBase;
	};

// =================================================================================================
// MARK: - ZWeakRefBase

class ZWeakRefBase
	{
protected:
	typedef ZCountedBase::WeakRefProxy WeakRefProxy;

	ZWeakRefBase();
	~ZWeakRefBase();
	ZWeakRefBase(const ZWeakRefBase& iOther);
	ZWeakRefBase& operator=(const ZWeakRefBase& iOther);

	ZWeakRefBase(const ZRef<WeakRefProxy>& iWeakRefProxy);

	void pAssign(const ZRef<WeakRefProxy>& iWeakRefProxy);
	void pClear();

	ZRef<ZCountedBase> pGet() const;
	ZRef<WeakRefProxy> pGetWeakRefProxy() const;

private:
	ZRef<WeakRefProxy> fWeakRefProxy;
	};

// =================================================================================================
// MARK: - ZWeakRef

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
		{}

	template <class O>
	ZWeakRef& operator=(const ZWeakRef<O>& iOther)
		{
		ZWeakRefBase::operator=(iOther);
		return *this;
		}

	ZWeakRef(const ZRef<WeakRefProxy>& iWeakRefProxy)
	:	ZWeakRefBase(iWeakRefProxy)
		{}

	ZWeakRef& operator=(const ZRef<WeakRefProxy>& iWeakRefProxy)
		{
		ZWeakRefBase::pAssign(iWeakRefProxy);
		return *this;
		}

	ZWeakRef(WeakRefProxy* iWeakRefProxy)
	:	ZWeakRefBase(iWeakRefProxy)
		{}

	ZWeakRef& operator=(WeakRefProxy* iWeakRefProxy)
		{
		ZWeakRefBase::pAssign(iWeakRefProxy);
		return *this;
		}

	template <class O,bool Sense>
	ZWeakRef(const ZRef<O,Sense>& iRef)
	:	ZWeakRefBase(iRef ? iRef->GetWeakRefProxy() : null)
		{ (void)static_cast<T*>(static_cast<O*>(0)); }

	template <class O,bool Sense>
	ZWeakRef& operator=(const ZRef<O,Sense>& iRef)
		{
		(void)static_cast<T*>(static_cast<O*>(0));
		ZWeakRefBase::pAssign(iRef ? iRef->GetWeakRefProxy() : null);
		return *this;
		}

	void Clear()
		{ ZWeakRefBase::pClear(); }

	ZRef<WeakRefProxy> GetWeakRefProxy() const
		{ return ZWeakRefBase::pGetWeakRefProxy(); }

	ZRef<T> Get() const
		{
		ZRef<ZCountedBase> theCB = ZWeakRefBase::pGet();
		return theCB.DynamicCast<T>();
		}

	template <class O, bool Sense>
	operator ZRef<O,Sense>() const
		{
		(void)static_cast<T*>(static_cast<O*>(0));
		ZRef<ZCountedBase> theCB = ZWeakRefBase::pGet();
		return theCB.DynamicCast<O>();
		}

	bool operator==(const ZWeakRef& iOther) const
		{ return this->GetWeakRefProxy() == iOther.GetWeakRefProxy(); }

	bool operator!=(const ZWeakRef& iOther) const
		{ return this->GetWeakRefProxy() != iOther.GetWeakRefProxy(); }

	bool operator<(const ZWeakRef& iOther) const
		{ return this->GetWeakRefProxy() < iOther.GetWeakRefProxy(); }

	};

// =================================================================================================
// MARK: - MakeWeakRef

template <class T>
ZWeakRef<T> sWeakRef(T* iP)
	{
	if (iP)
		return ZWeakRef<T>(iP->GetWeakRefProxy());
	return null;
	}

template <class T,bool Sense>
ZWeakRef<T> sWeakRef(ZRef<T,Sense> iP)
	{
	if (iP)
		return ZWeakRef<T>(iP->GetWeakRefProxy());
	return null;
	}

} // namespace ZooLib

#endif // __ZCounted_h__
