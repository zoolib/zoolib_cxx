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

ZRef<ZYadR> sMakeYadR(const ZAny& iVal)
	{
	if (const ZMap_Any* theVal = iVal.PGet_T<ZMap_Any>())
		return sMakeYadR(*theVal);
		
	if (const ZList_Any* theVal = iVal.PGet_T<ZList_Any>())
		return sMakeYadR(*theVal);

	if (const string8* theVal = iVal.PGet_T<string8>())
		return sMakeYadR(*theVal);

	if (const ZData_Any* theVal = iVal.PGet_T<ZData_Any>())
		return new ZYadStreamRPos_Any(*theVal);

	return new ZYadPrimR_Any(iVal);
	}

ZRef<ZYadStreamR> sMakeYadR(const ZData_Any& iData)
	{ return new ZYadStreamRPos_Any(iData); }

ZRef<ZYadListR> sMakeYadR(const ZList_Any& iList)
	{ return new ZYadListRPos_Any(iList); }

ZRef<ZYadMapR> sMakeYadR(const ZMap_Any& iMap)
	{ return new ZYadMapRPos_Any(iMap); }

NAMESPACE_ZOOLIB_END
