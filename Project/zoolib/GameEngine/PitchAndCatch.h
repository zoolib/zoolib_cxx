// Copyright (c) 2019 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_GameEngine_PitchAndCatch_h__
#define __ZooLib_GameEngine_PitchAndCatch_h__ 1
#include "zconfig.h"

#include "zoolib/GameEngine/Cog.h"

namespace ZooLib {
namespace GameEngine {

// =================================================================================================
#pragma mark - Nook_Catcher

class Nook_Catcher
:	public Nook
	{
public:
	Nook_Catcher(const ZP<NookScope>& iNookScope, const Name& iName);

// Our protocol
	const Name& GetName();

	void Catch(const Val& iVal);

	const std::vector<Val>& GetPitches(uint64 iEra);

	const std::set<string8> GetStringPitches(uint64 iEra);

private:
	uint64 fEra;
	Name fName;
	std::vector<Val> fPitches_New;
	std::vector<Val> fPitches_Old;
	};

void sPitch(const Param& iParam, const Name& iName, const Val& iVal);

const std::vector<Val>& sGetPitches(const Param& iParam, const Name& iName);

// =================================================================================================
#pragma mark - Cog_Pitch

Cog sCog_Pitch(const Map& iMap);

} // namespace GameEngine
} // namespace ZooLib

#endif // __ZooLib_GameEngine_PitchAndCatch_h__
