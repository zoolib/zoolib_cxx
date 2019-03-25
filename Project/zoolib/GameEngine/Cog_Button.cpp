#include "zoolib/GameEngine/Cog_Button.h"
#include "zoolib/GameEngine/PitchAndCatch.h"

#include "zoolib/GameEngine/CoT.h"
#include "zoolib/GameEngine/Util.h"

namespace ZooLib {
namespace GameEngine {

static
Cog spCogFun_TouchIn(const Cog& iSelf, const Param& iParam,
	const ZRef<TouchListener>& iTouchListener)
	{
	if (iTouchListener->fActive.empty())
		return false;

	if (iTouchListener->fUps.size())
		return false;

	ZRef<Touch> theTouch = *iTouchListener->fActive.begin();
	const CVec3 localPos = iTouchListener->GetMat() * theTouch->fPos;
	if (sContains(iTouchListener->fBounds, localPos))
		return true;

	return false;
	}

static
Cog spCog_TouchIn(const ZRef<TouchListener>& iTouchListener)
	{
	GEMACRO_Callable(spCallable, spCogFun_TouchIn);	
	return sBindR(spCallable, iTouchListener);
	}

static
Cog spCogFun_TouchOut(const Cog& iSelf, const Param& iParam,
	const ZRef<TouchListener>& iTouchListener)
	{
	if (iTouchListener->fActive.empty())
		return false;

	ZRef<Touch> theTouch = *iTouchListener->fActive.begin();
	const CVec3 localPos = iTouchListener->GetMat() * theTouch->fPos;
	if (sContains(iTouchListener->fBounds, localPos))
		return false;

	return true;
	}

static
Cog spCog_TouchOut(const ZRef<TouchListener>& iTouchListener)
	{
	GEMACRO_Callable(spCallable, spCogFun_TouchOut);	
	return sBindR(spCallable, iTouchListener);
	}

static
Cog spCogFun_TouchUp(const Cog& iSelf, const Param& iParam,
	const ZRef<TouchListener>& iTouchListener)
	{
	if (iTouchListener->fUps.size())
		{
		ZRef<Touch> theTouch = *iTouchListener->fUps.begin();
		const CVec3 localPos = iTouchListener->GetMat() * theTouch->fPos;
		if (sContains(iTouchListener->fBounds, localPos))
			return true;
		}
	return false;
	}

static
Cog spCog_TouchUp(const ZRef<TouchListener>& iTouchListener)
	{
	GEMACRO_Callable(spCallable, spCogFun_TouchUp);	
	return sBindR(spCallable, iTouchListener);
	}

// =================================================================================================
// MARK: -

Cog sCog_Button(
	const GRect& iHotRect,
	const Cog& iCog_UpOut,
	const Cog& iCog_DownIn,
	const Cog& iCog_DownOut,
	const Cog& iCog_Pushed)
	{
	const Cog toon_DownOut = sCog_Fallback(iCog_DownOut, iCog_UpOut);

	ZRef<TouchListener> theTL = new TouchListener(true);
	theTL->fBounds = iHotRect;

	const Cog cog_TouchNothing = sCog_TouchNothing(theTL);
	const Cog cog_TouchDown = sCog_TouchDown(theTL);
	const Cog cog_TouchIn = spCog_TouchIn(theTL);
	const Cog cog_TouchOut = spCog_TouchOut(theTL);
	const Cog cog_TouchUp = spCog_TouchUp(theTL);

	Cog aCog;

	aCog ^= sCog_Repeat(cog_TouchNothing) % iCog_UpOut;

	aCog ^= cog_TouchDown >> sCog_Repeat(cog_TouchIn) % iCog_DownIn;

	aCog ^= sCog_Repeat((sCog_Repeat(cog_TouchOut) % iCog_DownOut) ^ cog_TouchIn >> +(sCog_Repeat(cog_TouchIn) % iCog_DownIn));

	aCog ^= cog_TouchUp >> iCog_Pushed;

	return sCog_Repeat(+aCog) & sCog_Repeat(sCog_UpdateTouchListener(theTL));
	}

static
Cog spCogCtor_Button(const Map& iMap)
	{	
	const Cog toon_UpOut = sCog_Repeat(sCoT(iMap.QGet("CoT_UpOut")));
	const Cog toon_DownIn = sCog_Repeat(sCoT(iMap.QGet("CoT_DownIn")));
	const Cog toon_DownOut = sCog_Repeat(sCoT(iMap.QGet("CoT_DownOut")));

// There is currently no "UpIn" behavior in a touch interface. To handle mouse-style cursor
// highlighting we'll need a concept of a 'non-touching' touch of some sort.

//	const Cog toon_UpIn = sCog_Fallback(sCog_Repeat(sCoT(iMap.QGet("CoT_UpIn"))), toon_UpOut);

	GRect theHotRect = sDGet(sGRect(10,10), sQGRect(iMap["HotRect"]));

	const Cog theCog_Pushed = sCog_Pitch(iMap.Get<Map>("Pushed"));

	return sCog_Button(theHotRect, toon_UpOut, toon_DownIn, toon_DownOut, theCog_Pushed);
	}

static
CogRegistration spCogRegistration_Button("CogCtor_Button", spCogCtor_Button);

} // namespace GameEngine
} // namespace ZooLib
