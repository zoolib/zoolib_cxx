#ifndef __ZooLib_GameEngine_Cog_Group_h__
#define __ZooLib_GameEngine_Cog_Group_h__ 1

#include "zoolib/GameEngine/Cog.h"
#include "zoolib/GameEngine/Tween_BlushGainMat.h"
#include "zoolib/GameEngine/Types.h"

namespace ZooLib {
namespace GameEngine {

Cog sCog_Group_WithState(const SharedState& iState, const Cog& iChild);

Cog sCog_Group_WithBlushGainMat(const BlushGainMat& iBlushGainMat, const Cog& iChild);

Cog sCog_Group_Loop(const ZRef<Tween_BlushGainMat>& iTween, const Cog& iChild);
Cog sCog_Group_Terminate(const ZRef<Tween_BlushGainMat>& iTween, const Cog& iChild);
Cog sCog_Group_Continue(const ZRef<Tween_BlushGainMat>& iTween, const Cog& iChild);

ZRef<Rendered> sRendered_BlushGainMat(const BlushGainMat& iBGM, ZRef<Rendered> iRendered);

} // namespace GameEngine
} // namespace ZooLib

#endif // __ZooLib_GameEngine_Cog_Group_h__
