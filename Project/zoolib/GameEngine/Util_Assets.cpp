#include "zoolib/GameEngine/Util_Assets.h"
#include "zoolib/GameEngine/Util.h" // For sQReadMap_Any

#include "zoolib/Util_string.h"

namespace ZooLib {
namespace GameEngine {

using std::map;
using namespace Util_string;

// =================================================================================================
#pragma mark -

bool sReadAnim(const FileSpec& iFS, map<string8,FileSpec>& oFiles, Map_Any& oMap)
	{
	if (not iFS.IsDir())
		return false;

	for (FileIter iter = iFS; iter; iter.Advance())
		{
		const FileSpec current = iter.Current();
		if (ZQ<string8> woSuffix = sQWithoutSuffix(".png", current.Name()))
			oFiles[current.Name()] = current;
		}

	if (oFiles.empty())
		return false;

	oMap = sGet(sQReadMap_Any(iFS.Child("meta.txt")));

	return true;
	}

} // namespace GameEngine
} // namespace ZooLib
