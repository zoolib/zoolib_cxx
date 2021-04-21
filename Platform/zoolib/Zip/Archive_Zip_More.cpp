// Copyright (c) 2021 Andrew Green and Mark/Space. MIT License. http://www.zoolib.org

#include "zoolib/Zip/Archive_Zip_More.h"

#include "zoolib/ChanRWPos_XX_RAM.h"
#include "zoolib/Util_Channer.h"

#include "zoolib/POSIX/FILE_Channer.h" // For sFILE_RPos

namespace ZooLib {

// =================================================================================================
#pragma mark -

ZP<Archive_Zip> sArchive_Zip(ZP<ChannerRPos_Bin> iChannerRPos)
	{ return sArchive_Zip(sFILE_RPos(iChannerRPos)); }

ZP<Archive_Zip> sArchive_Zip(ZP<ChannerR_Bin> iChannerR)
	{
	// If it's actually an RPos, then we can use it directly.
	if (ZP<ChannerRPos_Bin> asRPos = iChannerR.DynamicCast<ChannerRPos_Bin>())
		return sArchive_Zip(asRPos);

	// Otherwise wrap a sChannerRPos_XX_ChannerR round it.
	return sArchive_Zip(
		sChannerRPos_XX_ChannerR<byte>(iChannerR, sChanner_T<ChanRWPos_XX_RAM<byte>>()));
	}

} // namespace ZooLib
