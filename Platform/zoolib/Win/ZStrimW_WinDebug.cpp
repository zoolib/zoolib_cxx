// Copyright (c) 2010 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/ZStrimW_WinDebug.h"

#if ZCONFIG_SPI_Enabled(Win)

#include "zoolib/ZCompat_Win.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - ZStrimW_WinDebug

void ZStrimW_WinDebug::Imp_WriteUTF8(const UTF8* iSource, size_t iCountCU, size_t* oCountCU)
	{
	std::string theString(iSource, iCountCU);
	::OutputDebugStringA(theString.c_str());
	if (oCountCU)
		*oCountCU = iCountCU;
	}

} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(Win)
