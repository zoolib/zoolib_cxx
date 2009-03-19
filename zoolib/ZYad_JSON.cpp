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
#include "zoolib/ZFactoryChain.h"
#include "zoolib/ZStrim_Escaped.h"
#include "zoolib/ZTuple.h"
#include "zoolib/ZUtil_Strim.h"
#include "zoolib/ZYad_JSON.h"
#include "zoolib/ZYad_StdMore.h"

NAMESPACE_ZOOLIB_BEGIN

using std::string;
using std::vector;

/*! \namespace ZYad_JSON
JSON is JavaScript Object Notation. See <http://www.crockford.com/JSON/index.html>.

ZYad_JSON provides Yad facilities to read and write JSON source.

ZTuple is isomorphic to JSON's object, vector<ZTValue> to JSON's array, and strings, booleans
and null translate back and forth without trouble. JSON's only other primitive is
the number, whereas ZTValue explicitly stores and retrieves integers of different sizes,
floats and doubles, raw bytes and other composite types.

ZYad_JSON writes all ZTValue number types as JSON numbers. When reading, JSON numbers
are stored as int64s, unless the mantissa has a fractional component or exceeds 2^64,
or if there is an exponent,in which case a double is used.

The mappings are as follows:

<pre>

JSON          ZTValue
----          -------
null          null
object        ZTuple
array         vector<ZTValue>
boolean       bool
string        string
number        int64 or double


ZTValue          JSON
-------          ----
null             null
ZTuple           object
vector<ZTValue>  array
bool             boolean
string           string
int8             number
int16            number
int32            number
int64            number
float            number
double           number
uint64 (ID)      number
time             number

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
#pragma mark * ZYadStrimR_JSONNormalize

class ZYadStrimR_JSONNormalize
:	public ZYadR_Std,
	public ZYadStrimR
	{
public:
	ZYadStrimR_JSONNormalize(ZRef<ZYadStrimR> iSource);

// From ZYadR_Std
	virtual void Finish();

// From ZStrimmerR via ZYadStrimR
	const ZStrimR& GetStrimR();

private:
	ZRef<ZYadStrimR> fSource;
	};

ZYadStrimR_JSONNormalize::ZYadStrimR_JSONNormalize(ZRef<ZYadStrimR> iSource)
:	fSource(iSource)
	{}

void ZYadStrimR_JSONNormalize::Finish()
	{}

const ZStrimR& ZYadStrimR_JSONNormalize::GetStrimR()
	{ return fSource->GetStrimR(); }

// =================================================================================================
#pragma mark -
#pragma mark * Helpers

static void sThrowParseException(const string& iMessage)
	{
	throw ZYadParseException_JSON(iMessage);
	}

static bool sTryRead_JSONString(const ZStrimU& s, string& oString)
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

static bool sTryRead_NumberOrKeyword(const ZStrimU& s, string& oString)
	{
	for (bool gotAny = false; /*no test*/; gotAny = true)
		{
		UTF32 theCP;
		if (!s.ReadCP(theCP))
			return gotAny;

		if (!ZUnicode::sIsAlphaDigit(theCP)
			&& theCP != '.'
			&& theCP != '-')
			{
			s.Unread();
			return gotAny;
			}
		oString += theCP;
		}
	}

// Read the mantissa, putting it into oInt64 and oDouble and setting oIsDouble if
// it won't fit in 64 bits or is an inf. We consume any sign and set oIsNegative appropriately.
static bool sTryRead_Mantissa(const ZStrimU& s,
	int64& oInt64, double& oDouble, bool& oIsNegative, bool& oIsDouble)
	{
	using namespace ZUtil_Strim;

	oInt64 = 0;
	oDouble = 0;
	oIsNegative = false;
	oIsDouble = false;

	bool hadPrefix = true;
	if (sTryRead_CP(s, '-'))
		oIsNegative = true;
	else if (!sTryRead_CP(s, '+'))
		hadPrefix = false;

	// Unlike ZUtil_Strim, we don't read nan here. The first char of nan is the same
	// as that of null, and so we'd throw a parse error. Instead we handle it in sFromStrim.
	if (sTryRead_CP(s, 'i') || sTryRead_CP(s, 'I'))
		{
		if (sTryRead_CP(s, 'n') || sTryRead_CP(s, 'F'))
			{
			if (sTryRead_CP(s, 'f') || sTryRead_CP(s, 'F'))
				{
				// It's an inf
				oIsDouble = true;
				oDouble = INFINITY;
				return true;
				}
			}
		throw ParseException("Illegal character when trying to read a double");
		}

	for (bool gotAny = false; /*no test*/; gotAny = true)
		{
		UTF32 curCP;
		if (!s.ReadCP(curCP) || curCP == '.' || !ZUnicode::sIsDigit(curCP))
			{
			if (curCP == '.')
				{
				s.Unread();
				oIsDouble = true;
				return true;
				}

			if (gotAny)
				{	
				return true;
				}
			else
				{
				if (hadPrefix)
					{
					// We've already absorbed a plus or minus sign, hence we have a parse exception.
					if (oIsNegative)
						throw ParseException("Expected a valid number after '-' prefix");
					else
						throw ParseException("Expected a valid number after '+' prefix");
					}
				return false;
				}
			}

		int digit = curCP - '0';
		if (!oIsDouble)
			{
			int64 priorInt64 = oInt64;
			oInt64 *= 10;
			oInt64 += digit;
			if (oInt64 < priorInt64)
				oIsDouble = true;
			}
		oDouble *= 10;
		oDouble += digit;
		}
	}

static bool sTryRead_Number(const ZStrimU& s, ZTValue& oTV)
	{
	using namespace ZUtil_Strim;

	int64 asInt64;
	double asDouble;
	bool isNegative;
	bool isDouble;
	if (!sTryRead_Mantissa(s, asInt64, asDouble, isNegative, isDouble))
		return false;

	if (sTryRead_CP(s, '.'))
		{
		// sTryRead_Mantissa must have already set isDouble.
		ZAssert(isDouble);
		double fracPart = 0.0;
		double divisor = 1.0;
		for (;;)
			{
			UTF32 curCP;
			if (!s.ReadCP(curCP))
				break;
			divisor *= 10;
			fracPart *= 10;
			fracPart += (curCP - '0');
			}
		asDouble += fracPart / divisor;
		}

	if (isNegative)
		{
		asInt64 = -asInt64;
		asDouble = -asDouble;
		}

	if (sTryRead_CP(s, 'e') || sTryRead_CP(s, 'E'))
		{
		isDouble = true;
		int64 exponent;
		if (!sTryRead_SignedDecimalInteger(s, exponent))
			throw ParseException("Expected a valid exponent after 'e'");
		asDouble = asDouble * pow(10.0, int(exponent));
		}

	if (isDouble)
		oTV.SetDouble(asDouble);
	else
		oTV.SetInt64(asInt64);

	return true;
	}

static bool sFromStrim_TValue(const ZStrimU& iStrimU, ZTValue& oTV)
	{
	string theString;
	if (sTryRead_JSONString(iStrimU, theString))
		{
		oTV.SetString(theString);
		}
	else
		{
		if (!sTryRead_NumberOrKeyword(iStrimU, theString))
			return false;

		if (!sTryRead_Number(ZStrimU_String(theString), oTV))
			{
			string lowerString = ZUnicode::sToLower(theString);
			if (lowerString == "nan")
				{
				oTV.SetDouble(NAN);
				}
			else if (lowerString == "null")
				{
				oTV.SetNull();
				}
			else if (lowerString == "false")
				{
				oTV.SetBool(false);
				}
			else if (lowerString == "true")
				{
				oTV.SetBool(true);
				}
			else
				{
				sThrowParseException("Unknown keyword " + theString);
				}
			}
		}
	return true;
	}

static bool sNormalizeSimpleTValue(const ZTValue& iTV, ZTValue& oTV)
	{
	ZAssert(&iTV != &oTV);
	switch (iTV.TypeOf())
		{
		case eZType_String:
		case eZType_Int64:
		case eZType_Double:
		case eZType_Bool:
		case eZType_Null:
			{
			oTV = iTV;
			return true;
			}
		case eZType_Int8:
			{
			oTV.SetInt64(iTV.GetInt8());
			return true;
			}
		case eZType_Int16:
			{
			oTV.SetInt64(iTV.GetInt16());
			return true;
			}
		case eZType_Int32:
			{
			oTV.SetInt64(iTV.GetInt32());
			return true;
			}
		case eZType_Float:
			{
			oTV.SetDouble(iTV.GetFloat());
			return true;
			}
		case eZType_ID:
			{
			oTV.SetInt64(iTV.GetID());
			return true;
			}
		case eZType_Time:
			{
			oTV.SetDouble(iTV.GetTime().fVal);
			return true;
			}
		}
	return false;
	}

static ZRef<ZYadR_Std> sMakeYadR_JSON(const ZStrimU& iStrimU)
	{
	using namespace ZUtil_Strim;

	sSkip_WSAndCPlusPlusComments(iStrimU);
	
	if (sTryRead_CP(iStrimU, '['))
		{
		return new ZYadListR_JSON(iStrimU);
		}
	else if (sTryRead_CP(iStrimU, '{'))
		{
		return new ZYadMapR_JSON(iStrimU);
		}
	else if (sTryRead_CP(iStrimU, '"'))
		{
		return new ZYadStrimR_JSON(iStrimU);
		}
	else
		{
		ZTValue theTV;
		if (sFromStrim_TValue(iStrimU, theTV))
			return new ZYadPrimR_Std(theTV);
		}

	return ZRef<ZYadPrimR_Std>();
	}

static ZRef<ZYadR_Std> sMakeYadR_JSONNormalize(
	ZRef<ZYadR> iYadR, bool iPreserve, bool iPreserveLists, bool iPreserveMaps)
	{
	if (ZRef<ZYadListR> theYadListR = ZRefDynamicCast<ZYadListR>(iYadR))
		{
		return new ZYadListR_JSONNormalize(theYadListR, iPreserveLists, iPreserveMaps);
		}
	else if (ZRef<ZYadMapR> theYadMapR = ZRefDynamicCast<ZYadMapR>(iYadR))
		{
		return new ZYadMapR_JSONNormalize(theYadMapR, iPreserveLists, iPreserveMaps);
		}
	else if (ZRef<ZYadStrimR> theYadStrimR = ZRefDynamicCast<ZYadStrimR>(iYadR))
		{
		if (ZRef<ZYadR_Std> theYadR_Std = ZRefDynamicCast<ZYadR_Std>(theYadStrimR))
			return theYadR_Std;
		return new ZYadStrimR_JSONNormalize(theYadStrimR);
		}
	else
		{
		ZTValue theValue;
		if (ZFactoryChain_T<ZTValue, ZRef<ZYadR> >::sMake(theValue, iYadR))
			{
			// We were able to turn the value into something
			// legitimate. Now normalize it if possible.
			ZTValue normalized;
			if (sNormalizeSimpleTValue(theValue, normalized))
				return new ZYadPrimR_Std(normalized);
			}

		if (iPreserve)
			{
			// We weren't able to get a ZTValue, or it couldn't normalized.
			// We've been asked to preserve values, so return a null.
			return new ZYadPrimR_Std(ZTValue());
			}
		}

	return ZRef<ZYadPrimR_Std>();
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

ZYadStrimR_JSON::ZYadStrimR_JSON(const ZStrimU& iStrimU)
:	fStrimU(iStrimU),
	fStrimR(iStrimU, '"')
	{}

void ZYadStrimR_JSON::Finish()
	{
	using namespace ZUtil_Strim;
	fStrimR.SkipAll();
	if (!sTryRead_CP(fStrimU, '"'))
		throw ParseException("Missing string delimiter");
	}

const ZStrimR& ZYadStrimR_JSON::GetStrimR()
	{ return fStrimR; }

// =================================================================================================
#pragma mark -
#pragma mark * ZYadListR_JSON

ZYadListR_JSON::ZYadListR_JSON(const ZStrimU& iStrimU)
:	fStrimU(iStrimU)
	{}

void ZYadListR_JSON::Imp_ReadInc(bool iIsFirst, ZRef<ZYadR_Std>& oYadR)
	{
	using namespace ZUtil_Strim;

	sSkip_WSAndCPlusPlusComments(fStrimU);

	if (sTryRead_CP(fStrimU, ']'))
		{
		// Reached end.
		return;
		}

	if (!iIsFirst)
		{
		// Must read a separator
		if (!sTryRead_CP(fStrimU, ','))
			sThrowParseException("Require ',' to separate array elements");
		sSkip_WSAndCPlusPlusComments(fStrimU);
		}

	if (!(oYadR = sMakeYadR_JSON(fStrimU)))
		sThrowParseException("Expected a value");
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZYadMapR_JSON

ZYadMapR_JSON::ZYadMapR_JSON(const ZStrimU& iStrimU)
:	fStrimU(iStrimU)
	{}

void ZYadMapR_JSON::Imp_ReadInc(bool iIsFirst, std::string& oName, ZRef<ZYadR_Std>& oYadR)
	{
	using namespace ZUtil_Strim;

	sSkip_WSAndCPlusPlusComments(fStrimU);

	if (sTryRead_CP(fStrimU, '}'))
		{
		// Reached end.
		return;
		}

	if (!iIsFirst)
		{
		// Must read a separator
		if (!sTryRead_CP(fStrimU, ','))
			sThrowParseException("Require ',' to separate array elements");
		sSkip_WSAndCPlusPlusComments(fStrimU);
		}

	if (!sTryRead_JSONString(fStrimU, oName))
		sThrowParseException("Expected a member name");

	sSkip_WSAndCPlusPlusComments(fStrimU);

	if (!sTryRead_CP(fStrimU, ':'))
		sThrowParseException("Expected ':' after a member name");

	if (!(oYadR = sMakeYadR_JSON(fStrimU)))
		sThrowParseException("Expected value after ':'");
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZYadListR_JSONNormalize

ZYadListR_JSONNormalize::ZYadListR_JSONNormalize(
	ZRef<ZYadListR> iYadListR, bool iPreserveLists, bool iPreserveMaps)
:	fYadListR(iYadListR),
	fPreserveLists(iPreserveLists),
	fPreserveMaps(iPreserveMaps)
	{}

void ZYadListR_JSONNormalize::Imp_ReadInc(bool iIsFirst, ZRef<ZYadR_Std>& oYadR)
	{
	for (;;)
		{
		ZRef<ZYadR> next = fYadListR->ReadInc();
		if (!next)
			break;
		oYadR = sMakeYadR_JSONNormalize(
			next, fPreserveLists, fPreserveLists, fPreserveMaps);
		if (oYadR)
			break;
		}
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZYadMapR_JSONNormalize

ZYadMapR_JSONNormalize::ZYadMapR_JSONNormalize(
	ZRef<ZYadMapR> iYadMapR, bool iPreserveLists, bool iPreserveMaps)
:	fYadMapR(iYadMapR),
	fPreserveLists(iPreserveLists),
	fPreserveMaps(iPreserveMaps)
	{}

void ZYadMapR_JSONNormalize::Imp_ReadInc(bool iIsFirst, std::string& oName, ZRef<ZYadR_Std>& oYadR)
	{
	for (;;)
		{
		ZRef<ZYadR> next = fYadMapR->ReadInc(oName);
		if (!next)
			break;
		oYadR = sMakeYadR_JSONNormalize(
			next, fPreserveMaps, fPreserveLists, fPreserveMaps);
		if (oYadR)
			break;
		}
	}

// =================================================================================================
#pragma mark -
#pragma mark * Static writing functions

static void sWriteIndent(const ZStrimW& iStrimW,
	size_t iCount, const ZYadOptions& iOptions)
	{
	while (iCount--)
		iStrimW.Write(iOptions.fIndentString);
	}

static void sWriteLFIndent(const ZStrimW& iStrimW,
	size_t iCount, const ZYadOptions& iOptions)
	{
	iStrimW.Write(iOptions.fEOLString);
	sWriteIndent(iStrimW, iCount, iOptions);
	}

static void sWriteString(const ZStrimW& s, const string& iString)
	{
	s.Write("\"");

	ZStrimW_Escaped::Options theOptions;
	theOptions.fQuoteQuotes = true;
	theOptions.fEscapeHighUnicode = false;
	
	ZStrimW_Escaped(theOptions, s).Write(iString);

	s.Write("\"");
	}

static void sWriteString(const ZStrimW& s, const ZStrimR& iStrimR)
	{
	s.Write("\"");

	ZStrimW_Escaped::Options theOptions;
	theOptions.fQuoteQuotes = true;
	theOptions.fEscapeHighUnicode = false;
	
	ZStrimW_Escaped(theOptions, s).CopyAllFrom(iStrimR);

	s.Write("\"");
	}

static void sToStrim_SimpleTValue(const ZStrimW& s, const ZTValue& iTV,
	size_t iLevel, const ZYadOptions& iOptions)
	{
	ZTValue normalized;
	sNormalizeSimpleTValue(iTV, normalized);

	switch (normalized.TypeOf())
		{
		case eZType_String:
			{
			sWriteString(s, normalized.GetString());
			break;
			}
		case eZType_Int64: 
			{
			s.Writef("%lld", normalized.GetInt64());
			break;
			}
		case eZType_Bool:
			{
			if (normalized.GetBool())
				s.Write("true");
			else
				s.Write("false");
			break;
			}
		case eZType_Double:
			{
			// 17 decimal digits are necessary and sufficient for double precision IEEE 754.
			// <http://docs.sun.com/source/806-3568/ncg_goldberg.html>
			s.Writef("%.17g", normalized.GetDouble());
			break;
			}
		case eZType_Null:
		default:
			{
			s.Write("null");
			break;
			}
		}
	}

static void sToStrim_Yad(const ZStrimW& s, ZRef<ZYadR> iYadR,
	size_t iInitialIndent, const ZYadOptions& iOptions, bool iMayNeedInitialLF);

static void sToStrim_List(const ZStrimW& s, ZRef<ZYadListR> iYadListR,
	size_t iLevel, const ZYadOptions& iOptions, bool iMayNeedInitialLF)
	{
	bool needsIndentation = false;
	if (iOptions.DoIndentation())
		{
		// We're supposed to be indenting if we're complex, ie if any element is:
		// 1. A non-empty vector.
		// 2. A non-empty tuple.
		// or if iOptions.fBreakStrings is true, any element is a string with embedded
		// line breaks or more than iOptions.fStringLineLength characters.
		needsIndentation = !iYadListR->IsSimple(iOptions);
		}

	if (needsIndentation)
		{
		// We need to indent.
		if (iMayNeedInitialLF)
			{
			// We were invoked by a tuple which has already issued the property
			// name and equals sign, so we need to start a fresh line.
			sWriteLFIndent(s, iLevel, iOptions);
			}

		s.Write("[");
		for (bool isFirst = true; /*no test*/ ; isFirst = false)
			{
			if (ZRef<ZYadR> cur = iYadListR->ReadInc())
				{
				if (!isFirst)
					s.Write(",");
				sWriteLFIndent(s, iLevel, iOptions);
				sToStrim_Yad(s, cur, iLevel, iOptions, false);
				}
			else
				{
				break;
				}
			}
		sWriteLFIndent(s, iLevel, iOptions);
		s.Write("]");
		}
	else
		{
		// We're not indenting, so we can just dump everything out on
		// one line, with just some spaces to keep things legible.
		s.Write("[");
		for (bool isFirst = true; /*no test*/ ; isFirst = false)
			{
			if (ZRef<ZYadR> cur = iYadListR->ReadInc())
				{
				if (!isFirst)
					s.Write(", ");
				sToStrim_Yad(s, cur, iLevel, iOptions, false);
				}
			else
				{
				break;
				}
			}
		s.Write("]");
		}
	}

static void sToStrim_Map(const ZStrimW& s, ZRef<ZYadMapR> iYadMapR,
	size_t iLevel, const ZYadOptions& iOptions, bool iMayNeedInitialLF)
	{
	bool needsIndentation = false;
	if (iOptions.DoIndentation())
		{
		needsIndentation = ! iYadMapR->IsSimple(iOptions);
		}

	if (needsIndentation)
		{
		if (iMayNeedInitialLF)
			{
			// We're going to be indenting, but need to start
			// a fresh line to have our { and contents line up.
			sWriteLFIndent(s, iLevel, iOptions);
			}

		s.Write("{");
		for (bool isFirst = true; /*no test*/ ; isFirst = false)
			{
			string curName;
			if (ZRef<ZYadR> cur = iYadMapR->ReadInc(curName))
				{
				if (!isFirst)
					s.Write(",");
				sWriteLFIndent(s, iLevel, iOptions);
				sWriteString(s, curName);
				s << " : ";
				sToStrim_Yad(s, cur, iLevel + 1, iOptions, true);
				}
			else
				{
				break;
				}
			}
		sWriteLFIndent(s, iLevel, iOptions);
		s.Write("}");
		}
	else
		{
		s.Write("{");
		for (bool isFirst = true; /*no test*/ ; isFirst = false)
			{
			string curName;
			if (ZRef<ZYadR> cur = iYadMapR->ReadInc(curName))
				{
				if (!isFirst)
					s.Write(",");
				s.Write(" ");
				sWriteString(s, curName);
				s << " : ";
				sToStrim_Yad(s, cur, iLevel + 1, iOptions, true);
				}
			else
				{
				break;
				}
			}
		s.Write(" }");
		}
	}

static void sToStrim_Yad(const ZStrimW& s, ZRef<ZYadR> iYadR,
	size_t iLevel, const ZYadOptions& iOptions, bool iMayNeedInitialLF)
	{
	if (!iYadR)
		{
		return;
		}
	else if (ZRef<ZYadMapR> theYadMapR = ZRefDynamicCast<ZYadMapR>(iYadR))
		{
		sToStrim_Map(s, theYadMapR, iLevel, iOptions, iMayNeedInitialLF);
		}
	else if (ZRef<ZYadListR> theYadListR = ZRefDynamicCast<ZYadListR>(iYadR))
		{
		sToStrim_List(s, theYadListR, iLevel, iOptions, iMayNeedInitialLF);
		}
	else if (ZRef<ZYadStrimR> theYadStrimR = ZRefDynamicCast<ZYadStrimR>(iYadR))
		{
		sWriteString(s, theYadStrimR->GetStrimR());
		}
	else
		{
		sToStrim_SimpleTValue(s, ZYad_ZooLib::sFromYadR(iYadR), iLevel, iOptions);
		}
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZYad_JSON

ZRef<ZYadR> ZYad_JSON::sMakeYadR(const ZStrimU& iStrimU)
	{ return sMakeYadR_JSON(iStrimU); }

ZRef<ZYadR> ZYad_JSON::sMakeYadR_Normalize(
	ZRef<ZYadR> iYadR, bool iPreserveLists, bool iPreserveMaps)
	{ return sMakeYadR_JSONNormalize(iYadR, true, iPreserveLists, iPreserveMaps); }

void ZYad_JSON::sToStrim(const ZStrimW& s, ZRef<ZYadR> iYadR)
	{ sToStrim_Yad(s, iYadR, 0, ZYadOptions(), false); }

void ZYad_JSON::sToStrim(const ZStrimW& s, ZRef<ZYadR> iYadR,
	size_t iInitialIndent, const ZYadOptions& iOptions)
	{ sToStrim_Yad(s, iYadR, iInitialIndent, iOptions, false); }


NAMESPACE_ZOOLIB_END
