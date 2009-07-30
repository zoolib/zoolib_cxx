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

#include "zoolib/ZYad_Any.h"
#include "zoolib/ZYad_Std.h"

NAMESPACE_ZOOLIB_BEGIN

using std::string;

// =================================================================================================
#pragma mark -
#pragma mark * sMakeYadR

ZRef<ZYadR> sMakeYadR(const ZVal_Any& iVal)
	{
	ZValMap_Any asMap;
	if (iVal.QGet_T<ZValMap_Any>(asMap))
		return new ZYadMapRPos_Any(asMap);
		
	ZValList_Any asList;
	if (iVal.QGet_T<ZValList_Any>(asList))
		return new ZYadListRPos_Any(asList);
		
	string8 asString;
	if (iVal.QGet_T<string8>(asString))
		return new ZYadStrimU_String(asString);

	ZValData_Any asData;
	if (iVal.QGet_T<ZValData_Any>(asData))
		return new ZYadStreamRPos_Any(asData);

	return new ZYadPrimR_Std(iVal);
	}

ZRef<ZYadStreamR> sMakeYadR(const ZValData_Any& iData)
	{ return new ZYadStreamRPos_Any(iData); }

ZRef<ZYadListR> sMakeYadR(const ZValList_Any& iList)
	{ return new ZYadListRPos_Any(iList); }

ZRef<ZYadMapR> sMakeYadR(const ZValMap_Any& iMap)
	{ return new ZYadMapRPos_Any(iMap); }

NAMESPACE_ZOOLIB_END
