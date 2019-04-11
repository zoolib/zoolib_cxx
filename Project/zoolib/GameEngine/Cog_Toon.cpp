#include "zoolib/GameEngine/Cog_Toon.h"

namespace ZooLib {
namespace GameEngine {

// =================================================================================================
// MARK: - Toon cogs

class Callable_Cog_Toon
:	public Cog::Callable
	{
public:
	const ZRef<Toon> fToon;

	Callable_Cog_Toon(const ZRef<Toon>& iToon)
	:	fToon(iToon)
		{}

	virtual ZQ<Cog> QCall(const Cog& iSelf, const Param& iParam)
		{
		ZRef<AssetCatalog> theAssetCatalog = iParam.fOutChannel.fAssetCatalog;
		if (NotQ<CelStack> theCelStackQ = fToon->QValAt(iParam.fElapsed))
			{ return true; }
		else
			{
			const ZRef<Rendered_Group>& theGroup = iParam.fOutChannel.GetGroup();
			foreachv (const Cel& theCel, (*theCelStackQ)->Get())
				{
				theAssetCatalog->Load(theCel.fNameFrame, 0);
				theGroup->Append(new Rendered_Cel(theCel));
				}
			}

		if (ZQ<CelStack> theCelStackQ = fToon->QValAt(iParam.fSubsequentTime))
			{
			foreachv (const Cel& theCel, (*theCelStackQ)->Get())
				theAssetCatalog->Load(theCel.fNameFrame, 1);
			}

		return iSelf;
		}
	};

Cog sCog_Toon_NoEpoch(const ZRef<Toon>& iToon)
	{
	if (not iToon)
		return null;
	
	return new Callable_Cog_Toon(iToon);
	}

Cog sCog_Toon(const ZRef<Toon>& iToon)
	{ return sCog_NewEpoch(sCog_Toon_NoEpoch(iToon)); }

// =================================================================================================
// MARK: - CogCtor_Toon

static
Cog spCogCtor_Toon(const Map& iMap)
	{
	ZRef<Toon> theToon = sToon(sQGetNamed(iMap, "Toon"));
	if (not theToon)
		theToon = sToon(iMap);
	
	if (Cog theCog = sCog_Toon_NoEpoch(theToon))
		{
		if (iMap.DGet<bool>(true, "NewEpoch"))
			theCog = sCog_NewEpoch(theCog);

		if (iMap.Get<bool>("Repeat"))
			theCog = sCog_Repeat(theCog);

		return theCog;
		}
	return null;
	}

static
CogRegistration spCogRegistrationToon("CogCtor_Toon", spCogCtor_Toon);

} // namespace GameEngine
} // namespace ZooLib
