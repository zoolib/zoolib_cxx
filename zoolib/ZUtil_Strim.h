/* -------------------------------------------------------------------------------------------------
Copyright (c) 2003 Andrew Green and Learning in Motion, Inc.
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

#ifndef __ZUtil_Strim__
#define __ZUtil_Strim__ 1
#include "zconfig.h"

#include "zoolib/ZStrim.h"

#include <stdexcept>

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * ZUtil_Strim

namespace ZUtil_Strim {

class ParseException : public std::runtime_error
	{
public:
	ParseException(const char* iWhat) : runtime_error(iWhat) {}
	ParseException(const std::string& iWhat) : runtime_error(iWhat.c_str()) {}
	};

// =================================================================================================

bool sTryRead_CP(const ZStrimU& iStrimU, UTF32 iCP);

bool sTryRead_CaselessString(const ZStrimU& iStrimU, const string8& iTarget);

bool sTryRead_Digit(const ZStrimU& iStrimU, int& oDigit);
bool sTryRead_HexDigit(const ZStrimU& iStrimU, int& oDigit);

bool sTryRead_SignedGenericInteger(const ZStrimU& iStrimU, int64& oInt64);

bool sTryRead_HexInteger(const ZStrimU& iStrimU, int64& oInt64);

bool sTryRead_Sign(const ZStrimU& iStrimU, bool& oIsNegative);
bool sTryRead_Mantissa(const ZStrimU& iStrimU, int64& oInt64, double& oDouble, bool& oIsDouble);

bool sTryRead_DecimalInteger(const ZStrimU& iStrimU, int64& oInt64);
bool sTryRead_SignedDecimalInteger(const ZStrimU& iStrimU, int64& oInt64);

bool sTryRead_DecimalNumber(const ZStrimU& iStrimU,
	int64& oInt64, double& oDouble, bool& oIsDouble);
bool sTryRead_SignedDecimalNumber(const ZStrimU& iStrimU,
	int64& oInt64, double& oDouble, bool& oIsDouble);

bool sTryRead_Double(const ZStrimU& iStrimU, double& oDouble);
bool sTryRead_SignedDouble(const ZStrimU& iStrimU, double& oDouble);


void sCopy_WS(const ZStrimU& iStrimU, const ZStrimW& iDest);
void sSkip_WS(const ZStrimU& iStrimU);


void sCopy_WSAndCPlusPlusComments(const ZStrimU& iStrimU, const ZStrimW& iDest);
void sSkip_WSAndCPlusPlusComments(const ZStrimU& iStrimU);


void sCopy_Line(const ZStrimR& iStrimR, const ZStrimW& iDest);
void sSkip_Line(const ZStrimR& iStrimR);
string8 sRead_Line(const ZStrimR& iStrimR);


bool sCopy_Until(const ZStrimR& iStrimR, UTF32 iTerminator, const ZStrimW& iDest);
bool sSkip_Until(const ZStrimR& iStrimR, UTF32 iTerminator);
string8 sRead_Until(const ZStrimR& iStrimR, UTF32 iTerminator);


bool sCopy_Until(const ZStrimR& iStrimR, const string8& iTerminator, const ZStrimW& iDest);
bool sSkip_Until(const ZStrimR& iStrimR, const string8& iTerminator);
string8 sRead_Until(const ZStrimR& iStrimR, const string8& iTerminator);


void sCopy_EscapedString(const ZStrimU& iStrimU, UTF32 iTerminator, const ZStrimW& iDest);
void sRead_EscapedString(const ZStrimU& iStrimU, UTF32 iTerminator, string8& oString);


bool sTryCopy_EscapedString(const ZStrimU& iStrimU, UTF32 iDelimiter, const ZStrimW& iDest);
bool sTryRead_EscapedString(const ZStrimU& iStrimU, UTF32 iDelimiter, string8& oString);


bool sTryCopy_Identifier(const ZStrimU& iStrimU, const ZStrimW& iDest);
bool sTryRead_Identifier(const ZStrimU& iStrimU, string8& oString);
// Other methods for other kinds of identifiers? -- Java, C++, Pascal, AlphaNum etc


void sWriteExact(const ZStrimW& iStrimW, float iFloat);
void sWriteExact(const ZStrimW& iStrimW, double iDouble);

} // namespace ZUtil_Strim

NAMESPACE_ZOOLIB_END

#endif // __ZUtil_Strim__
