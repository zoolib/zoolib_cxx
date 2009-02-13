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

#ifndef __ZRef__
#define __ZRef__ 1
#include "zconfig.h"

#include "zoolib/ZCompat_algorithm.h"
#include "zoolib/ZCompat_operator_bool.h"

#include "zoolib/ZDebug.h"

NAMESPACE_ZOOLIB_BEGIN

template <class T> void sRetain_T(T);
template <class T> void sRelease_T(T);

// =================================================================================================
#pragma mark -
#pragma mark * ZRef

template <typename T>
class ZRef
	{
private:
	ZOOLIB_DEFINE_OPERATOR_BOOL_TYPES_T(ZRef,
		operator_bool_generator_type, operator_bool_type);

	typedef T* P;

	static void spRetain(P iP)
		{
		if (iP)
			sRetain(*iP);
		}

	static void spRelease(P iP)
		{
		if (iP)
			sRelease(*iP);
		}

	static void spCheck(P iP)
		{
		ZAssert(iP);
//		if (sCheckAccessEnabled(Retainable_T<T>::sGet(iP))
//			sCheckAccess(Retainable_T<T>::sGet(iP))
		}

public:
	void swap(ZRef& iOther)
		{ std::swap(fP, iOther.fP); }

	ZRef()
	:	fP(nil)
		{}

	~ZRef()
		{ spRelease(fP); } 

	ZRef(P iP)
	:	fP(iP)
		{ spRetain(fP); }
	
	ZRef& operator=(P iP)
		{
		// Important to do the retain after we've set fP,
		// so that ZRefCountedWithFinalization's Initialize is
		// called *after* we've taken the reference to it.
		std::swap(iP, fP);
		spRetain(fP);
		spRelease(iP);	
		return *this;
		}
	
	bool operator==(const P iP) const
		{ return fP == iP; }

	bool operator!=(const P iP) const
		{ return fP != iP; }

	ZRef(const ZRef& iOther)
	:	fP(iOther.Get())
		{ spRetain(fP); }

	ZRef& operator=(const ZRef& iOther)
		{
		P theP = iOther.Get();
		std::swap(theP, fP);
		spRetain(fP);
		spRelease(theP);
		return *this;
		}

	bool operator==(const ZRef& iOther) const
		{ return fP == iOther.Get(); }

	bool operator!=(const ZRef& iOther) const
		{ return fP != iOther.Get(); }

	template <class O>
	ZRef(const ZRef<O>& iOther)
	:	fP(iOther.Get())
		{ spRetain(fP); }

	template <class O>
	ZRef& operator=(const ZRef<O>& iOther)
		{
		P theP = iOther.Get();
		std::swap(theP, fP);
		spRetain(fP);
		spRelease(theP);
		return *this;
		}

	template <class O>
	bool operator==(const ZRef<O>& iOther) const
		{ return fP == iOther.Get(); }

	template <class O>
	bool operator!=(const ZRef<O>& iOther) const
		{ return fP != iOther.Get(); }

	template <class O>
	bool operator<(const ZRef<O>& iOther) const
		{ return fP < iOther.Get(); }

	operator operator_bool_type() const
		{ return operator_bool_generator_type::translate(fP); }

	T* operator->() const
		{
		spCheck(fP);
		return fP;
		}

	P Get() const { return fP; }
	P GetObject() const { return fP; }

	void Clear()
		{
		P theP = nil;
		std::swap(theP, fP);
		spRelease(theP);
		}

	// Used with COM output parameters. See sCOMPtr and sCOMVoidPtr in ZWinCOM.h
	P& GetPtrRef() { return fP; }

private:
	P fP;
	};

template <class T>
void sRefCopy(void* iDest, T* iP)
	{
	iDest = iP;
	if (iP)
		sRetain(*iP);
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZRef partially specialized for pointer types

template <typename T>
struct NoRetain_t
	{
	mutable T fP;
	NoRetain_t(T iP) : fP(iP) {}
	};

template <typename T>
NoRetain_t<T> NoRetain(T iP)
	{ return NoRetain_t<T>(iP); }

template <typename T>
class ZRef<T*>
	{
private:
	ZOOLIB_DEFINE_OPERATOR_BOOL_TYPES_T(ZRef,
		operator_bool_generator_type, operator_bool_type);

	static void spRetain(T* iP)
		{
		if (iP)
			sRetain_T(iP);
		}

	static void spRelease(T* iP)
		{
		if (iP)
			sRelease_T(iP);
		}

public:
	void swap(ZRef& iOther)
		{ std::swap(fP, iOther.fP); }

	ZRef()
	:	fP(nil)
		{}

	~ZRef()
		{ spRelease(fP); } 

	ZRef(T* iP)
	:	fP(iP)
		{ spRetain(fP); }
	
	ZRef(const NoRetain_t<T*>& iNRP)
	:	fP(iNRP.fP)
		{}

	ZRef& operator=(T* iP)
		{
		// Important to do the retain after we've set fP,
		// so that ZRefCountedWithFinalization's Initialize is
		// called *after* we've taken the reference to it.
		std::swap(iP, fP);
		spRetain(fP);
		spRelease(iP);	
		return *this;
		}
	
	ZRef& operator=(const NoRetain_t<T*>& iNRP)
		{
		T* theP = iNRP.fP;
		std::swap(theP, fP);
		spRelease(theP);
		return *this;
		}

	bool operator==(const T* iP) const
		{ return fP == iP; }

	bool operator!=(const T* iP) const
		{ return fP != iP; }

	ZRef(const ZRef& iOther)
	:	fP(iOther.Get())
		{ spRetain(fP); }

	ZRef& operator=(const ZRef& iOther)
		{
		T* theP = iOther.Get();
		std::swap(theP, fP);
		spRetain(fP);
		spRelease(theP);
		return *this;
		}

	bool operator==(const ZRef& iOther) const
		{ return fP == iOther.Get(); }

	bool operator!=(const ZRef& iOther) const
		{ return fP != iOther.Get(); }

	template <class O>
	ZRef(const ZRef<O>& iOther)
	:	fP(iOther.Get())
		{ spRetain(fP); }

	template <class O>
	ZRef& operator=(const ZRef<O>& iOther)
		{
		T* theP = iOther.Get();
		std::swap(theP, fP);
		spRetain(fP);
		spRelease(theP);
		return *this;
		}

	template <class O>
	bool operator==(const ZRef<O>& iOther) const
		{ return fP == iOther.Get(); }

	template <class O>
	bool operator!=(const ZRef<O>& iOther) const
		{ return fP != iOther.Get(); }

	template <class O>
	bool operator<(const ZRef<O>& iOther) const
		{ return fP < iOther.Get(); }

	operator operator_bool_type() const
		{ return operator_bool_generator_type::translate(fP); }

	operator T*()
		{ return fP; }

	T* Get() const { return fP; }

	void Clear()
		{
		T* theP = nil;
		std::swap(theP, fP);
		spRelease(theP);
		}

private:
	T* fP;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZRef casts

template <class O, class T> inline O* ZRefDynamicCast(const ZRef<T>& iVal)
	{ return dynamic_cast<O*>(iVal.Get()); }

template <class O, class T> inline O* ZRefStaticCast(const ZRef<T>& iVal)
	{ return static_cast<O*>(iVal.Get()); }

// =================================================================================================

NAMESPACE_ZOOLIB_END

namespace std {
template <class T>
inline void swap(ZOOLIB_PREFIX::ZRef<T>& a, ZOOLIB_PREFIX::ZRef<T>& b)
	{ a.swap(b); }
}

#endif // __ZRef__
