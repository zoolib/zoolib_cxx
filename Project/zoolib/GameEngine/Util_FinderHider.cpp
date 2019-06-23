#include "zoolib/GameEngine/Util_FinderHider.h"

#include "zoolib/GameEngine/DebugFlags.h"

namespace ZooLib {
namespace GameEngine {
namespace Util {

// =================================================================================================
#pragma mark - sFinderHider

ZP<Rendered> sFinderHider(
	const ZP<Rendered>& iRendered,
	GPoint iBackingSize,
	GPoint iGameSize)
	{
	// Rectangles to mask off extraneous crud.
	ZP<Rendered_Group> finderHider = sRendered_Group();

	const Rat iWidth = X(iGameSize);
	const Rat iHeight = Y(iGameSize);

	// Left
	finderHider->Append(sRendered_Rect(sRect<GRect>(-1000, -1000, 0, iHeight+1000)));

	// Top
	finderHider->Append(sRendered_Rect(sRect<GRect>(-1000, -1000, iWidth+1000, 0)));

	// Right
	finderHider->Append(sRendered_Rect(sRect<GRect>(iWidth, -1000, iWidth+1000, iHeight+1000)));

	// Bottom
	finderHider->Append(sRendered_Rect(sRect<GRect>(-1000, iHeight, iWidth+1000, iHeight+1000)));

	const RGBA theColor = DebugFlags::sTouches ? sRGBA(0.25,1) : sRGBA(0, 1);

	ZP<Rendered_Group> result = sRendered_Group();

	// include what we were passed
	result->Append(iRendered);

	// Apply color to the finderHider and bring it fairly far forward, behind frontmost at 128
	result->Append(sRendered_Blush(theColor, sRendered_Mat(sTranslate3Z<Rat>(20), finderHider)));

	return result;
	}

} // namespace Util
} // namespace GameEngine
} // namespace ZooLib
