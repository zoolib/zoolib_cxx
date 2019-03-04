#include "zoolib/GameEngine/CoT.h"

#include "zoolib/GameEngine/Cog_Toon.h"

namespace ZooLib {
namespace GameEngine {

// =================================================================================================
// MARK: - sCoT

Cog sCoT(const ZQ<Val>& iVal)
	{
	if (Cog theCog = sCog(iVal))
		return theCog;

	return sCog_Toon(sToon(iVal));
	}

} // namespace GameEngine
} // namespace ZooLib
