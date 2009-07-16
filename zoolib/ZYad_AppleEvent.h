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

#ifndef __ZYad_AppleEvent__
#define __ZYad_AppleEvent__ 1
#include "zconfig.h"

#include "zoolib/ZVal_AppleEvent.h"
#include "zoolib/ZYad_Val_T.h"

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * ZYadR_AppleEvent

typedef ZYadR_Val_T<ZVal_AppleEvent> ZYadR_AppleEvent;

// =================================================================================================
#pragma mark -
#pragma mark * ZYadListRPos_AppleEvent

class ZYadListRPos_AppleEvent
:	public ZYadR_AppleEvent
,	public ZYadListRPos_Val_T<ZYadListRPos_AppleEvent, ZValList_AppleEvent>
	{
public:
	ZYadListRPos_AppleEvent(const ZValList_AppleEvent& iList);
	ZYadListRPos_AppleEvent(const ZValList_AppleEvent& iList, uint64 iPosition);
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYadMapRPos_AppleEvent

class ZYadMapRPos_AppleEvent
:	public ZYadR_AppleEvent
,	public ZYadMapRPos_Val_T<ZYadMapRPos_AppleEvent, ZValMap_AppleEvent>
	{
public:
	ZYadMapRPos_AppleEvent(const ZValMap_AppleEvent& iMap);
	ZYadMapRPos_AppleEvent(const ZValMap_AppleEvent& iMap, const Index_t& iPosition);

// From ZYadMapR via ZYadMapRPos_Val_T
	virtual ZRef<ZYadR> ReadInc(std::string& oName);

// From ZYadMapRPos via ZYadMapRPos_Val_T
	virtual void SetPosition(const std::string& iName);
	};

// =================================================================================================
#pragma mark -
#pragma mark * sMakeYadR

ZRef<ZYadR> sMakeYadR(const ZVal_AppleEvent& iVal);

NAMESPACE_ZOOLIB_END

#endif // __ZYad_AppleEvent__
