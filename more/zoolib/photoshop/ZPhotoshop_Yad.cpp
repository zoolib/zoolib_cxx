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

namespace ZooLib {
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
#pragma mark * YadSeqRPos

typedef ZYadSeqRPos_Val_T<Seq> YadSeqRPos;

// =================================================================================================
#pragma mark -
#pragma mark * YadMapRPos

typedef ZYadMapRPos_Val_T<Map> YadMapRPos;

// =================================================================================================
#pragma mark -
#pragma mark * ZPhotoshop::MakeYadR

ZRef<ZYadR> sMakeYadR(const Val& iVal)
	{
	if (const Map* asMap = iVal.PGet<Map>())
		return new YadMapRPos(*asMap);

	if (const Seq* asSeq = iVal.PGet<Seq>())
		return new YadSeqRPos(*asSeq);

	if (const UnitFloat* asUnitFloat = iVal.PGet<UnitFloat>())
		{
		const ZMap_Any theMap = ZMap_Any()
			.Set("!Type", string("UnitFloat"))
			.Set("UnitID", spIntAsVal(asUnitFloat->fUnitID))
			.Set("Value", asUnitFloat->fValue);
		return sMakeYadR(theMap);
		}

	if (const Enumerated* asEnumerated = iVal.PGet<Enumerated>())
		{
		const ZMap_Any theMap = ZMap_Any()
			.Set("!Type", string("Enumerated"))
			.Set("UnitID", spIntAsVal(asEnumerated->fEnumType))
			.Set("Value", spIntAsVal(asEnumerated->fValue));
		return sMakeYadR(theMap);
		}

	if (const FileRef* asFileRef = iVal.PGet<FileRef>())
		{
		ZTrail theTrail;
		if (ZQ<ZTrail> theQ = asFileRef->AsTrail())
			theTrail = theQ.Get();
		const ZMap_Any theMap = ZMap_Any()
			.Set("!Type", string("FileRef"))
			.Set("PathTrail", theTrail.AsString());
		return sMakeYadR(theMap);
		}

	if (/*const Spec* asSpec = */iVal.PGet<Spec>())
		{
		const ZMap_Any theMap = ZMap_Any()
			.Set("!Type", string("Spec"));
		return sMakeYadR(theMap);
		}

	if (const string* asString = iVal.PGet<string>())
		return ZooLib::sMakeYadR(*asString);

	return new ZYadAtomR_Any(iVal.AsAny());
	}

ZRef<ZYadSeqRPos> sMakeYadR(const Seq& iSeq)
	{ return new YadSeqRPos(iSeq); }

ZRef<ZYadMapRPos> sMakeYadR(const Map& iMap)
	{ return new YadMapRPos(iMap); }

} // namespace ZPhotoshop
} // namespace ZooLib
