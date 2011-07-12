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

#ifndef __ZCallable_Function__
#define __ZCallable_Function__ 1
#include "zconfig.h"

#include "zoolib/ZCallable.h"

namespace ZooLib {
namespace ZCallable_Function {

// =================================================================================================
#pragma mark -
#pragma mark * Callable

template <class Signature> class Callable;

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 0 params)

template <class R>
class Callable<R(void)>
:	public ZCallable<R(void)>
	{
public:
	typedef R (*FunctionPtr_t)();

	Callable(FunctionPtr_t iFunctionPtr)
	:	fFunctionPtr(iFunctionPtr)
		{}

// From ZCallable
	virtual ZQ<R> QCall()
		{
		if (fFunctionPtr)
			return fFunctionPtr();
		return null;
		}

private:
	FunctionPtr_t fFunctionPtr;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 0 params, void return)

template <>
class Callable<void(void)>
:	public ZCallable<void(void)>
	{
public:
	typedef void (*FunctionPtr_t)();

	Callable(FunctionPtr_t iFunctionPtr)
	:	fFunctionPtr(iFunctionPtr)
		{}

// From ZCallable
	virtual ZQ<void> QCall()
		{
		if (fFunctionPtr)
			{
			fFunctionPtr();
			return true;
			}
		return null;
		}

private:
	FunctionPtr_t fFunctionPtr;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 1 param)

template <class R,
	class P0>
class Callable<R(P0)>
:	public ZCallable<R(P0)>
	{
public:
	typedef R (*FunctionPtr_t)(P0);

	Callable(FunctionPtr_t iFunctionPtr)
	:	fFunctionPtr(iFunctionPtr)
		{}

// From ZCallable
	virtual ZQ<R> QCall(P0 i0)
		{
		if (fFunctionPtr)
			return fFunctionPtr(i0);
		return null;
		}

private:
	FunctionPtr_t fFunctionPtr;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 1 param, void return)

template <
	class P0>
class Callable<void(P0)>
:	public ZCallable<void(P0)>
	{
public:
	typedef void (*FunctionPtr_t)(P0);

	Callable(FunctionPtr_t iFunctionPtr)
	:	fFunctionPtr(iFunctionPtr)
		{}

// From ZCallable
	virtual ZQ<void> QCall(P0 i0)
		{
		if (fFunctionPtr)
			{
			fFunctionPtr(i0);
			return true;
			}
		return null;
		}

private:
	FunctionPtr_t fFunctionPtr;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 2 params)

template <class R,
	class P0, class P1>
class Callable<R(P0,P1)>
:	public ZCallable<R(P0,P1)>
	{
public:
	typedef R (*FunctionPtr_t)(P0,P1);

	Callable(FunctionPtr_t iFunctionPtr)
	:	fFunctionPtr(iFunctionPtr)
		{}

// From ZCallable
	virtual ZQ<R> QCall(P0 i0, P1 i1)
		{
		if (fFunctionPtr)
			return fFunctionPtr(i0, i1);
		return null;
		}

private:
	FunctionPtr_t fFunctionPtr;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 2 params, void return)

template <
	class P0, class P1>
class Callable<void(P0,P1)>
:	public ZCallable<void(P0,P1)>
	{
public:
	typedef void (*FunctionPtr_t)(P0,P1);

	Callable(FunctionPtr_t iFunctionPtr)
	:	fFunctionPtr(iFunctionPtr)
		{}

// From ZCallable
	virtual ZQ<void> QCall(P0 i0, P1 i1)
		{
		if (fFunctionPtr)
			{
			fFunctionPtr(i0, i1);
			return true;
			}
		return null;
		}

private:
	FunctionPtr_t fFunctionPtr;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 3 params)

template <class R,
	class P0, class P1, class P2>
class Callable<R(P0,P1,P2)>
:	public ZCallable<R(P0,P1,P2)>
	{
public:
	typedef R (*FunctionPtr_t)(P0,P1,P2);

	Callable(FunctionPtr_t iFunctionPtr)
	:	fFunctionPtr(iFunctionPtr)
		{}

// From ZCallable
	virtual ZQ<R> QCall(P0 i0, P1 i1, P2 i2)
		{
		if (fFunctionPtr)
			return fFunctionPtr(i0, i1, i2);
		return null;
		}

private:
	FunctionPtr_t fFunctionPtr;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 3 params, void return)

template <
	class P0, class P1, class P2>
class Callable<void(P0,P1,P2)>
:	public ZCallable<void(P0,P1,P2)>
	{
public:
	typedef void (*FunctionPtr_t)(P0,P1,P2);

	Callable(FunctionPtr_t iFunctionPtr)
	:	fFunctionPtr(iFunctionPtr)
		{}

// From ZCallable
	virtual ZQ<void> QCall(P0 i0, P1 i1, P2 i2)
		{
		if (fFunctionPtr)
			{
			fFunctionPtr(i0, i1, i2);
			return true;
			}
		return null;
		}

private:
	FunctionPtr_t fFunctionPtr;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 4 params)

template <class R,
	class P0, class P1, class P2, class P3>
class Callable<R(P0,P1,P2,P3)>
:	public ZCallable<R(P0,P1,P2,P3)>
	{
public:
	typedef R (*FunctionPtr_t)(P0,P1,P2,P3);

	Callable(FunctionPtr_t iFunctionPtr)
	:	fFunctionPtr(iFunctionPtr)
		{}

// From ZCallable
	virtual ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3)
		{
		if (fFunctionPtr)
			return fFunctionPtr(i0, i1, i2, i3);
		return null;
		}

private:
	FunctionPtr_t fFunctionPtr;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 4 params, void return)

template <
	class P0, class P1, class P2, class P3>
class Callable<void(P0,P1,P2,P3)>
:	public ZCallable<void(P0,P1,P2,P3)>
	{
public:
	typedef void (*FunctionPtr_t)(P0,P1,P2,P3);

	Callable(FunctionPtr_t iFunctionPtr)
	:	fFunctionPtr(iFunctionPtr)
		{}

// From ZCallable
	virtual ZQ<void> QCall(P0 i0, P1 i1, P2 i2, P3 i3)
		{
		if (fFunctionPtr)
			{
			fFunctionPtr(i0, i1, i2, i3);
			return true;
			}
		return null;
		}

private:
	FunctionPtr_t fFunctionPtr;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 5 params)

template <class R,
	class P0, class P1, class P2, class P3,
	class P4>
class Callable<R(P0,P1,P2,P3,P4)>
:	public ZCallable<R(P0,P1,P2,P3,P4)>
	{
public:
	typedef R (*FunctionPtr_t)(P0,P1,P2,P3,P4);

	Callable(FunctionPtr_t iFunctionPtr)
	:	fFunctionPtr(iFunctionPtr)
		{}

// From ZCallable
	virtual ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4)
		{
		if (fFunctionPtr)
			return fFunctionPtr(i0, i1, i2, i3, i4);
		return null;
		}

private:
	FunctionPtr_t fFunctionPtr;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 5 params, void return)

template <
	class P0, class P1, class P2, class P3,
	class P4>
class Callable<void(P0,P1,P2,P3,P4)>
:	public ZCallable<void(P0,P1,P2,P3,P4)>
	{
public:
	typedef void (*FunctionPtr_t)(P0,P1,P2,P3,P4);

	Callable(FunctionPtr_t iFunctionPtr)
	:	fFunctionPtr(iFunctionPtr)
		{}

// From ZCallable
	virtual ZQ<void> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4)
		{
		if (fFunctionPtr)
			{
			fFunctionPtr(i0, i1, i2, i3, i4);
			return true;
			}
		return null;
		}

private:
	FunctionPtr_t fFunctionPtr;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 6 params)

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5>
class Callable<R(P0,P1,P2,P3,P4,P5)>
:	public ZCallable<R(P0,P1,P2,P3,P4,P5)>
	{
public:
	typedef R (*FunctionPtr_t)(P0,P1,P2,P3,P4,P5);

	Callable(FunctionPtr_t iFunctionPtr)
	:	fFunctionPtr(iFunctionPtr)
		{}

// From ZCallable
	virtual ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5)
		{
		if (fFunctionPtr)
			return fFunctionPtr(i0, i1, i2, i3, i4, i5);
		return null;
		}

private:
	FunctionPtr_t fFunctionPtr;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 6 params, void return)

template <
	class P0, class P1, class P2, class P3,
	class P4, class P5>
class Callable<void(P0,P1,P2,P3,P4,P5)>
:	public ZCallable<void(P0,P1,P2,P3,P4,P5)>
	{
public:
	typedef void (*FunctionPtr_t)(P0,P1,P2,P3,P4,P5);

	Callable(FunctionPtr_t iFunctionPtr)
	:	fFunctionPtr(iFunctionPtr)
		{}

// From ZCallable
	virtual ZQ<void> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5)
		{
		if (fFunctionPtr)
			{
			fFunctionPtr(i0, i1, i2, i3, i4, i5);
			return true;
			}
		return null;
		}

private:
	FunctionPtr_t fFunctionPtr;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 7 params)

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6>
class Callable<R(P0,P1,P2,P3,P4,P5,P6)>
:	public ZCallable<R(P0,P1,P2,P3,P4,P5,P6)>
	{
public:
	typedef R (*FunctionPtr_t)(P0,P1,P2,P3,P4,P5,P6);

	Callable(FunctionPtr_t iFunctionPtr)
	:	fFunctionPtr(iFunctionPtr)
		{}

// From ZCallable
	virtual ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6)
		{
		if (fFunctionPtr)
			return fFunctionPtr(i0, i1, i2, i3, i4, i5, i6);
		return null;
		}

private:
	FunctionPtr_t fFunctionPtr;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 7 params, void return)

template <
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6>
class Callable<void(P0,P1,P2,P3,P4,P5,P6)>
:	public ZCallable<void(P0,P1,P2,P3,P4,P5,P6)>
	{
public:
	typedef void (*FunctionPtr_t)(P0,P1,P2,P3,P4,P5,P6);

	Callable(FunctionPtr_t iFunctionPtr)
	:	fFunctionPtr(iFunctionPtr)
		{}

// From ZCallable
	virtual ZQ<void> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6)
		{
		if (fFunctionPtr)
			{
			fFunctionPtr(i0, i1, i2, i3, i4, i5, i6);
			return true;
			}
		return null;
		}

private:
	FunctionPtr_t fFunctionPtr;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 8 params)

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7>
class Callable<R(P0,P1,P2,P3,P4,P5,P6,P7)>
:	public ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7)>
	{
public:
	typedef R (*FunctionPtr_t)(P0,P1,P2,P3,P4,P5,P6,P7);

	Callable(FunctionPtr_t iFunctionPtr)
	:	fFunctionPtr(iFunctionPtr)
		{}

// From ZCallable
	virtual ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7)
		{
		if (fFunctionPtr)
			return fFunctionPtr(i0, i1, i2, i3, i4, i5, i6, i7);
		return null;
		}

private:
	FunctionPtr_t fFunctionPtr;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 8 params, void return)

template <
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7>
class Callable<void(P0,P1,P2,P3,P4,P5,P6,P7)>
:	public ZCallable<void(P0,P1,P2,P3,P4,P5,P6,P7)>
	{
public:
	typedef void (*FunctionPtr_t)(P0,P1,P2,P3,P4,P5,P6,P7);

	Callable(FunctionPtr_t iFunctionPtr)
	:	fFunctionPtr(iFunctionPtr)
		{}

// From ZCallable
	virtual ZQ<void> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7)
		{
		if (fFunctionPtr)
			{
			fFunctionPtr(i0, i1, i2, i3, i4, i5, i6, i7);
			return true;
			}
		return null;
		}

private:
	FunctionPtr_t fFunctionPtr;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 9 params)

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8>
class Callable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8)>
:	public ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8)>
	{
public:
	typedef R (*FunctionPtr_t)(P0,P1,P2,P3,P4,P5,P6,P7,P8);

	Callable(FunctionPtr_t iFunctionPtr)
	:	fFunctionPtr(iFunctionPtr)
		{}

// From ZCallable
	virtual ZQ<R> QCall
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8)
		{
		if (fFunctionPtr)
			return fFunctionPtr(i0, i1, i2, i3, i4, i5, i6, i7, i8);
		return null;
		}

private:
	FunctionPtr_t fFunctionPtr;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 9 params, void return)

template <
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8>
class Callable<void(P0,P1,P2,P3,P4,P5,P6,P7,P8)>
:	public ZCallable<void(P0,P1,P2,P3,P4,P5,P6,P7,P8)>
	{
public:
	typedef void (*FunctionPtr_t)(P0,P1,P2,P3,P4,P5,P6,P7,P8);

	Callable(FunctionPtr_t iFunctionPtr)
	:	fFunctionPtr(iFunctionPtr)
		{}

// From ZCallable
	virtual ZQ<void> QCall
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8)
		{
		if (fFunctionPtr)
			{
			fFunctionPtr(i0, i1, i2, i3, i4, i5, i6, i7, i8);
			return true;
			}
		return null;
		}

private:
	FunctionPtr_t fFunctionPtr;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 10 params)

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9>
class Callable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9)>
:	public ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9)>
	{
public:
	typedef R (*FunctionPtr_t)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9);

	Callable(FunctionPtr_t iFunctionPtr)
	:	fFunctionPtr(iFunctionPtr)
		{}

// From ZCallable
	virtual ZQ<R> QCall
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9)
		{
		if (fFunctionPtr)
			return fFunctionPtr(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9);
		return null;
		}

private:
	FunctionPtr_t fFunctionPtr;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 10 params, void return)

template <
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9>
class Callable<void(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9)>
:	public ZCallable<void(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9)>
	{
public:
	typedef void (*FunctionPtr_t)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9);

	Callable(FunctionPtr_t iFunctionPtr)
	:	fFunctionPtr(iFunctionPtr)
		{}

// From ZCallable
	virtual ZQ<void> QCall
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9)
		{
		if (fFunctionPtr)
			{
			fFunctionPtr(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9);
			return true;
			}
		return null;
		}

private:
	FunctionPtr_t fFunctionPtr;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 11 params)

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA>
class Callable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA)>
:	public ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA)>
	{
public:
	typedef R (*FunctionPtr_t)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA);

	Callable(FunctionPtr_t iFunctionPtr)
	:	fFunctionPtr(iFunctionPtr)
		{}

// From ZCallable
	virtual ZQ<R> QCall
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA)
		{
		if (fFunctionPtr)
			return fFunctionPtr(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA);
		return null;
		}

private:
	FunctionPtr_t fFunctionPtr;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 11 params, void return)

template <
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA>
class Callable<void(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA)>
:	public ZCallable<void(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA)>
	{
public:
	typedef void (*FunctionPtr_t)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA);

	Callable(FunctionPtr_t iFunctionPtr)
	:	fFunctionPtr(iFunctionPtr)
		{}

// From ZCallable
	virtual ZQ<void> QCall
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA)
		{
		if (fFunctionPtr)
			{
			fFunctionPtr(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA);
			return true;
			}
		return null;
		}

private:
	FunctionPtr_t fFunctionPtr;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 12 params)

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB>
class Callable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB)>
:	public ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB)>
	{
public:
	typedef R (*FunctionPtr_t)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB);

	Callable(FunctionPtr_t iFunctionPtr)
	:	fFunctionPtr(iFunctionPtr)
		{}

// From ZCallable
	virtual ZQ<R> QCall
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB)
		{
		if (fFunctionPtr)
			return fFunctionPtr(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB);
		return null;
		}

private:
	FunctionPtr_t fFunctionPtr;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 12 params, void return)

template <
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB>
class Callable<void(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB)>
:	public ZCallable<void(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB)>
	{
public:
	typedef void (*FunctionPtr_t)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB);

	Callable(FunctionPtr_t iFunctionPtr)
	:	fFunctionPtr(iFunctionPtr)
		{}

// From ZCallable
	virtual ZQ<void> QCall
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB)
		{
		if (fFunctionPtr)
			{
			fFunctionPtr(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB);
			return true;
			}
		return null;
		}

private:
	FunctionPtr_t fFunctionPtr;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 13 params)

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB,
	class PC>
class Callable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC)>
:	public ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC)>
	{
public:
	typedef R (*FunctionPtr_t)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC);

	Callable(FunctionPtr_t iFunctionPtr)
	:	fFunctionPtr(iFunctionPtr)
		{}

// From ZCallable
	virtual ZQ<R> QCall
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB, PC iC)
		{
		if (fFunctionPtr)
			return fFunctionPtr(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC);
		return null;
		}

private:
	FunctionPtr_t fFunctionPtr;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 13 params, void return)

template <
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB,
	class PC>
class Callable<void(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC)>
:	public ZCallable<void(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC)>
	{
public:
	typedef void (*FunctionPtr_t)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC);

	Callable(FunctionPtr_t iFunctionPtr)
	:	fFunctionPtr(iFunctionPtr)
		{}

// From ZCallable
	virtual ZQ<void> QCall
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB, PC iC)
		{
		if (fFunctionPtr)
			{
			fFunctionPtr(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC);
			return true;
			}
		return null;
		}

private:
	FunctionPtr_t fFunctionPtr;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 14 params)

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB,
	class PC, class PD>
class Callable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD)>
:	public ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD)>
	{
public:
	typedef R (*FunctionPtr_t)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD);

	Callable(FunctionPtr_t iFunctionPtr)
	:	fFunctionPtr(iFunctionPtr)
		{}

// From ZCallable
	virtual ZQ<R> QCall
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB, PC iC, PD iD)
		{
		if (fFunctionPtr)
			return fFunctionPtr(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD);
		return null;
		}

private:
	FunctionPtr_t fFunctionPtr;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 14 params, void return)

template <
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB,
	class PC, class PD>
class Callable<void(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD)>
:	public ZCallable<void(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD)>
	{
public:
	typedef void (*FunctionPtr_t)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD);

	Callable(FunctionPtr_t iFunctionPtr)
	:	fFunctionPtr(iFunctionPtr)
		{}

// From ZCallable
	virtual ZQ<void> QCall
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB, PC iC, PD iD)
		{
		if (fFunctionPtr)
			{
			fFunctionPtr(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD);
			return true;
			}
		return null;
		}

private:
	FunctionPtr_t fFunctionPtr;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 15 params)

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB,
	class PC, class PD, class PE>
class Callable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE)>
:	public ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE)>
	{
public:
	typedef R (*FunctionPtr_t)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE);

	Callable(FunctionPtr_t iFunctionPtr)
	:	fFunctionPtr(iFunctionPtr)
		{}

// From ZCallable
	virtual ZQ<R> QCall
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB, PC iC, PD iD, PE iE)
		{
		if (fFunctionPtr)
			return fFunctionPtr(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD, iE);
		return null;
		}

private:
	FunctionPtr_t fFunctionPtr;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 15 params, void return)

template <
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB,
	class PC, class PD, class PE>
class Callable<void(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE)>
:	public ZCallable<void(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE)>
	{
public:
	typedef void (*FunctionPtr_t)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE);

	Callable(FunctionPtr_t iFunctionPtr)
	:	fFunctionPtr(iFunctionPtr)
		{}

// From ZCallable
	virtual ZQ<void> QCall
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB, PC iC, PD iD, PE iE)
		{
		if (fFunctionPtr)
			{
			fFunctionPtr(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD, iE);
			return true;
			}
		return null;
		}

private:
	FunctionPtr_t fFunctionPtr;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 16 params)

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB,
	class PC, class PD, class PE, class PF>
class Callable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE,PF)>
:	public ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE,PF)>
	{
public:
	typedef R (*FunctionPtr_t)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE,PF);

	Callable(FunctionPtr_t iFunctionPtr)
	:	fFunctionPtr(iFunctionPtr)
		{}

// From ZCallable
	virtual ZQ<R> QCall
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB, PC iC, PD iD, PE iE, PF iF)
		{
		if (fFunctionPtr)
			return fFunctionPtr(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD, iE, iF);
		return null;
		}

private:
	FunctionPtr_t fFunctionPtr;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 16 params, void return)

template <
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB,
	class PC, class PD, class PE, class PF>
class Callable<void(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE,PF)>
:	public ZCallable<void(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE,PF)>
	{
public:
	typedef void (*FunctionPtr_t)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE,PF);

	Callable(FunctionPtr_t iFunctionPtr)
	:	fFunctionPtr(iFunctionPtr)
		{}

// From ZCallable
	virtual ZQ<void> QCall
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB, PC iC, PD iD, PE iE, PF iF)
		{
		if (fFunctionPtr)
			{
			fFunctionPtr(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD, iE, iF);
			return true;
			}
		return null;
		}

private:
	FunctionPtr_t fFunctionPtr;
	};

// =================================================================================================
#pragma mark -
#pragma mark * sCallable

template <class R>
ZRef<ZCallable<R(void)> >
sCallable(R (*iFunctionPtr)())
	{ return new Callable<R(void)>(iFunctionPtr); }

template <class R,
	class P0>
ZRef<ZCallable<R(P0)> >
sCallable(R (*iFunctionPtr)(P0))
	{ return new Callable<R(P0)>(iFunctionPtr); }

template <class R,
	class P0, class P1>
ZRef<ZCallable<R(P0,P1)> >
sCallable(R (*iFunctionPtr)(P0,P1))
	{ return new Callable<R(P0,P1)>(iFunctionPtr); }

template <class R,
	class P0, class P1, class P2>
ZRef<ZCallable<R(P0,P1,P2)> >
sCallable(R (*iFunctionPtr)(P0,P1,P2))
	{ return new Callable<R(P0,P1,P2)>(iFunctionPtr); }

template <class R,
	class P0, class P1, class P2, class P3>
ZRef<ZCallable<R(P0,P1,P2,P3)> >
sCallable(R (*iFunctionPtr)(P0,P1,P2,P3))
	{ return new Callable<R(P0,P1,P2,P3)>(iFunctionPtr); }

template <class R,
	class P0, class P1, class P2, class P3,
	class P4>
ZRef<ZCallable<R(P0,P1,P2,P3,P4)> >
sCallable(R (*iFunctionPtr)(P0,P1,P2,P3,P4))
	{ return new Callable<R(P0,P1,P2,P3,P4)>(iFunctionPtr); }

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5>
ZRef<ZCallable<R(P0,P1,P2,P3,P4,P5)> >
sCallable(R (*iFunctionPtr)(P0,P1,P2,P3,P4,P5))
	{ return new Callable<R(P0,P1,P2,P3,P4,P5)>(iFunctionPtr); }

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6>
ZRef<ZCallable<R(P0,P1,P2,P3,P4,P5,P6)> >
sCallable(R (*iFunctionPtr)(P0,P1,P2,P3,P4,P5,P6))
	{ return new Callable<R(P0,P1,P2,P3,P4,P5,P6)>(iFunctionPtr); }

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7>
ZRef<ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7)> >
sCallable(R (*iFunctionPtr)(P0,P1,P2,P3,P4,P5,P6,P7))
	{ return new Callable<R(P0,P1,P2,P3,P4,P5,P6,P7)>(iFunctionPtr); }

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8>
ZRef<ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8)> >
sCallable(R (*iFunctionPtr)(P0,P1,P2,P3,P4,P5,P6,P7,P8))
	{ return new Callable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8)>(iFunctionPtr); }

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9>
ZRef<ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9)> >
sCallable(R (*iFunctionPtr)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9))
	{ return new Callable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9)>(iFunctionPtr); }

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA>
ZRef<ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA)> >
sCallable(R (*iFunctionPtr)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA))
	{ return new Callable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA)>(iFunctionPtr); }

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB>
ZRef<ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB)> >
sCallable(R (*iFunctionPtr)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB))
	{ return new Callable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB)>(iFunctionPtr); }

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB,
	class PC>
ZRef<ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC)> >
sCallable(R (*iFunctionPtr)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC))
	{ return new Callable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC)>(iFunctionPtr); }

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB,
	class PC, class PD>
ZRef<ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD)> >
sCallable(R (*iFunctionPtr)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD))
	{ return new Callable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD)>(iFunctionPtr); }

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB,
	class PC, class PD, class PE>
ZRef<ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE)> >
sCallable(R (*iFunctionPtr)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE))
	{ return new Callable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE)>(iFunctionPtr); }

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB,
	class PC, class PD, class PE, class PF>
ZRef<ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE,PF)> >
sCallable(R (*iFunctionPtr)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE,PF))
	{ return new Callable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE,PF)>(iFunctionPtr); }

} // namespace ZCallable_Function

// =================================================================================================
#pragma mark -
#pragma mark * sCallable

using ZCallable_Function::sCallable;

} // namespace ZooLib

#endif // __ZCallable_Function__
