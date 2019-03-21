#include "zoolib/GameEngine/Cog_Delay.h"

namespace ZooLib {
namespace GameEngine {

// =================================================================================================
// MARK: -

static
Cog spCogFun_DelayUntil(const Cog& iSelf, const Param& iParam,
	double iElapsed)
	{
	if (iParam.fElapsed >= iElapsed)
		return true;
	return iSelf;
	}

Cog sCog_DelayUntil(double iElapsed)
	{
	GEMACRO_Callable(spCallable, spCogFun_DelayUntil);
	return sBindR(spCallable, iElapsed);
	}

static
Cog spCogFun_DelayFor(const Cog& iSelf, const Param& iParam,
	double iDelay)
	{ return sCallCog(sCog_DelayUntil(iParam.fElapsed + iDelay), iParam); }

Cog sCog_DelayFor(double iDelay)
	{
	GEMACRO_Callable(spCallable, spCogFun_DelayFor);
	return sBindR(spCallable, iDelay);
	}

Cog sCog_StartAt(double iElapsed, const Cog& iCog)
	{ return sCog_DelayUntil(iElapsed) ^ iCog; }

Cog sCog_StopAt(double iElapsed, const Cog& iCog)
	{ return sCog_DelayUntil(iElapsed) % iCog; }

Cog sCog_StartAfter(double iDelay, const Cog& iCog)
	{ return sCog_DelayFor(iDelay) ^ iCog; }

Cog sCog_StopAfter(double iDelay, const Cog& iCog)
	{ return sCog_DelayFor(iDelay) % iCog; }

static
Cog spCogCtor_Delay(const Map& iMap)
	{ return sCog_DelayFor(sRat(iMap.QGet("Delay"))); }

static
CogRegistration spCogRegistration_Delay("CogCtor_Delay", spCogCtor_Delay);

} // namespace GameEngine
} // namespace ZooLib
