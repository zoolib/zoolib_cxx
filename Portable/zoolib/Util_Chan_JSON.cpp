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

#include "zoolib/Util_Chan_JSON.h"

#include "zoolib/Any_Coerce.h"
#include "zoolib/Chan_Bin_ASCIIStrim.h"
#include "zoolib/Chan_Bin_Base64.h"
#include "zoolib/Chan_UTF_Escaped.h"
#include "zoolib/ChanW_Bin_HexStrim.h"
#include "zoolib/Compat_cmath.h" // For fmod
#include "zoolib/Unicode.h"
#include "zoolib/UTCDateTime.h"
#include "zoolib/Util_Chan_UTF.h"
#include "zoolib/Util_Chan_UTF_Operators.h"
#include "zoolib/Util_Time.h"

#include <vector>

#if ZCONFIG(Compiler,GCC) || ZCONFIG(Compiler,Clang)
	#include <cxxabi.h>
	#include <stdlib.h> // For free, required by __cxa_demangle
#endif

namespace ZooLib {
namespace Util_Chan_JSON {

// =================================================================================================
#pragma mark -
#pragma mark ReadOptions

ReadOptions sReadOptions_Extended()
	{
	ReadOptions theRO;
	theRO.fAllowUnquotedPropertyNames = true;
	theRO.fAllowEquals = true;
	theRO.fAllowSemiColons = true;
	theRO.fAllowTerminators = true;
	theRO.fLooseSeparators = true;
	theRO.fAllowBinary = true;

	return theRO;
	}

// =================================================================================================
#pragma mark -

string sPrettyName(const std::type_info& iTI)
	{
	#if ZCONFIG(Compiler,GCC) || ZCONFIG(Compiler,Clang)
	if (char* unmangled = abi::__cxa_demangle(iTI.name(), 0, 0, 0))
		{
		string result = unmangled;
		free(unmangled);
		return result;
		}
	#endif

	return iTI.name();
	}

bool sTryRead_Identifier(const ChanR_UTF& iChanR, const ChanU_UTF& iChanU,
	string* oStringLC, string* oStringExact)
	{
	if (oStringExact)
		oStringExact->reserve(32);

	if (oStringLC)
		oStringLC->reserve(32);

	bool gotAny = false;
	for (;;)
		{
		const ZQ<UTF32> theCPQ = sQRead(iChanR);
		if (not theCPQ)
			break;
		const UTF32 theCP = *theCPQ;
		if (not Unicode::sIsAlphaDigit(theCP) && theCP != '_')
			{
			sUnread(iChanU, theCP);
			break;
			}

		gotAny = true;

		if (oStringLC)
			*oStringLC += Unicode::sToLower(theCP);
		if (oStringExact)
			*oStringExact += theCP;
		}
	return gotAny;
	}

bool sTryRead_PropertyName(const ChanR_UTF& iChanR, const ChanU_UTF& iChanU,
	string& oName, bool iAllowUnquoted)
	{
	using namespace Util_Chan;

	if (sTryRead_EscapedString('"', iChanR, iChanU, oName))
		return true;

	if (sTryRead_EscapedString('\'', iChanR, iChanU, oName))
		return true;

	if (iAllowUnquoted && sTryRead_Identifier(iChanR, iChanU, nullptr, &oName))
		return true;

	return false;
	}

bool sTryRead_JSONString(const ChanR_UTF& iChanR, const ChanU_UTF& iChanU,
	string& oString)
	{
	using namespace Util_Chan;

	if (sTryRead_CP('"', iChanR, iChanU))
		{
		// We've got a string, delimited by ".
		for (;;)
			{
			string tempString;
			sRead_EscapedString('"', iChanR, iChanU, tempString);

			if (not sTryRead_CP('"', iChanR, iChanU))
				throw ParseException("Expected '\"' to close a string");

			oString += tempString;

			sSkip_WSAndCPlusPlusComments(iChanR, iChanU);

			if (not sTryRead_CP('"', iChanR, iChanU))
				return true;
			}
		}
	else if (sTryRead_CP('\'', iChanR, iChanU))
		{
		// We've got a string, delimited by '.
		for (;;)
			{
			string tempString;
			sRead_EscapedString( '\'', iChanR, iChanU, tempString);

			if (not sTryRead_CP('\'', iChanR, iChanU))
				throw ParseException("Expected \"'\" to close a string");

			oString += tempString;

			sSkip_WSAndCPlusPlusComments(iChanR, iChanU);

			if (not sTryRead_CP('\'', iChanR, iChanU))
				return true;
			}
		}

	return false;
	}

// =================================================================================================
#pragma mark -
#pragma mark WriteOptions

WriteOptions::WriteOptions()
:	YadOptions()
	{}

WriteOptions::WriteOptions(const YadOptions& iOther)
:	YadOptions(iOther)
	{}

WriteOptions::WriteOptions(const WriteOptions& iOther)
:	YadOptions(iOther)
,	fUseExtendedNotation(iOther.fUseExtendedNotation)
,	fBinaryAsBase64(iOther.fBinaryAsBase64)
,	fPreferSingleQuotes(iOther.fPreferSingleQuotes)
,	fNumberSequences(iOther.fNumberSequences)
	{}

// =================================================================================================
#pragma mark -

void sWriteIndent(size_t iCount, const WriteOptions& iOptions, const ChanW_UTF& iChanW)
	{
	while (iCount--)
		iChanW << sIndentString(iOptions);
	}

void sWriteLFIndent(size_t iCount, const WriteOptions& iOptions, const ChanW_UTF& iChanW)
	{
	iChanW << sEOLString(iOptions);
	sWriteIndent(iCount, iOptions, iChanW);
	}

void sWriteString(const string& iString, bool iPreferSingleQuotes, const ChanW_UTF& iChanW)
	{
	ChanW_UTF_Escaped::Options theOptions;
	theOptions.fEscapeHighUnicode = false;

	if (iPreferSingleQuotes)
		{
		iChanW << "'";

		theOptions.fQuoteQuotes = false;

		ChanW_UTF_Escaped(theOptions, iChanW) << iString;

		iChanW << "'";
		}
	else
		{
		iChanW << "\"";

		theOptions.fQuoteQuotes = true;

		ChanW_UTF_Escaped(theOptions, iChanW) << iString;

		iChanW << "\"";
		}
	}

void sWriteString(const ChanR_UTF& iChanR, const ChanW_UTF& iChanW)
	{
	iChanW << "\"";

	ChanW_UTF_Escaped::Options theOptions;
	theOptions.fQuoteQuotes = true;
	theOptions.fEscapeHighUnicode = false;

	ChanW_UTF_Escaped(theOptions, iChanW) << iChanR;

	iChanW << "\"";
	}

bool sContainsProblemChars(const string& iString)
	{
	if (iString.empty())
		{
		// An empty string can't be distinguished from no string at all, so
		// we treat it as if it has problem chars so that it will be wrapped in quotes.
		return true;
		}

	for (string::const_iterator ii = iString.begin(), end = iString.end();;)
		{
		UTF32 theCP;
		if (not Unicode::sReadInc(ii, end, theCP))
			break;
		if (not Unicode::sIsAlphaDigit(theCP) && '_' != theCP)
			return true;
		}

	return false;
	}

void sWritePropName(const string& iString, bool iUseSingleQuotes, const ChanW_UTF& w)
	{
	if (sContainsProblemChars(iString))
		sWriteString(iString, iUseSingleQuotes, w);
	else
		w << iString;
	}

void sWriteSimpleValue(const Any& iAny, const WriteOptions& iOptions, const ChanW_UTF& w)
	{
	if (false)
		{}
	else if (iAny.IsNull())
		{
		w << "null";
		}
	else if (const bool* theValue = iAny.PGet<bool>())
		{
		if (*theValue)
			w << "true";
		else
			w << "false";
		}
	else if (ZQ<int64> theQ = sQCoerceInt(iAny))
		{
		if (iOptions.fUseExtendedNotation.DGet(false) and (*theQ >= 1000000 || *theQ <= -1000000))
			sEWritef(w, "0x%016llX", (unsigned long long)*theQ);
		else
			w << *theQ;
		}
	else if (const float* asFloat = iAny.PGet<float>())
		{
		Util_Chan::sWriteExact(w, *asFloat);
		}
	else if (const double* asDouble = iAny.PGet<double>())
		{
		Util_Chan::sWriteExact(w, *asDouble);
		if (not fmod(*asDouble, 1.0))
			{
			// There's no fractional part, so we'll have written a sequence of digits with no
			// decimal point. So the number would likely be subsequently interpreted as an
			// integer, so we append a ".0".
			w << ".0";
			}
		}
	else if (const UTCDateTime* asTime = iAny.PGet<UTCDateTime>())
		{
		Util_Chan::sWriteExact(w, sGet(*asTime));
		if (sTimesHaveUserLegibleComment(iOptions))
			w << " /* " << Util_Time::sAsString_ISO8601_us(sGet(*asTime), true) << " */ ";
		}
	else
		{
		w << "NULL" << " /*!! Unhandled: " << sPrettyName(iAny.Type()) << " !!*/";
		}
	}

void sToStrim_Stream(const ChanR_Bin& iChanR,
	size_t iLevel, const WriteOptions& iOptions, bool iMayNeedInitialLF,
	const ChanW_UTF& w)
	{
	string chunkSeparator;
	size_t chunkSize = 0;
	if (sDoIndentation(iOptions))
		{
		chunkSeparator = sEOLString(iOptions);
		for (size_t xx = 0; xx < iLevel; ++xx)
			chunkSeparator += sIndentString(iOptions);

		chunkSize = sRawChunkSize(iOptions);
		}

	if (iOptions.fBinaryAsBase64.DGet(false))
		{
		w << "<";
		w << "=";

		sCopyAll(iChanR,
			ChanW_Bin_Base64Encode(
				Base64::sEncode_Normal(),
				ChanW_Bin_ASCIIStrim(ChanW_UTF_InsertSeparator(chunkSize * 3, chunkSeparator, w))
				)
			);

		w << ">";
		}
	else if (sRawAsASCII(iOptions) && chunkSize)
		{
		w << "<";
		std::vector<char> buffer(chunkSize, 0);
		for (;;)
			{
			const size_t countRead = sReadMem(iChanR, &buffer[0], chunkSize);
			if (not countRead)
				break;
			const size_t countCopied = sWriteMemFully(
				ChanW_Bin_HexStrim(sRawByteSeparator(iOptions), "", 0, w),
				&buffer[0], countRead);

			if (size_t extraSpaces = chunkSize - countCopied)
				{
				// We didn't write a complete line of bytes, so pad it out.
				while (extraSpaces--)
					{
					// Two saces for the two nibbles
					w << "  ";
					// And then the separator sequence
					w << sRawByteSeparator(iOptions);
					}
				}

			w << " // ";
			for (size_t xx = 0; xx < countCopied; ++xx)
				{
				char theChar = buffer[xx];
				if (theChar < 0x20 || theChar > 0x7E)
					w << ".";
				else
					w << theChar;
				}
			sWriteLFIndent(iLevel, iOptions, w);
			}
		w << ">";
		}
	else
		{
		w << "<";

		sCopyAll(iChanR,
			ChanW_Bin_HexStrim(sRawByteSeparator(iOptions), chunkSeparator, chunkSize, w));

		w << ">";

		}
	}


} // namespace Util_Chan_JSON
} // namespace ZooLib
