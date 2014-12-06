/* -------------------------------------------------------------------------------------------------
Copyright (c) 2011 Andrew Green
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

#ifndef __ZooLib_Util_string_h__
#define __ZooLib_Util_string_h__ 1
#include "zconfig.h"

#include "zoolib/ZQ.h"

#include <string>
#include <vector>

namespace ZooLib {
namespace Util_string {

using std::string;

// =================================================================================================
// MARK: - Util_string

ZQ<__int64> sQInt64(const string& iString);
__int64 sDInt64(__int64 iDefault, const string& iString);
__int64 sInt64(const string& iString);

ZQ<__uint64> sQUInt64(const string& iString);
__uint64 sDUInt64(__uint64 iDefault, const string& iString);
__uint64 sUInt64(const string& iString);

ZQ<double> sQDouble(const string& iString);
double sDDouble(double iDefault, const string& iString);
double sDouble(const string& iString);

// -----

int sComparei(const string& iLeft, const string& iRight);
bool sEquali(const string& iLeft, const string& iRight);
bool sContainsi(const string& iPattern, const string& iTarget);

// -----

ZQ<string> sQWithoutPrefix(const string& iPattern, const string& iTarget);
ZQ<string> sQWithoutSuffix(const string& iPattern, const string& iTarget);

bool sStartsWith(const string& iPattern, const string& iTarget);
bool sEndsWith(const string& iPattern, const string& iTarget);

// -----

string sFromPString(const unsigned char* iPString);
void sToPString(const string& iString, unsigned char* oPString, size_t iMaxLength);
void sToPString(const char* iString, unsigned char* oPString, size_t iMaxLength);

// -----

std::vector<string> sSplit(const string& iString, char iChar);

} // namespace Util_string
} // namespace ZooLib

#endif // __ZooLib_Util_string_h__
