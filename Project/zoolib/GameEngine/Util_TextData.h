// Copyright (c) 2019 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_GameEngine_Util_TextData_h__
#define __ZooLib_GameEngine_Util_TextData_h__ 1

#include "zoolib/File.h" // For FileSpec
#include "zoolib/Val_ZZ.h"

namespace ZooLib {
namespace GameEngine {

// =================================================================================================
#pragma mark -

ZQ<Map_ZZ> sQReadMap_ZZ(const ChanR_Bin& iChanR, const string8* iName = nullptr);
ZQ<Map_ZZ> sQReadMap_ZZ(const ChanR_Bin& iChanR, const string8& iName);
ZQ<Map_ZZ> sQReadMap_ZZ(const FileSpec& iFS);

Map_ZZ sReadTextData(const FileSpec& iFS);

} // namespace GameEngine
} // namespace ZooLib

#endif // __ZooLib_GameEngine_Util_TextData_h__
