#ifndef __GameEngine_Util_AssetCatalog_h__
#define __GameEngine_Util_AssetCatalog_h__ 1
#include "zconfig.h"

#include "zoolib/GameEngine/AssetCatalog.h"

namespace ZooLib {
namespace GameEngine {

// =================================================================================================
// MARK: -

void sPopulate(
	const ZRef<AssetCatalog>& iAC,
	const FileSpec& iRoot,
	const ZRef<Callable_TextureFromPixmap>& iTFP,
	bool iPreferProcessedArt, bool iPreferSmallArt);

} // namespace GameEngine
} // namespace ZooLib

#endif // __GameEngine_Util_AssetCatalog_h__
