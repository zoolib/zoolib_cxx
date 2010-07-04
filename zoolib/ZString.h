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

#include "zoolib/ZQ.h"
#include "zoolib/ZStdInt.h" // for uint64

#ifndef ZMACRO_ATTRIBUTE_FORMAT_PRINTF
	#if ZCONFIG(Compiler,GCC)
		#define ZMACRO_ATTRIBUTE_FORMAT_PRINTF(m,n) __attribute__((format(printf,m,n)))
	#else
		#define ZMACRO_ATTRIBUTE_FORMAT_PRINTF(m,n)
	#endif
#endif

// ==================================================

namespace ZooLib {

std::string ZStringf(const char* iString, ...)
	ZMACRO_ATTRIBUTE_FORMAT_PRINTF(1,2);

std::string ZStringf(const char* iString, std::va_list iArgs);

namespace ZString {

using std::string;

string sFormat(const char* iString, ...)
	ZMACRO_ATTRIBUTE_FORMAT_PRINTF(1,2);

string sFormat(const char* iString, std::va_list iArgs);

string sSubstitute(const string& iString,
	const string& iS0);

string sSubstitute(const string& iString,
	const string& iS0,
	const string& iS1);

string sSubstitute(const string& iString,
	const string& iS0,
	const string& iS1,
	const string& iS2);

string sSubstitute(const string& iString,
	const string& iS0,
	const string& iS1,
	const string& iS2,
	const string& iS3);

string sSubstitute(const string& iString,
	const string& iS0,
	const string& iS1,
	const string& iS2,
	const string& iS3,
	const string& iS4);

string sSubstitute(const string& iString,
	const string* iParams, size_t iParamsCount);

int sComparei(const string& iLeft, const string& iRight);
bool sEquali(const string& iLeft, const string& iRight);
bool sContainsi(const string& iTarget, const string& iCandidate);

// --
// ZUtil_Tuple-style conversion API

ZQ<int64> sQInt64(const string& iString);
bool sQInt64(const string& iString, int64& oVal);
int64 sDInt64(int64 iDefault, const string& iString);
int64 sInt64(const string& iString);

ZQ<uint64> sQUInt64(const string& iString);
bool sQUInt64(const string& iString, uint64& oVal);
uint64 sDUInt64(uint64 iDefault, const string& iString);
uint64 sUInt64(const string& iString);

ZQ<double> sQDouble(const string& iString);
bool sQDouble(const string& iString, double& oVal);
double sDDouble(double iDefault, const string& iString);
double sDouble(const string& iString);

// --

string sFresh(const string& iOther);
void sMakeFresh(string& ioString);

// --

inline int sAsInt(const string& iString)
	{ return int(sInt64(iString)); }

inline uint64 sAsUInt64(const string& iString)
	{ return sUInt64(iString); }

string sFromInt(int iVal);
string sFromUInt64(uint64 iVal); // Why uint64 and not int64?
string sHexFromInt(int iVal);
string sHexFromUInt64(uint64 iVal);

string sFromPString(const unsigned char* iPString);
void sToPString(const string& iString, unsigned char* oPString, size_t iMaxLength);
void sToPString(const char* iString, unsigned char* oPString, size_t iMaxLength);

} // namespace ZString

} // namespace ZooLib

#endif // __ZString__
