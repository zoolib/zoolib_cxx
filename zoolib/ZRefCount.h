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

#include "zoolib/ZCompat_algorithm.h"
#include "zoolib/ZCompat_operator_bool.h"
#include "zoolib/ZThreadSafe.h"
#include "zoolib/ZTypes.h" // For size_t

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

NAMESPACE_ZOOLIB_BEGIN

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
	ZRef(bool iIncRefCount, T* iObject);
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
#pragma mark * ZRef inline definitions

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
inline ZRef<T>::ZRef(bool iIncRefCount, T* iObject)
:	fObject(iObject)
	{
	if (iIncRefCount)
		T::sIncRefCount(fObject);
	}

template <class T>
inline ZRef<T>& ZRef<T>::operator=(T* iObject)
	{
	std::swap(iObject, fObject);
	T::sIncRefCount(fObject);
	T::sDecRefCount(iObject);	
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
#pragma mark * ZRef casts

template <class O, class T> inline O* ZRefDynamicCast(const ZRef<T>& iVal)
	{ return dynamic_cast<O*>(iVal.GetObject()); }

template <class O, class T> inline O* ZRefStaticCast(const ZRef<T>& iVal)
	{ return static_cast<O*>(iVal.GetObject()); }

// =================================================================================================

NAMESPACE_ZOOLIB_END

namespace std {
template <class T>
inline void swap(ZOOLIB_PREFIX::ZRef<T>& a, ZOOLIB_PREFIX::ZRef<T>& b)
	{ a.swap(b); }
}


#endif // __ZRefCount__
