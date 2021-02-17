// Copyright (c) 2021 Andrew Green and Mark/Space, Inc. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Zip_Archive_Zip_More_h__
#define __ZooLib_Zip_Archive_Zip_More_h__ 1
#include "zconfig.h"

#include "zoolib/Zip/Archive_Zip.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -

ZP<Archive_Zip> sArchive_Zip(ZP<ChannerRPos_Bin> iChannerRPos);

ZP<Archive_Zip> sArchive_Zip(ZP<ChannerR_Bin> iChannerR, ZP<ChannerRWPos_Bin> iBuffer);

ZP<Archive_Zip> sArchive_Zip(ZP<ChannerR_Bin> iChannerR);

} // namespace ZooLib

#endif // __ZooLib_Zip_Archive_Zip_More_h__
