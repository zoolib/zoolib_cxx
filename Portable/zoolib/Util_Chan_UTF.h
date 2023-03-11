// Copyright (c) 2014 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Util_Chan_UTF_h__
#define __ZooLib_Util_Chan_UTF_h__ 1
#include "zconfig.h"

#include "zoolib/Chan_UTF.h"
#include "zoolib/Util_Chan.h"
#include "zoolib/UnicodeString8.h"

namespace ZooLib {
namespace Util_Chan {

// =================================================================================================
#pragma mark -

string8 sRead_Until(const ChanR_UTF& iSource, UTF32 iTerminator);

// -----------------

bool sTryRead_CP(const ChanRU_UTF& iChanRU, UTF32 iCP);

// -----------------

ZQ<int> sQRead_Digit(const ChanRU_UTF& iChanRU);

ZQ<int> sQValueIfHex(UTF32 iCP); // Temporary home

ZQ<int> sQRead_HexDigit(const ChanRU_UTF& iChanRU);

// -----------------

bool sRead_String(const ChanR_UTF& iChanR, const string8& iPattern);

// -----------------

bool sTryRead_String(const ChanRU_UTF& iChanRU, const string8& iPattern);

bool sTryRead_CaselessString(const ChanRU_UTF& iChanRU, const string8& iPattern);

bool sTryRead_Sign(const ChanRU_UTF& iChanRU, bool& oIsNegative);

bool sTryRead_HexInteger(const ChanRU_UTF& iChanRU, int64& oInt64);

bool sTryRead_DecimalInteger(const ChanRU_UTF& iChanRU, int64& oInt64);

ZQ<int64> sQTryRead_DecimalInteger(const ChanRU_UTF& iChanRU);

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

void sWrite_Exact(const ChanW_UTF& iChanW, float iFloat);
void sWrite_Exact(const ChanW_UTF& iChanW, double iDouble);
void sWrite_Exact(const ChanW_UTF& iChanW, long double iVal);

} // namespace Util_Chan
} // namespace ZooLib

#endif // __ZooLib_Util_Chan_UTF_h__
