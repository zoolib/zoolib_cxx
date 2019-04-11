#include "zoolib/GameEngine/Cog_Group.h"
#include "zoolib/GameEngine/CoT.h"

#include "zoolib/Compat_algorithm.h" // SaveSetRestore

namespace ZooLib {
namespace GameEngine {

// =================================================================================================
#pragma mark - sCog_Group_WithState

class Callable_Cog_Group_WithState
:	public Cog::Callable
	{
public:
	const SharedState fState;
	const Cog fChild;

	Callable_Cog_Group_WithState(const SharedState& iState, const Cog& iChild)
	:	fState(iState)
	,	fChild(iChild)
		{}

	virtual ZQ<Cog> QCall(const Cog& iSelf, const Param& iParam)
		{
		ZAssert(sIsPending(fChild));

		Mat priorMat = sTranslate(sCVec3(0, fState.QGet("Pos")));
		if (fState.Get<bool>("Flipped"))
			priorMat *= sScale3X<Rat>(-1);

		SaveSetRestore<ZRef<Rendered_Group> > theSSR(
			iParam.fOutChannel.GetGroup(), sRendered_Group());

		const ZQ<Cog> newChildQ = 
			fChild->QCall(fChild, Param(iParam, InChannel(iParam.fInChannel, priorMat)));

		AlphaGainMat theAGM;
		
		if (ZQ<Alpha> theAlphaQ = fState.QGet<Alpha>("Alpha"))
			theAGM *= *theAlphaQ;

		if (ZQ<Gain> theGainQ = sQRat(fState.QGet("Gain")))
			theAGM *= *theGainQ;

		theAGM *= sTranslate(sCVec3(0, fState.QGet("Pos")));

		if (fState.Get<bool>("Flipped"))
			theAGM *= sScale3X<Rat>(-1);

		theSSR.Prior()->Append(sRendered_AlphaGainMat(theAGM, theSSR.Current()));
		
		if (newChildQ)
			{
			const Cog& newChild = *newChildQ;

			if (newChild == fChild)
				return iSelf;

			if (sIsFinished(newChild))
				return newChild;

			return new Callable_Cog_Group_WithState(fState, *newChildQ);
			}
		return false;
		}
	};

Cog sCog_Group_WithState(const SharedState& iState, const Cog& iChild)
	{
	if (sIsFinished(iChild))
		return iChild;

	return new Callable_Cog_Group_WithState(iState, iChild);
	}

// =================================================================================================
#pragma mark -

namespace { // anonymous

Cog spCogCtor_Group(const Map& iMap)
	{
	Cog theCog = sCoT(sQGetNamed(iMap, "Cog", "CoT"));
	
	if (not theCog)
		theCog = sCog_Repeat(sCoT(sQGetNamed(iMap, "CogRepeated")));

	if (theCog)
		{
		ZRef<Tween<AlphaGainMat> > theAGM =
			sTween<AlphaGainMat>(sQGetNamed(iMap, "AlphaGainMat", "AGM"));

		if (not theAGM)
			theAGM = sTween_AlphaGainMat(iMap);

		ZRef<Tween<Mat> > theScale = sTween_Const<Mat>(sScale3Z<Rat>(0.5));
		if (theAGM)
			theAGM *= theScale;
		else
			theAGM = sTween_AlphaGainMat(theScale);

		if (const string8* theModeP = iMap.PGet<string8>("Mode"))
			{
			if ("Terminate" == *theModeP)
				return sCog_Group_Terminate(theAGM, theCog);
			if ("Continue" == *theModeP)
				return sCog_Group_Continue(theAGM, theCog);
			if ("Loop" == *theModeP)
				return sCog_Group_Loop(theAGM, theCog);
			}
		return sCog_Group_Loop(theAGM, theCog);
		}
	return null;
	}

CogRegistration spCogRegistration_Group("CogCtor_Group", spCogCtor_Group);

} // anonymous namespace

// =================================================================================================
#pragma mark - sCog_Group_WithAlphaGainMat

class Callable_Cog_Group_WithAlphaGainMat
:	public Cog::Callable
	{
public:
	const AlphaGainMat fAlphaGainMat;
	const Cog fChild;

	Callable_Cog_Group_WithAlphaGainMat(const AlphaGainMat& iAlphaGainMat, const Cog& iChild)
	:	fAlphaGainMat(iAlphaGainMat)
	,	fChild(iChild)
		{}

	virtual ZQ<Cog> QCall(const Cog& iSelf, const Param& iParam)
		{
		ZAssert(sIsPending(fChild));

		SaveSetRestore<ZRef<Rendered_Group> > theSSR(
			iParam.fOutChannel.GetGroup(), sRendered_Group());

		const ZQ<Cog> newChildQ = 
			fChild->QCall(fChild, Param(iParam, InChannel(iParam.fInChannel, fAlphaGainMat.fMat)));

		theSSR.Prior()->Append(sRendered_AlphaGainMat(fAlphaGainMat, theSSR.Current()));

		if (newChildQ)
			{
			if (*newChildQ == fChild)
				return iSelf;
			
			return sCog_Group_WithAlphaGainMat(fAlphaGainMat, *newChildQ);
			}
		return false;
		}
	};

Cog sCog_Group_WithAlphaGainMat(const AlphaGainMat& iAlphaGainMat, const Cog& iChild)
	{
	if (sIsFinished(iChild))
		return iChild;

	return new Callable_Cog_Group_WithAlphaGainMat(iAlphaGainMat, iChild);
	}

// =================================================================================================
#pragma mark - sCog_Group_Loop

class Callable_Cog_Group_Loop
:	public Cog::Callable
	{
public:
	const ZRef<Tween_AlphaGainMat> fTween;
	const Cog fChild;
	const double fStartTime;

	Callable_Cog_Group_Loop(const ZRef<Tween_AlphaGainMat>& iTween, const Cog& iChild, double iStartTime)
	:	fTween(iTween)
	,	fChild(iChild)
	,	fStartTime(iStartTime)
		{}

	virtual ZQ<Cog> QCall(const Cog& iSelf, const Param& iParam)
		{
		ZAssert(sIsPending(fChild));

		SaveSetRestore<ZRef<Rendered_Group> > theSSR(
			iParam.fOutChannel.GetGroup(), sRendered_Group());

		const AlphaGainMat theAM = fTween->ValAtWrapped(iParam.fElapsed - fStartTime);

		const ZQ<Cog> newChildQ =
			fChild->QCall(fChild, Param(iParam, InChannel(iParam.fInChannel, theAM.fMat)));

		theSSR.Prior()->Append(sRendered_AlphaGainMat(theAM, theSSR.Current()));
		
		if (newChildQ)
			{
			const Cog& newChild = *newChildQ;

			if (newChild == fChild)
				return iSelf;

			if (sIsFinished(newChild))
				return newChild;

			return new Callable_Cog_Group_Loop(fTween, *newChildQ, fStartTime);
			}
		return false;
		}
	};

class Callable_Cog_Group_Loop_Init
:	public Cog::Callable
	{
public:
	const ZRef<Tween_AlphaGainMat> fTween;
	const Cog fChild;

	Callable_Cog_Group_Loop_Init(const ZRef<Tween_AlphaGainMat>& iTween, const Cog& iChild)
	:	fTween(iTween)
	,	fChild(iChild)
		{}

	virtual ZQ<Cog> QCall(const Cog& iSelf, const Param& iParam)
		{
		return sCallCog<const Param&>
			(new Callable_Cog_Group_Loop(fTween, fChild, iParam.fElapsed), iParam);
		}
	};

Cog sCog_Group_Loop(const ZRef<Tween_AlphaGainMat>& iTween, const Cog& iChild)
	{
	if (sIsFinished(iChild) || not iTween)
		return iChild;

	return new Callable_Cog_Group_Loop_Init(iTween, iChild);
	}

// =================================================================================================
#pragma mark - sCog_Group_Terminate

namespace { // anonymous

Cog spCog_Group_Terminate(
	const ZRef<Tween_AlphaGainMat>& iTween, const Cog& iChild, double iStartTime);

Cog spCogFun_Group_Terminate(const Cog& iSelf, const Param& iParam,
	const ZRef<Tween_AlphaGainMat>& iTween, const Cog& iChild, double iStartTime)
	{
	ZAssert(sIsPending(iChild));

	if (NotQ<AlphaGainMat> theQ = iTween->QValAt(iParam.fElapsed - iStartTime))
		{
		return true;
		}
	else
		{
		SaveSetRestore<ZRef<Rendered_Group> > theSSR(
			iParam.fOutChannel.GetGroup(), sRendered_Group());

		Cog newChild = iChild;
		const bool unchanged = sCallUpdate_Cog_Unchanged(
			newChild, Param(iParam, InChannel(iParam.fInChannel, theQ->fMat)));

		theSSR.Prior()->Append(sRendered_AlphaGainMat(*theQ, theSSR.Current()));

		if (unchanged)
			return iSelf;

		if (sIsFinished(newChild))
			return newChild;

		return spCog_Group_Terminate(iTween, newChild, iStartTime);
		}
	}

GEMACRO_Callable(spCallable_Group_Terminate, spCogFun_Group_Terminate);

Cog spCog_Group_Terminate(
	const ZRef<Tween_AlphaGainMat>& iTween, const Cog& iChild, double iStartTime)
	{
	ZAssert(sIsPending(iChild) && iTween);

	return sBindR(spCallable_Group_Terminate, iTween, iChild, iStartTime);
	}

Cog spCogFun_Group_Terminate_Init(const Cog& iSelf, const Param& iParam,
	const ZRef<Tween_AlphaGainMat>& iTween, const Cog& iChild)
	{ return sCallCog(spCog_Group_Terminate(iTween, iChild, iParam.fElapsed), iParam); }

GEMACRO_Callable(spCallable_Group_Terminate_Init, spCogFun_Group_Terminate_Init);

} // anonymous namespace

Cog sCog_Group_Terminate(const ZRef<Tween_AlphaGainMat>& iTween, const Cog& iChild)
	{
	if (sIsFinished(iChild) || not iTween)
		return true;

	return sBindR(spCallable_Group_Terminate_Init, iTween, iChild);
	}

// =================================================================================================
#pragma mark - sCog_Group_Continue

namespace { // anonymous

Cog spCog_Group_Continue(
	const ZRef<Tween_AlphaGainMat>& iTween, const Cog& iChild, double iStartTime);

Cog spCogFun_Group_Continue(const Cog& iSelf, const Param& iParam,
	const ZRef<Tween_AlphaGainMat>& iTween, const Cog& iChild, double iStartTime)
	{
	ZAssert(sIsPending(iChild));

	if (NotQ<AlphaGainMat> theQ = iTween->QValAt(iParam.fElapsed - iStartTime))
		{
		return sCallCog(iChild, iParam);
		}
	else
		{
		SaveSetRestore<ZRef<Rendered_Group> > theSSR(
			iParam.fOutChannel.GetGroup(), sRendered_Group());

		Cog newChild = iChild;
		const bool unchanged = sCallUpdate_Cog_Unchanged(
			newChild, Param(iParam, InChannel(iParam.fInChannel, theQ->fMat)));

		theSSR.Prior()->Append(sRendered_AlphaGainMat(*theQ, theSSR.Current()));

		if (unchanged)
			return iSelf;

		if (sIsFinished(newChild))
			return newChild;

		return spCog_Group_Continue(iTween, newChild, iStartTime);
		}
	}

GEMACRO_Callable(spCallable_Group_Continue, spCogFun_Group_Continue);

Cog spCog_Group_Continue(
	const ZRef<Tween_AlphaGainMat>& iTween, const Cog& iChild, double iStartTime)
	{
	ZAssert(sIsPending(iChild) && iTween);

	return sBindR(spCallable_Group_Continue, iTween, iChild, iStartTime);
	}

Cog spCogFun_Group_Continue_Init(const Cog& iSelf, const Param& iParam,
	const ZRef<Tween_AlphaGainMat>& iTween, const Cog& iChild)
	{ return sCallCog(spCog_Group_Continue(iTween, iChild, iParam.fElapsed), iParam); }

GEMACRO_Callable(spCallable_Group_Continue_Init, spCogFun_Group_Continue_Init);

} // anonymous namespace

Cog sCog_Group_Continue(const ZRef<Tween_AlphaGainMat>& iTween, const Cog& iChild)
	{
	if (sIsFinished(iChild) || not iTween)
		return iChild;

	return sBindR(spCallable_Group_Continue_Init, iTween, iChild);
	}

} // namespace GameEngine
} // namespace ZooLib
