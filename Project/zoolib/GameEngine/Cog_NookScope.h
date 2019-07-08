// Copyright (c) 2019 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_GameEngine_Cog_NookScope_h__
#define __ZooLib_GameEngine_Cog_NookScope_h__ 1

#include "zoolib/GameEngine/Cog.h"

namespace ZooLib {
namespace GameEngine {

Cog sCog_NookScope(const ZP<NookScope>& iNookScope, const Cog& iChild);

Cog sCog_NookScope(const Cog& iChild);

} // namespace GameEngine
} // namespace ZooLib

#endif // __ZooLib_GameEngine_Cog_NookScope_h__
