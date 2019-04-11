#ifndef __ZooLib_GameEngine_Cog_Delay_h__
#define __ZooLib_GameEngine_Cog_Delay_h__ 1

#include "zoolib/GameEngine/Cog.h"

namespace ZooLib {
namespace GameEngine {

// =================================================================================================
#pragma mark - Delay cogs

Cog sCog_DelayUntil(double iElapsed);
Cog sCog_DelayFor(double iDelay);

Cog sCog_StartAt(double iElapsed, const Cog& iCog);
Cog sCog_StopAt(double iElapsed, const Cog& iCog);
Cog sCog_StartAfter(double iDelay, const Cog& iCog);
Cog sCog_StopAfter(double iDelay, const Cog& iCog);

} // namespace GameEngine
} // namespace ZooLib

#endif // __ZooLib_GameEngine_Cog_Delay_h__
