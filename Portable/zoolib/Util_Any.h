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

#ifndef __ZooLib_Util_Any_h__
#define __ZooLib_Util_Any_h__ 1
#include "zconfig.h"

#include "zoolib/Any.h"

// =================================================================================================
#pragma mark -
#pragma mark Any coercion

namespace ZooLib {

ZQ<bool> sQCoerceBool(const Any& iAny);
bool sQCoerceBool(const Any& iAny, bool& oVal);
bool sDCoerceBool(bool iDefault, const Any& iAny);
bool sCoerceBool(const Any& iAny);

ZQ<__int64> sQCoerceInt(const Any& iAny);
bool sQCoerceInt(const Any& iAny, __int64& oVal);
__int64 sDCoerceInt(__int64 iDefault, const Any& iAny);
__int64 sCoerceInt(const Any& iAny);

ZQ<double> sQCoerceRat(const Any& iAny);
bool sQCoerceRat(const Any& iAny, double& oVal);
double sDCoerceRat(double iDefault, const Any& iAny);
double sCoerceRat(const Any& iAny);

} // namespace ZooLib

#endif // __ZooLib_Util_Any_h__
