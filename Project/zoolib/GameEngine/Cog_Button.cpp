#include "zoolib/GameEngine/Cog_Button.h"
#include "zoolib/GameEngine/PitchAndCatch.h"

#include "zoolib/GameEngine/CoT.h"
#include "zoolib/GameEngine/Util.h"

namespace ZooLib {
namespace GameEngine {

static
Cog spCogFun_TouchIn(const Cog& iSelf, const Param& iParam,
	const ZP<TouchListener>& iTouchListener)
	{ return sTouchIn(iTouchListener); }

static
Cog spCog_TouchIn(const ZP<TouchListener>& iTouchListener)
	{
	GEMACRO_Callable(spCallable, spCogFun_TouchIn);	
	return sBindR(spCallable, iTouchListener);
	}

static
Cog spCogFun_TouchOut(const Cog& iSelf, const Param& iParam,
	const ZP<TouchListener>& iTouchListener)
	{ return sTouchOut(iTouchListener); }

static
Cog spCog_TouchOut(const ZP<TouchListener>& iTouchListener)
	{
	GEMACRO_Callable(spCallable, spCogFun_TouchOut);	
	return sBindR(spCallable, iTouchListener);
	}

static
Cog spCogFun_TouchUp(const Cog& iSelf, const Param& iParam,
	const ZP<TouchListener>& iTouchListener)
	{ return sTouchUp(iTouchListener); }

static
Cog spCog_TouchUp(const ZP<TouchListener>& iTouchListener)
	{
	GEMACRO_Callable(spCallable, spCogFun_TouchUp);	
	return sBindR(spCallable, iTouchListener);
	}

// =================================================================================================
#pragma mark -

Cog sCog_Button(
	const ZP<TouchListener>& iTouchListener,
	const Cog& iCog_UpOut,
	const Cog& iCog_DownIn,
	const Cog& iCog_DownOut,
	const Cog& iCog_Pushed)
	{
	const Cog theCog_DownOut = iCog_DownOut ? iCog_DownOut : iCog_UpOut;

	const Cog cog_TouchNothing = sCog_TouchNothing(iTouchListener);
	const Cog cog_TouchDown = sCog_TouchDown(iTouchListener);
	const Cog cog_TouchIn = spCog_TouchIn(iTouchListener);
	const Cog cog_TouchOut = spCog_TouchOut(iTouchListener);
	const Cog cog_TouchUp = spCog_TouchUp(iTouchListener);

	Cog aCog;

	aCog ^= sCog_Repeat(cog_TouchNothing) % iCog_UpOut;

	aCog ^= cog_TouchDown >> sCog_Repeat(cog_TouchIn) % iCog_DownIn;

	aCog ^= sCog_Repeat((sCog_Repeat(cog_TouchOut) % theCog_DownOut) ^ cog_TouchIn >> +(sCog_Repeat(cog_TouchIn) % iCog_DownIn));

	aCog ^= cog_TouchUp >> iCog_Pushed;

	return sCog_Repeat(+aCog) & sCog_Repeat(sCog_UpdateTouchListener(iTouchListener));
	}

static
Cog spCogCtor_Button(const Map& iMap)
	{	
	const Cog theCog_UpOut = sCog_Repeat(sCoT(iMap.QGet("CoT_UpOut")));
	const Cog theCog_DownIn = sCog_Repeat(sCoT(iMap.QGet("CoT_DownIn")));
	const Cog theCog_DownOut = sCog_Repeat(sCoT(iMap.QGet("CoT_DownOut")));

// There is currently no "UpIn" behavior in a touch interface. To handle mouse-style cursor
// highlighting we'll need a concept of a 'non-touching' touch of some sort.

	ZP<TouchListener> theTouchListener = new TouchListener(true);
	theTouchListener->SetBounds(sDGet(sGRect(10,10), sQGRect(iMap["HotRect"])));

	const Cog theCog_Pushed = sCog_Pitch(iMap.Get<Map>("Pushed"));

	return sCog_Button(theTouchListener, theCog_UpOut, theCog_DownIn, theCog_DownOut, theCog_Pushed);
	}

static
CogRegistration spCogRegistration_Button("CogCtor_Button", spCogCtor_Button);

} // namespace GameEngine
} // namespace ZooLib
