#ifndef __ZooLib_GameEngine_Game_Render_h__
#define __ZooLib_GameEngine_Game_Render_h__ 1
#include "zconfig.h"

#include "zoolib/GameEngine/Rendered.h"
#include "zoolib/GameEngine/Sound.h"

namespace ZooLib {
namespace GameEngine {

// =================================================================================================
#pragma mark -

GPoint sPixelToGame(const GPoint& iPixelSize, const GPoint& iGameSize, GPoint iPoint);

void sGame_Render(const ZRef<Rendered>& iRendered,
	const GPoint& iPixelSize,
	const GPoint& iGameSize,
	bool iShowBounds, bool iShowOrigin,
	const ZRef<SoundMeister>& iSoundMeister, Rat iListenerL, Rat iListenerR, Rat iListenerD);

} // namespace GameEngine
} // namespace ZooLib {

#endif // __ZooLib_GameEngine_Game_Render_h__
