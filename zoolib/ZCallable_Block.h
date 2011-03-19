/* -------------------------------------------------------------------------------------------------
Copyright (c) 2011 Andrew Green
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

#ifndef __ZCallable_Block__
#define __ZCallable_Block__ 1
#include "zconfig.h"

#if defined(__BLOCKS__)
#include "zoolib/ZCallable.h"

#include <Block.h>

namespace ZooLib {

namespace ZCallable_Block {

// =================================================================================================
#pragma mark -
#pragma mark * Callable

template <class Signature> class Callable;

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 0 params)

template <class R>
class Callable<R()>
:	public ZCallable<R()>
	{
public:
	typedef R (^BlockPtr_t)();

	Callable(BlockPtr_t iBlockPtr)
	:	fBlockPtr(Block_copy(iBlockPtr))
		{}

	virtual ~Callable()
		{ Block_release(fBlockPtr); }

	// From ZCallable
	virtual R Call()
		{ return fBlockPtr(); }

private:
	BlockPtr_t fBlockPtr;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 0 params, void return)

template <>
class Callable<void()>
:	public ZCallable<void()>
	{
public:
	typedef void (^BlockPtr_t)();

	Callable(BlockPtr_t iBlockPtr)
	:	fBlockPtr(Block_copy(iBlockPtr))
		{}

	virtual ~Callable()
		{ Block_release(fBlockPtr); }

	// From ZCallable
	virtual void Call()
		{ fBlockPtr(); }

private:
	BlockPtr_t fBlockPtr;
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
	typedef R (^BlockPtr_t)(P0);

	Callable(BlockPtr_t iBlockPtr)
	:	fBlockPtr(Block_copy(iBlockPtr))
		{}

	virtual ~Callable()
		{ Block_release(fBlockPtr); }

	// From ZCallable
	virtual R Call(P0 i0)
		{ return fBlockPtr(i0); }

private:
	BlockPtr_t fBlockPtr;
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
	typedef void (^BlockPtr_t)(P0);

	Callable(BlockPtr_t iBlockPtr)
	:	fBlockPtr(Block_copy(iBlockPtr))
		{}

	virtual ~Callable()
		{ Block_release(fBlockPtr); }

	// From ZCallable
	virtual void Call(P0 i0)
		{ fBlockPtr(i0); }

private:
	BlockPtr_t fBlockPtr;
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
	typedef R (^BlockPtr_t)(P0,P1);

	Callable(BlockPtr_t iBlockPtr)
	:	fBlockPtr(Block_copy(iBlockPtr))
		{}

	virtual ~Callable()
		{ Block_release(fBlockPtr); }

	// From ZCallable
	virtual R Call(P0 i0, P1 i1)
		{ return fBlockPtr(i0, i1); }

private:
	BlockPtr_t fBlockPtr;
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
	typedef void (^BlockPtr_t)(P0,P1);

	Callable(BlockPtr_t iBlockPtr)
	:	fBlockPtr(Block_copy(iBlockPtr))
		{}

	virtual ~Callable()
		{ Block_release(fBlockPtr); }

	// From ZCallable
	virtual void Call(P0 i0, P1 i1)
		{ fBlockPtr(i0, i1); }

private:
	BlockPtr_t fBlockPtr;
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
	typedef R (^BlockPtr_t)(P0,P1,P2);

	Callable(BlockPtr_t iBlockPtr)
	:	fBlockPtr(Block_copy(iBlockPtr))
		{}

	virtual ~Callable()
		{ Block_release(fBlockPtr); }

	// From ZCallable
	virtual R Call(P0 i0, P1 i1, P2 i2)
		{ return fBlockPtr(i0, i1, i2); }

private:
	BlockPtr_t fBlockPtr;
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
	typedef void (^BlockPtr_t)(P0,P1,P2);

	Callable(BlockPtr_t iBlockPtr)
	:	fBlockPtr(Block_copy(iBlockPtr))
		{}

	virtual ~Callable()
		{ Block_release(fBlockPtr); }

	// From ZCallable
	virtual void Call(P0 i0, P1 i1, P2 i2)
		{ fBlockPtr(i0, i1, i2); }

private:
	BlockPtr_t fBlockPtr;
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
	typedef R (^BlockPtr_t)(P0,P1,P2,P3);

	Callable(BlockPtr_t iBlockPtr)
	:	fBlockPtr(Block_copy(iBlockPtr))
		{}

	virtual ~Callable()
		{ Block_release(fBlockPtr); }

	// From ZCallable
	virtual R Call(P0 i0, P1 i1, P2 i2, P3 i3)
		{ return fBlockPtr(i0, i1, i2, i3); }

private:
	BlockPtr_t fBlockPtr;
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
	typedef void (^BlockPtr_t)(P0,P1,P2,P3);

	Callable(BlockPtr_t iBlockPtr)
	:	fBlockPtr(Block_copy(iBlockPtr))
		{}

	virtual ~Callable()
		{ Block_release(fBlockPtr); }

	// From ZCallable
	virtual void Call(P0 i0, P1 i1, P2 i2, P3 i3)
		{ fBlockPtr(i0, i1, i2, i3); }

private:
	BlockPtr_t fBlockPtr;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 5 params)

template <class R,
	class P0, class P1, class P2, class P3, class P4>
class Callable<R(P0,P1,P2,P3,P4)>
:	public ZCallable<R(P0,P1,P2,P3,P4)>
	{
public:
	typedef R (^BlockPtr_t)(P0,P1,P2,P3,P4);

	Callable(BlockPtr_t iBlockPtr)
	:	fBlockPtr(Block_copy(iBlockPtr))
		{}

	virtual ~Callable()
		{ Block_release(fBlockPtr); }

	// From ZCallable
	virtual R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4)
		{ return fBlockPtr(i0, i1, i2, i3, i4); }

private:
	BlockPtr_t fBlockPtr;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 5 params, void return)

template <
	class P0, class P1, class P2, class P3, class P4>
class Callable<void(P0,P1,P2,P3,P4)>
:	public ZCallable<void(P0,P1,P2,P3,P4)>
	{
public:
	typedef void (^BlockPtr_t)(P0,P1,P2,P3,P4);

	Callable(BlockPtr_t iBlockPtr)
	:	fBlockPtr(Block_copy(iBlockPtr))
		{}

	virtual ~Callable()
		{ Block_release(fBlockPtr); }

	// From ZCallable
	virtual void Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4)
		{ fBlockPtr(i0, i1, i2, i3, i4); }

private:
	BlockPtr_t fBlockPtr;
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
	typedef R (^BlockPtr_t)(P0,P1,P2,P3,P4,P5);

	Callable(BlockPtr_t iBlockPtr)
	:	fBlockPtr(Block_copy(iBlockPtr))
		{}

	virtual ~Callable()
		{ Block_release(fBlockPtr); }

	// From ZCallable
	virtual R Call
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5)
		{ return fBlockPtr(i0, i1, i2, i3, i4, i5); }

private:
	BlockPtr_t fBlockPtr;
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
	typedef void (^BlockPtr_t)(P0,P1,P2,P3,P4,P5);

	Callable(BlockPtr_t iBlockPtr)
	:	fBlockPtr(Block_copy(iBlockPtr))
		{}

	virtual ~Callable()
		{ Block_release(fBlockPtr); }

	// From ZCallable
	virtual void Call
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5)
		{ fBlockPtr(i0, i1, i2, i3, i4, i5); }

private:
	BlockPtr_t fBlockPtr;
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
	typedef R (^BlockPtr_t)(P0,P1,P2,P3,P4,P5,P6);

	Callable(BlockPtr_t iBlockPtr)
	:	fBlockPtr(Block_copy(iBlockPtr))
		{}

	virtual ~Callable()
		{ Block_release(fBlockPtr); }

	// From ZCallable
	virtual R Call
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6)
		{ return fBlockPtr(i0, i1, i2, i3, i4, i5, i6); }

private:
	BlockPtr_t fBlockPtr;
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
	typedef void (^BlockPtr_t)(P0,P1,P2,P3,P4,P5,P6);

	Callable(BlockPtr_t iBlockPtr)
	:	fBlockPtr(Block_copy(iBlockPtr))
		{}

	virtual ~Callable()
		{ Block_release(fBlockPtr); }

	// From ZCallable
	virtual void Call
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6)
		{ fBlockPtr(i0, i1, i2, i3, i4, i5, i6); }

private:
	BlockPtr_t fBlockPtr;
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
	typedef R (^BlockPtr_t)(P0,P1,P2,P3,P4,P5,P6,P7);

	Callable(BlockPtr_t iBlockPtr)
	:	fBlockPtr(Block_copy(iBlockPtr))
		{}

	virtual ~Callable()
		{ Block_release(fBlockPtr); }

	// From ZCallable
	virtual R Call
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7)
		{ return fBlockPtr(i0, i1, i2, i3, i4, i5, i6, i7); }

private:
	BlockPtr_t fBlockPtr;
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
	typedef void (^BlockPtr_t)(P0,P1,P2,P3,P4,P5,P6,P7);

	Callable(BlockPtr_t iBlockPtr)
	:	fBlockPtr(Block_copy(iBlockPtr))
		{}

	virtual ~Callable()
		{ Block_release(fBlockPtr); }

	// From ZCallable
	virtual void Call
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7)
		{ fBlockPtr(i0, i1, i2, i3, i4, i5, i6, i7); }

private:
	BlockPtr_t fBlockPtr;
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
	typedef R (^BlockPtr_t)(P0,P1,P2,P3,P4,P5,P6,P7,P8);

	Callable(BlockPtr_t iBlockPtr)
	:	fBlockPtr(Block_copy(iBlockPtr))
		{}

	virtual ~Callable()
		{ Block_release(fBlockPtr); }

	// From ZCallable
	virtual R Call
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8)
		{ return fBlockPtr(i0, i1, i2, i3, i4, i5, i6, i7, i8); }

private:
	BlockPtr_t fBlockPtr;
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
	typedef void (^BlockPtr_t)(P0,P1,P2,P3,P4,P5,P6,P7,P8);

	Callable(BlockPtr_t iBlockPtr)
	:	fBlockPtr(Block_copy(iBlockPtr))
		{}

	virtual ~Callable()
		{ Block_release(fBlockPtr); }

	// From ZCallable
	virtual void Call
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8)
		{ fBlockPtr(i0, i1, i2, i3, i4, i5, i6, i7, i8); }

private:
	BlockPtr_t fBlockPtr;
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
	typedef R (^BlockPtr_t)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9);

	Callable(BlockPtr_t iBlockPtr)
	:	fBlockPtr(Block_copy(iBlockPtr))
		{}

	virtual ~Callable()
		{ Block_release(fBlockPtr); }

	// From ZCallable
	virtual R Call
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9)
		{ return fBlockPtr(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9); }

private:
	BlockPtr_t fBlockPtr;
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
	typedef void (^BlockPtr_t)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9);

	Callable(BlockPtr_t iBlockPtr)
	:	fBlockPtr(Block_copy(iBlockPtr))
		{}

	virtual ~Callable()
		{ Block_release(fBlockPtr); }

	// From ZCallable
	virtual void Call
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9)
		{ fBlockPtr(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9); }

private:
	BlockPtr_t fBlockPtr;
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
	typedef R (^BlockPtr_t)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA);

	Callable(BlockPtr_t iBlockPtr)
	:	fBlockPtr(Block_copy(iBlockPtr))
		{}

	virtual ~Callable()
		{ Block_release(fBlockPtr); }

	// From ZCallable
	virtual R Call
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA)
		{ return fBlockPtr(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA); }

private:
	BlockPtr_t fBlockPtr;
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
	typedef void (^BlockPtr_t)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA);

	Callable(BlockPtr_t iBlockPtr)
	:	fBlockPtr(Block_copy(iBlockPtr))
		{}

	virtual ~Callable()
		{ Block_release(fBlockPtr); }

	// From ZCallable
	virtual void Call
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA)
		{ fBlockPtr(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA); }

private:
	BlockPtr_t fBlockPtr;
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
	typedef R (^BlockPtr_t)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB);

	Callable(BlockPtr_t iBlockPtr)
	:	fBlockPtr(Block_copy(iBlockPtr))
		{}

	virtual ~Callable()
		{ Block_release(fBlockPtr); }

	// From ZCallable
	virtual R Call
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB)
		{ return fBlockPtr(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB); }

private:
	BlockPtr_t fBlockPtr;
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
	typedef void (^BlockPtr_t)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB);

	Callable(BlockPtr_t iBlockPtr)
	:	fBlockPtr(Block_copy(iBlockPtr))
		{}

	virtual ~Callable()
		{ Block_release(fBlockPtr); }

	// From ZCallable
	virtual void Call
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB)
		{ fBlockPtr(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB); }

private:
	BlockPtr_t fBlockPtr;
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
	typedef R (^BlockPtr_t)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC);

	Callable(BlockPtr_t iBlockPtr)
	:	fBlockPtr(Block_copy(iBlockPtr))
		{}

	virtual ~Callable()
		{ Block_release(fBlockPtr); }

	// From ZCallable
	virtual R Call
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB, PC iC)
		{ return fBlockPtr(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC); }

private:
	BlockPtr_t fBlockPtr;
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
	typedef void (^BlockPtr_t)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC);

	Callable(BlockPtr_t iBlockPtr)
	:	fBlockPtr(Block_copy(iBlockPtr))
		{}

	virtual ~Callable()
		{ Block_release(fBlockPtr); }

	// From ZCallable
	virtual void Call
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB, PC iC)
		{ fBlockPtr(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC); }

private:
	BlockPtr_t fBlockPtr;
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
	typedef R (^BlockPtr_t)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD);

	Callable(BlockPtr_t iBlockPtr)
	:	fBlockPtr(Block_copy(iBlockPtr))
		{}

	virtual ~Callable()
		{ Block_release(fBlockPtr); }

	// From ZCallable
	virtual R Call
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB, PC iC, PD iD)
		{ return fBlockPtr(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD); }

private:
	BlockPtr_t fBlockPtr;
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
	typedef void (^BlockPtr_t)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD);

	Callable(BlockPtr_t iBlockPtr)
	:	fBlockPtr(Block_copy(iBlockPtr))
		{}

	virtual ~Callable()
		{ Block_release(fBlockPtr); }

	// From ZCallable
	virtual void Call
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB, PC iC, PD iD)
		{ fBlockPtr(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD); }

private:
	BlockPtr_t fBlockPtr;
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
	typedef R (^BlockPtr_t)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE);

	Callable(BlockPtr_t iBlockPtr)
	:	fBlockPtr(Block_copy(iBlockPtr))
		{}

	virtual ~Callable()
		{ Block_release(fBlockPtr); }

	// From ZCallable
	virtual R Call
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB, PC iC, PD iD, PE iE)
		{ return fBlockPtr(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD, iE); }

private:
	BlockPtr_t fBlockPtr;
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
	typedef void (^BlockPtr_t)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE);

	Callable(BlockPtr_t iBlockPtr)
	:	fBlockPtr(Block_copy(iBlockPtr))
		{}

	virtual ~Callable()
		{ Block_release(fBlockPtr); }

	// From ZCallable
	virtual void Call
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB, PC iC, PD iD, PE iE)
		{ fBlockPtr(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD, iE); }

private:
	BlockPtr_t fBlockPtr;
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
	typedef R (^BlockPtr_t)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE,PF);

	Callable(BlockPtr_t iBlockPtr)
	:	fBlockPtr(Block_copy(iBlockPtr))
		{}

	virtual ~Callable()
		{ Block_release(fBlockPtr); }

	// From ZCallable
	virtual R Call
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB, PC iC, PD iD, PE iE, PF iF)
		{ return fBlockPtr(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD, iE, iF); }

private:
	BlockPtr_t fBlockPtr;
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
	typedef void (^BlockPtr_t)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE,PF);

	Callable(BlockPtr_t iBlockPtr)
	:	fBlockPtr(Block_copy(iBlockPtr))
		{}

	virtual ~Callable()
		{ Block_release(fBlockPtr); }

	// From ZCallable
	virtual void Call
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB, PC iC, PD iD, PE iE, PF iF)
		{ fBlockPtr(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD, iE, iF); }

private:
	BlockPtr_t fBlockPtr;
	};

// =================================================================================================
#pragma mark -
#pragma mark * MakeCallable

template <class R>
ZRef<ZCallable<R()> >
MakeCallable(R (^iBlockPtr)())
	{ return new Callable<R()>(iBlockPtr); }

inline
ZRef<ZCallable<void()> >
MakeCallable(void (^iBlockPtr)())
	{ return new Callable<void()>(iBlockPtr); }

template <class R,
	class P0>
ZRef<ZCallable<R(P0)> >
MakeCallable(R (^iBlockPtr)(P0))
	{ return new Callable<R(P0)>(iBlockPtr); }

template <
	class P0>
ZRef<ZCallable<void(P0)> >
MakeCallable(void (^iBlockPtr)(P0))
	{ return new Callable<void(P0)>(iBlockPtr); }

template <class R,
	class P0, class P1>
ZRef<ZCallable<R(P0,P1)> >
MakeCallable(R (^iBlockPtr)(P0,P1))
	{ return new Callable<R(P0,P1)>(iBlockPtr); }

template <
	class P0, class P1>
ZRef<ZCallable<void(P0,P1)> >
MakeCallable(void (^iBlockPtr)(P0,P1))
	{ return new Callable<void(P0,P1)>(iBlockPtr); }

template <class R,
	class P0, class P1, class P2>
ZRef<ZCallable<R(P0,P1,P2)> >
MakeCallable(R (^iBlockPtr)(P0,P1,P2))
	{ return new Callable<R(P0,P1,P2)>(iBlockPtr); }

template <
	class P0, class P1, class P2>
ZRef<ZCallable<void(P0,P1,P2)> >
MakeCallable(void (^iBlockPtr)(P0,P1,P2))
	{ return new Callable<void(P0,P1,P2)>(iBlockPtr); }

template <class R,
	class P0, class P1, class P2, class P3>
ZRef<ZCallable<R(P0,P1,P2,P3)> >
MakeCallable(R (^iBlockPtr)(P0,P1,P2,P3))
	{ return new Callable<R(P0,P1,P2,P3)>(iBlockPtr); }

template <
	class P0, class P1, class P2, class P3>
ZRef<ZCallable<void(P0,P1,P2,P3)> >
MakeCallable(void (^iBlockPtr)(P0,P1,P2,P3))
	{ return new Callable<void(P0,P1,P2,P3)>(iBlockPtr); }

template <class R,
	class P0, class P1, class P2, class P3,
	class P4>
ZRef<ZCallable<R(P0,P1,P2,P3,P4)> >
MakeCallable(R (^iBlockPtr)(P0,P1,P2,P3,P4))
	{ return new Callable<R(P0,P1,P2,P3,P4)>(iBlockPtr); }

template <
	class P0, class P1, class P2, class P3,
	class P4>
ZRef<ZCallable<void(P0,P1,P2,P3,P4)> >
MakeCallable(void (^iBlockPtr)(P0,P1,P2,P3,P4))
	{ return new Callable<void(P0,P1,P2,P3,P4)>(iBlockPtr); }

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5>
ZRef<ZCallable<R(P0,P1,P2,P3,P4,P5)> >
MakeCallable(R (^iBlockPtr)(P0,P1,P2,P3,P4,P5))
	{ return new Callable<R(P0,P1,P2,P3,P4,P5)>(iBlockPtr); }

template <
	class P0, class P1, class P2, class P3,
	class P4, class P5>
ZRef<ZCallable<void(P0,P1,P2,P3,P4,P5)> >
MakeCallable(void (^iBlockPtr)(P0,P1,P2,P3,P4,P5))
	{ return new Callable<void(P0,P1,P2,P3,P4,P5)>(iBlockPtr); }

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6>
ZRef<ZCallable<R(P0,P1,P2,P3,P4,P5,P6)> >
MakeCallable(R (^iBlockPtr)(P0,P1,P2,P3,P4,P5,P6))
	{ return new Callable<R(P0,P1,P2,P3,P4,P5,P6)>(iBlockPtr); }

template <
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6>
ZRef<ZCallable<void(P0,P1,P2,P3,P4,P5,P6)> >
MakeCallable(void (^iBlockPtr)(P0,P1,P2,P3,P4,P5,P6))
	{ return new Callable<void(P0,P1,P2,P3,P4,P5,P6)>(iBlockPtr); }

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7>
ZRef<ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7)> >
MakeCallable(R (^iBlockPtr)(P0,P1,P2,P3,P4,P5,P6,P7))
	{ return new Callable<R(P0,P1,P2,P3,P4,P5,P6,P7)>(iBlockPtr); }

template <
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7>
ZRef<ZCallable<void(P0,P1,P2,P3,P4,P5,P6,P7)> >
MakeCallable(void (^iBlockPtr)(P0,P1,P2,P3,P4,P5,P6,P7))
	{ return new Callable<void(P0,P1,P2,P3,P4,P5,P6,P7)>(iBlockPtr); }

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8>
ZRef<ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8)> >
MakeCallable(R (^iBlockPtr)(P0,P1,P2,P3,P4,P5,P6,P7,P8))
	{ return new Callable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8)>(iBlockPtr); }

template <
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8>
ZRef<ZCallable<void(P0,P1,P2,P3,P4,P5,P6,P7,P8)> >
MakeCallable(void (^iBlockPtr)(P0,P1,P2,P3,P4,P5,P6,P7,P8))
	{ return new Callable<void(P0,P1,P2,P3,P4,P5,P6,P7,P8)>(iBlockPtr); }

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9>
ZRef<ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9)> >
MakeCallable(R (^iBlockPtr)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9))
	{ return new Callable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9)>(iBlockPtr); }

template <
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9>
ZRef<ZCallable<void(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9)> >
MakeCallable(void (^iBlockPtr)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9))
	{ return new Callable<void(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9)>(iBlockPtr); }

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA>
ZRef<ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA)> >
MakeCallable(R (^iBlockPtr)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA))
	{ return new Callable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA)>(iBlockPtr); }

template <
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA>
ZRef<ZCallable<void(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA)> >
MakeCallable(void (^iBlockPtr)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA))
	{ return new Callable<void(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA)>(iBlockPtr); }

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB>
ZRef<ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB)> >
MakeCallable(R (^iBlockPtr)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB))
	{ return new Callable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB)>(iBlockPtr); }

template <
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB>
ZRef<ZCallable<void(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB)> >
MakeCallable(void (^iBlockPtr)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB))
	{ return new Callable<void(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB)>(iBlockPtr); }

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB,
	class PC>
ZRef<ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC)> >
MakeCallable(R (^iBlockPtr)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC))
	{ return new Callable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC)>(iBlockPtr); }

template <
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB,
	class PC>
ZRef<ZCallable<void(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC)> >
MakeCallable(void (^iBlockPtr)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC))
	{ return new Callable<void(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC)>(iBlockPtr); }

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB,
	class PC, class PD>
ZRef<ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD)> >
MakeCallable(R (^iBlockPtr)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD))
	{ return new Callable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD)>(iBlockPtr); }

template <
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB,
	class PC, class PD>
ZRef<ZCallable<void(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD)> >
MakeCallable(void (^iBlockPtr)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD))
	{ return new Callable<void(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD)>(iBlockPtr); }

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB,
	class PC, class PD, class PE>
ZRef<ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE)> >
MakeCallable(R (^iBlockPtr)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE))
	{ return new Callable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE)>(iBlockPtr); }

template <
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB,
	class PC, class PD, class PE>
ZRef<ZCallable<void(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE)> >
MakeCallable(void (^iBlockPtr)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE))
	{ return new Callable<void(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE)>(iBlockPtr); }

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB,
	class PC, class PD, class PE, class PF>
ZRef<ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE,PF)> >
MakeCallable(R (^iBlockPtr)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE,PF))
	{ return new Callable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE,PF)>(iBlockPtr); }

template <
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB,
	class PC, class PD, class PE, class PF>
ZRef<ZCallable<void(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE,PF)> >
MakeCallable(void (^iBlockPtr)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE,PF))
	{ return new Callable<void(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE,PF)>(iBlockPtr); }

} // namespace ZCallable_Block

// =================================================================================================
#pragma mark -
#pragma mark * MakeCallable

using ZCallable_Block::MakeCallable;

} // namespace ZooLib

#endif // defined(__BLOCKS__)
#endif // __ZCallable_Block__
