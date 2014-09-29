/* -------------------------------------------------------------------------------------------------
Copyright (c) 2014 Andrew Green
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

#ifndef __ZooLib_Multi_h__
#define __ZooLib_Multi_h__ 1

#include "zoolib/Callable_Macros.h"
#include "zoolib/Util_Relops.h"

namespace ZooLib {

// =================================================================================================
// MARK: - Multi

template <class P0_p>
class Multi1
	{
public:
	Multi1() {};

	Multi1(const Multi1& iOther)
	:	f0(iOther.f0)
		{}

	Multi1& operator=(const Multi1& iOther)
		{
		f0 = iOther.f0;
		return *this;
		}

	Multi1(typename CallableUtil::VT<P0_p>::P i0)
	:	f0(i0)
		{};

	bool operator<(const Multi1& iOther) const
		{
		if (f0 < iOther.f0)
			return true;
		return false;
		}

	int Compare(const Multi1& iOther) const
		{
		if (f0 < iOther.f0)
			return -1;
		else if (iOther.f0 < f0)
			return 1;
		return 0;
		}

	typename CallableUtil::VT<P0_p>::Mutable f0;
	};

template <class P0_p>
class RelopsTraits_HasLT<Multi1<P0_p> > : public RelopsTraits_Has {};

template <class P0_p>
Multi1<P0_p> sMulti(typename CallableUtil::VT<P0_p>::P i0)
	{ return Multi1<P0_p>(i0); }

// =================================================================================================
// MARK: - Multi

#define ZMACRO_Multi(X_2,X_1,X) \
template <ZMACRO_Callable_Class_P##X_1> \
class Multi##X : public Multi##X_1<ZMACRO_Callable_P##X_2> \
	{ \
	typedef Multi##X_1<ZMACRO_Callable_P##X_2> inherited; \
public: \
	Multi##X () {}; \
\
	Multi##X(const Multi##X& iOther) \
	:	inherited(iOther) \
	,	f##X_1(iOther.f##X_1) \
		{} \
\
	Multi##X(ZMACRO_Callable_VT##X_1) \
	:	inherited(ZMACRO_Callable_i##X_2) \
	,	f##X_1(i##X_1) \
		{} \
\
	Multi##X& operator=(const Multi##X& iOther) \
		{ \
		inherited::operator=(iOther); \
		f##X_1 = iOther.f##X_1; \
		return *this; \
		} \
\
	bool operator<(const Multi##X& iOther) const \
		{ \
		if (inherited::operator<(iOther)) \
			return true; \
		return false; \
		} \
\
	int Compare(const Multi##X& iOther) const \
		{ \
		if (const int result = inherited::Compare(iOther)) \
			return result; \
		else if (f##X_1 < iOther.f##X_1) \
			return -1; \
		else  if (iOther.f##X_1 < f##X_1) \
			return 1; \
		else \
			return 0; \
		} \
\
	typename CallableUtil::VT<P##X_1##_p>::Mutable f##X_1; \
	}; \
\
template <ZMACRO_Callable_Class_P##X_1> \
class RelopsTraits_HasLT<Multi##X<ZMACRO_Callable_P##X_1> > : public RelopsTraits_Has {}; \
\
template <ZMACRO_Callable_Class_P##X> \
Multi##X<ZMACRO_Callable_P##X_1> sMulti(ZMACRO_Callable_VT##X_1) \
	{ return Multi##X<ZMACRO_Callable_P##X_1>(ZMACRO_Callable_i##X_1); } \

// =================================================================================================
// MARK: - Multi

ZMACRO_Multi(0,1,2)
ZMACRO_Multi(1,2,3)
ZMACRO_Multi(2,3,4)
ZMACRO_Multi(3,4,5)
ZMACRO_Multi(4,5,6)
ZMACRO_Multi(5,6,7)
ZMACRO_Multi(6,7,8)
ZMACRO_Multi(7,8,9)
ZMACRO_Multi(8,9,A)
ZMACRO_Multi(9,A,B)
ZMACRO_Multi(A,B,C)
ZMACRO_Multi(B,C,D)
ZMACRO_Multi(C,D,E)
ZMACRO_Multi(D,E,F)

#undef ZMACRO_Multi

} // namespace ZooLib

#endif // __ZooLib_Multi_h__
