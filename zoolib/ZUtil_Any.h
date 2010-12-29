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

#ifndef __ZUtil_Any__
#define __ZUtil_Any__
#include "zconfig.h"

#include "zoolib/ZAny.h"
#include "zoolib/ZStdInt.h" // For int64

// =================================================================================================
#pragma mark -
#pragma mark * ZAny coercion

namespace ZooLib {

ZQ<bool> sQCoerceBool(const ZAny& iAny);
bool sQCoerceBool(const ZAny& iAny, bool& oVal);
bool sDCoerceBool(bool iDefault, const ZAny& iAny);
bool sCoerceBool(const ZAny& iAny);

ZQ<int64> sQCoerceInt(const ZAny& iAny);
bool sQCoerceInt(const ZAny& iAny, int64& oVal);
int64 sDCoerceInt(int64 iDefault, const ZAny& iAny);
int64 sCoerceInt(const ZAny& iAny);

ZQ<double> sQCoerceReal(const ZAny& iAny);
bool sQCoerceReal(const ZAny& iAny, double& oVal);
double sDCoerceReal(double iDefault, const ZAny& iAny);
double sCoerceReal(const ZAny& iAny);

} // namespace ZooLib

#endif // __ZUtil_Any__
