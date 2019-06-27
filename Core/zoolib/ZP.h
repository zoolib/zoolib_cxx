/* -------------------------------------------------------------------------------------------------
Copyright (c) 2019 Andrew Green
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

#ifndef __ZooLib_ZP_h__
#define __ZooLib_ZP_h__ 1
#include "zconfig.h"

#if not defined(__OBJC__)
	#include "zoolib/Compat_operator_bool.h"
#endif

#include "zoolib/Compat_algorithm.h" // For std::swap
#include "zoolib/Not.h" // For std::swap

#include "zoolib/ZAtomic.h" // For sAtomic_CASPtr
#include "zoolib/ZTypes.h" // For Adopt_T

#ifndef ZMACRO_Attribute_NoThrow_ZP
	#define ZMACRO_Attribute_NoThrow_ZP ZMACRO_Attribute_NoThrow
#endif

namespace ZooLib {

// =================================================================================================
#pragma mark - ZP

template <class T>
class ZP
	{
	#if defined(__APPLE__)
		typedef __unsafe_unretained T* TPtr;
	#else
		typedef T* TPtr;
	#endif

	inline
	ZMACRO_Attribute_NoThrow_ZP
	static void spRetain(TPtr iPtr) { if (iPtr) sRetain(*iPtr); }

	inline
	ZMACRO_Attribute_NoThrow_ZP
	static void spRelease(TPtr iPtr) { if (iPtr) sRelease(*iPtr); }

public:
	#if defined(__OBJC__)
		operator bool() const { return true && fPtr; }
		operator TPtr() const { return fPtr; }
	#else
		ZMACRO_operator_bool_T(ZP, operator_bool) const
			{ return operator_bool_gen::translate(true && fPtr); }
	#endif

	T& operator *()
		{
		sCheck(fPtr);
		return *fPtr;
		}

	T& operator *() const
		{
		sCheck(fPtr);
		return *fPtr;
		}

	template <class O>
	ZMACRO_Attribute_NoThrow_ZP
	inline
	void swap(ZP<O>& ioOther)
		{
		using std::swap;
		swap(fPtr, ioOther.fPtr);
		}

	typedef T Type_t;
	typedef TPtr Ptr_t;

//--

	ZMACRO_Attribute_NoThrow_ZP
	inline
	ZP()
	:	fPtr(0)
		{}

	ZMACRO_Attribute_NoThrow_ZP
	inline
	ZP(const ZP& iOther)
	:	fPtr(iOther.Copy())
		{}

	inline
	~ZP()
		{ spRelease(fPtr); }

	inline
	ZP& operator=(const ZP& iOther)
		{
		using std::swap;
		TPtr otherP = iOther.Copy();
		swap(otherP, fPtr);
		spRelease(otherP);
		return *this;
		}

// --

#if ZCONFIG_CPP >= 2011

	inline
	ZP(ZP&& iOther)
	:	fPtr(iOther.Orphan())
		{}

	inline
	ZP& operator=(ZP&& iOther)
		{
		using std::swap;
		swap(iOther.fPtr, fPtr);
		return *this;
		}

#endif // ZCONFIG_CPP >= 2011

//--

	ZMACRO_Attribute_NoThrow_ZP
	inline
	ZP(const null_t&)
	:	fPtr(0)
		{}

	inline
	ZP(TPtr iPtr)
	:	fPtr(iPtr)
		{ spRetain(fPtr); }

	inline
	ZP& operator=(TPtr iPtr)
		{
		using std::swap;
		swap(iPtr, fPtr);
		spRetain(fPtr);
		spRelease(iPtr);
		return *this;
		}

//--

	template <class O>
	ZMACRO_Attribute_NoThrow_ZP
	inline
	ZP(const ZP<O>& iOther)
	:	fPtr(iOther.Copy())
		{}

	template <class O>
	inline
	ZP& operator=(const ZP<O>& iOther)
		{
		using std::swap;
		TPtr otherP = iOther.Copy();
		swap(otherP, fPtr);
		spRelease(otherP);
		return *this;
		}

// --

#if ZCONFIG_CPP >= 2011

	template <class O>
	inline
	ZP(ZP<O>&& iOther)
	:	fPtr(iOther.Orphan())
		{}

	template <class O>
	inline
	ZP& operator=(ZP<O>&& iOther)
		{
		using std::swap;
		TPtr otherP = iOther.Orphan();
		swap(otherP, fPtr);
		spRelease(otherP);
		return *this;
		}

#endif // ZCONFIG_CPP >= 2011

//--

	template <class O>
	ZMACRO_Attribute_NoThrow_ZP
	inline
	ZP(const Adopt_T<O>& iAdopt)
	:	fPtr(iAdopt.Get())
		{}

	template <class O>
	inline
	ZP& operator=(const Adopt_T<O>& iAdopt)
		{
		using std::swap;
		TPtr otherP = iAdopt.Get();
		swap(otherP, fPtr);
		spRelease(otherP);
		return *this;
		}

//--

	template <class O>
	ZMACRO_Attribute_NoThrow_ZP
	inline
	bool operator==(O* iPtr) const
		{ return fPtr == iPtr; }

	template <class O>
	ZMACRO_Attribute_NoThrow_ZP
	inline
	bool operator!=(O* iPtr) const
		{ return fPtr != iPtr; }

	template <class O>
	ZMACRO_Attribute_NoThrow_ZP
	inline
	bool operator==(const ZP<O>& iOther) const
		{ return fPtr == iOther.Get(); }

	template <class O>
	ZMACRO_Attribute_NoThrow_ZP
	inline
	bool operator!=(const ZP<O>& iOther) const
		{ return fPtr != iOther.Get(); }

	template <class O>
	ZMACRO_Attribute_NoThrow_ZP
	inline
	bool operator<(const ZP<O>& iOther) const
		{ return fPtr < iOther.Get(); }

	ZMACRO_Attribute_NoThrow_ZP
	inline
	TPtr operator->() const
		{
		sCheck(fPtr);
		return fPtr;
		}

	ZMACRO_Attribute_NoThrow_ZP
	inline
	TPtr Get() const
		{ return fPtr; }

	inline
	TPtr Copy() const
		{
		spRetain(fPtr);
		return fPtr;
		}

	ZMACRO_Attribute_NoThrow_ZP
	inline
	TPtr Orphan()
		{
		using std::swap;
		TPtr otherP = 0;
		swap(otherP, fPtr);
		return otherP;
		}

	inline
	void Clear()
		{
		using std::swap;
		TPtr otherP = 0;
		swap(otherP, fPtr);
		spRelease(otherP);
		}

	template <class O>
	ZMACRO_Attribute_NoThrow_ZP
	inline
	O* DynamicCast() const
		{ return dynamic_cast<O*>(fPtr); }

	template <class O>
	ZMACRO_Attribute_NoThrow_ZP
	inline
	O* StaticCast() const
		{ return static_cast<O*>(fPtr); }

	inline
	bool AtomicCAS(TPtr iPrior, TPtr iNew)
		{
		if (not sAtomicPtr_CAS(&fPtr, iPrior, iNew))
			return false;
		spRetain(fPtr);
		spRelease(iPrior);
		return true;
		}

	inline
	bool CAS(TPtr iPrior, TPtr iNew)
		{
		if (fPtr != iPrior)
			return false;
		fPtr = iNew;
		spRetain(fPtr);
		spRelease(iPrior);
		return true;
		}

	inline
	static TPtr sCFAllocatorRetain(TPtr iPtr)
		{
		spRetain(iPtr);
		return iPtr;
		}

	inline
	static void sCFAllocatorRelease(TPtr iPtr)
		{ spRelease(iPtr); }

	inline
	void Retain()
		{ spRetain(fPtr); }

	inline
	void Release()
		{ spRelease(fPtr); }

private:
	TPtr fPtr;
	};

template <class T>
void sRefCopy(void* oDest, T* iPtr)
	{
	*static_cast<T**>(oDest) = iPtr;
	if (iPtr)
		sRetain(*iPtr);
	}

// =================================================================================================
#pragma mark - ZP partially specialized for pointer types

template <class T> void sRetain_T(T*& ioPtr);
template <class T> void sRelease_T(T* iPtr);

template <class T>
class ZP<T*>
	{
public:
	operator bool() const { return !!fPtr; }
	operator T*() const { return fPtr; }

	template <class O>
	void swap(ZP<O>& ioOther)
		{
		using std::swap;
		swap(fPtr, ioOther.fPtr);
		}

	typedef T* Type_t;
	typedef T* Ptr_t;

//--

	ZP()
	:	fPtr(0)
		{}

	ZP(const ZP& iOther)
	:	fPtr(iOther.Copy())
		{}

	~ZP()
		{ spRelease(fPtr); }

	ZP& operator=(const ZP& iOther)
		{
		using std::swap;
		T* otherP = iOther.Copy();
		swap(otherP, fPtr);
		spRelease(otherP);
		return *this;
		}

//--

#if ZCONFIG_CPP >= 2011

	ZP(ZP&& iOther)
	:	fPtr(iOther.Orphan())
		{}

	ZP& operator=(ZP&& iOther)
		{
		using std::swap;
		T* otherP = iOther.Orphan();
		swap(otherP, fPtr);
		spRelease(otherP);
		return *this;
		}

#endif // ZCONFIG_CPP >= 2011

//--

	ZP(const null_t&)
	:	fPtr(0)
		{}

	ZP(T* iPtr)
	:	fPtr(iPtr)
		{ spRetain(fPtr); }

	ZP& operator=(T* iPtr)
		{
		using std::swap;
		swap(iPtr, fPtr);
		spRetain(fPtr);
		spRelease(iPtr);
		return *this;
		}

//--

	template <class O>
	ZP(const ZP<O*>& iOther)
	:	fPtr(iOther.Copy())
		{}

	template <class O>
	ZP& operator=(const ZP<O*>& iOther)
		{
		using std::swap;
		T* otherP = iOther.Copy();
		swap(otherP, fPtr);
		spRelease(otherP);
		return *this;
		}

//--

#if ZCONFIG_CPP >= 2011

	template <class O>
	ZP(ZP<O*>&& iOther)
	:	fPtr(iOther.Orphan())
		{}

	template <class O>
	ZP& operator=(ZP<O*>&& iOther)
		{
		using std::swap;
		T* otherP = iOther.Orphan();
		swap(otherP, fPtr);
		spRelease(otherP);
		return *this;
		}

#endif // ZCONFIG_CPP >= 2011

//--

	template <class O>
	ZP(const Adopt_T<O*>& iAdopt)
	:	fPtr(iAdopt.Get())
		{}

	template <class O>
	ZP& operator=(const Adopt_T<O*>& iAdopt)
		{
		using std::swap;
		T* otherP = iAdopt.Get();
		swap(otherP, fPtr);
		spRelease(otherP);
		return *this;
		}

//--

	template <class O>
	bool operator==(O* iPtr) const
		{ return fPtr == iPtr; }

	template <class O>
	bool operator!=(O* iPtr) const
		{ return fPtr != iPtr; }

	template <class O>
	bool operator==(const ZP<O*>& iOther) const
		{ return fPtr == iOther.Get(); }

	template <class O>
	bool operator!=(const ZP<O*>& iOther) const
		{ return fPtr != iOther.Get(); }

	template <class O>
	bool operator<(const ZP<O*>& iOther) const
		{ return fPtr < iOther.Get(); }

	T* Get() const
		{ return fPtr; }

	T* Copy() const
		{
		T* result = fPtr;
		spRetain(result);
		return result;
		}

	T* Orphan()
		{
		using std::swap;
		T* otherP = 0;
		swap(otherP, fPtr);
		return otherP;
		}

	void Clear()
		{
		using std::swap;
		T* otherP = 0;
		swap(otherP, fPtr);
		spRelease(otherP);
		}

	template <class O>
	O StaticCast() const
		{ return static_cast<O>(fPtr); }

	bool AtomicCAS(T* iPrior, T* iNew)
		{
		if (not sAtomic_CASPtr(&fPtr, iPrior, iNew))
			return false;
		spRetain(fPtr);
		spRelease(iPrior);
		return true;
		}

	bool CAS(T* iPrior, T* iNew)
		{
		if (fPtr != iPrior)
			return false;
		fPtr = iNew;
		spRetain(fPtr);
		spRelease(iPrior);
		return true;
		}

	inline
	static T* sCFAllocatorRetain(T* iPtr)
		{
		spRetain(iPtr);
		return iPtr;
		}

	inline
	static void sCFAllocatorRelease(T* iPtr)
		{ spRelease(iPtr); }

	inline
	void Retain()
		{ spRetain(fPtr); }

	inline
	void Release()
		{ spRelease(fPtr); }

private:
	static void spRetain(T*& ioPtr) { if (ioPtr) sRetain_T(ioPtr); }
	static void spRelease(T* iPtr) { if (iPtr) sRelease_T(iPtr); }
	T* fPtr;
	};

// =================================================================================================
#pragma mark - NotRef and ZP

template <class T> using NotP = Not<ZP<T>>;

// =================================================================================================
#pragma mark - sRef

const struct
	{
	template <class T>
	ZP<T> operator()(T* iPtr) const { return ZP<T>(iPtr); }

	template <class T>
	ZP<T> operator()(const ZP<T>& iP) const { return ZP<T>(iP); }

	template <class T>
	ZP<T> operator()(const NotP<T>& iP) const { return ZP<T>(iP); }

	template <class T>
	ZP<T> operator&(T* iPtr) const { return ZP<T>(iPtr); }

	template <class T>
	ZP<T> operator&(const ZP<T>& iP) const { return ZP<T>(iP); }

	} sZP = {}, sRef = {};

// =================================================================================================
#pragma mark - sClear

template <class T>
inline
void sClear(ZP<T>& ioRef)
	{ ioRef.Clear(); }

template <class T>
inline
ZP<T> sGetClear(ZP<T>& ioRef)
	{
	const ZP<T> result = ioRef;
	sClear(ioRef);
	return result;
	}

// =================================================================================================
#pragma mark - swap

template <class T>
ZMACRO_Attribute_NoThrow
inline
void swap(ZP<T>& a, ZP<T>& b)
	{ a.swap(b); }

} // namespace ZooLib

#endif // __ZooLib_ZP_h__
