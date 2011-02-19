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
#include "zoolib/ZStrim_Escaped.h"
#include "zoolib/ZTime.h"
#include "zoolib/ZUtil_Any.h"
#include "zoolib/ZUtil_Strim.h"
#include "zoolib/ZYad_JSON.h"

namespace ZooLib {

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
#pragma mark -
#pragma mark * Helpers

static void spThrowParseException(const string& iMessage)
	{
	throw ZYadParseException_JSON(iMessage);
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

			if (!sTryRead_CP(s, '"'))
				throw ParseException("Expected '\"' to close a string");

			oString += tempString;

			sSkip_WSAndCPlusPlusComments(s);

			if (!sTryRead_CP(s, '"'))
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

			if (!sTryRead_CP(s, '\''))
				throw ParseException("Expected \"'\" to close a string");

			oString += tempString;

			sSkip_WSAndCPlusPlusComments(s);

			if (!sTryRead_CP(s, '\''))
				return true;
			}
		}

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
		oVal = ZAny();
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

static ZRef<ZYadR> spMakeYadR_JSON(ZRef<ZStrimmerU> iStrimmerU)
	{
	using namespace ZUtil_Strim;

	const ZStrimU& theStrimU = iStrimmerU->GetStrimU();

	sSkip_WSAndCPlusPlusComments(theStrimU);

	if (sTryRead_CP(theStrimU, '['))
		{
		return new ZYadSeqR_JSON(iStrimmerU);
		}
	else if (sTryRead_CP(theStrimU, '{'))
		{
		return new ZYadMapR_JSON(iStrimmerU);
		}
	else if (sTryRead_CP(theStrimU, '"'))
		{
		return new ZYadStrimR_JSON(iStrimmerU);
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
#pragma mark -
#pragma mark * ZYadParseException_JSON

ZYadParseException_JSON::ZYadParseException_JSON(const string& iWhat)
:	ZYadParseException_Std(iWhat)
	{}

ZYadParseException_JSON::ZYadParseException_JSON(const char* iWhat)
:	ZYadParseException_Std(iWhat)
	{}

// =================================================================================================
#pragma mark -
#pragma mark * ZYadStrimR_JSON

ZYadStrimR_JSON::ZYadStrimR_JSON(ZRef<ZStrimmerU> iStrimmerU)
:	fStrimmerU(iStrimmerU),
	fStrimR(iStrimmerU->GetStrimU(), '"')
	{}

void ZYadStrimR_JSON::Finish()
	{
	using namespace ZUtil_Strim;
	fStrimR.SkipAll();
	if (!sTryRead_CP(fStrimmerU->GetStrimU(), '"'))
		throw ParseException("Missing string delimiter");
	}

const ZStrimR& ZYadStrimR_JSON::GetStrimR()
	{ return fStrimR; }

// =================================================================================================
#pragma mark -
#pragma mark * ZYadSeqR_JSON

ZYadSeqR_JSON::ZYadSeqR_JSON(ZRef<ZStrimmerU> iStrimmerU)
:	fStrimmerU(iStrimmerU)
	{}

void ZYadSeqR_JSON::Imp_ReadInc(bool iIsFirst, ZRef<ZYadR>& oYadR)
	{
	using namespace ZUtil_Strim;

	const ZStrimU& theStrimU = fStrimmerU->GetStrimU();

	sSkip_WSAndCPlusPlusComments(theStrimU);

	if (sTryRead_CP(theStrimU, ']'))
		{
		// Reached end.
		return;
		}

	if (!iIsFirst)
		{
		// Must read a separator
		if (!sTryRead_CP(theStrimU, ','))
			spThrowParseException("Require ',' to separate array elements");
		sSkip_WSAndCPlusPlusComments(theStrimU);
		}

	if (!(oYadR = spMakeYadR_JSON(fStrimmerU)))
		spThrowParseException("Expected a value");
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZYadMapR_JSON

ZYadMapR_JSON::ZYadMapR_JSON(ZRef<ZStrimmerU> iStrimmerU)
:	fStrimmerU(iStrimmerU)
	{}

void ZYadMapR_JSON::Imp_ReadInc(bool iIsFirst, std::string& oName, ZRef<ZYadR>& oYadR)
	{
	using namespace ZUtil_Strim;

	const ZStrimU& theStrimU = fStrimmerU->GetStrimU();

	sSkip_WSAndCPlusPlusComments(theStrimU);

	if (sTryRead_CP(theStrimU, '}'))
		{
		// Reached end.
		return;
		}

	if (!iIsFirst)
		{
		// Must read a separator
		if (!sTryRead_CP(theStrimU, ','))
			spThrowParseException("Require ',' to separate object elements");
		sSkip_WSAndCPlusPlusComments(theStrimU);
		}

	if (!spTryRead_JSONString(theStrimU, oName))
		spThrowParseException("Expected a member name");

	sSkip_WSAndCPlusPlusComments(theStrimU);

	if (!sTryRead_CP(theStrimU, ':'))
		spThrowParseException("Expected ':' after a member name");

	if (!(oYadR = spMakeYadR_JSON(fStrimmerU)))
		spThrowParseException("Expected value after ':'");
	}

// =================================================================================================
#pragma mark -
#pragma mark * Static writing functions

static void spWriteIndent(const ZStrimW& iStrimW,
	size_t iCount, const ZYadOptions& iOptions)
	{
	while (iCount--)
		iStrimW.Write(iOptions.fIndentString);
	}

static void spWriteLFIndent(const ZStrimW& iStrimW,
	size_t iCount, const ZYadOptions& iOptions)
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

static void spToStrim_SimpleValue(const ZStrimW& s, const ZAny& iAny)
	{
	int64 asInt64;

	if (false)
		{}
	else if (!iAny)
		{
		s << "null";
		}
	else if (const bool* theValue = iAny.PGet<bool>())
		{
		if (*theValue)
			s.Write("true");
		else
			s.Write("false");
		}
	else if (sQCoerceInt(iAny, asInt64))
		{
		s.Writef("%lld", asInt64);
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
		s << "null /*!! Unhandled: " << iAny.Type().name() << " !!*/";
		}
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZVisitor_Yad_JSONWriter::SaveState

class ZVisitor_Yad_JSONWriter::SaveState
	{
public:
	SaveState(ZVisitor_Yad_JSONWriter* iVisitor);
	~SaveState();

	ZVisitor_Yad_JSONWriter* fVisitor;
	size_t fIndent;
	bool fMayNeedInitialLF;
	};

ZVisitor_Yad_JSONWriter::SaveState::SaveState(ZVisitor_Yad_JSONWriter* iVisitor)
:	fVisitor(iVisitor),
	fIndent(fVisitor->fIndent),
	fMayNeedInitialLF(fVisitor->fMayNeedInitialLF)
	{}

ZVisitor_Yad_JSONWriter::SaveState::~SaveState()
	{
	fVisitor->fIndent = fIndent;
	fVisitor->fMayNeedInitialLF = fMayNeedInitialLF;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZVisitor_Yad_JSONWriter

ZVisitor_Yad_JSONWriter::ZVisitor_Yad_JSONWriter(
	size_t iIndent, const ZYadOptions& iOptions, const ZStrimW& iStrimW)
:	fIndent(iIndent),
	fOptions(iOptions),
	fStrimW(iStrimW),
	fMayNeedInitialLF(false)
	{}

void ZVisitor_Yad_JSONWriter::Visit_YadR(const ZRef<ZYadR>& iYadR)
	{
	fStrimW << "null";
	if (fOptions.fBreakStrings)
		fStrimW << " /*!! Unhandled yad !!*/";
	}

void ZVisitor_Yad_JSONWriter::Visit_YadAtomR(const ZRef<ZYadAtomR>& iYadAtomR)
	{ spToStrim_SimpleValue(fStrimW, iYadAtomR->AsAny()); }

void ZVisitor_Yad_JSONWriter::Visit_YadStreamR(const ZRef<ZYadStreamR>& iYadStreamR)
	{
	fStrimW << "null";
	if (fOptions.fBreakStrings)
		fStrimW << " /*!! ZYadStreamR not representable in JSON !!*/";
	}

void ZVisitor_Yad_JSONWriter::Visit_YadStrimR(const ZRef<ZYadStrimR>& iYadStrimR)
	{ spWriteString(fStrimW, iYadStrimR->GetStrimR()); }

void ZVisitor_Yad_JSONWriter::Visit_YadSeqR(const ZRef<ZYadSeqR>& iYadSeqR)
	{
	bool needsIndentation = false;
	if (fOptions.DoIndentation())
		{
		// We're supposed to be indenting if we're complex, ie if any element is:
		// 1. A non-empty vector.
		// 2. A non-empty tuple.
		// or if iOptions.fBreakStrings is true, any element is a string with embedded
		// line breaks or more than iOptions.fStringLineLength characters.
		needsIndentation = ! iYadSeqR->IsSimple(fOptions);
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

		SaveState save(this);
		fMayNeedInitialLF = false;

		fStrimW.Write("[");
		for (bool isFirst = true; /*no test*/ ; isFirst = false)
			{
			if (ZRef<ZYadR,false> cur = iYadSeqR->ReadInc())
				{
				break;
				}
			else
				{
				if (!isFirst)
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
			else
				{
				if (!isFirst)
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

void ZVisitor_Yad_JSONWriter::Visit_YadMapR(const ZRef<ZYadMapR>& iYadMapR)
	{
	bool needsIndentation = false;
	if (fOptions.DoIndentation())
		{
		needsIndentation = ! iYadMapR->IsSimple(fOptions);
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
			else
				{
				if (!isFirst)
					fStrimW.Write(",");
				spWriteLFIndent(fStrimW, fIndent, fOptions);
				spWriteString(fStrimW, curName);
				fStrimW << ": ";

				SaveState save(this);
				fIndent = fIndent + 1;
				fMayNeedInitialLF = true;
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
			else
				{
				if (!isFirst)
					fStrimW.Write(",");
				if (fOptions.fBreakStrings)
					fStrimW.Write(" ");
				spWriteString(fStrimW, curName);
				fStrimW.Write(":");
				if (fOptions.fBreakStrings)
					fStrimW.Write(" ");

				SaveState save(this);
				fIndent = fIndent + 1;
				fMayNeedInitialLF = true;
				cur->Accept(*this);
				}
			}
		if (fOptions.fBreakStrings)
			fStrimW.Write(" ");
		fStrimW.Write("}");
		}
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZYad_JSON

ZRef<ZYadR> ZYad_JSON::sMakeYadR(ZRef<ZStrimmerU> iStrimmerU)
	{ return spMakeYadR_JSON(iStrimmerU); }

void ZYad_JSON::sToStrim(ZRef<ZYadR> iYadR, const ZStrimW& s)
	{ sToStrim(0, ZYadOptions(), iYadR, s); }

void ZYad_JSON::sToStrim(size_t iInitialIndent, const ZYadOptions& iOptions,
	ZRef<ZYadR> iYadR, const ZStrimW& s)
	{
	ZVisitor_Yad_JSONWriter theWriter(iInitialIndent, iOptions, s);
	iYadR->Accept(theWriter);
	}

} // namespace ZooLib
