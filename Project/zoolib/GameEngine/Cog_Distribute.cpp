#include "zoolib/GameEngine/Cog_Distribute.h"
#include "zoolib/GameEngine/Cog_Group.h"
#include "zoolib/GameEngine/CoT.h"
#include "zoolib/GameEngine/Geometry.h"

#include "zoolib/PairwiseCombiner_T.h"

namespace ZooLib {
namespace GameEngine {

using std::vector;

// =================================================================================================
// MARK: -

Cog sCog_Distribute(CVec3 iSpacing, const vector<Cog>& iCogs)
	{
	PairwiseCombiner_T<Cog, CogAccumulatorCombiner_Each, std::list<Cog> > theAcc;

	for (size_t ii = 0; ii < iCogs.size(); ++ii)
		{
		Cog theCog = sCog_Group_WithBlushGainMat(sTranslate(iSpacing * ii), iCogs[ii]);
		theAcc.Include(theCog);
		}

	return theAcc.Get();
	}

static
Cog spCogCtor_Distribute(const Map& iMap)
	{
	const CVec3 theSpacing = sCVec3(0, iMap["Spacing"]);

	const Seq theSeq = iMap.Get<Seq>("Cogs");

	PairwiseCombiner_T<Cog, CogAccumulatorCombiner_Each, std::list<Cog> > theAcc;

	for (size_t ii = 0; ii < theSeq.Count(); ++ii)
		{
		Cog theCog = sCog_Group_WithBlushGainMat(sTranslate(theSpacing * ii), sCoT(theSeq.QGet(ii)));
		theAcc.Include(theCog);
		}

	return theAcc.Get();
	}

CogRegistration spCogRegistration_Distribute("CogCtor_Distribute", spCogCtor_Distribute);

} // anonymous namespace
} // namespace GlassCeiling
