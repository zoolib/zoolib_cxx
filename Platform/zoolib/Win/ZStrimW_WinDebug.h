// Copyright (c) 2010 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZStrimW_WinDebug_h__
#define __ZStrimW_WinDebug_h__ 1
#include "zconfig.h"

#include "zoolib/ZCONFIG_SPI.h"
#include "zoolib/ZStrim.h"

#if ZCONFIG_SPI_Enabled(Win)

namespace ZooLib {

// =================================================================================================
#pragma mark - ZStrimW_WinDebug

class ZStrimW_WinDebug : public ZStrimW_NativeUTF8
	{
public:
	virtual void Imp_WriteUTF8(const UTF8* iSource, size_t iCountCU, size_t* oCountCU);
	};

} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(Win)

#endif // __ZStrimW_WinDebug_h__
