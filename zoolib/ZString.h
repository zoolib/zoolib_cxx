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

#include <stdarg.h>
#include <string>

#include "zoolib/ZTypes.h" // for uint64

// ==================================================

NAMESPACE_ZOOLIB_BEGIN

class ZStreamR;
class ZStreamW;

namespace ZString {

using std::string;

string sFormat(const char* iString, ...);
string sFormat(const char* iString, va_list iArgs);

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

bool sInt64(const string& iString, int64& oVal);
int64 sInt64D(const string& iString, int64 iDefault);
int64 sInt64(const string& iString);

bool sUInt64(const string& iString, uint64& oVal);
uint64 sUInt64D(const string& iString, uint64 iDefault);
uint64 sUInt64(const string& iString);

bool sDouble(const string& iString, double& oVal);
double sDoubleD(const string& iString, double iDefault);
double sDouble(const string& iString);

// -- 

void sToStream(const string& iString, const ZStreamW& iStream);
void sFromStream(string& outString, const ZStreamR& iStream);
string sFromStream(const ZStreamR& iStream);

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

NAMESPACE_ZOOLIB_END

#endif // __ZString__
