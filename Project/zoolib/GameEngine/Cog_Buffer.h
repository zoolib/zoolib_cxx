#ifndef __ZooLib_GameEngine_Cog_Buffer_h__
#define __ZooLib_GameEngine_Cog_Buffer_h__ 1

#include "zoolib/GameEngine/Cog.h"
#include "zoolib/GameEngine/Tween_BlushGainMat.h"

namespace ZooLib {
namespace GameEngine {

Cog sCog_Buffer(int iWidth, int iHeight, const RGBA& iFill, const Cog& iChild);

Cog sCog_Buffer_Continue(const ZP<Tween_BlushGainMat>& iTween,
	int iWidth, int iHeight, const RGBA& iFill, const Cog& iChild);

} // namespace GameEngine
} // namespace ZooLib

#endif // __ZooLib_GameEngine_Cog_Buffer_h__
