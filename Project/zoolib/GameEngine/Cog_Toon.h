#ifndef __ZooLib_GameEngine_Cog_Toon_h__
#define __ZooLib_GameEngine_Cog_Toon_h__ 1

#include "zoolib/GameEngine/Cog.h"
#include "zoolib/GameEngine/Toon.h"

namespace ZooLib {
namespace GameEngine {

Cog sCog_Toon_NoEpoch(const ZRef<Toon>& iToon);

Cog sCog_Toon(const ZRef<Toon>& iToon);

} // namespace GameEngine
} // namespace ZooLib

#endif // __ZooLib_GameEngine_Cog_Toon_h__
