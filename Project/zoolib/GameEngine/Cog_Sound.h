#ifndef __ZooLib_GameEngine_Cog_Sound_h__
#define __ZooLib_GameEngine_Cog_Sound_h__ 1

#include "zoolib/GameEngine/Cog.h"

namespace ZooLib {
namespace GameEngine {

// =================================================================================================
#pragma mark - Sound cogs

Cog sCog_Sound(const string8& iName);
Cog sCog_Sound(const string8& iName, const string8& iScope, bool iDetached);

} // namespace GameEngine
} // namespace ZooLib

#endif // __ZooLib_GameEngine_Cog_Sound_h__
