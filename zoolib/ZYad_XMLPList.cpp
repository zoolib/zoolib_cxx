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

#include "zoolib/ZDebug.h"
#include "zoolib/ZStrimU_Unreader.h"
#include "zoolib/ZUtil_Strim.h"
#include "zoolib/ZUtil_Time.h"
#include "zoolib/ZYad_StdMore.h"
#include "zoolib/ZYad_XMLPList.h"

NAMESPACE_ZOOLIB_BEGIN

using std::string;

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

static bool sTryRead_SimpleValue(ZML::Reader& r, ZVal_ZooLib& oVal)
	{
	if (r.Current() == ZML::eToken_TagEmpty)
		{
		if (r.Name() == "true")
			{
			oVal.SetBool(true);
			r.Advance();
			return true;
			}

		if (r.Name() == "false")
			{
			oVal.SetBool(false);
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
	if (false)
		{}
	else if (tagName == "integer")
		{
		int64 theInteger;
		if (!ZUtil_Strim::sTryRead_SignedDecimalInteger(ZStrimU_Unreader(r.TextStrim()), theInteger))
			sThrowParseException("Expected valid integer");

		oVal.SetInt32(theInteger);
		}
	else if (tagName == "real")
		{
		double theDouble;
		if (!ZUtil_Strim::sTryRead_SignedDouble(ZStrimU_Unreader(r.TextStrim()), theDouble))
			sThrowParseException("Expected valid real");

		oVal.SetDouble(theDouble);
		}
	else if (tagName == "date")
		{
		const string theDate = r.TextString();

		oVal.SetTime(ZUtil_Time::sFromString_ISO8601(theDate));
		}
	else if (tagName == "dict")
		{
		ZDebugStopf(1, ("sTryRead_SimpleValue, given a dict"));
		}
	else if (tagName == "array")
		{
		ZDebugStopf(1, ("sTryRead_SimpleValue, given an array"));
		}
	else if (tagName == "data")
		{
		ZDebugStopf(1, ("sTryRead_SimpleValue, given a data"));
		}
	else if (tagName == "string")
		{
		ZDebugStopf(1, ("sTryRead_SimpleValue, given a string"));
		}
	else
		{
		// Hmm. Ignore tags we don't recognize?
		sThrowParseException("Invalid begin tag '" + tagName + "'");
		}

	sEnd(r, tagName);
	return true;
	}

static void sRead_SimpleValue(ZML::Reader& r, ZVal_ZooLib& oVal)
	{
	sSkipText(r);

	if (!sTryRead_SimpleValue(r, oVal))
		sThrowParseException("Expected value");
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
			return new ZYadStreamR_XMLPList(r, true);
			}
		else if (r.Name() == "string")
			{
			r.Advance();
			return new ZYadStrimR_XMLPList(r, true);
			}
		}
	
	ZVal_ZooLib theVal;
	if (sTryRead_SimpleValue(r, theVal))
		return new ZYadPrimR_Std(theVal);

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
#pragma mark * ZYadStreamR_XMLPList

ZYadStreamR_XMLPList::ZYadStreamR_XMLPList(ZML::Reader& iReader, bool iMustReadEndTag)
:	fR(iReader),
	fMustReadEndTag(iMustReadEndTag),
	fStreamR_ASCIIStrim(fR.TextStrim()),
	fStreamR_Base64Decode(fStreamR_ASCIIStrim)
	{}

void ZYadStreamR_XMLPList::Finish()
	{
	fStreamR_Base64Decode.SkipAll();

	if (fMustReadEndTag)
		sEnd(fR, "data");
	}

const ZStreamR& ZYadStreamR_XMLPList::GetStreamR()
	{ return fStreamR_Base64Decode; }

// =================================================================================================
#pragma mark -
#pragma mark * ZYadStrimR_XMLPList

ZYadStrimR_XMLPList::ZYadStrimR_XMLPList(ZML::Reader& iReader, bool iMustReadEndTag)
:	fR(iReader),
	fMustReadEndTag(iMustReadEndTag)
	{}

void ZYadStrimR_XMLPList::Finish()
	{
	fR.Advance();
	if (fMustReadEndTag)
		sEnd(fR, "string");
	}

const ZStrimR& ZYadStrimR_XMLPList::GetStrimR()
	{ return fR.TextStrim(); }

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

	oName = fR.TextString();

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

static void sToStrim_Stream(const ZML::StrimW& s, const ZStreamR& iStreamR)
	{
	s.Begin("data");
		iStreamR.CopyAllTo(ZStreamW_Base64Encode(ZStreamW_ASCIIStrim(s)));
	s.End("data");	
	}

static void sToStrim_Strim(const ZML::StrimW& s, const ZStrimR& iStrimR)
	{
	s.Begin("string");
		iStrimR.CopyAllTo(s);
	s.End("string");	
	}

static void sToStrim_List(const ZML::StrimW& s, ZRef<ZYadListR> iYadListR)
	{
	s.Begin("array");
		while (ZRef<ZYadR> theChild = iYadListR->ReadInc())
			ZYad_XMLPList::sToStrimW_ML(s, theChild);
	s.End("array");
	}

static void sToStrim_Map(const ZML::StrimW& s, ZRef<ZYadMapR> iYadMapR)
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

static void sToStrim_SimpleValue(const ZML::StrimW& s, const ZVal_ZooLib& iVal)
	{
	switch (iVal.TypeOf())
		{
		case eZType_Int32:
			{
			s.Begin("integer");
				s.Writef("%d", iVal.GetInt32());
			s.End("integer");
			break;
			}
		case eZType_Double:
			{
			s.Begin("real");
				s.Writef("%.17g", iVal.GetDouble());
			s.End("real");
			break;
			}
		case eZType_Bool:
			{
			if (iVal.GetBool())
				s.Empty("true");
			else
				s.Empty("false");
			break;
			}
		case eZType_Raw:
			{
			sToStrim_Stream(s, ZStreamRPos_ValData_T<ZValData_ZooLib>(iVal.GetData()));
			break;
			}
		case eZType_Time:
			{
			s.Begin("date");
				s << ZUtil_Time::sAsString_ISO8601(iVal.GetTime(), true);
			s.End("date");
			break;
			}
		case eZType_Tuple:
			{
			ZDebugStopf(0, ("sToStrim_SimpleValue, given a tuple"));
			break;
			}
		case eZType_Vector:
			{
			ZDebugStopf(0, ("sToStrim_SimpleValue, given a vector"));
			break;
			}
		case eZType_String:
			{
			ZDebugStopf(0, ("sToStrim_SimpleValue, given a string"));
			break;
			}
		default:
			{
			ZDebugStopf(0, ("sToStrim_SimpleValue, given an unsupported type"));
			}
		}
	}

void ZYad_XMLPList::sToStrimW_ML(const ZML::StrimW& s, ZRef<ZYadR> iYadR)
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
	else if (ZRef<ZYadStreamR> theYadStreamR = ZRefDynamicCast<ZYadStreamR>(iYadR))
		{
		sToStrim_Stream(s, theYadStreamR->GetStreamR());
		}
	else if (ZRef<ZYadStrimR> theYadStrimR = ZRefDynamicCast<ZYadStrimR>(iYadR))
		{
		sToStrim_Strim(s, theYadStrimR->GetStrimR());
		}
	else
		{
		sToStrim_SimpleValue(s, ZYad_ZooLib::sFromYadR(iYadR));
		}	
	}

NAMESPACE_ZOOLIB_END
