// Copyright (c) 2003 Andrew Green and Learning in Motion, Inc.
// MIT License. http://www.zoolib.org

#ifndef __ZooLib_FILE_Channer_h__
#define __ZooLib_FILE_Channer_h__ 1
#include "zconfig.h"

#include "zoolib/Channer_Bin.h"

#include <stdio.h>

namespace ZooLib {

// =================================================================================================
#pragma mark - FILE backed by a Channer

FILE* sFILE_R(ZP<ChannerR_Bin> iChannerR);
FILE* sFILE_RPos(ZP<ChannerRPos_Bin> iChannerRPos);
FILE* sFILE_W(ZP<ChannerW_Bin> iChannerW);

} // namespace ZooLib

#endif // __ZooLib_FILE_Channer_h__
