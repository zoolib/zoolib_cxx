// Copyright (c) 2019 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_GameEngine_Util_Assets_h__
#define __ZooLib_GameEngine_Util_Assets_h__ 1
#include "zconfig.h"

#include "zoolib/File.h"
#include "zoolib/Val_ZZ.h"

#include <map>

namespace ZooLib {
namespace GameEngine {

// =================================================================================================
#pragma mark -

bool sReadAnim(const FileSpec& iFS, std::map<string8,FileSpec>& oFiles, Map_ZZ& oMap);

} // namespace GameEngine
} // namespace ZooLib

#endif // __ZooLib_GameEngine_Util_Assets_h__
