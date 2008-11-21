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
	{ ZYadUtil_ZooLibStrim::sWrite_PropName(iStrimW, iPropName.AsString()); }

bool ZUtil_Strim_Tuple::sRead_Identifier(
	const ZStrimU& iStrimU, string* oStringLC, string* oStringExact)
	{ return ZYadUtil_ZooLibStrim::sRead_Identifier(iStrimU, oStringLC, oStringExact); }

// =================================================================================================
#pragma mark -
#pragma mark * ZTValue

void ZUtil_Strim_Tuple::sToStrim(const ZStrimW& s, const ZTValue& iTV)
	{
	ZRef<ZYadR> theYadR = ZYadUtil_ZooLib::sMakeYadR(iTV);
	ZYadUtil_ZooLibStrim::sToStrim(s, theYadR, 0, ZYadOptions());
	}

void ZUtil_Strim_Tuple::sToStrim(const ZStrimW& s, const ZTValue& iTV,
	size_t iInitialIndent, const ZYadOptions& iOptions)
	{
	ZYadUtil_ZooLibStrim::sToStrim(
		s, ZYadUtil_ZooLib::sMakeYadR(iTV), iInitialIndent, iOptions);
	}

string ZUtil_Strim_Tuple::sAsString(const ZTValue& iTV)
	{
	string theString;
	sToStrim(ZStrimW_String(theString), iTV);
	return theString;
	}

bool ZUtil_Strim_Tuple::sFromStrim(const ZStrimU& iStrimU, ZTValue& oTV)
	{
	if (ZRef<ZYadR> theYadR = ZYadUtil_ZooLibStrim::sMakeYadR(iStrimU))
		{
		oTV = ZYadUtil_ZooLib::sFromYadR(theYadR);
		return true;
		}
	return false;
	}

bool ZUtil_Strim_Tuple::sFromString(const string& iString, ZTValue& oTV)
	{ return sFromStrim(ZStrimU_String(iString), oTV); }

// =================================================================================================
#pragma mark -
#pragma mark * vector<ZTValue>

void ZUtil_Strim_Tuple::sToStrim(const ZStrimW& s, const vector<ZTValue>& iVector)
	{
	ZRef<ZYadListR> theYadListR = new ZYadListRPos_Vector(iVector);
	ZYadUtil_ZooLibStrim::sToStrim(s, theYadListR, 0, ZYadOptions());
	}

void ZUtil_Strim_Tuple::sToStrim(const ZStrimW& s, const vector<ZTValue>& iVector,
	size_t iInitialIndent, const ZYadOptions& iOptions)
	{
	ZRef<ZYadListR> theYadListR = new ZYadListRPos_Vector(iVector);
	ZYadUtil_ZooLibStrim::sToStrim(s, theYadListR, iInitialIndent, iOptions);
	}

string ZUtil_Strim_Tuple::sAsString(const vector<ZTValue>& iVector)
	{
	string theString;
	sToStrim(ZStrimW_String(theString), iVector);
	return theString;
	}

bool ZUtil_Strim_Tuple::sFromStrim(const ZStrimU& iStrimU, vector<ZTValue>& oVector)
	{
	ZTValue theTV;
	if (!sFromStrim(iStrimU, theTV))
		return false;

	if (theTV.TypeOf() != eZType_Vector)
		return false;

	theTV.GetVector(oVector);
	return true;
	}

bool ZUtil_Strim_Tuple::sFromString(const string& iString, vector<ZTValue>& oVector)
	{ return sFromStrim(ZStrimU_String(iString), oVector); }

// =================================================================================================
#pragma mark -
#pragma mark * ZTuple

void ZUtil_Strim_Tuple::sToStrim(const ZStrimW& s, const ZTuple& iTuple)
	{
	ZRef<ZYadMapR> theYadMapR = new ZYadListMapRPos_Tuple(iTuple);
	ZYadUtil_ZooLibStrim::sToStrim(s, theYadMapR, 0, ZYadOptions());
	}

void ZUtil_Strim_Tuple::sToStrim(const ZStrimW& s, const ZTuple& iTuple,
	size_t iInitialIndent, const ZYadOptions& iOptions)
	{
	ZRef<ZYadMapR> theYadMapR = new ZYadListMapRPos_Tuple(iTuple);
	ZYadUtil_ZooLibStrim::sToStrim(s, theYadMapR, iInitialIndent, iOptions);
	}

string ZUtil_Strim_Tuple::sAsString(const ZTuple& iTuple)
	{
	string theString;
	sToStrim(ZStrimW_String(theString), iTuple);
	return theString;
	}

string ZUtil_Strim_Tuple::sAsString(const ZTuple& iTuple,
	size_t iInitialIndent, const Options& iOptions)
	{
	string theString;
	sToStrim(ZStrimW_String(theString), iTuple, iInitialIndent, iOptions);
	return theString;
	}

bool ZUtil_Strim_Tuple::sFromStrim(const ZStrimU& iStrimU, ZTuple& oTuple)
	{
	ZTValue theTV;
	if (!sFromStrim(iStrimU, theTV))
		return false;

	if (theTV.TypeOf() != eZType_Tuple)
		return false;

	oTuple = theTV.GetTuple();
	return true;
	}

bool ZUtil_Strim_Tuple::sFromString(const string& iString, ZTuple& oTuple)
	{ return sFromStrim(ZStrimU_String(iString), oTuple); }

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
