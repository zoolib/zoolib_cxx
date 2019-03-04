#ifndef __GameEngine_Util_GrindAssets_h__
#define __GameEngine_Util_GrindAssets_h__ 1
#include "zconfig.h"

#include "zoolib/File.h"

#include "zoolib/GameEngine/Types.h"

#include <map>

namespace GameEngine {
namespace Util {

void sGrindArt(
	Rat iScale, int iBaseDim, int iDoublings,
	const FileSpec& iFS, const FileSpec& iFS_Sheets, Map_Any& ioMap);

void sGrindAnim(
	Rat iScale, int iBaseDim, int iDoublings,
	const FileSpec& iFS, const FileSpec& iFS_Sheets, Map_Any& ioMap);

} // namespace Util
} // namespace GameEngine

#endif // __GameEngine_Util_GrindAssets_h__
