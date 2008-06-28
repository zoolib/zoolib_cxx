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

#include "zoolib/ZUtil_Strim_XMLRPC.h"

#include "zoolib/ZMemoryBlock.h"
#include "zoolib/ZStream_AsciiStrim.h"
#include "zoolib/ZStream_Base64.h"
#include "zoolib/ZStrimW_ML.h"
#include "zoolib/ZString.h"
#include "zoolib/ZUtil_Strim.h"
#include "zoolib/ZUtil_Time.h"

using std::string;
using std::vector;

// =================================================================================================
#pragma mark -
#pragma mark * Helpers

namespace ZANONYMOUS {

void sSkipCruft(ZML::Reader& r)
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

void sBegin(ZML::Reader& r, const string& iTagName)
	{
	sSkipText(r);

	if (!sTryRead_Begin(r, iTagName))
		throw ZUtil_Strim_XMLRPC::ParseException("Expected begin tag '" + iTagName + "'");
	}

void sEnd(ZML::Reader& r, const string& iTagName)
	{
	sSkipText(r);

	if (!sTryRead_End(r, iTagName))
		throw ZUtil_Strim_XMLRPC::ParseException("Expected end tag '" + iTagName + "'");
	}

void sRead_TagWrappedText(ZML::Reader& r, const string& iTagName, string& oText)
	{
	sBegin(r, iTagName);

		ZStrimW_String(oText).CopyAllFrom(r.Text());
	
	sEnd(r, iTagName);
	}

void sRead_BodyOfStruct(ZML::Reader& r, ZTuple& oTuple);
void sRead_BodyOfArray(ZML::Reader& r, vector<ZTValue>& oVector);

bool sTryRead_Value(ZML::Reader& r, ZTValue& oValue)
	{
	if (!sTryRead_Begin(r, "value"))
		return false;

	string textInsideValue;
	ZStrimW_String(textInsideValue).CopyAllFrom(r.Text());

	if (r.Current() != ZML::eToken_TagBegin)
		{
		oValue.SetString(textInsideValue);
		}
	else
		{
		// We've got a tag inside value, so it's a specified type rather than an implied string.
		const string tagName = r.Name();
		r.Advance();

		if (tagName == "i4" || tagName == "int")
			{
			int64 theInteger;
			if (!ZUtil_Strim::sTryRead_SignedDecimalInteger(ZStrimU_Unreader(r.Text()), theInteger))
				throw ZUtil_Strim_XMLRPC::ParseException("Expected valid integer");

			oValue.SetInt32(theInteger);
			}
		else if (tagName == "boolean")
			{
			int64 theInteger;
			if (!ZUtil_Strim::sTryRead_SignedDecimalInteger(ZStrimU_Unreader(r.Text()), theInteger))
				throw ZUtil_Strim_XMLRPC::ParseException("Expected integer (for boolean)");

			oValue.SetBool(theInteger);
			}
		else if (tagName == "string")
			{
			string theString;
			ZStrimW_String(theString).CopyAllFrom(r.Text());

			oValue.SetString(theString);
			}
		else if (tagName == "double")
			{
			double theDouble;
			if (!ZUtil_Strim::sTryRead_SignedDouble(ZStrimU_Unreader(r.Text()), theDouble))
				throw ZUtil_Strim_XMLRPC::ParseException("Expected valid double");

			oValue.SetDouble(theDouble);
			}
		else if (tagName == "dateTime.iso8601")
			{
			string theString;
			ZStrimW_String(theString).CopyAllFrom(r.Text());

			oValue.SetTime(ZUtil_Time::sFromString_ISO8601(theString));
			}
		else if (tagName == "base64")
			{
			ZMemoryBlock theMB;
			ZStreamR_Base64Decode(ZStreamR_ASCIIStrim(r.Text())).CopyAllTo(ZStreamRWPos_MemoryBlock(theMB));

			oValue.SetRaw(theMB);
			}
		else if (tagName == "struct")
			{
			sRead_BodyOfStruct(r, oValue.SetMutableTuple());
			}
		else if (tagName == "array")
			{
			sRead_BodyOfArray(r, oValue.SetMutableVector());
			}
		sEnd(r, tagName);
		}

	sEnd(r, "value");

	return true;
	}

void sRead_Value(ZML::Reader& r, ZTValue& oValue)
	{
	sSkipText(r);

	if (!sTryRead_Value(r, oValue))
		throw ZUtil_Strim_XMLRPC::ParseException("Expected value");
	}

void sRead_BodyOfStruct(ZML::Reader& r, ZTuple& oTuple)
	{
	ZRef<ZTupleRep> theRep = new ZTupleRep;
	ZTuple::PropList& theProperties = theRep->fProperties;

	// We've consumed a <struct> tag, and are pointing at any text following it.
	for (;;)
		{
		sSkipText(r);

		if (!sTryRead_Begin(r, "member"))
			break;
		
			string theName;
			sRead_TagWrappedText(r, "name", theName);

			ZTValue theValue;
			sRead_Value(r, theValue);

		sEnd(r, "member");

		theProperties.push_back(ZTuple::NameTV(ZTName(theName), theValue));
		}

	oTuple = ZTuple(theRep);
	}

void sRead_BodyOfArray(ZML::Reader& r, vector<ZTValue>& oVector)
	{
	sBegin(r, "data");

	for (;;)
		{
		sSkipText(r);

		ZTValue theValue;
		if (!sTryRead_Value(r, theValue))
			break;

		oVector.push_back(theValue);
		}

	sEnd(r, "data");
	}

void sWrite_Value(ZStrimW_ML& s, const ZTValue& iTValue)
	{
	s.Begin("value");
	switch (iTValue.TypeOf())
		{
		case eZType_Tuple:
			{
			s.Begin("struct");
				const ZTuple& asTuple = iTValue.GetTuple();
				for (ZTuple::const_iterator i = asTuple.begin(); i != asTuple.end(); ++i)
					{
					s.Begin("member");

						s.Begin("name");
							s << asTuple.NameOf(i).AsString();
						s.End("name");

						sWrite_Value(s, asTuple.GetValue(i));
					s.End("member");
					}
			s.End("struct");
			break;
			}
		case eZType_Vector:
			{
			s.Begin("array");
				s.Begin("data");				
					const vector<ZTValue>& asVector = iTValue.GetVector();
					for (vector<ZTValue>::const_iterator i = asVector.begin(); i != asVector.end(); ++i)
						sWrite_Value(s, *i);
				s.End("data");
			s.End("array");
			break;
			}
		case eZType_String:
			{
			s.Begin("string");
				s << iTValue.GetString();
			s.End("string");
			break;
			}
		case eZType_Int32:
			{
			s.Begin("int");
				s.Writef("%d", iTValue.GetInt32());
			s.End("int");
			break;
			}
		case eZType_Double:
			{
			s.Begin("double");
				s.Writef("%.17g", iTValue.GetDouble());
			s.End("double");
			break;
			}
		case eZType_Bool:
			{
			s.Begin("boolean");
				if (iTValue.GetBool())
					s << "1";
				else
					s << "0";
			s.End("boolean");
			break;
			}
		case eZType_Raw:
			{
			s.Begin("base64");
				const ZMemoryBlock& theMB = iTValue.GetRaw();
				ZStreamW_Base64Encode(ZStreamW_ASCIIStrim(s)).Write(theMB.GetData(), theMB.GetSize());
			s.End("base64");
			}
		case eZType_Time:
			{
			s.Begin("dateTime.iso8601");
				s << ZUtil_Time::sAsString_ISO8601(iTValue.GetTime(), true);
			s.End("dateTime.iso8601");
			break;
			}
		}
	s.End("value");
	}

} // anonymous namespace

// =================================================================================================
#pragma mark -
#pragma mark * ZUtil_Strim_XMLRPC::ParseException

ZUtil_Strim_XMLRPC::ParseException::ParseException(const string& iWhat)
:	runtime_error(iWhat)
	{}

ZUtil_Strim_XMLRPC::ParseException::ParseException(const char* iWhat)
:	runtime_error(iWhat)
	{}

// =================================================================================================
#pragma mark -
#pragma mark * ZUtil_Strim_XMLRPC, request

static void sRequestToStrimW_ML(ZStrimW_ML& s,
	const string& iMethodName, const vector<ZTValue>& iParams)
	{
	s.Begin("methodCall");

		s.Begin("methodName");
			s << iMethodName;
		s.End("methodName");

		if (!iParams.empty())
			{
			s.Begin("params");
			for (vector<ZTValue>::const_iterator i = iParams.begin(); i != iParams.end(); ++i)
				{
				s.Begin("param");
					sWrite_Value(s, *i);
				s.End("param");
				}
			s.End("params");
			}

	s.End("methodCall");
	}

void ZUtil_Strim_XMLRPC::sRequestToStrim(const ZStrimW& iStrimW,
	const string& iMethodName, const vector<ZTValue>& iParams)
	{
	ZStrimW_ML theStrimW_ML(iStrimW);
	sRequestToStrimW_ML(theStrimW_ML, iMethodName, iParams);
	}

bool ZUtil_Strim_XMLRPC::sRequestFromStrim(const ZStrimU& iStrimU,
	string& oMethodName, vector<ZTValue>& oParams)
	{
	ZML::Reader r(iStrimU);
	return sRequestFromML(r, oMethodName, oParams);
	}

bool ZUtil_Strim_XMLRPC::sRequestFromML(ZML::Reader& r,
	string& oMethodName, vector<ZTValue>& oParams)
	{
	sSkipCruft(r);

	if (!sTryRead_Begin(r, "methodCall"))
		return false;

	sRead_TagWrappedText(r, "methodName", oMethodName);

	sSkipText(r);

	if (sTryRead_Begin(r, "params"))
		{
		for (;;)
			{
			sSkipText(r);
			if (!sTryRead_Begin(r, "param"))
				break;
			ZTValue theValue;
			sRead_Value(r, theValue);
			oParams.push_back(theValue);
			}
		sEnd(r, "params");
		}

	sEnd(r, "methodCall");

	return true;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZUtil_Strim_XMLRPC, response

static void sResponseToStrimW_ML(ZStrimW_ML& s,
	const ZUtil_Strim_XMLRPC::Result_t& iResult)
	{
	s.Begin("methodResponse");
		if (iResult.fIsFault)
			{
			ZTuple t;
			t.SetInt32("faultCode", iResult.fFaultCode);
			t.SetString("faultString", iResult.fFaultString);
			s.Begin("fault");
				sWrite_Value(s, t);
			s.End("fault");
			}
		else
			{
			s.Begin("params");
				s.Begin("param");
					sWrite_Value(s, iResult.fValue);
				s.End("param");
			s.End("params");
			}
		
	s.End("methodResponse");
	}

void ZUtil_Strim_XMLRPC::sResponseToStrim(const ZStrimW& iStrimW, const Result_t& iResult)
	{
	ZStrimW_ML theStrimW_ML(iStrimW);
	sResponseToStrimW_ML(theStrimW_ML, iResult);
	}

bool ZUtil_Strim_XMLRPC::sResponseFromStrim(const ZStrimU& iStrimU, Result_t& oResult)
	{
	ZML::Reader r(iStrimU);
	return sResponseFromML(r, oResult);
	}

bool ZUtil_Strim_XMLRPC::sResponseFromML(ZML::Reader& r, Result_t& oResult)
	{
	sSkipCruft(r);

	if (!sTryRead_Begin(r, "methodResponse"))
		return false;

	sSkipText(r);

	if (sTryRead_Begin(r, "fault"))
		{
		oResult.fIsFault = true;

		ZTValue theValue;
		sRead_Value(r, theValue);
		oResult.fFaultCode = theValue.GetTuple().GetInt32("faultCode");
		oResult.fFaultString = theValue.GetTuple().GetInt32("faultString");
		sEnd(r, "fault");
		}
	else if (sTryRead_Begin(r, "params"))
		{
		oResult.fIsFault = false;

		sBegin(r, "param");
		sSkipText(r);
		sRead_Value(r, oResult.fValue);
		sSkipText(r);
		sEnd(r, "param");
		sEnd(r, "params");
		}
	else
		{
		throw ZUtil_Strim_XMLRPC::ParseException("Expected 'fault' or 'params'");
		}

	sEnd(r, "methodResponse");

	return true;
	}
