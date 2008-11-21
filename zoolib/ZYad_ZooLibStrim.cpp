/* -------------------------------------------------------------------------------------------------
Copyright (c) 2008 Andrew Green
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

#include "zoolib/ZMemoryBlock.h"
#include "zoolib/ZStrimR_Boundary.h"
#include "zoolib/ZStrimW_Escapify.h"
#include "zoolib/ZUtil_Strim.h"
#include "zoolib/ZUtil_Time.h"
#include "zoolib/ZYad_ZooLib.h"
#include "zoolib/ZYad_ZooLibStrim.h"

using std::string;
using std::vector;

// =================================================================================================
#pragma mark -
#pragma mark * Static parsing functions

static void sThrowParseException(const string& iMessage)
	{
	throw ZYadParseException_ZooLibStrim(iMessage);
	}

static bool sTryRead_PropertyName(const ZStrimU& iStrimU, string& oName)
	{
	using namespace ZUtil_Strim;

	if (!sTryRead_EscapedString(iStrimU, '"', oName))
		{
		if (!sTryRead_EscapedString(iStrimU, '\'', oName))
			{
			if (!ZYadUtil_ZooLibStrim::sRead_Identifier(iStrimU, nil, &oName))
				return false;
			}
		}

	return true;
	}

static int64 sMustRead_GenericInteger(const ZStrimU& iStrimU)
	{
	using namespace ZUtil_Strim;
	int64 theInteger;
	if (!sTryRead_GenericInteger(iStrimU, theInteger))
		sThrowParseException("Expected an integer");
	return theInteger;
	}

static void sMustRead_WSCommaWS(const ZStrimU& iStrimU)
	{
	using namespace ZUtil_Strim;
	sSkip_WSAndCPlusPlusComments(iStrimU);

	if (!sTryRead_CP(iStrimU, ','))
		sThrowParseException("Expected a ','");

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
					sThrowParseException("Expected \"\"\" to close a string");

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
				sThrowParseException("Expected \" to close a string");
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
			sThrowParseException("Expected ' to close a string");

		sSkip_WSAndCPlusPlusComments(iStrimU);

		if (!sTryRead_CP(iStrimU, '\''))
			break;
		}

	oTValue.SetString(theString);
	}

static bool sFromStrim_TValue(const ZStrimU& iStrimU, ZTValue& oTValue);

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
		if (!sTryRead_PropertyName(iStrimU, propertyName))
			break;

		sSkip_WSAndCPlusPlusComments(iStrimU);

		if (!sTryRead_CP(iStrimU, '='))
			sThrowParseException("Expected '=' after property name");

		sSkip_WSAndCPlusPlusComments(iStrimU);

		ZTValue& theTValue = oTuple.SetMutableNull(ZTName(propertyName));
		if (!sFromStrim_TValue(iStrimU, theTValue))
			sThrowParseException("Expected a property value after '='");

		sSkip_WSAndCPlusPlusComments(iStrimU);

		if (!sTryRead_CP(iStrimU, ';') && !sTryRead_CP(iStrimU, ','))
			sThrowParseException("Expected a ';' or a ',' after property value");
		}
	}

static bool sFromStrim_TValue(const ZStrimU& iStrimU, ZTValue& oTValue)
	{
	using namespace ZUtil_Strim;

	sSkip_WSAndCPlusPlusComments(iStrimU);

	if (sTryRead_CP(iStrimU, '['))
		{
		sFromStrim_BodyOfVector(iStrimU, oTValue.SetMutableVector());

		sSkip_WSAndCPlusPlusComments(iStrimU);

		if (!sTryRead_CP(iStrimU, ']'))
			sThrowParseException("Expected ']' to close a vector");
		}
	else if (sTryRead_CP(iStrimU, '{'))
		{
		// It's a tuple.
		sFromStrim_BodyOfTuple(iStrimU, oTValue.SetMutableTuple());

		sSkip_WSAndCPlusPlusComments(iStrimU);

		if (!sTryRead_CP(iStrimU, '}'))
			sThrowParseException("Expected '}' to close a tuple");
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
				if (!ZYadUtil_ZooLibStrim::sRead_Identifier(iStrimU, nil, &propertyName))
					sThrowParseException("Expected a property name after '#'");
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
			sThrowParseException("Expected ')' to close a raw");
		oTValue.SetRaw(theMemoryBlock);
		}
	else
		{
		string theTypeLC, theType;
		if (!ZYadUtil_ZooLibStrim::sRead_Identifier(iStrimU, &theTypeLC, &theType))
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
				sThrowParseException("Expected '(' following a type designator");

			sSkip_WSAndCPlusPlusComments(iStrimU);

			if (theTypeLC == "type")
				{
				string typeValueLC, typeValue;
				if (!ZYadUtil_ZooLibStrim::sRead_Identifier(iStrimU, &typeValueLC, &typeValue))
					sThrowParseException("Expected a type name");

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
					sThrowParseException("Unknown type name '" + typeValue + "'");
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
				if (!ZYadUtil_ZooLibStrim::sRead_Identifier(iStrimU, &theBool, nil))
					sThrowParseException("Expected 'false' or 'true'");

				if (theBool == "true")
					oTValue.SetBool(true);
				else if (theBool == "false")
					oTValue.SetBool(false);
				else
					sThrowParseException("Expected 'false' or 'true'");
				}
			else if (theTypeLC == "float")
				{
				double theDouble;
				if (!sTryRead_SignedDouble(iStrimU, theDouble))
					sThrowParseException("Expected a floating point number");
				oTValue.SetFloat(theDouble);
				}
			else if (theTypeLC == "double")
				{
				double theDouble;
				if (!sTryRead_SignedDouble(iStrimU, theDouble))
					sThrowParseException("Expected a floating point number");
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
					sThrowParseException("Expected a floating point time");
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
				sThrowParseException("Unknown type designator '" + theType + "'");
				}

			sSkip_WSAndCPlusPlusComments(iStrimU);

			if (!sTryRead_CP(iStrimU, ')'))
				sThrowParseException("Expected ')' to close a value");
			}
		}
	return true;
	}

static ZRef<ZYadR_ZooLibStrim> sMakeYadR_ZooLibStrim(const ZStrimU& iStrimU)
	{
	using namespace ZUtil_Strim;

	sSkip_WSAndCPlusPlusComments(iStrimU);
	
	if (sTryRead_CP(iStrimU, '['))
		{
		return new ZYadListR_ZooLibStrim(iStrimU, true);
		}
	else if (sTryRead_CP(iStrimU, '{'))
		{
		return new ZYadMapR_ZooLibStrim(iStrimU, true);
		}
	else if (sTryRead_CP(iStrimU, '('))
		{
		return new ZYadRawR_ZooLibStrim(iStrimU, true);
		}
	else
		{
		ZTValue theTV;
		if (sFromStrim_TValue(iStrimU, theTV))
			return new ZYadPrimR_ZooLibStrim(theTV);
		}

	return ZRef<ZYadR_ZooLibStrim>();
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZYadParseException_ZooLibStrim

ZYadParseException_ZooLibStrim::ZYadParseException_ZooLibStrim(const string& iWhat)
:	ZYadParseException(iWhat)
	{}

ZYadParseException_ZooLibStrim::ZYadParseException_ZooLibStrim(const char* iWhat)
:	ZYadParseException(iWhat)
	{}

// =================================================================================================
#pragma mark -
#pragma mark * ZYadPrimR_ZooLibStrim

ZYadPrimR_ZooLibStrim::ZYadPrimR_ZooLibStrim(const ZTValue& iTV)
:	ZYadR_TValue(iTV)
	{}

void ZYadPrimR_ZooLibStrim::Finish()
	{}

// =================================================================================================
#pragma mark -
#pragma mark * ZYadRawR_ZooLibStrim

ZYadRawR_ZooLibStrim::ZYadRawR_ZooLibStrim(const ZStrimU& iStrimU, bool iReadDelimiter)
:	fStrimU(iStrimU),
	fReadDelimiter(iReadDelimiter),
	fStreamR(iStrimU)
	{}

void ZYadRawR_ZooLibStrim::Finish()
	{
	using namespace ZUtil_Strim;

	fStreamR.SkipAll();

	if (fReadDelimiter)
		{
		sSkip_WSAndCPlusPlusComments(fStrimU);
		if (!sTryRead_CP(fStrimU, ']'))
			sThrowParseException("Expected ')' to close a raw");
		}
	}

const ZStreamR& ZYadRawR_ZooLibStrim::GetStreamR()
	{ return fStreamR; }

// =================================================================================================
#pragma mark -
#pragma mark * ZYadListR_ZooLibStrim

ZYadListR_ZooLibStrim::ZYadListR_ZooLibStrim(const ZStrimU& iStrimU, bool iReadDelimiter)
:	fStrimU(iStrimU),
	fReadDelimiter(iReadDelimiter),
	fPosition(0)
	{}

bool ZYadListR_ZooLibStrim::HasChild()
	{
	this->pMoveIfNecessary();
	return fValue_Current;
	}

ZRef<ZYadR> ZYadListR_ZooLibStrim::NextChild()
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

size_t ZYadListR_ZooLibStrim::GetPosition()
	{ return fPosition; }

void ZYadListR_ZooLibStrim::Finish()
	{
	using namespace ZUtil_Strim;

	this->SkipAll();

	if (fReadDelimiter)
		{
		sSkip_WSAndCPlusPlusComments(fStrimU);
		if (!sTryRead_CP(fStrimU, ']'))
			sThrowParseException("Expected ']' to close a vector");
		}
	}

void ZYadListR_ZooLibStrim::pMoveIfNecessary()
	{
	using namespace ZUtil_Strim;

	if (fValue_Current)
		return;

	if (fValue_Prior)
		{
		fValue_Prior->Finish();
		fValue_Prior.Clear();
		sSkip_WSAndCPlusPlusComments(fStrimU);
		if (!sTryRead_CP(fStrimU, ',') && !sTryRead_CP(fStrimU, ';'))
			return;
		}

	fValue_Current = sMakeYadR_ZooLibStrim(fStrimU);
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZYadMapR_ZooLibStrim

ZYadMapR_ZooLibStrim::ZYadMapR_ZooLibStrim(const ZStrimU& iStrimU, bool iReadDelimiter)
:	fStrimU(iStrimU),
	fReadDelimiter(iReadDelimiter)
	{}
	
bool ZYadMapR_ZooLibStrim::HasChild()
	{
	this->pMoveIfNecessary();

	return fValue_Current;
	}

ZRef<ZYadR> ZYadMapR_ZooLibStrim::NextChild()
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

string ZYadMapR_ZooLibStrim::Name()
	{
	this->pMoveIfNecessary();

	return fName;
	}

void ZYadMapR_ZooLibStrim::Finish()
	{
	using namespace ZUtil_Strim;

	this->SkipAll();

	if (fReadDelimiter)
		{
		sSkip_WSAndCPlusPlusComments(fStrimU);
		if (!sTryRead_CP(fStrimU, '}'))
			sThrowParseException("Expected '}' to close a tuple");
		}
	}

void ZYadMapR_ZooLibStrim::pMoveIfNecessary()
	{
	using namespace ZUtil_Strim;

	if (fValue_Current)
		return;

	if (fValue_Prior)
		{
		fValue_Prior->Finish();
		fValue_Prior.Clear();

		sSkip_WSAndCPlusPlusComments(fStrimU);

		if (!sTryRead_CP(fStrimU, ',') && !sTryRead_CP(fStrimU, ';'))
			sThrowParseException("Expected ';' or ',' after property");
		}

	sSkip_WSAndCPlusPlusComments(fStrimU);

	if (!sTryRead_PropertyName(fStrimU, fName))
		return;

	sSkip_WSAndCPlusPlusComments(fStrimU);

	if (!sTryRead_CP(fStrimU, '='))
		sThrowParseException("Expected '=' after property name");

	fValue_Current = sMakeYadR_ZooLibStrim(fStrimU);
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
	if (iOptions.fBreakStrings
		&& iOptions.DoIndentation()
		&& string::npos != iString.find_first_of("\n\r"))
		{
		// We put a newline after the opening """, which will be
		// ignored by sFromStrim, so the first line of iString
		// will be in column zero.
		s << "\"\"\"\n";
		ZStrimU_String strim_String(iString);
		ZStrimR_Boundary strim_Boundary("\"\"\"", strim_String);
		for (;;)
			{
			s.CopyAllFrom(strim_Boundary);
			if (!strim_Boundary.HitBoundary())
				{
				// We've returned without having hit the boundary, so we're done.
				break;
				}
			strim_Boundary.Reset();

			// Close the triple quotes.
			s << "\"\"\"";
			// A space to separate the triple-quote from the single quote
			s << " ";
			// An open quote
			s << "\"";
			// Three escaped quotes.
			s << "\\\"\\\"\\\"";
			// A close quote.
			s << "\"";
			// Another space, for symmetry
			s << " ";
			// And re-open triple quotes again.
			s << "\"\"\"";
			// With a newline, so the text will again
			// start in column zero.
			s << "\n";
			}
		s << "\"\"\"";
		}
	else
		{
		string delimiter = "\"";
		bool quoteQuotes = true;
		if (string::npos != iString.find('"') && string::npos == iString.find('\''))
			{
			delimiter = "'";
			quoteQuotes = false;
			}

		s.Write(delimiter);

		ZStrimW_Escapify::Options theOptions;
		theOptions.fQuoteQuotes = quoteQuotes;
		theOptions.fEscapeHighUnicode = false;
		
		ZStrimW_Escapify(theOptions, s).Write(iString);

		s.Write(delimiter);
		}
	}

static void sToStrim_SimpleTValue(const ZStrimW& s, const ZTValue& iTV,
	size_t iLevel, const ZYadOptions& iOptions)
	{
	switch (iTV.TypeOf())
		{
		case eZType_Null: s.Write("Null"); break;
		case eZType_Type:
			{
			s.Write("Type(");
			s.Write(ZTypeAsString(iTV.GetType()));
			s.Write(")");
			break;
			}
		case eZType_ID:
			{
			s.Writef("ID(0x%0llX)", iTV.GetID());
			if (iOptions.fIDsHaveDecimalVersionComment)
				s.Writef(" /*%lld*/", iTV.GetID());
			break;
			}
		case eZType_Int8: s.Writef("int8(%d)", iTV.GetInt8()); break;
		case eZType_Int16: s.Writef("int16(%d)", iTV.GetInt16()); break;
		case eZType_Int32: s.Writef("int32(%d)", iTV.GetInt32()); break;
		case eZType_Int64: s.Writef("int64(0x%0llX)", iTV.GetInt64()); break;
		case eZType_Bool:
			{
			if (iTV.GetBool())
				s.Write("true");
			else
				s.Write("false");
			break;
			}
		case eZType_Float:
			{
			// 9 decimal digits are necessary and sufficient for single precision IEEE 754.
			// "What Every Computer Scientist Should Know About Floating Point", Goldberg, 1991.
			// <http://docs.sun.com/source/806-3568/ncg_goldberg.html>
			s.Writef("float(%.9g)", iTV.GetFloat());
			break;
			}
		case eZType_Double:
			{
			// 17 decimal digits are necessary and sufficient for double precision IEEE 754.
			s.Writef("double(%.17g)", iTV.GetDouble());
			break;
			}
		case eZType_Time:
			{
			// For the moment I'm just writing times as a count of seconds, putting
			// the broken-out Gregorian version in a comment. Later we can improve
			// the parsing of dates, and then we can write them in human readable form.
			if (ZTime theTime = iTV.GetTime())
				{
				s.Writef("time(%.17g)", theTime.fVal);
				if (iOptions.fTimesHaveUserLegibleComment)
					s << " /*" << ZUtil_Time::sAsString_ISO8601(theTime, true) << "*/";
				}
			else
				{
				// We're now allowing empty parens to represent invalid times.
				s.Write("time()");
				}
			break;
			}
		case eZType_Pointer: s.Writef("pointer(%08X)", iTV.GetPointer()); break;
		case eZType_Rect:
			{
			const ZRectPOD& theRect = iTV.GetRect();
			s.Writef("Rect(%d, %d, %d, %d)",
				theRect.left,
				theRect.top,
				theRect.right,
				theRect.bottom);
			break;
			}
		case eZType_Point:
			{
			const ZPointPOD& thePoint = iTV.GetPoint();
			s.Writef("Point(%d, %d)",
				thePoint.h,
				thePoint.v);
			break;
			}
#if 0//##
		case eZType_Name:
			{
			s.Write("@");
			ZYadUtil_ZooLibStrim::sWrite_PropName(s, iTV.GetName());
			break;
			}
#endif
		case eZType_String:
			{
			sWriteString(s, iOptions, iTV.GetString());
			break;
			}
		case eZType_RefCounted:
			s.Writef("RefCounted(%08X)", iTV.GetRefCounted().GetObject());
			break;
		case eZType_Raw:
		case eZType_Tuple:
		case eZType_Vector:
			{
			ZDebugStopf(0,
				("sToStrim_SimpleTValue should only be called on simple tuple values"));
			break;
			}
		default:
			{
			ZDebugStopf(0, ("Unrecognized type %d", iTV.TypeOf()));
			break;
			}
		}
	}

static void sToStrim_Raw(const ZStrimW& s, const ZStreamRPos& iStreamRPos,
	size_t iLevel, const ZYadOptions& iOptions, bool iMayNeedInitialLF)
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
				sWriteLFIndent(s, iLevel, iOptions);
			
			s.Writef("( // %lld bytes", theSize);
			sWriteLFIndent(s, iLevel, iOptions);
			if (iOptions.fRawAsASCII)
				{
				for (;;)
					{
					uint64 lastPos = iStreamRPos.GetPosition();
					uint64 countCopied;
					ZStreamW_HexStrim(iOptions.fRawByteSeparator, "", 0, s)
						.CopyFrom(iStreamRPos, iOptions.fRawChunkSize, &countCopied, nil);

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
					sWriteLFIndent(s, iLevel, iOptions);
					}
				}
			else
				{
				string eol = iOptions.fEOLString;
				for (size_t x = 0; x < iLevel; ++x)
					eol += iOptions.fIndentString;

				ZStreamW_HexStrim(iOptions.fRawByteSeparator,
					eol, iOptions.fRawChunkSize, s).CopyAllFrom(iStreamRPos);

				sWriteLFIndent(s, iLevel, iOptions);
				}

			s.Write(")");
			}
		else
			{
			s.Write("(");

			ZStreamW_HexStrim(iOptions.fRawByteSeparator, "", 0, s)
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

static void sToStrim_Raw(const ZStrimW& s, const ZStreamR& iStreamR,
	size_t iLevel, const ZYadOptions& iOptions, bool iMayNeedInitialLF)
	{
	if (const ZStreamRPos* theStreamRPos = dynamic_cast<const ZStreamRPos*>(&iStreamR))
		{
		sToStrim_Raw(s, *theStreamRPos, iLevel, iOptions, iMayNeedInitialLF);
		return;
		}

	s.Write("(");

	ZStreamW_HexStrim(iOptions.fRawByteSeparator, "", 0, s)
		.CopyAllFrom(iStreamR);

	s.Write(")");
	}

static void sToStrim_Yad(const ZStrimW& s, ZRef<ZYadR> iYadR,
	size_t iInitialIndent, const ZYadOptions& iOptions, bool iMayNeedInitialLF);

static void sToStrim_List(const ZStrimW& s, ZRef<ZYadListR> iYadListR,
	size_t iLevel, const ZYadOptions& iOptions, bool iMayNeedInitialLF)
	{
	if (!iYadListR->HasChild())
		{
		// We've got an empty vector.
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
			if (!iYadListR)
				break;
			s.Write(", ");
			}
		s.Write("]");
		}
	}

static void sToStrim_Map(const ZStrimW& s, ZRef<ZYadMapR> iYadMapR,
	size_t iLevel, const ZYadOptions& iOptions, bool iMayNeedInitialLF)
	{
	if (!iYadMapR->HasChild())
		{
		// We've got an empty tuple.
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
		while (iYadMapR->HasChild())
			{
			sWriteLFIndent(s, iLevel, iOptions);

			ZYadUtil_ZooLibStrim::sWrite_PropName(s, iYadMapR->Name());

			s << " = ";

			sToStrim_Yad(s, iYadMapR->NextChild(), iLevel + 1, iOptions, true);
			s.Write(";");
			}

		sWriteLFIndent(s, iLevel, iOptions);

		s.Write("}");
		}
	else
		{
		s.Write("{");
		while (iYadMapR->HasChild())
			{
			s.Write(" ");

			ZYadUtil_ZooLibStrim::sWrite_PropName(s, iYadMapR->Name());

			s << " = ";

			sToStrim_Yad(s, iYadMapR->NextChild(), iLevel + 1, iOptions, true);
			s.Write(";");
			}

		s.Write(" }");
		}
	}

static void sToStrim_Yad(const ZStrimW& s, ZRef<ZYadR> iYadR,
	size_t iLevel, const ZYadOptions& iOptions, bool iMayNeedInitialLF)
	{
	if (!iYadR)
		return;

	if (ZRef<ZYadMapR> theYadMapR = ZRefDynamicCast<ZYadMapR>(iYadR))
		{
		sToStrim_Map(s, theYadMapR, iLevel, iOptions, iMayNeedInitialLF);
		}
	else if (ZRef<ZYadListR> theYadListR = ZRefDynamicCast<ZYadListR>(iYadR))
		{
		sToStrim_List(s, theYadListR, iLevel, iOptions, iMayNeedInitialLF);
		}
	else if (ZRef<ZYadRawR> theYadRawR = ZRefDynamicCast<ZYadRawR>(iYadR))
		{
		sToStrim_Raw(s, theYadRawR->GetStreamR(), iLevel, iOptions, iMayNeedInitialLF);
		}
	else
		{
		sToStrim_SimpleTValue(s, ZYadUtil_ZooLib::sFromYadR(iYadR), iLevel, iOptions);
		}
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZYadUtil_ZooLibStrim

bool ZYadUtil_ZooLibStrim::sRead_Identifier(
	const ZStrimU& iStrimU, string* oStringLC, string* oStringExact)
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

ZRef<ZYadR> ZYadUtil_ZooLibStrim::sMakeYadR(const ZStrimU& iStrimU)
	{ return sMakeYadR_ZooLibStrim(iStrimU); }

void ZYadUtil_ZooLibStrim::sToStrim(const ZStrimW& s, ZRef<ZYadListR> iYadListR)
	{ sToStrim_List(s, iYadListR, 0, ZYadOptions(), false); }

void ZYadUtil_ZooLibStrim::sToStrim(const ZStrimW& s, ZRef<ZYadListR> iYadListR,
	size_t iInitialIndent, const ZYadOptions& iOptions)
	{ sToStrim_List(s, iYadListR, iInitialIndent, iOptions, false); }

void ZYadUtil_ZooLibStrim::sToStrim(const ZStrimW& s, ZRef<ZYadMapR> iYadMapR)
	{ sToStrim_Map(s, iYadMapR, 0, ZYadOptions(), false); }

void ZYadUtil_ZooLibStrim::sToStrim(const ZStrimW& s, ZRef<ZYadMapR> iYadMapR,
	size_t iInitialIndent, const ZYadOptions& iOptions)
	{ sToStrim_Map(s, iYadMapR, iInitialIndent, iOptions, false); }

void ZYadUtil_ZooLibStrim::sToStrim(const ZStrimW& s, ZRef<ZYadRawR> iYadRawR)
	{ sToStrim_Raw(s, iYadRawR->GetStreamR(), 0, ZYadOptions(), false); }

void ZYadUtil_ZooLibStrim::sToStrim(const ZStrimW& s, ZRef<ZYadRawR> iYadRawR,
	size_t iInitialIndent, const ZYadOptions& iOptions)
	{ sToStrim_Raw(s, iYadRawR->GetStreamR(), iInitialIndent, iOptions, false); }

void ZYadUtil_ZooLibStrim::sToStrim(const ZStrimW& s, ZRef<ZYadR> iYadR)
	{ sToStrim_Yad(s, iYadR, 0, ZYadOptions(), false); }

void ZYadUtil_ZooLibStrim::sToStrim(const ZStrimW& s, ZRef<ZYadR> iYadR,
	size_t iInitialIndent, const ZYadOptions& iOptions)
	{ sToStrim_Yad(s, iYadR, iInitialIndent, iOptions, false); }

static bool sContainsProblemChars(const string& iString)
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
		if (!ZUnicode::sReadInc(i, end, theCP))
			break;
		if (!ZUnicode::sIsAlphaDigit(theCP) && '_' != theCP)
			return true;
		}

	return false;
	}

void ZYadUtil_ZooLibStrim::sWrite_PropName(const ZStrimW& iStrimW, const string& iPropName)
	{
	if (sContainsProblemChars(iPropName))
		{
		iStrimW << "\"";
		ZStrimW_Escapify(iStrimW) << iPropName;
		iStrimW << "\"";
		}
	else
		{
		iStrimW << iPropName;
		}
	}
