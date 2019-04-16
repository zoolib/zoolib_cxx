#include "zoolib/GameEngine/Util_FinderHider.h"

#include "zoolib/GameEngine/DebugFlags.h"

namespace ZooLib {
namespace GameEngine {
namespace Util {

// =================================================================================================
#pragma mark - sFinderHider

ZRef<Rendered> sFinderHider(
	const ZRef<Rendered>& iRendered,
	GPoint iBackingSize,
	GPoint iGameSize)
	{
	ZRef<Rendered_Group> finderHider = sRendered_Group();

	const Rat iWidth = X(iGameSize);
	const Rat iHeight = Y(iGameSize);

	RGBA theColor = sRGBA(0, 1);

	if (DebugFlags::sTouches)
		theColor  = sRGBA(0.25,1);

	// Left
	finderHider->Append(sRendered_Rect(
		theColor, sRect<GRect>(-1000, -1000, 0, iHeight+1000)));

	// Top
	finderHider->Append(sRendered_Rect(
		theColor, sRect<GRect>(-1000, -1000, iWidth+1000, 0)));

	// Right
	finderHider->Append(sRendered_Rect(
		theColor, sRect<GRect>(iWidth, -1000, iWidth+1000, iHeight+1000)));

	// Bottom
	finderHider->Append(sRendered_Rect(
		theColor, sRect<GRect>(-1000, iHeight, iWidth+1000, iHeight+1000)));
	
	ZRef<Rendered_Group> theGroup = sRendered_Group();
	theGroup->Append(iRendered);

	// Bring it fairly far forward, but well behind frontmost (which is at 128)
		theGroup->Append(sRendered_Mat(sTranslate3Z<Rat>(20), finderHider));

	return theGroup;
	}

} // namespace Util
} // namespace GameEngine
} // namespace ZooLib
