static const char rcsid[] =
"@(#) $Id: ZUtil_Strim_OSXPList.cpp,v 1.3 2008/02/27 21:49:41 agreen Exp $";

/* ------------------------------------------------------------
Copyright (c) 2007 Andrew Green
http://www.zoolib.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
COPYRIGHT HOLDER(S) BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
------------------------------------------------------------ */

#include "ZUtil_Strim_OSXPList.h"

#include "ZMemoryBlock.h"
#include "ZML.h"
#include "ZStream_AsciiStrim.h"
#include "ZStream_Base64.h"
#include "ZStrimW_ML.h"
#include "ZString.h"
#include "ZUtil_Strim.h"
#include "ZUtil_Time.h"

using std::string;
using std::vector;

// =================================================================================================
#pragma mark -
#pragma mark * ZUtil_Strim_OSXPList::ParseException

ZUtil_Strim_OSXPList::ParseException::ParseException(const string& iWhat)
:	runtime_error(iWhat)
	{}

ZUtil_Strim_OSXPList::ParseException::ParseException(const char* iWhat)
:	runtime_error(iWhat)
	{}

// =================================================================================================
#pragma mark -
#pragma mark * Helpers

static void sBegin(ZML::Reader& r, const string& iTagName)
	{
	sSkipText(r);

	if (!sTryRead_Begin(r, iTagName))
		throw ZUtil_Strim_OSXPList::ParseException("Expected begin tag '" + iTagName + "'");
	}

static void sEnd(ZML::Reader& r, const string& iTagName)
	{
	sSkipText(r);

	if (!sTryRead_End(r, iTagName))
		throw ZUtil_Strim_OSXPList::ParseException("Expected end tag '" + iTagName + "'");
	}

static void sRead_BodyOfDict(ZML::Reader& r, ZTuple& oTuple);
void sRead_BodyOfArray(ZML::Reader& r, vector<ZTValue>& oVector);

static bool sTryRead_Value(ZML::Reader& r, ZTValue& oTV)
	{
	if (r.Current() == ZML::eToken_TagEmpty)
		{
		if (r.Name() == "true")
			{
			oTV.SetBool(true);
			r.Advance();
			return true;
			}

		if (r.Name() == "false")
			{
			oTV.SetBool(false);
			r.Advance();
			return true;
			}

		return false;
		}

	// If there's no open tag, then we're not at the start of a value.
	if (r.Current() != ZML::eToken_TagBegin)
		return false;

	const string tagName = r.Name();
	r.Advance();

	// We've read and advanced past an open tag, in tagName.
	if (tagName == "dict")
		{
		sRead_BodyOfDict(r, oTV.SetMutableTuple());
		}
	else if (tagName == "integer")
		{
		int64 theInteger;
		if (!ZUtil_Strim::sTryRead_SignedDecimalInteger(ZStrimU_Unreader(r.Text()), theInteger))
			throw ZUtil_Strim_OSXPList::ParseException("Expected valid integer");

		oTV.SetInt32(theInteger);
		}
	else if (tagName == "real")
		{
		double theDouble;
		if (!ZUtil_Strim::sTryRead_SignedDouble(ZStrimU_Unreader(r.Text()), theDouble))
			throw ZUtil_Strim_OSXPList::ParseException("Expected valid real");

		oTV.SetDouble(theDouble);
		}
	else if (tagName == "string")
		{
		string theString;
		ZStrimW_String(theString).CopyAllFrom(r.Text());

		oTV.SetString(theString);
		}
	else if (tagName == "data")
		{
		ZMemoryBlock theMB;
		ZStreamR_Base64Decode(ZStreamR_ASCIIStrim(r.Text())).CopyAllTo(ZStreamRWPos_MemoryBlock(theMB));

		oTV.SetRaw(theMB);
		}
	else if (tagName == "date")
		{
		string theDate;
		ZStrimW_String(theDate).CopyAllFrom(r.Text());

		oTV.SetTime(ZUtil_Time::sFromString_ISO8601(theDate));
		}
	else if (tagName == "array")
		{
		sRead_BodyOfArray(r, oTV.SetMutableVector());
		}
	else
		{
		// Hmm. Ignore tags we don't recognize?
		// throw ZUtil_Strim_OSXPList::ParseException("Invalid begin tag '" + tagName + "'");
		}

	sEnd(r, tagName);
	return true;
	}

static void sRead_Value(ZML::Reader& r, ZTValue& oValue)
	{
	sSkipText(r);

	if (!sTryRead_Value(r, oValue))
		throw ZUtil_Strim_OSXPList::ParseException("Expected value");
	}

static void sRead_BodyOfDict(ZML::Reader& r, ZTuple& oTuple)
	{
	ZRef<ZTupleRep> theRep = new ZTupleRep;
	ZTuple::PropList& theProperties = theRep->fProperties;

	// We've consumed a <dict> tag, and are pointing at any text following it.
	for (;;)
		{
		sSkipText(r);

		if (!sTryRead_Begin(r, "key"))
			break;

		string propertyName;
		ZStrimW_String(propertyName).CopyAllFrom(r.Text());

		sEnd(r, "key");

		ZTValue theTV;
		sRead_Value(r, theTV);

		theProperties.push_back(ZTuple::NameTV(ZTName(propertyName), theTV));
		}

	oTuple = ZTuple(theRep);
	}

void sRead_BodyOfArray(ZML::Reader& r, vector<ZTValue>& oVector)
	{
	for (;;)
		{
		sSkipText(r);

		ZTValue theTV;
		if (!sTryRead_Value(r, theTV))
			break;
		oVector.push_back(theTV);
		}
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZUtil_Strim_OSXPList

static void sToStrimW_ML(const ZStrimW_ML& s, const ZTValue& iTValue)
	{
	switch (iTValue.TypeOf())
		{
		case eZType_Tuple:
			{
			s.Begin("dict");
				const ZTuple& asTuple = iTValue.GetTuple();
				for (ZTuple::const_iterator i = asTuple.begin(); i != asTuple.end(); ++i)
					{
					s.Begin("key");
						s << asTuple.NameOf(i).AsString();
					s.End("key");

					sToStrimW_ML(s, asTuple.GetValue(i));
					}
			s.End("dict");
			break;
			}
		case eZType_Vector:
			{
			s.Begin("array");
				const vector<ZTValue>& asVector = iTValue.GetVector();
				for (vector<ZTValue>::const_iterator i = asVector.begin(); i != asVector.end(); ++i)
					sToStrimW_ML(s, *i);
			s.End("array");
			break;
			}
		case eZType_String:
			{
			s.Begin("string")
				.Write(iTValue.GetString())
			.End();
//				s << iTValue.GetString();
//			s.End("string");
			break;
			}
		case eZType_Int32:
			{
			s.Begin("integer");
				s.Writef("%d", iTValue.GetInt32());
			s.End("integer");
			break;
			}
		case eZType_Double:
			{
			s.Begin("real");
				s.Writef("%.17g", iTValue.GetDouble());
			s.End("real");
			break;
			}
		case eZType_Bool:
			{
			if (iTValue.GetBool())
				s.Empty("true");
			else
				s.Empty("false");
			break;
			}
		case eZType_Raw:
			{
			s.Begin("data");
				const ZMemoryBlock& theMB = iTValue.GetRaw();
				ZStreamW_Base64Encode(ZStreamW_ASCIIStrim(s)).Write(theMB.GetData(), theMB.GetSize());
			s.End("data");
			}
		case eZType_Time:
			{
			s.Begin("date");
				s << ZUtil_Time::sAsString_ISO8601(iTValue.GetTime(), true);
			s.End("date");
			}
		}
	}

void ZUtil_Strim_OSXPList::sToStrim(const ZStrimW& iStrimW, const ZTValue& iTValue)
	{
	ZStrimW_ML theStrimW_ML(iStrimW);
	sToStrimW_ML(theStrimW_ML, iTValue);
	}

bool ZUtil_Strim_OSXPList::sFromStrim(const ZStrimU& iStrimU, ZTValue& oTValue)
	{
	ZML::Reader r(iStrimU);
	return sFromML(r, oTValue);
	}

bool ZUtil_Strim_OSXPList::sFromML(ZML::Reader& r, ZTValue& oTValue)
	{
	// Ignore the leading text, ?xml, !DOCTYPE and plist tags
	for (;;)
		{
		sSkipText(r);

		if (r.Current() == ZML::eToken_TagBegin)
			{
			const string& tagName = r.Name();
			if (tagName == "?xml" || tagName == "!DOCTYPE" || tagName == "plist")
				{
				r.Advance();
				continue;
				}
			}
		break;
		}

	return sTryRead_Value(r, oTValue);
	}
