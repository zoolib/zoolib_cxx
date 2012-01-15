/* -------------------------------------------------------------------------------------------------
Copyright (c) 2008 Andrew Green
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

#ifndef __ZMulti_T_h__
#define __ZMulti_T_h__ 1
#include "zconfig.h"

namespace ZooLib {

// =================================================================================================
// MARK: - ZMulti_T1

template <class T0>
class ZMulti_T1
	{
public:
	ZMulti_T1() {};

	ZMulti_T1(const T0& i0)
	:	f0(i0)
		{}

	T0 f0;
	};

template <class T0>
ZMulti_T1<T0> sMulti(const T0& i0)
	{ return ZMulti_T1<T0>(i0); }

// =================================================================================================
// MARK: - ZMulti_T2

template <class T0, class T1>
class ZMulti_T2
	{
public:
	ZMulti_T2() {};

	ZMulti_T2(const T0& i0, const T1& i1)
	:	f0(i0)
	,	f1(i1)
		{}

	T0 f0;
	T1 f1;
	};

template <class T0, class T1>
ZMulti_T2<T0,T1> sMulti(const T0& i0, const T1& i1)
	{ return ZMulti_T2<T0,T1>(i0, i1); }

// =================================================================================================
// MARK: - ZMulti_T3

template <class T0, class T1, class T2>
class ZMulti_T3
	{
public:
	ZMulti_T3() {};

	ZMulti_T3(const T0& i0, const T1& i1, const T2& i2)
	:	f0(i0)
	,	f1(i1)
	,	f2(i2)
		{}

	T0 f0;
	T1 f1;
	T2 f2;
	};

template <class T0, class T1, class T2>
ZMulti_T3<T0,T1,T2> sMulti(const T0& i0, const T1& i1, const T2& i2)
	{ return ZMulti_T3<T0,T1,T2>(i0, i1, i2); }

// =================================================================================================
// MARK: - ZMulti_T4

template <class T0, class T1, class T2, class T3>
class ZMulti_T4
	{
public:
	ZMulti_T4() {};

	ZMulti_T4(const T0& i0, const T1& i1, const T2& i2, const T3& i3)
	:	f0(i0)
	,	f1(i1)
	,	f2(i2)
	,	f3(i3)
		{}

	T0 f0;
	T1 f1;
	T2 f2;
	T3 f3;
	};

template <class T0, class T1, class T2, class T3>
ZMulti_T3<T0,T1,T2> sMulti(const T0& i0, const T1& i1, const T2& i2, const T3& i3)
	{ return ZMulti_T4<T0,T1,T2,T3>(i0, i1, i2, i3); }

} // namespace ZooLib

#endif // __ZMulti_T_h__
