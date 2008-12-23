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

#ifndef __ZThreadSafe__
#define __ZThreadSafe__ 1
#include "zconfig.h"

#include "zoolib/ZAtomic.h"

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * ZThreadSafe Macros

// "Safe" increment and decrement. In non-preemptive environments this will do cheap
// addition/subtraction, and in preemptive situations it will do slightly less efficient, but
// thread-safe operations. You need to use the ZThreadSafe_t type for variables that will
// manipulated by this. Right now it is a struct, but might degenerate into a simple integer
// at some point. ZThreadSafe_IncReturnNew and ZThreadSafe_DecReturnNew are used for
// COM's AddRef and Release.

typedef ZAtomic_t ZThreadSafe_t;

// If your environment is not preemptive, then define ZCONFIG_Thread_Preemptive as zero.
#if !defined(ZCONFIG_Thread_Preemptive) || ZCONFIG_Thread_Preemptive

	#define ZThreadSafe_Get(x) ZAtomic_Get(&x)
	#define ZThreadSafe_Set(x, y) ZAtomic_Set(&x, y)
	#define ZThreadSafe_Swap(x, y) ZAtomic_Swap(&x, y)

	#define ZThreadSafe_Inc(x) ZAtomic_Inc(&x)
	#define ZThreadSafe_Dec(x) ZAtomic_Dec(&x)
	#define ZThreadSafe_DecAndTest(x) ZAtomic_DecAndTest(&x)

	#define ZThreadSafe_IncReturnNew(x) (ZAtomic_Add(&x, 1) + 1)
	#define ZThreadSafe_DecReturnNew(x) (ZAtomic_Add(&x, -1) - 1)

	#define ZThreadSafe_IncReturnOld(x) (ZAtomic_Add(&x, 1))
	#define ZThreadSafe_DecReturnOld(x) (ZAtomic_Add(&x, -1))

#else // ZCONFIG_Thread_Preemptive

	inline int ZThreadSafe_SwapHelper(ZThreadSafe_t& inX, int inY)
		{
		int oldValue = inX.fValue;
		inX.fValue = inY;
		return oldValue;
		}
	#define ZThreadSafe_Swap(x, y) ZThreadSafe_SwapHelper(x, y)

	#define ZThreadSafe_Get(x) (x.fValue)
	#define ZThreadSafe_Set(x, y) ((void)((x).fValue = y))

	#define ZThreadSafe_Inc(x) ((void)(++(x).fValue))
	#define ZThreadSafe_Dec(x) ((void)(--(x).fValue))
	#define ZThreadSafe_DecAndTest(x) ((--(x).fValue) == 0)

	#define ZThreadSafe_IncReturnNew(x) (++(x).fValue)
	#define ZThreadSafe_DecReturnNew(x) (--(x).fValue)

	#define ZThreadSafe_IncReturnOld(x) ((x).fValue++)
	#define ZThreadSafe_DecReturnOld(x) ((x).fValue--)

#endif // ZCONFIG_Thread_Preemptive

NAMESPACE_ZOOLIB_END

#endif // __ZThreadSafe__
