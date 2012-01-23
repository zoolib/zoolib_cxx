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

#if ZCONFIG_SPI_Enabled(AppleEvent)

#include "zoolib/ZYad_Any.h"
//#include "zoolib/ZYad_Std.h"

namespace ZooLib {

using std::string;

// =================================================================================================
// MARK: - Helpers

static ZMap_Any spAsMap(const AEDesc& iAEDesc)
	{
	ZMap_Any theMap;
	theMap.Set("!AEType", sAEKeywordAsString(iAEDesc.descriptorType));

	const size_t theSize = ::AEGetDescDataSize(&iAEDesc);
	ZData_Any theData(theSize);
	::AEGetDescData(&iAEDesc, theData.GetPtrMutable(), theSize);

	theMap.Set("Value", theData);

	return theMap;
	}

// =================================================================================================
// MARK: - sYadR

ZRef<ZYadR> sYadR(const ZVal_AppleEvent& iVal)
	{
	if (ZQ<ZMap_AppleEvent> theQ = iVal.QGet<ZMap_AppleEvent>())
		return new ZYadMapRPos_AppleEvent(theQ.Get());

	if (ZQ<ZSeq_AppleEvent> theQ = iVal.QGet<ZSeq_AppleEvent>())
		return new ZYadSeqRPos_AppleEvent(theQ.Get());

	if (ZQ<string8> theQ = iVal.QGet<string8>())
		return sYadR(theQ.Get());

//	if (iVal.QGetData(asData))
//		return new ZYadStreamRPos_AppleEvent(asData);

	return new ZYadMapRPos_Any(spAsMap(iVal));
	}

ZRef<ZYadSeqRPos> sYadR(const ZSeq_AppleEvent& iSeq)
	{ return new ZYadSeqRPos_AppleEvent(iSeq); }

ZRef<ZYadMapRPos> sYadR(const ZMap_AppleEvent& iMap)
	{ return new ZYadMapRPos_AppleEvent(iMap); }

} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(AppleEvent)
