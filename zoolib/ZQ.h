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

#include "zoolib/ZCompat_operator_bool.h"
#include "zoolib/ZCompat_algorithm.h" // For std::swap
#include "zoolib/ZCtorDtor.h" // For placement ctor/copy/dtor/assign
#include "zoolib/ZDebug.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * ZQ

// The 'Q' stands for 'Questionable', 'Queryable', 'Quibble' or perhaps 'Quib'

// c.f. Haskell's 'Data.Maybe', Scala's 'Option'.

template <class T, bool Sense = true>
class ZQ
	{
public:
	void swap(ZQ& iOther)
		{
		if (fHasValue)
			{
			if (iOther.fHasValue)
				{
				std::swap(*sFetch_T<T>(fBytes), *sFetch_T<T>(iOther.fBytes));
				}
			else
				{
				sCtorFromVoidStar_T<T>(iOther.fBytes, fBytes);
				iOther.fHasValue = true;
				fHasValue = false;
				sDtor_T<T>(fBytes);
				}
			}
		else if (iOther.fHasValue)
			{
			sCtorFromVoidStar_T<T>(fBytes, iOther.fBytes);
			fHasValue = true;
			iOther.fHasValue = false;
			sDtor_T<T>(iOther.fBytes);
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
	ZQ(const ZQ<OtherT, OtherSense>& iOther)
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
	ZQ& operator=(const ZQ<OtherT, OtherSense>& iOther)
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
		{ sCtor_T<T, P0, P1>(fBytes, i0, i1); }

// -----------------

	ZMACRO_operator_bool_T(ZQ, operator_bool) const
		{ return operator_bool_gen::translate(fHasValue == Sense); }

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

	void Clear()
		{
		if (fHasValue)
			{
			fHasValue = false;
			sDtor_T<T>(fBytes);
			}
		}

	T* PGetMutable()
		{
		if (fHasValue)
			return sFetch_T<T>(fBytes);
		return nullptr;
		}

	const T* PGet() const
		{
		if (fHasValue)
			return sFetch_T<T>(fBytes);
		return nullptr;
		}

	T DGet(const T& iDefault) const
		{
		if (fHasValue)
			return *sFetch_T<T>(fBytes);
		return iDefault;
		}

	T& GetMutable()
		{
		ZAssert(fHasValue);
		return *sFetch_T<T>(fBytes);
		}

	const T& Get() const
		{
		ZAssert(fHasValue);
		return *sFetch_T<T>(fBytes);
		}

	void Set(const T& iVal)
		{
		if (fHasValue)
			{
			sAssign_T<T>(fBytes, iVal);
			}
		else
			{
			sCtor_T<T>(fBytes, iVal);
			fHasValue = true;
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
	#if ZCONFIG(Compiler,GCC)
		char fBytes[sizeof(T)] __attribute__((aligned));
	#else
		char fBytes[sizeof(T)];
	#endif

	bool fHasValue;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZQ (specialized for void)

template <bool Sense>
class ZQ<void,Sense>
	{
public:
	void swap(ZQ& iOther)
		{ swap(fHasValue, iOther.fHasValue); }

// -----------------

	ZQ(void)
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

	void Clear()
		{ fHasValue = false; }

	void Get() const
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

// =================================================================================================
#pragma mark -
#pragma mark * swap

template <class T>
inline void swap(ZQ<T>& a, ZQ<T>& b)
	{ a.swap(b); }

} // namespace ZooLib

#endif // __ZQ_h__
