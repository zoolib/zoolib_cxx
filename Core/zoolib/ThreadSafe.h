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

#ifndef __ZooLib_ThreadSafe_h__
#define __ZooLib_ThreadSafe_h__ 1
#include "zconfig.h"

#include "zoolib/ZAtomic.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - ZMACRO_ThreadSafe Macros

// "Safe" increment and decrement. In non-preemptive environments this will do cheap
// addition/subtraction, and in preemptive situations it will do slightly less efficient, but
// thread-safe operations. You need to use the ThreadSafe_t type for variables that will
// manipulated by this. Right now it is a struct, but might degenerate into a simple integer
// at some point. ZMACRO_ThreadSafe_IncReturnNew and ZMACRO_ThreadSafe_DecReturnNew are used for
// COM's AddRef and Release.

typedef ZAtomic_t ThreadSafe_t;

// If your environment is not preemptive, then define ZCONFIG_Thread_Preemptive as zero.
#if !defined(ZCONFIG_Thread_Preemptive) || ZCONFIG_Thread_Preemptive

	#define ZMACRO_ThreadSafe_Get(x) sAtomic_Get(&x)
	#define ZMACRO_ThreadSafe_Set(x, y) sAtomic_Set(&x, y)
	#define ZMACRO_ThreadSafe_Swap(x, y) sAtomic_Swap(&x, y)

	#define ZMACRO_ThreadSafe_Inc(x) sAtomic_Inc(&x)
	#define ZMACRO_ThreadSafe_Dec(x) sAtomic_Dec(&x)
	#define ZMACRO_ThreadSafe_DecAndTest(x) sAtomic_DecAndTest(&x)

	#define ZMACRO_ThreadSafe_IncReturnNew(x) (sAtomic_Add(&x, 1) + 1)
	#define ZMACRO_ThreadSafe_DecReturnNew(x) (sAtomic_Add(&x, -1) - 1)

	#define ZMACRO_ThreadSafe_IncReturnOld(x) (sAtomic_Add(&x, 1))
	#define ZMACRO_ThreadSafe_DecReturnOld(x) (sAtomic_Add(&x, -1))

#else // ZCONFIG_Thread_Preemptive

	inline int ZMACRO_ThreadSafe_SwapHelper(ThreadSafe_t& inX, int inY)
		{
		int oldValue = inX.fValue;
		inX.fValue = inY;
		return oldValue;
		}
	#define ZMACRO_ThreadSafe_Swap(x, y) ZMACRO_ThreadSafe_SwapHelper(x, y)

	#define ZMACRO_ThreadSafe_Get(x) (x.fValue)
	#define ZMACRO_ThreadSafe_Set(x, y) ((void)((x).fValue = y))

	#define ZMACRO_ThreadSafe_Inc(x) ((void)(++(x).fValue))
	#define ZMACRO_ThreadSafe_Dec(x) ((void)(--(x).fValue))
	#define ZMACRO_ThreadSafe_DecAndTest(x) ((--(x).fValue) == 0)

	#define ZMACRO_ThreadSafe_IncReturnNew(x) (++(x).fValue)
	#define ZMACRO_ThreadSafe_DecReturnNew(x) (--(x).fValue)

	#define ZMACRO_ThreadSafe_IncReturnOld(x) ((x).fValue++)
	#define ZMACRO_ThreadSafe_DecReturnOld(x) ((x).fValue--)

#endif // ZCONFIG_Thread_Preemptive

} // namespace ZooLib

#endif // __ZooLib_ThreadSafe_h__
