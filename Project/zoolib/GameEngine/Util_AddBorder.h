#ifndef __ZooLib_GameEngine_Util_AddBorder_h__
#define __ZooLib_GameEngine_Util_AddBorder_h__ 1
#include "zconfig.h"

#include "zoolib/GameEngine/Rendered.h"

namespace ZooLib {
namespace GameEngine {
namespace Util {

// =================================================================================================
// MARK: - sAddBorder

ZRef<Rendered> sAddBorder(
	const ZRef<Rendered>& iRendered,
	GPoint iBackingSize,
	GPoint iGameSize);

} // namespace Util
} // namespace GameEngine
} // namespace ZooLib

#endif // __ZooLib_GameEngine_Util_AddBorder_h__
