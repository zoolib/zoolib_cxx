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
#include "zoolib/ZStream_ASCIIStrim.h"
#include "zoolib/ZStream_Base64.h"
#include "zoolib/ZUtil_Any.h"
#include "zoolib/ZUtil_Strim.h"
#include "zoolib/ZUtil_Strim_Operators.h"
#include "zoolib/ZUtil_Time.h"
#include "zoolib/ZYad_XMLRPC.h"

namespace ZooLib {

using std::string;

// =================================================================================================
// MARK: - ZYadStreamR_XMLRPC

class ZYadStreamR_XMLRPC
:	public ZYadStreamR
	{
public:
	ZYadStreamR_XMLRPC(ZRef<ZML::StrimmerU> iStrimmerU);

// From ZYadR
	virtual void Finish();

// From ZStreamerR via ZYadStreamR
	const ZStreamR& GetStreamR();

private:
	ZRef<ZML::StrimmerU>& fStrimmerU;
	ZStreamR_ASCIIStrim fStreamR_ASCIIStrim;
	ZStreamR_Base64Decode fStreamR_Base64Decode;
	};

// =================================================================================================
// MARK: - ZYadStrimR_XMLRPC

class ZYadStrimR_XMLRPC
:	public ZYadStrimR
	{
public:
	ZYadStrimR_XMLRPC(ZRef<ZML::StrimmerU> iStrimmerU);

// From ZYadR
	virtual void Finish();

// From ZStrimmerU via ZYadStrimR
	const ZStrimR& GetStrimR();

private:
	ZRef<ZML::StrimmerU> fStrimmerU;
	};

// =================================================================================================
// MARK: - ZYadSeqR_XMLRPC

class ZYadSeqR_XMLRPC : public ZYadSeqR_Std
	{
public:
	ZYadSeqR_XMLRPC(ZRef<ZML::StrimmerU> iStrimmerU);

// From ZYadSeqR_Std
	virtual void Imp_ReadInc(bool iIsFirst, ZRef<ZYadR>& oYadR);

private:
	ZRef<ZML::StrimmerU> fStrimmerU;
	};

// =================================================================================================
// MARK: - ZYadMapR_XMLRPC

class ZYadMapR_XMLRPC : public ZYadMapR_Std
	{
public:
	ZYadMapR_XMLRPC(ZRef<ZML::StrimmerU> iStrimmerU);

// From ZYadMapR_Std
	virtual void Imp_ReadInc(bool iIsFirst, std::string& oName, ZRef<ZYadR>& oYadR);

private:
	ZRef<ZML::StrimmerU> fStrimmerU;
	};

// =================================================================================================
// MARK: - Static parsing functions

static void spThrowParseException(const string& iMessage)
	{
	throw ZYadParseException_XMLRPC(iMessage);
	}

void spSkipCruft(ZML::StrimU& r)
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

static void spBegin(ZML::StrimU& r, const string& iTagName)
	{
	sSkipText(r);

	if (!sTryRead_Begin(r, iTagName))
		spThrowParseException("Expected begin tag '" + iTagName + "'");
	}

static void spEnd(ZML::StrimU& r, const string& iTagName)
	{
	sSkipText(r);

	if (!sTryRead_End(r, iTagName))
		spThrowParseException("Expected end tag '" + iTagName + "'");
	}

static void spRead_TagWrappedText(ZML::StrimU& r, const string& iTagName, string& oText)
	{
	spBegin(r, iTagName);

		oText = r.ReadAll8();

	spEnd(r, iTagName);
	}

static bool spTryRead_SimpleValue(ZML::StrimU& r, ZAny& oVal)
	{
	if (r.Current() == ZML::eToken_TagEmpty)
		{
		if (r.Name() == "nil")
			{
			oVal = ZAny();
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
		if (not ZUtil_Strim::sTryRead_SignedDecimalInteger(r, theInt64))
			spThrowParseException("Expected valid integer");

		oVal = int32(theInt64);
		}
	else if (tagName == "boolean")
		{
		int64 theInt64;
		if (not ZUtil_Strim::sTryRead_DecimalInteger(r, theInt64)
			|| (theInt64 != 0 && theInt64 != 1))
			{
			spThrowParseException("Expected 1 or 0 for boolean");
			}

		oVal = bool(theInt64);
		}
	else if (tagName == "double")
		{
		double theDouble;
		if (not ZUtil_Strim::sTryRead_SignedDouble(r, theDouble))
			spThrowParseException("Expected valid double");

		oVal = theDouble;
		}
	else if (tagName == "dateTime.iso8601")
		{
		oVal = ZUtil_Time::sFromString_ISO8601(r.ReadAll8());
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

static ZRef<ZYadR> spMakeYadR_XMLRPC(ZRef<ZML::StrimmerU> iStrimmerU)
	{
	ZML::StrimU& theR = iStrimmerU->GetStrim();

	sSkipText(theR);

	if (theR.Current() == ZML::eToken_TagBegin)
		{
		if (theR.Name() == "struct")
			{
			theR.Advance();
			return new ZYadMapR_XMLRPC(iStrimmerU);
			}
		else if (theR.Name() == "array")
			{
			theR.Advance();
			return new ZYadSeqR_XMLRPC(iStrimmerU);
			}
		else if (theR.Name() == "base64")
			{
			theR.Advance();
			return new ZYadStreamR_XMLRPC(iStrimmerU);
			}
		else if (theR.Name() == "string")
			{
			theR.Advance();
			return new ZYadStrimR_XMLRPC(iStrimmerU);
			}
		}

	ZAny theVal;
	if (spTryRead_SimpleValue(theR, theVal))
		return new ZYadAtomR_Std(theVal);

	return null;
	}

// =================================================================================================
// MARK: - ZYadParseException_XMLRPC

ZYadParseException_XMLRPC::ZYadParseException_XMLRPC(const string& iWhat)
:	ZYadParseException_Std(iWhat)
	{}

ZYadParseException_XMLRPC::ZYadParseException_XMLRPC(const char* iWhat)
:	ZYadParseException_Std(iWhat)
	{}

// =================================================================================================
// MARK: - ZYadStreamR_XMLRPC

ZYadStreamR_XMLRPC::ZYadStreamR_XMLRPC(ZRef<ZML::StrimmerU> iStrimmerU)
:	fStrimmerU(iStrimmerU),
	fStreamR_ASCIIStrim(fStrimmerU->GetStrimR()),
	fStreamR_Base64Decode(fStreamR_ASCIIStrim)
	{}

void ZYadStreamR_XMLRPC::Finish()
	{
	fStreamR_Base64Decode.SkipAll();
	spEnd(fStrimmerU->GetStrim(), "base64");
	}

const ZStreamR& ZYadStreamR_XMLRPC::GetStreamR()
	{ return fStreamR_Base64Decode; }

// =================================================================================================
// MARK: - ZYadStrimR_XMLRPC

ZYadStrimR_XMLRPC::ZYadStrimR_XMLRPC(ZRef<ZML::StrimmerU> iStrimmerU)
:	fStrimmerU(iStrimmerU)
	{}

void ZYadStrimR_XMLRPC::Finish()
	{
	ZML::StrimU& theR = fStrimmerU->GetStrim();

	theR.Advance();
	spEnd(theR, "string");
	}

const ZStrimR& ZYadStrimR_XMLRPC::GetStrimR()
	{ return fStrimmerU->GetStrimR(); }

// =================================================================================================
// MARK: - ZYadSeqR_XMLRPC

ZYadSeqR_XMLRPC::ZYadSeqR_XMLRPC(ZRef<ZML::StrimmerU> iStrimmerU)
:	fStrimmerU(iStrimmerU)
	{}

void ZYadSeqR_XMLRPC::Imp_ReadInc(bool iIsFirst, ZRef<ZYadR>& oYadR)
	{
	ZML::StrimU& theR = fStrimmerU->GetStrim();

	sSkipText(theR);

	if (not iIsFirst)
		{
		spEnd(theR, "value");

		sSkipText(theR);
		}

	if (sTryRead_End(theR, "data"))
		{
		sSkipText(theR);

		spEnd(theR, "array");

		sSkipText(theR);

		return;
		}

	spBegin(theR, "value");

	if (!(oYadR = spMakeYadR_XMLRPC(fStrimmerU)))
		spThrowParseException("Expected a value");
	}

// =================================================================================================
// MARK: - ZYadSeqR_XMLRPC_Params

class ZYadSeqR_XMLRPC_Params : public ZYadSeqR_Std
	{
public:
	ZYadSeqR_XMLRPC_Params(ZRef<ZML::StrimmerU> iStrimmerU, bool iIsResponse);

// From ZYadSeqR_Std
	virtual void Imp_ReadInc(bool iIsFirst, ZRef<ZYadR>& oYadR);

private:
	ZRef<ZML::StrimmerU> fStrimmerU;
	bool fIsResponse;
	};

ZYadSeqR_XMLRPC_Params::ZYadSeqR_XMLRPC_Params(ZRef<ZML::StrimmerU> iStrimmerU, bool iIsResponse)
:	fStrimmerU(iStrimmerU),
	fIsResponse(iIsResponse)
	{}

void ZYadSeqR_XMLRPC_Params::Imp_ReadInc(bool iIsFirst, ZRef<ZYadR>& oYadR)
	{
	ZML::StrimU& theR = fStrimmerU->GetStrim();

	sSkipText(theR);

	if (not iIsFirst)
		{
		spEnd(theR, "value");

		sSkipText(theR);

		spEnd(theR, "param");

		sSkipText(theR);
		}

	if (sTryRead_End(theR, "params"))
		{
		sSkipText(theR);

		if (fIsResponse)
			spEnd(theR, "methodResponse");
		else
			spEnd(theR, "methodCall");

		sSkipText(theR);

		return;
		}

	spBegin(theR, "param");

	sSkipText(theR);

	spBegin(theR, "value");

	if (!(oYadR = spMakeYadR_XMLRPC(fStrimmerU)))
		spThrowParseException("Expected a value");
	}

// =================================================================================================
// MARK: - ZYadSeqR_XMLRPC_Fault

class ZYadSeqR_XMLRPC_Fault : public ZYadSeqR_Std
	{
public:
	ZYadSeqR_XMLRPC_Fault(ZRef<ZML::StrimmerU> iStrimmerU);

// From ZYadSeqR_Std
	virtual void Imp_ReadInc(bool iIsFirst, ZRef<ZYadR>& oYadR);

private:
	ZRef<ZML::StrimmerU> fStrimmerU;
	};

ZYadSeqR_XMLRPC_Fault::ZYadSeqR_XMLRPC_Fault(ZRef<ZML::StrimmerU> iStrimmerU)
:	fStrimmerU(iStrimmerU)
	{}

void ZYadSeqR_XMLRPC_Fault::Imp_ReadInc(bool iIsFirst, ZRef<ZYadR>& oYadR)
	{
	ZML::StrimU& theR = fStrimmerU->GetStrim();

	sSkipText(theR);

	if (not iIsFirst)
		{
		spEnd(theR, "value");

		sSkipText(theR);

		spEnd(theR, "fault");

		sSkipText(theR);

		spEnd(theR, "methodResponse");

		sSkipText(theR);

		return;
		}

	spBegin(theR, "value");

	if (!(oYadR = spMakeYadR_XMLRPC(fStrimmerU)))
		spThrowParseException("Expected a value");
	}

// =================================================================================================
// MARK: - ZYadMapR_XMLRPC

ZYadMapR_XMLRPC::ZYadMapR_XMLRPC(ZRef<ZML::StrimmerU> iStrimmerU)
:	fStrimmerU(iStrimmerU)
	{}

void ZYadMapR_XMLRPC::Imp_ReadInc(bool iIsFirst, std::string& oName, ZRef<ZYadR>& oYadR)
	{
	ZML::StrimU& theR = fStrimmerU->GetStrim();

	sSkipText(theR);

	if (not iIsFirst)
		{
		spEnd(theR, "value");

		sSkipText(theR);

		spEnd(theR, "member");

		sSkipText(theR);
		}

	if (sTryRead_End(theR, "struct"))
		{
		sSkipText(theR);
		return;
		}

	spBegin(theR, "member");

	sSkipText(theR);

	spBegin(theR, "name");

	oName = theR.ReadAll8();

	spEnd(theR, "name");

	sSkipText(theR);

	spBegin(theR, "value");

	if (!(oYadR = spMakeYadR_XMLRPC(fStrimmerU)))
		spThrowParseException("Expected value");
	}

// =================================================================================================
// MARK: - Static writing functions

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

static void spToStrim_Seq(const ZML::StrimW& s, ZRef<ZYadSeqR> iYadSeqR)
	{
	s.Begin("array");
		s.Begin("data");
			while (ZRef<ZYadR> theChild = iYadSeqR->ReadInc())
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

static void spToStrim_Any(const ZML::StrimW& s, const ZAny& iVal)
	{
	int64 asInt64;
	double asDouble;

	if (false)
		{}
	else if (iVal.Type() == typeid(void))
		{
		s.Empty("nil");
		}
	else if (const bool* theValue = iVal.PGet<bool>())
		{
		s.Begin("boolean");
			if (*theValue)
				s << "1";
			else
				s << "0";
		s.End("boolean");
		}
	else if (sQCoerceInt(iVal, asInt64))
		{
		s.Begin("i4");
			s.Writef("%d", int(asInt64));
		s.End("i4");
		}
	else if (sQCoerceRat(iVal, asDouble))
		{
		s.Begin("double");
			ZUtil_Strim::sWriteExact(s, asDouble);
		s.End("double");
		}
	else if (const ZTime* theValue = iVal.PGet<ZTime>())
		{
		s.Begin("dateTime.iso8601");
			s << ZUtil_Time::sAsString_ISO8601(*theValue, true);
		s.End("dateTime.iso8601");
		}
	else
		{
		s.Raw() << "<nil/> <!--!! Unhandled: */" << iVal.Type().name() << " !!-->";
		}
	}

static void spToStrim(const ZML::StrimW& s, ZRef<ZYadR> iYadR)
	{
	if (not iYadR)
		{
		return;
		}
	else if (ZRef<ZYadMapR> theYadMapR = iYadR.DynamicCast<ZYadMapR>())
		{
		spToStrim_Map(s, theYadMapR);
		}
	else if (ZRef<ZYadSeqR> theYadSeqR = iYadR.DynamicCast<ZYadSeqR>())
		{
		spToStrim_Seq(s, theYadSeqR);
		}
	else if (ZRef<ZYadStreamR> theYadStreamR = iYadR.DynamicCast<ZYadStreamR>())
		{
		spToStrim_Stream(s, theYadStreamR->GetStreamR());
		}
	else if (ZRef<ZYadStrimR> theYadStrimR = iYadR.DynamicCast<ZYadStrimR>())
		{
		spToStrim_Strim(s, theYadStrimR->GetStrimR());
		}
	else if (ZRef<ZYadAtomR> theYadAtomR = iYadR.DynamicCast<ZYadAtomR>())
		{
		spToStrim_Any(s, theYadAtomR->AsAny());
		}
	else
		{
		s.Raw() << "<nil/> <!--!! Unhandled Yad !!-->";
		}
	}

// =================================================================================================
// MARK: - ZYad_XMLRPC

namespace ZYad_XMLRPC {

bool sFromStrimmer(ZRef<ZML::StrimmerU> iStrimmerU, Request_t& oRequest)
	{
	ZML::StrimU& theR = iStrimmerU->GetStrim();

	spSkipCruft(theR);

	if (!sTryRead_Begin(theR, "methodCall"))
		return false;

	spRead_TagWrappedText(theR, "methodName", oRequest.fMethodName);

	sSkipText(theR);

	if (sTryRead_Begin(theR, "params"))
		{
		oRequest.fParams = new ZYadSeqR_XMLRPC_Params(iStrimmerU, false);
		}
	else
		{
		oRequest.fParams.Clear();

		spEnd(theR, "methodCall");
		}

	return true;
	}

bool sFromStrimmer(ZRef<ZML::StrimmerU> iStrimmerU, Response_t& oResponse)
	{
	ZML::StrimU& theR = iStrimmerU->GetStrim();

	spSkipCruft(theR);

	if (!sTryRead_Begin(theR, "methodResponse"))
		return false;

	sSkipText(theR);

	if (sTryRead_Begin(theR, "params"))
		{
		oResponse.fResult = new ZYadSeqR_XMLRPC_Params(iStrimmerU, true);
		}
	else if (sTryRead_Begin(theR, "fault"))
		{
		oResponse.fFault = new ZYadSeqR_XMLRPC_Fault(iStrimmerU);
		}
	else
		{
		spThrowParseException("Expected 'fault' or 'params'");
		}

	return true;
	}

void sToStrim(const Request_t& iRequest, const ZML::StrimW& s)
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

void sToStrim(const Response_t& iResponse, const ZML::StrimW& s)
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
} // namespace ZooLib
