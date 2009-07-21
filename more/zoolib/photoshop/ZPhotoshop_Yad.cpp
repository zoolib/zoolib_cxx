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

#include "zoolib/ZFunctionChain.h"
#include "zoolib/ZYad_ZooLib.h"

NAMESPACE_ZOOLIB_BEGIN

namespace ZPhotoshop {

// =================================================================================================
#pragma mark -
#pragma mark * Helpers

bool sAppendIfASCII(char iChar, string& ioString)
	{
	if (iChar < 32 || iChar > 126)
		return false;
	ioString += iChar;
	return true;
	}

static ZVal_ZooLib sIntAsVal(int32 iInt)
	{
	string theString;
	if (sFromRuntimeTypeID(iInt, theString))
		return theString;

	if (!sAppendIfASCII(iInt >> 24, theString))
		return iInt;

	if (!sAppendIfASCII(iInt >> 16, theString))
		return iInt;

	if (!sAppendIfASCII(iInt >> 8, theString))
		return iInt;

	if (!sAppendIfASCII(iInt, theString))
		return iInt;

	return theString;
	}

static bool sAsVal_ZooLib(const Val& iVal, ZVal_ZooLib& oVal)
	{
	Data asData;
	if (iVal.QGetData(asData))
		{
		oVal = asData;
		return true;
		}

	string8 asString;
	if (iVal.QGetString(asString))
		{
		oVal = asString;
		return true;
		}

	int32 asInt32;
	if (iVal.QGetInt32(asInt32))
		{
		oVal = asInt32;
		return true;
		}

	double asDouble;
	if (iVal.QGetDouble(asDouble))
		{
		oVal = asDouble;
		return true;
		}

	bool asBool;
	if (iVal.QGetBool(asBool))
		{
		oVal = asBool;
		return true;
		}

	UnitFloat asUnitFloat;
	if (iVal.QGetUnitFloat(asUnitFloat))
		{
		ZValMap_ZooLib theMap;
		theMap.Set("!Type", "UnitFloat");
		theMap.Set("UnitID", sIntAsVal(asUnitFloat.fUnitID));
		theMap.Set("Value", asUnitFloat.fValue);
		oVal = theMap;
		return true;
		}

	Enumerated asEnumerated;
	if (iVal.QGetEnumerated(asEnumerated))
		{
		ZValMap_ZooLib theMap;
		theMap.Set("!Type", "Enumerated");
		theMap.Set("UnitID", sIntAsVal(asEnumerated.fEnumType));
		theMap.Set("Value", sIntAsVal(asEnumerated.fValue));
		oVal = theMap;
		return true;
		}

	FileRef asFileRef;
	if (iVal.QGetFileRef(asFileRef))
		{
		ZValMap_ZooLib theMap;
		theMap.Set("!Type", "FileRef");
		theMap.Set("PathPOSIX", asFileRef.AsPathPOSIX());
		theMap.Set("PathNative", asFileRef.AsPathNative());
		oVal = theMap;
		return true;
		}

	return false;
	}

// =================================================================================================
#pragma mark -
#pragma mark * Factory

namespace ZANONYMOUS {

class Maker0
:	public ZFunctionChain_T<ZVal_ZooLib, ZRef<ZYadR> >
	{
	virtual bool Invoke(Result_t& oResult, Param_t iParam)
		{
		if (ZRef<YadR> theYadR = ZRefDynamicCast<YadR>(iParam))
			{
			return sAsVal_ZooLib(theYadR->GetVal(), oResult);
			}
		return false;
		}	
	} sMaker0;

} // anonymous namespace

// =================================================================================================
#pragma mark -
#pragma mark * YadListRPos

class YadListRPos
:	public YadR
,	public ZYadListRPos_Val_T<YadListRPos, List>
	{
public:
	YadListRPos(const List& iList);
	YadListRPos(const List& iList, uint64 iPosition);
	};

YadListRPos::YadListRPos(const List& iList)
:	YadR(iList)
,	YadListBase_t(iList)
	{}

YadListRPos::YadListRPos(const List& iList, uint64 iPosition)
:	YadR(iList)
,	YadListBase_t(iList, iPosition)
	{}

// =================================================================================================
#pragma mark -
#pragma mark * YadMapRPos

class YadMapRPos
:	public YadR
,	public ZYadMapRPos_Val_T<YadMapRPos, Map>
	{
public:
	YadMapRPos(const Map& iMap);
	YadMapRPos(const Map& iMap, const Index_t& iIndex);

// From ZYadR via ZYadMapRPos_Val_T
	virtual ZRef<ZYadR> ReadInc(std::string& oName);

// From ZYadMapR via ZYadMapRPos_Val_T
	virtual void SetPosition(const std::string& iName);
	};

YadMapRPos::YadMapRPos(const Map& iMap)
:	YadR(iMap)
,	YadMapBase_t(iMap, iMap.begin())
	{}

YadMapRPos::YadMapRPos(const Map& iMap, const Index_t& iIndex)
:	YadR(iMap)
,	YadMapBase_t(iMap, iIndex)
	{}

ZRef<ZYadR> YadMapRPos::ReadInc(string& oName)
	{
	if (fIndex != fMap.end())
		{
		oName = fMap.NameOf(fIndex);
		return sMakeYadR(fMap.Get(fIndex++));
		}
	return ZRef<ZYadR>();
	}

void YadMapRPos::SetPosition(const string& iName)
	{ fIndex = fMap.IndexOf(iName); }

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

	ZVal_ZooLib theVal_Z;
	if (sAsVal_ZooLib(iVal, theVal_Z))
		return sMakeYadR(theVal_Z);

	return ZRef<ZYadR>();
	}

} // namespace ZPhotoshop

NAMESPACE_ZOOLIB_END
