// Copyright (c) 2021 Andrew Green and Mark/Space. MIT License. http://www.zoolib.org

#include "zoolib/Zip/Archive_Zip_More.h"

#include "zoolib/Chan_Bin_Data.h"
#include "zoolib/ChanRPos_XX_ChanR.h"
#include "zoolib/Data_ZZ.h"

#include "zoolib/POSIX/FILE_Channer.h"

namespace ZooLib {

template <class EE>
ZP<ChannerRPos<EE>> sChannerRPos_XX_ChannerR(
	const ZP<ChannerR<EE>>& iChannerR,
	const ZP<ChannerRWPos<EE>>& iChannerRWPos)
	{
	if (iChannerR && iChannerRWPos)
		{
		typedef std::tuple<ZP<Counted>,ZP<Counted>> Holder_t;

		return new Channer_Holder_T<Holder_t,ChanRPos_XX_ChanR<EE>>
			(Holder_t(iChannerR,iChannerRWPos), *iChannerR, *iChannerRWPos);
		}
	return null;
	}

// =================================================================================================
#pragma mark -

ZP<Archive_Zip> sArchive_Zip(ZP<ChannerRPos_Bin> iChannerRPos)
	{ return sArchive_Zip(sFILE_RPos(iChannerRPos)); }

ZP<Archive_Zip> sArchive_Zip(ZP<ChannerR_Bin> iChannerR, ZP<ChannerRWPos_Bin> iBuffer)
	{return sArchive_Zip(sChannerRPos_XX_ChannerR<byte>(iChannerR, iBuffer)); }

ZP<Archive_Zip> sArchive_Zip(ZP<ChannerR_Bin> iChannerR)
	{ return sArchive_Zip(iChannerR, new Channer_T<ChanRWPos_Bin_Data<Data_ZZ>>); }

} // namespace ZooLib
