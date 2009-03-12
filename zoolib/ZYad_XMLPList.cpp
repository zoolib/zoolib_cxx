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
#include "zoolib/ZStrimW_ML.h"
#include "zoolib/ZUtil_Strim.h"
#include "zoolib/ZUtil_Time.h"
#include "zoolib/ZYad_XMLPList.h"
#include "zoolib/ZYad_ZooLib.h"

NAMESPACE_ZOOLIB_BEGIN

using std::string;
using std::vector;

// =================================================================================================
#pragma mark -
#pragma mark * Static parsing functions

static void sThrowParseException(const string& iMessage)
	{
	throw ZYadParseException_XMLPList(iMessage);
	}

static void sBegin(ZML::Reader& r, const string& iTagName)
	{
	sSkipText(r);

	if (!sTryRead_Begin(r, iTagName))
		sThrowParseException("Expected begin tag '" + iTagName + "'");
	}

static void sEnd(ZML::Reader& r, const string& iTagName)
	{
	sSkipText(r);

	if (!sTryRead_End(r, iTagName))
		sThrowParseException("Expected end tag '" + iTagName + "'");
	}

static void sRead_BodyOfDict(ZML::Reader& r, ZTuple& oTuple);
static void sRead_BodyOfArray(ZML::Reader& r, vector<ZTValue>& oVector);

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
			sThrowParseException("Expected valid integer");

		oTV.SetInt32(theInteger);
		}
	else if (tagName == "real")
		{
		double theDouble;
		if (!ZUtil_Strim::sTryRead_SignedDouble(ZStrimU_Unreader(r.Text()), theDouble))
			sThrowParseException("Expected valid real");

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
		ZStreamR_Base64Decode(ZStreamR_ASCIIStrim(r.Text()))
			.CopyAllTo(ZStreamRWPos_MemoryBlock(theMB));

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
		sThrowParseException("Invalid begin tag '" + tagName + "'");
		}

	sEnd(r, tagName);
	return true;
	}

static void sRead_Value(ZML::Reader& r, ZTValue& oValue)
	{
	sSkipText(r);

	if (!sTryRead_Value(r, oValue))
		sThrowParseException("Expected value");
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

static void sRead_BodyOfArray(ZML::Reader& r, vector<ZTValue>& oVector)
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

static ZRef<ZYadR_Std> sMakeYadR_XMLPList(ZML::Reader& r)
	{
	sSkipText(r);

	if (r.Current() == ZML::eToken_TagBegin)
		{
		if (r.Name() == "dict")
			{
			r.Advance();
			return new ZYadMapR_XMLPList(r, true);
			}
		else if (r.Name() == "array")
			{
			r.Advance();
			return new ZYadListR_XMLPList(r, true);
			}
		else if (r.Name() == "data")
			{
			r.Advance();
			return new ZYadRawR_XMLPList(r, true);
			}
		}
	
	ZTValue theTV;
	if (sTryRead_Value(r, theTV))
		return new ZYadPrimR_Std(theTV);

	return ZRef<ZYadR_Std>();
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZYadParseException_XMLPList

ZYadParseException_XMLPList::ZYadParseException_XMLPList(const string& iWhat)
:	ZYadParseException_Std(iWhat)
	{}

ZYadParseException_XMLPList::ZYadParseException_XMLPList(const char* iWhat)
:	ZYadParseException_Std(iWhat)
	{}

// =================================================================================================
#pragma mark -
#pragma mark * ZYadRawR_XMLPList

ZYadRawR_XMLPList::ZYadRawR_XMLPList(ZML::Reader& iReader, bool iMustReadEndTag)
:	fR(iReader),
	fMustReadEndTag(iMustReadEndTag),
	fStreamR_ASCIIStrim(fR.Text()),
	fStreamR_Base64Decode(fStreamR_ASCIIStrim)
	{}

void ZYadRawR_XMLPList::Finish()
	{
	fStreamR_Base64Decode.SkipAll();

	if (fMustReadEndTag)
		sEnd(fR, "data");
	}

const ZStreamR& ZYadRawR_XMLPList::GetStreamR()
	{ return fStreamR_Base64Decode; }

// =================================================================================================
#pragma mark -
#pragma mark * ZYadReaderRep_XMLPList

ZYadListR_XMLPList::ZYadListR_XMLPList(ZML::Reader& iReader, bool iMustReadEndTag)
:	fR(iReader),
	fMustReadEndTag(iMustReadEndTag)
	{}

void ZYadListR_XMLPList::Imp_ReadInc(bool iIsFirst, ZRef<ZYadR_Std>& oYadR)
	{
	sSkipText(fR);

	if (fMustReadEndTag)
		{
		if (sTryRead_End(fR, "array"))
			return;
		}

	if (!(oYadR = sMakeYadR_XMLPList(fR)))
		{
		if (!fMustReadEndTag)
			return;
		sThrowParseException("Expected a value");
		}
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZYadMapR_XMLPList

ZYadMapR_XMLPList::ZYadMapR_XMLPList(ZML::Reader& iReader, bool iMustReadEndTag)
:	fR(iReader),
	fMustReadEndTag(iMustReadEndTag)
	{}
	
void ZYadMapR_XMLPList::Imp_ReadInc(bool iIsFirst, std::string& oName, ZRef<ZYadR_Std>& oYadR)
	{
	sSkipText(fR);

	if (fMustReadEndTag)
		{
		if (sTryRead_End(fR, "dict"))
			return;
		}

	if (!sTryRead_Begin(fR, "key"))
		{
		if (!fMustReadEndTag)
			return;
		sThrowParseException("Expected <key>");
		}

	ZStrimW_String(oName).CopyAllFrom(fR.Text());
	sEnd(fR, "key");

	if (!(oYadR = sMakeYadR_XMLPList(fR)))
		sThrowParseException("Expected value after <key>...</key>");
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZYad_XMLPList

ZRef<ZYadR> ZYad_XMLPList::sMakeYadR(ZML::Reader& r)
	{
	for (;;)
		{
		sSkipText(r);
		if (r.Current() != ZML::eToken_TagBegin || r.Name() != "plist")
			break;
		r.Advance();
		}
	return sMakeYadR_XMLPList(r);
	}

static void sToStrim_Raw(const ZStrimW_ML& s, const ZStreamR& iStreamR)
	{
	s.Begin("data");
		iStreamR.CopyAllTo(ZStreamW_Base64Encode(ZStreamW_ASCIIStrim(s)));
	s.End("data");	
	}

static void sToStrim_List(const ZStrimW_ML& s, ZRef<ZYadListR> iYadListR)
	{
	s.Begin("array");
	while (ZRef<ZYadR> theChild = iYadListR->ReadInc())
		ZYad_XMLPList::sToStrimW_ML(s, theChild);
	s.End("array");
	}

static void sToStrim_Map(const ZStrimW_ML& s, ZRef<ZYadMapR> iYadMapR)
	{
	s.Begin("dict");
	string theName;
	while (ZRef<ZYadR> theChild = iYadMapR->ReadInc(theName))
		{
		s.Begin("key");
			s << theName;
		s.End("key");
		ZYad_XMLPList::sToStrimW_ML(s, theChild);
		}
	s.End("dict");
	}


static void sToStrim_SimpleTValue(const ZStrimW_ML& s, const ZTValue& iTV)
	{
	switch (iTV.TypeOf())
		{
		case eZType_Tuple:
			{
			sToStrim_Map(s, new ZYadMapRPos_Tuple(iTV.GetTuple()));
			break;
			}
		case eZType_Vector:
			{
			sToStrim_List(s, new ZYadListRPos_Vector(iTV.GetVector()));
			break;
			}
		case eZType_String:
			{
			s.Begin("string");
				s.Write(iTV.GetString());
			s.End();
			break;
			}
		case eZType_Int32:
			{
			s.Begin("integer");
				s.Writef("%d", iTV.GetInt32());
			s.End("integer");
			break;
			}
		case eZType_Double:
			{
			s.Begin("real");
				s.Writef("%.17g", iTV.GetDouble());
			s.End("real");
			break;
			}
		case eZType_Bool:
			{
			if (iTV.GetBool())
				s.Empty("true");
			else
				s.Empty("false");
			break;
			}
		case eZType_Raw:
			{
			sToStrim_Raw(s, ZStreamRPos_MemoryBlock(iTV.GetRaw()));
			}
		case eZType_Time:
			{
			s.Begin("date");
				s << ZUtil_Time::sAsString_ISO8601(iTV.GetTime(), true);
			s.End("date");
			}
		}
	}

void ZYad_XMLPList::sToStrimW_ML(const ZStrimW_ML& s, ZRef<ZYadR> iYadR)
	{
	if (!iYadR)
		{
		return;
		}
	else if (ZRef<ZYadMapR> theYadMapR = ZRefDynamicCast<ZYadMapR>(iYadR))
		{
		sToStrim_Map(s, theYadMapR);
		}
	else if (ZRef<ZYadListR> theYadListR = ZRefDynamicCast<ZYadListR>(iYadR))
		{
		sToStrim_List(s, theYadListR);
		}
	else if (ZRef<ZYadRawR> theYadRawR = ZRefDynamicCast<ZYadRawR>(iYadR))
		{
		sToStrim_Raw(s, theYadRawR->GetStreamR());
		}
	else
		{
		sToStrim_SimpleTValue(s, ZYad_ZooLib::sFromYadR(iYadR));
		}	
	}

NAMESPACE_ZOOLIB_END
