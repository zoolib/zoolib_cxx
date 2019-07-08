// Copyright (c) 2019 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_GameEngine_Cog_Distribute_h__
#define __ZooLib_GameEngine_Cog_Distribute_h__ 1

#include "zoolib/GameEngine/Cog.h"

namespace ZooLib {
namespace GameEngine {

// =================================================================================================
#pragma mark -

Cog sCog_Distribute(CVec3 iSpacing, const std::vector<Cog>& iCogs);

} // namespace GameEngine
} // namespace ZooLib

#endif // __ZooLib_GameEngine_Cog_Distribute_h__
