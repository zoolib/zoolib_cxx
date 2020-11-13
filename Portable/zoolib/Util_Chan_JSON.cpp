// Copyright (c) 2014 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/Util_Chan_JSON.h"

#include "zoolib/Chan_Bin_ASCIIStrim.h"
#include "zoolib/Chan_Bin_Base64.h"
#include "zoolib/Chan_UTF_Escaped.h"
#include "zoolib/ChanW_Bin_HexStrim.h"
#include "zoolib/Coerce_Any.h"
#include "zoolib/Compat_cmath.h" // For fmod
#include "zoolib/ParseException.h"
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
#pragma mark - PullTextOptions_JSON

PullTextOptions_JSON sPullTextOptions_Extended()
	{
	PullTextOptions_JSON theOptions;
	theOptions.fAllowUnquotedPropertyNames = true;
	theOptions.fAllowEquals = true;
	theOptions.fAllowSemiColons = true;
	theOptions.fAllowTerminators = true;
	theOptions.fLooseSeparators = true;
	theOptions.fAllowBinary = true;

	return theOptions;
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

bool sTryRead_Identifier(const ChanRU_UTF& iChanRU,
	string* oStringLC, string* oStringExact)
	{
	if (oStringExact)
		oStringExact->reserve(32);

	if (oStringLC)
		oStringLC->reserve(32);

	bool gotAny = false;
	for (;;)
		{
		const ZQ<UTF32> theCPQ = sQRead(iChanRU);
		if (not theCPQ)
			break;
		const UTF32 theCP = *theCPQ;
		if (not Unicode::sIsAlphaDigit(theCP) && theCP != '_')
			{
			sUnread(iChanRU, theCP);
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

bool sTryRead_PropertyName(const ChanRU_UTF& iChanRU,
	string& oName, bool iAllowUnquoted)
	{
	using namespace Util_Chan;

	if (sTryRead_EscapedString('"', iChanRU, oName))
		return true;

	if (sTryRead_EscapedString('\'', iChanRU, oName))
		return true;

	if (iAllowUnquoted && sTryRead_Identifier(iChanRU, nullptr, &oName))
		return true;

	return false;
	}

bool sTryRead_JSONString(const ChanRU_UTF& iChanRU,
	string& oString)
	{
	using namespace Util_Chan;

	if (sTryRead_CP('"', iChanRU))
		{
		// We've got a string, delimited by ".
		for (;;)
			{
			string tempString;
			sRead_EscapedString('"', iChanRU, tempString);

			if (not sTryRead_CP('"', iChanRU))
				sThrow_ParseException("Expected '\"' to close a string");

			oString += tempString;

			sSkip_WSAndCPlusPlusComments(iChanRU);

			if (not sTryRead_CP('"', iChanRU))
				return true;
			}
		}
	else if (sTryRead_CP('\'', iChanRU))
		{
		// We've got a string, delimited by '.
		for (;;)
			{
			string tempString;
			sRead_EscapedString('\'', iChanRU, tempString);

			if (not sTryRead_CP('\'', iChanRU))
				sThrow_ParseException("Expected \"'\" to close a string");

			oString += tempString;

			sSkip_WSAndCPlusPlusComments(iChanRU);

			if (not sTryRead_CP('\'', iChanRU))
				return true;
			}
		}

	return false;
	}

// =================================================================================================
#pragma mark - PushTextOptions_JSON

PushTextOptions_JSON::PushTextOptions_JSON()
:	PushTextOptions()
	{}

PushTextOptions_JSON::PushTextOptions_JSON(const PushTextOptions& iOther)
:	PushTextOptions(iOther)
	{}

PushTextOptions_JSON::PushTextOptions_JSON(const PushTextOptions_JSON& iOther)
:	PushTextOptions(iOther)
,	fUseExtendedNotationQ(iOther.fUseExtendedNotationQ)
,	fBinaryAsBase64Q(iOther.fBinaryAsBase64Q)
,	fPreferSingleQuotesQ(iOther.fPreferSingleQuotesQ)
,	fNumberSequencesQ(iOther.fNumberSequencesQ)
,	fLowercaseHexQ(iOther.fLowercaseHexQ)
,	fIntegersAsHexQ(iOther.fIntegersAsHexQ)
	{}

PushTextOptions_JSON& PushTextOptions_JSON::operator=(const PushTextOptions& iOther)
	{
	PushTextOptions::operator=(iOther);
	return *this;
	}

// =================================================================================================
#pragma mark -

void sWriteLF(const ChanW_UTF& iChanW, const PushTextOptions& iOptions)
	{
	iChanW << sEOLString(iOptions);
	}

void sWriteIndent(const ChanW_UTF& iChanW, size_t iCount, const PushTextOptions& iOptions)
	{
	while (iCount--)
		iChanW << sIndentString(iOptions);
	}

void sWriteLFIndent(const ChanW_UTF& iChanW, size_t iCount, const PushTextOptions& iOptions)
	{
	iChanW << sEOLString(iOptions);
	sWriteIndent(iChanW, iCount, iOptions);
	}

void sWriteString(const ChanW_UTF& iChanW, const string& iString, bool iPreferSingleQuotes)
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

void sWriteString(const ChanW_UTF& iChanW, const ChanR_UTF& iChanR)
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

void sWritePropName(const ChanW_UTF& ww, const string& iString, bool iUseSingleQuotes)
	{
	if (sContainsProblemChars(iString))
		sWriteString(ww, iString, iUseSingleQuotes);
	else
		ww << iString;
	}

void sWriteSimpleValue(const ChanW_UTF& ww, const AnyBase& iAny, const PushTextOptions_JSON& iOptions)
	{
	if (false)
		{}
	else if (iAny.IsNull())
		{
		ww << "null";
		}
	else if (const bool* theValue = iAny.PGet<bool>())
		{
		if (*theValue)
			ww << "true";
		else
			ww << "false";
		}
	else if (ZQ<int64> theQ = sQCoerceInt(iAny))
		{
		if (iOptions.fIntegersAsHexQ.Get())
			{
			if (iOptions.fLowercaseHexQ.Get())
				sEWritef(ww, "0x%016llx", (unsigned long long)*theQ);
			else
				sEWritef(ww, "0x%016llX", (unsigned long long)*theQ);
			}
		else
			{
			ww << *theQ;
			}
		}
	else if (const float* asFloat = iAny.PGet<float>())
		{
		Util_Chan::sWriteExact(ww, *asFloat);
		}
	else if (const double* asDouble = iAny.PGet<double>())
		{
		Util_Chan::sWriteExact(ww, *asDouble);
		if (not fmod(*asDouble, 1.0))
			{
			// There's no fractional part, so we'll have written a sequence of digits with no
			// decimal point. So the number would likely be subsequently interpreted as an
			// integer, so we append a ".0".
			ww << ".0";
			}
		}
	else if (const UTCDateTime* asTime = iAny.PGet<UTCDateTime>())
		{
		Util_Chan::sWriteExact(ww, sGet(*asTime));
		if (sTimesHaveUserLegibleComment(iOptions))
			ww << " /* " << Util_Time::sAsString_ISO8601_us(sGet(*asTime), true) << " */ ";
		}
	else
		{
		ww << "NULL" << " /*!! Unhandled: " << sPrettyName(iAny.Type()) << " !!*/";
		}
	}

void sPull_Bin_Push_JSON(const ChanR_Bin& iChanR,
	size_t iLevel, const PushTextOptions_JSON& iOptions,
	const ChanW_UTF& ww)
	{
	string chunkSeparator;
	size_t chunkSize = 0;
	if (sDoIndentation(iOptions) && not iOptions.fIndentOnlySequencesQ.Get())
		{
		chunkSeparator = sEOLString(iOptions);
		for (size_t xx = 0; xx < iLevel; ++xx)
			chunkSeparator += sIndentString(iOptions);

		chunkSize = sRawChunkSize(iOptions);
		}

	if (iOptions.fBinaryAsBase64Q.DGet(false))
		{
		ww << "<";
		ww << "=";

		sCopyAll(iChanR,
			ChanW_Bin_Base64Encode(
				Base64::sEncode_Normal(),
				ChanW_Bin_ASCIIStrim(ChanW_UTF_InsertSeparator(chunkSize * 3, chunkSeparator, ww))
				)
			);

		ww << ">";
		}
	else if (sRawAsASCII(iOptions) && chunkSize)
		{
		std::vector<char> buffer(chunkSize, 0);
		if (size_t countRead = sReadMem(iChanR, &buffer[0], chunkSize))
			{
			ww << "<";
			if (countRead == chunkSize)
				sWriteLFIndent(ww, iLevel, iOptions);

			std::vector<char> nextBuffer(chunkSize, 0);
			for (;;)
				{
				const size_t countCopied = sWriteMemFully(
					ChanW_Bin_HexStrim(sRawByteSeparator(iOptions), "", 0, iOptions.fLowercaseHexQ.Get(), ww),
					&buffer[0], countRead);

				countRead = sReadMem(iChanR, &nextBuffer[0], chunkSize);

				if (size_t extraSpaces = chunkSize - countCopied)
					{
					// We didn't write a complete line of bytes.
					if (countRead == 0)
						{
						// And it was the end. Emit the close and a space (matching a pair of nibbles).
						ww << sRawByteSeparator(iOptions);
						ww << "> ";
						--extraSpaces;
						}

					while (extraSpaces--)
						{
						// Separator sequence
						ww << sRawByteSeparator(iOptions);
						// Two spaces for the two nibbles
						ww << "  ";
						}
					ww << " /* ";
					}
				else if (countRead == 0)
					{
					// We'd exactly consumed the data. Emit a close tight up against the data.
					ww << ">/* ";
					}
				else
					{
					ww << " /* ";
					}
				for (size_t xx = 0; xx < countCopied; ++xx)
					{
					char theChar = buffer[xx];
					if (theChar < 0x20 || theChar > 0x7E)
						ww << ".";
					else
						ww << theChar;
					}
				ww << " */ ";
				if (countRead == 0)
					break;
				sWriteLFIndent(ww, iLevel, iOptions);
				swap(buffer, nextBuffer);
				}
			}
		else
			{
			ww << "<>";
			}
		}
	else
		{
		ww << "<";

		sCopyAll(iChanR,
			ChanW_Bin_HexStrim(sRawByteSeparator(iOptions), chunkSeparator, chunkSize, iOptions.fLowercaseHexQ.Get(), ww));

		ww << ">";

		}
	}

ZQ<string8> sQRead_PropName(const ChanRU_UTF& iChanRU)
	{
	string8 theString8;
	if (not sTryRead_PropertyName(iChanRU, theString8, true))
		return null;
	return theString8;
	}

void sWrite_PropName(const ChanW_UTF& ww, const string& iPropName)
	{
	if (sContainsProblemChars(iPropName))
		{
		ww << "\"";
		ChanW_UTF_Escaped(ww) << iPropName;
		ww << "\"";
		}
	else
		{
		ww << iPropName;
		}
	}

} // namespace Util_Chan_JSON
} // namespace ZooLib
