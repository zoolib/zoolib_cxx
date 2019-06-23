#include "zoolib/GameEngine/Cog_Buffer.h"

#include "zoolib/Compat_algorithm.h" // SaveSetRestore

#include "zoolib/GameEngine/Cog_Group.h" // For sRendered_BlushGainMat
#include "zoolib/GameEngine/Rendered.h"

namespace ZooLib {
namespace GameEngine {

// =================================================================================================
#pragma mark - sCog_Buffer

static
Cog spCogFun_Buffer(const Cog& iSelf, const Param& iParam,
	int iWidth, int iHeight, const RGBA& iFill, const Cog& iChild)
	{
	ZAssert(sIsPending(iChild));

	SaveSetRestore<ZP<Rendered_Group> > theSSR(
		iParam.fOutChannel.GetGroup(), sRendered_Group());

	const ZQ<Cog> newChildQ =
		iChild->QCall(iChild, iParam);

	theSSR.Prior()->Append(sRendered_Buffer(iWidth, iHeight, iFill, theSSR.Current()));

	if (newChildQ)
		{
		if (*newChildQ == iChild)
			return iSelf;
		
		return sCog_Buffer(iWidth, iHeight, iFill, *newChildQ);
		}
	return false;
	}

GEMACRO_Callable(spCallable_Buffer, spCogFun_Buffer);

static
Cog spCogCtor_Buffer(const Map& iMap)
	{
	int theWidth = sRat(iMap["Width"]);
	int theHeight = sRat(iMap["Height"]);
	RGBA theRGBA = sDGet(sRGBA(0, 0), sQRGBA(iMap["Color"]));
	Cog theCog = sCog(iMap["Cog"]);
	return sCog_Buffer(theWidth, theHeight, theRGBA, theCog);
	}

Cog sCog_Buffer(int iWidth, int iHeight, const RGBA& iFill, const Cog& iChild)
	{
	if (sIsFinished(iChild))
		return iChild;

	return sBindR(spCallable_Buffer, iWidth, iHeight, iFill, iChild);
	}

static
CogRegistration spCogRegistration_Buffer("CogCtor_Buffer", spCogCtor_Buffer);

// =================================================================================================
#pragma mark - sCog_Buffer_Continue

namespace { // anonymous

Cog spCog_Buffer_Continue(const ZP<Tween_BlushGainMat>& iTween,
	int iWidth, int iHeight, const RGBA& iFill, const Cog& iChild,
	double iStartTime);

Cog spCogFun_Buffer_Continue(const Cog& iSelf, const Param& iParam,
	const ZP<Tween_BlushGainMat>& iTween,
	int iWidth, int iHeight, const RGBA& iFill, const Cog& iChild,
	double iStartTime)
	{
	ZAssert(sIsPending(iChild));

	if (NotQ<BlushGainMat> theQ = iTween->QValAt(iParam.fElapsed - iStartTime))
		{
		return sCallCog(iChild, iParam);
		}
	else
		{
		SaveSetRestore<ZP<Rendered_Group> > theSSR(
			iParam.fOutChannel.GetGroup(), sRendered_Group());

		Cog newChild = iChild;
		const bool unchanged = sCallUpdate_Cog_Unchanged(
			newChild, Param(iParam, InChannel(iParam.fInChannel, theQ->fMat)));

		theSSR.Prior()->Append(
			sRendered_BlushGainMat(*theQ, sRendered_Buffer(
				iWidth, iHeight, iFill, theSSR.Current())));

		if (unchanged)
			return iSelf;

		if (sIsFinished(newChild))
			return newChild;

		return spCog_Buffer_Continue(iTween, iWidth, iHeight, iFill, newChild, iStartTime);
		}
	}

GEMACRO_Callable(spCallable_Buffer_Continue, spCogFun_Buffer_Continue);

Cog spCog_Buffer_Continue(const ZP<Tween_BlushGainMat>& iTween,
	int iWidth, int iHeight, const RGBA& iFill, const Cog& iChild,
	double iStartTime)
	{
	ZAssert(sIsPending(iChild) && iTween);

	return sBindR(spCallable_Buffer_Continue, iTween, iWidth, iHeight, iFill, iChild, iStartTime);
	}

Cog spCogFun_Buffer_Continue_Init(const Cog& iSelf, const Param& iParam,
	const ZP<Tween_BlushGainMat>& iTween,
	int iWidth, int iHeight, const RGBA& iFill,
	const Cog& iChild)
	{
	return sCallCog(
		spCog_Buffer_Continue(iTween, iWidth, iHeight, iFill, iChild, iParam.fElapsed),
		iParam);
	}

GEMACRO_Callable(spCallable_Buffer_Continue_Init, spCogFun_Buffer_Continue_Init);

} // anonymous namespace

Cog sCog_Buffer_Continue(const ZP<Tween_BlushGainMat>& iTween,
	int iWidth, int iHeight, const RGBA& iFill, const Cog& iChild)
	{
	if (sIsFinished(iChild) || not iTween)
		return iChild;

	return sBindR(spCallable_Buffer_Continue_Init, iTween, iWidth, iHeight, iFill, iChild);
	}

} // namespace GameEngine
} // namespace ZooLib
