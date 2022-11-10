// Copyright (c) 2011 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/Util_string.h"

#include "zoolib/Compat_algorithm.h" // For min
#include "zoolib/Compat_string.h" // For strncasecmp
#include "zoolib/Memory.h" // For sMemCopy
#include "zoolib/Unicode.h"

#include <stdio.h>

#if ZCONFIG(Compiler, MSVC)
	#define strncasecmp _strnicmp
#endif

namespace ZooLib {
namespace Util_string {

using std::min;
using std::vector;

// =================================================================================================
#pragma mark - Util_string

ZQ<int64> sQInt64(const string& iString)
	{
	if (iString.size())
		{
		long long result;
		if (sscanf(iString.c_str(), "%lld", &result) > 0)
			return result;
		}
	return null;
	}

int64 sInt64(const string& iString)
	{
	if (ZQ<int64> qInt64 = sQInt64(iString))
		return qInt64.Get();
	return 0;
	}

ZQ<uint64> sQUInt64(const string& iString)
	{
	if (iString.size())
		{
		unsigned long long result;
		if (sscanf(iString.c_str(), "%llu", &result) > 0)
			return result;
		}
	return null;
	}

uint64 sUInt64(const string& iString)
	{
	if (ZQ<uint64> qUInt64 = sQUInt64(iString))
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

double sDouble(const string& iString)
	{
	if (ZQ<double> qDouble = sQDouble(iString))
		return qDouble.Get();
	return 0.0;
	}

// =================================================================================================

bool sContains(const string& iTarget, char iPattern)
	{ return string::npos != iTarget.find(iPattern); }

bool sContains(const string& iTarget, const string& iPattern)
	{ return string::npos != iTarget.find(iPattern); }

bool sContainsi(const string& iTarget, const string& iPattern)
	{ return sContains(Unicode::sToLower(iTarget), Unicode::sToLower(iPattern)); }

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

ZQ<string> sQWithoutPrefix(const string& iTarget, const string& iPrefix)
	{
	if (size_t prefixLength = iPrefix.length())
		{
		if (size_t targetLength = iTarget.length())
			{
			if (prefixLength <= targetLength)
				{
				if (iTarget.substr(0, prefixLength) == iPrefix)
					return iTarget.substr(prefixLength);
				}
			}
		return null;
		}
	return iTarget;
	}

ZQ<string> sQWithoutPrefixi(const string& iTarget, const string& iPrefix)
	{
	if (size_t prefixLength = iPrefix.length())
		{
		if (size_t targetLength = iTarget.length())
			{
			if (prefixLength <= targetLength)
				{
				if (sEquali(iPrefix, iTarget.substr(0, prefixLength)))
					return iTarget.substr(prefixLength);
				}
			}
		return null;
		}
	return iTarget;
	}

ZQ<string> sQWithoutSuffix(const string& iTarget, const string& iSuffix)
	{
	if (size_t suffixLength = iSuffix.length())
		{
		if (size_t targetLength = iTarget.length())
			{
			if (suffixLength <= targetLength)
				{
				if (iTarget.substr(targetLength - suffixLength) == iSuffix)
					return iTarget.substr(0, targetLength - suffixLength);
				}
			}
		return null;
		}
	return iTarget;
	}

ZQ<string> sQWithoutSuffixi(const string& iTarget, const string& iSuffix)
	{
	if (size_t suffixLength = iSuffix.length())
		{
		if (size_t targetLength = iTarget.length())
			{
			if (suffixLength <= targetLength)
				{
				if (sEquali(iSuffix, iTarget.substr(targetLength - suffixLength)))
					return iTarget.substr(0, targetLength - suffixLength);
				}
			}
		return null;
		}
	return iTarget;
	}

bool sStartsWith(const string& iTarget, const string& iPossiblePrefix)
	{ return true && sQWithoutPrefix(iTarget, iPossiblePrefix); }

bool sEndsWith(const string& iTarget, const string& iPossibleSuffix)
	{ return true && sQWithoutSuffix(iTarget, iPossibleSuffix); }

// =================================================================================================

const char* const sStandardWSCharacters = " \t\n\r\f\v";

string sTrimmedL(const string& iString, const char* iWS)
	{
	const size_t start = iString.find_first_not_of(iWS);
	if (start == string::npos)
		return string();
	return iString.substr(start);
	}

string sTrimmedR(const string& iString, const char* iWS)
	{
	const size_t end = iString.find_last_not_of(iWS);
	if (end == string::npos)
		return string();
	return iString.substr(0, end + 1);
	}

string sTrimmed(const string& iString, const char* iWS)
	{
	const size_t first = iString.find_first_not_of(iWS);
	if (first == string::npos)
		return string();

	// end cannot be npos, because we already found a not_of.
	const size_t last = iString.find_last_not_of(iWS);
	return iString.substr(first, last + 1 - first);
	}

// =================================================================================================

string sFromPString(const unsigned char* inPString)
	{ return string((const char*)(&(inPString[1])), (size_t)inPString[0]); }

void sToPString(const string& inString, unsigned char* outPString, size_t inMaxLength)
	{
	size_t sourceLength = min(inString.size(), inMaxLength);
	if (sourceLength)
		sMemCopy(&(outPString[1]), inString.data(), sourceLength);
	outPString[0] = sourceLength;
	}

void sToPString(const char* inString, unsigned char* outPString, size_t inMaxLength)
	{
	size_t sourceLength = min(strlen(inString), inMaxLength);
	if (sourceLength)
		sMemCopy(&(outPString[1]), inString, sourceLength);
	outPString[0] = sourceLength;
	}

// =================================================================================================

vector<string> sSplit(char iChar, const string& iString)
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

} // namespace Util_string
} // namespace ZooLib
