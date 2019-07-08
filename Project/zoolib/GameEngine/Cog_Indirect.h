// Copyright (c) 2019 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_GameEngine_Cog_Indirect_h__
#define __ZooLib_GameEngine_Cog_Indirect_h__ 1

#include "zoolib/GameEngine/Cog.h"
#include "zoolib/GameEngine/Types.h"

namespace ZooLib {
namespace GameEngine {

// =================================================================================================
#pragma mark - CogIndirect

void sSetIndirect(const Cog& iLHS, const Cog& iRHS);

Cog sCog_Indirect();
Cog sCog_Indirect(Seq& ioSeq);

Cog sCog_KillIndirects(SharedState_Mutable& ioState, const string8& iName);

} // namespace GameEngine
} // namespace ZooLib

#endif // __ZooLib_GameEngine_Cog_Indirect_h__
