#ifndef __ZooLib_GameEngine_Cog_Group_h__
#define __ZooLib_GameEngine_Cog_Group_h__ 1

#include "zoolib/GameEngine/Cog.h"
#include "zoolib/GameEngine/Tween_BlushGainMat.h"
#include "zoolib/GameEngine/Types.h"

namespace ZooLib {
namespace GameEngine {

Cog sCog_Group_WithState(const SharedState& iState, const Cog& iChild);

Cog sCog_Group_WithBlushGainMat(const BlushGainMat& iBlushGainMat, const Cog& iChild);

Cog sCog_Group_Loop(const ZP<Tween_BlushGainMat>& iTween, const Cog& iChild);
Cog sCog_Group_Terminate(const ZP<Tween_BlushGainMat>& iTween, const Cog& iChild);
Cog sCog_Group_Continue(const ZP<Tween_BlushGainMat>& iTween, const Cog& iChild);

ZP<Rendered> sRendered_BlushGainMat(const BlushGainMat& iBGM, ZP<Rendered> iRendered);

} // namespace GameEngine
} // namespace ZooLib

#endif // __ZooLib_GameEngine_Cog_Group_h__
