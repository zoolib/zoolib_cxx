#ifndef __GameEngine_Util_AssetCatalog_h__
#define __GameEngine_Util_AssetCatalog_h__ 1
#include "zconfig.h"

#include "zoolib/GameEngine/AssetCatalog.h"

namespace ZooLib {
namespace GameEngine {

// =================================================================================================
#pragma mark -

void sPopulate(
	const ZP<AssetCatalog>& iAC,
	const FileSpec& iRoot,
	const ZP<Callable_TextureFromPixmap>& iTFP,
	bool iPreferProcessedArt, bool iPreferSmallArt);

} // namespace GameEngine
} // namespace ZooLib

#endif // __GameEngine_Util_AssetCatalog_h__
