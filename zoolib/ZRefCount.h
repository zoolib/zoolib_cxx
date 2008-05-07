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

#ifndef __ZRefCount__
#define __ZRefCount__ 1
#include "zconfig.h"

#include "ZThread.h" // For ZThreadSafe_Get, ZThreadSafe_Inc && ZThreadSafe_DecAndTest

#include "ZCompat_operator_bool.h"

/*
Checked/slow versus unchecked/fast access to refcounted objects can be enabled
on a class by class basis. Simply declare/define in your class a method thus:

  static inline bool sCheckAccessEnabled() { return true; }

Have it return true if access checks should be made when using ZRefs to instances
of this class, or false if not.

The implementations provided by ZRefCounted and ZRefCountedWithFinalization return
whatever is #defined in ZCONFIG_RefCount_CheckAccessDefault. If no definition of
ZCONFIG_RefCount_CheckAccessDefault exists then this file defines it to be true if
ZCONFIG_Debug is greater than zero. So for debug builds we default to checked access, and
for non-debug builds we default to unchecked access.

NB The method must be declared public, otherwise ZRef<> will not be able to access it.
*/

#ifndef ZCONFIG_RefCount_CheckAccessDefault
#	define ZCONFIG_RefCount_CheckAccessDefault (ZCONFIG_Debug > 0)
#endif

// =================================================================================================
#pragma mark -
#pragma mark * ZRefCounted

class ZRefCounted
	{
public:
	ZRefCounted() { ZThreadSafe_Set(fRefCount, 0); }
	virtual ~ZRefCounted();

	int GetRefCount() const { return ZThreadSafe_Get(fRefCount); }

	static void sIncRefCount(ZRefCounted* iObject)
		{
		if (iObject)
			ZThreadSafe_Inc(iObject->fRefCount);
		}
	static void sDecRefCount(ZRefCounted* iObject)
		{
		if (iObject && ZThreadSafe_DecAndTest(iObject->fRefCount))
			delete iObject;
		}

	static void sCheckAccess(ZRefCounted* iObject);

	static bool sCheckAccessEnabled() { return ZCONFIG_RefCount_CheckAccessDefault; }

private:
	ZThreadSafe_t fRefCount;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZRefCountedWithFinalization

class ZRefCountedWithFinalization
	{
public:
	ZRefCountedWithFinalization() { ZThreadSafe_Set(fRefCount, 0); }
	virtual ~ZRefCountedWithFinalization();

	int GetRefCount() const { return ZThreadSafe_Get(fRefCount); }

	virtual void Initialize();

	virtual void Finalize();

	void FinalizationComplete();

	static void sIncRefCount(ZRefCountedWithFinalization* iObject)
		{
		if (iObject && 0 == ZThreadSafe_IncReturnOld(iObject->fRefCount))
			iObject->Initialize();
		}

	static void sDecRefCount(ZRefCountedWithFinalization* iObject);

	static void sCheckAccess(ZRefCountedWithFinalization* iObject);

	static bool sCheckAccessEnabled() { return ZCONFIG_RefCount_CheckAccessDefault; }

protected:
	int AddRef();
	int Release();

private:
	ZThreadSafe_t fRefCount;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZRef declaration

template <class S> class ZRefSafe;

template <class T>
class ZRef
	{
private:
    ZOOLIB_DEFINE_OPERATOR_BOOL_TYPES_T(ZRef<T>,
    	operator_bool_generator_type, operator_bool_type);

public:
	ZRef();
	~ZRef();

	ZRef(T* iObject);
	ZRef& operator=(T* iObject);
	bool operator==(const T* iObject) const;
	bool operator!=(const T* iObject) const;

	ZRef(const ZRef& iOther);
	ZRef& operator=(const ZRef& iOther);
	bool operator==(const ZRef& iOther) const;
	bool operator!=(const ZRef& iOther) const;

	template <class O> ZRef(const ZRef<O>& iOther);
	template <class O> ZRef& operator=(const ZRef<O>& iOther);
	template <class O> bool operator==(const ZRef<O>& iOther) const;
	template <class O> bool operator!=(const ZRef<O>& iOther) const;
	template <class O> bool operator<(const ZRef<O>& iOther) const;

	template <class S> ZRef(const ZRefSafe<S>& iOther);
	template <class S> ZRef& operator=(const ZRefSafe<S>& iOther);

	T* operator->() const;

	operator operator_bool_type() const
		{ return operator_bool_generator_type::translate(fObject); }

	void Clear();

	T* GetObject() const { return fObject; }

	void swap(ZRef& iOther) { std::swap(fObject, iOther.fObject); }

private:
	T* fObject;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZRefSafe declaration

template <class S>
class ZRefSafe
	{
    ZOOLIB_DEFINE_OPERATOR_BOOL_TYPES_T(ZRefSafe<S>,
    	operator_bool_generator_type, operator_bool_type);

	ZRefSafe(const ZRefSafe&); // Not implemented
	const ZRefSafe& operator=(const ZRefSafe&); // Not implemented
public:
	ZRefSafe();
	~ZRefSafe();

	ZRefSafe(S* iObject);
	ZRefSafe& operator=(S* iObject);

	template <class T> ZRefSafe(const ZRef<T>& iOther);
	template <class T> ZRefSafe& operator=(const ZRef<T>& iOther);

	operator operator_bool_type() const
		{ return operator_bool_generator_type::translate(fObject); }

	S* GetObject() const { return fObject; }

	void Acquire() const { fMutex.Acquire(); }
	void Release() const { fMutex.Release(); }

private:
	mutable ZMutexNR fMutex;
	S* fObject;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZRef inline definitions

namespace std {
template <class T>
inline void swap(ZRef<T>& a, ZRef<T>& b)
	{ a.swap(b); }
}

template <class T>
inline ZRef<T>::ZRef()
:	fObject(nil)
	{}

template <class T>
inline ZRef<T>::~ZRef()
	{
	T::sDecRefCount(fObject);
	}

template <class T>
inline ZRef<T>::ZRef(T* iObject)
:	fObject(iObject)
	{
	T::sIncRefCount(fObject);
	}

template <class T>
inline ZRef<T>& ZRef<T>::operator=(T* iObject)
	{
#if 1
	std::swap(iObject, fObject);
	T::sIncRefCount(fObject);
	T::sDecRefCount(iObject);	
#else
	T::sIncRefCount(iObject);
	T::sDecRefCount(fObject);
	fObject = iObject;
#endif
	return *this;
	}

template <class T>
inline bool ZRef<T>::operator==(const T* iObject) const
	{ return fObject == iObject; }

template <class T>
inline bool ZRef<T>::operator!=(const T* iObject) const
	{ return fObject != iObject; }

template <class T>
inline ZRef<T>::ZRef(const ZRef& iOther)
:	fObject(iOther.GetObject())
	{
	T::sIncRefCount(fObject);
	}

template <class T>
inline ZRef<T>& ZRef<T>::operator=(const ZRef& iOther)
	{
	T::sIncRefCount(iOther.GetObject());
	T::sDecRefCount(fObject);
	fObject = iOther.GetObject();
	return *this;
	}

template <class T>
inline bool ZRef<T>::operator==(const ZRef& iOther) const
	{ return fObject == iOther.GetObject(); }

template <class T>
inline bool ZRef<T>::operator!=(const ZRef& iOther) const
	{ return fObject != iOther.GetObject(); }

template <class T> template <class O>
inline ZRef<T>::ZRef(const ZRef<O>& iOther)
:	fObject(iOther.GetObject())
	{
	T::sIncRefCount(fObject);
	}

template <class T> template <class O>
inline ZRef<T>& ZRef<T>::operator=(const ZRef<O>& iOther)
	{
	O::sIncRefCount(iOther.GetObject());
	T::sDecRefCount(fObject);
	fObject = iOther.GetObject();
	return *this;
	}

template <class T> template <class O>
inline bool ZRef<T>::operator==(const ZRef<O>& iOther) const
	{ return fObject == iOther.GetObject(); }

template <class T> template <class O>
inline bool ZRef<T>::operator!=(const ZRef<O>& iOther) const
	{ return fObject != iOther.GetObject(); }

template <class T> template <class O>
inline bool ZRef<T>::operator<(const ZRef<O>& iOther) const
	{ return fObject < iOther.GetObject(); }

template <class T> template <class S>
ZRef<T>::ZRef(const ZRefSafe<S>& iOther)
	{
	iOther.Acquire();
	fObject = iOther.GetObject();
	T::sIncRefCount(fObject);
	iOther.Release();
	}

template <class T> template <class S>
ZRef<T>& ZRef<T>::operator=(const ZRefSafe<S>& iOther)
	{
	iOther.Acquire();
	T* temp = iOther.GetObject();
	S::sIncRefCount(temp);
	iOther.Release();

	T::sDecRefCount(fObject);
	fObject = temp;
	}

template <class T>
inline T* ZRef<T>::operator->() const
	{
	if (T::sCheckAccessEnabled())
		T::sCheckAccess(fObject);
	return fObject;
	}

template <class T>
inline void ZRef<T>::Clear()
	{
	T::sDecRefCount(fObject);
	fObject = nil;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZRefSafe inline definitions

template <class S>
inline ZRefSafe<S>::ZRefSafe()
:	fObject(nil)
	{}

template <class S>
inline ZRefSafe<S>::~ZRefSafe()
	{
	S::sDecRefCount(fObject);
	}

template <class S>
inline ZRefSafe<S>::ZRefSafe(S* iObject)
:	fObject(iObject)
	{
	S::sIncRefCount(iObject);
	}

template <class S>
inline ZRefSafe<S>& ZRefSafe<S>::operator=(S* iObject)
	{
	S::sIncRefCount(iObject);

	fMutex.Acquire();
	S* temp = fObject;
	fObject = iObject;
	fMutex.Release();

	S::sDecRefCount(temp);
	return *this;
	}

template <class S> template <class T>
inline ZRefSafe<S>::ZRefSafe(const ZRef<T>& iOther)
:	fObject(iOther.GetObject())
	{
	S::sIncRefCount(fObject);
	}

template<class S> template <class T>
inline ZRefSafe<S>& ZRefSafe<S>::operator=(const ZRef<T>& iOther)
	{
	T::sIncRefCount(iOther.GetObject());

	fMutex.Acquire();
	S* temp = fObject;
	fObject = iOther.GetObject();
	fMutex.Release();

	S::sDecRefCount(temp);
	return *this;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZRef casts

template <class O, class T> inline O* ZRefDynamicCast(const ZRef<T>& iVal)
	{ return dynamic_cast<O*>(iVal.GetObject()); }

template <class O, class T> inline O* ZRefStaticCast(const ZRef<T>& iVal)
	{ return static_cast<O*>(iVal.GetObject()); }

// =================================================================================================

#endif // __ZRefCount__
