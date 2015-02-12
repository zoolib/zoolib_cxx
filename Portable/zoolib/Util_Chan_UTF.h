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

#include "zoolib/ChanR_UTF.h"
#include "zoolib/ChanU_UTF.h"
#include "zoolib/ChanW_UTF.h"
#include "zoolib/Util_Chan.h"

namespace ZooLib {
namespace Util_Chan {

// =================================================================================================
#pragma mark -

class ParseException : public std::runtime_error
	{
public:
	ParseException(const char* iWhat) : runtime_error(iWhat) {}
	ParseException(const std::string& iWhat) : runtime_error(iWhat.c_str()) {}
	};

// =================================================================================================
#pragma mark -

string8 sRead_Until(const ChanR_UTF& iSource, UTF32 iTerminator);

// -----------------

bool sTryRead_CP(UTF32 iCP, const ChanR_UTF& iChanR, const ChanU_UTF& iChanU);

// -----------------

ZQ<int> sQRead_Digit(const ChanR_UTF& iChanR, const ChanU_UTF& iChanU);
ZQ<int> sQRead_HexDigit(const ChanR_UTF& iChanR, const ChanU_UTF& iChanU);

// -----------------

bool sTryRead_CaselessString(const string8& iTarget,
	const ChanR_UTF& iChanR, const ChanU_UTF& iChanU);

bool sTryRead_Sign(const ChanR_UTF& iChanR, const ChanU_UTF& iChanU, bool& oIsNegative);

bool sTryRead_SignedGenericNumber(const ChanR_UTF& iChanR, const ChanU_UTF& iChanU,
	int64& oInt64, double& oDouble, bool& oIsDouble);

// -----------------

void sCopy_Line(const ChanR_UTF& iSource, const ChanW_UTF& oDest);
void sSkip_Line(const ChanR_UTF& iSource);
string8 sRead_Line(const ChanR_UTF& iSource);

// -----------------

void sCopy_WSAndCPlusPlusComments(const ChanR_UTF& iChanR, const ChanU_UTF& iChanU,
	const ChanW_UTF& oDest);

void sSkip_WSAndCPlusPlusComments(const ChanR_UTF& iChanR, const ChanU_UTF& iChanU);

// -----------------

void sCopy_EscapedString(UTF32 iTerminator, const ChanR_UTF& iChanR, const ChanU_UTF& iChanU,
	const ChanW_UTF& oDest);

void sRead_EscapedString(UTF32 iTerminator, const ChanR_UTF& iChanR, const ChanU_UTF& iChanU,
	string8& oString);

// -----------------

bool sTryCopy_EscapedString(UTF32 iDelimiter, const ChanR_UTF& iChanR, const ChanU_UTF& iChanU,
	const ChanW_UTF& oDest);

bool sTryRead_EscapedString(UTF32 iDelimiter, const ChanR_UTF& iChanR, const ChanU_UTF& iChanU,
	string8& oString);

// =================================================================================================
#pragma mark -

void sWriteExact(float iFloat, const ChanW_UTF& iChanW);
void sWriteExact(double iDouble, const ChanW_UTF& iChanW);
void sWriteExact(long double iVal, const ChanW_UTF& iChanW);

} // namespace Util_Chan
} // namespace ZooLib

#endif // __ZooLib_Util_Chan_UTF_h__
