// Copyright (c) 2019 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/GameEngine/Cog_Toon.h"

namespace ZooLib {
namespace GameEngine {

// =================================================================================================
#pragma mark - Toon cogs

class Callable_Cog_Toon
:	public Cog::Callable
	{
public:
	const ZP<Toon> fToon;

	Callable_Cog_Toon(const ZP<Toon>& iToon)
	:	fToon(iToon)
		{}

	virtual ZQ<Cog> QCall(const Cog& iSelf, const Param& iParam)
		{
		ZP<AssetCatalog> theAssetCatalog = iParam.fOutChannel.fAssetCatalog;
		if (NotQ<CelStack> theCelStackQ = fToon->QValAt(iParam.fElapsed))
			{ return true; }
		else
			{
			const ZP<Rendered_Group>& theGroup = iParam.fOutChannel.GetGroup();
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

Cog sCog_Toon_NoEpoch(const ZP<Toon>& iToon)
	{
	if (not iToon)
		return null;
	
	return new Callable_Cog_Toon(iToon);
	}

Cog sCog_Toon(const ZP<Toon>& iToon)
	{ return sCog_NewEpoch(sCog_Toon_NoEpoch(iToon)); }

// =================================================================================================
#pragma mark - CogCtor_Toon

static
Cog spCogCtor_Toon(const Map& iMap)
	{
	ZP<Toon> theToon = sToon(sQGetNamed(iMap, "Toon"));
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
