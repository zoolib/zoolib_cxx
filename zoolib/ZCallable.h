/* -------------------------------------------------------------------------------------------------
Copyright (c) 2010 Andrew Green
http://www.zoolib.org

Permission is hereby granted, free of charge, to any person obtaining a copy of this software
and associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge,Publish, distribute,
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

#ifndef __ZCallable_h__
#define __ZCallable_h__ 1
#include "zconfig.h"

#include "zoolib/ZCounted.h"
#include "zoolib/ZQ.h"

namespace ZooLib {

// =================================================================================================
// MARK: - ZCallableUtil

namespace ZCallableUtil {

template <class T> struct VT
	{
	typedef const T Field;
	typedef const T& Param;

	typedef Field F;
	typedef Param P;
	};

template <class T> struct VT<const T&>
	{
	typedef const T Field;
	typedef const T& Param;

	typedef Field F;
	typedef Param P;
	};

template <class T> struct VT<T&>
	{
	typedef T Field;
	typedef T& Param;

	typedef Field F;
	typedef Param P;
	};

} // namespace ZCallableUtil

// =================================================================================================
// MARK: - ZCallable

template <class Signature> class ZCallable;

// =================================================================================================
// MARK: - ZCallable (specialization for 0 params)

template <class R>
class ZCallable<R(void)>
:	public ZCounted
	{
public:
	typedef R(Signature)(void);

	virtual ZQ<R> QCall() = 0;

	inline
	R DCall(const R& iDefault)
		{
		if (ZQ<R> theQ = this->QCall())
			return *theQ;
		return iDefault;
		}
	
	inline
	R Call()
		{
		if (ZQ<R> theQ = this->QCall())
			return *theQ;
		return R();
		}
	};

// =================================================================================================
// MARK: - ZCallable (specialization for 0 params, void return)

template <>
class ZCallable<void(void)>
:	public ZCounted
	{
public:
	typedef void(Signature)(void);

	virtual ZQ<void> QCall() = 0;

	inline
	void Call()
		{
		this->QCall();
		}
	};

// =================================================================================================
// MARK: - ZCallable (specialization for 1 param)

template <class R,
	class P0>
class ZCallable<R(P0)>
:	public ZCounted
	{
public:
	typedef R(Signature)(P0);

	virtual ZQ<R> QCall(P0) = 0;

	inline
	R DCall(const R& iDefault,
		typename ZCallableUtil::VT<P0>::P i0)
		{
		if (ZQ<R> theQ = this->QCall(i0))
			return *theQ;
		return iDefault;
		}
	
	inline
	R Call(typename ZCallableUtil::VT<P0>::P i0)
		{
		if (ZQ<R> theQ = this->QCall(i0))
			return *theQ;
		return R();
		}
	};

// =================================================================================================
// MARK: - ZCallable (specialization for 1 param, void return)

template <
	class P0>
class ZCallable<void(P0)>
:	public ZCounted
	{
public:
	typedef void(Signature)(P0);

	virtual ZQ<void> QCall(P0) = 0;

	inline
	void Call(typename ZCallableUtil::VT<P0>::P i0)
		{
		this->QCall(i0);
		}
	};

// =================================================================================================
// MARK: - ZCallable (specialization for 2 params)

template <class R,
	class P0, class P1>
class ZCallable<R(P0,P1)>
:	public ZCounted
	{
public:
	typedef R(Signature)(P0,P1);

	virtual ZQ<R> QCall(P0,P1) = 0;

	inline
	R DCall(const R& iDefault,
		typename ZCallableUtil::VT<P0>::P i0,
		typename ZCallableUtil::VT<P1>::P i1)
		{
		if (ZQ<R> theQ = this->QCall(i0, i1))
			return *theQ;
		return iDefault;
		}
	
	inline
	R Call
		(typename ZCallableUtil::VT<P0>::P i0,
		typename ZCallableUtil::VT<P1>::P i1)
		{
		if (ZQ<R> theQ = this->QCall(i0, i1))
			return *theQ;
		return R();
		}
	};

// =================================================================================================
// MARK: - ZCallable (specialization for 2 params, void return)

template <
	class P0, class P1>
class ZCallable<void(P0,P1)>
:	public ZCounted
	{
public:
	typedef void(Signature)(P0,P1);

	virtual ZQ<void> QCall(P0,P1) = 0;

	inline
	void Call
		(typename ZCallableUtil::VT<P0>::P i0,
		typename ZCallableUtil::VT<P1>::P i1)
		{
		this->QCall(i0, i1);
		}
	};

// =================================================================================================
// MARK: - ZCallable (specialization for 3 params)

template <class R,
	class P0, class P1, class P2>
class ZCallable<R(P0,P1,P2)>
:	public ZCounted
	{
public:
	typedef R(Signature)(P0,P1,P2);

	virtual ZQ<R> QCall(P0,P1,P2) = 0;

	inline
	R DCall(const R& iDefault,
		typename ZCallableUtil::VT<P0>::P i0,
		typename ZCallableUtil::VT<P1>::P i1,
		typename ZCallableUtil::VT<P2>::P i2)
		{
		if (ZQ<R> theQ = this->QCall(i0, i1, i2))
			return *theQ;
		return iDefault;
		}
	
	inline
	R Call
		(typename ZCallableUtil::VT<P0>::P i0,
		typename ZCallableUtil::VT<P1>::P i1,
		typename ZCallableUtil::VT<P2>::P i2)
		{
		if (ZQ<R> theQ = this->QCall(i0, i1, i2))
			return *theQ;
		return R();
		}
	};

// =================================================================================================
// MARK: - ZCallable (specialization for 3 params, void return)

template <
	class P0, class P1, class P2>
class ZCallable<void(P0,P1,P2)>
:	public ZCounted
	{
public:
	typedef void(Signature)(P0,P1,P2);

	virtual ZQ<void> QCall(P0,P1,P2) = 0;

	inline
	void Call
		(typename ZCallableUtil::VT<P0>::P i0,
		typename ZCallableUtil::VT<P1>::P i1,
		typename ZCallableUtil::VT<P2>::P i2)
		{
		this->QCall(i0, i1, i2);
		}
	};

// =================================================================================================
// MARK: - ZCallable (specialization for 4 params)

template <class R,
	class P0, class P1, class P2, class P3>
class ZCallable<R(P0,P1,P2,P3)>
:	public ZCounted
	{
public:
	typedef R(Signature)(P0,P1,P2,P3);

	virtual ZQ<R> QCall(P0,P1,P2,P3) = 0;

	inline
	R DCall(const R& iDefault,
		typename ZCallableUtil::VT<P0>::P i0,
		typename ZCallableUtil::VT<P1>::P i1,
		typename ZCallableUtil::VT<P2>::P i2,
		typename ZCallableUtil::VT<P3>::P i3)
		{
		if (ZQ<R> theQ = this->QCall(i0, i1, i2, i3))
			return *theQ;
		return iDefault;
		}
	
	inline
	R Call
		(typename ZCallableUtil::VT<P0>::P i0,
		typename ZCallableUtil::VT<P1>::P i1,
		typename ZCallableUtil::VT<P2>::P i2,
		typename ZCallableUtil::VT<P3>::P i3)
		{
		if (ZQ<R> theQ = this->QCall(i0, i1, i2, i3))
			return *theQ;
		return R();
		}
	};

// =================================================================================================
// MARK: - ZCallable (specialization for 4 params, void return)

template <
	class P0, class P1, class P2, class P3>
class ZCallable<void(P0,P1,P2,P3)>
:	public ZCounted
	{
public:
	typedef void(Signature)(P0,P1,P2,P3);

	virtual ZQ<void> QCall(P0,P1,P2,P3) = 0;

	inline
	void Call
		(typename ZCallableUtil::VT<P0>::P i0,
		typename ZCallableUtil::VT<P1>::P i1,
		typename ZCallableUtil::VT<P2>::P i2,
		typename ZCallableUtil::VT<P3>::P i3)
		{
		this->QCall(i0, i1, i2, i3);
		}
	};

// =================================================================================================
// MARK: - ZCallable (specialization for 5 params)

template <class R,
	class P0, class P1, class P2, class P3,
	class P4>
class ZCallable<R(P0,P1,P2,P3,P4)>
:	public ZCounted
	{
public:
	typedef R(Signature)(P0,P1,P2,P3,P4);

	virtual ZQ<R> QCall(P0,P1,P2,P3,P4) = 0;

	inline
	R DCall(const R& iDefault,
		typename ZCallableUtil::VT<P0>::P i0,
		typename ZCallableUtil::VT<P1>::P i1,
		typename ZCallableUtil::VT<P2>::P i2,
		typename ZCallableUtil::VT<P3>::P i3,
		typename ZCallableUtil::VT<P4>::P i4)
		{
		if (ZQ<R> theQ = this->QCall(i0, i1, i2, i3, i4))
			return *theQ;
		return iDefault;
		}
	
	inline
	R Call
		(typename ZCallableUtil::VT<P0>::P i0,
		typename ZCallableUtil::VT<P1>::P i1,
		typename ZCallableUtil::VT<P2>::P i2,
		typename ZCallableUtil::VT<P3>::P i3,
		typename ZCallableUtil::VT<P4>::P i4)
		{
		if (ZQ<R> theQ = this->QCall(i0, i1, i2, i3, i4))
			return *theQ;
		return R();
		}
	};

// =================================================================================================
// MARK: - ZCallable (specialization for 5 params, void return)

template <
	class P0, class P1, class P2, class P3,
	class P4>
class ZCallable<void(P0,P1,P2,P3,P4)>
:	public ZCounted
	{
public:
	typedef void(Signature)(P0,P1,P2,P3,P4);

	virtual ZQ<void> QCall(P0,P1,P2,P3,P4) = 0;
	
	inline
	void Call
		(typename ZCallableUtil::VT<P0>::P i0,
		typename ZCallableUtil::VT<P1>::P i1,
		typename ZCallableUtil::VT<P2>::P i2,
		typename ZCallableUtil::VT<P3>::P i3,
		typename ZCallableUtil::VT<P4>::P i4)
		{
		this->QCall(i0, i1, i2, i3, i4);
		}
	};

// =================================================================================================
// MARK: - ZCallable (specialization for 6 params)

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5>
class ZCallable<R(P0,P1,P2,P3,P4,P5)>
:	public ZCounted
	{
public:
	typedef R(Signature)(P0,P1,P2,P3,P4,P5);

	virtual ZQ<R> QCall(P0,P1,P2,P3,P4,P5) = 0;

	inline
	R DCall(const R& iDefault,
		typename ZCallableUtil::VT<P0>::P i0,
		typename ZCallableUtil::VT<P1>::P i1,
		typename ZCallableUtil::VT<P2>::P i2,
		typename ZCallableUtil::VT<P3>::P i3,
		typename ZCallableUtil::VT<P4>::P i4,
		typename ZCallableUtil::VT<P5>::P i5)
		{
		if (ZQ<R> theQ = this->QCall(i0, i1, i2, i3, i4, i5))
			return *theQ;
		return iDefault;
		}
	
	inline
	R Call
		(typename ZCallableUtil::VT<P0>::P i0,
		typename ZCallableUtil::VT<P1>::P i1,
		typename ZCallableUtil::VT<P2>::P i2,
		typename ZCallableUtil::VT<P3>::P i3,
		typename ZCallableUtil::VT<P4>::P i4,
		typename ZCallableUtil::VT<P5>::P i5)
		{
		if (ZQ<R> theQ = this->QCall(i0, i1, i2, i3, i4, i5))
			return *theQ;
		return R();
		}
	};

// =================================================================================================
// MARK: - ZCallable (specialization for 6 params, void return)

template <
	class P0, class P1, class P2, class P3,
	class P4, class P5>
class ZCallable<void(P0,P1,P2,P3,P4,P5)>
:	public ZCounted
	{
public:
	typedef void(Signature)(P0,P1,P2,P3,P4,P5);

	virtual ZQ<void> QCall(P0,P1,P2,P3,P4,P5) = 0;

	inline
	void Call
		(typename ZCallableUtil::VT<P0>::P i0,
		typename ZCallableUtil::VT<P1>::P i1,
		typename ZCallableUtil::VT<P2>::P i2,
		typename ZCallableUtil::VT<P3>::P i3,
		typename ZCallableUtil::VT<P4>::P i4,
		typename ZCallableUtil::VT<P5>::P i5)
		{
		this->QCall(i0, i1, i2, i3, i4, i5);
		}
	};

// =================================================================================================
// MARK: - ZCallable (specialization for 7 params)

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6>
class ZCallable<R(P0,P1,P2,P3,P4,P5,P6)>
:	public ZCounted
	{
public:
	typedef R(Signature)(P0,P1,P2,P3,P4,P5,P6);

	virtual ZQ<R> QCall(P0,P1,P2,P3,P4,P5,P6) = 0;

	inline
	R DCall(const R& iDefault,
		typename ZCallableUtil::VT<P0>::P i0,
		typename ZCallableUtil::VT<P1>::P i1,
		typename ZCallableUtil::VT<P2>::P i2,
		typename ZCallableUtil::VT<P3>::P i3,
		typename ZCallableUtil::VT<P4>::P i4,
		typename ZCallableUtil::VT<P5>::P i5,
		typename ZCallableUtil::VT<P6>::P i6)
		{
		if (ZQ<R> theQ = this->QCall(i0, i1, i2, i3, i4, i5, i6))
			return *theQ;
		return iDefault;
		}
	
	inline
	R Call
		(typename ZCallableUtil::VT<P0>::P i0,
		typename ZCallableUtil::VT<P1>::P i1,
		typename ZCallableUtil::VT<P2>::P i2,
		typename ZCallableUtil::VT<P3>::P i3,
		typename ZCallableUtil::VT<P4>::P i4,
		typename ZCallableUtil::VT<P5>::P i5,
		typename ZCallableUtil::VT<P6>::P i6)
		{
		if (ZQ<R> theQ = this->QCall(i0, i1, i2, i3, i4, i5, i6))
			return *theQ;
		return R();
		}
	};

// =================================================================================================
// MARK: - ZCallable (specialization for 7 params, void return)

template <
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6>
class ZCallable<void(P0,P1,P2,P3,P4,P5,P6)>
:	public ZCounted
	{
public:
	typedef void(Signature)(P0,P1,P2,P3,P4,P5,P6);

	virtual ZQ<void> QCall(P0,P1,P2,P3,P4,P5,P6) = 0;

	inline
	void Call
		(typename ZCallableUtil::VT<P0>::P i0,
		typename ZCallableUtil::VT<P1>::P i1,
		typename ZCallableUtil::VT<P2>::P i2,
		typename ZCallableUtil::VT<P3>::P i3,
		typename ZCallableUtil::VT<P4>::P i4,
		typename ZCallableUtil::VT<P5>::P i5,
		typename ZCallableUtil::VT<P6>::P i6)
		{
		this->QCall(i0, i1, i2, i3, i4, i5, i6);
		}
	};

// =================================================================================================
// MARK: - ZCallable (specialization for 8 params)

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7>
class ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7)>
:	public ZCounted
	{
public:
	typedef R(Signature)(P0,P1,P2,P3,P4,P5,P6,P7);

	virtual ZQ<R> QCall(P0,P1,P2,P3,P4,P5,P6,P7) = 0;

	inline
	R DCall(const R& iDefault,
		typename ZCallableUtil::VT<P0>::P i0,
		typename ZCallableUtil::VT<P1>::P i1,
		typename ZCallableUtil::VT<P2>::P i2,
		typename ZCallableUtil::VT<P3>::P i3,
		typename ZCallableUtil::VT<P4>::P i4,
		typename ZCallableUtil::VT<P5>::P i5,
		typename ZCallableUtil::VT<P6>::P i6,
		typename ZCallableUtil::VT<P7>::P i7)
		{
		if (ZQ<R> theQ = this->QCall(i0, i1, i2, i3, i4, i5, i6, i7))
			return *theQ;
		return iDefault;
		}
	
	inline
	R Call
		(typename ZCallableUtil::VT<P0>::P i0,
		typename ZCallableUtil::VT<P1>::P i1,
		typename ZCallableUtil::VT<P2>::P i2,
		typename ZCallableUtil::VT<P3>::P i3,
		typename ZCallableUtil::VT<P4>::P i4,
		typename ZCallableUtil::VT<P5>::P i5,
		typename ZCallableUtil::VT<P6>::P i6,
		typename ZCallableUtil::VT<P7>::P i7)
		{
		if (ZQ<R> theQ = this->QCall(i0, i1, i2, i3, i4, i5, i6, i7))
			return *theQ;
		return R();
		}
	};

// =================================================================================================
// MARK: - ZCallable (specialization for 8 params, void return)

template <
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7>
class ZCallable<void(P0,P1,P2,P3,P4,P5,P6,P7)>
:	public ZCounted
	{
public:
	typedef void(Signature)(P0,P1,P2,P3,P4,P5,P6,P7);

	virtual ZQ<void> QCall(P0,P1,P2,P3,P4,P5,P6,P7) = 0;

	inline
	void Call
		(typename ZCallableUtil::VT<P0>::P i0,
		typename ZCallableUtil::VT<P1>::P i1,
		typename ZCallableUtil::VT<P2>::P i2,
		typename ZCallableUtil::VT<P3>::P i3,
		typename ZCallableUtil::VT<P4>::P i4,
		typename ZCallableUtil::VT<P5>::P i5,
		typename ZCallableUtil::VT<P6>::P i6,
		typename ZCallableUtil::VT<P7>::P i7)
		{
		this->QCall(i0, i1, i2, i3, i4, i5, i6, i7);
		}
	};

// =================================================================================================
// MARK: - ZCallable (specialization for 9 params)

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8>
class ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8)>
:	public ZCounted
	{
public:
	typedef R(Signature)(P0,P1,P2,P3,P4,P5,P6,P7,P8);

	virtual ZQ<R> QCall(P0,P1,P2,P3,P4,P5,P6,P7,P8) = 0;

	inline
	R DCall(const R& iDefault,
		typename ZCallableUtil::VT<P0>::P i0,
		typename ZCallableUtil::VT<P1>::P i1,
		typename ZCallableUtil::VT<P2>::P i2,
		typename ZCallableUtil::VT<P3>::P i3,
		typename ZCallableUtil::VT<P4>::P i4,
		typename ZCallableUtil::VT<P5>::P i5,
		typename ZCallableUtil::VT<P6>::P i6,
		typename ZCallableUtil::VT<P7>::P i7,
		typename ZCallableUtil::VT<P8>::P i8)
		{
		if (ZQ<R> theQ = this->QCall(i0, i1, i2, i3, i4, i5, i6, i7, i8))
			return *theQ;
		return iDefault;
		}
	
	inline
	R Call
		(typename ZCallableUtil::VT<P0>::P i0,
		typename ZCallableUtil::VT<P1>::P i1,
		typename ZCallableUtil::VT<P2>::P i2,
		typename ZCallableUtil::VT<P3>::P i3,
		typename ZCallableUtil::VT<P4>::P i4,
		typename ZCallableUtil::VT<P5>::P i5,
		typename ZCallableUtil::VT<P6>::P i6,
		typename ZCallableUtil::VT<P7>::P i7,
		typename ZCallableUtil::VT<P8>::P i8)
		{
		if (ZQ<R> theQ = this->QCall(i0, i1, i2, i3, i4, i5, i6, i7, i8))
			return *theQ;
		return R();
		}
	};

// =================================================================================================
// MARK: - ZCallable (specialization for 9 params, void return)

template <
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8>
class ZCallable<void(P0,P1,P2,P3,P4,P5,P6,P7,P8)>
:	public ZCounted
	{
public:
	typedef void(Signature)(P0,P1,P2,P3,P4,P5,P6,P7,P8);

	virtual ZQ<void> QCall(P0,P1,P2,P3,P4,P5,P6,P7,P8) = 0;

	inline
	void Call
		(typename ZCallableUtil::VT<P0>::P i0,
		typename ZCallableUtil::VT<P1>::P i1,
		typename ZCallableUtil::VT<P2>::P i2,
		typename ZCallableUtil::VT<P3>::P i3,
		typename ZCallableUtil::VT<P4>::P i4,
		typename ZCallableUtil::VT<P5>::P i5,
		typename ZCallableUtil::VT<P6>::P i6,
		typename ZCallableUtil::VT<P7>::P i7,
		typename ZCallableUtil::VT<P8>::P i8)
		{
		this->QCall(i0, i1, i2, i3, i4, i5, i6, i7, i8);
		}
	};

// =================================================================================================
// MARK: - ZCallable (specialization for 10 params)

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9>
class ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9)>
:	public ZCounted
	{
public:
	typedef R(Signature)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9);

	virtual ZQ<R> QCall(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9) = 0;

	inline
	R DCall(const R& iDefault,
		typename ZCallableUtil::VT<P0>::P i0,
		typename ZCallableUtil::VT<P1>::P i1,
		typename ZCallableUtil::VT<P2>::P i2,
		typename ZCallableUtil::VT<P3>::P i3,
		typename ZCallableUtil::VT<P4>::P i4,
		typename ZCallableUtil::VT<P5>::P i5,
		typename ZCallableUtil::VT<P6>::P i6,
		typename ZCallableUtil::VT<P7>::P i7,
		typename ZCallableUtil::VT<P8>::P i8,
		typename ZCallableUtil::VT<P9>::P i9)
		{
		if (ZQ<R> theQ = this->QCall(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9))
			return *theQ;
		return iDefault;
		}
	
	inline
	R Call
		(typename ZCallableUtil::VT<P0>::P i0,
		typename ZCallableUtil::VT<P1>::P i1,
		typename ZCallableUtil::VT<P2>::P i2,
		typename ZCallableUtil::VT<P3>::P i3,
		typename ZCallableUtil::VT<P4>::P i4,
		typename ZCallableUtil::VT<P5>::P i5,
		typename ZCallableUtil::VT<P6>::P i6,
		typename ZCallableUtil::VT<P7>::P i7,
		typename ZCallableUtil::VT<P8>::P i8,
		typename ZCallableUtil::VT<P9>::P i9)
		{
		if (ZQ<R> theQ = this->QCall(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9))
			return *theQ;
		return R();
		}
	};

// =================================================================================================
// MARK: - ZCallable (specialization for 10 params, void return)

template <
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9>
class ZCallable<void(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9)>
:	public ZCounted
	{
public:
	typedef void(Signature)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9);

	virtual ZQ<void> QCall(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9) = 0;

	inline
	void Call
		(typename ZCallableUtil::VT<P0>::P i0,
		typename ZCallableUtil::VT<P1>::P i1,
		typename ZCallableUtil::VT<P2>::P i2,
		typename ZCallableUtil::VT<P3>::P i3,
		typename ZCallableUtil::VT<P4>::P i4,
		typename ZCallableUtil::VT<P5>::P i5,
		typename ZCallableUtil::VT<P6>::P i6,
		typename ZCallableUtil::VT<P7>::P i7,
		typename ZCallableUtil::VT<P8>::P i8,
		typename ZCallableUtil::VT<P9>::P i9)
		{
		this->QCall(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9);
		}
	};

// =================================================================================================
// MARK: - ZCallable (specialization for 11 params)

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA>
class ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA)>
:	public ZCounted
	{
public:
	typedef R(Signature)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA);

	virtual ZQ<R> QCall(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA) = 0;

	inline
	R DCall(const R& iDefault,
		typename ZCallableUtil::VT<P0>::P i0,
		typename ZCallableUtil::VT<P1>::P i1,
		typename ZCallableUtil::VT<P2>::P i2,
		typename ZCallableUtil::VT<P3>::P i3,
		typename ZCallableUtil::VT<P4>::P i4,
		typename ZCallableUtil::VT<P5>::P i5,
		typename ZCallableUtil::VT<P6>::P i6,
		typename ZCallableUtil::VT<P7>::P i7,
		typename ZCallableUtil::VT<P8>::P i8,
		typename ZCallableUtil::VT<P9>::P i9,
		typename ZCallableUtil::VT<PA>::P iA)
		{
		if (ZQ<R> theQ = this->QCall(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA))
			return *theQ;
		return iDefault;
		}
	
	inline
	R Call
		(typename ZCallableUtil::VT<P0>::P i0,
		typename ZCallableUtil::VT<P1>::P i1,
		typename ZCallableUtil::VT<P2>::P i2,
		typename ZCallableUtil::VT<P3>::P i3,
		typename ZCallableUtil::VT<P4>::P i4,
		typename ZCallableUtil::VT<P5>::P i5,
		typename ZCallableUtil::VT<P6>::P i6,
		typename ZCallableUtil::VT<P7>::P i7,
		typename ZCallableUtil::VT<P8>::P i8,
		typename ZCallableUtil::VT<P9>::P i9,
		typename ZCallableUtil::VT<PA>::P iA)
		{
		if (ZQ<R> theQ = this->QCall(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA))
			return *theQ;
		return R();
		}
	};

// =================================================================================================
// MARK: - ZCallable (specialization for 11 params, void return)

template <
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA>
class ZCallable<void(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA)>
:	public ZCounted
	{
public:
	typedef void(Signature)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA);

	virtual ZQ<void> QCall(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA) = 0;

	inline
	void Call
		(typename ZCallableUtil::VT<P0>::P i0,
		typename ZCallableUtil::VT<P1>::P i1,
		typename ZCallableUtil::VT<P2>::P i2,
		typename ZCallableUtil::VT<P3>::P i3,
		typename ZCallableUtil::VT<P4>::P i4,
		typename ZCallableUtil::VT<P5>::P i5,
		typename ZCallableUtil::VT<P6>::P i6,
		typename ZCallableUtil::VT<P7>::P i7,
		typename ZCallableUtil::VT<P8>::P i8,
		typename ZCallableUtil::VT<P9>::P i9,
		typename ZCallableUtil::VT<PA>::P iA)
		{
		this->QCall(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA);
		}
	};

// =================================================================================================
// MARK: - ZCallable (specialization for 12 params)

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB>
class ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB)>
:	public ZCounted
	{
public:
	typedef R(Signature)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB);

	virtual ZQ<R> QCall(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB) = 0;

	inline
	R DCall(const R& iDefault,
		typename ZCallableUtil::VT<P0>::P i0,
		typename ZCallableUtil::VT<P1>::P i1,
		typename ZCallableUtil::VT<P2>::P i2,
		typename ZCallableUtil::VT<P3>::P i3,
		typename ZCallableUtil::VT<P4>::P i4,
		typename ZCallableUtil::VT<P5>::P i5,
		typename ZCallableUtil::VT<P6>::P i6,
		typename ZCallableUtil::VT<P7>::P i7,
		typename ZCallableUtil::VT<P8>::P i8,
		typename ZCallableUtil::VT<P9>::P i9,
		typename ZCallableUtil::VT<PA>::P iA,
		typename ZCallableUtil::VT<PB>::P iB)
		{
		if (ZQ<R> theQ = this->QCall(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB))
			return *theQ;
		return iDefault;
		}
	
	inline
	R Call
		(typename ZCallableUtil::VT<P0>::P i0,
		typename ZCallableUtil::VT<P1>::P i1,
		typename ZCallableUtil::VT<P2>::P i2,
		typename ZCallableUtil::VT<P3>::P i3,
		typename ZCallableUtil::VT<P4>::P i4,
		typename ZCallableUtil::VT<P5>::P i5,
		typename ZCallableUtil::VT<P6>::P i6,
		typename ZCallableUtil::VT<P7>::P i7,
		typename ZCallableUtil::VT<P8>::P i8,
		typename ZCallableUtil::VT<P9>::P i9,
		typename ZCallableUtil::VT<PA>::P iA,
		typename ZCallableUtil::VT<PB>::P iB)
		{
		if (ZQ<R> theQ = this->QCall(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB))
			return *theQ;
		return R();
		}
	};

// =================================================================================================
// MARK: - ZCallable (specialization for 12 params, void return)

template <
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB>
class ZCallable<void(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB)>
:	public ZCounted
	{
public:
	typedef void(Signature)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB);

	virtual ZQ<void> QCall(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB) = 0;

	inline
	void Call
		(typename ZCallableUtil::VT<P0>::P i0,
		typename ZCallableUtil::VT<P1>::P i1,
		typename ZCallableUtil::VT<P2>::P i2,
		typename ZCallableUtil::VT<P3>::P i3,
		typename ZCallableUtil::VT<P4>::P i4,
		typename ZCallableUtil::VT<P5>::P i5,
		typename ZCallableUtil::VT<P6>::P i6,
		typename ZCallableUtil::VT<P7>::P i7,
		typename ZCallableUtil::VT<P8>::P i8,
		typename ZCallableUtil::VT<P9>::P i9,
		typename ZCallableUtil::VT<PA>::P iA,
		typename ZCallableUtil::VT<PB>::P iB)
		{
		this->QCall(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB);
		}
	};

// =================================================================================================
// MARK: - ZCallable (specialization for 13 params)

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB,
	class PC>
class ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC)>
:	public ZCounted
	{
public:
	typedef R(Signature)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC);

	virtual ZQ<R> QCall(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC) = 0;

	inline
	R DCall(const R& iDefault,
		typename ZCallableUtil::VT<P0>::P i0,
		typename ZCallableUtil::VT<P1>::P i1,
		typename ZCallableUtil::VT<P2>::P i2,
		typename ZCallableUtil::VT<P3>::P i3,
		typename ZCallableUtil::VT<P4>::P i4,
		typename ZCallableUtil::VT<P5>::P i5,
		typename ZCallableUtil::VT<P6>::P i6,
		typename ZCallableUtil::VT<P7>::P i7,
		typename ZCallableUtil::VT<P8>::P i8,
		typename ZCallableUtil::VT<P9>::P i9,
		typename ZCallableUtil::VT<PA>::P iA,
		typename ZCallableUtil::VT<PB>::P iB,
		typename ZCallableUtil::VT<PC>::P iC)
		{
		if (ZQ<R> theQ = this->QCall(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC))
			return *theQ;
		return iDefault;
		}
	
	inline
	R Call
		(typename ZCallableUtil::VT<P0>::P i0,
		typename ZCallableUtil::VT<P1>::P i1,
		typename ZCallableUtil::VT<P2>::P i2,
		typename ZCallableUtil::VT<P3>::P i3,
		typename ZCallableUtil::VT<P4>::P i4,
		typename ZCallableUtil::VT<P5>::P i5,
		typename ZCallableUtil::VT<P6>::P i6,
		typename ZCallableUtil::VT<P7>::P i7,
		typename ZCallableUtil::VT<P8>::P i8,
		typename ZCallableUtil::VT<P9>::P i9,
		typename ZCallableUtil::VT<PA>::P iA,
		typename ZCallableUtil::VT<PB>::P iB,
		typename ZCallableUtil::VT<PC>::P iC)
		{
		if (ZQ<R> theQ = this->QCall(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC))
			return *theQ;
		return R();
		}
	};

// =================================================================================================
// MARK: - ZCallable (specialization for 13 params, void return)

template <
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB,
	class PC>
class ZCallable<void(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC)>
:	public ZCounted
	{
public:
	typedef void(Signature)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC);

	virtual ZQ<void> QCall(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC) = 0;

	inline
	void Call
		(typename ZCallableUtil::VT<P0>::P i0,
		typename ZCallableUtil::VT<P1>::P i1,
		typename ZCallableUtil::VT<P2>::P i2,
		typename ZCallableUtil::VT<P3>::P i3,
		typename ZCallableUtil::VT<P4>::P i4,
		typename ZCallableUtil::VT<P5>::P i5,
		typename ZCallableUtil::VT<P6>::P i6,
		typename ZCallableUtil::VT<P7>::P i7,
		typename ZCallableUtil::VT<P8>::P i8,
		typename ZCallableUtil::VT<P9>::P i9,
		typename ZCallableUtil::VT<PA>::P iA,
		typename ZCallableUtil::VT<PB>::P iB,
		typename ZCallableUtil::VT<PC>::P iC)
		{
		this->QCall(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC);
		}
	};

// =================================================================================================
// MARK: - ZCallable (specialization for 14 params)

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB,
	class PC, class PD>
class ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD)>
:	public ZCounted
	{
public:
	typedef R(Signature)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD);

	virtual ZQ<R> QCall(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD) = 0;

	inline
	R DCall(const R& iDefault,
		typename ZCallableUtil::VT<P0>::P i0,
		typename ZCallableUtil::VT<P1>::P i1,
		typename ZCallableUtil::VT<P2>::P i2,
		typename ZCallableUtil::VT<P3>::P i3,
		typename ZCallableUtil::VT<P4>::P i4,
		typename ZCallableUtil::VT<P5>::P i5,
		typename ZCallableUtil::VT<P6>::P i6,
		typename ZCallableUtil::VT<P7>::P i7,
		typename ZCallableUtil::VT<P8>::P i8,
		typename ZCallableUtil::VT<P9>::P i9,
		typename ZCallableUtil::VT<PA>::P iA,
		typename ZCallableUtil::VT<PB>::P iB,
		typename ZCallableUtil::VT<PC>::P iC,
		typename ZCallableUtil::VT<PD>::P iD)
		{
		if (ZQ<R> theQ = this->QCall(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD))
			return *theQ;
		return iDefault;
		}
	
	inline
	R Call
		(typename ZCallableUtil::VT<P0>::P i0,
		typename ZCallableUtil::VT<P1>::P i1,
		typename ZCallableUtil::VT<P2>::P i2,
		typename ZCallableUtil::VT<P3>::P i3,
		typename ZCallableUtil::VT<P4>::P i4,
		typename ZCallableUtil::VT<P5>::P i5,
		typename ZCallableUtil::VT<P6>::P i6,
		typename ZCallableUtil::VT<P7>::P i7,
		typename ZCallableUtil::VT<P8>::P i8,
		typename ZCallableUtil::VT<P9>::P i9,
		typename ZCallableUtil::VT<PA>::P iA,
		typename ZCallableUtil::VT<PB>::P iB,
		typename ZCallableUtil::VT<PC>::P iC,
		typename ZCallableUtil::VT<PD>::P iD)
		{
		if (ZQ<R> theQ = this->QCall(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD))
			return *theQ;
		return R();
		}
	};

// =================================================================================================
// MARK: - ZCallable (specialization for 14 params, void return)

template <
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB,
	class PC, class PD>
class ZCallable<void(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD)>
:	public ZCounted
	{
public:
	typedef void(Signature)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD);

	virtual ZQ<void> QCall(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD) = 0;

	inline
	void Call
		(typename ZCallableUtil::VT<P0>::P i0,
		typename ZCallableUtil::VT<P1>::P i1,
		typename ZCallableUtil::VT<P2>::P i2,
		typename ZCallableUtil::VT<P3>::P i3,
		typename ZCallableUtil::VT<P4>::P i4,
		typename ZCallableUtil::VT<P5>::P i5,
		typename ZCallableUtil::VT<P6>::P i6,
		typename ZCallableUtil::VT<P7>::P i7,
		typename ZCallableUtil::VT<P8>::P i8,
		typename ZCallableUtil::VT<P9>::P i9,
		typename ZCallableUtil::VT<PA>::P iA,
		typename ZCallableUtil::VT<PB>::P iB,
		typename ZCallableUtil::VT<PC>::P iC,
		typename ZCallableUtil::VT<PD>::P iD)
		{
		this->QCall(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD);
		}
	};

// =================================================================================================
// MARK: - ZCallable (specialization for 15 params)

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB,
	class PC, class PD, class PE>
class ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE)>
:	public ZCounted
	{
public:
	typedef R(Signature)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE);

	virtual ZQ<R> QCall(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE) = 0;

	inline
	R DCall(const R& iDefault,
		typename ZCallableUtil::VT<P0>::P i0,
		typename ZCallableUtil::VT<P1>::P i1,
		typename ZCallableUtil::VT<P2>::P i2,
		typename ZCallableUtil::VT<P3>::P i3,
		typename ZCallableUtil::VT<P4>::P i4,
		typename ZCallableUtil::VT<P5>::P i5,
		typename ZCallableUtil::VT<P6>::P i6,
		typename ZCallableUtil::VT<P7>::P i7,
		typename ZCallableUtil::VT<P8>::P i8,
		typename ZCallableUtil::VT<P9>::P i9,
		typename ZCallableUtil::VT<PA>::P iA,
		typename ZCallableUtil::VT<PB>::P iB,
		typename ZCallableUtil::VT<PC>::P iC,
		typename ZCallableUtil::VT<PD>::P iD,
		typename ZCallableUtil::VT<PE>::P iE)
		{
		if (ZQ<R> theQ = this->QCall(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD, iE))
			return *theQ;
		return iDefault;
		}
	
	inline
	R Call
		(typename ZCallableUtil::VT<P0>::P i0,
		typename ZCallableUtil::VT<P1>::P i1,
		typename ZCallableUtil::VT<P2>::P i2,
		typename ZCallableUtil::VT<P3>::P i3,
		typename ZCallableUtil::VT<P4>::P i4,
		typename ZCallableUtil::VT<P5>::P i5,
		typename ZCallableUtil::VT<P6>::P i6,
		typename ZCallableUtil::VT<P7>::P i7,
		typename ZCallableUtil::VT<P8>::P i8,
		typename ZCallableUtil::VT<P9>::P i9,
		typename ZCallableUtil::VT<PA>::P iA,
		typename ZCallableUtil::VT<PB>::P iB,
		typename ZCallableUtil::VT<PC>::P iC,
		typename ZCallableUtil::VT<PD>::P iD,
		typename ZCallableUtil::VT<PE>::P iE)
		{
		if (ZQ<R> theQ = this->QCall(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD, iE))
			return *theQ;
		return R();
		}
	};

// =================================================================================================
// MARK: - ZCallable (specialization for 15 params, void return)

template <
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB,
	class PC, class PD, class PE>
class ZCallable<void(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE)>
:	public ZCounted
	{
public:
	typedef void(Signature)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE);

	virtual ZQ<void> QCall(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE) = 0;

	inline
	void Call
		(typename ZCallableUtil::VT<P0>::P i0,
		typename ZCallableUtil::VT<P1>::P i1,
		typename ZCallableUtil::VT<P2>::P i2,
		typename ZCallableUtil::VT<P3>::P i3,
		typename ZCallableUtil::VT<P4>::P i4,
		typename ZCallableUtil::VT<P5>::P i5,
		typename ZCallableUtil::VT<P6>::P i6,
		typename ZCallableUtil::VT<P7>::P i7,
		typename ZCallableUtil::VT<P8>::P i8,
		typename ZCallableUtil::VT<P9>::P i9,
		typename ZCallableUtil::VT<PA>::P iA,
		typename ZCallableUtil::VT<PB>::P iB,
		typename ZCallableUtil::VT<PC>::P iC,
		typename ZCallableUtil::VT<PD>::P iD,
		typename ZCallableUtil::VT<PE>::P iE)
		{
		this->QCall(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD, iE);
		}
	};

// =================================================================================================
// MARK: - ZCallable (specialization for 16 params)

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB,
	class PC, class PD, class PE, class PF>
class ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE,PF)>
:	public ZCounted
	{
public:
	typedef R(Signature)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE,PF);

	virtual ZQ<R> QCall(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE,PF) = 0;

	inline
	R DCall(const R& iDefault,
		typename ZCallableUtil::VT<P0>::P i0,
		typename ZCallableUtil::VT<P1>::P i1,
		typename ZCallableUtil::VT<P2>::P i2,
		typename ZCallableUtil::VT<P3>::P i3,
		typename ZCallableUtil::VT<P4>::P i4,
		typename ZCallableUtil::VT<P5>::P i5,
		typename ZCallableUtil::VT<P6>::P i6,
		typename ZCallableUtil::VT<P7>::P i7,
		typename ZCallableUtil::VT<P8>::P i8,
		typename ZCallableUtil::VT<P9>::P i9,
		typename ZCallableUtil::VT<PA>::P iA,
		typename ZCallableUtil::VT<PB>::P iB,
		typename ZCallableUtil::VT<PC>::P iC,
		typename ZCallableUtil::VT<PD>::P iD,
		typename ZCallableUtil::VT<PE>::P iE,
		typename ZCallableUtil::VT<PF>::P iF)
		{
		if (ZQ<R> theQ =
			this->QCall(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD, iE, iF))
			{ return *theQ; }
		return iDefault;
		}
	
	inline
	R Call
		(typename ZCallableUtil::VT<P0>::P i0,
		typename ZCallableUtil::VT<P1>::P i1,
		typename ZCallableUtil::VT<P2>::P i2,
		typename ZCallableUtil::VT<P3>::P i3,
		typename ZCallableUtil::VT<P4>::P i4,
		typename ZCallableUtil::VT<P5>::P i5,
		typename ZCallableUtil::VT<P6>::P i6,
		typename ZCallableUtil::VT<P7>::P i7,
		typename ZCallableUtil::VT<P8>::P i8,
		typename ZCallableUtil::VT<P9>::P i9,
		typename ZCallableUtil::VT<PA>::P iA,
		typename ZCallableUtil::VT<PB>::P iB,
		typename ZCallableUtil::VT<PC>::P iC,
		typename ZCallableUtil::VT<PD>::P iD,
		typename ZCallableUtil::VT<PE>::P iE,
		typename ZCallableUtil::VT<PF>::P iF)
		{
		if (ZQ<R> theQ =
			this->QCall(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD, iE, iF))
			{ return *theQ; }
		return R();
		}
	};

// =================================================================================================
// MARK: - ZCallable (specialization for 16 params, void return)

template <
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB,
	class PC, class PD, class PE, class PF>
class ZCallable<void(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE,PF)>
:	public ZCounted
	{
public:
	typedef void(Signature)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE,PF);

	virtual ZQ<void> QCall(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE,PF) = 0;

	inline
	void Call
		(typename ZCallableUtil::VT<P0>::P i0,
		typename ZCallableUtil::VT<P1>::P i1,
		typename ZCallableUtil::VT<P2>::P i2,
		typename ZCallableUtil::VT<P3>::P i3,
		typename ZCallableUtil::VT<P4>::P i4,
		typename ZCallableUtil::VT<P5>::P i5,
		typename ZCallableUtil::VT<P6>::P i6,
		typename ZCallableUtil::VT<P7>::P i7,
		typename ZCallableUtil::VT<P8>::P i8,
		typename ZCallableUtil::VT<P9>::P i9,
		typename ZCallableUtil::VT<PA>::P iA,
		typename ZCallableUtil::VT<PB>::P iB,
		typename ZCallableUtil::VT<PC>::P iC,
		typename ZCallableUtil::VT<PD>::P iD,
		typename ZCallableUtil::VT<PE>::P iE,
		typename ZCallableUtil::VT<PF>::P iF)
		{
		this->QCall(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD, iE, iF);
		}
	};

// =================================================================================================
// MARK: - sCall variants (specialization for 0 params)

template <class R>
ZQ<R> sQCall
	(const ZRef<ZCallable<R(void)> >& iCallable)
	{
	if (iCallable)
		return iCallable->QCall();
	return null;
	}

template <class R>
R sDCall
	(typename ZCallableUtil::VT<R>::P iDefault,
	const ZRef<ZCallable<R(void)> >& iCallable)
	{
	if (iCallable)
		{
		if (ZQ<R> theQ = iCallable->QCall())
			return *theQ;
		}
	return iDefault;
	}

template <class R>
R sCall
	(const ZRef<ZCallable<R(void)> >& iCallable)
	{
	if (iCallable)
		{
		if (ZQ<R> theQ = iCallable->QCall())
			return *theQ;
		}
	return R();
	}

// =================================================================================================
// MARK: - sCall variants (specialization for 1 param)

template <class R,
	class P0>
ZQ<R> sQCall
	(const ZRef<ZCallable<R(P0)> >& iCallable,
	typename ZCallableUtil::VT<P0>::P i0)
	{
	if (iCallable)
		return iCallable->QCall(i0);
	return null;
	}

template <class R,
	class P0>
R sDCall
	(typename ZCallableUtil::VT<R>::P iDefault,
	const ZRef<ZCallable<R(P0)> >& iCallable,
	typename ZCallableUtil::VT<P0>::P i0)
	{
	if (iCallable)
		{
		if (ZQ<R> theQ = iCallable->QCall(i0))
			return *theQ;
		}
	return iDefault;
	}

template <class R,
	class P0>
R sCall
	(const ZRef<ZCallable<R(P0)> >& iCallable,
	typename ZCallableUtil::VT<P0>::P i0)
	{
	if (iCallable)
		{
		if (ZQ<R> theQ = iCallable->QCall(i0))
			return *theQ;
		}
	return R();
	}

// =================================================================================================
// MARK: - sCall variants (specialization for 2 params)

template <class R,
	class P0, class P1>
ZQ<R> sQCall
	(const ZRef<ZCallable<R(P0,P1)> >& iCallable,
	typename ZCallableUtil::VT<P0>::P i0,
	typename ZCallableUtil::VT<P1>::P i1)
	{
	if (iCallable)
		return iCallable->QCall(i0, i1);
	return null;
	}

template <class R,
	class P0, class P1>
R sDCall
	(typename ZCallableUtil::VT<R>::P iDefault,
	const ZRef<ZCallable<R(P0,P1)> >& iCallable,
	typename ZCallableUtil::VT<P0>::P i0,
	typename ZCallableUtil::VT<P1>::P i1)
	{
	if (iCallable)
		{
		if (ZQ<R> theQ = iCallable->QCall(i0, i1))
			return *theQ;
		}
	return iDefault;
	}

template <class R,
	class P0, class P1>
R sCall
	(const ZRef<ZCallable<R(P0,P1)> >& iCallable,
	typename ZCallableUtil::VT<P0>::P i0,
	typename ZCallableUtil::VT<P1>::P i1)
	{
	if (iCallable)
		{
		if (ZQ<R> theQ = iCallable->QCall(i0, i1))
			return *theQ;
		}
	return R();
	}

// =================================================================================================
// MARK: - sCall variants (specialization for 3 params)

template <class R,
	class P0, class P1, class P2>
ZQ<R> sQCall
	(const ZRef<ZCallable<R(P0,P1,P2)> >& iCallable,
	typename ZCallableUtil::VT<P0>::P i0,
	typename ZCallableUtil::VT<P1>::P i1,
	typename ZCallableUtil::VT<P2>::P i2)
	{
	if (iCallable)
		return iCallable->QCall(i0, i1, i2);
	return null;
	}

template <class R,
	class P0, class P1, class P2>
R sDCall
	(typename ZCallableUtil::VT<R>::P iDefault,
	const ZRef<ZCallable<R(P0,P1,P2)> >& iCallable,
	typename ZCallableUtil::VT<P0>::P i0,
	typename ZCallableUtil::VT<P1>::P i1,
	typename ZCallableUtil::VT<P2>::P i2)
	{
	if (iCallable)
		{
		if (ZQ<R> theQ = iCallable->QCall(i0, i1, i2))
			return *theQ;
		}
	return iDefault;
	}

template <class R,
	class P0, class P1, class P2>
R sCall
	(const ZRef<ZCallable<R(P0,P1,P2)> >& iCallable,
	typename ZCallableUtil::VT<P0>::P i0,
	typename ZCallableUtil::VT<P1>::P i1,
	typename ZCallableUtil::VT<P2>::P i2)
	{
	if (iCallable)
		{
		if (ZQ<R> theQ = iCallable->QCall(i0, i1, i2))
			return *theQ;
		}
	return R();
	}

// =================================================================================================
// MARK: - sCall variants (specialization for 4 params)

template <class R,
	class P0, class P1, class P2, class P3>
ZQ<R> sQCall
	(const ZRef<ZCallable<R(P0,P1,P2,P3)> >& iCallable,
	typename ZCallableUtil::VT<P0>::P i0,
	typename ZCallableUtil::VT<P1>::P i1,
	typename ZCallableUtil::VT<P2>::P i2,
	typename ZCallableUtil::VT<P3>::P i3)
	{
	if (iCallable)
		return iCallable->QCall(i0, i1, i2, i3);
	return null;
	}

template <class R,
	class P0, class P1, class P2, class P3>
R sDCall
	(typename ZCallableUtil::VT<R>::P iDefault,
	const ZRef<ZCallable<R(P0,P1,P2,P3)> >& iCallable,
	typename ZCallableUtil::VT<P0>::P i0,
	typename ZCallableUtil::VT<P1>::P i1,
	typename ZCallableUtil::VT<P2>::P i2,
	typename ZCallableUtil::VT<P3>::P i3)
	{
	if (iCallable)
		{
		if (ZQ<R> theQ = iCallable->QCall(i0, i1, i2, i3))
			return *theQ;
		}
	return iDefault;
	}

template <class R,
	class P0, class P1, class P2, class P3>
R sCall
	(const ZRef<ZCallable<R(P0,P1,P2,P3)> >& iCallable,
	typename ZCallableUtil::VT<P0>::P i0,
	typename ZCallableUtil::VT<P1>::P i1,
	typename ZCallableUtil::VT<P2>::P i2,
	typename ZCallableUtil::VT<P3>::P i3)
	{
	if (iCallable)
		{
		if (ZQ<R> theQ = iCallable->QCall(i0, i1, i2, i3))
			return *theQ;
		}
	return R();
	}

// =================================================================================================
// MARK: - sCall variants (specialization for 5 params)

template <class R,
	class P0, class P1, class P2, class P3,
	class P4>
ZQ<R> sQCall
	(const ZRef<ZCallable<R(P0,P1,P2,P3,P4)> >& iCallable,
	typename ZCallableUtil::VT<P0>::P i0,
	typename ZCallableUtil::VT<P1>::P i1,
	typename ZCallableUtil::VT<P2>::P i2,
	typename ZCallableUtil::VT<P3>::P i3,
	typename ZCallableUtil::VT<P4>::P i4)
	{
	if (iCallable)
		return iCallable->QCall(i0, i1, i2, i3, i4);
	return null;
	}

template <class R,
	class P0, class P1, class P2, class P3,
	class P4>
R sDCall
	(typename ZCallableUtil::VT<R>::P iDefault,
	const ZRef<ZCallable<R(P0,P1,P2,P3,P4)> >& iCallable,
	typename ZCallableUtil::VT<P0>::P i0,
	typename ZCallableUtil::VT<P1>::P i1,
	typename ZCallableUtil::VT<P2>::P i2,
	typename ZCallableUtil::VT<P3>::P i3,
	typename ZCallableUtil::VT<P4>::P i4)
	{
	if (iCallable)
		{
		if (ZQ<R> theQ = iCallable->QCall(i0, i1, i2, i3, i4))
			return *theQ;
		}
	return iDefault;
	}

template <class R,
	class P0, class P1, class P2, class P3,
	class P4>
R sCall
	(const ZRef<ZCallable<R(P0,P1,P2,P3,P4)> >& iCallable,
	typename ZCallableUtil::VT<P0>::P i0,
	typename ZCallableUtil::VT<P1>::P i1,
	typename ZCallableUtil::VT<P2>::P i2,
	typename ZCallableUtil::VT<P3>::P i3,
	typename ZCallableUtil::VT<P4>::P i4)
	{
	if (iCallable)
		{
		if (ZQ<R> theQ = iCallable->QCall(i0, i1, i2, i3, i4))
			return *theQ;
		}
	return R();
	}

// =================================================================================================
// MARK: - sCall variants (specialization for 6 params)

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5>
ZQ<R> sQCall
	(const ZRef<ZCallable<R(P0,P1,P2,P3,P4,P5)> >& iCallable,
	typename ZCallableUtil::VT<P0>::P i0,
	typename ZCallableUtil::VT<P1>::P i1,
	typename ZCallableUtil::VT<P2>::P i2,
	typename ZCallableUtil::VT<P3>::P i3,
	typename ZCallableUtil::VT<P4>::P i4,
	typename ZCallableUtil::VT<P5>::P i5)
	{
	if (iCallable)
		return iCallable->QCall(i0, i1, i2, i3, i4, i5);
	return null;
	}

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5>
R sDCall
	(typename ZCallableUtil::VT<R>::P iDefault,
	const ZRef<ZCallable<R(P0,P1,P2,P3,P4,P5)> >& iCallable,
	typename ZCallableUtil::VT<P0>::P i0,
	typename ZCallableUtil::VT<P1>::P i1,
	typename ZCallableUtil::VT<P2>::P i2,
	typename ZCallableUtil::VT<P3>::P i3,
	typename ZCallableUtil::VT<P4>::P i4,
	typename ZCallableUtil::VT<P5>::P i5)
	{
	if (iCallable)
		{
		if (ZQ<R> theQ = iCallable->QCall(i0, i1, i2, i3, i4, i5))
			return *theQ;
		}
	return iDefault;
	}

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5>
R sCall
	(const ZRef<ZCallable<R(P0,P1,P2,P3,P4,P5)> >& iCallable,
	typename ZCallableUtil::VT<P0>::P i0,
	typename ZCallableUtil::VT<P1>::P i1,
	typename ZCallableUtil::VT<P2>::P i2,
	typename ZCallableUtil::VT<P3>::P i3,
	typename ZCallableUtil::VT<P4>::P i4,
	typename ZCallableUtil::VT<P5>::P i5)
	{
	if (iCallable)
		{
		if (ZQ<R> theQ = iCallable->QCall(i0, i1, i2, i3, i4, i5))
			return *theQ;
		}
	return R();
	}

// =================================================================================================
// MARK: - sCall variants (specialization for 7 params)

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6>
ZQ<R> sQCall
	(const ZRef<ZCallable<R(P0,P1,P2,P3,P4,P5,P6)> >& iCallable,
	typename ZCallableUtil::VT<P0>::P i0,
	typename ZCallableUtil::VT<P1>::P i1,
	typename ZCallableUtil::VT<P2>::P i2,
	typename ZCallableUtil::VT<P3>::P i3,
	typename ZCallableUtil::VT<P4>::P i4,
	typename ZCallableUtil::VT<P5>::P i5,
	typename ZCallableUtil::VT<P6>::P i6)
	{
	if (iCallable)
		return iCallable->QCall(i0, i1, i2, i3, i4, i5, i6);
	return null;
	}

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6>
R sDCall
	(typename ZCallableUtil::VT<R>::P iDefault,
	const ZRef<ZCallable<R(P0,P1,P2,P3,P4,P5,P6)> >& iCallable,
	typename ZCallableUtil::VT<P0>::P i0,
	typename ZCallableUtil::VT<P1>::P i1,
	typename ZCallableUtil::VT<P2>::P i2,
	typename ZCallableUtil::VT<P3>::P i3,
	typename ZCallableUtil::VT<P4>::P i4,
	typename ZCallableUtil::VT<P5>::P i5,
	typename ZCallableUtil::VT<P6>::P i6)
	{
	if (iCallable)
		{
		if (ZQ<R> theQ = iCallable->QCall(i0, i1, i2, i3, i4, i5, i6))
			return *theQ;
		}
	return iDefault;
	}

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6>
R sCall
	(const ZRef<ZCallable<R(P0,P1,P2,P3,P4,P5,P6)> >& iCallable,
	typename ZCallableUtil::VT<P0>::P i0,
	typename ZCallableUtil::VT<P1>::P i1,
	typename ZCallableUtil::VT<P2>::P i2,
	typename ZCallableUtil::VT<P3>::P i3,
	typename ZCallableUtil::VT<P4>::P i4,
	typename ZCallableUtil::VT<P5>::P i5,
	typename ZCallableUtil::VT<P6>::P i6)
	{
	if (iCallable)
		{
		if (ZQ<R> theQ = iCallable->QCall(i0, i1, i2, i3, i4, i5, i6))
			return *theQ;
		}
	return R();
	}

// =================================================================================================
// MARK: - sCall variants (specialization for 8 params)

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7>
ZQ<R> sQCall
	(const ZRef<ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7)> >& iCallable,
	typename ZCallableUtil::VT<P0>::P i0,
	typename ZCallableUtil::VT<P1>::P i1,
	typename ZCallableUtil::VT<P2>::P i2,
	typename ZCallableUtil::VT<P3>::P i3,
	typename ZCallableUtil::VT<P4>::P i4,
	typename ZCallableUtil::VT<P5>::P i5,
	typename ZCallableUtil::VT<P6>::P i6,
	typename ZCallableUtil::VT<P7>::P i7)
	{
	if (iCallable)
		return iCallable->QCall(i0, i1, i2, i3, i4, i5, i6, i7);
	return null;
	}

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7>
R sDCall
	(typename ZCallableUtil::VT<R>::P iDefault,
	const ZRef<ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7)> >& iCallable,
	typename ZCallableUtil::VT<P0>::P i0,
	typename ZCallableUtil::VT<P1>::P i1,
	typename ZCallableUtil::VT<P2>::P i2,
	typename ZCallableUtil::VT<P3>::P i3,
	typename ZCallableUtil::VT<P4>::P i4,
	typename ZCallableUtil::VT<P5>::P i5,
	typename ZCallableUtil::VT<P6>::P i6,
	typename ZCallableUtil::VT<P7>::P i7)
	{
	if (iCallable)
		{
		if (ZQ<R> theQ = iCallable->QCall(i0, i1, i2, i3, i4, i5, i6, i7))
			return *theQ;
		}
	return iDefault;
	}

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7>
R sCall
	(const ZRef<ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7)> >& iCallable,
	typename ZCallableUtil::VT<P0>::P i0,
	typename ZCallableUtil::VT<P1>::P i1,
	typename ZCallableUtil::VT<P2>::P i2,
	typename ZCallableUtil::VT<P3>::P i3,
	typename ZCallableUtil::VT<P4>::P i4,
	typename ZCallableUtil::VT<P5>::P i5,
	typename ZCallableUtil::VT<P6>::P i6,
	typename ZCallableUtil::VT<P7>::P i7)
	{
	if (iCallable)
		{
		if (ZQ<R> theQ = iCallable->QCall(i0, i1, i2, i3, i4, i5, i6, i7))
			return *theQ;
		}
	return R();
	}

// =================================================================================================
// MARK: - sCall variants (specialization for 9 params)

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8>
ZQ<R> sQCall
	(const ZRef<ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8)> >& iCallable,
	typename ZCallableUtil::VT<P0>::P i0,
	typename ZCallableUtil::VT<P1>::P i1,
	typename ZCallableUtil::VT<P2>::P i2,
	typename ZCallableUtil::VT<P3>::P i3,
	typename ZCallableUtil::VT<P4>::P i4,
	typename ZCallableUtil::VT<P5>::P i5,
	typename ZCallableUtil::VT<P6>::P i6,
	typename ZCallableUtil::VT<P7>::P i7,
	typename ZCallableUtil::VT<P8>::P i8)
	{
	if (iCallable)
		return iCallable->QCall(i0, i1, i2, i3, i4, i5, i6, i7, i8);
	return null;
	}

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8>
R sDCall
	(typename ZCallableUtil::VT<R>::P iDefault,
	const ZRef<ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8)> >& iCallable,
	typename ZCallableUtil::VT<P0>::P i0,
	typename ZCallableUtil::VT<P1>::P i1,
	typename ZCallableUtil::VT<P2>::P i2,
	typename ZCallableUtil::VT<P3>::P i3,
	typename ZCallableUtil::VT<P4>::P i4,
	typename ZCallableUtil::VT<P5>::P i5,
	typename ZCallableUtil::VT<P6>::P i6,
	typename ZCallableUtil::VT<P7>::P i7,
	typename ZCallableUtil::VT<P8>::P i8)
	{
	if (iCallable)
		{
		if (ZQ<R> theQ = iCallable->QCall(i0, i1, i2, i3, i4, i5, i6, i7, i8))
			return *theQ;
		}
	return iDefault;
	}

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8>
R sCall
	(const ZRef<ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8)> >& iCallable,
	typename ZCallableUtil::VT<P0>::P i0,
	typename ZCallableUtil::VT<P1>::P i1,
	typename ZCallableUtil::VT<P2>::P i2,
	typename ZCallableUtil::VT<P3>::P i3,
	typename ZCallableUtil::VT<P4>::P i4,
	typename ZCallableUtil::VT<P5>::P i5,
	typename ZCallableUtil::VT<P6>::P i6,
	typename ZCallableUtil::VT<P7>::P i7,
	typename ZCallableUtil::VT<P8>::P i8)
	{
	if (iCallable)
		{
		if (ZQ<R> theQ = iCallable->QCall(i0, i1, i2, i3, i4, i5, i6, i7, i8))
			return *theQ;
		}
	return R();
	}

// =================================================================================================
// MARK: - sCall variants (specialization for 10 params)

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9>
ZQ<R> sQCall
	(const ZRef<ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9)> >& iCallable,
	typename ZCallableUtil::VT<P0>::P i0,
	typename ZCallableUtil::VT<P1>::P i1,
	typename ZCallableUtil::VT<P2>::P i2,
	typename ZCallableUtil::VT<P3>::P i3,
	typename ZCallableUtil::VT<P4>::P i4,
	typename ZCallableUtil::VT<P5>::P i5,
	typename ZCallableUtil::VT<P6>::P i6,
	typename ZCallableUtil::VT<P7>::P i7,
	typename ZCallableUtil::VT<P8>::P i8,
	typename ZCallableUtil::VT<P9>::P i9)
	{
	if (iCallable)
		return iCallable->QCall(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9);
	return null;
	}

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9>
R sDCall
	(typename ZCallableUtil::VT<R>::P iDefault,
	const ZRef<ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9)> >& iCallable,
	typename ZCallableUtil::VT<P0>::P i0,
	typename ZCallableUtil::VT<P1>::P i1,
	typename ZCallableUtil::VT<P2>::P i2,
	typename ZCallableUtil::VT<P3>::P i3,
	typename ZCallableUtil::VT<P4>::P i4,
	typename ZCallableUtil::VT<P5>::P i5,
	typename ZCallableUtil::VT<P6>::P i6,
	typename ZCallableUtil::VT<P7>::P i7,
	typename ZCallableUtil::VT<P8>::P i8,
	typename ZCallableUtil::VT<P9>::P i9)
	{
	if (iCallable)
		{
		if (ZQ<R> theQ = iCallable->QCall(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9))
			return *theQ;
		}
	return iDefault;
	}

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9>
R sCall
	(const ZRef<ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9)> >& iCallable,
	typename ZCallableUtil::VT<P0>::P i0,
	typename ZCallableUtil::VT<P1>::P i1,
	typename ZCallableUtil::VT<P2>::P i2,
	typename ZCallableUtil::VT<P3>::P i3,
	typename ZCallableUtil::VT<P4>::P i4,
	typename ZCallableUtil::VT<P5>::P i5,
	typename ZCallableUtil::VT<P6>::P i6,
	typename ZCallableUtil::VT<P7>::P i7,
	typename ZCallableUtil::VT<P8>::P i8,
	typename ZCallableUtil::VT<P9>::P i9)
	{
	if (iCallable)
		{
		if (ZQ<R> theQ = iCallable->QCall(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9))
			return *theQ;
		}
	return R();
	}

// =================================================================================================
// MARK: - sCall variants (specialization for 11 params)

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA>
ZQ<R> sQCall
	(const ZRef<ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA)> >& iCallable,
	typename ZCallableUtil::VT<P0>::P i0,
	typename ZCallableUtil::VT<P1>::P i1,
	typename ZCallableUtil::VT<P2>::P i2,
	typename ZCallableUtil::VT<P3>::P i3,
	typename ZCallableUtil::VT<P4>::P i4,
	typename ZCallableUtil::VT<P5>::P i5,
	typename ZCallableUtil::VT<P6>::P i6,
	typename ZCallableUtil::VT<P7>::P i7,
	typename ZCallableUtil::VT<P8>::P i8,
	typename ZCallableUtil::VT<P9>::P i9,
	typename ZCallableUtil::VT<PA>::P iA)
	{
	if (iCallable)
		return iCallable->QCall(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA);
	return null;
	}

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA>
R sDCall
	(typename ZCallableUtil::VT<R>::P iDefault,
	const ZRef<ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA)> >& iCallable,
	typename ZCallableUtil::VT<P0>::P i0,
	typename ZCallableUtil::VT<P1>::P i1,
	typename ZCallableUtil::VT<P2>::P i2,
	typename ZCallableUtil::VT<P3>::P i3,
	typename ZCallableUtil::VT<P4>::P i4,
	typename ZCallableUtil::VT<P5>::P i5,
	typename ZCallableUtil::VT<P6>::P i6,
	typename ZCallableUtil::VT<P7>::P i7,
	typename ZCallableUtil::VT<P8>::P i8,
	typename ZCallableUtil::VT<P9>::P i9,
	typename ZCallableUtil::VT<PA>::P iA)
	{
	if (iCallable)
		{
		if (ZQ<R> theQ = iCallable->QCall(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA))
			return *theQ;
		}
	return iDefault;
	}

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA>
R sCall
	(const ZRef<ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA)> >& iCallable,
	typename ZCallableUtil::VT<P0>::P i0,
	typename ZCallableUtil::VT<P1>::P i1,
	typename ZCallableUtil::VT<P2>::P i2,
	typename ZCallableUtil::VT<P3>::P i3,
	typename ZCallableUtil::VT<P4>::P i4,
	typename ZCallableUtil::VT<P5>::P i5,
	typename ZCallableUtil::VT<P6>::P i6,
	typename ZCallableUtil::VT<P7>::P i7,
	typename ZCallableUtil::VT<P8>::P i8,
	typename ZCallableUtil::VT<P9>::P i9,
	typename ZCallableUtil::VT<PA>::P iA)
	{
	if (iCallable)
		{
		if (ZQ<R> theQ = iCallable->QCall(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA))
			return *theQ;
		}
	return R();
	}

// =================================================================================================
// MARK: - sCall variants (specialization for 12 params)

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB>
ZQ<R> sQCall
	(const ZRef<ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB)> >& iCallable,
	typename ZCallableUtil::VT<P0>::P i0,
	typename ZCallableUtil::VT<P1>::P i1,
	typename ZCallableUtil::VT<P2>::P i2,
	typename ZCallableUtil::VT<P3>::P i3,
	typename ZCallableUtil::VT<P4>::P i4,
	typename ZCallableUtil::VT<P5>::P i5,
	typename ZCallableUtil::VT<P6>::P i6,
	typename ZCallableUtil::VT<P7>::P i7,
	typename ZCallableUtil::VT<P8>::P i8,
	typename ZCallableUtil::VT<P9>::P i9,
	typename ZCallableUtil::VT<PA>::P iA,
	typename ZCallableUtil::VT<PB>::P iB)
	{
	if (iCallable)
		return iCallable->QCall(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB);
	return null;
	}

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB>
R sDCall
	(typename ZCallableUtil::VT<R>::P iDefault,
	const ZRef<ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB)> >& iCallable,
	typename ZCallableUtil::VT<P0>::P i0,
	typename ZCallableUtil::VT<P1>::P i1,
	typename ZCallableUtil::VT<P2>::P i2,
	typename ZCallableUtil::VT<P3>::P i3,
	typename ZCallableUtil::VT<P4>::P i4,
	typename ZCallableUtil::VT<P5>::P i5,
	typename ZCallableUtil::VT<P6>::P i6,
	typename ZCallableUtil::VT<P7>::P i7,
	typename ZCallableUtil::VT<P8>::P i8,
	typename ZCallableUtil::VT<P9>::P i9,
	typename ZCallableUtil::VT<PA>::P iA,
	typename ZCallableUtil::VT<PB>::P iB)
	{
	if (iCallable)
		{
		if (ZQ<R> theQ = iCallable->QCall(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB))
			return *theQ;
		}
	return iDefault;
	}

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB>
R sCall
	(const ZRef<ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB)> >& iCallable,
	typename ZCallableUtil::VT<P0>::P i0,
	typename ZCallableUtil::VT<P1>::P i1,
	typename ZCallableUtil::VT<P2>::P i2,
	typename ZCallableUtil::VT<P3>::P i3,
	typename ZCallableUtil::VT<P4>::P i4,
	typename ZCallableUtil::VT<P5>::P i5,
	typename ZCallableUtil::VT<P6>::P i6,
	typename ZCallableUtil::VT<P7>::P i7,
	typename ZCallableUtil::VT<P8>::P i8,
	typename ZCallableUtil::VT<P9>::P i9,
	typename ZCallableUtil::VT<PA>::P iA,
	typename ZCallableUtil::VT<PB>::P iB)
	{
	if (iCallable)
		{
		if (ZQ<R> theQ = iCallable->QCall(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB))
			return *theQ;
		}
	return R();
	}

// =================================================================================================
// MARK: - sCall variants (specialization for 13 params)

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB,
	class PC>
ZQ<R> sQCall
	(const ZRef<ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC)> >& iCallable,
	typename ZCallableUtil::VT<P0>::P i0,
	typename ZCallableUtil::VT<P1>::P i1,
	typename ZCallableUtil::VT<P2>::P i2,
	typename ZCallableUtil::VT<P3>::P i3,
	typename ZCallableUtil::VT<P4>::P i4,
	typename ZCallableUtil::VT<P5>::P i5,
	typename ZCallableUtil::VT<P6>::P i6,
	typename ZCallableUtil::VT<P7>::P i7,
	typename ZCallableUtil::VT<P8>::P i8,
	typename ZCallableUtil::VT<P9>::P i9,
	typename ZCallableUtil::VT<PA>::P iA,
	typename ZCallableUtil::VT<PB>::P iB,
	typename ZCallableUtil::VT<PC>::P iC)
	{
	if (iCallable)
		return iCallable->QCall(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC);
	return null;
	}

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB,
	class PC>
R sDCall
	(typename ZCallableUtil::VT<R>::P iDefault,
	const ZRef<ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC)> >& iCallable,
	typename ZCallableUtil::VT<P0>::P i0,
	typename ZCallableUtil::VT<P1>::P i1,
	typename ZCallableUtil::VT<P2>::P i2,
	typename ZCallableUtil::VT<P3>::P i3,
	typename ZCallableUtil::VT<P4>::P i4,
	typename ZCallableUtil::VT<P5>::P i5,
	typename ZCallableUtil::VT<P6>::P i6,
	typename ZCallableUtil::VT<P7>::P i7,
	typename ZCallableUtil::VT<P8>::P i8,
	typename ZCallableUtil::VT<P9>::P i9,
	typename ZCallableUtil::VT<PA>::P iA,
	typename ZCallableUtil::VT<PB>::P iB,
	typename ZCallableUtil::VT<PC>::P iC)
	{
	if (iCallable)
		{
		if (ZQ<R> theQ = iCallable->QCall(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC))
			return *theQ;
		}
	return iDefault;
	}

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB,
	class PC>
R sCall
	(const ZRef<ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC)> >& iCallable,
	typename ZCallableUtil::VT<P0>::P i0,
	typename ZCallableUtil::VT<P1>::P i1,
	typename ZCallableUtil::VT<P2>::P i2,
	typename ZCallableUtil::VT<P3>::P i3,
	typename ZCallableUtil::VT<P4>::P i4,
	typename ZCallableUtil::VT<P5>::P i5,
	typename ZCallableUtil::VT<P6>::P i6,
	typename ZCallableUtil::VT<P7>::P i7,
	typename ZCallableUtil::VT<P8>::P i8,
	typename ZCallableUtil::VT<P9>::P i9,
	typename ZCallableUtil::VT<PA>::P iA,
	typename ZCallableUtil::VT<PB>::P iB,
	typename ZCallableUtil::VT<PC>::P iC)
	{
	if (iCallable)
		{
		if (ZQ<R> theQ = iCallable->QCall(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC))
			return *theQ;
		}
	return R();
	}

// =================================================================================================
// MARK: - sCall variants (specialization for 14 params)

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB,
	class PC, class PD>
ZQ<R> sQCall
	(const ZRef<ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD)> >& iCallable,
	typename ZCallableUtil::VT<P0>::P i0,
	typename ZCallableUtil::VT<P1>::P i1,
	typename ZCallableUtil::VT<P2>::P i2,
	typename ZCallableUtil::VT<P3>::P i3,
	typename ZCallableUtil::VT<P4>::P i4,
	typename ZCallableUtil::VT<P5>::P i5,
	typename ZCallableUtil::VT<P6>::P i6,
	typename ZCallableUtil::VT<P7>::P i7,
	typename ZCallableUtil::VT<P8>::P i8,
	typename ZCallableUtil::VT<P9>::P i9,
	typename ZCallableUtil::VT<PA>::P iA,
	typename ZCallableUtil::VT<PB>::P iB,
	typename ZCallableUtil::VT<PC>::P iC,
	typename ZCallableUtil::VT<PD>::P iD)
	{
	if (iCallable)
		return iCallable->QCall(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD);
	return null;
	}

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB,
	class PC, class PD>
R sDCall
	(typename ZCallableUtil::VT<R>::P iDefault,
	const ZRef<ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD)> >& iCallable,
	typename ZCallableUtil::VT<P0>::P i0,
	typename ZCallableUtil::VT<P1>::P i1,
	typename ZCallableUtil::VT<P2>::P i2,
	typename ZCallableUtil::VT<P3>::P i3,
	typename ZCallableUtil::VT<P4>::P i4,
	typename ZCallableUtil::VT<P5>::P i5,
	typename ZCallableUtil::VT<P6>::P i6,
	typename ZCallableUtil::VT<P7>::P i7,
	typename ZCallableUtil::VT<P8>::P i8,
	typename ZCallableUtil::VT<P9>::P i9,
	typename ZCallableUtil::VT<PA>::P iA,
	typename ZCallableUtil::VT<PB>::P iB,
	typename ZCallableUtil::VT<PC>::P iC,
	typename ZCallableUtil::VT<PD>::P iD)
	{
	if (iCallable)
		{
		if (ZQ<R> theQ = iCallable->QCall(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD))
			return *theQ;
		}
	return iDefault;
	}

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB,
	class PC, class PD>
R sCall
	(const ZRef<ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD)> >& iCallable,
	typename ZCallableUtil::VT<P0>::P i0,
	typename ZCallableUtil::VT<P1>::P i1,
	typename ZCallableUtil::VT<P2>::P i2,
	typename ZCallableUtil::VT<P3>::P i3,
	typename ZCallableUtil::VT<P4>::P i4,
	typename ZCallableUtil::VT<P5>::P i5,
	typename ZCallableUtil::VT<P6>::P i6,
	typename ZCallableUtil::VT<P7>::P i7,
	typename ZCallableUtil::VT<P8>::P i8,
	typename ZCallableUtil::VT<P9>::P i9,
	typename ZCallableUtil::VT<PA>::P iA,
	typename ZCallableUtil::VT<PB>::P iB,
	typename ZCallableUtil::VT<PC>::P iC,
	typename ZCallableUtil::VT<PD>::P iD)
	{
	if (iCallable)
		{
		if (ZQ<R> theQ = iCallable->QCall(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD))
			return *theQ;
		}
	return R();
	}

// =================================================================================================
// MARK: - sCall variants (specialization for 15 params)

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB,
	class PC, class PD, class PE>
ZQ<R> sQCall
	(const ZRef<ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE)> >& iCallable,
	typename ZCallableUtil::VT<P0>::P i0,
	typename ZCallableUtil::VT<P1>::P i1,
	typename ZCallableUtil::VT<P2>::P i2,
	typename ZCallableUtil::VT<P3>::P i3,
	typename ZCallableUtil::VT<P4>::P i4,
	typename ZCallableUtil::VT<P5>::P i5,
	typename ZCallableUtil::VT<P6>::P i6,
	typename ZCallableUtil::VT<P7>::P i7,
	typename ZCallableUtil::VT<P8>::P i8,
	typename ZCallableUtil::VT<P9>::P i9,
	typename ZCallableUtil::VT<PA>::P iA,
	typename ZCallableUtil::VT<PB>::P iB,
	typename ZCallableUtil::VT<PC>::P iC,
	typename ZCallableUtil::VT<PD>::P iD,
	typename ZCallableUtil::VT<PE>::P iE)
	{
	if (iCallable)
		return iCallable->QCall(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD, iE);
	return null;
	}

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB,
	class PC, class PD, class PE>
R sDCall
	(typename ZCallableUtil::VT<R>::P iDefault,
	const ZRef<ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE)> >& iCallable,
	typename ZCallableUtil::VT<P0>::P i0,
	typename ZCallableUtil::VT<P1>::P i1,
	typename ZCallableUtil::VT<P2>::P i2,
	typename ZCallableUtil::VT<P3>::P i3,
	typename ZCallableUtil::VT<P4>::P i4,
	typename ZCallableUtil::VT<P5>::P i5,
	typename ZCallableUtil::VT<P6>::P i6,
	typename ZCallableUtil::VT<P7>::P i7,
	typename ZCallableUtil::VT<P8>::P i8,
	typename ZCallableUtil::VT<P9>::P i9,
	typename ZCallableUtil::VT<PA>::P iA,
	typename ZCallableUtil::VT<PB>::P iB,
	typename ZCallableUtil::VT<PC>::P iC,
	typename ZCallableUtil::VT<PD>::P iD,
	typename ZCallableUtil::VT<PE>::P iE)
	{
	if (iCallable)
		{
		if (ZQ<R> theQ =
			iCallable->QCall(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD, iE))
			{ return *theQ; }
		}
	return iDefault;
	}

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB,
	class PC, class PD, class PE>
R sCall
	(const ZRef<ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE)> >& iCallable,
	typename ZCallableUtil::VT<P0>::P i0,
	typename ZCallableUtil::VT<P1>::P i1,
	typename ZCallableUtil::VT<P2>::P i2,
	typename ZCallableUtil::VT<P3>::P i3,
	typename ZCallableUtil::VT<P4>::P i4,
	typename ZCallableUtil::VT<P5>::P i5,
	typename ZCallableUtil::VT<P6>::P i6,
	typename ZCallableUtil::VT<P7>::P i7,
	typename ZCallableUtil::VT<P8>::P i8,
	typename ZCallableUtil::VT<P9>::P i9,
	typename ZCallableUtil::VT<PA>::P iA,
	typename ZCallableUtil::VT<PB>::P iB,
	typename ZCallableUtil::VT<PC>::P iC,
	typename ZCallableUtil::VT<PD>::P iD,
	typename ZCallableUtil::VT<PE>::P iE)
	{
	if (iCallable)
		{
		if (ZQ<R> theQ =
			iCallable->QCall(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD, iE))
			{ return *theQ; }
		}
	return R();
	}

// =================================================================================================
// MARK: - sCall variants (specialization for 16 params)

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB,
	class PC, class PD, class PE, class PF>
ZQ<R> sQCall
	(const ZRef<ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE,PF)> >& iCallable,
	typename ZCallableUtil::VT<P0>::P i0,
	typename ZCallableUtil::VT<P1>::P i1,
	typename ZCallableUtil::VT<P2>::P i2,
	typename ZCallableUtil::VT<P3>::P i3,
	typename ZCallableUtil::VT<P4>::P i4,
	typename ZCallableUtil::VT<P5>::P i5,
	typename ZCallableUtil::VT<P6>::P i6,
	typename ZCallableUtil::VT<P7>::P i7,
	typename ZCallableUtil::VT<P8>::P i8,
	typename ZCallableUtil::VT<P9>::P i9,
	typename ZCallableUtil::VT<PA>::P iA,
	typename ZCallableUtil::VT<PB>::P iB,
	typename ZCallableUtil::VT<PC>::P iC,
	typename ZCallableUtil::VT<PD>::P iD,
	typename ZCallableUtil::VT<PE>::P iE,
	typename ZCallableUtil::VT<PF>::P iF)
	{
	if (iCallable)
		return iCallable->QCall(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD, iE, iF);
	return null;
	}

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB,
	class PC, class PD, class PE, class PF>
R sDCall
	(typename ZCallableUtil::VT<R>::P iDefault,
	const ZRef<ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE,PF)> >& iCallable,
	typename ZCallableUtil::VT<P0>::P i0,
	typename ZCallableUtil::VT<P1>::P i1,
	typename ZCallableUtil::VT<P2>::P i2,
	typename ZCallableUtil::VT<P3>::P i3,
	typename ZCallableUtil::VT<P4>::P i4,
	typename ZCallableUtil::VT<P5>::P i5,
	typename ZCallableUtil::VT<P6>::P i6,
	typename ZCallableUtil::VT<P7>::P i7,
	typename ZCallableUtil::VT<P8>::P i8,
	typename ZCallableUtil::VT<P9>::P i9,
	typename ZCallableUtil::VT<PA>::P iA,
	typename ZCallableUtil::VT<PB>::P iB,
	typename ZCallableUtil::VT<PC>::P iC,
	typename ZCallableUtil::VT<PD>::P iD,
	typename ZCallableUtil::VT<PE>::P iE,
	typename ZCallableUtil::VT<PF>::P iF)
	{
	if (iCallable)
		{
		if (ZQ<R> theQ =
			iCallable->QCall(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD, iE, iF))
			{ return *theQ; }
		}
	return iDefault;
	}

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB,
	class PC, class PD, class PE, class PF>
R sCall
	(const ZRef<ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE,PF)> >& iCallable,
	typename ZCallableUtil::VT<P0>::P i0,
	typename ZCallableUtil::VT<P1>::P i1,
	typename ZCallableUtil::VT<P2>::P i2,
	typename ZCallableUtil::VT<P3>::P i3,
	typename ZCallableUtil::VT<P4>::P i4,
	typename ZCallableUtil::VT<P5>::P i5,
	typename ZCallableUtil::VT<P6>::P i6,
	typename ZCallableUtil::VT<P7>::P i7,
	typename ZCallableUtil::VT<P8>::P i8,
	typename ZCallableUtil::VT<P9>::P i9,
	typename ZCallableUtil::VT<PA>::P iA,
	typename ZCallableUtil::VT<PB>::P iB,
	typename ZCallableUtil::VT<PC>::P iC,
	typename ZCallableUtil::VT<PD>::P iD,
	typename ZCallableUtil::VT<PE>::P iE,
	typename ZCallableUtil::VT<PF>::P iF)
	{
	if (iCallable)
		{
		if (ZQ<R> theQ =
			iCallable->QCall(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD, iE, iF))
			{ return *theQ; }
		}
	return R();
	}

// =================================================================================================
// MARK: - sCallable

template <class Signature>
ZRef<ZCallable<Signature> > sCallable(const ZRef<ZCallable<Signature> >& iCallable)
	{ return iCallable; }

// =================================================================================================
// MARK: - sCallReturnVoid

template <class T>
void sCallReturnVoid(ZRef<ZCallable<T(void)> > iCallable)
	{ sCall(iCallable); }

// =================================================================================================
// MARK: - sCallVoid

inline
void sCallVoid(ZRef<ZCallable<void(void)> > iCallable)
	{ sCall(iCallable); }

// =================================================================================================
// MARK: - Useful typedefs

typedef ZCallable<void(void)> ZCallable_Void;
typedef ZRef<ZCallable_Void> ZRef_ZCallable_Void;

typedef ZCallable<bool(void)> ZCallable_Bool;

// =================================================================================================
// MARK: - ZCallable_Null

class ZCallable_Null
:	public ZCallable_Void
	{
public:
// From ZCallable
	virtual ZQ<void> QCall()
		{ return notnull; }
	};

} // namespace ZooLib

#endif // __ZCallable_h__
