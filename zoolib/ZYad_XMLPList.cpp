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

NAMESPACE_ZOOLIB_USING

using std::string;
using std::vector;

// =================================================================================================
#pragma mark -
#pragma mark * Static parsing functions

static void sBegin(ZML::Reader& r, const string& iTagName)
	{
	sSkipText(r);

	if (!sTryRead_Begin(r, iTagName))
		throw ZYadParseException_XMLPList("Expected begin tag '" + iTagName + "'");
	}

static void sEnd(ZML::Reader& r, const string& iTagName)
	{
	sSkipText(r);

	if (!sTryRead_End(r, iTagName))
		throw ZYadParseException_XMLPList("Expected end tag '" + iTagName + "'");
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
			throw ZYadParseException_XMLPList("Expected valid integer");

		oTV.SetInt32(theInteger);
		}
	else if (tagName == "real")
		{
		double theDouble;
		if (!ZUtil_Strim::sTryRead_SignedDouble(ZStrimU_Unreader(r.Text()), theDouble))
			throw ZYadParseException_XMLPList("Expected valid real");

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
		throw ZYadParseException_XMLPList("Invalid begin tag '" + tagName + "'");
		}

	sEnd(r, tagName);
	return true;
	}

static void sRead_Value(ZML::Reader& r, ZTValue& oValue)
	{
	sSkipText(r);

	if (!sTryRead_Value(r, oValue))
		throw ZYadParseException_XMLPList("Expected value");
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

static ZRef<ZYadR_XMLPList> sMakeYadR_XMLPList(ZML::Reader& r)
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
		return new ZYadPrimR_XMLPList(theTV);

	return ZRef<ZYadR_XMLPList>();
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZYadParseException_XMLPList

ZYadParseException_XMLPList::ZYadParseException_XMLPList(const string& iWhat)
:	ZYadParseException(iWhat)
	{}

ZYadParseException_XMLPList::ZYadParseException_XMLPList(const char* iWhat)
:	ZYadParseException(iWhat)
	{}

// =================================================================================================
#pragma mark -
#pragma mark * ZYadPrimR_XMLPList

ZYadPrimR_XMLPList::ZYadPrimR_XMLPList(const ZTValue& iTV)
:	ZYadR_TValue(iTV)
	{}

void ZYadPrimR_XMLPList::Finish()
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
	fMustReadEndTag(iMustReadEndTag),
	fPosition(0)
	{}

void ZYadListR_XMLPList::Finish()
	{
	this->SkipAll();

	if (fMustReadEndTag)
		sEnd(fR, "array");
	}

bool ZYadListR_XMLPList::HasChild()
	{
	this->pMoveIfNecessary();
	return fValue_Current;
	}

ZRef<ZYadR> ZYadListR_XMLPList::NextChild()
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

size_t ZYadListR_XMLPList::GetPosition()
	{ return fPosition; }

void ZYadListR_XMLPList::pMoveIfNecessary()
	{
	if (fValue_Current)
		return;

	if (fValue_Prior)
		{
		fValue_Prior->Finish();
		fValue_Prior.Clear();
		}

	fValue_Current = sMakeYadR_XMLPList(fR);
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZYadMapR_XMLPList

ZYadMapR_XMLPList::ZYadMapR_XMLPList(ZML::Reader& iReader, bool iMustReadEndTag)
:	fR(iReader),
	fMustReadEndTag(iMustReadEndTag)
	{}
	
void ZYadMapR_XMLPList::Finish()
	{
	this->SkipAll();

	if (fMustReadEndTag)
		sEnd(fR, "dict");
	}

bool ZYadMapR_XMLPList::HasChild()
	{
	this->pMoveIfNecessary();

	return fValue_Current;
	}

ZRef<ZYadR> ZYadMapR_XMLPList::NextChild()
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

string ZYadMapR_XMLPList::Name()
	{
	this->pMoveIfNecessary();

	return fName;
	}

void ZYadMapR_XMLPList::pMoveIfNecessary()
	{
	if (fValue_Current)
		return;

	if (fValue_Prior)
		{
		fValue_Prior->Finish();
		fValue_Prior.Clear();
		}


	fName.clear();

	sSkipText(fR);
	if (sTryRead_Begin(fR, "key"))
		{
		ZStrimW_String(fName).CopyAllFrom(fR.Text());
		sEnd(fR, "key");
		fValue_Current = sMakeYadR_XMLPList(fR);
		}
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZYadUtil_XMLPList

ZRef<ZYadR> ZYadUtil_XMLPList::sMakeYadR(ZML::Reader& r)
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
	while (iYadListR->HasChild())
		ZYadUtil_XMLPList::sToStrimW_ML(s, iYadListR->NextChild());
	s.End("array");
	}

static void sToStrim_Map(const ZStrimW_ML& s, ZRef<ZYadMapR> iYadMapR)
	{
	s.Begin("dict");
	while (iYadMapR->HasChild())
		{
		s.Begin("key");
			s << iYadMapR->Name();
		s.End("key");
		ZYadUtil_XMLPList::sToStrimW_ML(s, iYadMapR->NextChild());
		}
	s.End("dict");
	}


static void sToStrim_SimpleTValue(const ZStrimW_ML& s, const ZTValue& iTV)
	{
	switch (iTV.TypeOf())
		{
		case eZType_Tuple:
			{
			sToStrim_Map(s, new ZYadListMapRPos_Tuple(iTV.GetTuple()));
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

void ZYadUtil_XMLPList::sToStrimW_ML(const ZStrimW_ML& s, ZRef<ZYadR> iYadR)
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
		sToStrim_SimpleTValue(s, ZYadUtil_ZooLib::sFromYadR(iYadR));
		}	
	}
