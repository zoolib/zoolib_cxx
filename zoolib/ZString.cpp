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

#include "zoolib/ZString.h"

#include "zoolib/ZCompat_algorithm.h" // For min
#include "zoolib/ZDebug.h"
#include "zoolib/ZStream.h"
#include "zoolib/ZMemory.h" // For ZBlockCopy
#include "zoolib/ZUnicode.h"

#include <ctype.h>
#include <stdio.h>
#include <stdexcept>
#include <string>

using std::min;
using std::runtime_error;
using std::string;

#if ZCONFIG(Compiler, MSVC)
#	define vsnprintf _vsnprintf
#endif

#ifndef va_copy
#	define va_copy(dest, src) dest = src
#endif

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================

string ZString::sFormat(const char* iString, ...)
	{
	va_list args;
	va_start(args, iString);
	string result = sFormat(iString, args);
	va_end(args);

	return result;
	}

std::string ZString::sFormat(const char* iString, va_list iArgs)
	{
	string result(512, ' ');
	while (true)
		{
		va_list localArgs;
		va_copy(localArgs, iArgs);

		int count = vsnprintf(const_cast<char*>(result.data()), result.size(), iString, localArgs);

		va_end(localArgs);

		if (count < 0)
			{
			// Handle -ve result from glibc prior to version 2.1 by growing the string.
			result.resize(result.size() * 2);
			}
		else if (count > result.size())
			{
			// Handle C99 standard, where count indicates how much space would have been needed.
			result.resize(count);
			}
		else
			{
			// The string fitted
			result.resize(count);
			break;
			}
		}

	return result;
	}

// =================================================================================================

string ZString::sSubstitute(const string& iString,
	const string& iS0)
	{
	return sSubstitute(iString, &iS0, 1);
	}

string ZString::sSubstitute(const string& iString,
	const string& iS0,
	const string& iS1)
	{
	string theParams[2];
	theParams[0] = iS0;
	theParams[1] = iS1;
	return sSubstitute(iString, theParams, 2);
	}

string ZString::sSubstitute(const string& iString,
	const string& iS0,
	const string& iS1,
	const string& iS2)
	{
	string theParams[4];
	theParams[0] = iS0;
	theParams[1] = iS1;
	theParams[2] = iS2;
	return sSubstitute(iString, theParams, 3);
	}

string ZString::sSubstitute(const string& iString,
	const string& iS0,
	const string& iS1,
	const string& iS2,
	const string& iS3)
	{
	string theParams[4];
	theParams[0] = iS0;
	theParams[1] = iS1;
	theParams[2] = iS2;
	theParams[3] = iS3;
	return sSubstitute(iString, theParams, 4);
	}

string ZString::sSubstitute(const string& iString,
	const string& iS0,
	const string& iS1,
	const string& iS2,
	const string& iS3,
	const string& iS4)
	{
	string theParams[5];
	theParams[0] = iS0;
	theParams[1] = iS1;
	theParams[2] = iS2;
	theParams[3] = iS3;
	theParams[4] = iS4;
	return sSubstitute(iString, theParams, 5);
	}


/// Return iString with occurrences of '%' followed by [0-9] replaced by strings from iParams.
/** A '%' followed by any character other than [0-9] will cause the following character to
appear in the result, so '%%' will appear as '%' in the result. If a substitution is indicated but
iParams/iParamsCount does not have the required string then an empty string is used.
*/
string ZString::sSubstitute(const string& iString, const string* iParams, size_t iParamsCount)
	{
	string result;
	string::size_type startPos = 0;
	while (true)
		{
		string::size_type nextPos = iString.find('%', startPos);
		if (nextPos == string::npos)
			{
			if (startPos == 0)
				{
				// We found no %'s at all, so we can simply return iString unchanged.
				return iString;
				}
			result += iString.substr(startPos, string::npos);
			break;
			}

		result += iString.substr(startPos, nextPos - startPos);

		// In the following comparison we add to pos rather than subtracting from
		// iString.size() because of the possibility of unsigned wraparound.
		if (nextPos + 2 > iString.size())
			break;

		char theFollowingChar = iString[nextPos + 1];
		if (theFollowingChar >= '0' && theFollowingChar <= '9')
			{
			if (theFollowingChar - '0' < iParamsCount)
				result += iParams[theFollowingChar - '0'];
			}
		else
			{
			result += theFollowingChar;
			}
		startPos = nextPos + 2;
		}
	return result;
	}

int ZString::sComparei(const string& iLeft, const string& iRight)
	{
	size_t sizeLeft = iLeft.size();
	size_t sizeRight = iRight.size();
	size_t sizeMin = min(sizeLeft, sizeRight);
	for (size_t x = 0; x < sizeMin; ++x)
		{
		if (int result = int(toupper(iLeft[x])) - int(toupper(iRight[x])))
			return result;
		}
	if (sizeLeft < sizeRight)
		return -1;
	if (sizeRight < sizeLeft)
		return 1;
	return 0;
	}

bool ZString::sEquali(const string& iLeft, const string& iRight)
	{ return 0 == sComparei(iLeft, iRight); }

bool ZString::sContainsi(const string& iTarget, const string& iCandidate)
	{
	return std::string::npos != ZUnicode::sToLower(iTarget).find(ZUnicode::sToLower(iCandidate));
	}

string ZString::sMacizeString(const string& iString)
	{
	string localString(iString);
	ZString::sMakeFresh(localString);
	char* thePtr = const_cast<char*>(localString.data());
	size_t count = localString.size();
	bool insideDoubleQuote = false;

	// We don't do curly single quotes -- the logic is a bit fiddly
	while (count--)
		{
		switch (*thePtr)
			{
			case '\n':
				*thePtr = '\r';
				break;
			case '\"':
				if (insideDoubleQuote)
					{
					*thePtr = 0xD3;
					insideDoubleQuote = false;
					}
				else
					{
					*thePtr = 0xD4;
					insideDoubleQuote = true;
					}
				break;
			case '\'':
				*thePtr = 0xD5;
				break;
			}
		++thePtr;
		}
	return localString;
	}

string ZString::sFromInt(int inInt)
	{
	char temp[21]; // 21 chars will work even for -ve 64 bit ints
	sprintf(temp, "%d", inInt);
	return temp;
	}

int ZString::sAsInt(const string& iString)
	{
	int theInt = 0;
	if (iString.size())
		{
		if (sscanf(iString.c_str(), "%d", &theInt) <= 0)
			theInt = 0;
		}
	return theInt;
	}

string ZString::sHexFromInt(int inInt)
	{
	char temp[16];
	sprintf(temp, "%X", inInt);
	return temp;
	}

string ZString::sHexFromUInt64(uint64 inUInt64)
	{
	char temp[21];
	sprintf(temp, "%llX", inUInt64);
	return temp;
	}

string ZString::sFromUInt64(uint64 inUInt64)
	{
	char temp[21];
	sprintf(temp, "%lld", inUInt64);
	return temp;
	}

uint64 ZString::sAsUInt64(const string& inString)
	{
	uint64 theUInt64 = 0;
	if (inString.size())
		{
		if (sscanf(inString.c_str(), "%lld", &theUInt64) <= 0)
			theUInt64 = 0;
		}
	return theUInt64;
	}

void ZString::sToStream(const string& inString, const ZStreamW& inStream)
	{
	size_t theLength = inString.size();
	inStream.WriteUInt32(theLength);
	if (theLength > 0)
		inStream.Write(inString.data(), theLength);
	}

void ZString::sFromStream(string& outString, const ZStreamR& inStream)
	{
	size_t theLength = inStream.ReadUInt32();
	if (theLength > 200000)
		throw runtime_error("ZString::FromStream. invalid length");
	outString = string(theLength, 0);
	if (theLength > 0)
		inStream.Read(const_cast<char*>(outString.data()), theLength);
	}

string ZString::sFromStream(const ZStreamR& iStream)
	{
	string theString;
	sFromStream(theString, iStream);
	return theString;
	}

string ZString::sFresh(const string& inOther)
	{
	if (inOther.empty())
		return string();
	return string(inOther.data(), inOther.size());
	}

void ZString::sMakeFresh(string& inOutString)
	{
	if (inOutString.empty())
		inOutString = string();
	else
		inOutString = string(inOutString.data(), inOutString.size());
	}

// =================================================================================================

static const long sStringCacheCount = 8;
static long sStringCacheCurrent;

typedef unsigned char ZStr255[256];
static ZStr255 sStringCache[sStringCacheCount];

const unsigned char* ZString::sAsPString(const string& inString)
	{
	size_t theLength = min(inString.size(), size_t(255));
	sStringCacheCurrent = (sStringCacheCurrent +1) % sStringCacheCount;
	if (theLength)
		ZBlockCopy(inString.data(), &(sStringCache[sStringCacheCurrent][1]), theLength);
	sStringCache[sStringCacheCurrent][0] = theLength;
	return sStringCache[sStringCacheCurrent];
	}

string ZString::sFromPString(const unsigned char* inPString)
	{ return string((const char*)(&(inPString[1])), (long)inPString[0]); }

void ZString::sToPString(const string& inString, unsigned char* outPString, size_t inMaxLength)
	{
	size_t sourceLength = min(inString.size(), inMaxLength);
	if (sourceLength)
		ZBlockCopy(inString.data(), &(outPString[1]), sourceLength);
	outPString[0] = sourceLength;
	}

void ZString::sToPString(const char* inString, unsigned char* outPString, size_t inMaxLength)
	{
	size_t sourceLength = min(strlen(inString), inMaxLength);
	if (sourceLength)
		ZBlockCopy(inString, &(outPString[1]), sourceLength);
	outPString[0] = sourceLength;
	}

NAMESPACE_ZOOLIB_END
