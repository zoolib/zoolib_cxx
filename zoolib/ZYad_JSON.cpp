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
#include "zoolib/ZUnicode.h"
#include "zoolib/ZUtil_Any.h"
#include "zoolib/ZUtil_Strim.h"
#include "zoolib/ZUtil_Strim_Operators.h"
#include "zoolib/ZYad_JSON.h"

namespace ZooLib {
namespace ZYad_JSON {

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

static ZRef<ZYadR> spMakeYadR_JSON
	(ZRef<ZStrimmerU> iStrimmerU, const ReadOptions& iReadOptions)
	{
	using namespace ZUtil_Strim;

	const ZStrimU& theStrimU = iStrimmerU->GetStrimU();

	sSkip_WSAndCPlusPlusComments(theStrimU);

	if (sTryRead_CP(theStrimU, '['))
		{
		return new YadSeqR(iStrimmerU, iReadOptions);
		}
	else if (sTryRead_CP(theStrimU, '{'))
		{
		return new YadMapR(iStrimmerU, iReadOptions);
		}
	else if (sTryRead_CP(theStrimU, '"'))
		{
		return new YadStrimR(iStrimmerU);
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
#pragma mark * ParseException

ParseException::ParseException(const string& iWhat)
:	ZYadParseException_Std(iWhat)
	{}

ParseException::ParseException(const char* iWhat)
:	ZYadParseException_Std(iWhat)
	{}

// =================================================================================================
#pragma mark -
#pragma mark * YadStrimR

YadStrimR::YadStrimR(ZRef<ZStrimmerU> iStrimmerU)
:	fStrimmerU(iStrimmerU),
	fStrimR(iStrimmerU->GetStrimU(), '"')
	{}

void YadStrimR::Finish()
	{
	using namespace ZUtil_Strim;
	fStrimR.SkipAll();
	if (!sTryRead_CP(fStrimmerU->GetStrimU(), '"'))
		throw ParseException("Missing string delimiter");
	}

const ZStrimR& YadStrimR::GetStrimR()
	{ return fStrimR; }

// =================================================================================================
#pragma mark -
#pragma mark * YadSeqR

YadSeqR::YadSeqR(ZRef<ZStrimmerU> iStrimmerU, const ReadOptions& iReadOptions)
:	fStrimmerU(iStrimmerU)
,	fReadOptions(iReadOptions)
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
		if (!sTryRead_CP(theStrimU, ','))
			{
			if (!fReadOptions.fAllowSemiColons.DGet(false) || !sTryRead_CP(theStrimU, ';'))
				spThrowParseException("Require ',' to separate array elements");
			}

		sSkip_WSAndCPlusPlusComments(theStrimU);

		if (fReadOptions.fAllowTerminators.DGet(false) && sTryRead_CP(theStrimU, ']'))
			{
			// The separator was actually a terminator, and we're done.
			return;
			}
		}

	if (!(oYadR = spMakeYadR_JSON(fStrimmerU, fReadOptions)))
		spThrowParseException("Expected a value");
	}

// =================================================================================================
#pragma mark -
#pragma mark * YadMapR

YadMapR::YadMapR(ZRef<ZStrimmerU> iStrimmerU, const ReadOptions& iReadOptions)
:	fStrimmerU(iStrimmerU)
,	fReadOptions(iReadOptions)
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
		if (!sTryRead_CP(theStrimU, ','))
			{
			if (!fReadOptions.fAllowSemiColons.DGet(false) || !sTryRead_CP(theStrimU, ';'))
				spThrowParseException("Require ',' to separate object elements");
			}

		sSkip_WSAndCPlusPlusComments(theStrimU);

		if (fReadOptions.fAllowTerminators.DGet(false) && sTryRead_CP(theStrimU, '}'))
			{
			// The separator was actually a terminator, and we're done.
			return;
			}
		}

	if (!spTryRead_PropertyName(theStrimU, oName, fReadOptions.fAllowUnquotedPropertyNames.DGet(false)))
		spThrowParseException("Expected a member name");

	sSkip_WSAndCPlusPlusComments(theStrimU);

	if (!sTryRead_CP(theStrimU, ':'))
		{
		if (!fReadOptions.fAllowEquals.DGet(false) || !sTryRead_CP(theStrimU, '='))
			spThrowParseException("Expected ':' after a member name");
		}

	if (!(oYadR = spMakeYadR_JSON(fStrimmerU, fReadOptions)))
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
	if (false)
		{}
	else if (not iAny)
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
		s << "null /*!! Unhandled: " << iAny.Type().name() << " !!*/";
		}
	}

// =================================================================================================
#pragma mark -
#pragma mark * Visitor_Writer::SaveState

class Visitor_Writer::SaveState
	{
public:
	SaveState(Visitor_Writer* iVisitor);
	~SaveState();

	Visitor_Writer* fVisitor;
	size_t fIndent;
	bool fMayNeedInitialLF;
	};

Visitor_Writer::SaveState::SaveState(Visitor_Writer* iVisitor)
:	fVisitor(iVisitor),
	fIndent(fVisitor->fIndent),
	fMayNeedInitialLF(fVisitor->fMayNeedInitialLF)
	{}

Visitor_Writer::SaveState::~SaveState()
	{
	fVisitor->fIndent = fIndent;
	fVisitor->fMayNeedInitialLF = fMayNeedInitialLF;
	}

// =================================================================================================
#pragma mark -
#pragma mark * Visitor_Writer

Visitor_Writer::Visitor_Writer
	(size_t iIndent, const ZYadOptions& iOptions, const ZStrimW& iStrimW)
:	fIndent(iIndent),
	fOptions(iOptions),
	fStrimW(iStrimW),
	fMayNeedInitialLF(false)
	{}

void Visitor_Writer::Visit_YadR(const ZRef<ZYadR>& iYadR)
	{
	fStrimW << "null";
	if (fOptions.fBreakStrings)
		fStrimW << " /*!! Unhandled yad !!*/";
	}

void Visitor_Writer::Visit_YadAtomR(const ZRef<ZYadAtomR>& iYadAtomR)
	{ spToStrim_SimpleValue(fStrimW, iYadAtomR->AsAny()); }

void Visitor_Writer::Visit_YadStreamR(const ZRef<ZYadStreamR>& iYadStreamR)
	{
	fStrimW << "null";
	if (fOptions.fBreakStrings)
		fStrimW << " /*!! ZYadStreamR not representable in JSON !!*/";
	}

void Visitor_Writer::Visit_YadStrimR(const ZRef<ZYadStrimR>& iYadStrimR)
	{ spWriteString(fStrimW, iYadStrimR->GetStrimR()); }

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
				if (not isFirst)
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
				if (not isFirst)
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
#pragma mark * sYadR and sToStrim

ZRef<ZYadR> sYadR(ZRef<ZStrimmerU> iStrimmerU)
	{ return spMakeYadR_JSON(iStrimmerU, ReadOptions()); }

ZRef<ZYadR> sYadR(ZRef<ZStrimmerU> iStrimmerU, const ReadOptions& iReadOptions)
	{ return spMakeYadR_JSON(iStrimmerU, iReadOptions); }

void sToStrim(ZRef<ZYadR> iYadR, const ZStrimW& s)
	{ sToStrim(0, ZYadOptions(), iYadR, s); }

void sToStrim(size_t iInitialIndent, const ZYadOptions& iOptions,
	ZRef<ZYadR> iYadR, const ZStrimW& s)
	{
	Visitor_Writer theWriter(iInitialIndent, iOptions, s);
	iYadR->Accept(theWriter);
	}

} // namespace ZYad_JSON
} // namespace ZooLib
