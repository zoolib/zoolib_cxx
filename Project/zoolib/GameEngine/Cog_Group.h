#ifndef __ZooLib_GameEngine_Cog_Group_h__
#define __ZooLib_GameEngine_Cog_Group_h__ 1

#include "zoolib/GameEngine/Cog.h"
#include "zoolib/GameEngine/Tween_AlphaGainMat.h"
#include "zoolib/GameEngine/Types.h"

namespace ZooLib {
namespace GameEngine {

Cog sCog_Group_WithState(const SharedState& iState, const Cog& iChild);

Cog sCog_Group_WithAlphaGainMat(const AlphaGainMat& iAlphaGainMat, const Cog& iChild);

Cog sCog_Group_Loop(const ZRef<Tween_AlphaGainMat>& iTween, const Cog& iChild);
Cog sCog_Group_Terminate(const ZRef<Tween_AlphaGainMat>& iTween, const Cog& iChild);
Cog sCog_Group_Continue(const ZRef<Tween_AlphaGainMat>& iTween, const Cog& iChild);

} // namespace GameEngine
} // namespace ZooLib

#endif // __ZooLib_GameEngine_Cog_Group_h__
