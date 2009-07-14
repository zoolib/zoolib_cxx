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

#include "zoolib/ZFunctionChain.h"
#include "zoolib/ZPhotoshop_Yad.h"
#include "zoolib/ZStream_ValData_T.h"
#include "zoolib/ZYad_ZooLib.h"

NAMESPACE_ZOOLIB_BEGIN

namespace ZPhotoshop {

using std::string;

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

	PSAlias asAlias;
	if (iVal.QGetAlias(asAlias))
		{
		ZValMap_ZooLib theMap;
		theMap.Set("!Type", "Alias");
		theMap.Set("Path", asAlias.AsString());
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
#pragma mark * YadR

YadR::YadR(const Val& iVal)
:	fVal(iVal)
	{}

bool YadR::IsSimple(const ZYadOptions& iOptions)
	{
	return ZYadR::IsSimple(iOptions);
	}

const Val& YadR::GetVal()
	{ return fVal; }

// =================================================================================================
#pragma mark -
#pragma mark * YadListRPos

class YadListRPos
:	public YadR,
	public ZYadListRPos
	{
public:
	YadListRPos(const List& iList);
	YadListRPos(const List& iList, uint64 iPosition);

// From ZYadR via ZYadListRPos
	virtual ZRef<ZYadR> ReadInc();

// From ZYadR, disambiguating between YadR and ZYadListRPos
	virtual bool IsSimple(const ZYadOptions& iOptions);

// From ZYadListR via ZYadListRPos
	virtual uint64 GetPosition();

// From ZYadListRPos
	virtual uint64 GetSize();
	virtual void SetPosition(uint64 iPosition);
	virtual ZRef<ZYadListRPos> Clone();

private:
	const List& fList;
	uint64 fPosition;
	};


YadListRPos::YadListRPos(const List& iList)
:	YadR(iList),
	fList(iList),
	fPosition(0)
	{}

YadListRPos::YadListRPos(const List& iList, uint64 iPosition)
:	YadR(iList),
	fList(iList),
	fPosition(iPosition)
	{}

ZRef<ZYadR> YadListRPos::ReadInc()
	{
	if (fPosition < fList.Count())
		return sMakeYadR(fList.Get(fPosition++));
	return ZRef<ZYadR>();
	}

bool YadListRPos::IsSimple(const ZYadOptions& iOptions)
	{ return ZYadListRPos::IsSimple(iOptions); }

uint64 YadListRPos::GetPosition()
	{ return fPosition; }

uint64 YadListRPos::GetSize()
	{ return fList.Count(); }

void YadListRPos::SetPosition(uint64 iPosition)
	{ fPosition = iPosition; }

ZRef<ZYadListRPos> YadListRPos::Clone()
	{ return new YadListRPos(fList, fPosition); }

// =================================================================================================
#pragma mark -
#pragma mark * YadMapRPos

class YadMapRPos
:	public YadR,
	public ZYadMapRPos
	{
public:
	YadMapRPos(const Map& iMap);
	YadMapRPos(const Map& iMap, const Map::Index_t& iIter);

// From ZYadR via ZYadMapRPos
	virtual ZRef<ZYadR> ReadInc(std::string& oName);

// From ZYadR, disambiguating between YadR and ZYadMapRPos
	virtual bool IsSimple(const ZYadOptions& iOptions);

// From ZYadMapR via ZYadMapRPos
	virtual void SetPosition(const std::string& iName);
	virtual ZRef<ZYadMapRPos> Clone();

private:
	const Map fMap;
	Map::Index_t fIter;
	};

YadMapRPos::YadMapRPos(const Map& iMap)
:	YadR(iMap),
	fMap(iMap),
	fIter(fMap.begin())
	{}

YadMapRPos::YadMapRPos(
	const Map& iMap,
	const Map::Index_t& iIter)
:	YadR(iMap),
	fMap(iMap),
	fIter(iIter)
	{}

ZRef<ZYadR> YadMapRPos::ReadInc(string& oName)
	{
	if (fIter != fMap.end())
		{
		oName = fMap.NameOf(fIter);
		return sMakeYadR(fMap.Get(fIter++));
		}
	return ZRef<ZYadR>();
	}

bool YadMapRPos::IsSimple(const ZYadOptions& iOptions)
	{ return ZYadMapRPos::IsSimple(iOptions); }

void YadMapRPos::SetPosition(const string& iName)
	{ fIter = fMap.IndexOf(iName); }

ZRef<ZYadMapRPos> YadMapRPos::Clone()
	{ return new YadMapRPos(fMap, fIter); }

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
		return ZYad_ZooLib::sMakeYadR(theVal_Z);

	return ZRef<ZYadR>();
	}

} // namespace ZPhotoshop

NAMESPACE_ZOOLIB_END
