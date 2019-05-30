#include "zoolib/GameEngine/Cog_Replicate.h"

#include "zoolib/GameEngine/Cog_Group.h"
#include "zoolib/GameEngine/CoT.h"
#include "zoolib/GameEngine/Geometry.h"

#include "zoolib/PairwiseCombiner_T.h"

namespace ZooLib {
namespace GameEngine {

// =================================================================================================
// MARK: -

Cog sCog_Replicate(CVec3 iSpacing, size_t iCount, Cog iCog)
	{
	PairwiseCombiner_T<Cog, CogAccumulatorCombiner_Plus, std::list<Cog> > theAcc;

	for (size_t ii = 0; ii < iCount; ++ii)
		{
		Cog theCog = sCog_Group_WithBlushGainMat(sTranslate(iSpacing * ii), iCog);
		theAcc.Include(theCog);
		}

	return theAcc.Get();
	}

static
Cog spCogCtor_Replicate(const Map& iMap)
	{
	const CVec3 theSpacing = sCVec3(0, iMap["Spacing"]);
	const Rat theCount = sRat(iMap["Count"]);
	const Cog theCog = sCoT(iMap["Cog"]);
	return sCog_Replicate(theSpacing, theCount, theCog);
	}

CogRegistration spCogRegistration_Replicate("CogCtor_Replicate", spCogCtor_Replicate);

} // anonymous namespace
} // namespace GlassCeiling
