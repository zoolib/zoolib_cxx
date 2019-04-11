#ifndef __GameEngine_Util_AssetCatalog_h__
#define __GameEngine_Util_AssetCatalog_h__ 1
#include "zconfig.h"

#include "zoolib/GameEngine/AssetCatalog.h"
#include "zoolib/GameEngine/FontCatalog.h"

namespace ZooLib {
namespace GameEngine {

// =================================================================================================
#pragma mark -

void sPopulate(
	const ZRef<AssetCatalog>& iAC,
	const ZRef<FontCatalog>& iFC,
	const FileSpec& iRoot,
	const ZRef<Callable_TextureFromPixmap>& iTFP,
	bool iPreferProcessedArt, bool iPreferSmallArt);

} // namespace GameEngine
} // namespace ZooLib

#endif // __GameEngine_Util_AssetCatalog_h__
