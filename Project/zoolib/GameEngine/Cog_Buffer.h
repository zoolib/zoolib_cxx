#ifndef __ZooLib_GameEngine_Cog_Buffer_h__
#define __ZooLib_GameEngine_Cog_Buffer_h__ 1

#include "zoolib/GameEngine/Cog.h"
#include "zoolib/GameEngine/Tween_AlphaGainMat.h"

namespace ZooLib {
namespace GameEngine {

Cog sCog_Buffer(int iWidth, int iHeight, const ZRGBA& iRGBA, const Cog& iChild);

Cog sCog_Buffer_Continue(const ZRef<Tween_AlphaGainMat>& iTween,
	int iWidth, int iHeight, const ZRGBA& iRGBA, const Cog& iChild);

} // namespace GameEngine
} // namespace ZooLib

#endif // __ZooLib_GameEngine_Cog_Buffer_h__
