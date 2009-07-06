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

#include "zoolib/ZStreamW_HexStrim.h"
#include "zoolib/ZStrimR_Boundary.h"
#include "zoolib/ZStrim_Escaped.h"
#include "zoolib/ZUnicode.h"
#include "zoolib/ZUtil_Strim.h"
#include "zoolib/ZUtil_Time.h"
#include "zoolib/ZYad_StdMore.h"
#include "zoolib/ZYad_ZooLib.h"
#include "zoolib/ZYad_ZooLibStrim.h"

NAMESPACE_ZOOLIB_BEGIN

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
			if (!ZYad_ZooLibStrim::sRead_Identifier(iStrimU, nullptr, &oName))
				return false;
			}
		}

	return true;
	}

static int64 sMustRead_GenericInteger(const ZStrimU& iStrimU)
	{
	using namespace ZUtil_Strim;
	int64 theInteger;
	if (!sTryRead_SignedGenericInteger(iStrimU, theInteger))
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

static bool sFromStrim_Value(const ZStrimU& iStrimU, ZVal_ZooLib& oVal)
	{
	using namespace ZUtil_Strim;

	sSkip_WSAndCPlusPlusComments(iStrimU);

	if (false)
		{}
#if 0//##
	else if (sTryRead_CP(iStrimU, '@'))
		{
		// It's a property name.
		string propertyName;
		if (!sTryRead_EscapedString(iStrimU, '"', propertyName))
			{
			if (!sTryRead_EscapedString(iStrimU, '\'', propertyName))
				{
				if (!ZYad_ZooLibStrim::sRead_Identifier(iStrimU, nullptr, &propertyName))
					sThrowParseException("Expected a property name after '#'");
				}
			}
		oVal.SetName(propertyName);
		}
#endif
	else
		{
		string theTypeLC, theType;
		if (!ZYad_ZooLibStrim::sRead_Identifier(iStrimU, &theTypeLC, &theType))
			{
			// We couldn't find any of the special characters, nor could
			// we read a type designator, so we fail to read a value,
			// which is not a parse error at this stage -- it might be for our caller.
			return false;
			}

		if (theTypeLC == "null")
			{
			oVal = ZVal_ZooLib();
			}
		else if (theTypeLC == "false")
			{
			oVal.SetBool(false);
			}
		else if (theTypeLC == "true")
			{
			oVal.SetBool(true);
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
				if (!ZYad_ZooLibStrim::sRead_Identifier(iStrimU, &typeValueLC, &typeValue))
					sThrowParseException("Expected a type name");

				if (typeValueLC == "null") oVal.SetType(eZType_Null);
				else if (typeValueLC == "string") oVal.SetType(eZType_String);
// AG 2007-08-06. cstring has never been used. Not even sure what it's point was.
//				else if (typeValueLC == "cstring") oVal.SetType(eZType_CString);
				else if (typeValueLC == "int8") oVal.SetType(eZType_Int8);
				else if (typeValueLC == "int16") oVal.SetType(eZType_Int16);
				else if (typeValueLC == "int32") oVal.SetType(eZType_Int32);
				else if (typeValueLC == "int64") oVal.SetType(eZType_Int64);
				else if (typeValueLC == "float") oVal.SetType(eZType_Float);
				else if (typeValueLC == "double") oVal.SetType(eZType_Double);
				else if (typeValueLC == "time") oVal.SetType(eZType_Time);
				else if (typeValueLC == "bool") oVal.SetType(eZType_Bool);
				else if (typeValueLC == "pointer") oVal.SetType(eZType_Pointer);
				else if (typeValueLC == "raw") oVal.SetType(eZType_Raw);
				else if (typeValueLC == "tuple") oVal.SetType(eZType_Tuple);
				else if (typeValueLC == "refcounted") oVal.SetType(eZType_RefCounted);
				else if (typeValueLC == "rect") oVal.SetType(eZType_Rect);
				else if (typeValueLC == "point") oVal.SetType(eZType_Point);
				else if (typeValueLC == "region") oVal.SetType(eZType_Region);
				else if (typeValueLC == "id") oVal.SetType(eZType_ID);
				else if (typeValueLC == "vector") oVal.SetType(eZType_Vector);
				else if (typeValueLC == "type") oVal.SetType(eZType_Type);
				else if (typeValueLC == "time") oVal.SetType(eZType_Time);
//##				else if (typeValueLC == "name") oVal.SetType(eZType_Name);
				else
					sThrowParseException("Unknown type name '" + typeValue + "'");
				}
			else if (theTypeLC == "id")
				{
				oVal.SetID(sMustRead_GenericInteger(iStrimU));
				}
			else if (theTypeLC == "int8")
				{
				oVal.SetInt8(sMustRead_GenericInteger(iStrimU));
				}
			else if (theTypeLC == "int16")
				{
				oVal.SetInt16(sMustRead_GenericInteger(iStrimU));
				}
			else if (theTypeLC == "int32")
				{
				oVal.SetInt32(sMustRead_GenericInteger(iStrimU));
				}
			else if (theTypeLC == "int64")
				{
				oVal.SetInt64(sMustRead_GenericInteger(iStrimU));
				}
			else if (theTypeLC == "bool")
				{
				string theBool;
				if (!ZYad_ZooLibStrim::sRead_Identifier(iStrimU, &theBool, nullptr))
					sThrowParseException("Expected 'false' or 'true'");

				if (theBool == "true")
					oVal.SetBool(true);
				else if (theBool == "false")
					oVal.SetBool(false);
				else
					sThrowParseException("Expected 'false' or 'true'");
				}
			else if (theTypeLC == "float")
				{
				double theDouble;
				if (!sTryRead_SignedDouble(iStrimU, theDouble))
					sThrowParseException("Expected a floating point number");
				oVal.SetFloat(theDouble);
				}
			else if (theTypeLC == "double")
				{
				double theDouble;
				if (!sTryRead_SignedDouble(iStrimU, theDouble))
					sThrowParseException("Expected a floating point number");
				oVal.SetDouble(theDouble);
				}
			else if (theTypeLC == "time")
				{
				if (sTryRead_CP(iStrimU, ')'))
					{
					// It's a time with no content, hence an invalid time.
					oVal.SetTime(ZTime());
					
					// We'll take an early exit so the normal code that
					// looks for a closing parenthesis doesn't choke.
					return true;
					}
				// Try to read a double, which is how we're representing
				// times in text streams for now.
				double theDouble;
				if (!sTryRead_SignedDouble(iStrimU, theDouble))
					sThrowParseException("Expected a floating point time");
				oVal.SetTime(theDouble);
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

				oVal.SetRect(theRect);
				}
			else if (theTypeLC == "point")
				{
				ZPointPOD thePoint;

				thePoint.h = sMustRead_GenericInteger(iStrimU);

				sMustRead_WSCommaWS(iStrimU);

				thePoint.v = sMustRead_GenericInteger(iStrimU);

				oVal.SetPoint(thePoint);
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

static ZRef<ZYadR_Std> sMakeYadR_ZooLibStrim(const ZStrimU& iStrimU)
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
		return new ZYadStreamR_ZooLibStrim(iStrimU, true);
		}
	else if (sTryRead_CP(iStrimU, '"'))
		{
		return new ZYadStrimR_ZooLibStrim_Quote(iStrimU);
		}
	else if (sTryRead_CP(iStrimU, '\''))
		{
		return new ZYadStrimR_ZooLibStrim_Apos(iStrimU);
		}
	else
		{
		ZVal_ZooLib theVal;
		if (sFromStrim_Value(iStrimU, theVal))
			return new ZYadPrimR_Std(theVal);
		}

	return ZRef<ZYadR_Std>();
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZYadParseException_ZooLibStrim

ZYadParseException_ZooLibStrim::ZYadParseException_ZooLibStrim(const string& iWhat)
:	ZYadParseException_Std(iWhat)
	{}

ZYadParseException_ZooLibStrim::ZYadParseException_ZooLibStrim(const char* iWhat)
:	ZYadParseException_Std(iWhat)
	{}

// =================================================================================================
#pragma mark -
#pragma mark * ZYadStreamR_ZooLibStrim

ZYadStreamR_ZooLibStrim::ZYadStreamR_ZooLibStrim(const ZStrimU& iStrimU, bool iReadDelimiter)
:	fStrimU(iStrimU),
	fReadDelimiter(iReadDelimiter),
	fStreamR(iStrimU)
	{}

void ZYadStreamR_ZooLibStrim::Finish()
	{
	using namespace ZUtil_Strim;

	fStreamR.SkipAll();

	if (fReadDelimiter)
		{
		sSkip_WSAndCPlusPlusComments(fStrimU);
		if (!sTryRead_CP(fStrimU, ')'))
			sThrowParseException("Expected ')' to close a raw");
		}
	}

const ZStreamR& ZYadStreamR_ZooLibStrim::GetStreamR()
	{ return fStreamR; }

// =================================================================================================
#pragma mark -
#pragma mark * ZYadStrimR_ZooLibStrim_Apos

ZYadStrimR_ZooLibStrim_Apos::ZYadStrimR_ZooLibStrim_Apos(const ZStrimU& iStrimU)
:	fStrimU(iStrimU),
	fStrimR(iStrimU, '\'')
	{}

void ZYadStrimR_ZooLibStrim_Apos::Finish()
	{
	using namespace ZUtil_Strim;
	fStrimR.SkipAll();
	if (!sTryRead_CP(fStrimU, '\''))
		throw ParseException("Missing string delimiter");
	}

const ZStrimR& ZYadStrimR_ZooLibStrim_Apos::GetStrimR()
	{ return fStrimR; }

// =================================================================================================
#pragma mark -
#pragma mark * ZYadStrimR_ZooLibStrim_Quote

ZYadStrimR_ZooLibStrim_Quote::ZYadStrimR_ZooLibStrim_Quote(const ZStrimU& iStrimU)
:	fStrimU(iStrimU),
	fStrimR_Boundary("\"\"\"", iStrimU),
	fQuotesSeen(1)
	{}

void ZYadStrimR_ZooLibStrim_Quote::Finish()
	{
	this->SkipAll();
	}

const ZStrimR& ZYadStrimR_ZooLibStrim_Quote::GetStrimR()
	{ return *this; }

void ZYadStrimR_ZooLibStrim_Quote::Imp_ReadUTF32(UTF32* iDest, size_t iCount, size_t* oCount)
	{
	using namespace ZUtil_Strim;

	UTF32* localDest = iDest;
	UTF32* localDestEnd = iDest + iCount;
	bool exhausted = false;
	while (localDestEnd > localDest && !exhausted)
		{
		switch (fQuotesSeen)
			{
			case 0:
				{
				sSkip_WSAndCPlusPlusComments(fStrimU);

				if (sTryRead_CP(fStrimU, '"'))
					fQuotesSeen = 1;
				else
					exhausted = true;
				break;
				}
			case 1:
				{
				if (sTryRead_CP(fStrimU, '"'))
					{
					fQuotesSeen = 2;
					}
				else
					{
					size_t countRead;
					ZStrimR_Escaped(fStrimU, '"')
						.Read(localDest, localDestEnd - localDest, &countRead);
					localDest += countRead;

					if (sTryRead_CP(fStrimU, '"'))
						fQuotesSeen = 0;
					else if (countRead == 0)
						sThrowParseException("Expected \" to close a string");
					}
				break;
				}
			case 2:
				{
				if (sTryRead_CP(fStrimU, '"'))
					{
					fQuotesSeen = 3;
					UTF32 theCP = fStrimU.ReadCP();
					if (!ZUnicode::sIsEOL(theCP))
						fStrimU.Unread(theCP);
					}
				else
					{
					// We have two quotes in a row, followed by something
					// else, so we had an empty string segment.
					fQuotesSeen = 0;
					}
				break;
				}
			case 3:
				{
				// We've got three quotes in a row, and any trailing EOL
				// has been stripped.
				size_t countRead;
				fStrimR_Boundary.Read(localDest, localDestEnd - localDest, &countRead);
				localDest += countRead;
				if (countRead == 0)
					{
					if (!fStrimR_Boundary.HitBoundary())
						sThrowParseException("Expected \"\"\" to close a string");
					fStrimR_Boundary.Reset();
					fQuotesSeen = 0;
					}
				break;
				}
			}
		}

	if (oCount)
		*oCount = localDest - iDest;	
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZYadListR_ZooLibStrim

ZYadListR_ZooLibStrim::ZYadListR_ZooLibStrim(const ZStrimU& iStrimU, bool iReadDelimiter)
:	fStrimU(iStrimU),
	fReadDelimiter(iReadDelimiter)
	{}

void ZYadListR_ZooLibStrim::Imp_ReadInc(bool iIsFirst, ZRef<ZYadR_Std>& oYadR)
	{
	using namespace ZUtil_Strim;

	sSkip_WSAndCPlusPlusComments(fStrimU);

	bool gotSeparator = true;
	if (!iIsFirst)
		{
		if (!sTryRead_CP(fStrimU, ',') && !sTryRead_CP(fStrimU, ';'))
			gotSeparator = false;
		else
			sSkip_WSAndCPlusPlusComments(fStrimU);
		}

	if (fReadDelimiter)
		{
		if (sTryRead_CP(fStrimU, ']'))
			return;
		}

	if (!gotSeparator)
		sThrowParseException("Expected ';' or ',' between values");

	if (!(oYadR = sMakeYadR_ZooLibStrim(fStrimU)))
		{
		if (!fReadDelimiter)
			return;
		sThrowParseException("Expected a value");
		}
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZYadMapR_ZooLibStrim

ZYadMapR_ZooLibStrim::ZYadMapR_ZooLibStrim(const ZStrimU& iStrimU, bool iReadDelimiter)
:	fStrimU(iStrimU),
	fReadDelimiter(iReadDelimiter)
	{}
	
void ZYadMapR_ZooLibStrim::Imp_ReadInc(bool iIsFirst, std::string& oName, ZRef<ZYadR_Std>& oYadR)
	{
	using namespace ZUtil_Strim;

	sSkip_WSAndCPlusPlusComments(fStrimU);

	if (!iIsFirst)
		{
		if (!sTryRead_CP(fStrimU, ',') && !sTryRead_CP(fStrimU, ';'))
			sThrowParseException("Expected ';' or ',' after property");
		sSkip_WSAndCPlusPlusComments(fStrimU);
		}

	if (fReadDelimiter)
		{
		if (sTryRead_CP(fStrimU, '}'))
			return;
		}

	if (!sTryRead_PropertyName(fStrimU, oName))
		{
		if (!fReadDelimiter)
			return;
		sThrowParseException("Expected property name");
		}

	sSkip_WSAndCPlusPlusComments(fStrimU);

	if (!sTryRead_CP(fStrimU, '='))
		sThrowParseException("Expected '=' after property name");

	if (!(oYadR = sMakeYadR_ZooLibStrim(fStrimU)))
		sThrowParseException("Expected value after '='");
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

		ZStrimW_Escaped::Options theOptions;
		theOptions.fQuoteQuotes = quoteQuotes;
		theOptions.fEscapeHighUnicode = false;
		
		ZStrimW_Escaped(theOptions, s).Write(iString);

		s.Write(delimiter);
		}
	}

static void sToStrim_Stream(const ZStrimW& s, const ZStreamRPos& iStreamRPos,
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
						.CopyFrom(iStreamRPos, iOptions.fRawChunkSize, &countCopied, nullptr);

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

static void sToStrim_Stream(const ZStrimW& s, const ZStreamR& iStreamR,
	size_t iLevel, const ZYadOptions& iOptions, bool iMayNeedInitialLF)
	{
	if (const ZStreamRPos* theStreamRPos = dynamic_cast<const ZStreamRPos*>(&iStreamR))
		{
		sToStrim_Stream(s, *theStreamRPos, iLevel, iOptions, iMayNeedInitialLF);
		return;
		}

	s.Write("(");

	ZStreamW_HexStrim(iOptions.fRawByteSeparator, "", 0, s)
		.CopyAllFrom(iStreamR);

	s.Write(")");
	}

static void sToStrim_Strim(const ZStrimW& s, const ZStrimR& iStrimR,
	size_t iLevel, const ZYadOptions& iOptions, bool iMayNeedInitialLF)
	{
	s.Write("\"");

	ZStrimW_Escaped::Options theOptions;
	theOptions.fQuoteQuotes = true;
	theOptions.fEscapeHighUnicode = false;
	
	ZStrimW_Escaped(theOptions, s)
		.CopyAllFrom(iStrimR);

	s.Write("\"");
	}

static void sToStrim_SimpleValue(const ZStrimW& s, const ZVal_ZooLib& iVal,
	size_t iLevel, const ZYadOptions& iOptions, bool iMayNeedInitialLF)
	{
	switch (iVal.TypeOf())
		{
		case eZType_Null: s.Write("Null"); break;
		case eZType_Type:
			{
			s.Write("Type(");
			s.Write(ZTypeAsString(iVal.GetType()));
			s.Write(")");
			break;
			}
		case eZType_ID:
			{
			s.Writef("ID(0x%0llX)", iVal.GetID());
			if (iOptions.fIDsHaveDecimalVersionComment)
				s.Writef(" /*%lld*/", iVal.GetID());
			break;
			}
		case eZType_Int8: s.Writef("int8(%d)", iVal.GetInt8()); break;
		case eZType_Int16: s.Writef("int16(%d)", iVal.GetInt16()); break;
		case eZType_Int32: s.Writef("int32(%d)", iVal.GetInt32()); break;
		case eZType_Int64: s.Writef("int64(0x%0llX)", iVal.GetInt64()); break;
		case eZType_Bool:
			{
			if (iVal.GetBool())
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
			s.Writef("float(%.9g)", iVal.GetFloat());
			break;
			}
		case eZType_Double:
			{
			// 17 decimal digits are necessary and sufficient for double precision IEEE 754.
			s.Writef("double(%.17g)", iVal.GetDouble());
			break;
			}
		case eZType_Time:
			{
			// For the moment I'm just writing times as a count of seconds, putting
			// the broken-out Gregorian version in a comment. Later we can improve
			// the parsing of dates, and then we can write them in human readable form.
			if (ZTime theTime = iVal.GetTime())
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
		case eZType_Pointer: s.Writef("pointer(%08X)", iVal.GetPointer()); break;
		case eZType_Rect:
			{
			const ZRectPOD& theRect = iVal.GetRect();
			s.Writef("Rect(%d, %d, %d, %d)",
				theRect.left,
				theRect.top,
				theRect.right,
				theRect.bottom);
			break;
			}
		case eZType_Point:
			{
			const ZPointPOD& thePoint = iVal.GetPoint();
			s.Writef("Point(%d, %d)",
				thePoint.h,
				thePoint.v);
			break;
			}
#if 0//##
		case eZType_Name:
			{
			s.Write("@");
			ZYad_ZooLibStrim::sWrite_PropName(s, iVal.GetName());
			break;
			}
#endif
		case eZType_String:
			{
			sWriteString(s, iOptions, iVal.GetString());
			break;
			}
		case eZType_RefCounted:
			s.Writef("RefCounted(%08X)", iVal.GetRefCounted().GetObject());
			break;
		case eZType_Raw:
		case eZType_Tuple:
		case eZType_Vector:
			{
			ZDebugStopf(0,
				("sToStrim_SimpleValue should only be called on simple tuple values"));
			break;
			}
		default:
			{
			ZDebugStopf(0, ("Unrecognized type %d", iVal.TypeOf()));
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
					s.Write(", ");
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
		for (;;)
			{
			string curName;
			if (ZRef<ZYadR> cur = iYadMapR->ReadInc(curName))
				{
				sWriteLFIndent(s, iLevel, iOptions);
				ZYad_ZooLibStrim::sWrite_PropName(s, curName);
				s << " = ";
				sToStrim_Yad(s, cur, iLevel + 1, iOptions, true);
				s.Write(";");
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
		for (;;)
			{
			string curName;
			if (ZRef<ZYadR> cur = iYadMapR->ReadInc(curName))
				{
				s.Write(" ");
				ZYad_ZooLibStrim::sWrite_PropName(s, curName);
				s << " = ";
				sToStrim_Yad(s, cur, iLevel + 1, iOptions, true);
				s.Write(";");
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
	else if (ZRef<ZYadStreamR> theYadStreamR = ZRefDynamicCast<ZYadStreamR>(iYadR))
		{
		sToStrim_Stream(s, theYadStreamR->GetStreamR(), iLevel, iOptions, iMayNeedInitialLF);
		}
	else if (ZRef<ZYadStrimR> theYadStrimR = ZRefDynamicCast<ZYadStrimR>(iYadR))
		{
		sToStrim_Strim(s, theYadStrimR->GetStrimR(), iLevel, iOptions, iMayNeedInitialLF);
		}
	else
		{
		sToStrim_SimpleValue(s,
			ZYad_ZooLib::sFromYadR(iYadR), iLevel, iOptions, iMayNeedInitialLF);
		}
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZYad_ZooLibStrim

bool ZYad_ZooLibStrim::sRead_Identifier(
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

ZRef<ZYadR> ZYad_ZooLibStrim::sMakeYadR(const ZStrimU& iStrimU)
	{ return sMakeYadR_ZooLibStrim(iStrimU); }

void ZYad_ZooLibStrim::sToStrim(const ZStrimW& s, ZRef<ZYadR> iYadR)
	{ sToStrim_Yad(s, iYadR, 0, ZYadOptions(), false); }

void ZYad_ZooLibStrim::sToStrim(const ZStrimW& s, ZRef<ZYadR> iYadR,
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

void ZYad_ZooLibStrim::sWrite_PropName(const ZStrimW& iStrimW, const string& iPropName)
	{
	if (sContainsProblemChars(iPropName))
		{
		iStrimW << "\"";
		ZStrimW_Escaped(iStrimW) << iPropName;
		iStrimW << "\"";
		}
	else
		{
		iStrimW << iPropName;
		}
	}

NAMESPACE_ZOOLIB_END
