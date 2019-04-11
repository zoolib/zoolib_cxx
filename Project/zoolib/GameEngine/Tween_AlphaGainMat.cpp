#include "zoolib/GameEngine/Tween_AlphaGainMat.h"

#include "zoolib/Singleton.h"
#include "zoolib/Util_STL_map.h"

#include "zoolib/GameEngine/Tween_Geometry.h"
#include "zoolib/GameEngine/Util.h"

using std::map;

// =================================================================================================
#pragma mark - sCombineTweenVals for Alpha, Gain, Mat and combinations

namespace ZooLib {

using GameEngine::Alpha;
using GameEngine::Gain;
using GameEngine::Mat;
using GameEngine::AlphaMat;
using GameEngine::AlphaGainMat;

template <>
Alpha sCombineTweenVals<Alpha,Alpha>(const Alpha& iAlpha0, const Alpha& iAlpha1)
	{ return iAlpha0 * iAlpha1; }

template <>
Gain sCombineTweenVals<Gain,Gain>(const Gain& iGain0, const Gain& iGain1)
	{ return iGain0 * iGain1; }

template <>
Mat sCombineTweenVals<Mat,Mat>(const Mat& iMat0, const Mat& iMat1)
	{
	return iMat1 * iMat0; // Note the order
	}

template <>
AlphaGainMat sCombineTweenVals<AlphaGainMat,Alpha>
	(const AlphaGainMat& iAlphaGainMat, const Alpha& iAlpha)
	{
	return AlphaGainMat(
		iAlphaGainMat.fAlpha * iAlpha, iAlphaGainMat.fGain, iAlphaGainMat.fMat);
	}

template <>
AlphaGainMat sCombineTweenVals<AlphaGainMat,Gain>
	(const AlphaGainMat& iAlphaGainMat, const Gain& iGain)
	{
	return AlphaGainMat(
		iAlphaGainMat.fAlpha, iAlphaGainMat.fGain * iGain, iAlphaGainMat.fMat);
	}

template <>
AlphaGainMat sCombineTweenVals<AlphaGainMat,Mat>
	(const AlphaGainMat& iAlphaGainMat, const Mat& iMat)
	{
	return AlphaGainMat(
		iAlphaGainMat.fAlpha, iAlphaGainMat.fGain, iMat * iAlphaGainMat.fMat);
	}

template <>
AlphaGainMat sCombineTweenVals<AlphaGainMat,AlphaGainMat>
	(const AlphaGainMat& iAlphaGainMat0, const AlphaGainMat& iAlphaGainMat1)
	{
	return iAlphaGainMat1 * iAlphaGainMat0; // Note the order
	}

} // namespace ZooLib

namespace ZooLib {
namespace GameEngine {

// =================================================================================================
#pragma mark - sTween_Alpha

ZRef<Tween_Alpha> sTween_Alpha(const ZRef<Tween_Rat>& iTween_Rat)
	{
	if (iTween_Rat)
		return new Tween_Filter_Coerce<Alpha,Rat>(iTween_Rat);
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
#pragma mark - sTween_AlphaGainMat

ZRef<Tween<AlphaGainMat> > sTween_AlphaGainMat(const Map& iMap)
	{
	ZRef<Tween_Alpha> theTweenAlpha;
	ZRef<Tween_Gain> theTweenGain;
	ZRef<Tween_Mat> theTweenMat = sTween_Mat_Identity();

	// ---------------------------------------------------------------------------------------------

	if (ZQ<Val> theQ = sQGetNamed(iMap, "Alpha", "A"))
		theTweenAlpha = sTween_Alpha(sTween<Rat>(theQ));

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

	return sTween_AlphaGainMat(theTweenAlpha, theTweenGain, theTweenMat);
	}

template <>
ZRef<Tween<AlphaGainMat> > sTween<AlphaGainMat>(const ZQ<Val>& iValQ)
	{
	if (not iValQ)
		return null;
	
	if (iValQ->IsNull())
		return null;

	if (ZQ<Seq> theSeqQ = iValQ->QGet<Seq>())
		{
		return sTweens<AlphaGainMat>(*theSeqQ);
		}
	else if (ZQ<Map> theMapQ = iValQ->QGet<Map>())
		{
		if (ZQ<string8> theAlphaMatCtor = theMapQ->QGet<string8>("AlphaMatCtor"))
			return AlphaGainMatRegistration::sCtor(*theAlphaMatCtor, *theMapQ);

		return sTween_AlphaGainMat(*theMapQ);
		}

	sDump(*iValQ);

	ZUnimplemented();

	return null;
	}

ZRef<Tween_AlphaGainMat> sTween_AlphaGainMat(const AlphaGainMat& iAlphaGainMat)
	{ return sTween_Const(iAlphaGainMat); }

ZRef<Tween_AlphaGainMat> sTween_AlphaGainMat(
	const ZRef<Tween_Alpha>& iTween_Alpha,
	const ZRef<Tween_Gain>& iTween_Gain,
	const ZRef<Tween_Mat>& iTween_Mat)
	{
	class Tween
	:	public Tween_AlphaGainMat
		{
	public:
		Tween(
			const ZRef<Tween_Alpha>& iTween_Alpha,
			const ZRef<Tween_Gain>& iTween_Gain,
			const ZRef<Tween_Mat>& iTween_Mat)
		:	fTween_Alpha(iTween_Alpha)
		,	fTween_Gain(iTween_Gain)
		,	fTween_Mat(iTween_Mat)
			{}

		virtual ZQ<AlphaGainMat> QValAt(double iPlace)
			{
			bool gotAny = false;
			AlphaGainMat theAGM;

			if (fTween_Alpha)
				{
				if (ZQ<Alpha> theQ =
					fTween_Alpha->QValAt(iPlace / spWeight(fTween_Alpha, fWeight_Alpha)))
					{
					gotAny = true;
					theAGM *= *theQ;
					}
				}

			if (fTween_Gain)
				{
				if (ZQ<Gain> theQ =
					fTween_Gain->QValAt(iPlace / spWeight(fTween_Gain, fWeight_Gain)))
					{
					gotAny = true;
					theAGM *= *theQ;
					}
				}

			if (fTween_Mat)
				{
				if (ZQ<Mat> theMatQ =
					fTween_Mat->QValAt(iPlace / spWeight(fTween_Mat, fWeight_Mat)))
					{
					gotAny = true;
					theAGM *= *theMatQ;
					}
				}

			if (gotAny)
				return theAGM;

			return null;
			}

	private:
		const ZRef<Tween_Alpha> fTween_Alpha;
		ZQ<double> fWeight_Alpha;
		
		const ZRef<Tween_Gain> fTween_Gain;
		ZQ<double> fWeight_Gain;

		const ZRef<Tween_Mat> fTween_Mat;
		ZQ<double> fWeight_Mat;
		};

	if (iTween_Alpha || iTween_Gain || iTween_Mat)
		return new Tween(iTween_Alpha, iTween_Gain, iTween_Mat);

	return null;
	}

ZRef<Tween_AlphaGainMat> sTween_AlphaGainMat(const ZRef<Tween_Alpha>& iTween_Alpha)
	{ return sTween_AlphaGainMat(iTween_Alpha, null, null); }

ZRef<Tween_AlphaGainMat> sTween_AlphaGainMat(const ZRef<Tween_Gain>& iTween_Gain)
	{ return sTween_AlphaGainMat(null, iTween_Gain, null); }

ZRef<Tween_AlphaGainMat> sTween_AlphaGainMat(const ZRef<Tween_Mat>& iTween_Mat)
	{ return sTween_AlphaGainMat(null, null, iTween_Mat); }

// =================================================================================================
#pragma mark - AlphaGainMatRegistration

AlphaGainMatRegistration::AlphaGainMatRegistration(const string8& iCtorName, Fun iFun)
	{ sSingleton<map<string8,Fun> >()[iCtorName] = iFun; }

ZRef<Tween_AlphaGainMat> AlphaGainMatRegistration::sCtor(const string8& iCtorName, const Map& iMap)
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
