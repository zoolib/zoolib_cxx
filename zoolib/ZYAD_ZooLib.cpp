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

#include "zoolib/ZYAD_ZooLib.h"
#include "zoolib/ZMemoryBlock.h"
#include "zoolib/ZStrimR_Boundary.h"
#include "zoolib/ZStrimW_Escapify.h"
#include "zoolib/ZUtil_Time.h"

using std::string;
using std::vector;

// =================================================================================================
#pragma mark -
#pragma mark * Helper functions

static bool sIsComplexString(const ZYADOptions& iOptions, const string& iString)
	{
	if (iOptions.fStringLineLength && iString.size() > iOptions.fStringLineLength)
		{
		// We have a non-zero desired line length, and the string contains
		// more characters than that. This doesn't account for increases in width
		// due to escaping etc.
		return true;
		}

	if (iOptions.fBreakStrings)
		{
		// We've been asked to break strings at line ending characters,
		// which means (here and in ZStrimW_Escapify) LF and CR. Strictly
		// speaking we should use ZUnicode::sIsEOL().
		if (string::npos != iString.find_first_of("\n\r"))
			return true;
		}

	return false;
	}

static bool sIsComplex(const ZYADOptions& iOptions, const ZTValue& iTV)
	{
	switch (iTV.TypeOf())
		{
		case eZType_Raw:
			{
			size_t theSize;
			iTV.GetRawAttributes(nil, &theSize);
			return theSize > iOptions.fRawChunkSize;
			}
		case eZType_Vector:
			{
			const vector<ZTValue>& theVector = iTV.GetVector();
			if (theVector.empty())
				return false;

			if (theVector.size() == 1)
				return sIsComplex(iOptions, theVector.at(0));

			return true;
			}
		case eZType_Tuple:
			{
			const ZTuple& theTuple = iTV.GetTuple();
			if (theTuple.Empty())
				return false;

			if (theTuple.Count() == 1)
				return sIsComplex(iOptions, theTuple.GetValue(theTuple.begin()));

			return true;
			}
		case eZType_String: return sIsComplexString(iOptions, iTV.GetString());
		default: return false;
		}
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZYAD_ZTValue

ZYAD_ZTValue::ZYAD_ZTValue(const ZTValue& iTV)
:	fTV(iTV)
	{}

ZYAD_ZTValue::~ZYAD_ZTValue()
	{}

bool ZYAD_ZTValue::GetTValue(ZTValue& oTV)
	{
	oTV = fTV;
	return true;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZYADReaderRep_ZTValue definition

ZYADReaderRep_ZTValue::ZYADReaderRep_ZTValue(const ZTValue& iTV)
:	fTV(iTV)
	{}

bool ZYADReaderRep_ZTValue::HasValue()
	{ return true; }

ZType ZYADReaderRep_ZTValue::Type()
	{ return fTV.TypeOf(); }

ZRef<ZMapReaderRep> ZYADReaderRep_ZTValue::ReadMap()
	{
	if (fTV.TypeOf() == eZType_Tuple)
		return new ZMapReaderRep_ZTuple(fTV.GetTuple());
	return ZRef<ZMapReaderRep>();
	}

ZRef<ZListReaderRep> ZYADReaderRep_ZTValue::ReadList()
	{
	if (fTV.TypeOf() == eZType_Vector)
		return new ZListReaderRep_ZVector(fTV.GetVector());
	return ZRef<ZListReaderRep>();
	}

ZRef<ZStreamerR> ZYADReaderRep_ZTValue::ReadRaw()
	{
	if (fTV.TypeOf() == eZType_Raw)
		return new ZStreamerRPos_T<ZStreamRPos_MemoryBlock>(fTV.GetRaw());
	return ZRef<ZStreamerR>();
	}

ZRef<ZYAD> ZYADReaderRep_ZTValue::ReadYAD()
	{
	return new ZYAD_ZTValue(fTV);
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZMapReaderRep_ZTuple definition

ZMapReaderRep_ZTuple::ZMapReaderRep_ZTuple(const ZTuple& iTuple)
:	fTuple(iTuple),
	fIter(fTuple.begin())
	{}

bool ZMapReaderRep_ZTuple::HasValue()
	{ return fIter != fTuple.end(); }

string ZMapReaderRep_ZTuple::Name()
	{
	if (fIter != fTuple.end())
		return fTuple.NameOf(fIter).AsString();
	return string();
	}

ZRef<ZYADReaderRep> ZMapReaderRep_ZTuple::Read()
	{
	if (fIter != fTuple.end())
		return new ZYADReaderRep_ZTValue(fTuple.GetValue(fIter++));

	return ZRef<ZYADReaderRep>();
	}

void ZMapReaderRep_ZTuple::Skip()
	{
	if (fIter != fTuple.end())
		++fIter;
	}

bool ZMapReaderRep_ZTuple::IsSimple(const ZYADOptions& iOptions)
	{ return !sIsComplex(iOptions, fTuple); }

bool ZMapReaderRep_ZTuple::CanRandomAccess()
	{ return true; }

ZRef<ZYADReaderRep> ZMapReaderRep_ZTuple::ReadWithName(const string& iName)
	{
	ZTuple::const_iterator i = fTuple.IteratorOf(iName);
	if (i != fTuple.end())
		return new ZYADReaderRep_ZTValue(fTuple.GetValue(i));

	return ZRef<ZYADReaderRep>();
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZListReaderRep_ZVector definition

ZListReaderRep_ZVector::ZListReaderRep_ZVector(const vector<ZTValue>& iVector)
:	fVector(iVector),
	fIter(fVector.begin())
	{}

bool ZListReaderRep_ZVector::HasValue()
	{ return fIter != fVector.end(); }

size_t ZListReaderRep_ZVector::Index() const
	{ return fIter - fVector.begin(); }

ZRef<ZYADReaderRep> ZListReaderRep_ZVector::Read()
	{
	if (fIter != fVector.end())
		return new ZYADReaderRep_ZTValue(*fIter++);

	return ZRef<ZYADReaderRep>();
	}

void ZListReaderRep_ZVector::Skip()
	{
	if (fIter != fVector.end())
		++fIter;
	}

bool ZListReaderRep_ZVector::IsSimple(const ZYADOptions& iOptions)
	{ return !sIsComplex(iOptions, fVector); }

bool ZListReaderRep_ZVector::CanRandomAccess()
	{ return true; }

size_t ZListReaderRep_ZVector::Count()
	{ return fVector.size(); }

ZRef<ZYADReaderRep> ZListReaderRep_ZVector::ReadWithIndex(size_t iIndex)
	{
	if (iIndex < fVector.size())
		return new ZYADReaderRep_ZTValue(fVector.at(iIndex));

	return ZRef<ZYADReaderRep>();
	}

// =================================================================================================
#pragma mark -
#pragma mark * sFromReader

ZTValue sReadValue(ZYADReader iYADReader);

static ZTuple sReadTuple(ZMapReader iMapReader)
	{
	ZTuple result;
	while (iMapReader)
		{
		string theName = iMapReader.Name();
		ZTValue theValue = ZYADUtil_ZooLib::sFromReader(iMapReader.Read());
		result.SetValue(theName, theValue);
		}
	return result;
	}

static ZTValue sReadVector(ZListReader iListReader)
	{
	ZTValue result;
	vector<ZTValue>& theVector = result.SetMutableVector();
	while (iListReader)
		theVector.push_back(ZYADUtil_ZooLib::sFromReader(iListReader.Read()));
	return result;
	}

bool ZYADUtil_ZooLib::sFromReader(ZYADReader iYADReader, ZTValue& oTV)
	{
	if (!iYADReader)
		return false;

	if (iYADReader.IsMap())
		{
		oTV = sReadTuple(iYADReader.ReadMap());
		}
	else if (iYADReader.IsList())
		{
		oTV = sReadVector(iYADReader.ReadList());
		}
	else if (iYADReader.IsRaw())
		{
		ZMemoryBlock theMB;
		ZStreamRWPos_MemoryBlock(theMB).CopyAllFrom(iYADReader.ReadRaw()->GetStreamR());
		oTV = theMB;
		}
	else
		{
		ZRef<ZYAD> theYAD = iYADReader.ReadYAD();
		oTV = theYAD->GetTValue();
		}
	return true;
	}

ZTValue ZYADUtil_ZooLib::sFromReader(ZYADReader iYADReader)
	{
	ZTValue theTV;
	if (sFromReader(iYADReader, theTV))
		return theTV;
	return ZTValue();
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZYADUtil_ZooLib, local implementation

static void sWriteIndent(const ZStrimW& iStrimW,
	size_t iCount, const ZYADOptions& iOptions)
	{
	while (iCount--)
		iStrimW.Write(iOptions.fIndentString);
	}

static void sWriteLFIndent(const ZStrimW& iStrimW,
	size_t iCount, const ZYADOptions& iOptions)
	{
	iStrimW.Write(iOptions.fEOLString);
	sWriteIndent(iStrimW, iCount, iOptions);
	}

static void sWriteString(
	const ZStrimW& s, const ZYADOptions& iOptions, const string& iString)
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
	size_t iLevel, const ZYADOptions& iOptions)
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
			ZYADUtil_ZooLib::sWrite_PropName(s, iTV.GetName());
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

static void sToStrim_Raw(const ZStrimW& s, const ZStreamR& iStreamR,
	size_t iLevel, const ZYADOptions& iOptions, bool iMayNeedInitialLF)
	{
#if 0
	const ZStreamRPos* theStreamRPos = dynamic_cast<const ZStreamRPos*>(&iStreamR);

	if (theStreamRPos && theStreamRPos->GetSize() == 0)
		{
		// we've got an empty Raw
		s.Write("()");
		}
	else
		{
		ZStreamRPos_Memory dataStream(theData, theSize);

		if (iOptions.DoIndentation() && theSize > iOptions.fRawChunkSize)
			{
			if (iMayNeedInitialLF)
				sWriteLFIndent(s, iLevel, iOptions);
			
			s.Writef("( // %d bytes", theSize);
			sWriteLFIndent(s, iLevel, iOptions);
			if (iOptions.fRawAsASCII)
				{
				for (;;)
					{
					uint64 lastPos = dataStream.GetPosition();
					uint64 countCopied;
					ZStreamW_HexStrim(iOptions.fRawByteSeparator, "", 0, s)
						.CopyFrom(dataStream, iOptions.fRawChunkSize, &countCopied, nil);

					if (countCopied == 0)
						break;

					dataStream.SetPosition(lastPos);
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
						char theChar = dataStream.ReadInt8();
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
					eol, iOptions.fRawChunkSize, s).CopyAllFrom(dataStream);

				sWriteLFIndent(s, iLevel, iOptions);
				}

			s.Write(")");
			}
		else
			{
			s.Write("(");

			ZStreamW_HexStrim(iOptions.fRawByteSeparator, "", 0, s)
				.CopyAllFrom(dataStream);

			if (iOptions.fRawAsASCII)
				{
				dataStream.SetPosition(0);
				s.Write(" /* ");
				while (theSize--)
					{
					char theChar = dataStream.ReadInt8();
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
#endif
	}

static void sToStrim_YAD(const ZStrimW& s, ZYADReader iYADReader,
	size_t iInitialIndent, const ZYADOptions& iOptions, bool iMayNeedInitialLF);

static void sToStrim_List(const ZStrimW& s, ZListReader iListReader,
	size_t iLevel, const ZYADOptions& iOptions, bool iMayNeedInitialLF)
	{
	if (!iListReader)
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
		needsIndentation = !iListReader.IsSimple(iOptions) ;
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
			sToStrim_YAD(s, iListReader.Read(), iLevel, iOptions, false);
			if (!iListReader)
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
			sToStrim_YAD(s, iListReader.Read(), iLevel, iOptions, false);
			if (!iListReader)
				break;
			s.Write(", ");
			}
		s.Write("]");
		}
	}

static void sToStrim_Map(const ZStrimW& s, ZMapReader iMapReader,
	size_t iLevel, const ZYADOptions& iOptions, bool iMayNeedInitialLF)
	{
	if (!iMapReader)
		{
		// We've got an empty tuple.
		s.Write("{}");
		return;
		}

	bool needsIndentation = false;
	if (iOptions.DoIndentation())
		{
		needsIndentation = ! iMapReader.IsSimple(iOptions);
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
		while (iMapReader)
			{
			sWriteLFIndent(s, iLevel, iOptions);

			ZYADUtil_ZooLib::sWrite_PropName(s, iMapReader.Name());

			s << " = ";

			sToStrim_YAD(s, iMapReader.Read(), iLevel + 1, iOptions, true);
			s.Write(";");
			}

		sWriteLFIndent(s, iLevel, iOptions);

		s.Write("}");
		}
	else
		{
		s.Write("{");
		while (iMapReader)
			{
			s.Write(" ");

			ZYADUtil_ZooLib::sWrite_PropName(s, iMapReader.Name());

			s << " = ";

			sToStrim_YAD(s, iMapReader.Read(), iLevel + 1, iOptions, true);
			s.Write(";");
			}

		s.Write(" }");
		}
	}

static void sToStrim_YAD(const ZStrimW& s, ZYADReader iYADReader,
	size_t iLevel, const ZYADOptions& iOptions, bool iMayNeedInitialLF)
	{
	if (!iYADReader)
		return;

	if (iYADReader.IsMap())
		{
		sToStrim_Map(s, iYADReader.ReadMap(), iLevel, iOptions, iMayNeedInitialLF);
		}
	else if (iYADReader.IsList())
		{
		sToStrim_List(s, iYADReader.ReadList(), iLevel, iOptions, iMayNeedInitialLF);
		}
	else if (iYADReader.IsRaw())
		{
		sToStrim_Raw(s, iYADReader.ReadRaw()->GetStreamR(), iLevel, iOptions, iMayNeedInitialLF);
		}
	else
		{
		sToStrim_SimpleTValue(s, iYADReader.ReadYAD()->GetTValue(), iLevel, iOptions);
		}
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZYADUtil_ZooLib

void ZYADUtil_ZooLib::sToStrim(const ZStrimW& s, ZListReader iListReader)
	{ sToStrim_List(s, iListReader, 0, ZYADOptions(), false); }

void ZYADUtil_ZooLib::sToStrim(const ZStrimW& s, ZListReader iListReader,
	size_t iInitialIndent, const ZYADOptions& iOptions)
	{ sToStrim_List(s, iListReader, iInitialIndent, iOptions, false); }

void ZYADUtil_ZooLib::sToStrim(const ZStrimW& s, ZMapReader iMapReader)
	{ sToStrim_Map(s, iMapReader, 0, ZYADOptions(), false); }

void ZYADUtil_ZooLib::sToStrim(const ZStrimW& s, ZMapReader iMapReader,
	size_t iInitialIndent, const ZYADOptions& iOptions)
	{ sToStrim_Map(s, iMapReader, iInitialIndent, iOptions, false); }

void ZYADUtil_ZooLib::sToStrim(const ZStrimW& s, const ZStreamR& iStreamR)
	{ sToStrim_Raw(s, iStreamR, 0, ZYADOptions(), false); }

void ZYADUtil_ZooLib::sToStrim(const ZStrimW& s, const ZStreamR& iStreamR,
	size_t iInitialIndent, const ZYADOptions& iOptions)
	{ sToStrim_Raw(s, iStreamR, iInitialIndent, iOptions, false); }

void ZYADUtil_ZooLib::sToStrim(const ZStrimW& s, ZYADReader iYADReader)
	{ sToStrim_YAD(s, iYADReader, 0, ZYADOptions(), false); }

void ZYADUtil_ZooLib::sToStrim(const ZStrimW& s, ZYADReader iYADReader,
	size_t iInitialIndent, const ZYADOptions& iOptions)
	{ sToStrim_YAD(s, iYADReader, iInitialIndent, iOptions, false); }

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

void ZYADUtil_ZooLib::sWrite_PropName(const ZStrimW& iStrimW, const string& iPropName)
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

