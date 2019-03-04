#include "zoolib/GameEngine/Cog_Buffer.h"

#include "zoolib/Compat_algorithm.h" // SaveSetRestore

#include "zoolib/GameEngine/Rendered.h"

namespace ZooLib {
namespace GameEngine {

// =================================================================================================
// MARK: - sCog_Buffer

static
Cog spCogFun_Buffer(const Cog& iSelf, const Param& iParam,
	int iWidth, int iHeight, const ZRGBA& iRGBA, const Cog& iChild)
	{
	ZAssert(sIsPending(iChild));

	SaveSetRestore<ZRef<Rendered_Group> > theSR(
		iParam.fOutChannel.GetGroup(), sRendered_Group());

	const ZQ<Cog> newChildQ =
		iChild->QCall(iChild, iParam);

	theSR.Prior()->Append(sRendered_Buffer(iWidth, iHeight, iRGBA, theSR.Current()));

	if (newChildQ)
		{
		if (*newChildQ == iChild)
			return iSelf;
		
		return sCog_Buffer(iWidth, iHeight, iRGBA, *newChildQ);
		}
	return false;
	}

GEMACRO_Callable(spCallable_Buffer, spCogFun_Buffer);

static
Cog spCogCtor_Buffer(const Map& iMap)
	{
	int theWidth = sRat(iMap["Width"]);
	int theHeight = sRat(iMap["Height"]);
	ZRGBA theRGBA = sDGet(ZRGBA(0.0f, 0.0f), sQRGBA(iMap["Color"]));
	Cog theCog = sCog(iMap["Cog"]);
	return sCog_Buffer(theWidth, theHeight, theRGBA, theCog);
	}

Cog sCog_Buffer(int iWidth, int iHeight, const ZRGBA& iRGBA, const Cog& iChild)
	{
	if (sIsFinished(iChild))
		return iChild;

	return sBindR(spCallable_Buffer, iWidth, iHeight, iRGBA, iChild);
	}

static
CogRegistration spCogRegistration_Buffer("CogCtor_Buffer", spCogCtor_Buffer);

// =================================================================================================
// MARK: - sCog_Buffer_Continue

namespace { // anonymous

Cog spCog_Buffer_Continue(
	const ZRef<Tween_AlphaGainMat>& iTween,
	int iWidth, int iHeight, const ZRGBA& iRGBA, const Cog& iChild,
	double iStartTime);

Cog spCogFun_Buffer_Continue(const Cog& iSelf, const Param& iParam,
	const ZRef<Tween_AlphaGainMat>& iTween,
	int iWidth, int iHeight, const ZRGBA& iRGBA, const Cog& iChild,
	double iStartTime)
	{
	ZAssert(sIsPending(iChild));

	if (ZQ<AlphaGainMat,0> theQ = iTween->QValAt(iParam.fElapsed - iStartTime))
		{
		return sCallCog(iChild, iParam);
		}
	else
		{
		SaveSetRestore<ZRef<Rendered_Group> > theSR(
			iParam.fOutChannel.GetGroup(), sRendered_Group());

		Cog newChild = iChild;
		const bool unchanged = sCallUpdate_Cog_Unchanged(
			newChild, Param(iParam, InChannel(iParam.fInChannel, theQ->fMat)));

		theSR.Prior()->Append(
			sRendered_AlphaGainMat(*theQ, sRendered_Buffer(
				iWidth, iHeight, iRGBA, theSR.Current())));

		if (unchanged)
			return iSelf;

		if (sIsFinished(newChild))
			return newChild;

		return spCog_Buffer_Continue(iTween, iWidth, iHeight, iRGBA, newChild, iStartTime);
		}
	}

GEMACRO_Callable(spCallable_Buffer_Continue, spCogFun_Buffer_Continue);

Cog spCog_Buffer_Continue(
	const ZRef<Tween_AlphaGainMat>& iTween,
	int iWidth, int iHeight, const ZRGBA& iRGBA, const Cog& iChild,
	double iStartTime)
	{
	ZAssert(sIsPending(iChild) && iTween);

	return sBindR(spCallable_Buffer_Continue, iTween, iWidth, iHeight, iRGBA, iChild, iStartTime);
	}

Cog spCogFun_Buffer_Continue_Init(const Cog& iSelf, const Param& iParam,
	const ZRef<Tween_AlphaGainMat>& iTween, int iWidth, int iHeight, const ZRGBA& iRGBA,
	const Cog& iChild)
	{
	return sCallCog(
		spCog_Buffer_Continue(iTween, iWidth, iHeight, iRGBA, iChild, iParam.fElapsed),
		iParam);
	}

GEMACRO_Callable(spCallable_Buffer_Continue_Init, spCogFun_Buffer_Continue_Init);

} // anonymous namespace

Cog sCog_Buffer_Continue(const ZRef<Tween_AlphaGainMat>& iTween,
	int iWidth, int iHeight, const ZRGBA& iRGBA, const Cog& iChild)
	{
	if (sIsFinished(iChild) || not iTween)
		return iChild;

	return sBindR(spCallable_Buffer_Continue_Init, iTween, iWidth, iHeight, iRGBA, iChild);
	}

} // namespace GameEngine
} // namespace ZooLib
