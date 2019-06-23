#ifndef __ZooLib_GameEngine_RenderGame_h__
#define __ZooLib_GameEngine_RenderGame_h__ 1
#include "zconfig.h"

#include "zoolib/GameEngine/Rendered.h"
#include "zoolib/GameEngine/Sound.h"

namespace ZooLib {
namespace GameEngine {

// =================================================================================================
#pragma mark -

GPoint sPixelToGame(const GPoint& iPixelSize, const GPoint& iGameSize, GPoint iPoint);

void sRenderGame(const ZP<Rendered>& iRendered,
	const GPoint& iPixelSize,
	const GPoint& iGameSize,
	bool iShowBounds, bool iShowOrigin,
	const ZP<SoundMeister>& iSoundMeister, Rat iListenerL, Rat iListenerR, Rat iListenerD);

} // namespace GameEngine
} // namespace ZooLib {

#endif // __ZooLib_GameEngine_RenderGame_h__
