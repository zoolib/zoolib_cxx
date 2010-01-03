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

#include "zoolib/ZYad_NS.h"

#if ZCONFIG_SPI_Enabled(Cocoa)

#include "zoolib/ZStream_Data_T.h"
#include "zoolib/ZStrim_NSString.h"
#include "zoolib/ZUtil_NSObject.h"

NAMESPACE_ZOOLIB_BEGIN

namespace ZUtil_NS = ZUtil_NSObject;

using std::min;
using std::string;
using std::vector;

// =================================================================================================
#pragma mark -
#pragma mark * ZYadPrimR_NS

ZYadPrimR_NS::ZYadPrimR_NS(const ZRef<NSObject>& iVal)
:	ZYadR_NS(iVal)
	{}

ZAny ZYadPrimR_NS::AsAny()
	{ return ZUtil_NS::sAsAny(this->GetVal()); }

// =================================================================================================
#pragma mark -
#pragma mark * ZYadStreamRPos_NS

ZYadStreamRPos_NS::ZYadStreamRPos_NS(const ZRef<NSData>& iDataRef)
:	ZYadR_NS(iDataRef)
,	ZStreamerRPos_NS(iDataRef)
	{}

ZYadStreamRPos_NS::~ZYadStreamRPos_NS()
	{}

bool ZYadStreamRPos_NS::IsSimple(const ZYadOptions& iOptions)
	{ return this->GetStreamRPos().GetSize() <= iOptions.fRawChunkSize; }

// =================================================================================================
#pragma mark -
#pragma mark * ZYadStrimR_NS

ZYadStrimR_NS::ZYadStrimR_NS(ZRef<NSString> iString)
:	ZYadR_NS(iString)
,	ZStrimmerR_NSString(iString)
	{}

// =================================================================================================
#pragma mark -
#pragma mark * ZYadListRPos_NS

ZYadListRPos_NS::ZYadListRPos_NS(const ZRef<NSArray>& iArray)
:	ZYadR_NS(iArray)
,	YadListBase_t(iArray)
	{}

ZYadListRPos_NS::ZYadListRPos_NS(const ZRef<NSArray>& iArray, uint64 iPosition)
:	ZYadR_NS(iArray)
,	YadListBase_t(iArray, iPosition)
	{}

// =================================================================================================
#pragma mark -
#pragma mark * ZYadMapRPos_NS

ZYadMapRPos_NS::ZYadMapRPos_NS(const ZRef<NSDictionary>& iDictionary,
	uint64 iPosition,
	const ZList_NS& iNames,
	const ZList_NS& iValues)
:	ZYadR_NS(iDictionary)
,	fDictionary(iDictionary)
,	fPosition(iPosition)
,	fNames(iNames)
,	fValues(iValues)
	{}

ZYadMapRPos_NS::ZYadMapRPos_NS(const ZRef<NSDictionary>& iDictionary)
:	ZYadR_NS(iDictionary)
,	fDictionary(iDictionary)
,	fPosition(0)
,	fNames(ZRef<NSArray>([iDictionary allKeys]))
,	fValues(ZRef<NSArray>([iDictionary allValues]))
	{}

ZRef<ZYadR> ZYadMapRPos_NS::ReadInc(string& oName)
	{
	if (fPosition < fNames.Count())
		{
		oName = fNames.Get(fPosition).GetString();
		return sMakeYadR(fValues.Get(fPosition++));
		}
	return ZRef<ZYadR>();
	}

ZRef<ZYadMapRPos> ZYadMapRPos_NS::Clone()
	{ return new ZYadMapRPos_NS(fDictionary, fPosition, fNames, fValues); }

void ZYadMapRPos_NS::SetPosition(const std::string& iName)
	{
	const size_t count = fNames.Count();
	for (fPosition = 0; fPosition < count; ++fPosition)
		{
		if (iName == fNames.Get(fPosition).GetString())
			break;
		}
	}

// =================================================================================================
#pragma mark -
#pragma mark * sMakeYadR

ZRef<ZYadR> sMakeYadR(const ZRef<NSObject>& iVal)
	{
	const ZVal_NS theVal = iVal;

	ZMap_NS asMap;
	if (theVal.QGetMap(asMap))
		return new ZYadMapRPos_NS(asMap);
		
	ZList_NS asList;
	if (theVal.QGetList(asList))
		return new ZYadListRPos_NS(asList);
		
	ZData_NS asData;
	if (theVal.QGetData(asData))
		return new ZYadStreamRPos_NS(asData);

	ZRef<NSString> asNSString;
	if (theVal.QGetNSString(asNSString))
		return new ZYadStrimR_NS(asNSString);

	return new ZYadPrimR_NS(iVal);
	}

ZRef<ZYadStrimR> sMakeYadR(const ZRef<NSMutableString>& iString)
	{ return new ZYadStrimR_NS(iString); }

ZRef<ZYadStrimR> sMakeYadR(const ZRef<NSString>& iString)
	{ return new ZYadStrimR_NS(iString); }

ZRef<ZYadStreamR> sMakeYadR(const ZRef<NSMutableData>& iData)
	{ return new ZYadStreamRPos_NS(iData); }

ZRef<ZYadStreamR> sMakeYadR(const ZRef<NSData>& iData)
	{ return new ZYadStreamRPos_NS(iData); }

ZRef<ZYadListR> sMakeYadR(const ZRef<NSMutableArray>& iList)
	{ return new ZYadListRPos_NS(iList); }

ZRef<ZYadListR> sMakeYadR(const ZRef<NSArray>& iList)
	{ return new ZYadListRPos_NS(iList); }

ZRef<ZYadMapR> sMakeYadR(const ZRef<NSMutableDictionary>& iMap)
	{ return new ZYadMapRPos_NS(iMap); }

ZRef<ZYadMapR> sMakeYadR(const ZRef<NSDictionary>& iMap)
	{ return new ZYadMapRPos_NS(iMap); }

// =================================================================================================
#pragma mark -
#pragma mark * sFromYadR

namespace ZANONYMOUS {

class YadVisitor_GetVal_NS : public ZYadVisitor
	{
public:
	YadVisitor_GetVal_NS(ZRef<NSObject> iDefault);

// From ZYadVisitor
	virtual bool Visit_YadPrimR(ZRef<ZYadPrimR> iYadPrimR);
	virtual bool Visit_YadStreamR(ZRef<ZYadStreamR> iYadStreamR);
	virtual bool Visit_YadStrimR(ZRef<ZYadStrimR> iYadStrimR);
	virtual bool Visit_YadListR(ZRef<ZYadListR> iYadListR);
	virtual bool Visit_YadMapR(ZRef<ZYadMapR> iYadMapR);

	ZRef<NSObject> fDefault;
	ZRef<NSObject> fOutput;
	};

} // anonymous namespace

ZRef<NSObject> sFromYadR(const ZRef<NSObject>& iDefault, ZRef<ZYadR> iYadR)
	{
	if (ZRef<ZYadR_NS> theYadR = ZRefDynamicCast<ZYadR_NS>(iYadR))
		return theYadR->GetVal();

	YadVisitor_GetVal_NS theVisitor(iDefault);
	iYadR->Accept(theVisitor);
	return theVisitor.fOutput;
	}

YadVisitor_GetVal_NS::YadVisitor_GetVal_NS(ZRef<NSObject> iDefault)
:	fDefault(iDefault)
	{}

bool YadVisitor_GetVal_NS::Visit_YadPrimR(ZRef<ZYadPrimR> iYadPrimR)
	{
	fOutput = ZUtil_NS::sAsNSObject(fDefault, iYadPrimR->AsAny());
	return true;
	}

bool YadVisitor_GetVal_NS::Visit_YadStreamR(ZRef<ZYadStreamR> iYadStreamR)
	{
	fOutput = sReadAll_T<ZData_NS>(iYadStreamR->GetStreamR());
	return true;
	}

bool YadVisitor_GetVal_NS::Visit_YadStrimR(ZRef<ZYadStrimR> iYadStrimR)
	{
	NSMutableString* result = ZUtil_NS::sStringMutable();
	ZStrimW_NSString(result).CopyAllFrom(iYadStrimR->GetStrimR());
	fOutput = result;
	return true;
	}

bool YadVisitor_GetVal_NS::Visit_YadListR(ZRef<ZYadListR> iYadListR)
	{
	ZList_NS theList;

	while (ZRef<ZYadR> theChild = iYadListR->ReadInc())
		theList.Append(sFromYadR(fDefault, theChild));

	fOutput = theList;
	return true;
	}

bool YadVisitor_GetVal_NS::Visit_YadMapR(ZRef<ZYadMapR> iYadMapR)
	{
	ZMap_NS theMap;

	string theName;
	while (ZRef<ZYadR> theChild = iYadMapR->ReadInc(theName))
		theMap.Set(theName, sFromYadR(fDefault, theChild));

	fOutput = theMap;
	return true;
	}

NAMESPACE_ZOOLIB_END

#endif // ZCONFIG_SPI_Enabled(Cocoa)
