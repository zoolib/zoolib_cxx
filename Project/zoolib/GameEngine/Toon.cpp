#include "zoolib/GameEngine/Toon.h"

#include "zoolib/Log.h"
#include "zoolib/Singleton.h"
#include "zoolib/Util_STL_map.h"

#include "zoolib/GameEngine/AssetCatalog.h"
#include "zoolib/GameEngine/Tween_AlphaGainMat.h"
#include "zoolib/GameEngine/Util.h"
//#include "zoolib/GameEngine/ZGameEngine_Tweens.h"

using std::map;
using std::pair;
using std::vector;

// =================================================================================================
// MARK: - sCombineTweenVals for CelStack, AlphaMat, Alpha and Mat combinations

namespace ZooLib {

using GameEngine::Alpha;
using GameEngine::AlphaMat;
using GameEngine::AlphaGainMat;
using GameEngine::Cel;
using GameEngine::CelStack;
using GameEngine::Mat;

template <>
ZQ<CelStack> sCombineTweenVals<CelStack,Alpha>
	(const ZQ<CelStack>& iCelStackQ, const ZQ<Alpha>& iAlphaQ)
	{
	if (not iCelStackQ)
		return null;
	else if (not iAlphaQ)
		return iCelStackQ;

	CelStack result = iCelStackQ->Get()->Clone();
	vector<Cel>& theVec = result->Mut();
	for (vector<Cel>::iterator ii = theVec.begin(); ii != theVec.end(); ++ii)
		ii->fAlphaMat.fAlpha *= *iAlphaQ;
	return result;
	}

template <>
ZQ<CelStack> sCombineTweenVals<CelStack,Mat>(const ZQ<CelStack>& iCelStackQ, const ZQ<Mat>& iMatQ)
	{
	if (not iCelStackQ)
		return null;
	else if (not iMatQ)
		return iCelStackQ;

	CelStack result = iCelStackQ->Get()->Clone();
	vector<Cel>& theVec = result->Mut();
	for (vector<Cel>::iterator ii = theVec.begin(); ii != theVec.end(); ++ii)
		ii->fAlphaMat.fMat = *iMatQ * ii->fAlphaMat.fMat;
	return result;
	}

template <>
ZQ<CelStack> sCombineTweenVals<CelStack,AlphaGainMat>
	(const ZQ<CelStack>& iCelStackQ, const ZQ<AlphaGainMat>& iAlphaGainMatQ)
	{
	if (not iCelStackQ)
		return null;
	else if (not iAlphaGainMatQ)
		return iCelStackQ;

	CelStack result = iCelStackQ->Get()->Clone();
	vector<Cel>& theVec = result->Mut();
	for (vector<Cel>::iterator ii = theVec.begin(); ii != theVec.end(); ++ii)
		{
		ii->fAlphaMat.fMat = iAlphaGainMatQ->fMat * ii->fAlphaMat.fMat;
		ii->fAlphaMat.fAlpha *= iAlphaGainMatQ->fAlpha;
		}
	return result;
	}

template <>
ZQ<CelStack> sCombineTweenVals<CelStack,CelStack>
	(const ZQ<CelStack>& iCelStackQ0, const ZQ<CelStack>& iCelStackQ1)
	{
	if (not iCelStackQ0)
		return iCelStackQ1;
	else if (not iCelStackQ1)
		return iCelStackQ0;

	CelStack result = iCelStackQ0->Get()->Clone();
	sMut(result).insert(sMut(result).end(),
		(*iCelStackQ1)->Get().begin(), (*iCelStackQ1)->Get().end());
	return result;
	}

} // namespace ZooLib

namespace ZooLib {
namespace GameEngine {

// =================================================================================================
// MARK: - spToon_NameFrame

namespace { // anonymous
ZRef<Toon> spToon_NameFrame(const Name& iName, double iWeight, size_t iFrameCount)
	{
	class Toon_NameFrame
	:	public Toon
		{
	public:
		Toon_NameFrame(const Name& iName, double iWeight, size_t iFrameCount)
		:	fName(iName)
		,	fWeight(iWeight > 0 ? iWeight : (iFrameCount == 0 ? 0 : 1))
		,	fFrameCount(iFrameCount)
			{}

		virtual ZQ<CelStack> QValAt(double iPlace)
			{
			if (iPlace < 0 || iPlace >= fWeight)
				return null;

			const size_t theFrame =
				sMinMax<size_t>(0, fFrameCount * iPlace / fWeight, fFrameCount - 1);

			return sCountedVal<vector<Cel> >(1, Cel(NameFrame(fName, theFrame), AlphaMat()));
			}

		virtual double Weight()
			{ return fWeight; }

	private:
		const Name fName;
		const double fWeight;
		const size_t fFrameCount;
		};

	return new Toon_NameFrame(iName, iWeight, iFrameCount);
	}
} // anonymous namespace

// =================================================================================================
// MARK: - sTween<CelStack>

template <>
ZRef<Toon> sTween<CelStack>(const ZQ<Val>& iValQ)
	{
	if (not iValQ)
		{
		return null;
		}
	else if (iValQ->IsNull())
		{
		return null;
		}
	else if (const string8* theNameP = iValQ->PGet<string8>())
		{
		const Name theName = *theNameP;
		const size_t theFrameCount = sToonFrameCount(theName);
		return spToon_NameFrame(theName, theFrameCount * 1.0/15.0, theFrameCount);
		}
	else if (ZQ<Seq> theSeqQ = iValQ->QGet<Seq>())
		{
		return sTweens<CelStack>(*theSeqQ);
		}
	else if (ZQ<Map> theMapQ = iValQ->QGet<Map>())
		{
		if (ZQ<string8> theToonCtor = theMapQ->QGet<string8>("ToonCtor"))
			return ToonRegistration::sCtor(*theToonCtor, *theMapQ);
		
		if (ZQ<Val> theCQ = sQGetNamed(*theMapQ, "Content", "C"))
			{
			ZRef<Toon> theToon;
			if (const string8* theNameP = theCQ->PGet<string8>())
				{
				const Name theName = *theNameP;

				ZQ<Rat> theFDQ = sQRat(sQGetNamed(*theMapQ, "FrameDuration", "FD"));
				ZQ<Rat> theTDQ = sQRat(sQGetNamed(*theMapQ, "TotalDuration", "TD"));

				const size_t theFrameCount = sToonFrameCount(theName);

				if (theTDQ)
					theToon = spToon_NameFrame(theName, *theTDQ, theFrameCount);
				else if (theFDQ)
					theToon = spToon_NameFrame(theName, *theFDQ * theFrameCount, theFrameCount);
				else
					theToon = spToon_NameFrame(theName, theFrameCount * 1.0/15.0, theFrameCount);
				}
			else
				{
				theToon = sTween<CelStack>(*theCQ);
				}

			if (theToon)
				{
				if (ZQ<Val> theAMQ = sQGetNamed(*theMapQ, "AlphaGainMat", "AGM"))
					{
					if (ZRef<Tween_AlphaGainMat> theAM = sTween<AlphaGainMat>(theAMQ))
						theToon /= theAM;
					}
				else if (ZRef<Tween_AlphaGainMat> theAM = sTween<AlphaGainMat>(iValQ))
					{
					theToon /= theAM;
					}

				return theToon;
				}
			}
		return null;
		}

	sDump(*iValQ);
	ZUnimplemented();
	}

// =================================================================================================
// MARK: - sToon

ZRef<Toon> sToon(const ZQ<Val>& iValQ)
	{ return sTween<CelStack>(iValQ); }

// =================================================================================================
// MARK: - ToonRegistration

ToonRegistration::ToonRegistration(const string8& iCtorName, Fun iFun)
	{ sSingleton<map<string8,Fun> >()[iCtorName] = iFun; }

ZRef<Toon> ToonRegistration::sCtor(const string8& iCtorName, const Map& iMap)
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
