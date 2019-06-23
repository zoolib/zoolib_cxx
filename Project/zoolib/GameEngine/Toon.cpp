#include "zoolib/GameEngine/Toon.h"

#include "zoolib/Log.h"
#include "zoolib/Singleton.h"
#include "zoolib/Util_STL_map.h"

#include "zoolib/GameEngine/AssetCatalog.h" // For sToonFrameCount
#include "zoolib/GameEngine/Tween_BlushGainMat.h"
#include "zoolib/GameEngine/Util.h"

using std::map;
using std::pair;
using std::vector;

// =================================================================================================
#pragma mark - sCombineTweenVals for CelStack, BlushMat, Blush and Mat combinations

namespace ZooLib {

using GameEngine::Blush;
using GameEngine::BlushMat;
using GameEngine::BlushGainMat;
using GameEngine::Cel;
using GameEngine::CelStack;
using GameEngine::Mat;

template <>
CelStack sCombineTweenVals<CelStack,Blush>
	(const CelStack& iCelStack, const Blush& iBlush)
	{
	CelStack result = iCelStack->Clone();
	vector<Cel>& theVec = result->Mut();
	for (vector<Cel>::iterator ii = theVec.begin(); ii != theVec.end(); ++ii)
		ii->fBlushMat.fBlush *= iBlush;
	return result;
	}

template <>
CelStack sCombineTweenVals<CelStack,Mat>(const CelStack& iCelStack, const Mat& iMat)
	{
	CelStack result = iCelStack.Get()->Clone();
	vector<Cel>& theVec = result->Mut();
	for (vector<Cel>::iterator ii = theVec.begin(); ii != theVec.end(); ++ii)
		ii->fBlushMat.fMat = iMat * ii->fBlushMat.fMat;
	return result;
	}

template <>
CelStack sCombineTweenVals<CelStack,BlushGainMat>
	(const CelStack& iCelStack, const BlushGainMat& iBlushGainMat)
	{
	CelStack result = iCelStack.Get()->Clone();
	vector<Cel>& theVec = result->Mut();
	for (vector<Cel>::iterator ii = theVec.begin(); ii != theVec.end(); ++ii)
		{
		ii->fBlushMat.fMat = iBlushGainMat.fMat * ii->fBlushMat.fMat;
		ii->fBlushMat.fBlush *= iBlushGainMat.fBlush;
		}
	return result;
	}

template <>
CelStack sCombineTweenVals<CelStack,CelStack>
	(const CelStack& iCelStack0, const CelStack& iCelStack1)
	{
	CelStack result = iCelStack0.Get()->Clone();
	sMut(result).insert(sMut(result).end(),
		iCelStack1->Get().begin(), iCelStack1->Get().end());
	return result;
	}

} // namespace ZooLib

namespace ZooLib {
namespace GameEngine {

// =================================================================================================
#pragma mark - spToon_NameFrame

namespace { // anonymous
ZP<Toon> spToon_NameFrame(const Name& iName, double iWeight, size_t iFrameCount)
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

			return sCountedVal<vector<Cel> >(1, Cel(NameFrame(fName, theFrame), BlushMat()));
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
#pragma mark - sTween<CelStack>

template <>
ZP<Toon> sTween<CelStack>(const ZQ<Val>& iValQ)
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
			ZP<Toon> theToon;
			if (const string8* theNameP = theCQ->PGet<string8>())
				{
				const Name theName = *theNameP;

				const size_t theFrameCount = sToonFrameCount(theName);

				if (ZQ<Rat> theTDQ = sQRat(sQGetNamed(*theMapQ, "TotalDuration", "TD")))
					{
					theToon = spToon_NameFrame(theName, *theTDQ, theFrameCount);
					}
				else if (ZQ<Rat> theFDQ = sQRat(sQGetNamed(*theMapQ, "FrameDuration", "FD")))
					{
					theToon = spToon_NameFrame(theName, *theFDQ * theFrameCount, theFrameCount);
					}
				else
					{
					theToon = spToon_NameFrame(theName, theFrameCount * 1.0/15.0, theFrameCount);
					}
				}
			else
				{
				theToon = sTween<CelStack>(*theCQ);
				}

			if (theToon)
				{
				if (ZQ<Val> theAMQ = sQGetNamed(*theMapQ, "BlushGainMat", "BGM"))
					{
					if (ZP<Tween_BlushGainMat> theAM = sTween<BlushGainMat>(theAMQ))
						theToon *= theAM;
					}
				else if (ZP<Tween_BlushGainMat> theAM = sTween<BlushGainMat>(iValQ))
					{
					theToon *= theAM;
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
#pragma mark - sToon

ZP<Toon> sToon(const ZQ<Val>& iValQ)
	{ return sTween<CelStack>(iValQ); }

// =================================================================================================
#pragma mark - ToonRegistration

ToonRegistration::ToonRegistration(const string8& iCtorName, Fun iFun)
	{ sSingleton<map<string8,Fun> >()[iCtorName] = iFun; }

ZP<Toon> ToonRegistration::sCtor(const string8& iCtorName, const Map& iMap)
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
