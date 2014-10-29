/* -------------------------------------------------------------------------------------------------
Copyright (c) 2009 Andrew Green
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

#include "zoolib/Unicode.h"
#include "zoolib/Util_Chan_UTF_Operators.h"

#include "zoolib/ZCompat_algorithm.h" // ZSetRestore_T
#include "zoolib/ZCompat_cmath.h"
#include "zoolib/ZStreamW_HexStrim.h"
#include "zoolib/ZStrim_Escaped.h"
#include "zoolib/ZTime.h"
#include "zoolib/ZUtil_Any.h"
#include "zoolib/ZUtil_Strim.h"
#include "zoolib/ZYad_JSON.h"

#if ZCONFIG(Compiler,GCC)
	#include <cxxabi.h>
	#include <stdlib.h> // For free
#endif

namespace ZooLib {
namespace ZYad_JSON {

using std::min;
using std::string;

/*! \namespace ZooLib::ZYad_JSON
JSON is JavaScript Object Notation. See <http://www.crockford.com/JSON/index.html>.

ZYad_JSON provides Yad facilities to read and write JSON source.

ZMap is isomorphic to JSON's object, ZSeq to JSON's array, and strings, booleans
and null translate back and forth without trouble. JSON's only other primitive is
the number, whereas ZVal explicitly stores and retrieves integers of different sizes,
floats and doubles, raw bytes and other composite types.

ZYad_JSON writes all ZVal number types as JSON numbers. When reading, JSON numbers
are stored as int64s, unless the mantissa has a fractional component or exceeds 2^64,
or if there is an exponent,in which case a double is used.

The mappings are as follows:

<pre>

JSON          ZVal
----          -------
null          null
object        ZMap
array         ZSeq
boolean       bool
string        string
number        int64 or double


ZVal         JSON
----         ----
null         null
ZMap         object
ZSeq         array
bool         boolean
string       string
int8         number
int16        number
int32        number
int64        number
float        number
double       number
uint64 (ID)  number
time         number

all other types are written as nulls in maps and lists if the appropriate
preserve flag is passed, otherwise they are skipped.
raw              -
refcounted       -
rect             -
point            -
region           -
type             -
</pre>

*/

// =================================================================================================
// MARK: - Helpers

static string spPrettyName(const std::type_info& iTI)
	{
	#if ZCONFIG(Compiler,GCC)
	if (char* unmangled = abi::__cxa_demangle(iTI.name(), 0, 0, 0))
		{
		string result = unmangled;
		free(unmangled);
		return result;
		}
	#endif

	return iTI.name();
	}

static void spThrowParseException(const string& iMessage)
	{
	throw ParseException(iMessage);
	}

static bool spRead_Identifier(
	const ZStrimU& iStrimU, string* oStringLC, string* oStringExact)
	{
	if (oStringExact)
		oStringExact->reserve(32);

	if (oStringLC)
		oStringLC->reserve(32);

	bool gotAny = false;
	for (;;)
		{
		UTF32 theCP;
		if (not iStrimU.ReadCP(theCP))
			break;
		if (not Unicode::sIsAlphaDigit(theCP) && theCP != '_')
			{
			iStrimU.Unread(theCP);
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

static bool spTryRead_JSONString(const ZStrimU& s, string& oString)
	{
	using namespace ZUtil_Strim;

	if (sTryRead_CP(s, '"'))
		{
		// We've got a string, delimited by ".
		for (;;)
			{
			string tempString;
			sRead_EscapedString(s, '"', tempString);

			if (not sTryRead_CP(s, '"'))
				throw ParseException("Expected '\"' to close a string");

			oString += tempString;

			sSkip_WSAndCPlusPlusComments(s);

			if (not sTryRead_CP(s, '"'))
				return true;
			}
		}
	else if (sTryRead_CP(s, '\''))
		{
		// We've got a string, delimited by '.
		for (;;)
			{
			string tempString;
			sRead_EscapedString(s, '\'', tempString);

			if (not sTryRead_CP(s, '\''))
				throw ParseException("Expected \"'\" to close a string");

			oString += tempString;

			sSkip_WSAndCPlusPlusComments(s);

			if (not sTryRead_CP(s, '\''))
				return true;
			}
		}

	return false;
	}

static bool spTryRead_PropertyName(const ZStrimU& iStrimU, string& oName, bool iAllowUnquoted)
	{
	using namespace ZUtil_Strim;

	if (sTryRead_EscapedString(iStrimU, '"', oName))
		return true;

	if (sTryRead_EscapedString(iStrimU, '\'', oName))
		return true;

	if (iAllowUnquoted && spRead_Identifier(iStrimU, nullptr, &oName))
		return true;

	return false;
	}

static bool spFromStrim_Value(const ZStrimU& iStrimU, ZAny& oVal)
	{
	using namespace ZUtil_Strim;

	string theString;

	int64 asInt64;
	double asDouble;
	bool isDouble;

	if (spTryRead_JSONString(iStrimU, theString))
		{
		oVal = theString;
		}
	else if (sTryRead_SignedDecimalNumber(iStrimU, asInt64, asDouble, isDouble))
		{
		if (isDouble)
			oVal = asDouble;
		else
			oVal = asInt64;
		}
	else if (sTryRead_CaselessString(iStrimU, "null"))
		{
		oVal = null; //## Watch this
		}
	else if (sTryRead_CaselessString(iStrimU, "false"))
		{
		oVal = false;
		}
	else if (sTryRead_CaselessString(iStrimU, "true"))
		{
		oVal = true;
		}
	else
		{
		spThrowParseException("Expected number, string or keyword");
		}

	return true;
	}

static ZRef<ZYadR> spMakeYadR_JSON(
	ZRef<ZStrimmerU> iStrimmerU, const ZRef<CountedVal<ReadOptions> >& iRO)
	{
	using namespace ZUtil_Strim;

	const ZStrimU& theStrimU = iStrimmerU->GetStrimU();

	sSkip_WSAndCPlusPlusComments(theStrimU);

	if (sTryRead_CP(theStrimU, '['))
		{
		return new YadSeqR(iStrimmerU, iRO);
		}
	else if (sTryRead_CP(theStrimU, '{'))
		{
		return new YadMapR(iStrimmerU, iRO);
		}
	else if (sTryRead_CP(theStrimU, '"'))
		{
		return new YadStrimmerR(iStrimmerU);
		}
	else if (iRO->Get().fAllowBinary.DGet(false) && sTryRead_CP(theStrimU, '('))
		{
		sSkip_WSAndCPlusPlusComments(theStrimU);
		if (sTryRead_CP(theStrimU, '='))
			{
			// It's Base64
			return new YadStreamerR_Base64(Base64::sDecode_Normal(), iStrimmerU);
			}
		else
			{
			// It's Hex
			return new YadStreamerR_Hex(iStrimmerU);
			}
		}
	else
		{
		ZAny theVal;
		if (spFromStrim_Value(theStrimU, theVal))
			return sMake_YadAtomR_Any(theVal);
		}

	return null;
	}

// =================================================================================================
// MARK: - ReadOptions

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
// MARK: - WriteOptions

WriteOptions::WriteOptions()
:	ZYadOptions()
	{}

WriteOptions::WriteOptions(const ZYadOptions& iOther)
:	ZYadOptions(iOther)
	{}

WriteOptions::WriteOptions(const WriteOptions& iOther)
:	ZYadOptions(iOther)
,	fUseExtendedNotation(iOther.fUseExtendedNotation)
,	fBinaryAsBase64(iOther.fBinaryAsBase64)
,	fPreferSingleQuotes(iOther.fPreferSingleQuotes)
,	fNumberSequences(iOther.fNumberSequences)
	{}

// =================================================================================================
// MARK: - ParseException

ParseException::ParseException(const string& iWhat)
:	ZYadParseException_Std(iWhat)
	{}

ParseException::ParseException(const char* iWhat)
:	ZYadParseException_Std(iWhat)
	{}

// =================================================================================================
// MARK: - YadStreamerR_Hex

YadStreamerR_Hex::YadStreamerR_Hex(ZRef<ZStrimmerU> iStrimmerU)
:	fStrimmerU(iStrimmerU)
,	fStreamR(iStrimmerU->GetStrimU())
	{}

void YadStreamerR_Hex::Finish()
	{
	using namespace ZUtil_Strim;
	fStreamR.SkipAll();
	if (not sTryRead_CP(fStrimmerU->GetStrimU(), ')'))
		spThrowParseException("Expected ')' to close a binary data");	
	}

const ZStreamR& YadStreamerR_Hex::GetStreamR()
	{ return fStreamR; }

// =================================================================================================
// MARK: - YadStreamerR_Base64

YadStreamerR_Base64::YadStreamerR_Base64(const Base64::Decode& iDecode, ZRef<ZStrimmerU> iStrimmerU)
:	fStrimmerU(iStrimmerU)
,	fStreamR_ASCIIStrim(iStrimmerU->GetStrimU())
,	fStreamR_Boundary(")", fStreamR_ASCIIStrim)
,	fStreamR(fStreamR_Boundary)
	{}

void YadStreamerR_Base64::Finish()
	{
	using namespace ZUtil_Strim;
	fStreamR.SkipAll();
	if (not fStreamR_Boundary.HitBoundary())
		spThrowParseException("Expected ')' to close a binary data");	
	}

const ZStreamR& YadStreamerR_Base64::GetStreamR()
	{ return fStreamR; }

// =================================================================================================
// MARK: - YadStrimmerR

YadStrimmerR::YadStrimmerR(ZRef<ZStrimmerU> iStrimmerU)
:	fStrimmerU(iStrimmerU),
	fStrimR(iStrimmerU->GetStrimU(), '"')
	{}

void YadStrimmerR::Finish()
	{
	using namespace ZUtil_Strim;
	fStrimR.SkipAll();
	if (not sTryRead_CP(fStrimmerU->GetStrimU(), '"'))
		throw ParseException("Missing string delimiter");
	}

const ZStrimR& YadStrimmerR::GetStrimR()
	{ return fStrimR; }

// =================================================================================================
// MARK: - YadSeqR

YadSeqR::YadSeqR(ZRef<ZStrimmerU> iStrimmerU, const ZRef<CountedVal<ReadOptions> >& iRO)
:	fStrimmerU(iStrimmerU)
,	fRO(iRO)
	{}

void YadSeqR::Imp_ReadInc(bool iIsFirst, ZRef<ZYadR>& oYadR)
	{
	using namespace ZUtil_Strim;

	const ZStrimU& theStrimU = fStrimmerU->GetStrimU();

	sSkip_WSAndCPlusPlusComments(theStrimU);

	if (sTryRead_CP(theStrimU, ']'))
		{
		// Reached end.
		return;
		}

	if (not iIsFirst)
		{
		// Must read a separator
		if (not sTryRead_CP(theStrimU, ','))
			{
			if (not fRO->Get().fAllowSemiColons.DGet(false) || not sTryRead_CP(theStrimU, ';'))
				{
				if (not fRO->Get().fLooseSeparators.DGet(false))
					spThrowParseException("Require ',' to separate array elements");
				}
			}

		sSkip_WSAndCPlusPlusComments(theStrimU);

		if (fRO->Get().fAllowTerminators.DGet(false) && sTryRead_CP(theStrimU, ']'))
			{
			// The separator was actually a terminator, and we're done.
			return;
			}
		}

	if (not (oYadR = spMakeYadR_JSON(fStrimmerU, fRO)))
		spThrowParseException("Expected a value");
	}

// =================================================================================================
// MARK: - YadMapR

YadMapR::YadMapR(ZRef<ZStrimmerU> iStrimmerU, const ZRef<CountedVal<ReadOptions> >& iRO)
:	fStrimmerU(iStrimmerU)
,	fRO(iRO)
	{}

void YadMapR::Imp_ReadInc(bool iIsFirst, ZName& oName, ZRef<ZYadR>& oYadR)
	{
	using namespace ZUtil_Strim;

	const ZStrimU& theStrimU = fStrimmerU->GetStrimU();

	sSkip_WSAndCPlusPlusComments(theStrimU);

	if (sTryRead_CP(theStrimU, '}'))
		{
		// Reached end, with no terminator
		return;
		}

	if (not iIsFirst)
		{
		// Must read a separator
		if (not sTryRead_CP(theStrimU, ','))
			{
			if (not fRO->Get().fAllowSemiColons.DGet(false) || not sTryRead_CP(theStrimU, ';'))
				{
				if (not fRO->Get().fLooseSeparators.DGet(false))
					spThrowParseException("Require ',' to separate object elements");
				}
			}

		sSkip_WSAndCPlusPlusComments(theStrimU);

		if (fRO->Get().fAllowTerminators.DGet(false) && sTryRead_CP(theStrimU, '}'))
			{
			// The separator was actually a terminator, and we're done.
			return;
			}
		}

	string theName;
	if (not spTryRead_PropertyName(theStrimU, theName, fRO->Get().fAllowUnquotedPropertyNames.DGet(false)))
		spThrowParseException("Expected a member name");
	oName = theName;

	sSkip_WSAndCPlusPlusComments(theStrimU);

	if (not sTryRead_CP(theStrimU, ':'))
		{
		if (not fRO->Get().fAllowEquals.DGet(false) || not sTryRead_CP(theStrimU, '='))
			spThrowParseException("Expected ':' after a member name");
		}

	if (not (oYadR = spMakeYadR_JSON(fStrimmerU, fRO)))
		spThrowParseException("Expected value after ':'");
	}

// =================================================================================================
// MARK: - Static writing functions

static void spWriteIndent(size_t iCount, const WriteOptions& iOptions, const ChanW_UTF& iStrimW)
	{
	while (iCount--)
		iStrimW << iOptions.fIndentString;
	}

static void spWriteLFIndent(size_t iCount, const WriteOptions& iOptions, const ChanW_UTF& iStrimW)
	{
	iStrimW << iOptions.fEOLString;
	spWriteIndent(iCount, iOptions, iStrimW);
	}

static void spWriteString(const string& iString, bool iPreferSingleQuotes, const ChanW_UTF& s)
	{
	ZStrimW_Escaped::Options theOptions;
	theOptions.fEscapeHighUnicode = false;

	if (iPreferSingleQuotes)
		{
		s << "'";

		theOptions.fQuoteQuotes = false;

		ZStrimW_Escaped(theOptions, s) << iString;

		s << "'";
		}
	else
		{
		s << "\"";

		theOptions.fQuoteQuotes = true;

		ZStrimW_Escaped(theOptions, s) << iString;

		s << "\"";
		}
	}

static void spWriteString(const ZStrimR& iStrimR, const ChanW_UTF& s)
	{
	s << "\"";

	ZStrimW_Escaped::Options theOptions;
	theOptions.fQuoteQuotes = true;
	theOptions.fEscapeHighUnicode = false;

	ZStrimW_Escaped(theOptions, s) << iStrimR;

	s << "\"";
	}

static bool spContainsProblemChars(const string& iString)
	{
	if (iString.empty())
		{
		// An empty string can't be distinguished from no string at all, so
		// we treat it as if it has problem chars, so it will be wrapped in quotes.
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

static void spWritePropName(const string& iString, bool iUseSingleQuotes, const ChanW_UTF& s)
	{
	if (spContainsProblemChars(iString))
		spWriteString(iString, iUseSingleQuotes, s);
	else
		s << iString;
	}

static void spToStrim_SimpleValue(const ZAny& iAny, const ChanW_UTF& s)
	{
	if (false)
		{}
	else if (iAny.IsNull())
		{
		s << "null";
		}
	else if (const bool* theValue = iAny.PGet<bool>())
		{
		if (*theValue)
			s << "true";
		else
			s << "false";
		}
	else if (ZQ<int64> theQ = sQCoerceInt(iAny))
		{
		s << *theQ;
		}
	else if (const float* asFloat = iAny.PGet<float>())
		{
		ZUtil_Strim::sWriteExact(*asFloat, s);
		}
	else if (const double* asDouble = iAny.PGet<double>())
		{
		ZUtil_Strim::sWriteExact(*asDouble, s);
		}
	else if (const ZTime* asTime = iAny.PGet<ZTime>())
		{
		ZUtil_Strim::sWriteExact(asTime->fVal, s);
		}
	else
		{
		s << " /*!! Unhandled: " << spPrettyName(iAny.Type()) << " !!*/";
		}
	}

static void spToStrim_Stream(const ZStreamRPos& iStreamRPos,
	size_t iLevel, const WriteOptions& iOptions, bool iMayNeedInitialLF,
	const ChanW_UTF& s)
	{
	uint64 theSize = iStreamRPos.GetSize();
	if (theSize == 0)
		{
		// we've got an empty Raw
		s << "()";
		}
	else
		{
		if (iOptions.DoIndentation() && theSize > iOptions.fRawChunkSize)
			{
			if (iMayNeedInitialLF)
				spWriteLFIndent(iLevel, iOptions, s);

			uint64 countRemaining = theSize;
			if (iOptions.fRawSizeCap)
				countRemaining = min(countRemaining, iOptions.fRawSizeCap.Get());

			sWritefMust(s, "( // %lld bytes", theSize);

			if (countRemaining < theSize)
				sWritefMust(s, " (truncated at %lld bytes)", (long long)iOptions.fRawSizeCap.Get());

			spWriteLFIndent(iLevel, iOptions, s);
			if (iOptions.fRawAsASCII)
				{
				for (;;)
					{
					uint64 lastPos = iStreamRPos.GetPosition();
					const size_t countToCopy =
						min(iOptions.fRawChunkSize, ZStream::sClampedSize(countRemaining));
					uint64 countCopied;
					ZStreamW_HexStrim(iOptions.fRawByteSeparator, string(), 0, s)
						.CopyFrom(iStreamRPos, countToCopy, &countCopied, nullptr);
					countRemaining -= countCopied;
					if (countCopied == 0)
						break;

					iStreamRPos.SetPosition(lastPos);
					if (size_t extraSpaces = iOptions.fRawChunkSize - countCopied)
						{
						// We didn't write a complete line of bytes, so pad it out.
						while (extraSpaces--)
							{
							// Two spaces for the two nibbles
							s << "  ";
							// And then the separator sequence
							s << iOptions.fRawByteSeparator;
							}
						}

					s << " // ";
					while (countCopied--)
						{
						UTF32 theChar = iStreamRPos.ReadInt8();
						if (theChar < 0x20 || theChar > 0x7E)
							sWriteMust(UTF32('.'), s);
						else
							sWriteMust(theChar, s);
						}
					spWriteLFIndent(iLevel, iOptions, s);
					}
				}
			else
				{
				string eol = iOptions.fEOLString;
				for (size_t xx = 0; xx < iLevel; ++xx)
					eol += iOptions.fIndentString;

				ZStreamW_HexStrim(iOptions.fRawByteSeparator,
					eol, iOptions.fRawChunkSize, s).CopyAllFrom(iStreamRPos);

				spWriteLFIndent(iLevel, iOptions, s);
				}

			s << ")";
			}
		else
			{
			s << "(";

			ZStreamW_HexStrim(iOptions.fRawByteSeparator, string(), 0, s)
				.CopyAllFrom(iStreamRPos);

			if (iOptions.fRawAsASCII)
				{
				iStreamRPos.SetPosition(0);
				s << " /* ";
				while (theSize--)
					{
					UTF32 theChar = iStreamRPos.ReadInt8();
					if (theChar < 0x20 || theChar > 0x7E)
						sWriteMust(UTF32('.'), s);
					else
						sWriteMust(theChar, s);
					}
				s << " */";
				}
			s << ")";
			}
		}
	}

static void spToStrim_Stream(const ZStreamR& iStreamR,
	size_t iLevel, const WriteOptions& iOptions, bool iMayNeedInitialLF,
	const ChanW_UTF& s)
	{
	string chunkSeparator;
	size_t chunkSize = 0;
	if (iOptions.DoIndentation())
		{
		chunkSeparator = iOptions.fEOLString;
		for (size_t xx = 0; xx < iLevel; ++xx)
			chunkSeparator += iOptions.fIndentString;

		chunkSize = iOptions.fRawChunkSize;
		}

	if (iOptions.fBinaryAsBase64.DGet(false))
		{
		s << "(";
		s << "=";
		
		Base64::StreamW_Encode(
			Base64::sEncode_Normal(),
			ZStreamW_ASCIIStrim(
				ChanW_UTF_InsertSeparator(chunkSize * 3, chunkSeparator, s)))
		.CopyAllFrom(iStreamR);

		s << ")";
		}
	else if (const ZStreamRPos* theStreamRPos = dynamic_cast<const ZStreamRPos*>(&iStreamR))
		{
		spToStrim_Stream(*theStreamRPos, iLevel, iOptions, iMayNeedInitialLF, s);
		}
	else
		{
		s << "(";

		ZStreamW_HexStrim(iOptions.fRawByteSeparator, chunkSeparator, chunkSize, s)
			.CopyAllFrom(iStreamR);

		s << ")";
		}
	}

// =================================================================================================
// MARK: - Visitor_Writer

Visitor_Writer::Visitor_Writer(
	size_t iIndent, const WriteOptions& iOptions, const ChanW_UTF& iStrimW)
:	fIndent(iIndent),
	fOptions(iOptions),
	fStrimW(iStrimW),
	fMayNeedInitialLF(false)
	{}

void Visitor_Writer::Visit_YadR(const ZRef<ZYadR>& iYadR)
	{
	fStrimW << "null";
	if (fOptions.fBreakStrings)
		{
		fStrimW << " /*!! Unhandled yad: "
			<< spPrettyName(typeid(*iYadR.Get()))
			<< " !!*/";
		}
	}

void Visitor_Writer::Visit_YadAtomR(const ZRef<ZYadAtomR>& iYadAtomR)
	{ spToStrim_SimpleValue(iYadAtomR->AsAny(), fStrimW); }

void Visitor_Writer::Visit_YadStreamerR(const ZRef<ZYadStreamerR>& iYadStreamerR)
	{ spToStrim_Stream(iYadStreamerR->GetStreamR(), fIndent, fOptions, fMayNeedInitialLF, fStrimW); }

void Visitor_Writer::Visit_YadStrimmerR(const ZRef<ZYadStrimmerR>& iYadStrimmerR)
	{ spWriteString(iYadStrimmerR->GetStrimR(), fStrimW); }

void Visitor_Writer::Visit_YadSeqR(const ZRef<ZYadSeqR>& iYadSeqR)
	{
	bool needsIndentation = false;
	if (fOptions.DoIndentation())
		{
		// We're supposed to be indenting if we're complex, ie if any element is:
		// 1. A non-empty vector.
		// 2. A non-empty tuple.
		// or if iOptions.fBreakStrings is true, any element is a string with embedded
		// line breaks or more than iOptions.fStringLineLength characters.
		needsIndentation = not iYadSeqR->IsSimple(fOptions);
		}

	if (needsIndentation)
		{
		// We need to indent.
		if (fMayNeedInitialLF)
			{
			// We were invoked by a tuple which has already issued the property
			// name and equals sign, so we need to start a fresh line.
			spWriteLFIndent(fIndent, fOptions, fStrimW);
			}

		SaveSetRestore<bool> ssr_MayNeedInitialLF(fMayNeedInitialLF, false);

		uint64 count = 0;
		fStrimW << "[";
		for (bool isFirst = true; /*no test*/ ; isFirst = false)
			{
			if (ZRef<ZYadR,false> cur = iYadSeqR->ReadInc())
				{
				break;
				}
			else if (false && fOptions.fUseExtendedNotation.DGet(false))
				{
				spWriteLFIndent(fIndent, fOptions, fStrimW);
				cur->Accept(*this);
				fStrimW << ";";
				}
			else
				{
				if (not isFirst)
					fStrimW << ",";
				spWriteLFIndent(fIndent, fOptions, fStrimW);
				if (fOptions.fNumberSequences.DGet(false))
					fStrimW << "/*" << count << "*/";
				cur->Accept(*this);
				}
			++count;
			}
		spWriteLFIndent(fIndent, fOptions, fStrimW);
		fStrimW << "]";
		}
	else
		{
		// We're not indenting, so we can just dump everything out on
		// one line, with just some spaces to keep things legible.
		fStrimW << "[";
		for (bool isFirst = true; /*no test*/ ; isFirst = false)
			{
			if (ZRef<ZYadR,false> cur = iYadSeqR->ReadInc())
				{
				break;
				}
			else if (false && fOptions.fUseExtendedNotation.DGet(false))
				{
				if (not isFirst && fOptions.fBreakStrings)
					fStrimW << " ";
				cur->Accept(*this);
				fStrimW << ";";
				}
			else
				{
				if (not isFirst)
					{
					fStrimW << ",";
					if (fOptions.fBreakStrings)
						fStrimW << " ";
					}
				cur->Accept(*this);
				}
			}
		fStrimW << "]";
		}
	}

void Visitor_Writer::Visit_YadMapR(const ZRef<ZYadMapR>& iYadMapR)
	{
	bool needsIndentation = false;
	if (fOptions.DoIndentation())
		{
		needsIndentation = not iYadMapR->IsSimple(fOptions);
		}

	const bool useSingleQuotes = fOptions.fPreferSingleQuotes.DGet(false);

	if (needsIndentation)
		{
		if (fMayNeedInitialLF)
			{
			// We're going to be indenting, but need to start
			// a fresh line to have our { and contents line up.
			spWriteLFIndent(fIndent, fOptions, fStrimW);
			}

		fStrimW << "{";
		for (bool isFirst = true; /*no test*/ ; isFirst = false)
			{
			ZName curName;
			if (ZRef<ZYadR,false> cur = iYadMapR->ReadInc(curName))
				{
				break;
				}
			else if (fOptions.fUseExtendedNotation.DGet(false))
				{
				spWriteLFIndent(fIndent, fOptions, fStrimW);
				spWritePropName(curName, useSingleQuotes, fStrimW);
				fStrimW << " = ";

				SaveSetRestore<size_t> ssr_Indent(fIndent, fIndent + 1);
				SaveSetRestore<bool> ssr_MayNeedInitialLF(fMayNeedInitialLF, true);
				cur->Accept(*this);
				fStrimW << ";";
				}
			else
				{
				if (not isFirst)
					fStrimW << ",";
				spWriteLFIndent(fIndent, fOptions, fStrimW);
				spWriteString(curName, useSingleQuotes, fStrimW);
				fStrimW << ": ";

				SaveSetRestore<size_t> ssr_Indent(fIndent, fIndent + 1);
				SaveSetRestore<bool> ssr_MayNeedInitialLF(fMayNeedInitialLF, true);
				cur->Accept(*this);
				}
			}
		spWriteLFIndent(fIndent, fOptions, fStrimW);
		fStrimW << "}";
		}
	else
		{
		fStrimW << "{";
		bool wroteAny = false;
		for (bool isFirst = true; /*no test*/ ; isFirst = false)
			{
			ZName curName;
			if (ZRef<ZYadR,false> cur = iYadMapR->ReadInc(curName))
				{
				break;
				}
			else if (fOptions.fUseExtendedNotation.DGet(false))
				{
				if (not isFirst && fOptions.fBreakStrings)
					fStrimW << " ";

				spWritePropName(curName, useSingleQuotes, fStrimW);
				if (fOptions.fBreakStrings)
					fStrimW << " = ";
				else
					fStrimW << "=";

				SaveSetRestore<size_t> ssr_Indent(fIndent, fIndent + 1);
				SaveSetRestore<bool> ssr_MayNeedInitialLF(fMayNeedInitialLF, true);
				cur->Accept(*this);
				fStrimW << ";";
				}
			else
				{
				if (not isFirst)
					fStrimW << ",";
				if (fOptions.fBreakStrings)
					fStrimW << " ";
				spWriteString(curName, useSingleQuotes, fStrimW);
				fStrimW << ":";
				if (fOptions.fBreakStrings)
					fStrimW << " ";

				SaveSetRestore<size_t> ssr_Indent(fIndent, fIndent + 1);
				SaveSetRestore<bool> ssr_MayNeedInitialLF(fMayNeedInitialLF, true);
				cur->Accept(*this);
				}
			wroteAny = true;
			}
		if (wroteAny && fOptions.fBreakStrings)
			fStrimW << " ";
		fStrimW << "}";
		}
	}

// =================================================================================================
// MARK: - sYadR and sToStrim

ZRef<ZYadR> sYadR(ZRef<ZStrimmerU> iStrimmerU)
	{
	if (iStrimmerU)
		return spMakeYadR_JSON(iStrimmerU, sCountedVal(ReadOptions()));
	return null;
	}

ZRef<ZYadR> sYadR(ZRef<ZStrimmerU> iStrimmerU, const ReadOptions& iReadOptions)
	{
	if (iStrimmerU)
		return spMakeYadR_JSON(iStrimmerU, sCountedVal(iReadOptions));
	return null;
	}

void sToStrim(ZRef<ZYadR> iYadR, const ChanW_UTF& s)
	{ sToStrim(0, WriteOptions(), iYadR, s); }

void sToStrim(size_t iInitialIndent, const WriteOptions& iOptions,
	ZRef<ZYadR> iYadR, const ChanW_UTF& s)
	{
	if (iYadR)
		iYadR->Accept(Visitor_Writer(iInitialIndent, iOptions, s));
	}

} // namespace ZYad_JSON
} // namespace ZooLib
