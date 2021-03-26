// Copyright (c) 2019-21 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/Archive.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - Archive

ZQ<uint32> Archive::QCRC(size_t iIndex)
	{ return null; }

ZP<ChannerR_Bin> Archive::OpenR(size_t iIndex)
	{ return this->OpenRPos(iIndex); }

ZP<ChannerRPos_Bin> Archive::OpenRPos(size_t iIndex)
	{ return null; }

} // namespace ZooLib
