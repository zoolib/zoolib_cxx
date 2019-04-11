#include "zoolib/GameEngine/Tween_BlushGainMat.h"

#include "zoolib/Singleton.h"
#include "zoolib/Util_STL_map.h"

#include "zoolib/GameEngine/Tween_Geometry.h"
#include "zoolib/GameEngine/Util.h"

using std::map;

// =================================================================================================
#pragma mark - sCombineTweenVals for Blush, Gain, Mat and combinations

namespace ZooLib {

using GameEngine::Blush;
using GameEngine::Gain;
using GameEngine::Mat;
using GameEngine::BlushMat;
using GameEngine::BlushGainMat;

template <>
Blush sCombineTweenVals<Blush,Blush>(const Blush& iBlush0, const Blush& iBlush1)
	{ return iBlush0 * iBlush1; }

template <>
Gain sCombineTweenVals<Gain,Gain>(const Gain& iGain0, const Gain& iGain1)
	{ return iGain0 * iGain1; }

template <>
Mat sCombineTweenVals<Mat,Mat>(const Mat& iMat0, const Mat& iMat1)
	{
	return iMat1 * iMat0; // Note the order
	}

template <>
BlushGainMat sCombineTweenVals<BlushGainMat,Blush>
	(const BlushGainMat& iBlushGainMat, const Blush& iBlush)
	{
	return BlushGainMat(
		iBlushGainMat.fBlush * iBlush, iBlushGainMat.fGain, iBlushGainMat.fMat);
	}

template <>
BlushGainMat sCombineTweenVals<BlushGainMat,Gain>
	(const BlushGainMat& iBlushGainMat, const Gain& iGain)
	{
	return BlushGainMat(
		iBlushGainMat.fBlush, iBlushGainMat.fGain * iGain, iBlushGainMat.fMat);
	}

template <>
BlushGainMat sCombineTweenVals<BlushGainMat,Mat>
	(const BlushGainMat& iBlushGainMat, const Mat& iMat)
	{
	return BlushGainMat(
		iBlushGainMat.fBlush, iBlushGainMat.fGain, iMat * iBlushGainMat.fMat);
	}

template <>
BlushGainMat sCombineTweenVals<BlushGainMat,BlushGainMat>
	(const BlushGainMat& iBlushGainMat0, const BlushGainMat& iBlushGainMat1)
	{
	return iBlushGainMat1 * iBlushGainMat0; // Note the order
	}

} // namespace ZooLib

namespace ZooLib {
namespace GameEngine {

// =================================================================================================
#pragma mark - sTween_Blush

static Blush spBlushFromRat(Rat iRat)
	{ return sRGBA(1, iRat); }

ZRef<Tween_Blush> sTween_Blush(const ZRef<Tween_Rat>& iTween_Rat)
	{
	if (iTween_Rat)
		return new Tween_Filter_Fun<Blush,Rat,spBlushFromRat>(iTween_Rat);
	return null;
	}

// =================================================================================================
#pragma mark - sTween_Gain

ZRef<Tween_Gain> sTween_Gain(const ZRef<Tween_Rat>& iTween_Rat)
	{
	if (iTween_Rat)
		return new Tween_Filter_Coerce<Gain,Rat>(iTween_Rat);
	return null;
	}

// =================================================================================================
#pragma mark - sTween_Mat_Identity

ZRef<Tween_Mat> sTween_Mat_Identity()
	{
	static ZRef<Tween_Mat> spTween = sTween_Const<Mat>(1);
	return spTween;
	}

// =================================================================================================
#pragma mark - sTween_BlushGainMat

ZRef<Tween<BlushGainMat> > sTween_BlushGainMat(const Map& iMap)
	{
	ZRef<Tween_Blush> theTweenBlush;
	ZRef<Tween_Gain> theTweenGain;
	ZRef<Tween_Mat> theTweenMat = sTween_Mat_Identity();

	// ---------------------------------------------------------------------------------------------

	if (ZQ<Val> theQ = sQGetNamed(iMap, "Blush", "B"))
		theTweenBlush = sTween_Blush(sTween<Rat>(theQ));

	// ---------------------------------------------------------------------------------------------

	if (ZQ<Val> theQ = sQGetNamed(iMap, "Gain", "G"))
		theTweenGain = sTween_Gain(sTween<Rat>(theQ));
	
	// ---------------------------------------------------------------------------------------------

	if (ZQ<Val> theQ = sQGetNamed(iMap, "Scale", "S"))
		{
		if (ZQ<Seq> theSeqQ = theQ->QGet<Seq>())
			{
			theTweenMat *= sTween_ScaleX(sTween<Rat>(theSeqQ->QGet(0)));
			theTweenMat *= sTween_ScaleY(sTween<Rat>(theSeqQ->QGet(1)));
			theTweenMat *= sTween_ScaleZ(sTween<Rat>(theSeqQ->QGet(2)));
			}
		else if (ZQ<CVec3> theCVecQ = sQCVec3(1, *theQ))
			{
			theTweenMat *= sTween_Const<Mat>(sScale<Rat>(*theCVecQ));
			}
		}
	
	// -----

	if (const ZQ<Val> theQ = sQGetNamed(iMap, "ScaleX", "SX"))
		theTweenMat *= sTween_ScaleX(sTween<Rat>(theQ));

	if (const ZQ<Val> theQ = sQGetNamed(iMap, "ScaleY", "SY"))
		theTweenMat *= sTween_ScaleY(sTween<Rat>(theQ));

	if (const ZQ<Val> theQ = sQGetNamed(iMap, "ScaleZ", "SZ"))
		theTweenMat *= sTween_ScaleZ(sTween<Rat>(theQ));

	// -----

	if (const ZQ<Val> theQ = sQGetNamed(iMap, "ScaleXY", "SXY"))
		{
		if (ZRef<Tween<Rat> > theTween = sTween<Rat>(theQ))
			theTweenMat *= sTween_ScaleX(theTween) * sTween_ScaleY(theTween);
		}

	if (const ZQ<Val> theQ = sQGetNamed(iMap, "ScaleXZ", "SXZ"))
		{
		if (ZRef<Tween<Rat> > theTween = sTween<Rat>(theQ))
			theTweenMat *= sTween_ScaleX(theTween) * sTween_ScaleZ(theTween);
		}

	if (const ZQ<Val> theQ = sQGetNamed(iMap, "ScaleYZ", "SYZ"))
		{
		if (ZRef<Tween<Rat> > theTween = sTween<Rat>(theQ))
			theTweenMat *= sTween_ScaleY(theTween) * sTween_ScaleZ(theTween);
		}

	if (const ZQ<Val> theQ = sQGetNamed(iMap, "ScaleXYZ", "SXYZ"))
		{
		if (ZRef<Tween<Rat> > theTween = sTween<Rat>(theQ))
			{
			theTweenMat *= sTween_ScaleX(theTween)
				* sTween_ScaleY(theTween) * sTween_ScaleZ(theTween);
			}
		}

	// -------------------------------------------------------------------------

	if (ZQ<Seq> theSeqQ = sGetNamed(iMap, "Rotate", "R").QGet<Seq>())
		{
		theTweenMat *= sTween_RotateX(sTween<Rat>(theSeqQ->QGet(0)));
		theTweenMat *= sTween_RotateY(sTween<Rat>(theSeqQ->QGet(1)));
		theTweenMat *= sTween_RotateZ(sTween<Rat>(theSeqQ->QGet(2)));
		}
	
	// -----

	if (const ZQ<Val> theQ = sQGetNamed(iMap, "RotateX", "RX"))
		theTweenMat *= sTween_RotateX(sTween<Rat>(theQ));
	
	if (const ZQ<Val> theQ = sQGetNamed(iMap, "RotateY", "RY"))
		theTweenMat *= sTween_RotateY(sTween<Rat>(theQ));
	
	if (const ZQ<Val> theQ = sQGetNamed(iMap, "RotateZ", "RZ"))
		theTweenMat *= sTween_RotateZ(sTween<Rat>(theQ));


	// -------------------------------------------------------------------------

	if (ZQ<Val> theQ = sQGetNamed(iMap, "Translate", "T", "Pos"))
		{
		if (ZQ<Seq> theSeqQ = theQ->QGet<Seq>())
			{
			theTweenMat *= sTween_TranslateX(sTween<Rat>(theSeqQ->QGet(0)));
			theTweenMat *= sTween_TranslateY(sTween<Rat>(theSeqQ->QGet(1)));
			theTweenMat *= sTween_TranslateZ(sTween<Rat>(theSeqQ->QGet(2)));
			}
		else if (ZQ<CVec3> theCVecQ = sQCVec3(0, *theQ))
			{
			theTweenMat *= sTween_Const<Mat>(sTranslate<Rat>(*theCVecQ));
			}
		}

	// -----

	if (const ZQ<Val> theQ = sQGetNamed(iMap, "TranslateX", "TX"))
		theTweenMat *= sTween_TranslateX(sTween<Rat>(theQ));

	if (const ZQ<Val> theQ = sQGetNamed(iMap, "TranslateY", "TY"))
		theTweenMat *= sTween_TranslateY(sTween<Rat>(theQ));

	if (const ZQ<Val> theQ = sQGetNamed(iMap, "TranslateZ", "TZ"))
		theTweenMat *= sTween_TranslateZ(sTween<Rat>(theQ));


	// -----

	if (const ZQ<Val> theQ = sQGetNamed(iMap, "TranslateXY", "TXY"))
		{
		if (ZRef<Tween<Rat> > theTween = sTween<Rat>(theQ))
			theTweenMat *= sTween_TranslateX(theTween) * sTween_TranslateY(theTween);
		}

	if (const ZQ<Val> theQ = sQGetNamed(iMap, "TranslateXZ", "TXZ"))
		{
		if (ZRef<Tween<Rat> > theTween = sTween<Rat>(theQ))
			theTweenMat *= sTween_TranslateX(theTween) * sTween_TranslateZ(theTween);
		}

	if (const ZQ<Val> theQ = sQGetNamed(iMap, "TranslateYZ", "TYZ"))
		{
		if (ZRef<Tween<Rat> > theTween = sTween<Rat>(theQ))
			theTweenMat *= sTween_TranslateY(theTween) * sTween_TranslateZ(theTween);
		}

	if (const ZQ<Val> theQ = sQGetNamed(iMap, "TranslateXYZ", "TXYZ"))
		{
		if (ZRef<Tween<Rat> > theTween = sTween<Rat>(theQ))
			{
			theTweenMat *= sTween_TranslateX(theTween)
				* sTween_TranslateY(theTween) * sTween_TranslateZ(theTween);
			}
		}

	// ---------------------------------------------------------------------------------------------

	if (theTweenMat == sTween_Mat_Identity())
		theTweenMat.Clear();

	return sTween_BlushGainMat(theTweenBlush, theTweenGain, theTweenMat);
	}

template <>
ZRef<Tween<BlushGainMat> > sTween<BlushGainMat>(const ZQ<Val>& iValQ)
	{
	if (not iValQ)
		return null;
	
	if (iValQ->IsNull())
		return null;

	if (ZQ<Seq> theSeqQ = iValQ->QGet<Seq>())
		{
		return sTweens<BlushGainMat>(*theSeqQ);
		}
	else if (ZQ<Map> theMapQ = iValQ->QGet<Map>())
		{
		if (ZQ<string8> theBlushMatCtor = theMapQ->QGet<string8>("BlushMatCtor"))
			return BlushGainMatRegistration::sCtor(*theBlushMatCtor, *theMapQ);

		return sTween_BlushGainMat(*theMapQ);
		}

	sDump(*iValQ);

	ZUnimplemented();

	return null;
	}

ZRef<Tween_BlushGainMat> sTween_BlushGainMat(const BlushGainMat& iBlushGainMat)
	{ return sTween_Const(iBlushGainMat); }

ZRef<Tween_BlushGainMat> sTween_BlushGainMat(
	const ZRef<Tween_Blush>& iTween_Blush,
	const ZRef<Tween_Gain>& iTween_Gain,
	const ZRef<Tween_Mat>& iTween_Mat)
	{
	class Tween
	:	public Tween_BlushGainMat
		{
	public:
		Tween(
			const ZRef<Tween_Blush>& iTween_Blush,
			const ZRef<Tween_Gain>& iTween_Gain,
			const ZRef<Tween_Mat>& iTween_Mat)
		:	fTween_Blush(iTween_Blush)
		,	fTween_Gain(iTween_Gain)
		,	fTween_Mat(iTween_Mat)
			{}

		virtual ZQ<BlushGainMat> QValAt(double iPlace)
			{
			bool gotAny = false;
			BlushGainMat theBGM;

			if (fTween_Blush)
				{
				if (ZQ<Blush> theQ =
					fTween_Blush->QValAt(iPlace / spWeight(fTween_Blush, fWeight_Blush)))
					{
					gotAny = true;
					theBGM *= *theQ;
					}
				}

			if (fTween_Gain)
				{
				if (ZQ<Gain> theQ =
					fTween_Gain->QValAt(iPlace / spWeight(fTween_Gain, fWeight_Gain)))
					{
					gotAny = true;
					theBGM *= *theQ;
					}
				}

			if (fTween_Mat)
				{
				if (ZQ<Mat> theMatQ =
					fTween_Mat->QValAt(iPlace / spWeight(fTween_Mat, fWeight_Mat)))
					{
					gotAny = true;
					theBGM *= *theMatQ;
					}
				}

			if (gotAny)
				return theBGM;

			return null;
			}

	private:
		const ZRef<Tween_Blush> fTween_Blush;
		ZQ<double> fWeight_Blush;
		
		const ZRef<Tween_Gain> fTween_Gain;
		ZQ<double> fWeight_Gain;

		const ZRef<Tween_Mat> fTween_Mat;
		ZQ<double> fWeight_Mat;
		};

	if (iTween_Blush || iTween_Gain || iTween_Mat)
		return new Tween(iTween_Blush, iTween_Gain, iTween_Mat);

	return null;
	}

ZRef<Tween_BlushGainMat> sTween_BlushGainMat(const ZRef<Tween_Blush>& iTween_Blush)
	{ return sTween_BlushGainMat(iTween_Blush, null, null); }

ZRef<Tween_BlushGainMat> sTween_BlushGainMat(const ZRef<Tween_Gain>& iTween_Gain)
	{ return sTween_BlushGainMat(null, iTween_Gain, null); }

ZRef<Tween_BlushGainMat> sTween_BlushGainMat(const ZRef<Tween_Mat>& iTween_Mat)
	{ return sTween_BlushGainMat(null, null, iTween_Mat); }

// =================================================================================================
#pragma mark - BlushGainMatRegistration

BlushGainMatRegistration::BlushGainMatRegistration(const string8& iCtorName, Fun iFun)
	{ sSingleton<map<string8,Fun> >()[iCtorName] = iFun; }

ZRef<Tween_BlushGainMat> BlushGainMatRegistration::sCtor(const string8& iCtorName, const Map& iMap)
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
