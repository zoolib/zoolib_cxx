#ifndef __ZooLib_GameEngine_Util_Assets_h__
#define __ZooLib_GameEngine_Util_Assets_h__ 1
#include "zconfig.h"

#include "zoolib/File.h"
#include "zoolib/Val_Any.h"

#include <map>

namespace ZooLib {
namespace GameEngine {

// =================================================================================================
// MARK: -

bool sReadAnim(const FileSpec& iFS, std::map<string8,FileSpec>& oFiles, Map_Any& oMap);

} // namespace GameEngine
} // namespace ZooLib

#endif // __ZooLib_GameEngine_Util_Assets_h__
