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
#include "zoolib/ZStrimW_Escapify.h"
#include "zoolib/ZTuple.h"
#include "zoolib/ZUtil_Strim.h"
#include "zoolib/ZYad_JSON.h"

NAMESPACE_ZOOLIB_BEGIN

using std::string;
using std::vector;

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

static ZRef<ZYadR_JSON> sMakeYadR_JSON(const ZStrimU& iStrimU)
	{
	using namespace ZUtil_Strim;

	sSkip_WSAndCPlusPlusComments(iStrimU);
	
	if (sTryRead_CP(iStrimU, '['))
		{
		return new ZYadListR_JSON(iStrimU, true);
		}
	else if (sTryRead_CP(iStrimU, '{'))
		{
		return new ZYadMapR_JSON(iStrimU, true);
		}
	else
		{
		ZTValue theTV;
		if (sFromStrim_TValue(iStrimU, theTV))
			return new ZYadPrimR_JSON(theTV);
		}

	return ZRef<ZYadR_JSON>();
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZYadParseException_JSON

ZYadParseException_JSON::ZYadParseException_JSON(const string& iWhat)
:	ZYadParseException(iWhat)
	{}

ZYadParseException_JSON::ZYadParseException_JSON(const char* iWhat)
:	ZYadParseException(iWhat)
	{}

// =================================================================================================
#pragma mark -
#pragma mark * ZYadPrimR_JSON

ZYadPrimR_JSON::ZYadPrimR_JSON(const ZTValue& iTV)
:	ZYadR_TValue(iTV)
	{}

void ZYadPrimR_JSON::Finish()
	{}

// =================================================================================================
#pragma mark -
#pragma mark * ZYadListR_JSON

ZYadListR_JSON::ZYadListR_JSON(const ZStrimU& iStrimU, bool iReadDelimiter)
:	fStrimU(iStrimU),
	fReadDelimiter(iReadDelimiter),
	fPosition(0)
	{}

void ZYadListR_JSON::Finish()
	{
	using namespace ZUtil_Strim;

	this->SkipAll();

	if (fReadDelimiter)
		{
		sSkip_WSAndCPlusPlusComments(fStrimU);
		if (!sTryRead_CP(fStrimU, ']'))
			sThrowParseException("Expected ']' to close an array");
		}
	}

bool ZYadListR_JSON::HasChild()
	{
	this->pMoveIfNecessary();
	return fValue_Current;
	}

ZRef<ZYadR> ZYadListR_JSON::NextChild()
	{
	this->pMoveIfNecessary();

	if (fValue_Current)
		{
		fValue_Prior = fValue_Current;
		fValue_Current.Clear();
		++fPosition;
		}

	return fValue_Prior;
	}

size_t ZYadListR_JSON::GetPosition()
	{ return fPosition; }

void ZYadListR_JSON::pMoveIfNecessary()
	{
	using namespace ZUtil_Strim;

	if (fValue_Current)
		return;

	if (fValue_Prior)
		{
		fValue_Prior->Finish();
		fValue_Prior.Clear();
		sSkip_WSAndCPlusPlusComments(fStrimU);
		if (!sTryRead_CP(fStrimU, ','))
			return;
		}

	fValue_Current = sMakeYadR_JSON(fStrimU);
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZYadMapR_JSON

ZYadMapR_JSON::ZYadMapR_JSON(const ZStrimU& iStrimU, bool iReadDelimiter)
:	fStrimU(iStrimU),
	fReadDelimiter(iReadDelimiter)
	{}
	
void ZYadMapR_JSON::Finish()
	{
	using namespace ZUtil_Strim;

	this->SkipAll();

	if (fReadDelimiter)
		{
		sSkip_WSAndCPlusPlusComments(fStrimU);
		if (!sTryRead_CP(fStrimU, '}'))
			sThrowParseException("Expected '}' to close an object");
		}
	}

bool ZYadMapR_JSON::HasChild()
	{
	this->pMoveIfNecessary();

	return fValue_Current;
	}

ZRef<ZYadR> ZYadMapR_JSON::NextChild()
	{
	this->pMoveIfNecessary();

	if (fValue_Current)
		{
		fValue_Prior = fValue_Current;
		fValue_Current.Clear();
		fName.clear();
		}

	return fValue_Prior;
	}

string ZYadMapR_JSON::Name()
	{
	this->pMoveIfNecessary();

	return fName;
	}

void ZYadMapR_JSON::pMoveIfNecessary()
	{
	using namespace ZUtil_Strim;

	if (fValue_Current)
		return;

	if (fValue_Prior)
		{
		fValue_Prior->Finish();
		fValue_Prior.Clear();

		sSkip_WSAndCPlusPlusComments(fStrimU);

		if (!sTryRead_CP(fStrimU, ','))
			return;
//			sThrowParseException("Expected ',' after property");
		}

	sSkip_WSAndCPlusPlusComments(fStrimU);

	if (!sTryRead_JSONString(fStrimU, fName))
		return;

	sSkip_WSAndCPlusPlusComments(fStrimU);

	if (!sTryRead_CP(fStrimU, ':'))
		sThrowParseException("Expected ':' after a member name");

	fValue_Current = sMakeYadR_JSON(fStrimU);
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

static void sWriteString(
	const ZStrimW& s, const ZYadOptions& iOptions, const string& iString)
	{
	s.Write("\"");

	ZStrimW_Escapify::Options theOptions;
	theOptions.fQuoteQuotes = true;
	theOptions.fEscapeHighUnicode = false;
	
	ZStrimW_Escapify(theOptions, s).Write(iString);

	s.Write("\"");
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

static void sToStrim_SimpleTValue(const ZStrimW& s, const ZTValue& iTV,
	size_t iLevel, const ZYadOptions& iOptions)
	{
	ZTValue normalized;
	sNormalizeSimpleTValue(iTV, normalized);

	switch (normalized.TypeOf())
		{
		case eZType_String:
			{
			sWriteString(s, iOptions, normalized.GetString());
			break;
			}
		case eZType_Int64: 
			{
			s.Writef("%lld", iTV.GetInt64());
			break;
			}
		case eZType_Bool:
			{
			if (iTV.GetBool())
				s.Write("true");
			else
				s.Write("false");
			break;
			}
		case eZType_Double:
			{
			// 17 decimal digits are necessary and sufficient for double precision IEEE 754.
			// <http://docs.sun.com/source/806-3568/ncg_goldberg.html>
			s.Writef("%.17g", iTV.GetDouble());
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
	if (!iYadListR->HasChild())
		{
		// We've got an empty list.
		s.Write("[]");
		return;
		}
		
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
		for (;;)
			{
			sWriteLFIndent(s, iLevel, iOptions);
			sToStrim_Yad(s, iYadListR->NextChild(), iLevel, iOptions, false);
			if (!iYadListR->HasChild())
				break;
			s.Write(", ");
			}
		sWriteLFIndent(s, iLevel, iOptions);
		s.Write("]");
		}
	else
		{
		// We're not indenting, so we can just dump everything out on
		// one line, with just some spaces to keep things legible.
		s.Write("[");
		for (;;)
			{
			sToStrim_Yad(s, iYadListR->NextChild(), iLevel, iOptions, false);
			if (!iYadListR->HasChild())
				break;
			s.Write(",");
			}
		s.Write("]");
		}
	}

static void sToStrim_Map(const ZStrimW& s, ZRef<ZYadMapR> iYadMapR,
	size_t iLevel, const ZYadOptions& iOptions, bool iMayNeedInitialLF)
	{
	if (!iYadMapR->HasChild())
		{
		// We've got an empty object.
		s.Write("{}");
		return;
		}

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
		for (;;)
			{
			sWriteLFIndent(s, iLevel, iOptions);

			sWriteString(s, iOptions, iYadMapR->Name());

			s << " : ";

			sToStrim_Yad(s, iYadMapR->NextChild(), iLevel + 1, iOptions, true);
			if (!iYadMapR->HasChild())
				break;
			s.Write(",");
			}

		sWriteLFIndent(s, iLevel, iOptions);

		s.Write("}");
		}
	else
		{
		s.Write("{");
		for (;;)
			{
			sWriteString(s, iOptions, iYadMapR->Name());

			s << ":";

			sToStrim_Yad(s, iYadMapR->NextChild(), iLevel + 1, iOptions, true);
			if (!iYadMapR->HasChild())
				break;
			s.Write(",");
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

void ZYad_JSON::sToStrim(const ZStrimW& s, ZRef<ZYadR> iYadR)
	{ sToStrim_Yad(s, iYadR, 0, ZYadOptions(), false); }

void ZYad_JSON::sToStrim(const ZStrimW& s, ZRef<ZYadR> iYadR,
	size_t iInitialIndent, const ZYadOptions& iOptions)
	{ sToStrim_Yad(s, iYadR, iInitialIndent, iOptions, false); }

NAMESPACE_ZOOLIB_END
