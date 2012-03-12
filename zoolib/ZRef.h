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

#ifndef __ZRef_h__
#define __ZRef_h__ 1
#include "zconfig.h"

#if not defined(__OBJC__)
	#include "zoolib/ZCompat_operator_bool.h"
#endif

#include "zoolib/ZAtomic.h" // For ZAtomic_CompareAndSwapPtr
#include "zoolib/ZCompat_algorithm.h" // For std::swap
#include "zoolib/ZTypes.h" // For Adopt_T

namespace ZooLib {

// =================================================================================================
// MARK: - ZRef

template <class T, bool Sense = true>
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

public:
	#if defined(__OBJC__)
		operator bool() const { return Sense == !!fP; }
		operator T*() const { return fP; }
	#else
		ZMACRO_operator_bool_T(ZRef, operator_bool) const
			{ return operator_bool_gen::translate(Sense == !!fP); }
	#endif

	template <class O, bool OtherSense>
	void swap(ZRef<O,OtherSense>& ioOther)
		{ std::swap(fP, ioOther.fP); }

	typedef T Type_t;
	typedef T* Ptr_t;

	ZRef()
	:	fP(0)
		{}

	ZRef(const ZRef& iOther)
	:	fP(iOther.Copy())
		{}

	~ZRef()
		{ spRelease(fP); }

	ZRef& operator=(const ZRef& iOther)
		{
		T* otherP = iOther.Copy();
		std::swap(otherP, fP);
		spRelease(otherP);
		return *this;
		}

	ZRef(const null_t&)
	:	fP(0)
		{}

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

	template <class O, bool OtherSense>
	ZRef(const ZRef<O,OtherSense>& iOther)
	:	fP(iOther.Copy())
		{}

	template <class O, bool OtherSense>
	ZRef& operator=(const ZRef<O,OtherSense>& iOther)
		{
		T* otherP = iOther.Copy();
		std::swap(otherP, fP);
		spRelease(otherP);
		return *this;
		}

	template <class O>
	ZRef(const Adopt_T<O>& iAdopt)
	:	fP(iAdopt.Get())
		{}

	template <class O>
	ZRef& operator=(const Adopt_T<O>& iAdopt)
		{
		T* otherP = iAdopt.Get();
		std::swap(otherP, fP);
		spRelease(otherP);
		return *this;
		}

	template <class O>
	bool operator==(O* iP) const
		{ return fP == iP; }

	template <class O>
	bool operator!=(O* iP) const
		{ return fP != iP; }

	template <class O, bool OtherSense>
	bool operator==(const ZRef<O,OtherSense>& iOther) const
		{ return fP == iOther.Get(); }

	template <class O, bool OtherSense>
	bool operator!=(const ZRef<O,OtherSense>& iOther) const
		{ return fP != iOther.Get(); }

	template <class O, bool OtherSense>
	bool operator<(const ZRef<O,OtherSense>& iOther) const
		{ return fP < iOther.Get(); }

	T* operator->() const
		{
		sCheck(fP);
		return fP;
		}

	T* Get() const
		{ return fP; }

	T* Copy() const
		{
		spRetain(fP);
		return fP;
		}

	T* Orphan()
		{
		T* otherP = 0;
		std::swap(otherP, fP);
		return otherP;
		}

	void Clear()
		{
		T* otherP = 0;
		std::swap(otherP, fP);
		spRelease(otherP);
		}

	T*& OParam()
		{
		this->Clear();
		return fP;
		}

	template <class O>
	O* DynamicCast() const
		{ return dynamic_cast<O*>(fP); }

	template <class O>
	O* StaticCast() const
		{ return static_cast<O*>(fP); }

	bool AtomicCAS(T* iPrior, T* iNew)
		{
		if (not ZAtomic_CompareAndSwapPtr(&fP, iPrior, iNew))
			return false;
		spRetain(iNew);
		spRelease(iPrior);
		return true;
		}

	bool CAS(T* iPrior, T* iNew)
		{
		if (fP != iPrior)
			return false;
		fP = iNew;
		spRetain(iNew);
		spRelease(iPrior);
		return true;
		}

	static T* sCFAllocatorRetain(T* iP)
		{
		spRetain(iP);
		return iP;
		}

	static void sCFAllocatorRelease(T* iP)
		{ spRelease(iP); }

private:
	T* fP;
	};

template <class T>
void sRefCopy(void* oDest, T* iP)
	{
	*static_cast<T**>(oDest) = iP;
	if (iP)
		sRetain(*iP);
	}

// =================================================================================================
// MARK: - ZRef partially specialized for pointer types

template <class T> void sRetain_T(T*& ioPtr);
template <class T> void sRelease_T(T* iPtr);

template <class T, bool Sense>
class ZRef<T*,Sense>
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
	operator bool() const { return Sense == !!fP; }
	operator T*() const { return fP; }

	template <class O, bool OtherSense>
	void swap(ZRef<O,OtherSense>& ioOther)
		{ std::swap(fP, ioOther.fP); }

	typedef T* Type_t;
	typedef T* Ptr_t;

	ZRef()
	:	fP(0)
		{}

	ZRef(const ZRef& iOther)
	:	fP(iOther.Copy())
		{}

	~ZRef()
		{ spRelease(fP); }

	ZRef& operator=(const ZRef& iOther)
		{
		T* otherP = iOther.Copy();
		std::swap(otherP, fP);
		spRelease(otherP);
		return *this;
		}

	ZRef(const null_t&)
	:	fP(0)
		{}

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

	template <class O, bool OtherSense>
	ZRef(const ZRef<O*,OtherSense>& iOther)
	:	fP(iOther.Copy())
		{}

	template <class O, bool OtherSense>
	ZRef& operator=(const ZRef<O*,OtherSense>& iOther)
		{
		T* otherP = iOther.Copy();
		std::swap(otherP, fP);
		spRelease(otherP);
		return *this;
		}

	template <class O>
	ZRef(const Adopt_T<O*>& iAdopt)
	:	fP(iAdopt.Get())
		{}

	template <class O>
	ZRef& operator=(const Adopt_T<O*>& iAdopt)
		{
		T* otherP = iAdopt.Get();
		std::swap(otherP, fP);
		spRelease(otherP);
		return *this;
		}

	template <class O>
	bool operator==(O* iP) const
		{ return fP == iP; }

	template <class O>
	bool operator!=(O* iP) const
		{ return fP != iP; }

	template <class O, bool OtherSense>
	bool operator==(const ZRef<O*,OtherSense>& iOther) const
		{ return fP == iOther.Get(); }

	template <class O, bool OtherSense>
	bool operator!=(const ZRef<O*,OtherSense>& iOther) const
		{ return fP != iOther.Get(); }

	template <class O, bool OtherSense>
	bool operator<(const ZRef<O*,OtherSense>& iOther) const
		{ return fP < iOther.Get(); }

	T* Get() const
		{ return fP; }

	T* Copy() const
		{
		T* result = fP;
		spRetain(result);
		return result;
		}

	T* Orphan()
		{
		T* otherP = 0;
		std::swap(otherP, fP);
		return otherP;
		}

	void Clear()
		{
		T* otherP = 0;
		std::swap(otherP, fP);
		spRelease(otherP);
		}

	T*& OParam()
		{
		this->Clear();
		return fP;
		}

	template <class O>
	O StaticCast() const
		{ return static_cast<O>(fP); }

	bool AtomicCAS(T* iPrior, T* iNew)
		{
		if (not ZAtomic_CompareAndSwapPtr(&fP, iPrior, iNew))
			return false;
		spRetain(iNew);
		spRelease(iPrior);
		return true;
		}

	bool CAS(T* iPrior, T* iNew)
		{
		if (fP != iPrior)
			return false;
		fP = iNew;
		spRetain(iNew);
		spRelease(iPrior);
		return true;
		}

	static T* sCFAllocatorRetain(T* iP)
		{
		spRetain(iP);
		return iP;
		}

	static void sCFAllocatorRelease(T* iP)
		{ spRelease(iP); }

private:
	T* fP;
	};

// =================================================================================================
// MARK: - sRef

template <class T>
ZRef<T,true> sRef(T* iP)
	{ return ZRef<T,true>(iP); }

template <class T, bool Sense>
ZRef<T,true> sRef(const ZRef<T,Sense>& iP)
	{ return iP; }

// =================================================================================================
// MARK: - sTempRef

const struct
	{
	template <class T>
	ZRef<T,true> operator&(T* iP) const { return ZRef<T,true>(Adopt_T<T>(iP)); }

	template <class T>
	ZRef<T,true> operator()(T* iP) const { return ZRef<T,true>(Adopt_T<T>(iP)); }
	} sTempRef = {};

// =================================================================================================
// MARK: -

template <class T>
void swap(ZRef<T>& a, ZRef<T>& b)
	{ a.swap(b); }

template <class T, bool SenseA, bool SenseB>
void swap(ZRef<T,SenseA>& a, ZRef<T,SenseB>& b)
	{ a.swap(b); }

} // namespace ZooLib

#endif // __ZRef_h__
