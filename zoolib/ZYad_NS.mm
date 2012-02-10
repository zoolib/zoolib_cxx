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
#include "zoolib/ZUtil_NS.h"

namespace ZooLib {

using std::min;
using std::string;
using std::vector;

// =================================================================================================
// MARK: - ZYadAtomR_NS

ZYadAtomR_NS::ZYadAtomR_NS(NSObject* iVal)
:	ZYadR_NS(iVal)
	{}

ZAny ZYadAtomR_NS::AsAny()
	{ return ZUtil_NS::sAsAny(this->GetVal()); }

// =================================================================================================
// MARK: - ZYadStreamRPos_NS

ZYadStreamRPos_NS::ZYadStreamRPos_NS(NSData* iData)
:	ZYadR_NS(iData)
,	ZStreamerRPos_NS(iData)
	{}

ZYadStreamRPos_NS::~ZYadStreamRPos_NS()
	{}

bool ZYadStreamRPos_NS::IsSimple(const ZYadOptions& iOptions)
	{ return this->GetStreamRPos().GetSize() <= iOptions.fRawChunkSize; }

// =================================================================================================
// MARK: - ZYadStrimR_NS

ZYadStrimR_NS::ZYadStrimR_NS(NSString* iString)
:	ZYadR_NS(iString)
,	ZStrimmerR_NSString(iString)
	{}

// =================================================================================================
// MARK: - ZYadSatRPos_NS

ZYadSatRPos_NS::ZYadSatRPos_NS(NSArray* iArray)
:	ZYadR_NS(iArray)
,	YadSeqBase_t(iArray)
	{}

ZYadSatRPos_NS::ZYadSatRPos_NS(NSArray* iArray, uint64 iPosition)
:	ZYadR_NS(iArray)
,	YadSeqBase_t(iArray, iPosition)
	{}

// =================================================================================================
// MARK: - ZYadMatRPos_NS

ZYadMatRPos_NS::ZYadMatRPos_NS(NSDictionary* iDictionary,
	uint64 iPosition,
	const ZSeq_NS& iNames,
	const ZSeq_NS& iValues)
:	ZYadR_NS(iDictionary)
,	fDictionary(iDictionary)
,	fPosition(iPosition)
,	fNames(iNames)
,	fValues(iValues)
	{}

ZYadMatRPos_NS::ZYadMatRPos_NS(NSDictionary* iDictionary)
:	ZYadR_NS(iDictionary)
,	fDictionary(iDictionary)
,	fPosition(-1)
,	fNames([iDictionary allKeys])
,	fValues([iDictionary allValues])
	{}

ZRef<ZYadR> ZYadMatRPos_NS::ReadInc(string& oName)
	{
	this->pSetupPosition();

	if (fPosition < fNames.Count())
		{
		oName = fNames.Get(fPosition).GetString();
		return sYadR(fValues.Get(fPosition++));
		}
	return null;
	}

ZRef<ZYadMapRClone> ZYadMatRPos_NS::Clone()
	{ return new ZYadMatRPos_NS(fDictionary, fPosition, fNames, fValues); }

void ZYadMatRPos_NS::SetPosition(const std::string& iName)
	{
	this->pSetupPosition();

	const size_t count = fNames.Count();
	for (fPosition = 0; fPosition < count; ++fPosition)
		{
		if (iName == fNames.Get(fPosition).GetString())
			break;
		}
	}

ZRef<ZYadR> ZYadMatRPos_NS::ReadAt(const std::string& iName)
	{ return sYadR((NSObject*)[fDictionary.Get() valueForKey:ZUtil_NS::sString(iName)]); }

void ZYadMatRPos_NS::pSetupPosition()
	{
	if (fPosition != -1)
		return;

	fPosition = 0;

	fNames = [fDictionary.Get() allKeys];
	fValues = [fDictionary.Get() allValues];
	}

// =================================================================================================
// MARK: - sYadR

ZRef<ZYadR> sYadR(NSObject* iVal)
	{
	const ZVal_NS theVal = iVal;

	if (ZQ<ZMap_NS> theQ = theVal.QGetMap())
		return new ZYadMatRPos_NS(*theQ);

	if (ZQ<ZSeq_NS> theQ = theVal.QGetSeq())
		return new ZYadSatRPos_NS(*theQ);

	if (ZQ<ZData_NS> theQ = theVal.QGetData())
		return new ZYadStreamRPos_NS(*theQ);

	if (ZQ<NSString*> theQ = theVal.QGetNSString())
		return new ZYadStrimR_NS(*theQ);

	return new ZYadAtomR_NS(iVal);
	}

ZRef<ZYadStrimR> sYadR(NSMutableString* iString)
	{ return new ZYadStrimR_NS(iString); }

ZRef<ZYadStrimR> sYadR(NSString* iString)
	{ return new ZYadStrimR_NS(iString); }

ZRef<ZYadStreamR> sYadR(NSMutableData* iData)
	{ return new ZYadStreamRPos_NS(iData); }

ZRef<ZYadStreamR> sYadR(NSData* iData)
	{ return new ZYadStreamRPos_NS(iData); }

ZRef<ZYadSatRPos> sYadR(NSMutableArray* iArray)
	{ return new ZYadSatRPos_NS(iArray); }

ZRef<ZYadSatRPos> sYadR(NSArray* iArray)
	{ return new ZYadSatRPos_NS(iArray); }

ZRef<ZYadMatRPos> sYadR(NSMutableDictionary* iDictionary)
	{ return new ZYadMatRPos_NS(iDictionary); }

ZRef<ZYadMatRPos> sYadR(NSDictionary* iDictionary)
	{ return new ZYadMatRPos_NS(iDictionary); }

// =================================================================================================
// MARK: - sFromYadR

namespace { // anonymous

class Visitor_GetVal : public ZVisitor_Yad_PreferAt
	{
public:
	Visitor_GetVal(ZRef<NSObject> iDefault);

// From ZVisitor_Yad
	virtual void Visit_YadAtomR(const ZRef<ZYadAtomR>& iYadAtomR);
	virtual void Visit_YadStreamR(const ZRef<ZYadStreamR>& iYadStreamR);
	virtual void Visit_YadStrimR(const ZRef<ZYadStrimR>& iYadStrimR);
	virtual void Visit_YadSeqR(const ZRef<ZYadSeqR>& iYadSeqR);
	virtual void Visit_YadMapR(const ZRef<ZYadMapR>& iYadMapR);

// Our protocol
	ZRef<NSObject> GetVal(const ZRef<ZYadR>& iYadR);

private:
	ZRef<NSObject> fDefault;
	ZRef<NSObject> fOutput;
	};

} // anonymous namespace

Visitor_GetVal::Visitor_GetVal(ZRef<NSObject> iDefault)
:	fDefault(iDefault)
	{}

void Visitor_GetVal::Visit_YadAtomR(const ZRef<ZYadAtomR>& iYadAtomR)
	{ fOutput = ZUtil_NS::sDAsNSObject(fDefault, iYadAtomR->AsAny()); }

void Visitor_GetVal::Visit_YadStreamR(const ZRef<ZYadStreamR>& iYadStreamR)
	{ fOutput = sReadAll_T<ZData_NS>(iYadStreamR->GetStreamR()); }

void Visitor_GetVal::Visit_YadStrimR(const ZRef<ZYadStrimR>& iYadStrimR)
	{
	NSMutableString* result = ZUtil_NS::sStringMutable();
	ZStrimW_NSString(result).CopyAllFrom(iYadStrimR->GetStrimR());
	fOutput = result;
	}

void Visitor_GetVal::Visit_YadSeqR(const ZRef<ZYadSeqR>& iYadSeqR)
	{
	ZSeq_NS theSeq;

	while (ZRef<ZYadR> theChild = iYadSeqR->ReadInc())
		theSeq.Append(this->GetVal(theChild));

	fOutput = theSeq;
	}

void Visitor_GetVal::Visit_YadMapR(const ZRef<ZYadMapR>& iYadMapR)
	{
	ZMap_NS theMap;

	string theName;
	while (ZRef<ZYadR> theChild = iYadMapR->ReadInc(theName))
		theMap.Set(theName, this->GetVal(theChild));

	fOutput = theMap;
	}

ZRef<NSObject> Visitor_GetVal::GetVal(const ZRef<ZYadR>& iYadR)
	{
	ZRef<NSObject> result;
	if (iYadR)
		{
		iYadR->Accept(*this);
		std::swap(result, fOutput);
		}
	return result;
	}

ZRef<NSObject> sFromYadR(NSObject* iDefault, ZRef<ZYadR> iYadR)
	{
	if (ZRef<ZYadR_NS> theYadR = iYadR.DynamicCast<ZYadR_NS>())
		return theYadR->GetVal();

	return Visitor_GetVal(iDefault).GetVal(iYadR);
	}
} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(Cocoa)
