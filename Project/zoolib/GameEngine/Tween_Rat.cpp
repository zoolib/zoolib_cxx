#include "zoolib/Singleton.h"
#include "zoolib/Util_STL_map.h"

#include "zoolib/GameEngine/Tween.h"
#include "zoolib/GameEngine/Tween_Rat.h"
#include "zoolib/GameEngine/Util.h"

#include "zoolib/ZMACRO_auto.h"

#include <map>

#define ZMACRO_RatRegister(a)\
static RatRegistration \
spRatRegistration_##a("RatCtor_"#a, spRatCtor_##a)

// =================================================================================================
#pragma mark - sCombineTweenVals

namespace ZooLib {

using GameEngine::Rat;

template <>
Rat sCombineTweenVals<Rat,Rat>(const Rat& iRat0, const Rat& iRat1)
	{ return iRat0 * iRat1; }

} // namespace ZooLib

namespace ZooLib {
namespace GameEngine {

using std::map;

// =================================================================================================
#pragma mark - sTween<Rat>

static ZP<Tween_Rat> spAugment(const Map& iMap, ZP<Tween_Rat> lTween_Rat)
	{
	if (lTween_Rat)
		{
		const Rat theZero = sDRat(0, iMap.QGet("Zero"));
		const Rat theOne = sDRat(1, iMap.QGet("One"));
		if (theZero != 0.0)
			{
			lTween_Rat = sTween_ValRange<Rat>(theZero, theOne, lTween_Rat);
			}
		else if (theOne != 1.0)
			{
			lTween_Rat = sTween_ValScale<Rat>(theOne, lTween_Rat);
			}
		}

	return lTween_Rat;
	}

template <>
ZP<Tween_Rat> sTween<Rat>(const ZQ<Val>& iValQ)
	{
	if (not iValQ)
		{
		return null;
		}
	else if (iValQ->IsNull())
		{
		return null;
		}
	if (ZQ<Rat> theQ = sQRat(*iValQ))
		{
		return sTween_Const(*theQ);
		}
	else if (ZQ<Seq> theSeqQ = iValQ->QGet<Seq>())
		{
		return sTweens<Rat>(*theSeqQ);
		}
	else if (ZQ<Map> theMapQ = iValQ->QGet<Map>())
		{
		if (ZQ<string8> theRatCtor = theMapQ->QGet<string8>("RatCtor"))
			return spAugment(*theMapQ, RatRegistration::sCtor(*theRatCtor, *theMapQ));
		
		if (ZQ<Val> theValueQ = sQGetNamed(*theMapQ, "Value", "V"))
			return spAugment(*theMapQ, sTween<Rat>(theValueQ));
		
		return null;
		}

	if (ZLOGF(w, eDebug))
		w << iValQ->Type().name();

	sDump(*iValQ);

	ZUnimplemented();

	return null;
	}

// =================================================================================================
#pragma mark - Const

static ZP<Tween_Rat> spRatCtor_Const(const Map& iMap)
	{ return sTween_Const(sDRat(0, iMap.QGet("Val"))); }

ZMACRO_RatRegister(Const);

// =================================================================================================
#pragma mark -

#define ZMACRO_RatRegistration(a)\
static ZP<Tween_Rat> spRatCtor_##a(const Map&) \
	{ static const ZMACRO_auto(spTween, sTween_##a<Rat>()); return spTween; } \
ZMACRO_RatRegister(a)

ZMACRO_RatRegistration(Linear);
ZMACRO_RatRegistration(Triangle);
ZMACRO_RatRegistration(Square);
ZMACRO_RatRegistration(Sin);
ZMACRO_RatRegistration(Cos);
ZMACRO_RatRegistration(Erf);
ZMACRO_RatRegistration(Sqrt);
ZMACRO_RatRegistration(QuadIn);
ZMACRO_RatRegistration(QuadOut);
ZMACRO_RatRegistration(CubicIn);
ZMACRO_RatRegistration(CubicOut);
ZMACRO_RatRegistration(QuartIn);
ZMACRO_RatRegistration(QuartOut);
ZMACRO_RatRegistration(QuintIn);
ZMACRO_RatRegistration(QuintOut);
ZMACRO_RatRegistration(SinIn);
ZMACRO_RatRegistration(SinOut);
ZMACRO_RatRegistration(ExpoIn);
ZMACRO_RatRegistration(ExpoOut);
ZMACRO_RatRegistration(CircIn);
ZMACRO_RatRegistration(CircOut);
ZMACRO_RatRegistration(ElasticIn);
ZMACRO_RatRegistration(ElasticOut);
ZMACRO_RatRegistration(BounceIn);
ZMACRO_RatRegistration(BounceOut);

// =================================================================================================
#pragma mark - BackIn

static ZP<Tween_Rat> spRatCtor_BackIn(const Map& iMap)
	{ return sTween_BackIn(sDRat(1.70158, iMap.QGet("Factor"))); }

ZMACRO_RatRegister(BackIn);

// =================================================================================================
#pragma mark - BackOut

static ZP<Tween_Rat> spRatCtor_BackOut(const Map& iMap)
	{ return sTween_BackOut(sDRat(1.70158, iMap.QGet("Factor"))); }

ZMACRO_RatRegister(BackOut);

// =================================================================================================
#pragma mark - RatRegistration

RatRegistration::RatRegistration(const string8& iCtorName, Fun iFun)
	{ sSingleton<map<string8,Fun> >()[iCtorName] = iFun; }

ZP<Tween_Rat> RatRegistration::sCtor(const string8& iCtorName, const Map& iMap)
	{
	ZQ<Fun> theFun = Util_STL::sQGet(sSingleton<map<string8,Fun> >(), iCtorName);

	if (not theFun || not *theFun)
		{
		if (ZLOGPF(w, eErr))
			w << "Couldn't find ctor for " << iCtorName;
		}

	return (*theFun)(iMap);
	}

} // namespace GameEngine
} // namespace ZooLib
