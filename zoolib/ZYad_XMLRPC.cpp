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

#include "zoolib/ZDebug.h"
#include "zoolib/ZMemoryBlock.h"
#include "zoolib/ZStream_ASCIIStrim.h"
#include "zoolib/ZStream_Base64.h"
#include "zoolib/ZStrimU_Unreader.h"
#include "zoolib/ZUtil_Strim.h"
#include "zoolib/ZUtil_Time.h"
#include "zoolib/ZYad_StdMore.h"
#include "zoolib/ZYad_XMLRPC.h"

NAMESPACE_ZOOLIB_BEGIN

using std::string;

// =================================================================================================
#pragma mark -
#pragma mark * ZYadStreamR_XMLRPC

class ZYadStreamR_XMLRPC
:	public ZYadR_Std,
	public ZYadStreamR
	{
public:
	ZYadStreamR_XMLRPC(ZML::Reader& iReader);

// From ZYadR_Std
	virtual void Finish();

// From ZStreamerR via ZYadStreamR
	const ZStreamR& GetStreamR();

private:
	ZML::Reader& fR;
	ZStreamR_ASCIIStrim fStreamR_ASCIIStrim;
	ZStreamR_Base64Decode fStreamR_Base64Decode;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYadStrimR_XMLRPC

class ZYadStrimR_XMLRPC
:	public ZYadR_Std,
	public ZYadStrimR
	{
public:
	ZYadStrimR_XMLRPC(ZML::Reader& iReader);

// From ZYadR_Std
	virtual void Finish();

// From ZStrimmerR via ZYadStrimR
	const ZStrimR& GetStrimR();

private:
	ZML::Reader& fR;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYadListR_XMLRPC

class ZYadListR_XMLRPC : public ZYadListR_Std
	{
public:
	ZYadListR_XMLRPC(ZML::Reader& iReader);

// From ZYadListR_Std
	virtual void Imp_ReadInc(bool iIsFirst, ZRef<ZYadR_Std>& oYadR);

private:
	ZML::Reader& fR;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYadMapR_XMLRPC

class ZYadMapR_XMLRPC : public ZYadMapR_Std
	{
public:
	ZYadMapR_XMLRPC(ZML::Reader& iReader);

// From ZYadMapR_Std
	virtual void Imp_ReadInc(bool iIsFirst, std::string& oName, ZRef<ZYadR_Std>& oYadR);

private:
	ZML::Reader& fR;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Static parsing functions

static void spThrowParseException(const string& iMessage)
	{
	throw ZYadParseException_XMLRPC(iMessage);
	}

void spSkipCruft(ZML::Reader& r)
	{
	// Ignore the leading text, ?xml, !DOCTYPE and plist tags
	for (;;)
		{
		sSkipText(r);

		if (r.Current() == ZML::eToken_TagBegin)
			{
			const string& tagName = r.Name();
			if (tagName == "?xml" || tagName == "!DOCTYPE")
				{
				r.Advance();
				continue;
				}
			}
		break;
		}
	}

static void spBegin(ZML::Reader& r, const string& iTagName)
	{
	sSkipText(r);

	if (!sTryRead_Begin(r, iTagName))
		spThrowParseException("Expected begin tag '" + iTagName + "'");
	}

static void spEnd(ZML::Reader& r, const string& iTagName)
	{
	sSkipText(r);

	if (!sTryRead_End(r, iTagName))
		spThrowParseException("Expected end tag '" + iTagName + "'");
	}

static void spRead_TagWrappedText(ZML::Reader& r, const string& iTagName, string& oText)
	{
	spBegin(r, iTagName);

		ZStrimW_String(oText).CopyAllFrom(r.TextStrim());
	
	spEnd(r, iTagName);
	}

static bool spTryRead_SimpleValue(ZML::Reader& r, ZVal_ZooLib& oVal)
	{
	if (r.Current() == ZML::eToken_TagEmpty)
		{
		if (r.Name() == "nil")
			{
			oVal.Clear();
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
	else if (tagName == "i4" || tagName == "int")
		{
		int64 theInt64;
		if (!ZUtil_Strim::sTryRead_SignedDecimalInteger(ZStrimU_Unreader(r.TextStrim()), theInt64))
			spThrowParseException("Expected valid integer");

		oVal.SetInt32(theInt64);
		}
	else if (tagName == "boolean")
		{
		int64 theInt64;
		if (!ZUtil_Strim::sTryRead_DecimalInteger(ZStrimU_Unreader(r.TextStrim()), theInt64)
			|| (theInt64 != 0 && theInt64 != 1))
			{
			spThrowParseException("Expected 1 or 0 for boolean");
			}
		oVal.SetBool(theInt64);
		}
	else if (tagName == "double")
		{
		double theDouble;
		if (!ZUtil_Strim::sTryRead_SignedDouble(ZStrimU_Unreader(r.TextStrim()), theDouble))
			spThrowParseException("Expected valid double");

		oVal.SetDouble(theDouble);
		}
	else if (tagName == "dateTime.iso8601")
		{
		string theDate;
		ZStrimW_String(theDate).CopyAllFrom(r.TextStrim());

		oVal.SetTime(ZUtil_Time::sFromString_ISO8601(theDate));
		}
	else if (tagName == "struct")
		{
		ZDebugStopf(1, ("sTryRead_SimpleValue, given a struct"));
		}
	else if (tagName == "array")
		{
		ZDebugStopf(1, ("sTryRead_SimpleValue, given an array"));
		}
	else if (tagName == "base64")
		{
		ZDebugStopf(1, ("sTryRead_SimpleValue, given a base64"));
		}
	else if (tagName == "string")
		{
		ZDebugStopf(1, ("sTryRead_SimpleValue, given a string"));
		}
	else
		{
		// Hmm. Ignore tags we don't recognize?
		spThrowParseException("Invalid begin tag '" + tagName + "'");
		}

	spEnd(r, tagName);
	return true;
	}

static void spRead_SimpleValue(ZML::Reader& r, ZVal_ZooLib& oVal)
	{
	sSkipText(r);

	if (!spTryRead_SimpleValue(r, oVal))
		spThrowParseException("Expected value");
	}

static ZRef<ZYadR_Std> spMakeYadR_XMLRPC(ZML::Reader& r)
	{
	sSkipText(r);

	if (r.Current() == ZML::eToken_TagBegin)
		{
		if (r.Name() == "struct")
			{
			r.Advance();
			return new ZYadMapR_XMLRPC(r);
			}
		else if (r.Name() == "array")
			{
			r.Advance();
			return new ZYadListR_XMLRPC(r);
			}
		else if (r.Name() == "base64")
			{
			r.Advance();
			return new ZYadStreamR_XMLRPC(r);
			}
		else if (r.Name() == "string")
			{
			r.Advance();
			return new ZYadStrimR_XMLRPC(r);
			}
		}
	
	ZVal_ZooLib theVal;
	if (spTryRead_SimpleValue(r, theVal))
		return new ZYadPrimR_Std(theVal);

	return ZRef<ZYadR_Std>();
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZYadParseException_XMLRPC

ZYadParseException_XMLRPC::ZYadParseException_XMLRPC(const string& iWhat)
:	ZYadParseException_Std(iWhat)
	{}

ZYadParseException_XMLRPC::ZYadParseException_XMLRPC(const char* iWhat)
:	ZYadParseException_Std(iWhat)
	{}

// =================================================================================================
#pragma mark -
#pragma mark * ZYadStreamR_XMLRPC

ZYadStreamR_XMLRPC::ZYadStreamR_XMLRPC(ZML::Reader& iReader)
:	fR(iReader),
	fStreamR_ASCIIStrim(fR.TextStrim()),
	fStreamR_Base64Decode(fStreamR_ASCIIStrim)
	{}

void ZYadStreamR_XMLRPC::Finish()
	{
	fStreamR_Base64Decode.SkipAll();
	spEnd(fR, "base64");
	}

const ZStreamR& ZYadStreamR_XMLRPC::GetStreamR()
	{ return fStreamR_Base64Decode; }

// =================================================================================================
#pragma mark -
#pragma mark * ZYadStrimR_XMLRPC

ZYadStrimR_XMLRPC::ZYadStrimR_XMLRPC(ZML::Reader& iReader)
:	fR(iReader)
	{}

void ZYadStrimR_XMLRPC::Finish()
	{
	fR.Advance();
	spEnd(fR, "string");
	}

const ZStrimR& ZYadStrimR_XMLRPC::GetStrimR()
	{ return fR.TextStrim(); }

// =================================================================================================
#pragma mark -
#pragma mark * ZYadListR_XMLRPC

ZYadListR_XMLRPC::ZYadListR_XMLRPC(ZML::Reader& iReader)
:	fR(iReader)
	{}

void ZYadListR_XMLRPC::Imp_ReadInc(bool iIsFirst, ZRef<ZYadR_Std>& oYadR)
	{
	sSkipText(fR);

	if (!iIsFirst)
		{
		spEnd(fR, "value");

		sSkipText(fR);
		}

	if (sTryRead_End(fR, "data"))
		{
		sSkipText(fR);

		spEnd(fR, "array");

		sSkipText(fR);

		return;
		}

	spBegin(fR, "value");
	
	if (!(oYadR = spMakeYadR_XMLRPC(fR)))
		spThrowParseException("Expected a value");
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZYadListR_XMLRPC_Params

class ZYadListR_XMLRPC_Params : public ZYadListR_Std
	{
public:
	ZYadListR_XMLRPC_Params(ZML::Reader& iReader, bool iIsResponse);

// From ZYadListR_Std
	virtual void Imp_ReadInc(bool iIsFirst, ZRef<ZYadR_Std>& oYadR);

private:
	ZML::Reader& fR;
	bool fIsResponse;
	};


ZYadListR_XMLRPC_Params::ZYadListR_XMLRPC_Params(ZML::Reader& iReader, bool iIsResponse)
:	fR(iReader),
	fIsResponse(iIsResponse)
	{}

void ZYadListR_XMLRPC_Params::Imp_ReadInc(bool iIsFirst, ZRef<ZYadR_Std>& oYadR)
	{
	sSkipText(fR);

	if (!iIsFirst)
		{
		spEnd(fR, "value");

		sSkipText(fR);

		spEnd(fR, "param");

		sSkipText(fR);
		}

	if (sTryRead_End(fR, "params"))
		{
		sSkipText(fR);

		if (fIsResponse)
			spEnd(fR, "methodResponse");
		else
			spEnd(fR, "methodCall");

		sSkipText(fR);

		return;
		}

	spBegin(fR, "param");

	sSkipText(fR);

	spBegin(fR, "value");
	
	if (!(oYadR = spMakeYadR_XMLRPC(fR)))
		spThrowParseException("Expected a value");
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZYadListR_XMLRPC_Fault

class ZYadListR_XMLRPC_Fault : public ZYadListR_Std
	{
public:
	ZYadListR_XMLRPC_Fault(ZML::Reader& iReader);

// From ZYadListR_Std
	virtual void Imp_ReadInc(bool iIsFirst, ZRef<ZYadR_Std>& oYadR);

private:
	ZML::Reader& fR;
	};


ZYadListR_XMLRPC_Fault::ZYadListR_XMLRPC_Fault(ZML::Reader& iReader)
:	fR(iReader)
	{}

void ZYadListR_XMLRPC_Fault::Imp_ReadInc(bool iIsFirst, ZRef<ZYadR_Std>& oYadR)
	{
	sSkipText(fR);

	if (!iIsFirst)
		{
		spEnd(fR, "value");

		sSkipText(fR);

		spEnd(fR, "fault");

		sSkipText(fR);

		spEnd(fR, "methodResponse");

		sSkipText(fR);

		return;
		}

	spBegin(fR, "value");
	
	if (!(oYadR = spMakeYadR_XMLRPC(fR)))
		spThrowParseException("Expected a value");
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZYadMapR_XMLRPC

ZYadMapR_XMLRPC::ZYadMapR_XMLRPC(ZML::Reader& iReader)
:	fR(iReader)
	{}
	
void ZYadMapR_XMLRPC::Imp_ReadInc(bool iIsFirst, std::string& oName, ZRef<ZYadR_Std>& oYadR)
	{
	sSkipText(fR);

	if (!iIsFirst)
		{
		spEnd(fR, "value");

		sSkipText(fR);

		spEnd(fR, "member");

		sSkipText(fR);
		}

	if (sTryRead_End(fR, "struct"))
		{
		sSkipText(fR);
		return;
		}

	spBegin(fR, "member");

	sSkipText(fR);

	spBegin(fR, "name");

	ZStrimW_String(oName).CopyAllFrom(fR.TextStrim());

	spEnd(fR, "name");

	sSkipText(fR);

	spBegin(fR, "value");

	if (!(oYadR = spMakeYadR_XMLRPC(fR)))
		spThrowParseException("Expected value");
	}

// =================================================================================================
#pragma mark -
#pragma mark * Static writing functions

static void spToStrim(const ZML::StrimW& s, ZRef<ZYadR> iYadR);

static void spToStrim_Stream(const ZML::StrimW& s, const ZStreamR& iStreamR)
	{
	s.Begin("base64");
		iStreamR.CopyAllTo(ZStreamW_Base64Encode(ZStreamW_ASCIIStrim(s)));
	s.End("base64");	
	}

static void spToStrim_Strim(const ZML::StrimW& s, const ZStrimR& iStrimR)
	{
	s.Begin("string");
		iStrimR.CopyAllTo(s);
	s.End("string");	
	}

static void spToStrim_List(const ZML::StrimW& s, ZRef<ZYadListR> iYadListR)
	{
	s.Begin("array");
		s.Begin("data");
			while (ZRef<ZYadR> theChild = iYadListR->ReadInc())
				{
				s.Begin("value");
					spToStrim(s, theChild);
				s.End("value");
				}
		s.End("data");
	s.End("array");
	}

static void spToStrim_Map(const ZML::StrimW& s, ZRef<ZYadMapR> iYadMapR)
	{
	s.Begin("struct");
		string theName;
		while (ZRef<ZYadR> theChild = iYadMapR->ReadInc(theName))
			{
			s.Begin("member");
				s.Begin("name");
					s << theName;
				s.End("name");

				s.Begin("value");
					spToStrim(s, theChild);
				s.End("value");
			s.End("member");
			}
	s.End("struct");
	}

static void spToStrim_SimpleValue(const ZML::StrimW& s, const ZVal_ZooLib& iVal)
	{
	switch (iVal.TypeOf())
		{
		case eZType_Int32:
			{
			s.Begin("i4");
				s.Writef("%d", iVal.GetInt32());
			s.End("i4");
			break;
			}
		case eZType_Double:
			{
			s.Begin("double");
				s.Writef("%.17g", iVal.GetDouble());
			s.End("double");
			break;
			}
		case eZType_Bool:
			{
			s.Begin("boolean");
				if (iVal.GetBool())
					s << "1";
				else
					s << "0";
			s.End("boolean");
			break;
			}
		case eZType_Raw:
			{
			spToStrim_Stream(s, ZStreamRPos_MemoryBlock(iVal.GetRaw()));
			break;
			}
		case eZType_Time:
			{
			s.Begin("dateTime.iso8601");
				s << ZUtil_Time::sAsString_ISO8601(iVal.GetTime(), true);
			s.End("dateTime.iso8601");
			break;
			}
		case eZType_Null:
			{
			s.Empty("nil");
			break;
			}
		case eZType_Tuple:
			{
			ZDebugStopf(1, ("spToStrim_SimpleValue, given a tuple"));
			break;
			}
		case eZType_Vector:
			{
			ZDebugStopf(1, ("spToStrim_SimpleValue, given a tuple"));
			break;
			}
		case eZType_String:
			{
			ZDebugStopf(1, ("spToStrim_SimpleValue, given a string"));
			break;
			}
		default:
			{
			ZDebugStopf(1, ("spToStrim_SimpleValue, given an unsupported type"));
			}
		}
	}

static void spToStrim(const ZML::StrimW& s, ZRef<ZYadR> iYadR)
	{
	if (!iYadR)
		{
		return;
		}
	else if (ZRef<ZYadMapR> theYadMapR = ZRefDynamicCast<ZYadMapR>(iYadR))
		{
		spToStrim_Map(s, theYadMapR);
		}
	else if (ZRef<ZYadListR> theYadListR = ZRefDynamicCast<ZYadListR>(iYadR))
		{
		spToStrim_List(s, theYadListR);
		}
	else if (ZRef<ZYadStreamR> theYadStreamR = ZRefDynamicCast<ZYadStreamR>(iYadR))
		{
		spToStrim_Stream(s, theYadStreamR->GetStreamR());
		}
	else if (ZRef<ZYadStrimR> theYadStrimR = ZRefDynamicCast<ZYadStrimR>(iYadR))
		{
		spToStrim_Strim(s, theYadStrimR->GetStrimR());
		}
	else
		{
		spToStrim_SimpleValue(s, sFromYadR_T<ZVal_ZooLib>(iYadR));
		}	
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZYad_XMLRPC

namespace ZYad_XMLRPC {

bool sFromReader(ZML::Reader& iR, Request_t& oRequest)
	{
	spSkipCruft(iR);

	if (!sTryRead_Begin(iR, "methodCall"))
		return false;

	spRead_TagWrappedText(iR, "methodName", oRequest.fMethodName);

	sSkipText(iR);

	if (sTryRead_Begin(iR, "params"))
		{
		oRequest.fParams = new ZYadListR_XMLRPC_Params(iR, false);
		}
	else
		{
		oRequest.fParams.Clear();

		spEnd(iR, "methodCall");
		}

	return true;
	}

bool sFromReader(ZML::Reader& iR, Response_t& oResponse)
	{
	spSkipCruft(iR);

	if (!sTryRead_Begin(iR, "methodResponse"))
		return false;

	sSkipText(iR);

	if (sTryRead_Begin(iR, "params"))
		{
		oResponse.fResult = new ZYadListR_XMLRPC_Params(iR, true);
		}
	else if (sTryRead_Begin(iR, "fault"))
		{
		oResponse.fFault = new ZYadListR_XMLRPC_Fault(iR);
		}
	else
		{
		spThrowParseException("Expected 'fault' or 'params'");
		}

	return true;
	}

void sToStrim(const ZML::StrimW& s, const Request_t& iRequest)
	{
	s.Begin("methodCall");

		s.Begin("methodName");
			s << iRequest.fMethodName;
		s.End("methodName");

		if (iRequest.fParams)
			{
			s.Begin("params");
			while (ZRef<ZYadR> theChild = iRequest.fParams->ReadInc())
				{
				s.Begin("param");
					s.Begin("value");
						spToStrim(s, theChild);
					s.End("value");
				s.End("param");
				}
			s.End("params");
			}

	s.End("methodCall");
	}

void sToStrim(const ZML::StrimW& s, const Response_t& iResponse)
	{
	s.Begin("methodResponse");

		if (iResponse.fResult)
			{
			s.Begin("params");
				s.Begin("param");
					s.Begin("value");
						spToStrim(s, iResponse.fResult);
					s.End("value");
				s.End("param");
			s.End("params");
			}
		else if (iResponse.fFault)
			{
			s.Begin("fault");
				s.Begin("value");
					spToStrim(s, iResponse.fFault);
				s.End("value");
			s.End("fault");
			}

	s.End("methodResponse");
	}

} // namespace ZYad_XMLRPC

NAMESPACE_ZOOLIB_END
