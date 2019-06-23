#include "zoolib/GameEngine/PitchAndCatch.h"

namespace ZooLib {
namespace GameEngine {

using std::set;
using std::vector;

// =================================================================================================
#pragma mark - Nook_Catcher

Nook_Catcher::Nook_Catcher(const ZP<NookScope>& iNookScope, const Name& iName)
:	Nook(iNookScope)
,	fEra(0)
,	fName(iName)
	{}

const Name& Nook_Catcher::GetName()
	{ return fName; }

void Nook_Catcher::Catch(const Val& iVal)
	{ fPitches_New.push_back(iVal); }

const vector<Val>& Nook_Catcher::GetPitches(uint64 iEra)
	{
	if (sQSet(fEra, iEra))
		{
		swap(fPitches_New, fPitches_Old);
		fPitches_New.clear();
		}
	return fPitches_Old;
	}

const set<string8> Nook_Catcher::GetStringPitches(uint64 iEra)
	{
	set<string8> result;
	foreachv (const Val& thePitch, this->GetPitches(iEra))
		{
		if (const string8* theString8P = thePitch.PGet<string8>())
			Util_STL::sInsert(result, *theString8P);
		}
	return result;
	}

void sPitch(const Param& iParam, const Name& iName, const Val& iVal)
	{
	foreachv (const ZP<Nook_Catcher>& theNook, sGetAllNooks<Nook_Catcher>(iParam))
		{
		if (iName.IsEmpty() || iName == theNook->GetName())
			theNook->Catch(iVal);
		}
	}

const vector<Val>& sGetPitches(const Param& iParam, const Name& iName)
	{
	foreachv (const ZP<Nook_Catcher>& theNook, sGetAllNooks<Nook_Catcher>(iParam))
		{
		if (iName.IsEmpty() || iName == theNook->GetName())
			return theNook->GetPitches(iParam.GetEra());
		}

	return sDefault();
	}

// =================================================================================================
#pragma mark - Cog_Pitch

static
Cog spCogFun(const Cog& iSelf, const Param& iParam,
	const Map& iMap)
	{
	const Val* thePitchP = iMap.PGet("Pitch");
	if (not thePitchP)
		thePitchP = iMap.PGet("What");
		
	if (thePitchP)
		{
		Name theCatcherName;
		if (const string8* theCatcherNameP = iMap.PGet<string8>("Catcher"))
			theCatcherName = *theCatcherNameP;

		sPitch(iParam, theCatcherName, *thePitchP);
		}
	return false;
	}

Cog sCog_Pitch(const Map& iMap)
	{
	GEMACRO_Callable(spCallable, spCogFun);
	return sBindR(spCallable, iMap);
	}

static
CogRegistration sCogRegistration("CogCtor_Pitch", sCog_Pitch);

} // namespace GameEngine
} // namespace ZooLib
