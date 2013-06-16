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

#include "zoolib/ZStrimmerFromStrim.h"
#include "zoolib/ZUtil_Strim_Tuple.h"
#include "zoolib/ZYad_Any.h"
#include "zoolib/ZYad_ZooLibStrim.h"

namespace ZooLib {

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
#pragma mark * ZUtil_Strim_Tuple::sFromStrim

bool ZUtil_Strim_Tuple::sFromStrim(const ZStrimU& iStrimU, ZTValue& oTV)
	{
	ZStrimmerFromStrimU theSFS(iStrimU);
	if (ZRef<ZYadR> theYadR = ZYad_ZooLibStrim::sYadR(theSFS))
		{
		oTV = sFromYadR(ZTValue(), theYadR);
		return true;
		}
	return false;
	}

// =================================================================================================
#pragma mark -
#pragma mark * operator<< overloads

const ZStrimW& operator<<(const ZStrimW& s, const ZTValue& iTV)
	{
	ZRef<ZYadR> theYadR = sYadR(iTV);
	ZYad_ZooLibStrim::sToStrim(0, ZYadOptions(), theYadR, s);
	return s;
	}

const ZStrimW& operator<<(const ZStrimW& s, const ZUtil_Strim_Tuple::Format& iFormat)
	{
	ZRef<ZYadR> theYadR = sYadR(iFormat.fTValue);
	ZYad_ZooLibStrim::sToStrim(0, ZYadOptions(), theYadR, s);
	return s;
	}

} // namespace ZooLib
