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

#include "zoolib/ZYad_AppleEvent.h"

NAMESPACE_ZOOLIB_BEGIN

using std::string;

// =================================================================================================
#pragma mark -
#pragma mark * ZYadListRPos_AppleEvent

ZYadListRPos_AppleEvent::ZYadListRPos_AppleEvent(const ZValList_AppleEvent& iList)
:	ZYadR_AppleEvent(static_cast<const AEDesc&>(iList))
,	YadListBase_t(iList)
	{}

ZYadListRPos_AppleEvent::ZYadListRPos_AppleEvent(
	const ZValList_AppleEvent& iList, uint64 iPosition)
:	ZYadR_AppleEvent(static_cast<const AEDesc&>(iList))
,	YadListBase_t(iList, iPosition)
	{}

// =================================================================================================
#pragma mark -
#pragma mark * ZYadMapRPos_AppleEvent

ZYadMapRPos_AppleEvent::ZYadMapRPos_AppleEvent(const ZValMap_AppleEvent& iMap)
:	ZYadR_AppleEvent(static_cast<const AEDesc&>(iMap))
,	YadMapBase_t(iMap, iMap.begin())
	{}

ZYadMapRPos_AppleEvent::ZYadMapRPos_AppleEvent(const ZValMap_AppleEvent& iMap, const Index_t& iIndex)
:	ZYadR_AppleEvent(static_cast<const AEDesc&>(iMap))
,	YadMapBase_t(iMap, iIndex)
	{}

ZRef<ZYadR> ZYadMapRPos_AppleEvent::ReadInc(string& oName)
	{
	if (fIndex != fMap.end())
		{
		oName = fMap.NameOf(fIndex);
		return sMakeYadR(fMap.Get(fIndex++));
		}
	return ZRef<ZYadR>();
	}

void ZYadMapRPos_AppleEvent::SetPosition(const std::string& iName)
	{ fIndex = fMap.IndexOf(iName); }

// =================================================================================================
#pragma mark -
#pragma mark * sMakeYadR

ZRef<ZYadR> sMakeYadR(const ZVal_AppleEvent& iVal)
	{
	ZValMap_AppleEvent asMap;
	if (iVal.QGetMap(asMap))
		return new ZYadMapRPos_AppleEvent(asMap);
		
	ZValList_AppleEvent asList;
	if (iVal.QGetList(asList))
		return new ZYadListRPos_AppleEvent(asList);
		
//	ZValData_AppleEvent asData;
//	if (iVal.QGetData(asData))
//		return new ZYadStreamRPos_AppleEvent(asData);

//	ZRef<CFStringRef> asCFString;
//	if (iVal.QGetCFString(asCFString))
//		return new ZYadStrimR_AppleEvent(asCFString);

	return new ZYadR_AppleEvent(iVal);
	}

NAMESPACE_ZOOLIB_END
