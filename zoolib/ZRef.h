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

#include "zoolib/ZCompat_algorithm.h" // For std::swap
#include "zoolib/ZCompat_operator_bool.h"
#include "zoolib/ZDebug.h"
#include "zoolib/ZTypes.h" // For Adopt_T

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * ZRef

template <class T>
class ZRef
	{
private:
	static void spRetain(T* iP)
		{
		if (iP)
			sRetain(*iP);
		}

	static void spRelease(T* iP)
		{
		if (iP)
			sRelease(*iP);
		}

	static void spCheck(T* iP)
		{
		ZAssert(iP);
		}

public:
//	ZOOLIB_DEFINE_OPERATOR_BOOL_TYPES_T(ZRef,
//		operator_bool_generator_type, operator_bool_type);

//	operator operator_bool_type() const
//		{ return operator_bool_generator_type::translate(fP); }

	// Needed for Objective C method invocation. Need to conditionalize this
	// vs operator_bool_type based on T being derived from objc_object.
	operator T*() const { return fP; }

	void swap(ZRef& iOther)
		{ std::swap(fP, iOther.fP); }

	ZRef()
	:	fP(nullptr)
		{}

	ZRef(const ZRef& iOther)
	:	fP(iOther.Get())
		{ spRetain(fP); }

	~ZRef()
		{ spRelease(fP); } 

	ZRef& operator=(const ZRef& iOther)
		{
		T* theP = iOther.Get();
		std::swap(theP, fP);
		spRetain(fP);
		spRelease(theP);
		return *this;
		}

	ZRef(T* iP)
	:	fP(iP)
		{ spRetain(fP); }
	
	ZRef& operator=(T* iP)
		{
		std::swap(iP, fP);
		spRetain(fP);
		spRelease(iP);	
		return *this;
		}
	
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

	ZRef(const Adopt_T<T>& iNRP)
	:	fP(iNRP.Get())
		{}

	ZRef& operator=(const Adopt_T<T>& iNRP)
		{
		T* theP = iNRP.Get();
		std::swap(theP, fP);
		spRelease(theP);
		return *this;
		}

	bool operator==(const T* iP) const
		{ return fP == iP; }

	bool operator!=(const T* iP) const
		{ return fP != iP; }

	bool operator==(const ZRef& iOther) const
		{ return fP == iOther.Get(); }

	bool operator!=(const ZRef& iOther) const
		{ return fP != iOther.Get(); }

	template <class O>
	bool operator==(const ZRef<O>& iOther) const
		{ return fP == iOther.Get(); }

	template <class O>
	bool operator!=(const ZRef<O>& iOther) const
		{ return fP != iOther.Get(); }

	template <class O>
	bool operator<(const ZRef<O>& iOther) const
		{ return fP < iOther.Get(); }

	T* operator->() const
		{
		spCheck(fP);
		return fP;
		}

	T* Copy() const
		{
		spRetain(fP);
		return fP;
		}

	T* Get() const { return fP; }

	void Clear()
		{
		T* theP = nullptr;
		std::swap(theP, fP);
		spRelease(theP);
		}

	// Used with COM output parameters. See sCOMPtr and sCOMVoidPtr in ZWinCOM.h
	T*& GetPtrRef() { return fP; }

	template <class O>
	O* DynamicCast() const
		{ return dynamic_cast<O*>(fP); }

	template <class O>
	O* StaticCast() const
		{ return static_cast<O*>(fP); }

private:
	T* fP;
	};

template <class T>
void sRefCopy(void* iDest, T* iP)
	{
	*static_cast<T**>(iDest) = iP;
	if (iP)
		sRetain(*iP);
	}

// =================================================================================================
#pragma mark -
#pragma mark * NoRetain wrapper

template <class P>
Adopt_T<P> NoRetain(P iP)
	{ return Adopt_T<P>(iP); }

// =================================================================================================
#pragma mark -
#pragma mark * ZRef partially specialized for pointer types

template <class T> void sRetain_T(T& ioPtr);
template <class T> void sRelease_T(T iPtr);

template <class T>
class ZRef<T*>
	{
private:
	static void spRetain(T*& iP)
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
	:	fP(nullptr)
		{}

	ZRef(const ZRef& iOther)
	:	fP(iOther.Get())
		{ spRetain(fP); }

	~ZRef()
		{ spRelease(fP); } 

	ZRef& operator=(const ZRef& iOther)
		{
		T* theP = iOther.Get();
		std::swap(theP, fP);
		spRetain(fP);
		spRelease(theP);
		return *this;
		}

	ZRef(T* iP)
	:	fP(iP)
		{ spRetain(fP); }
	
	ZRef& operator=(T* iP)
		{
		std::swap(iP, fP);
		spRetain(fP);
		spRelease(iP);	
		return *this;
		}
	
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

	ZRef(const Adopt_T<T*>& iNRP)
	:	fP(iNRP.Get())
		{}

	ZRef& operator=(const Adopt_T<T*>& iNRP)
		{
		T* theP = iNRP.Get();
		std::swap(theP, fP);
		spRelease(theP);
		return *this;
		}

	bool operator==(const T* iP) const
		{ return fP == iP; }

	bool operator!=(const T* iP) const
		{ return fP != iP; }

	bool operator==(const ZRef& iOther) const
		{ return fP == iOther.Get(); }

	bool operator!=(const ZRef& iOther) const
		{ return fP != iOther.Get(); }

	template <class O>
	bool operator==(const ZRef<O>& iOther) const
		{ return fP == iOther.Get(); }

	template <class O>
	bool operator!=(const ZRef<O>& iOther) const
		{ return fP != iOther.Get(); }

	template <class O>
	bool operator<(const ZRef<O>& iOther) const
		{ return fP < iOther.Get(); }

	operator T*() { return fP; }

	operator T*() const { return fP; }

	T* Get() const { return fP; }

	void Clear()
		{
		T* theP = nullptr;
		std::swap(theP, fP);
		spRelease(theP);
		}

	T* Orphan()
		{
		T* theP = nullptr;
		std::swap(theP, fP);
		return theP;
		}

	// Used with output parameters.
	T*& GetPtrRef() { return fP; }
	T*& OParam()
		{
		this->Clear();
		return this->GetPtrRef();
		}

	template <class O>
	O StaticCast() const
		{ return static_cast<O>(fP); }

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

template <class O, class T> inline O ZRefStaticCast(const ZRef<T*>& iVal)
	{ return static_cast<O>(iVal.Get()); }

template <class O, class T> inline O ZRefStaticCast(const ZRef<const T*>& iVal)
	{ return static_cast<O>(iVal.Get()); }

// =================================================================================================
#pragma mark -
#pragma mark * ZooLib::nullref, akin to std::nullptr

const class nullref_t
	{
public:
	template <class T> operator ZRef<T>() const { return ZRef<T>(); }
private:
	void operator&() const;
	} nullref = {};

NAMESPACE_ZOOLIB_END

namespace std {
template <class T>
inline void swap(ZOOLIB_PREFIX::ZRef<T>& a, ZOOLIB_PREFIX::ZRef<T>& b)
	{ a.swap(b); }
}

#endif // __ZRef__
