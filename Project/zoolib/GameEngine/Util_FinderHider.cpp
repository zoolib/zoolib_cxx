#include "zoolib/GameEngine/Util_FinderHider.h"

namespace ZooLib {
namespace GameEngine {
namespace Util {

// =================================================================================================
// MARK: - sFinderHider

ZRef<Rendered> sFinderHider(
	const ZRef<Rendered>& iRendered,
	GPoint iBackingSize,
	GPoint iGameSize)
	{
	ZRef<Rendered_Group> finderHider = sRendered_Group();

	const Rat iWidth = X(iGameSize);
	const Rat iHeight = Y(iGameSize);

	// Left
	finderHider->Append(new Rendered_Rect(
		ZRGBA::sBlack, sRect<GRect>(-1000, -1000, 0, iHeight+1000)));

	// Top
	finderHider->Append(new Rendered_Rect(
		ZRGBA::sBlack, sRect<GRect>(-1000, -1000, iWidth+1000, 0)));

	// Right
	finderHider->Append(new Rendered_Rect(
		ZRGBA::sBlack, sRect<GRect>(iWidth, -1000, iWidth+1000, iHeight+1000)));

	// Bottom
	finderHider->Append(new Rendered_Rect(
		ZRGBA::sBlack, sRect<GRect>(-1000, iHeight, iWidth+1000, iHeight+1000)));
	
	ZRef<Rendered_Group> theGroup = sRendered_Group();
	theGroup->Append(iRendered);

	// Bring it fairly far forward, but well behind frontmost (which is at 128)
	theGroup->Append(sRendered_AlphaGainMat(sTranslate3Z<Rat>(20), finderHider));

	return theGroup;
	}

} // namespace Util
} // namespace GameEngine
} // namespace ZooLib
