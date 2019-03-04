#include "zoolib/GameEngine/Cog_Sound.h"
#include "zoolib/GameEngine/Sound.h"

#include "zoolib/Log.h"

namespace ZooLib {
namespace GameEngine {

// =================================================================================================
// MARK: - Sound cogs

static
Cog spCogFun_Sound(const Cog& iSelf, const Param& iParam,
	const ZRef<Sound>& iSound)
	{
	if (iParam.fInterval > 0)
		{
		if (iSound->IsFinished())
			return true;
		iParam.fOutChannel.GetGroup()->Append(new Rendered_Sound(iSound));
		}
	return iSelf;
	}

static
Cog spCogFun_Init(const Cog& iSelf, const Param& iParam,
	const string8& iName, const string8& iScope, bool iDetached)
	{
	if (ZLOGF(w, eDebug+1))
		w << iName << ":" << iDetached;

	if (ZRef<SoundMeister> theSM = SoundMeister::sGet())
		{
		if (ZRef<Sound> theSound = theSM->MakeSound(iName))
			{
			theSound->fDetached = iDetached;
			theSound->fScope = iScope;
			Cog theCog = sBindR(sCallable(spCogFun_Sound), theSound);
			return sCallCog(theCog, iParam);
			}
		}

	return true;
	}

Cog sCog_Sound(const string8& iName)
	{ return sCog_Sound(iName, string8(), false); }

Cog sCog_Sound(const string8& iName, const string8& iScope, bool iDetached)
	{ return sBindR(sCallable(spCogFun_Init), iName, iScope, iDetached); }

static
Cog spCogCtor_Sound(const Map& iMap)
	{
	return sCog_Sound(
		iMap.Get<string8>("Name"), iMap.Get<string8>("Scope"), iMap.Get<bool>("Detached"));
	}

static
CogRegistration spCogRegistration_Sound("CogCtor_Sound", spCogCtor_Sound);

} // namespace GameEngine
} // namespace ZooLib
