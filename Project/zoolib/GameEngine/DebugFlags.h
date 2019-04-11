#ifndef __ZooLib_GameEngine_DebugFlags_h__
#define __ZooLib_GameEngine_DebugFlags_h__ 1
#include "zconfig.h"

// =================================================================================================
#pragma mark -

namespace ZooLib {
namespace GameEngine {

struct DebugFlags
	{
	static bool sTouches;
	static bool sTextureBounds;
	static bool sTextureNameFrame;
	static bool sInfo;
	static bool sInfoText;
	};

} // namespace GameEngine
} // namespace ZooLib

#endif // __ZooLib_GameEngine_DebugFlags_h__
