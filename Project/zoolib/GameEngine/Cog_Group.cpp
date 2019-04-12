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

		BlushGainMat theBGM;
		
		if (ZQ<Blush> theBlushQ = fState.QGet<Blush>("Blush"))
			theBGM *= *theBlushQ;

		if (ZQ<Gain> theGainQ = sQRat(fState.QGet("Gain")))
			theBGM *= *theGainQ;

		theBGM *= sTranslate(sCVec3(0, fState.QGet("Pos")));

		if (fState.Get<bool>("Flipped"))
			theBGM *= sScale3X<Rat>(-1);

		theSSR.Prior()->Append(sRendered_BlushGainMat(theBGM, theSSR.Current()));
		
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
		ZRef<Tween<BlushGainMat> > theBGM =
			sTween<BlushGainMat>(sQGetNamed(iMap, "BlushGainMat", "BGM"));

		if (not theBGM)
			theBGM = sTween_BlushGainMat(iMap);

		ZRef<Tween<Mat> > theScale = sTween_Const<Mat>(sScale3Z<Rat>(0.5));
		if (theBGM)
			theBGM *= theScale;
		else
			theBGM = sTween_BlushGainMat(theScale);

		if (const string8* theModeP = iMap.PGet<string8>("Mode"))
			{
			if ("Terminate" == *theModeP)
				return sCog_Group_Terminate(theBGM, theCog);
			if ("Continue" == *theModeP)
				return sCog_Group_Continue(theBGM, theCog);
			if ("Loop" == *theModeP)
				return sCog_Group_Loop(theBGM, theCog);
			}
		return sCog_Group_Loop(theBGM, theCog);
		}
	return null;
	}

CogRegistration spCogRegistration_Group("CogCtor_Group", spCogCtor_Group);

} // anonymous namespace

// =================================================================================================
#pragma mark - sCog_Group_WithBlushGainMat

class Callable_Cog_Group_WithBlushGainMat
:	public Cog::Callable
	{
public:
	const BlushGainMat fBlushGainMat;
	const Cog fChild;

	Callable_Cog_Group_WithBlushGainMat(const BlushGainMat& iBlushGainMat, const Cog& iChild)
	:	fBlushGainMat(iBlushGainMat)
	,	fChild(iChild)
		{}

	virtual ZQ<Cog> QCall(const Cog& iSelf, const Param& iParam)
		{
		ZAssert(sIsPending(fChild));

		SaveSetRestore<ZRef<Rendered_Group> > theSSR(
			iParam.fOutChannel.GetGroup(), sRendered_Group());

		const ZQ<Cog> newChildQ = 
			fChild->QCall(fChild, Param(iParam, InChannel(iParam.fInChannel, fBlushGainMat.fMat)));

		theSSR.Prior()->Append(sRendered_BlushGainMat(fBlushGainMat, theSSR.Current()));

		if (newChildQ)
			{
			if (*newChildQ == fChild)
				return iSelf;
			
			return sCog_Group_WithBlushGainMat(fBlushGainMat, *newChildQ);
			}
		return false;
		}
	};

Cog sCog_Group_WithBlushGainMat(const BlushGainMat& iBlushGainMat, const Cog& iChild)
	{
	if (sIsFinished(iChild))
		return iChild;

	return new Callable_Cog_Group_WithBlushGainMat(iBlushGainMat, iChild);
	}

// =================================================================================================
#pragma mark - sCog_Group_Loop

class Callable_Cog_Group_Loop
:	public Cog::Callable
	{
public:
	const ZRef<Tween_BlushGainMat> fTween;
	const Cog fChild;
	const double fStartTime;

	Callable_Cog_Group_Loop(const ZRef<Tween_BlushGainMat>& iTween, const Cog& iChild, double iStartTime)
	:	fTween(iTween)
	,	fChild(iChild)
	,	fStartTime(iStartTime)
		{}

	virtual ZQ<Cog> QCall(const Cog& iSelf, const Param& iParam)
		{
		ZAssert(sIsPending(fChild));

		SaveSetRestore<ZRef<Rendered_Group> > theSSR(
			iParam.fOutChannel.GetGroup(), sRendered_Group());

		const BlushGainMat theBGM = fTween->ValAtWrapped(iParam.fElapsed - fStartTime);

		const ZQ<Cog> newChildQ =
			fChild->QCall(fChild, Param(iParam, InChannel(iParam.fInChannel, theBGM.fMat)));

		theSSR.Prior()->Append(sRendered_BlushGainMat(theBGM, theSSR.Current()));
		
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
	const ZRef<Tween_BlushGainMat> fTween;
	const Cog fChild;

	Callable_Cog_Group_Loop_Init(const ZRef<Tween_BlushGainMat>& iTween, const Cog& iChild)
	:	fTween(iTween)
	,	fChild(iChild)
		{}

	virtual ZQ<Cog> QCall(const Cog& iSelf, const Param& iParam)
		{
		return sCallCog<const Param&>
			(new Callable_Cog_Group_Loop(fTween, fChild, iParam.fElapsed), iParam);
		}
	};

Cog sCog_Group_Loop(const ZRef<Tween_BlushGainMat>& iTween, const Cog& iChild)
	{
	if (sIsFinished(iChild) || not iTween)
		return iChild;

	return new Callable_Cog_Group_Loop_Init(iTween, iChild);
	}

// =================================================================================================
#pragma mark - sCog_Group_Terminate

namespace { // anonymous

Cog spCog_Group_Terminate(
	const ZRef<Tween_BlushGainMat>& iTween, const Cog& iChild, double iStartTime);

Cog spCogFun_Group_Terminate(const Cog& iSelf, const Param& iParam,
	const ZRef<Tween_BlushGainMat>& iTween, const Cog& iChild, double iStartTime)
	{
	ZAssert(sIsPending(iChild));

	if (NotQ<BlushGainMat> theQ = iTween->QValAt(iParam.fElapsed - iStartTime))
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

		theSSR.Prior()->Append(sRendered_BlushGainMat(*theQ, theSSR.Current()));

		if (unchanged)
			return iSelf;

		if (sIsFinished(newChild))
			return newChild;

		return spCog_Group_Terminate(iTween, newChild, iStartTime);
		}
	}

GEMACRO_Callable(spCallable_Group_Terminate, spCogFun_Group_Terminate);

Cog spCog_Group_Terminate(
	const ZRef<Tween_BlushGainMat>& iTween, const Cog& iChild, double iStartTime)
	{
	ZAssert(sIsPending(iChild) && iTween);

	return sBindR(spCallable_Group_Terminate, iTween, iChild, iStartTime);
	}

Cog spCogFun_Group_Terminate_Init(const Cog& iSelf, const Param& iParam,
	const ZRef<Tween_BlushGainMat>& iTween, const Cog& iChild)
	{ return sCallCog(spCog_Group_Terminate(iTween, iChild, iParam.fElapsed), iParam); }

GEMACRO_Callable(spCallable_Group_Terminate_Init, spCogFun_Group_Terminate_Init);

} // anonymous namespace

Cog sCog_Group_Terminate(const ZRef<Tween_BlushGainMat>& iTween, const Cog& iChild)
	{
	if (sIsFinished(iChild) || not iTween)
		return true;

	return sBindR(spCallable_Group_Terminate_Init, iTween, iChild);
	}

// =================================================================================================
#pragma mark - sCog_Group_Continue

namespace { // anonymous

Cog spCog_Group_Continue(
	const ZRef<Tween_BlushGainMat>& iTween, const Cog& iChild, double iStartTime);

Cog spCogFun_Group_Continue(const Cog& iSelf, const Param& iParam,
	const ZRef<Tween_BlushGainMat>& iTween, const Cog& iChild, double iStartTime)
	{
	ZAssert(sIsPending(iChild));

	if (NotQ<BlushGainMat> theQ = iTween->QValAt(iParam.fElapsed - iStartTime))
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

		theSSR.Prior()->Append(sRendered_BlushGainMat(*theQ, theSSR.Current()));

		if (unchanged)
			return iSelf;

		if (sIsFinished(newChild))
			return newChild;

		return spCog_Group_Continue(iTween, newChild, iStartTime);
		}
	}

GEMACRO_Callable(spCallable_Group_Continue, spCogFun_Group_Continue);

Cog spCog_Group_Continue(
	const ZRef<Tween_BlushGainMat>& iTween, const Cog& iChild, double iStartTime)
	{
	ZAssert(sIsPending(iChild) && iTween);

	return sBindR(spCallable_Group_Continue, iTween, iChild, iStartTime);
	}

Cog spCogFun_Group_Continue_Init(const Cog& iSelf, const Param& iParam,
	const ZRef<Tween_BlushGainMat>& iTween, const Cog& iChild)
	{ return sCallCog(spCog_Group_Continue(iTween, iChild, iParam.fElapsed), iParam); }

GEMACRO_Callable(spCallable_Group_Continue_Init, spCogFun_Group_Continue_Init);

} // anonymous namespace

Cog sCog_Group_Continue(const ZRef<Tween_BlushGainMat>& iTween, const Cog& iChild)
	{
	if (sIsFinished(iChild) || not iTween)
		return iChild;

	return sBindR(spCallable_Group_Continue_Init, iTween, iChild);
	}

} // namespace GameEngine
} // namespace ZooLib
