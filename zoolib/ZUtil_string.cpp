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

#include "zoolib/ZCompat_algorithm.h" // For min
#include "zoolib/ZCompat_string.h" // For strncasecmp
#include "zoolib/ZMemory.h" // For ZMemCopy
#include "zoolib/ZUnicode.h"
#include "zoolib/ZUtil_string.h"

#include <stdio.h>

#if ZCONFIG(Compiler, MSVC)
	#define strncasecmp _strnicmp
#endif

namespace ZooLib {
namespace ZUtil_string {

using std::min;
using std::vector;

// =================================================================================================
// MARK: - ZUtil_string

ZQ<int64> sQInt64(const string& iString)
	{
	if (iString.size())
		{
		__int64 result;
		if (sscanf(iString.c_str(), "%lld", &result) > 0)
			return result;
		}
	return null;
	}

__int64 sDInt64(__int64 iDefault, const string& iString)
	{
	if (ZQ<__int64> qInt64 = sQInt64(iString))
		return qInt64.Get();
	return iDefault;
	}

__int64 sInt64(const string& iString)
	{
	if (ZQ<__int64> qInt64 = sQInt64(iString))
		return qInt64.Get();
	return 0;
	}

ZQ<__uint64> sQUInt64(const string& iString)
	{
	if (iString.size())
		{
		__uint64 result;
		if (sscanf(iString.c_str(), "%llu", &result) > 0)
			return result;
		}
	return null;
	}

__uint64 sDUInt64(__uint64 iDefault, const string& iString)
	{
	if (ZQ<__uint64> qUInt64 = sQUInt64(iString))
		return qUInt64.Get();
	return iDefault;
	}

__uint64 sUInt64(const string& iString)
	{
	if (ZQ<__uint64> qUInt64 = sQUInt64(iString))
		return qUInt64.Get();
	return 0;
	}

ZQ<double> sQDouble(const string& iString)
	{
	if (iString.size())
		{
		double result;
		if (sscanf(iString.c_str(), "%lf", &result) > 0)
			return result;
		}
	return null;
	}

double sDDouble(double iDefault, const string& iString)
	{
	if (ZQ<double> qDouble = sQDouble(iString))
		return qDouble.Get();
	return iDefault;
	}

double sDouble(const string& iString)
	{
	if (ZQ<double> qDouble = sQDouble(iString))
		return qDouble.Get();
	return 0.0;
	}

// =================================================================================================

int sComparei(const string& iLeft, const string& iRight)
	{
	if (const size_t sizeLeft = iLeft.size())
		{
		if (const size_t sizeRight = iRight.size())
			{
			if (const int result =
				strncasecmp(iLeft.data(), iRight.data(), min(sizeLeft, sizeRight)))
				{
				return result;
				}
			else if (sizeLeft < sizeRight)
				return -1;
			else if (sizeRight < sizeLeft)
				return 1;
			else
				return 0;
			}
		else
			{
			return 1;
			}
		}
	else if (iRight.size())
		{
		return -1;
		}
	else
		{
		return 0;
		}
	}

bool sEquali(const string& iLeft, const string& iRight)
	{
	if (const size_t theSize = iLeft.size())
		{
		if (theSize != iRight.size())
			return false;

		return 0 == strncasecmp(iLeft.data(), iRight.data(), theSize);
		}
	else
		{
		return ! iRight.size();
		}
	}

bool sContainsi(const string& iPattern, const string& iTarget)
	{
	return string::npos != ZUnicode::sToLower(iTarget).find(ZUnicode::sToLower(iPattern));
	}

ZQ<string> sQWithoutPrefix(const string& iPattern, const string& iTarget)
	{
	if (size_t patternLength = iPattern.length())
		{
		if (size_t targetLength = iTarget.length())
			{
			if (patternLength <= targetLength)
				{
				if (iTarget.substr(0, patternLength) == iPattern)
					return iTarget.substr(patternLength);
				}
			}
		return null;
		}
	return iTarget;
	}

ZQ<string> sQWithoutSuffix(const string& iPattern, const string& iTarget)
	{
	if (size_t patternLength = iPattern.length())
		{
		if (size_t targetLength = iTarget.length())
			{
			if (patternLength <= targetLength)
				{
				if (iTarget.substr(targetLength - patternLength) == iPattern)
					return iTarget.substr(0, targetLength - patternLength);
				}
			}
		return null;
		}
	return iTarget;
	}

bool sStartsWith(const string& iPattern, const string& iTarget)
	{ return sQWithoutPrefix(iPattern, iTarget); }

bool sEndsWith(const string& iPattern, const string& iTarget)
	{ return sQWithoutSuffix(iPattern, iTarget); }

// =================================================================================================

string sFromPString(const unsigned char* inPString)
	{ return string((const char*)(&(inPString[1])), (long)inPString[0]); }

void sToPString(const string& inString, unsigned char* outPString, size_t inMaxLength)
	{
	size_t sourceLength = min(inString.size(), inMaxLength);
	if (sourceLength)
		ZMemCopy(&(outPString[1]), inString.data(), sourceLength);
	outPString[0] = sourceLength;
	}

void sToPString(const char* inString, unsigned char* outPString, size_t inMaxLength)
	{
	size_t sourceLength = min(strlen(inString), inMaxLength);
	if (sourceLength)
		ZMemCopy(&(outPString[1]), inString, sourceLength);
	outPString[0] = sourceLength;
	}

// =================================================================================================

vector<string> sSplit(const string& iString, char iChar)
	{
	vector<string> result;
	size_t index = 0;

	for (;;)
		{
		const size_t next = iString.find(iChar, index);
		result.push_back(iString.substr(index, next - index));
		if (next == string::npos)
			break;
		index = next + 1;
		}

	return result;
	}

} // namespace ZUtil_string
} // namespace ZooLib
