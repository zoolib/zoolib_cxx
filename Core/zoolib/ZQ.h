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

#ifndef __ZQ_h__
#define __ZQ_h__
#include "zconfig.h"

#include "zoolib/Compat_operator_bool.h"
#include "zoolib/Compat_algorithm.h" // For std::swap
#include "zoolib/CtorDtor.h" // For placement ctor/copy/dtor/assign
#include "zoolib/Singleton.h" // For sDefault
#include "zoolib/Util_Relops.h"

#include "zoolib/ZDebug.h"
#include "zoolib/ZTypes.h" // For null

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark ZQ

// The 'Q' stands for 'Questionable' or 'Queryable'.
// It is pronounced 'Quid', as in the latin interrogative pronoun.

// c.f. boost::optional, Haskell's 'Data.Maybe', Scala's 'Option'.

template <class T, bool Sense = true>
class ZQ
	{
	friend class ZQ<T,not Sense>;

	template <bool SenseNoValue, bool SenseHasValue>
	static
	void spSwap(ZQ<T,SenseNoValue>& ioNoValue, ZQ<T,SenseHasValue>& ioHasValue)
		{
		sCtorFromVoidStar_T<T>(ioNoValue.fBytes, ioHasValue.fBytes);
		ioNoValue.fHasValue = true;
		ioHasValue.fHasValue = false;
		sDtor_T<T>(ioHasValue.fBytes);
		}

public:
	template <bool OtherSense>
	ZMACRO_Attribute_NoThrow
	void swap(ZQ<T,OtherSense>& ioOther)
		{
		if (fHasValue)
			{
			if (ioOther.fHasValue)
				{
				using std::swap;
				swap(*sFetch_T<T>(fBytes), *sFetch_T<T>(ioOther.fBytes));
				}
			else
				{
				spSwap(ioOther, *this);
				}
			}
		else if (ioOther.fHasValue)
			{
			spSwap(*this, ioOther);
			}
		}

// -----------------

	ZQ()
	:	fHasValue(false)
		{}

	ZQ(const ZQ& iOther)
	:	fHasValue(iOther.fHasValue)
		{
		if (fHasValue)
			sCtorFromVoidStar_T<T>(fBytes, iOther.fBytes);
		}

	~ZQ()
		{
		if (fHasValue)
			sDtor_T<T>(fBytes);
		}

	ZQ& operator=(const ZQ& iOther)
		{
		if (fHasValue)
			{
			if (iOther.fHasValue)
				{
				sAssignFromVoidStar_T<T>(fBytes, iOther.fBytes);
				}
			else
				{
				fHasValue = false;
				sDtor_T<T>(fBytes);
				}
			}
		else if (iOther.fHasValue)
			{
			sCtorFromVoidStar_T<T>(fBytes, iOther.fBytes);
			fHasValue = true;
			}
		return *this;
		}

// -----------------

	template <class OtherT, bool OtherSense>
	ZQ(const ZQ<OtherT,OtherSense>& iOther)
		{
		if (const OtherT* theOther = iOther.PGet())
			{
			sCtor_T<T,OtherT>(fBytes, *theOther);
			fHasValue = true;
			}
		else
			{
			fHasValue = false;
			}
		}

	template <class OtherT, bool OtherSense>
	ZQ& operator=(const ZQ<OtherT,OtherSense>& iOther)
		{
		if (fHasValue)
			{
			if (const OtherT* theOther = iOther.PGet())
				{
				sAssign_T<T>(fBytes, *theOther);
				}
			else
				{
				fHasValue = false;
				sDtor_T<T>(fBytes);
				}			
			}
		else if (const OtherT* theOther = iOther.PGet())
			{
			sCtor_T<T,OtherT>(fBytes, *theOther);
			fHasValue = true;
			}
		return *this;
		}

// -----------------

	ZQ(const null_t&)
	:	fHasValue(false)
		{}

	ZQ& operator=(const null_t&)
		{
		if (fHasValue)
			{
			fHasValue = false;
			sDtor_T<T>(fBytes);
			}
		return *this;
		}

// -----------------

	template <class P0>
	ZQ(const P0& i0)
	:	fHasValue(true)
		{ sCtor_T<T, P0>(fBytes, i0); }

	template <class P0>
	ZQ& operator=(const P0& iValue)
		{
		if (fHasValue)
			{
			sAssign_T<T>(fBytes, iValue);
			}
		else
			{
			sCtor_T<T,P0>(fBytes, iValue);
			fHasValue = true;
			}
		return *this;
		}

// -----------------

	template <class P0, class P1>
	ZQ(const P0& i0, const P1& i1)
	:	fHasValue(true)
		{ sCtor_T<T,P0,P1>(fBytes, i0, i1); }

// -----------------

	ZMACRO_operator_bool_T(ZQ, operator_bool) const
		{ return operator_bool_gen::translate(fHasValue == Sense); }

	bool HasValue() const
		{ return fHasValue; }

	T& operator*()
		{
		ZAssert(fHasValue);
		return *sFetch_T<T>(fBytes);
		}
	
	const T& operator*() const
		{
		ZAssert(fHasValue);
		return *sFetch_T<T>(fBytes);
		}
	
	T* operator->()
		{
		ZAssert(fHasValue);
		return sFetch_T<T>(fBytes);
		}

	const T* operator->() const
		{
		ZAssert(fHasValue);
		return sFetch_T<T>(fBytes);
		}

// -----------------

	void Clear()
		{
		if (fHasValue)
			{
			fHasValue = false;
			sDtor_T<T>(fBytes);
			}
		}

	const T* PGet() const
		{
		if (fHasValue)
			return sFetch_T<T>(fBytes);
		return nullptr;
		}

	const T& DGet(const T& iDefault) const
		{
		if (fHasValue)
			return *sFetch_T<T>(fBytes);
		return iDefault;
		}

	const T& Get() const
		{
		if (fHasValue)
			return *sFetch_T<T>(fBytes);
		return sDefault<T>();
		}

	T* PMut()
		{
		if (fHasValue)
			return sFetch_T<T>(fBytes);
		return nullptr;
		}

	T& DMut(const T& iDefault)
		{
		if (not fHasValue)
			{
			sCtor_T<T>(fBytes, iDefault);
			fHasValue = true;
			}
		return *sFetch_T<T>(fBytes);
		}

	T& Mut()
		{
		if (not fHasValue)
			{
			sCtor_T<T>(fBytes);
			fHasValue = true;
			}
		return *sFetch_T<T>(fBytes);
		}

	T& Set(const T& iVal)
		{
		if (fHasValue)
			{
			return sAssign_T<T>(fBytes, iVal);
			}
		else
			{
			sCtor_T<T>(fBytes, iVal);
			fHasValue = true;
			return *sFetch_T<T>(fBytes);
			}
		}

	bool QSet(const T& iVal)
		{
		if (fHasValue)
			return false;
		sCtor_T<T>(fBytes, iVal);
		fHasValue = true;
		return true;
		}

	T& OParam()
		{
		if (fHasValue)
			{
			fHasValue = false;
			sDtor_T<T>(fBytes);
			}

		sCtor_T<T>(fBytes);
		fHasValue = true;
		return *sFetch_T<T>(fBytes);
		}

private:
	#if ZCONFIG_CPP >= 2011
		alignas(T) char fBytes[sizeof(T)];
	#else
		char fBytes[sizeof(T)] ZMACRO_Attribute_Aligned;
	#endif

	bool fHasValue;
	};

template <class T, bool SenseL, bool SenseR>
bool operator==(const ZQ<T,SenseL>& iL, const ZQ<T,SenseR>& iR)
	{
	if (const T* ll = iL.PGet())
		{
		if (const T* rr = iR.PGet())
			return *ll == *rr;
		return false;
		}
	return not iR.HasValue();
	}

template <class T, bool SenseL, bool SenseR>
bool operator<(const ZQ<T,SenseL>& iL, const ZQ<T,SenseR>& iR)
	{
	if (const T* ll = iL.PGet())
		{
		if (const T* rr = iR.PGet())
			return *ll < *rr;
		return false;
		}
	return iR.HasValue();
	}

template <class T, bool Sense>
struct RelopsTraits_HasEQ<ZQ<T,Sense> > : public RelopsTraits_Has {};

template <class T, bool Sense>
struct RelopsTraits_HasLT<ZQ<T,Sense> > : public RelopsTraits_Has {};

// =================================================================================================
#pragma mark -
#pragma mark ZQ (specialized for void)

template <bool Sense>
class ZQ<void,Sense>
	{
public:
	void swap(ZQ& iOther)
		{ swap(fHasValue, iOther.fHasValue); }

// -----------------

	ZQ()
	:	fHasValue(false)
		{}

	ZQ(const ZQ& iOther)
	:	fHasValue(iOther.fHasValue)
		{}

	~ZQ()
		{}

	ZQ& operator=(const ZQ& iOther)
		{
		fHasValue = iOther.fHasValue;
		return *this;
		}

// -----------------

	ZQ(const ZQ<void,!Sense>& iOther)
	:	fHasValue(iOther.fHasValue)
		{}

	ZQ& operator=(const ZQ<void,!Sense>& iOther)
		{
		fHasValue = iOther.fHasValue;
		return *this;
		}

// -----------------

	ZQ(const null_t&)
	:	fHasValue(false)
		{}

	ZQ& operator=(const null_t&)
		{
		fHasValue = false;
		return *this;
		}

// -----------------

	ZQ(const notnull_t&)
	:	fHasValue(true)
		{}

	ZQ& operator=(const notnull_t&)
		{
		fHasValue = true;
		return *this;
		}

// -----------------

	ZMACRO_operator_bool_T(ZQ, operator_bool) const
		{ return operator_bool_gen::translate(fHasValue == Sense); }

	void operator*() const
		{ ZAssert(fHasValue); }

	void Clear()
		{ fHasValue = false; }

	const void Get() const
		{}

	void Set()
		{ fHasValue = true; }

	bool QSet()
		{
		if (fHasValue)
			return false;
		fHasValue = true;
		return true;
		}

private:
	bool fHasValue;

	friend class ZQ<void, !Sense>;
	};

template <bool SenseL, bool SenseR>
bool operator==(const ZQ<void,SenseL>& iL, const ZQ<void,SenseR>& iR)
	{
	if (iL.HasValue())
		return iR.HasValue();
	return not iR.HasValue();
	}

template <bool SenseL, bool SenseR>
bool operator<(const ZQ<void,SenseL>& iL, const ZQ<void,SenseR>& iR)
	{ return not iL.HasValue() && iR.HasValue(); }

// =================================================================================================
#pragma mark -
#pragma mark Pseudo-ctor

template <class T>
ZQ<T> sQ()
	{ return ZQ<T>(); }

template <class T>
ZQ<T> sQ(const T& iT)
	{ return ZQ<T>(iT); }

template <class T, class P0>
ZQ<T> sQ(const P0& i0)
	{ return ZQ<T>(i0); }

template <class T, class P0, class P1>
ZQ<T> sQ(const P0& i0, const P1& i1)
	{ return ZQ<T>(i0, i1); }

// =================================================================================================
#pragma mark -
#pragma mark Accessor functions

template <class T, bool Sense>
const T* sPGet(const ZQ<T,Sense>& iQ)
	{ return iQ.PGet(); }

template <class T, bool Sense>
const T& sDGet(const T& iDefault, const ZQ<T,Sense>& iQ)
	{ return iQ.DGet(iDefault); }

template <class T, bool Sense>
const T& sGet(const ZQ<T,Sense>& iQ)
	{ return iQ.Get(); }

template <class T, bool Sense>
T* sPMut(ZQ<T,Sense>& ioQ)
	{ return ioQ.PMut(); }

template <class T, bool Sense>
T& sDMut(const T& iDefault, ZQ<T,Sense>& ioQ)
	{ return ioQ.DMut(iDefault); }

template <class T, bool Sense>
T& sMut(ZQ<T,Sense>& ioQ)
	{ return ioQ.Mut(); }

template <class T, bool Sense>
T& sSet(ZQ<T,Sense>& ioQ, const T& iVal)
	{ return ioQ.Set(iVal); }

// =================================================================================================
#pragma mark -
#pragma mark 

template <class T, bool Sense>
void sClear(ZQ<T,Sense>& ioQ)
	{ ioQ.Clear(); }

template <class T, bool Sense>
T sGetClear(ZQ<T,Sense>& ioQ)
	{
	const T result = ioQ.Get();
	ioQ.Clear();
	return result;
	}

template <class T, bool Sense>
ZQ<T> sQGetClear(ZQ<T,Sense>& ioQ)
	{
	if (ioQ)
		{
		const T result = ioQ.Get();
		ioQ.Clear();
		return result;
		}
	return null;
	}

// =================================================================================================
#pragma mark -
#pragma mark swap

template <class T, bool Sense>
void swap(ZQ<T,Sense>& a, ZQ<T,Sense>& b)
	{ a.swap(b); }

template <class T, bool Sense>
void swap(ZQ<T,Sense>& a, ZQ<T,not Sense>& b)
	{ a.swap(b); }

} // namespace ZooLib

#endif // __ZQ_h__
