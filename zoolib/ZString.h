/* -------------------------------------------------------------------------------------------------
Copyright (c) 2000 Andrew Green and Learning in Motion, Inc.
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

#ifndef __ZString__
#define __ZString__ 1
#include "zconfig.h"

#include <cstdarg>
#include <string>

#include "zoolib/ZTypes.h" // for uint64

// ==================================================

class ZStreamR;
class ZStreamW;

namespace ZString {

std::string sFormat(const char* inString, ...);
std::string sFormat(const char* inString, va_list iArgs);

std::string sSubstitute(const std::string& iString,
	const std::string& iS0);

std::string sSubstitute(const std::string& iString,
	const std::string& iS0,
	const std::string& iS1);

std::string sSubstitute(const std::string& iString,
	const std::string& iS0,
	const std::string& iS1,
	const std::string& iS2);

std::string sSubstitute(const std::string& iString,
	const std::string& iS0,
	const std::string& iS1,
	const std::string& iS2,
	const std::string& iS3);

std::string sSubstitute(const std::string& iString,
	const std::string& iS0,
	const std::string& iS1,
	const std::string& iS2,
	const std::string& iS3,
	const std::string& iS4);

std::string sSubstitute(const std::string& iString,
	const std::string* iParams, size_t iParamsCount);

int sComparei(const std::string& iLeft, const std::string& iRight);
bool sEquali(const std::string& iLeft, const std::string& iRight);
bool sContainsi(const std::string& iTarget, const std::string& iCandidate);

std::string sMacizeString(const std::string& iString);

std::string sFromInt(int iVal);
int sAsInt(const std::string& iString);

std::string sHexFromInt(int iVal);
std::string sHexFromUInt64(uint64 iVal);

std::string sFromUInt64(uint64 iVal); // Why uint64 and not int64?
uint64 sAsUInt64(const std::string& iString);

void sToStream(const std::string& iString, const ZStreamW& iStream);
void sFromStream(std::string& outString, const ZStreamR& iStream);
std::string sFromStream(const ZStreamR& iStream);

std::string sFresh(const std::string& iOther);
void sMakeFresh(std::string& ioString);

const unsigned char* sAsPString(const std::string& iString);
std::string sFromPString(const unsigned char* iPString);
void sToPString(const std::string& iString, unsigned char* oPString, size_t iMaxLength);
void sToPString(const char* iString, unsigned char* oPString, size_t iMaxLength);

} // namespace ZString

#endif // __ZString__
