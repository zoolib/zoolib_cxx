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
#include "zoolib/ZFunctionChain.h"
#include "zoolib/ZStrim_Escaped.h"
#include "zoolib/ZUtil_Strim.h"
#include "zoolib/ZYad_JSON.h"
#include "zoolib/ZYad_StdMore.h"

NAMESPACE_ZOOLIB_BEGIN

using std::string;
using std::vector;

/*! \namespace ZYad_JSON
JSON is JavaScript Object Notation. See <http://www.crockford.com/JSON/index.html>.

ZYad_JSON provides Yad facilities to read and write JSON source.

ZValMap is isomorphic to JSON's object, ZValList to JSON's array, and strings, booleans
and null translate back and forth without trouble. JSON's only other primitive is
the number, whereas ZVal_ZooLib explicitly stores and retrieves integers of different sizes,
floats and doubles, raw bytes and other composite types.

ZYad_JSON writes all ZVal_ZooLib number types as JSON numbers. When reading, JSON numbers
are stored as int64s, unless the mantissa has a fractional component or exceeds 2^64,
or if there is an exponent,in which case a double is used.

The mappings are as follows:

<pre>

JSON          ZVal_ZooLib
----          -------
null          null
object        ZValMap_ZooLib
array         ZValList_ZooLib
boolean       bool
string        string
number        int64 or double


ZVal_ZooLib          JSON
-------          ----
null             null
ZValMap_ZooLib   object
ZValList_ZooLib  array
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

static bool sFromStrim_Value(const ZStrimU& iStrimU, ZVal_ZooLib& oVal)
	{
	using namespace ZUtil_Strim;

	string theString;

	int64 asInt64;
	double asDouble;
	bool isDouble;

	if (sTryRead_JSONString(iStrimU, theString))
		{
		oVal.SetString(theString);
		}
	else if (sTryRead_SignedDecimalNumber(iStrimU, asInt64, asDouble, isDouble))
		{
		if (isDouble)
			oVal.SetDouble(asDouble);
		else
			oVal.SetInt64(asInt64);
		}
	else if (sTryRead_CaselessString(iStrimU, "null"))
		{
		oVal = ZVal_ZooLib();
		}
	else if (sTryRead_CaselessString(iStrimU, "false"))
		{
		oVal.SetBool(false);
		}
	else if (sTryRead_CaselessString(iStrimU, "true"))
		{
		oVal.SetBool(true);
		}
	else
		{
		sThrowParseException("Expected number, string or keyword");
		}

	return true;
	}

static bool sNormalizeSimpleValue(const ZVal_ZooLib& iVal, ZVal_ZooLib& oVal)
	{
	ZAssert(&iVal != &oVal);
	switch (iVal.TypeOf())
		{
		case eZType_String:
		case eZType_Int64:
		case eZType_Double:
		case eZType_Bool:
		case eZType_Null:
			{
			oVal = iVal;
			return true;
			}
		case eZType_Int8:
			{
			oVal.SetInt64(iVal.GetInt8());
			return true;
			}
		case eZType_Int16:
			{
			oVal.SetInt64(iVal.GetInt16());
			return true;
			}
		case eZType_Int32:
			{
			oVal.SetInt64(iVal.GetInt32());
			return true;
			}
		case eZType_Float:
			{
			oVal.SetDouble(iVal.GetFloat());
			return true;
			}
		case eZType_ID:
			{
			oVal.SetInt64(iVal.GetID());
			return true;
			}
		case eZType_Time:
			{
			oVal.SetDouble(iVal.GetTime().fVal);
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
		ZVal_ZooLib theVal;
		if (sFromStrim_Value(iStrimU, theVal))
			return new ZYadPrimR_Std(theVal);
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
		ZVal_ZooLib theValue;
		if (ZFunctionChain_T<ZVal_ZooLib, ZRef<ZYadR> >::sInvoke(theValue, iYadR))
			{
			// We were able to turn the value into something
			// legitimate. Now normalize it if possible.
			ZVal_ZooLib normalized;
			if (sNormalizeSimpleValue(theValue, normalized))
				return new ZYadPrimR_Std(normalized);
			}

		if (iPreserve)
			{
			// We weren't able to get a ZVal_ZooLib, or it couldn't normalized.
			// We've been asked to preserve values, so return a null.
			return new ZYadPrimR_Std(ZVal_ZooLib());
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

static void sToStrim_SimpleValue(const ZStrimW& s, const ZVal_ZooLib& iVal)
	{
	ZVal_ZooLib normalized;
	sNormalizeSimpleValue(iVal, normalized);

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

// =================================================================================================
#pragma mark -
#pragma mark * ZYadVisitor_JSONWriter::SaveState

class ZYadVisitor_JSONWriter::SaveState
	{
public:
	SaveState(ZYadVisitor_JSONWriter* iVisitor);
	~SaveState();

	ZYadVisitor_JSONWriter* fVisitor;
	size_t fIndent;
	bool fMayNeedInitialLF;
	};

ZYadVisitor_JSONWriter::SaveState::SaveState(ZYadVisitor_JSONWriter* iVisitor)
:	fVisitor(iVisitor),
	fIndent(fVisitor->fIndent),
	fMayNeedInitialLF(fVisitor->fMayNeedInitialLF)
	{}

ZYadVisitor_JSONWriter::SaveState::~SaveState()
	{
	fVisitor->fIndent = fIndent;
	fVisitor->fMayNeedInitialLF = fMayNeedInitialLF;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZYadVisitor_JSONWriter

ZYadVisitor_JSONWriter::ZYadVisitor_JSONWriter(
	const ZStrimW& iStrimW, size_t iIndent, const ZYadOptions& iOptions)
:	fStrimW(iStrimW),
	fIndent(iIndent),
	fOptions(iOptions),
	fMayNeedInitialLF(false)
	{}

bool ZYadVisitor_JSONWriter::Visit_YadR(ZRef<ZYadR> iYadR)
	{
	sToStrim_SimpleValue(fStrimW, sFromYadR_T<ZVal_ZooLib>(iYadR));
	return true;
	}

bool ZYadVisitor_JSONWriter::Visit_YadStrimR(ZRef<ZYadStrimR> iYadStrimR)
	{
	sWriteString(fStrimW, iYadStrimR->GetStrimR());
	return true;
	}

bool ZYadVisitor_JSONWriter::Visit_YadListR(ZRef<ZYadListR> iYadListR)
	{
	bool needsIndentation = false;
	if (fOptions.DoIndentation())
		{
		// We're supposed to be indenting if we're complex, ie if any element is:
		// 1. A non-empty vector.
		// 2. A non-empty tuple.
		// or if iOptions.fBreakStrings is true, any element is a string with embedded
		// line breaks or more than iOptions.fStringLineLength characters.
		needsIndentation = ! iYadListR->IsSimple(fOptions);
		}

	if (needsIndentation)
		{
		// We need to indent.
		if (fMayNeedInitialLF)
			{
			// We were invoked by a tuple which has already issued the property
			// name and equals sign, so we need to start a fresh line.
			sWriteLFIndent(fStrimW, fIndent, fOptions);
			}

		SaveState save(this);
		fMayNeedInitialLF = false;

		fStrimW.Write("[");
		for (bool isFirst = true; /*no test*/ ; isFirst = false)
			{
			if (ZRef<ZYadR> cur = iYadListR->ReadInc())
				{
				if (!isFirst)
					fStrimW.Write(",");
				sWriteLFIndent(fStrimW, fIndent, fOptions);
				cur->Accept(*this);
				}
			else
				{
				break;
				}
			}
		sWriteLFIndent(fStrimW, fIndent, fOptions);
		fStrimW.Write("]");
		}
	else
		{
		// We're not indenting, so we can just dump everything out on
		// one line, with just some spaces to keep things legible.
		fStrimW.Write("[");
		for (bool isFirst = true; /*no test*/ ; isFirst = false)
			{
			if (ZRef<ZYadR> cur = iYadListR->ReadInc())
				{
				if (!isFirst)
					fStrimW.Write(", ");
				cur->Accept(*this);
				}
			else
				{
				break;
				}
			}
		fStrimW.Write("]");
		}
	return true;
	}

bool ZYadVisitor_JSONWriter::Visit_YadMapR(ZRef<ZYadMapR> iYadMapR)
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
			sWriteLFIndent(fStrimW, fIndent, fOptions);
			}

		fStrimW.Write("{");
		for (bool isFirst = true; /*no test*/ ; isFirst = false)
			{
			string curName;
			if (ZRef<ZYadR> cur = iYadMapR->ReadInc(curName))
				{
				if (!isFirst)
					fStrimW.Write(",");
				sWriteLFIndent(fStrimW, fIndent, fOptions);
				sWriteString(fStrimW, curName);
				fStrimW << " : ";

				SaveState save(this);
				fIndent = fIndent + 1;
				fMayNeedInitialLF = true;
				cur->Accept(*this);
				}
			else
				{
				break;
				}
			}
		sWriteLFIndent(fStrimW, fIndent, fOptions);
		fStrimW.Write("}");
		}
	else
		{
		fStrimW.Write("{");
		for (bool isFirst = true; /*no test*/ ; isFirst = false)
			{
			string curName;
			if (ZRef<ZYadR> cur = iYadMapR->ReadInc(curName))
				{
				if (!isFirst)
					fStrimW.Write(",");
				fStrimW.Write(" ");
				sWriteString(fStrimW, curName);
				fStrimW << " : ";

				SaveState save(this);
				fIndent = fIndent + 1;
				fMayNeedInitialLF = true;
				cur->Accept(*this);
				}
			else
				{
				break;
				}
			}
		fStrimW.Write(" }");
		}
	return true;
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
	{
	sToStrim(s, iYadR, 0, ZYadOptions());
	}

void ZYad_JSON::sToStrim(const ZStrimW& s, ZRef<ZYadR> iYadR,
	size_t iInitialIndent, const ZYadOptions& iOptions)
	{
	ZYadVisitor_JSONWriter theWriter(s, iInitialIndent, iOptions);
	iYadR->Accept(theWriter);
	}

NAMESPACE_ZOOLIB_END
