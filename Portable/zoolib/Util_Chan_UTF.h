/* -------------------------------------------------------------------------------------------------
Copyright (c) 2014 Andrew Green
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

#ifndef __ZooLib_Util_Chan_UTF_h__
#define __ZooLib_Util_Chan_UTF_h__ 1
#include "zconfig.h"

#include "zoolib/ChanRU_UTF.h"
#include "zoolib/ChanW_UTF.h"
#include "zoolib/ParseException.h"
#include "zoolib/Util_Chan.h"

namespace ZooLib {
namespace Util_Chan {

// =================================================================================================
#pragma mark -

string8 sRead_Until(const ChanR_UTF& iSource, UTF32 iTerminator);

// -----------------

bool sTryRead_CP(UTF32 iCP, const ChanRU_UTF& iChanRU);

// -----------------

ZQ<int> sQRead_Digit(const ChanRU_UTF& iChanRU);

ZQ<int> sQRead_HexDigit(const ChanRU_UTF& iChanRU);

// -----------------

bool sRead_String(const string8& iPattern, const ChanR_UTF& iChanR);

// -----------------

bool sTryRead_String(const string8& iPattern, const ChanRU_UTF& iChanRU);

bool sTryRead_CaselessString(const string8& iPattern, const ChanRU_UTF& iChanRU);

bool sTryRead_Sign(const ChanRU_UTF& iChanRU, bool& oIsNegative);

bool sTryRead_HexInteger(const ChanRU_UTF& iChanRU, int64& oInt64);

bool sTryRead_DecimalInteger(const ChanRU_UTF& iChanRU, int64& oInt64);

bool sTryRead_SignedDecimalInteger(const ChanRU_UTF& iChanRU, int64& oInt64);

bool sTryRead_SignedGenericNumber(const ChanRU_UTF& iChanRU,
int64& oInt64, double& oDouble, bool& oIsDouble);

// -----------------

bool sSkip_WS(const ChanRU_UTF& iChanRU);

// -----------------

bool sCopy_Line(const ChanR_UTF& iSource, const ChanW_UTF& oDest);
bool sSkip_Line(const ChanR_UTF& iSource);
ZQ<string8> sQRead_Line(const ChanR_UTF& iSource);

// -----------------

bool sCopy_WSAndCPlusPlusComments(const ChanRU_UTF& iChanRU, const ChanW_UTF& oDest);

bool sSkip_WSAndCPlusPlusComments(const ChanRU_UTF& iChanRU);

// -----------------

bool sCopy_Until(const ChanR_UTF& iChanR, const string8& iTerminator, const ChanW_UTF& oDest);

bool sSkip_Until(const ChanR_UTF& iChanR, const string8& iTerminator);

bool sRead_Until(const ChanR_UTF& iChanR, const string8& iTerminator, string8& oString);

// -----------------

void sCopy_EscapedString(UTF32 iTerminator, const ChanRU_UTF& iChanRU, const ChanW_UTF& oDest);

void sRead_EscapedString(UTF32 iTerminator, const ChanRU_UTF& iChanRU, string8& oString);

// -----------------

bool sTryCopy_EscapedString(UTF32 iDelimiter, const ChanRU_UTF& iChanRU, const ChanW_UTF& oDest);

bool sTryRead_EscapedString(UTF32 iDelimiter, const ChanRU_UTF& iChanRU, string8& oString);

// =================================================================================================
#pragma mark -

void sWriteExact(const ChanW_UTF& iChanW, float iFloat);
void sWriteExact(const ChanW_UTF& iChanW, double iDouble);
void sWriteExact(const ChanW_UTF& iChanW, long double iVal);

} // namespace Util_Chan
} // namespace ZooLib

#endif // __ZooLib_Util_Chan_UTF_h__
