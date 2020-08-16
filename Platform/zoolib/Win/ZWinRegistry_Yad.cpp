// Copyright (c) 2009 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/ZWinRegistry_Yad.h"

#if ZCONFIG_SPI_Enabled(Win)

#include "zoolib/ZUnicode.h"
#include "zoolib/ZYad_Any.h"
#include "zoolib/ZYad_Val_T.h"

namespace ZooLib {
namespace ZWinRegistry {

// =================================================================================================
#pragma mark - ZWinRegistry

ZRef<ZYadR> sYadR(const Val& iVal)
	{
	if (const KeyRef* theVal = iVal.PGet<KeyRef>())
		return sYadR(*theVal);

	if (const string16* theVal = iVal.PGet<string16>())
		return sYadR(ZUnicode::sAsUTF8(*theVal));

	return sYadR(iVal.AsAny());
	}

ZRef<ZYadMapRPos> sYadR(const KeyRef& iKeyRef)
	{ return new ZYadMapAtRPos_Val_T<KeyRef>(iKeyRef); }

} // namespace ZWinRegistry
} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(Win)
