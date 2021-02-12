// Copyright (c) 2021 Andrew Green and Mark/Space, Inc. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Zip_File_Zip_More_h__
#define __ZooLib_Zip_File_Zip_More_h__ 1
#include "zconfig.h"

#include "zoolib/Zip/File_Zip.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -

FileSpec sFileSpec_Zip(ZP<ChannerRPos_Bin> iChannerRPos);

FileSpec sFileSpec_Zip(ZP<ChannerR_Bin> iChannerR, ZP<ChannerRWPos_Bin> iBuffer);

FileSpec sFileSpec_Zip(ZP<ChannerR_Bin> iChannerR);

} // namespace ZooLib

#endif // __ZooLib_Zip_File_Zip_More_h__
