/* -------------------------------------------------------------------------------------------------
Copyright (c) 2016 Andrew Green
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

#ifndef __ZooLib_TypeList_h__
#define __ZooLib_TypeList_h__

#include "zconfig.h"

#include "zoolib/ZTypes.h" // For null_t

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark TypeList

template <
	class P0 = null_t, class P1 = null_t, class P2 = null_t, class P3 = null_t,
	class P4 = null_t, class P5 = null_t, class P6 = null_t, class P7 = null_t
	>
struct TypeList
	{
	typedef P0 T0;
	typedef P1 T1;
	typedef P2 T2;
	typedef P3 T3;
	typedef P4 T4;
	typedef P5 T5;
	typedef P6 T6;
	typedef P7 T7;
	};

} // namespace ZooLib

#endif // __ZooLib_TypeList_h__
