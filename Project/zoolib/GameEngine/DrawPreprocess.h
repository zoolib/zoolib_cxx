#ifndef __ZooLib_GameEngine_Util_DrawPreprocess_h__
#define __ZooLib_GameEngine_Util_DrawPreprocess_h__ 1
#include "zconfig.h"

#include "zoolib/GameEngine/AssetCatalog.h"
#include "zoolib/GameEngine/FontCatalog.h"
#include "zoolib/GameEngine/Rendered.h"

namespace ZooLib {
namespace GameEngine {

ZRef<Rendered> sDrawPreprocess(const ZRef<Rendered>& iRendered,
	const ZRef<AssetCatalog>& iAssetCatalog, bool iShowNameFrame,
	const ZRef<FontCatalog>& iFontCatalog,
	const GPoint& iGameSize);

} // namespace GameEngine
} // namespace ZooLib

#endif // __ZooLib_GameEngine_Util_DrawPreprocess_h__
