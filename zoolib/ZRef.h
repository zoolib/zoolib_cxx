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

#include "zoolib/ZAtomic.h" // For sAtomic_CASPtr
#include "zoolib/ZCompat_algorithm.h" // For std::swap
#include "zoolib/ZTypes.h" // For Adopt_T

#ifndef ZMACRO_Attribute_NoThrow_Ref
	#define ZMACRO_Attribute_NoThrow_Ref ZMACRO_Attribute_NoThrow
#endif

namespace ZooLib {

// =================================================================================================
// MARK: - ZRef

template <class T, bool Sense = true>
class ZRef
	{
	inline
	ZMACRO_Attribute_NoThrow_Ref
	static void spRetain(T* iP) { if (iP) sRetain(*iP); }

	inline
	ZMACRO_Attribute_NoThrow_Ref
	static void spRelease(T* iP) { if (iP) sRelease(*iP); }

public:
	#if defined(__OBJC__)
		operator bool() const { return Sense == !!fP; }
		operator T*() const { return fP; }
	#else
		ZMACRO_operator_bool_T(ZRef, operator_bool) const
			{ return operator_bool_gen::translate(Sense == !!fP); }
	#endif

	friend class ZRef<T,not Sense>;

	template <class O, bool OtherSense>
	ZMACRO_Attribute_NoThrow_Ref
	inline
	void swap(ZRef<O,OtherSense>& ioOther)
		{
		using std::swap;
		swap(fP, ioOther.fP);
		}

	typedef T Type_t;
	typedef T* Ptr_t;

	ZMACRO_Attribute_NoThrow_Ref
	inline
	ZRef()
	:	fP(0)
		{}

	ZMACRO_Attribute_NoThrow_Ref
	inline
	ZRef(const ZRef& iOther)
	:	fP(0)
		{ fP = iOther.Copy(); }

	inline
	~ZRef()
		{ spRelease(fP); }

	inline
	ZRef& operator=(const ZRef& iOther)
		{
		using std::swap;
		T* otherP = iOther.Copy();
		swap(otherP, fP);
		spRelease(otherP);
		return *this;
		}

	ZMACRO_Attribute_NoThrow_Ref
	inline
	ZRef(const null_t&)
	:	fP(0)
		{}

	inline
	ZRef(T* iP)
	:	fP(iP)
		{ spRetain(fP); }

	inline
	ZRef& operator=(T* iP)
		{
		using std::swap;
		swap(iP, fP);
		spRetain(fP);
		spRelease(iP);
		return *this;
		}

	template <class O, bool OtherSense>
	ZMACRO_Attribute_NoThrow_Ref
	inline
	ZRef(const ZRef<O,OtherSense>& iOther)
	:	fP(iOther.Copy())
		{}

	template <class O, bool OtherSense>
	inline
	ZRef& operator=(const ZRef<O,OtherSense>& iOther)
		{
		using std::swap;
		T* otherP = iOther.Copy();
		swap(otherP, fP);
		spRelease(otherP);
		return *this;
		}

	template <class O>
	ZMACRO_Attribute_NoThrow_Ref
	inline
	ZRef(const Adopt_T<O>& iAdopt)
	:	fP(iAdopt.Get())
		{}

	template <class O>
	inline
	ZRef& operator=(const Adopt_T<O>& iAdopt)
		{
		using std::swap;
		T* otherP = iAdopt.Get();
		swap(otherP, fP);
		spRelease(otherP);
		return *this;
		}

	template <class O>
	ZMACRO_Attribute_NoThrow_Ref
	inline
	bool operator==(O* iP) const
		{ return fP == iP; }

	template <class O>
	ZMACRO_Attribute_NoThrow_Ref
	inline
	bool operator!=(O* iP) const
		{ return fP != iP; }

	template <class O, bool OtherSense>
	ZMACRO_Attribute_NoThrow_Ref
	inline
	bool operator==(const ZRef<O,OtherSense>& iOther) const
		{ return fP == iOther.Get(); }

	template <class O, bool OtherSense>
	ZMACRO_Attribute_NoThrow_Ref
	inline
	bool operator!=(const ZRef<O,OtherSense>& iOther) const
		{ return fP != iOther.Get(); }

	template <class O, bool OtherSense>
	ZMACRO_Attribute_NoThrow_Ref
	inline
	bool operator<(const ZRef<O,OtherSense>& iOther) const
		{ return fP < iOther.Get(); }

	ZMACRO_Attribute_NoThrow_Ref
	inline
	T* operator->() const
		{
		sCheck(fP);
		return fP;
		}

	ZMACRO_Attribute_NoThrow_Ref
	inline
	T* Get() const
		{ return fP; }

	inline
	T* Copy() const
		{
		spRetain(fP);
		return fP;
		}

	ZMACRO_Attribute_NoThrow_Ref
	inline
	T* Orphan()
		{
		using std::swap;
		T* otherP = 0;
		swap(otherP, fP);
		return otherP;
		}

	inline
	void Clear()
		{
		using std::swap;
		T* otherP = 0;
		swap(otherP, fP);
		spRelease(otherP);
		}

	inline
	T*& OParam()
		{
		this->Clear();
		return fP;
		}

	template <class O>
	ZMACRO_Attribute_NoThrow_Ref
	inline
	O* DynamicCast() const
		{ return dynamic_cast<O*>(fP); }

	template <class O>
	ZMACRO_Attribute_NoThrow_Ref
	inline
	O* StaticCast() const
		{ return static_cast<O*>(fP); }

	inline
	bool AtomicCAS(T* iPrior, T* iNew)
		{
		if (not sAtomicPtr_CAS(&fP, iPrior, iNew))
			return false;
		spRetain(fP);
		spRelease(iPrior);
		return true;
		}

	inline
	bool CAS(T* iPrior, T* iNew)
		{
		if (fP != iPrior)
			return false;
		fP = iNew;
		spRetain(fP);
		spRelease(iPrior);
		return true;
		}

	inline
	static T* sCFAllocatorRetain(T* iP)
		{
		spRetain(iP);
		return iP;
		}

	inline
	static void sCFAllocatorRelease(T* iP)
		{ spRelease(iP); }

	inline
	void Retain()
		{ spRetain(fP); }

	inline
	void Release()
		{ spRelease(fP); }

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
public:
	operator bool() const { return Sense == !!fP; }
	operator T*() const { return fP; }

	friend class ZRef<T,not Sense>;

	template <class O, bool OtherSense>
	void swap(ZRef<O,OtherSense>& ioOther)
		{
		using std::swap;
		swap(fP, ioOther.fP);
		}

	typedef T* Type_t;
	typedef T* Ptr_t;

	ZRef()
	:	fP(0)
		{}

	ZRef(const ZRef& iOther)
	:	fP(0)
		{ fP = iOther.Copy(); }

	~ZRef()
		{ spRelease(fP); }

	ZRef& operator=(const ZRef& iOther)
		{
		using std::swap;
		T* otherP = iOther.Copy();
		swap(otherP, fP);
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
		using std::swap;
		swap(iP, fP);
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
		using std::swap;
		T* otherP = iOther.Copy();
		swap(otherP, fP);
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
		using std::swap;
		T* otherP = iAdopt.Get();
		swap(otherP, fP);
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
		using std::swap;
		T* otherP = 0;
		swap(otherP, fP);
		return otherP;
		}

	void Clear()
		{
		using std::swap;
		T* otherP = 0;
		swap(otherP, fP);
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
		if (not sAtomic_CASPtr(&fP, iPrior, iNew))
			return false;
		spRetain(fP);
		spRelease(iPrior);
		return true;
		}

	bool CAS(T* iPrior, T* iNew)
		{
		if (fP != iPrior)
			return false;
		fP = iNew;
		spRetain(fP);
		spRelease(iPrior);
		return true;
		}

	inline
	static T* sCFAllocatorRetain(T* iP)
		{
		spRetain(iP);
		return iP;
		}

	inline
	static void sCFAllocatorRelease(T* iP)
		{ spRelease(iP); }

	inline
	void Retain()
		{ spRetain(fP); }

	inline
	void Release()
		{ spRelease(fP); }

private:
	static void spRetain(T*& ioP) { if (ioP) sRetain_T(ioP); }
	static void spRelease(T* iP) { if (iP) sRelease_T(iP); }
	T* fP;
	};

// =================================================================================================
// MARK: - sRef

const struct
	{
	template <class T>
	ZRef<T,true> operator()(T* iP) const { return ZRef<T,true>(iP); }

	template <class T, bool Sense>
	ZRef<T,true> operator()(const ZRef<T,Sense>& iRef) const { return ZRef<T,true>(iRef); }

	template <class T>
	ZRef<T,true> operator&(T* iP) const { return ZRef<T,true>(iP); }

	template <class T, bool Sense>
	ZRef<T,true> operator&(const ZRef<T,Sense>& iRef) const { return ZRef<T,true>(iRef); }
	} sRef = {};

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
// MARK: - sTempPtrRef

const struct
	{
	template <class T>
	ZRef<T*,true> operator&(T* iP) const { return ZRef<T*,true>(Adopt_T<T*>(iP)); }

	template <class T>
	ZRef<T*,true> operator()(T* iP) const { return ZRef<T*,true>(Adopt_T<T*>(iP)); }
	} sTempPtrRef = {};

// =================================================================================================
// MARK: - swap

template <class T, bool Sense>
ZMACRO_Attribute_NoThrow
inline
void swap(ZRef<T,Sense>& a, ZRef<T,Sense>& b)
	{ a.swap(b); }

template <class T, bool Sense>
ZMACRO_Attribute_NoThrow
inline
void swap(ZRef<T,Sense>& a, ZRef<T,not Sense>& b)
	{ a.swap(b); }

} // namespace ZooLib

#endif // __ZRef_h__
