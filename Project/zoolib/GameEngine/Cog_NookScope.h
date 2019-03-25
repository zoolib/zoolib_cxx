#ifndef __ZooLib_GameEngine_Cog_NookScope_h__
#define __ZooLib_GameEngine_Cog_NookScope_h__ 1

#include "zoolib/GameEngine/Cog.h"

namespace ZooLib {
namespace GameEngine {

Cog sCog_NookScope(const ZRef<NookScope>& iNookScope, const Cog& iChild);

Cog sCog_NookScope(const Cog& iChild);

} // namespace GameEngine
} // namespace ZooLib

#endif // __ZooLib_GameEngine_Cog_NookScope_h__
