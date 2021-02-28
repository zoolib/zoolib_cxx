// Copyright (c) 2021 Andrew Green and Mark/Space. MIT License. http://www.zoolib.org

#include "zoolib/Zip/Archive_Zip_More.h"

#include "zoolib/Chan_Bin_Data.h"
#include "zoolib/Data_ZZ.h"
#include "zoolib/Util_Channer.h"

#include "zoolib/POSIX/FILE_Channer.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -

ZP<Archive_Zip> sArchive_Zip(ZP<ChannerRPos_Bin> iChannerRPos)
	{ return sArchive_Zip(sFILE_RPos(iChannerRPos)); }

ZP<Archive_Zip> sArchive_Zip(ZP<ChannerR_Bin> iChannerR)
	{
	return sArchive_Zip(
		sChannerRPos_XX_ChannerR<byte>(iChannerR, new Channer_T<ChanRWPos_Bin_Data<Data_ZZ>>()));
	}

} // namespace ZooLib
