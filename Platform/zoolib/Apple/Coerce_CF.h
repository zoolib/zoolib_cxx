/* -------------------------------------------------------------------------------------------------
Copyright (c) 2018 Andrew Green and Mark/Space, Inc.
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

#ifndef __ZooLib_Apple_Coerce_CF_h__
#define __ZooLib_Apple_Coerce_CF_h__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_SPI.h"

#if ZCONFIG_SPI_Enabled(CFType)

#include "zoolib/ZQ.h"
#include "zoolib/Apple/ZRef_CF.h"

// =================================================================================================
#pragma mark -
#pragma mark Any coercion

namespace ZooLib {

ZQ<bool> sQCoerceBool(CFTypeRef iCF);
bool sQCoerceBool(CFTypeRef iCF, bool& oVal);
bool sDCoerceBool(bool iDefault, CFTypeRef iCF);
bool sCoerceBool(CFTypeRef iCF);

ZQ<__int64> sQCoerceInt(CFTypeRef iCF);
bool sQCoerceInt(CFTypeRef iCF, __int64& oVal);
__int64 sDCoerceInt(__int64 iDefault, CFTypeRef iCF);
__int64 sCoerceInt(CFTypeRef iCF);

ZQ<double> sQCoerceRat(CFTypeRef iCF);
bool sQCoerceRat(CFTypeRef iCF, double& oVal);
double sDCoerceRat(double iDefault, CFTypeRef iCF);
double sCoerceRat(CFTypeRef iCF);

} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(CFType)

#endif // __ZooLib_Apple_Coerce_CF_h__
