#include "zoolib/GameEngine/Cog_Random.h"

namespace ZooLib {
namespace GameEngine {

// =================================================================================================
#pragma mark -

static
Cog spCogFun_Random(const Cog& iSelf, const Param& iParam,
	double iRatio, const Cog& iCog0, const Cog& iCog1)
	{
	if (sRandomInRange(0, 1) <= iRatio)
		return sCallCog(iCog0, iParam);
	else
		return sCallCog(iCog1, iParam);
	}

Cog sCog_Random(double iRatio, const Cog& iCog0, const Cog& iCog1)
	{
	GEMACRO_Callable(spCallable, spCogFun_Random);
	return sBindR(spCallable, iRatio, iCog0, iCog1);
	}

static
Cog spCogCtor_Random(const Map& iMap)
	{
	double ratio = sDRat(1, iMap.Get("Ratio"));
	Cog cog0 = sCog(iMap.Get("Cog0"));
	Cog cog1 = sCog(iMap.Get("Cog1"));
	return sCog_Random(ratio, cog0, cog1);
	}

static
CogRegistration spCogRegistration_Random("CogCtor_Random", spCogCtor_Random);

} // namespace GameEngine
} // namespace ZooLib
