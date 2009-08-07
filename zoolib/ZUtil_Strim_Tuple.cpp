/* -------------------------------------------------------------------------------------------------
Copyright (c) 2003 Andrew Green and Learning in Motion, Inc.
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

#include "zoolib/ZUtil_Strim_Tuple.h"

#include "zoolib/ZYad_ZooLib.h"
#include "zoolib/ZYad_ZooLibStrim.h"

NAMESPACE_ZOOLIB_BEGIN

using std::string;
using std::vector;

// =================================================================================================
#pragma mark -
#pragma mark * Format

ZUtil_Strim_Tuple::Format::Format(
	const ZTValue& iTV, int iInitialIndent, const ZYadOptions& iOptions)
:	fTValue(iTV),
	fInitialIndent(iInitialIndent),
	fOptions(iOptions)
	{}

// =================================================================================================
#pragma mark -
#pragma mark * ZUtil_Strim_Tuple, writing and parsing pieces

void ZUtil_Strim_Tuple::sWrite_PropName(const ZStrimW& iStrimW, const ZTName& iPropName)
	{ ZYad_ZooLibStrim::sWrite_PropName(iStrimW, iPropName.AsString()); }

bool ZUtil_Strim_Tuple::sRead_Identifier(
	const ZStrimU& iStrimU, string* oStringLC, string* oStringExact)
	{ return ZYad_ZooLibStrim::sRead_Identifier(iStrimU, oStringLC, oStringExact); }

// =================================================================================================
#pragma mark -
#pragma mark * ZTValue

void ZUtil_Strim_Tuple::sToStrim(const ZStrimW& s, const ZTValue& iTV)
	{
	ZRef<ZYadR> theYadR = sMakeYadR(iTV);
	ZYad_ZooLibStrim::sToStrim(s, theYadR, 0, ZYadOptions());
	}

void ZUtil_Strim_Tuple::sToStrim(const ZStrimW& s, const ZTValue& iTV,
	size_t iInitialIndent, const ZYadOptions& iOptions)
	{
	ZYad_ZooLibStrim::sToStrim(s, sMakeYadR(iTV), iInitialIndent, iOptions);
	}

string ZUtil_Strim_Tuple::sAsString(const ZTValue& iTV)
	{
	string theString;
	sToStrim(ZStrimW_String(theString), iTV);
	return theString;
	}

bool ZUtil_Strim_Tuple::sFromStrim(const ZStrimU& iStrimU, ZTValue& oTV)
	{
	ZRef<ZStrimmerU_Strim> theStrimmerU = new ZStrimmerU_Strim(iStrimU);
	if (ZRef<ZYadR> theYadR = ZYad_ZooLibStrim::sMakeYadR(theStrimmerU))
		{
		oTV = sFromYadR(theYadR, ZTValue());
		return true;
		}
	return false;
	}

bool ZUtil_Strim_Tuple::sFromString(const string& iString, ZTValue& oTV)
	{ return sFromStrim(ZStrimU_String(iString), oTV); }

// =================================================================================================
#pragma mark -
#pragma mark * operator<< overloads

const ZStrimW& operator<<(const ZStrimW& s, const ZTValue& iTV)
	{
	ZUtil_Strim_Tuple::sToStrim(s, iTV);
	return s;
	}

const ZStrimW& operator<<(const ZStrimW& s, const ZUtil_Strim_Tuple::Format& iFormat)
	{
	ZUtil_Strim_Tuple::sToStrim(s, iFormat.fTValue, iFormat.fInitialIndent, iFormat.fOptions);
	return s;
	}

NAMESPACE_ZOOLIB_END
