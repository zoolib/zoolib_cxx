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

#include "zoolib/ZCompat_cmath.h"
#include "zoolib/ZSetRestore_T.h"
#include "zoolib/ZStreamW_HexStrim.h"
#include "zoolib/ZStrim_Escaped.h"
#include "zoolib/ZTime.h"
#include "zoolib/ZUnicode.h"
#include "zoolib/ZUtil_Any.h"
#include "zoolib/ZUtil_Strim.h"
#include "zoolib/ZUtil_Strim_Operators.h"
#include "zoolib/ZYad_JSON.h"

#if ZCONFIG(Compiler,GCC)
	#include <cxxabi.h>
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

static bool spRead_Identifier
	(const ZStrimU& iStrimU, string* oStringLC, string* oStringExact)
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
		if (not ZUnicode::sIsAlphaDigit(theCP) && theCP != '_')
			{
			iStrimU.Unread(theCP);
			break;
			}

		gotAny = true;

		if (oStringLC)
			*oStringLC += ZUnicode::sToLower(theCP);
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

static ZRef<ZYadR> spMakeYadR_JSON
	(ZRef<ZStrimmerU> iStrimmerU, const ZRef<ZCountedVal<ReadOptions> >& iRO)
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
		return new YadStreamerR(iStrimmerU);
		}
	else
		{
		ZAny theVal;
		if (spFromStrim_Value(theStrimU, theVal))
			return new ZYadAtomR_Std(theVal);
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
,	fUseExtendedNotation(false)
	{}

WriteOptions::WriteOptions(const ZYadOptions& iOther)
:	ZYadOptions(iOther)
,	fUseExtendedNotation(false)
	{}

WriteOptions::WriteOptions(const WriteOptions& iOther)
:	ZYadOptions(iOther)
,	fUseExtendedNotation(iOther.fUseExtendedNotation)
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
// MARK: - YadStreamerR

YadStreamerR::YadStreamerR(ZRef<ZStrimmerU> iStrimmerU)
:	fStrimmerU(iStrimmerU)
,	fStreamR(iStrimmerU->GetStrimU())
	{}

void YadStreamerR::Finish()
	{
	using namespace ZUtil_Strim;
	fStreamR.SkipAll();
	if (not sTryRead_CP(fStrimmerU->GetStrimU(), ')'))
		spThrowParseException("Expected ')' to close a binary data");	
	}

const ZStreamR& YadStreamerR::GetStreamR()
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

YadSeqR::YadSeqR(ZRef<ZStrimmerU> iStrimmerU, const ZRef<ZCountedVal<ReadOptions> >& iRO)
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

YadMapR::YadMapR(ZRef<ZStrimmerU> iStrimmerU, const ZRef<ZCountedVal<ReadOptions> >& iRO)
:	fStrimmerU(iStrimmerU)
,	fRO(iRO)
	{}

void YadMapR::Imp_ReadInc(bool iIsFirst, std::string& oName, ZRef<ZYadR>& oYadR)
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

	if (not spTryRead_PropertyName(theStrimU, oName, fRO->Get().fAllowUnquotedPropertyNames.DGet(false)))
		spThrowParseException("Expected a member name");

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

static void spWriteIndent(const ZStrimW& iStrimW,
	size_t iCount, const WriteOptions& iOptions)
	{
	while (iCount--)
		iStrimW.Write(iOptions.fIndentString);
	}

static void spWriteLFIndent(const ZStrimW& iStrimW,
	size_t iCount, const WriteOptions& iOptions)
	{
	iStrimW.Write(iOptions.fEOLString);
	spWriteIndent(iStrimW, iCount, iOptions);
	}

static void spWriteString(const ZStrimW& s, const string& iString)
	{
	s.Write("\"");

	ZStrimW_Escaped::Options theOptions;
	theOptions.fQuoteQuotes = true;
	theOptions.fEscapeHighUnicode = false;

	ZStrimW_Escaped(theOptions, s).Write(iString);

	s.Write("\"");
	}

static void spWriteString(const ZStrimW& s, const ZStrimR& iStrimR)
	{
	s.Write("\"");

	ZStrimW_Escaped::Options theOptions;
	theOptions.fQuoteQuotes = true;
	theOptions.fEscapeHighUnicode = false;

	ZStrimW_Escaped(theOptions, s).CopyAllFrom(iStrimR);

	s.Write("\"");
	}

static bool spContainsProblemChars(const string& iString)
	{
	if (iString.empty())
		{
		// An empty string can't be distinguished from no string at all, so
		// we treat it as if it has problem chars, so it will be wrapped in quotes.
		return true;
		}

	for (string::const_iterator i = iString.begin(), end = iString.end();;)
		{
		UTF32 theCP;
		if (not ZUnicode::sReadInc(i, end, theCP))
			break;
		if (not ZUnicode::sIsAlphaDigit(theCP) && '_' != theCP)
			return true;
		}

	return false;
	}

static void spWritePropName(const ZStrimW& s, const string& iString)
	{
	ZStrimW_Escaped::Options theOptions;
	theOptions.fQuoteQuotes = true;
	theOptions.fEscapeHighUnicode = false;

	if (spContainsProblemChars(iString))
		{
		s << "\"";
		ZStrimW_Escaped(theOptions, s) << iString;
		s << "\"";
		}
	else
		{
		ZStrimW_Escaped(theOptions, s) << iString;
		}
	}

static void spToStrim_SimpleValue(const ZStrimW& s, const ZAny& iAny)
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
		ZUtil_Strim::sWriteExact(s, *asFloat);
		}
	else if (const double* asDouble = iAny.PGet<double>())
		{
		ZUtil_Strim::sWriteExact(s, *asDouble);
		}
	else if (const ZTime* asTime = iAny.PGet<ZTime>())
		{
		ZUtil_Strim::sWriteExact(s, asTime->fVal);
		}
	else
		{
		s << " /*!! Unhandled: " << spPrettyName(iAny.Type()) << " !!*/";
		}
	}

static void spToStrim_Stream(const ZStrimW& s, const ZStreamRPos& iStreamRPos,
	size_t iLevel, const WriteOptions& iOptions, bool iMayNeedInitialLF)
	{
	uint64 theSize = iStreamRPos.GetSize();
	if (theSize == 0)
		{
		// we've got an empty Raw
		s.Write("()");
		}
	else
		{
		if (iOptions.DoIndentation() && theSize > iOptions.fRawChunkSize)
			{
			if (iMayNeedInitialLF)
				spWriteLFIndent(s, iLevel, iOptions);

			uint64 countRemaining = theSize;
			if (iOptions.fRawSizeCap)
				countRemaining = min(countRemaining, iOptions.fRawSizeCap.Get());

			s.Writef("( // %lld bytes", theSize);

			if (countRemaining < theSize)
				s.Writef(" (truncated at %lld bytes)", iOptions.fRawSizeCap.Get());

			spWriteLFIndent(s, iLevel, iOptions);
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
							s.Write("  ");
							// And then the separator sequence
							s.Write(iOptions.fRawByteSeparator);
							}
						}

					s.Write(" // ");
					while (countCopied--)
						{
						char theChar = iStreamRPos.ReadInt8();
						if (theChar < 0x20 || theChar > 0x7E)
							s.WriteCP('.');
						else
							s.WriteCP(theChar);
						}
					spWriteLFIndent(s, iLevel, iOptions);
					}
				}
			else
				{
				string eol = iOptions.fEOLString;
				for (size_t x = 0; x < iLevel; ++x)
					eol += iOptions.fIndentString;

				ZStreamW_HexStrim(iOptions.fRawByteSeparator,
					eol, iOptions.fRawChunkSize, s).CopyAllFrom(iStreamRPos);

				spWriteLFIndent(s, iLevel, iOptions);
				}

			s.Write(")");
			}
		else
			{
			s.Write("(");

			ZStreamW_HexStrim(iOptions.fRawByteSeparator, string(), 0, s)
				.CopyAllFrom(iStreamRPos);

			if (iOptions.fRawAsASCII)
				{
				iStreamRPos.SetPosition(0);
				s.Write(" /* ");
				while (theSize--)
					{
					char theChar = iStreamRPos.ReadInt8();
					if (theChar < 0x20 || theChar > 0x7E)
						s.WriteCP('.');
					else
						s.WriteCP(theChar);
					}
				s.Write(" */");
				}
			s.Write(")");
			}
		}
	}

static void spToStrim_Stream(const ZStrimW& s, const ZStreamR& iStreamR,
	size_t iLevel, const WriteOptions& iOptions, bool iMayNeedInitialLF)
	{
	if (const ZStreamRPos* theStreamRPos = dynamic_cast<const ZStreamRPos*>(&iStreamR))
		{
		spToStrim_Stream(s, *theStreamRPos, iLevel, iOptions, iMayNeedInitialLF);
		}
	else
		{
		s.Write("(");

		ZStreamW_HexStrim(iOptions.fRawByteSeparator, string(), 0, s)
			.CopyAllFrom(iStreamR);

		s.Write(")");
		}
	}

// =================================================================================================
// MARK: - Visitor_Writer

Visitor_Writer::Visitor_Writer
	(size_t iIndent, const WriteOptions& iOptions, const ZStrimW& iStrimW)
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
	{ spToStrim_SimpleValue(fStrimW, iYadAtomR->AsAny()); }

void Visitor_Writer::Visit_YadStreamerR(const ZRef<ZYadStreamerR>& iYadStreamerR)
	{ spToStrim_Stream(fStrimW, iYadStreamerR->GetStreamR(), fIndent, fOptions, fMayNeedInitialLF); }

void Visitor_Writer::Visit_YadStrimmerR(const ZRef<ZYadStrimmerR>& iYadStrimmerR)
	{ spWriteString(fStrimW, iYadStrimmerR->GetStrimR()); }

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
			spWriteLFIndent(fStrimW, fIndent, fOptions);
			}

		ZSetRestore_T<bool> theSR_MayNeedInitialLF(fMayNeedInitialLF, false);

		fStrimW.Write("[");
		for (bool isFirst = true; /*no test*/ ; isFirst = false)
			{
			if (ZRef<ZYadR,false> cur = iYadSeqR->ReadInc())
				{
				break;
				}
			else if (false && fOptions.fUseExtendedNotation.DGet(false))
				{
				spWriteLFIndent(fStrimW, fIndent, fOptions);
				cur->Accept(*this);
				fStrimW.Write(";");
				}
			else
				{
				if (not isFirst)
					fStrimW.Write(",");
				spWriteLFIndent(fStrimW, fIndent, fOptions);
				cur->Accept(*this);
				}
			}
		spWriteLFIndent(fStrimW, fIndent, fOptions);
		fStrimW.Write("]");
		}
	else
		{
		// We're not indenting, so we can just dump everything out on
		// one line, with just some spaces to keep things legible.
		fStrimW.Write("[");
		for (bool isFirst = true; /*no test*/ ; isFirst = false)
			{
			if (ZRef<ZYadR,false> cur = iYadSeqR->ReadInc())
				{
				break;
				}
			else if (false && fOptions.fUseExtendedNotation.DGet(false))
				{
				if (not isFirst && fOptions.fBreakStrings)
					fStrimW.Write(" ");
				cur->Accept(*this);
				fStrimW.Write(";");
				}
			else
				{
				if (not isFirst)
					{
					fStrimW.Write(",");
					if (fOptions.fBreakStrings)
						fStrimW.Write(" ");
					}
				cur->Accept(*this);
				}
			}
		fStrimW.Write("]");
		}
	}

void Visitor_Writer::Visit_YadMapR(const ZRef<ZYadMapR>& iYadMapR)
	{
	bool needsIndentation = false;
	if (fOptions.DoIndentation())
		{
		needsIndentation = not iYadMapR->IsSimple(fOptions);
		}

	if (needsIndentation)
		{
		if (fMayNeedInitialLF)
			{
			// We're going to be indenting, but need to start
			// a fresh line to have our { and contents line up.
			spWriteLFIndent(fStrimW, fIndent, fOptions);
			}

		fStrimW.Write("{");
		for (bool isFirst = true; /*no test*/ ; isFirst = false)
			{
			string curName;
			if (ZRef<ZYadR,false> cur = iYadMapR->ReadInc(curName))
				{
				break;
				}
			else if (fOptions.fUseExtendedNotation.DGet(false))
				{
				spWriteLFIndent(fStrimW, fIndent, fOptions);
				spWritePropName(fStrimW, curName);
				fStrimW << " = ";

				ZSetRestore_T<size_t> theSR_Indent(fIndent, fIndent + 1);
				ZSetRestore_T<bool> theSR_MayNeedInitialLF(fMayNeedInitialLF, true);
				cur->Accept(*this);
				fStrimW.Write(";");
				}
			else
				{
				if (not isFirst)
					fStrimW.Write(",");
				spWriteLFIndent(fStrimW, fIndent, fOptions);
				spWriteString(fStrimW, curName);
				fStrimW << ": ";

				ZSetRestore_T<size_t> theSR_Indent(fIndent, fIndent + 1);
				ZSetRestore_T<bool> theSR_MayNeedInitialLF(fMayNeedInitialLF, true);
				cur->Accept(*this);
				}
			}
		spWriteLFIndent(fStrimW, fIndent, fOptions);
		fStrimW.Write("}");
		}
	else
		{
		fStrimW.Write("{");
		for (bool isFirst = true; /*no test*/ ; isFirst = false)
			{
			string curName;
			if (ZRef<ZYadR,false> cur = iYadMapR->ReadInc(curName))
				{
				break;
				}
			else if (fOptions.fUseExtendedNotation.DGet(false))
				{
				if (not isFirst && fOptions.fBreakStrings)
					fStrimW.Write(" ");

				spWritePropName(fStrimW, curName);
				if (fOptions.fBreakStrings)
					fStrimW.Write(" = ");
				else
					fStrimW.Write("=");

				ZSetRestore_T<size_t> theSR_Indent(fIndent, fIndent + 1);
				ZSetRestore_T<bool> theSR_MayNeedInitialLF(fMayNeedInitialLF, true);
				cur->Accept(*this);
				fStrimW.Write(";");
				}
			else
				{
				if (not isFirst)
					fStrimW.Write(",");
				if (fOptions.fBreakStrings)
					fStrimW.Write(" ");
				spWriteString(fStrimW, curName);
				fStrimW.Write(":");
				if (fOptions.fBreakStrings)
					fStrimW.Write(" ");

				ZSetRestore_T<size_t> theSR_Indent(fIndent, fIndent + 1);
				ZSetRestore_T<bool> theSR_MayNeedInitialLF(fMayNeedInitialLF, true);
				cur->Accept(*this);
				}
			}
		if (fOptions.fBreakStrings)
			fStrimW.Write(" ");
		fStrimW.Write("}");
		}
	}

// =================================================================================================
// MARK: - sYadR and sToStrim

ZRef<ZYadR> sYadR(ZRef<ZStrimmerU> iStrimmerU)
	{ return spMakeYadR_JSON(iStrimmerU, sCountedVal(ReadOptions())); }

ZRef<ZYadR> sYadR(ZRef<ZStrimmerU> iStrimmerU, const ReadOptions& iReadOptions)
	{ return spMakeYadR_JSON(iStrimmerU, sCountedVal(iReadOptions)); }

void sToStrim(ZRef<ZYadR> iYadR, const ZStrimW& s)
	{ sToStrim(0, WriteOptions(), iYadR, s); }

void sToStrim(size_t iInitialIndent, const WriteOptions& iOptions,
	ZRef<ZYadR> iYadR, const ZStrimW& s)
	{
	if (iYadR)
		iYadR->Accept(Visitor_Writer(iInitialIndent, iOptions, s));
	}

} // namespace ZYad_JSON
} // namespace ZooLib
