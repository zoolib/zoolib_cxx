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

class ZCountedBase::WeakRefProxy : public ZCountedWithoutFinalize
	{
public:
	WeakRefProxy(ZCountedBase* iCountedBase);
	virtual ~WeakRefProxy();

	ZRef<ZCountedBase> GetCountedBase();

	void Clear();

private:
	ZMtx fMtx;
	ZCountedBase* fCountedBase;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZWeakRef

template <class T>
class ZWeakRef
	{
public:
	ZWeakRef()
		{}

	~ZWeakRef()
		{}

	ZWeakRef(const ZWeakRef& iOther)
	:	fWeakRefProxy(iOther.fWeakRefProxy)
		{}

	ZWeakRef& operator=(const ZWeakRef& iOther)
		{
		fWeakRefProxy = iOther.fWeakRefProxy;
		return *this;
		}

	template <class O>
	ZWeakRef(const ZWeakRef<O>& iOther)
	:	fWeakRefProxy(iOther.fWeakRefProxy)
		{
		// Ensure that T is a supertype of O
		static_cast<T*>(static_cast<O*>(0));
		}

	template <class O>
	ZWeakRef& operator=(const ZWeakRef<O>& iOther)
		{
		fWeakRefProxy = iOther.fWeakRefProxy;
		return *this;
		}

	ZWeakRef(const null_t&)
		{}

	ZWeakRef(const ZRef<ZCountedBase::WeakRefProxy>& iWeakRefProxy)
	:	fWeakRefProxy(iWeakRefProxy)
		{}

	template <class O>
	ZWeakRef(const ZRef<O>& iRef)
		{
		if (iRef)
			fWeakRefProxy = iRef->GetWeakRefProxy();
		}

	template <class O>
	ZWeakRef& operator=(const ZRef<O>& iRef)
		{
		if (iRef)
			fWeakRefProxy = iRef->GetWeakRefProxy();
		else
			fWeakRefProxy.Clear();
		return *this;
		}

	void Clear()
		{ fWeakRefProxy.Clear(); }

	ZRef<T> Get() const
		{
		if (fWeakRefProxy)
			{
			if (ZRef<ZCountedBase> theCB = fWeakRefProxy->GetCountedBase())
				return theCB.DynamicCast<T>();
			}
		return null;
		}

	template <class O>
	operator ZRef<O>() const
		{
		if (fWeakRefProxy)
			{
			if (ZRef<ZCountedBase> theCB = fWeakRefProxy->GetCountedBase())
				return theCB.DynamicCast<O>();
			}
		return null;
		}

private:
	ZRef<ZCountedBase::WeakRefProxy> fWeakRefProxy;
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
