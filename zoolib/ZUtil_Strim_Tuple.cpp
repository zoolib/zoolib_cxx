/* -------------------------------------------------------------------------------------------------
Copyright (c) 2003 Andrew Green and Learning in Motion, Inc.
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

#include "zoolib/ZUtil_Strim_Tuple.h"

#include "zoolib/ZDebug.h"
#include "zoolib/ZMemoryBlock.h"
#include "zoolib/ZStream_HexStrim.h"
#include "zoolib/ZStream_Memory.h"
#include "zoolib/ZStrimR_Boundary.h"
#include "zoolib/ZStrimW_Escapify.h"
#include "zoolib/ZString.h"
#include "zoolib/ZTuple.h"
#include "zoolib/ZUtil_Strim.h"
#include "zoolib/ZUtil_Time.h"

#include <cctype>
#include <cmath> // For fmod

using std::pair;
using std::string;
using std::vector;

// =================================================================================================
#pragma mark -
#pragma mark * Static parsing methods.

static int64 sMustRead_GenericInteger(const ZStrimU& iStrimU)
	{
	using namespace ZUtil_Strim;
	int64 theInteger;
	if (!sTryRead_GenericInteger(iStrimU, theInteger))
		throw ParseException("Expected an integer");
	return theInteger;
	}

static void sMustRead_WSCommaWS(const ZStrimU& iStrimU)
	{
	using namespace ZUtil_Strim;
	sSkip_WSAndCPlusPlusComments(iStrimU);

	if (!sTryRead_CP(iStrimU, ','))
		throw ParseException("Expected a ','");

	sSkip_WSAndCPlusPlusComments(iStrimU);
	}

static void sReadQuotedString_Quote(const ZStrimU& iStrimU, ZTValue& oTValue)
	{
	using namespace ZUtil_Strim;

	string theString;
	theString.reserve(100);
	ZStrimW_String theStrimW(theString);

	for (;;)
		{
		// We've read, and could un-read, a quote mark.
		if (sTryRead_CP(iStrimU, '"'))
			{
			// We've now seen a second quote, abutting the first.
			if (sTryRead_CP(iStrimU, '"'))
				{
				// We have three quotes in a row, which opens a verbatim string.
				// If the next character is an EOL then absorb it, so the verbatim
				// text can start on a fresh line, but not be parsed as
				// beginning with an EOL.
				UTF32 theCP = iStrimU.ReadCP();
				if (!ZUnicode::sIsEOL(theCP))
					iStrimU.Unread();

				// Now copy everything till we see three quotes in a row again.
				ZStrimR_Boundary theStrimR_Boundary("\"\"\"", iStrimU);
				theStrimW.CopyAllFrom(theStrimR_Boundary);
				if (!theStrimR_Boundary.HitBoundary())
					throw ParseException("Expected \"\"\" to close a string");

				if (sTryRead_CP(iStrimU, '"'))
					{
					// We have another quote, so there were at least four in a row,
					// which we get with a quote in the text immediately followed
					// by the triple quote. So emit a quote.
					theStrimW.WriteCP('"');
					if (sTryRead_CP(iStrimU, '"'))
						{
						// Same again -- five quotes in a row, which is two content
						// quotes followed by the closing triple.
						theStrimW.WriteCP('"');
						// This is why it's essential that when using triple quotes
						// you put whitespace before the opening, and after the closing
						// triple, so we don't mistake included quotes for ones that
						// are (say) opening a subsequent regular quoted sequence.
						}
					}
				}
			else
				{
				// We have two quotes in a row, followed by something else, so
				// we had an empty string segment.
				}
			}
		else
			{
			sCopy_EscapedString(iStrimU, '"', theStrimW);
			if (!sTryRead_CP(iStrimU, '"'))
				throw ParseException("Expected \" to close a string");
			}

		sSkip_WSAndCPlusPlusComments(iStrimU);

		if (!sTryRead_CP(iStrimU, '"'))
			break;
		}

	oTValue.SetString(theString);
	}

static void sReadQuotedString_Apos(const ZStrimU& iStrimU, ZTValue& oTValue)
	{
	using namespace ZUtil_Strim;

	string theString;
	theString.reserve(100);
	ZStrimW_String theStrimW(theString);

	for (;;)
		{
		sCopy_EscapedString(iStrimU, '\'', theStrimW);
		if (!sTryRead_CP(iStrimU, '\''))
			throw ParseException("Expected ' to close a string");

		sSkip_WSAndCPlusPlusComments(iStrimU);

		if (!sTryRead_CP(iStrimU, '\''))
			break;
		}

	oTValue.SetString(theString);
	}

static void sFromStrim_BodyOfVector(const ZStrimU& iStrimU, vector<ZTValue>& oVector);
static void sFromStrim_BodyOfTuple(const ZStrimU& iStrimU, ZTuple& oTuple);

static bool sFromStrim_TValue(const ZStrimU& iStrimU, ZTValue& oTValue)
	{
	using namespace ZUtil_Strim;

	sSkip_WSAndCPlusPlusComments(iStrimU);

	if (sTryRead_CP(iStrimU, '['))
		{
		sFromStrim_BodyOfVector(iStrimU, oTValue.SetMutableVector());

		sSkip_WSAndCPlusPlusComments(iStrimU);

		if (!sTryRead_CP(iStrimU, ']'))
			throw ParseException("Expected ']' to close a vector");
		}
	else if (sTryRead_CP(iStrimU, '{'))
		{
		// It's a tuple.
		sFromStrim_BodyOfTuple(iStrimU, oTValue.SetMutableTuple());

		sSkip_WSAndCPlusPlusComments(iStrimU);

		if (!sTryRead_CP(iStrimU, '}'))
			throw ParseException("Expected '}' to close a tuple");
		}
	else if (sTryRead_CP(iStrimU, '"'))
		{
		// It's a string, delimited by ".
		sReadQuotedString_Quote(iStrimU, oTValue);
		}
	else if (sTryRead_CP(iStrimU, '\''))
		{
		// It's a string, delimited by '.
		sReadQuotedString_Apos(iStrimU, oTValue);
		}
#if 0//##
	else if (sTryRead_CP(iStrimU, '@'))
		{
		// It's a property name.
		string propertyName;
		if (!sTryRead_EscapedString(iStrimU, '"', propertyName))
			{
			if (!sTryRead_EscapedString(iStrimU, '\'', propertyName))
				{
				if (!ZUtil_Strim_Tuple::sRead_Identifier(iStrimU, nil, &propertyName))
					throw ParseException("Expected a property name after '#'");
				}
			}
		oTValue.SetName(propertyName);
		}
#endif
	else if (sTryRead_CP(iStrimU, '('))
		{
		// It's a raw.
		ZMemoryBlock theMemoryBlock;
		ZStreamR_HexStrim(iStrimU).CopyAllTo(ZStreamRWPos_MemoryBlock(theMemoryBlock));
		sSkip_WSAndCPlusPlusComments(iStrimU);

		if (!sTryRead_CP(iStrimU, ')'))
			throw ParseException("Expected ')' to close a raw");
		oTValue.SetRaw(theMemoryBlock);
		}
	else
		{
		string theTypeLC, theType;
		if (!ZUtil_Strim_Tuple::sRead_Identifier(iStrimU, &theTypeLC, &theType))
			{
			// We couldn't find any of the special characters, nor could
			// we read a type designator, so we fail to read a TValue,
			// which is not a parse error at this stage -- it might be for our caller.
			return false;
			}

		if (theTypeLC == "null")
			{
			oTValue.SetNull();
			}
		else if (theTypeLC == "false")
			{
			oTValue.SetBool(false);
			}
		else if (theTypeLC == "true")
			{
			oTValue.SetBool(true);
			}
		else
			{
			sSkip_WSAndCPlusPlusComments(iStrimU);
			if (!sTryRead_CP(iStrimU, '('))
				throw ParseException("Expected '(' following a type designator");

			sSkip_WSAndCPlusPlusComments(iStrimU);

			if (theTypeLC == "type")
				{
				string typeValueLC, typeValue;
				if (!ZUtil_Strim_Tuple::sRead_Identifier(iStrimU, &typeValueLC, &typeValue))
					throw ParseException("Expected a type name");

				if (typeValueLC == "null") oTValue.SetType(eZType_Null);
				else if (typeValueLC == "string") oTValue.SetType(eZType_String);
// AG 2007-08-06. cstring has never been used. Not even sure what it's point was.
//				else if (typeValueLC == "cstring") oTValue.SetType(eZType_CString);
				else if (typeValueLC == "int8") oTValue.SetType(eZType_Int8);
				else if (typeValueLC == "int16") oTValue.SetType(eZType_Int16);
				else if (typeValueLC == "int32") oTValue.SetType(eZType_Int32);
				else if (typeValueLC == "int64") oTValue.SetType(eZType_Int64);
				else if (typeValueLC == "float") oTValue.SetType(eZType_Float);
				else if (typeValueLC == "double") oTValue.SetType(eZType_Double);
				else if (typeValueLC == "time") oTValue.SetType(eZType_Time);
				else if (typeValueLC == "bool") oTValue.SetType(eZType_Bool);
				else if (typeValueLC == "pointer") oTValue.SetType(eZType_Pointer);
				else if (typeValueLC == "raw") oTValue.SetType(eZType_Raw);
				else if (typeValueLC == "tuple") oTValue.SetType(eZType_Tuple);
				else if (typeValueLC == "refcounted") oTValue.SetType(eZType_RefCounted);
				else if (typeValueLC == "rect") oTValue.SetType(eZType_Rect);
				else if (typeValueLC == "point") oTValue.SetType(eZType_Point);
				else if (typeValueLC == "region") oTValue.SetType(eZType_Region);
				else if (typeValueLC == "id") oTValue.SetType(eZType_ID);
				else if (typeValueLC == "vector") oTValue.SetType(eZType_Vector);
				else if (typeValueLC == "type") oTValue.SetType(eZType_Type);
				else if (typeValueLC == "time") oTValue.SetType(eZType_Time);
//##				else if (typeValueLC == "name") oTValue.SetType(eZType_Name);
				else
					throw ParseException("Unknown type name '" + typeValue + "'");
				}
			else if (theTypeLC == "id")
				{
				oTValue.SetID(sMustRead_GenericInteger(iStrimU));
				}
			else if (theTypeLC == "int8")
				{
				oTValue.SetInt8(sMustRead_GenericInteger(iStrimU));
				}
			else if (theTypeLC == "int16")
				{
				oTValue.SetInt16(sMustRead_GenericInteger(iStrimU));
				}
			else if (theTypeLC == "int32")
				{
				oTValue.SetInt32(sMustRead_GenericInteger(iStrimU));
				}
			else if (theTypeLC == "int64")
				{
				oTValue.SetInt64(sMustRead_GenericInteger(iStrimU));
				}
			else if (theTypeLC == "bool")
				{
				string theBool;
				if (!ZUtil_Strim_Tuple::sRead_Identifier(iStrimU, &theBool, nil))
					throw ParseException("Expected 'false' or 'true'");

				if (theBool == "true")
					oTValue.SetBool(true);
				else if (theBool == "false")
					oTValue.SetBool(false);
				else
					throw ParseException("Expected 'false' or 'true'");
				}
			else if (theTypeLC == "float")
				{
				double theDouble;
				if (!sTryRead_SignedDouble(iStrimU, theDouble))
					throw ParseException("Expected a floating point number");
				oTValue.SetFloat(theDouble);
				}
			else if (theTypeLC == "double")
				{
				double theDouble;
				if (!sTryRead_SignedDouble(iStrimU, theDouble))
					throw ParseException("Expected a floating point number");
				oTValue.SetDouble(theDouble);
				}
			else if (theTypeLC == "time")
				{
				if (sTryRead_CP(iStrimU, ')'))
					{
					// It's a time with no content, hence an invalid time.
					oTValue.SetTime(ZTime());
					
					// We'll take an early exit so the normal code that
					// looks for a closing parenthesis doesn't choke.
					return true;
					}
				// Try to read a double, which is how we're representing
				// times in text streams for now.
				double theDouble;
				if (!sTryRead_SignedDouble(iStrimU, theDouble))
					throw ParseException("Expected a floating point time");
				oTValue.SetTime(theDouble);
				}
			else if (theTypeLC == "rect")
				{
				ZRectPOD theRect;

				theRect.left = sMustRead_GenericInteger(iStrimU);

				sMustRead_WSCommaWS(iStrimU);

				theRect.top = sMustRead_GenericInteger(iStrimU);

				sMustRead_WSCommaWS(iStrimU);

				theRect.right = sMustRead_GenericInteger(iStrimU);

				sMustRead_WSCommaWS(iStrimU);

				theRect.bottom = sMustRead_GenericInteger(iStrimU);

				oTValue.SetRect(theRect);
				}
			else if (theTypeLC == "point")
				{
				ZPointPOD thePoint;

				thePoint.h = sMustRead_GenericInteger(iStrimU);

				sMustRead_WSCommaWS(iStrimU);

				thePoint.v = sMustRead_GenericInteger(iStrimU);

				oTValue.SetPoint(thePoint);
				}
			else
				{
				throw ParseException("Unknown type designator '" + theType + "'");
				}

			sSkip_WSAndCPlusPlusComments(iStrimU);

			if (!sTryRead_CP(iStrimU, ')'))
				throw ParseException("Expected ')' to close a value");
			}
		}
	return true;
	}

static void sFromStrim_BodyOfVector(const ZStrimU& iStrimU, vector<ZTValue>& oVector)
	{
	using namespace ZUtil_Strim;
	for (;;)
		{
		sSkip_WSAndCPlusPlusComments(iStrimU);

		oVector.push_back(ZTValue());
		if (!sFromStrim_TValue(iStrimU, oVector.back()))
			{
			// Lose the unread TValue.
			oVector.pop_back();
			break;
			}

		sSkip_WSAndCPlusPlusComments(iStrimU);

		if (!sTryRead_CP(iStrimU, ';') && !sTryRead_CP(iStrimU, ','))
			break;
		}
	}

static void sFromStrim_BodyOfTuple(const ZStrimU& iStrimU, ZTuple& oTuple)
	{
	using namespace ZUtil_Strim;
	for (;;)
		{
		sSkip_WSAndCPlusPlusComments(iStrimU);

		string propertyName;
		if (!sTryRead_EscapedString(iStrimU, '"', propertyName))
			{
			if (!sTryRead_EscapedString(iStrimU, '\'', propertyName))
				{
				if (!ZUtil_Strim_Tuple::sRead_Identifier(iStrimU, nil, &propertyName))
					break;
				}
			}

		sSkip_WSAndCPlusPlusComments(iStrimU);

		if (!sTryRead_CP(iStrimU, '='))
			throw ParseException("Expected '=' after property name");

		sSkip_WSAndCPlusPlusComments(iStrimU);

		ZTValue& theTValue = oTuple.SetMutableNull(ZTName(propertyName));
		if (!sFromStrim_TValue(iStrimU, theTValue))
			throw ParseException("Expected a property value after '='");

		sSkip_WSAndCPlusPlusComments(iStrimU);

		if (!sTryRead_CP(iStrimU, ';') && !sTryRead_CP(iStrimU, ','))
			throw ParseException("Expected a ';' or a ',' after property value");
		}
	}

// =================================================================================================
#pragma mark -
#pragma mark * Format

ZUtil_Strim_Tuple::Format::Format(const ZTValue& iTValue, int iInitialIndent, const ZUtil_Strim_Tuple::Options& iOptions)
:	fTValue(iTValue),
	fInitialIndent(iInitialIndent),
	fOptions(iOptions)
	{}

// =================================================================================================
#pragma mark -
#pragma mark * ZUtil_Strim_Tuple, writing and parsing pieces

void ZUtil_Strim_Tuple::sWrite_PropName(const ZStrimW& iStrimW, const ZTName& iPropName)
	{ ZYADUtil_ZooLib::sWrite_PropName(iStrimW, iPropName.AsString()); }

bool ZUtil_Strim_Tuple::sRead_Identifier(const ZStrimU& iStrimU, string* oStringLC, string* oStringExact)
	{
	bool gotAny = false;
	for (;;)
		{
		UTF32 theCP;
		if (!iStrimU.ReadCP(theCP))
			break;
		if (!ZUnicode::sIsAlphaDigit(theCP) && theCP != '_')
			{
			iStrimU.Unread();
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

// =================================================================================================
#pragma mark -
#pragma mark * ZTValue

void ZUtil_Strim_Tuple::sToStrim(const ZStrimW& s, const ZTValue& iTV)
	{ ZYADUtil_ZooLib::sToStrim(s, new ZYADReaderRep_ZTValue(iTV), 0, Options()); }

void ZUtil_Strim_Tuple::sToStrim(const ZStrimW& s, const ZTValue& iTV,
	size_t iInitialIndent, const Options& iOptions)
	{ ZYADUtil_ZooLib::sToStrim(s, new ZYADReaderRep_ZTValue(iTV), 0, Options()); }

string ZUtil_Strim_Tuple::sAsString(const ZTValue& iTV)
	{
	string theString;
	sToStrim(ZStrimW_String(theString), iTV);
	return theString;
	}

bool ZUtil_Strim_Tuple::sFromStrim(const ZStrimU& iStrimU, ZTValue& oTValue)
	{ return sFromStrim_TValue(iStrimU, oTValue); }

bool ZUtil_Strim_Tuple::sFromString(const string& iString, ZTValue& oTValue)
	{ return sFromStrim_TValue(ZStrimU_String(iString), oTValue); }

// =================================================================================================
#pragma mark -
#pragma mark * vector<ZTValue>

void ZUtil_Strim_Tuple::sToStrim(const ZStrimW& s, const vector<ZTValue>& iVector)
	{ ZYADUtil_ZooLib::sToStrim(s, new ZListReaderRep_ZVector(iVector), 0, Options()); }

void ZUtil_Strim_Tuple::sToStrim(const ZStrimW& s, const vector<ZTValue>& iVector,
	size_t iInitialIndent, const Options& iOptions)
	{ ZYADUtil_ZooLib::sToStrim(s, new ZListReaderRep_ZVector(iVector), 0, Options()); }

string ZUtil_Strim_Tuple::sAsString(const vector<ZTValue>& iVector)
	{
	string theString;
	sToStrim(ZStrimW_String(theString), iVector);
	return theString;
	}

bool ZUtil_Strim_Tuple::sFromStrim(const ZStrimU& iStrimU, vector<ZTValue>& oVector)
	{
	using namespace ZUtil_Strim;
	sSkip_WSAndCPlusPlusComments(iStrimU);

	UTF32 theCP;
	if (!iStrimU.ReadCP(theCP))
		return false;

	if (theCP != '[')
		throw ParseException("Expected '[' to open vector");

	sFromStrim_BodyOfVector(iStrimU, oVector);

	sSkip_WSAndCPlusPlusComments(iStrimU);

	if (!sTryRead_CP(iStrimU, ']'))
		throw ParseException("Expected ']' to close vector");

	return true;
	}

bool ZUtil_Strim_Tuple::sFromString(const string& iString, vector<ZTValue>& oVector)
	{ return sFromStrim(ZStrimU_String(iString), oVector); }

// =================================================================================================
#pragma mark -
#pragma mark * ZTuple

void ZUtil_Strim_Tuple::sToStrim(const ZStrimW& s, const ZTuple& iTuple)
	{ ZYADUtil_ZooLib::sToStrim(s, new ZMapReaderRep_ZTuple(iTuple), 0, Options()); }

void ZUtil_Strim_Tuple::sToStrim(const ZStrimW& s, const ZTuple& iTuple,
	size_t iInitialIndent, const Options& iOptions)
	{ ZYADUtil_ZooLib::sToStrim(s, new ZMapReaderRep_ZTuple(iTuple), 0, Options()); }

string ZUtil_Strim_Tuple::sAsString(const ZTuple& iTuple)
	{
	string theString;
	sToStrim(ZStrimW_String(theString), iTuple);
	return theString;
	}

string ZUtil_Strim_Tuple::sAsString(const ZTuple& iTuple,
	size_t iInitialIndent, const Options& iOptions)
	{
	string theString;
	sToStrim(ZStrimW_String(theString), iTuple, iInitialIndent, iOptions);
	return theString;
	}

bool ZUtil_Strim_Tuple::sFromStrim(const ZStrimU& iStrimU, ZTuple& oTuple)
	{
	using namespace ZUtil_Strim;
	sSkip_WSAndCPlusPlusComments(iStrimU);

	UTF32 theCP;
	if (!iStrimU.ReadCP(theCP))
		return false;

	if (theCP != '{')
		throw ParseException("Expected '{' to open tuple");

	sFromStrim_BodyOfTuple(iStrimU, oTuple);

	sSkip_WSAndCPlusPlusComments(iStrimU);

	if (!sTryRead_CP(iStrimU, '}'))
		throw ParseException("Expected '}' to close tuple");

	return true;
	}

bool ZUtil_Strim_Tuple::sFromString(const string& iString, ZTuple& oTuple)
	{ return sFromStrim(ZStrimU_String(iString), oTuple); }

// =================================================================================================
#pragma mark -
#pragma mark * operator<< overloads

const ZStrimW& operator<<(const ZStrimW& s, const ZTValue& iTValue)
	{
	ZUtil_Strim_Tuple::sToStrim(s, iTValue);
	return s;
	}

const ZStrimW& operator<<(const ZStrimW& s, const ZUtil_Strim_Tuple::Format& iFormat)
	{
	ZUtil_Strim_Tuple::sToStrim(s, iFormat.fTValue, iFormat.fInitialIndent, iFormat.fOptions);
	return s;
	}
