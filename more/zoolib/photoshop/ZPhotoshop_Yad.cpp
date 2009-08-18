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

#include "zoolib/photoshop/ZPhotoshop_Yad.h"

#include "zoolib/ZYad_Any.h"
#include "zoolib/ZYad_Std.h"

NAMESPACE_ZOOLIB_BEGIN

namespace ZPhotoshop {

// =================================================================================================
#pragma mark -
#pragma mark * YadR

typedef ZYadR_Val_T<Val> YadR;

// =================================================================================================
#pragma mark -
#pragma mark * Helpers

static bool spAppendIfASCII(char iChar, string& ioString)
	{
	if (iChar < 32 || iChar > 126)
		return false;
	ioString += iChar;
	return true;
	}

static ZVal_Any spIntAsVal(int32 iInt)
	{
	string theString;
	if (sFromRuntimeTypeID(iInt, theString))
		return theString;

	if (!spAppendIfASCII(iInt >> 24, theString))
		return iInt;

	if (!spAppendIfASCII(iInt >> 16, theString))
		return iInt;

	if (!spAppendIfASCII(iInt >> 8, theString))
		return iInt;

	if (!spAppendIfASCII(iInt, theString))
		return iInt;

	return theString;
	}

// =================================================================================================
#pragma mark -
#pragma mark * YadListRPos

typedef ZYadListRPos_Val_T<List> YadListRPos;

// =================================================================================================
#pragma mark -
#pragma mark * YadMapRPos

typedef ZYadMapRPos_Val_T<Map> YadMapRPos;

// =================================================================================================
#pragma mark -
#pragma mark * ZPhotoshop::MakeYadR

ZRef<ZYadR> sMakeYadR(const Val& iVal)
	{
	Map asMap;
	if (iVal.QGetMap(asMap))
		return new YadMapRPos(asMap);

	List asList;
	if (iVal.QGetList(asList))
		return new YadListRPos(asList);

	UnitFloat asUnitFloat;
	if (iVal.QGetUnitFloat(asUnitFloat))
		{
		ZMap_Any theMap;
		theMap.Set("!Type", string("UnitFloat"));
		theMap.Set("UnitID", spIntAsVal(asUnitFloat.fUnitID));
		theMap.Set("Value", asUnitFloat.fValue);
		return sMakeYadR(theMap);
		}

	Enumerated asEnumerated;
	if (iVal.QGetEnumerated(asEnumerated))
		{
		ZMap_Any theMap;
		theMap.Set("!Type", string("Enumerated"));
		theMap.Set("UnitID", spIntAsVal(asEnumerated.fEnumType));
		theMap.Set("Value", spIntAsVal(asEnumerated.fValue));
		return sMakeYadR(theMap);
		}

	FileRef asFileRef;
	if (iVal.QGetFileRef(asFileRef))
		{
		ZMap_Any theMap;
		theMap.Set("!Type", string("FileRef"));
		theMap.Set("PathPOSIX", asFileRef.AsPathPOSIX());
		theMap.Set("PathNative", asFileRef.AsPathNative());
		return sMakeYadR(theMap);
		}

	Spec asSpec;
	if (iVal.QGetSpec(asSpec))
		{
		ZMap_Any theMap;
		theMap.Set("!Type", string("Spec"));
		return sMakeYadR(theMap);
		}

	return new ZYadPrimR_Std(iVal.AsVal_Any().AsAny());
	}

ZRef<ZYadListR> sMakeYadR(const List& iList)
	{ return new YadListRPos(iList); }

ZRef<ZYadMapR> sMakeYadR(const Map& iMap)
	{ return new YadMapRPos(iMap); }

} // namespace ZPhotoshop

NAMESPACE_ZOOLIB_END
