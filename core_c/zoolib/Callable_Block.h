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

#ifndef __ZooLib_Callable_Block_h__
#define __ZooLib_Callable_Block_h__ 1
#include "zconfig.h"

#include "zoolib/Callable.h"

#if defined(__BLOCKS__)

#include <Block.h>

namespace ZooLib {

// =================================================================================================
// MARK: - Callable

template <class Signature> class Callable_Block;

// =================================================================================================
// MARK: - Callable (specialization for 0 params, void return)
// This is the only variant usable before Clang 2.0.

template <>
class Callable_Block<void(void)>
:	public Callable<void(void)>
	{
public:
	typedef void (^BlockPtr_t)();

	Callable_Block(BlockPtr_t iBlockPtr)
	:	fBlockPtr(Block_copy(iBlockPtr))
		{}

	virtual ~Callable_Block()
		{ Block_release(fBlockPtr); }

// From ZCallable
	virtual ZQ<void> QCall()
		{
		fBlockPtr();
		return notnull;
		}

private:
	BlockPtr_t fBlockPtr;
	};

#if defined(__clang_major__) && __clang_major__ >= 2

// =================================================================================================
// MARK: - Callable (specialization for 0 params)

template <class R>
class Callable_Block<R(void)>
:	public Callable<R(void)>
	{
public:
	typedef R (^BlockPtr_t)();

	Callable_Block(BlockPtr_t iBlockPtr)
	:	fBlockPtr(Block_copy(iBlockPtr))
		{}

	virtual ~Callable_Block()
		{ Block_release(fBlockPtr); }

// From ZCallable
	virtual ZQ<R> QCall()
		{ return fBlockPtr(); }

private:
	BlockPtr_t fBlockPtr;
	};

// =================================================================================================
// MARK: - Callable (specialization for 1 param)

#define ZMACRO_Callable_Callable(X) \
\
template <class R, ZMACRO_Callable_Class_P##X> \
class Callable_Block<R(ZMACRO_Callable_P##X)> \
:	public ZCallable<R(ZMACRO_Callable_P##X)> \
	{ \
public: \
	typedef R (^BlockPtr_t)(ZMACRO_Callable_P##X); \
\
	Callable_Block(BlockPtr_t iBlockPtr) \
	:	fBlockPtr(Block_copy(iBlockPtr)) \
		{} \
\
	virtual ~Callable_Block() \
		{ Block_release(fBlockPtr); } \
\
	virtual ZQ<R> QCall(ZMACRO_Callable_Pi##X) \
		{ return fBlockPtr(ZMACRO_Callable_i##X); } \
\
private: \
	BlockPtr_t fBlockPtr; \
	};

ZMACRO_Callable_Callable(0)
ZMACRO_Callable_Callable(1)
ZMACRO_Callable_Callable(2)
ZMACRO_Callable_Callable(3)
ZMACRO_Callable_Callable(4)
ZMACRO_Callable_Callable(5)
ZMACRO_Callable_Callable(6)
ZMACRO_Callable_Callable(7)
ZMACRO_Callable_Callable(8)
ZMACRO_Callable_Callable(9)
ZMACRO_Callable_Callable(A)
ZMACRO_Callable_Callable(B)
ZMACRO_Callable_Callable(C)
ZMACRO_Callable_Callable(D)
ZMACRO_Callable_Callable(E)
ZMACRO_Callable_Callable(F)

#undef ZMACRO_Callable_Callable

// =================================================================================================
// MARK: - sCallable

template <class R>
ZRef<ZCallable<R(void)> >
sCallable(R(^iBlockPtr)())
	{
	if (iBlockPtr)
		return new Callable_Block<R(void)>(iBlockPtr);
	return null;
	}

#define ZMACRO_Callable_sCallable(X) \
\
template <class R, ZMACRO_Callable_Class_P##X> \
ZRef<ZCallable<R(ZMACRO_Callable_P##X)> > \
sCallable(R(^iBlockPtr)(ZMACRO_Callable_P##X)) \
	{ \
	if (iBlockPtr) \
		return new Callable_Block<R(ZMACRO_Callable_P##X)>(iBlockPtr); \
	return null; \
	}

ZMACRO_Callable_sCallable(0)
ZMACRO_Callable_sCallable(1)
ZMACRO_Callable_sCallable(2)
ZMACRO_Callable_sCallable(3)
ZMACRO_Callable_sCallable(4)
ZMACRO_Callable_sCallable(5)
ZMACRO_Callable_sCallable(6)
ZMACRO_Callable_sCallable(7)
ZMACRO_Callable_sCallable(8)
ZMACRO_Callable_sCallable(9)
ZMACRO_Callable_sCallable(A)
ZMACRO_Callable_sCallable(B)
ZMACRO_Callable_sCallable(C)
ZMACRO_Callable_sCallable(D)
ZMACRO_Callable_sCallable(E)
ZMACRO_Callable_sCallable(F)

#undef ZMACRO_Callable_sCallable

#endif // defined(__clang_major__) && __clang_major__ >= 2

} // namespace ZooLib

#endif // defined(__BLOCKS__)

#endif // __ZooLib_Callable_Block_h__
