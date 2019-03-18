#include "zoolib/GameEngine/Tween_AlphaGainMat.h"

#include "zoolib/Singleton.h"
#include "zoolib/Util_STL_map.h"

#include "zoolib/GameEngine/Tween_Geometry.h"
#include "zoolib/GameEngine/Util.h"

using std::map;

// =================================================================================================
// MARK: - sCombineTweenVals for Alpha, Gain, Mat and combinations

namespace ZooLib {

using GameEngine::Alpha;
using GameEngine::Gain;
using GameEngine::Mat;
using GameEngine::AlphaMat;
using GameEngine::AlphaGainMat;

template <>
ZQ<Alpha> sCombineTweenVals<Alpha,Alpha>(const ZQ<Alpha>& i0Q, const ZQ<Alpha>& i1Q)
	{
	if (not i0Q)
		return i1Q;
	if (not i1Q)
		return i0Q;
	return *i1Q * *i0Q;
	}

template <>
ZQ<Gain> sCombineTweenVals<Gain,Gain>(const ZQ<Gain>& i0Q, const ZQ<Gain>& i1Q)
	{
	if (not i0Q)
		return i1Q;
	if (not i1Q)
		return i0Q;
	return *i1Q * *i0Q;
	}

template <>
ZQ<Mat> sCombineTweenVals<Mat,Mat>(const ZQ<Mat>& i0Q, const ZQ<Mat>& i1Q)
	{
	if (not i0Q)
		return i1Q;
	if (not i1Q)
		return i0Q;
	return *i1Q * *i0Q; // Note the order
	}

template <>
ZQ<AlphaGainMat> sCombineTweenVals<AlphaGainMat,Alpha>
	(const ZQ<AlphaGainMat>& iAlphaGainMatQ, const ZQ<Alpha>& iAlphaQ)
	{
	if (iAlphaGainMatQ)
		{
		if (iAlphaQ)
			{
			return AlphaGainMat(
				iAlphaGainMatQ->fAlpha * *iAlphaQ, iAlphaGainMatQ->fGain, iAlphaGainMatQ->fMat);
			}
		return iAlphaGainMatQ;
		}
	else if (iAlphaQ)
		{
		return AlphaGainMat(*iAlphaQ);
		}
	return null;
	}

template <>
ZQ<AlphaGainMat> sCombineTweenVals<AlphaGainMat,Gain>
	(const ZQ<AlphaGainMat>& iAlphaGainMatQ, const ZQ<Gain>& iGainQ)
	{
	if (iAlphaGainMatQ)
		{
		if (iGainQ)
			{
			return AlphaGainMat(
				iAlphaGainMatQ->fAlpha, iAlphaGainMatQ->fGain * *iGainQ, iAlphaGainMatQ->fMat);
			}
		return iAlphaGainMatQ;
		}
	else if (iGainQ)
		{
		return AlphaGainMat(*iGainQ);
		}
	return null;
	}

template <>
ZQ<AlphaGainMat> sCombineTweenVals<AlphaGainMat,Mat>
	(const ZQ<AlphaGainMat>& iAlphaGainMatQ, const ZQ<Mat>& iMatQ)
	{
	if (iAlphaGainMatQ)
		{
		if (iMatQ)
			{
			return AlphaGainMat(
				iAlphaGainMatQ->fAlpha, iAlphaGainMatQ->fGain, *iMatQ * iAlphaGainMatQ->fMat);
			}
		return iAlphaGainMatQ;
		}
	else if (iMatQ)
		{
		return AlphaGainMat(*iMatQ);
		}
	return null;
	}

template <>
ZQ<AlphaGainMat> sCombineTweenVals<AlphaGainMat,AlphaGainMat>
	(const ZQ<AlphaGainMat>& i0Q, const ZQ<AlphaGainMat>& i1Q)
	{
	if (not i0Q)
		return i1Q;
	if (not i1Q)
		return i0Q;
	return *i1Q * *i0Q; // Note the order
	}

} // namespace ZooLib

namespace ZooLib {
namespace GameEngine {

// =================================================================================================
// MARK: - sTween_Alpha

ZRef<Tween_Alpha> sTween_Alpha(const ZRef<Tween_Rat>& iTween_Rat)
	{
	if (iTween_Rat)
		return new Tween_Filter_Coerce<Alpha,Rat>(iTween_Rat);
	return null;
	}

// =================================================================================================
// MARK: - sTween_Gain

ZRef<Tween_Gain> sTween_Gain(const ZRef<Tween_Rat>& iTween_Rat)
	{
	if (iTween_Rat)
		return new Tween_Filter_Coerce<Gain,Rat>(iTween_Rat);
	return null;
	}

// =================================================================================================
// MARK: - sTween_Mat_Identity

ZRef<Tween_Mat> sTween_Mat_Identity()
	{
	static ZRef<Tween_Mat> spTween = sTween_Const<Mat>(1);
	return spTween;
	}

// =================================================================================================
// MARK: - sTween_AlphaGainMat

ZRef<Tween<AlphaGainMat> > sTween_AlphaGainMat(const Map& iMap)
	{
	ZRef<Tween_Alpha> theAlpha;
	ZRef<Tween_Gain> theGain;
	ZRef<Tween_Mat> theMat = sTween_Mat_Identity();

	// ---------------------------------------------------------------------------------------------

	if (ZQ<Val> theQ = sQGetNamed(iMap, "Alpha", "A"))
		theAlpha = sTween_Alpha(sTween<Rat>(theQ));

	// ---------------------------------------------------------------------------------------------

	if (ZQ<Val> theQ = sQGetNamed(iMap, "Gain", "G"))
		theGain = sTween_Gain(sTween<Rat>(theQ));
	
	// ---------------------------------------------------------------------------------------------

	if (ZQ<Val> theQ = sQGetNamed(iMap, "Scale", "S"))
		{
		if (ZQ<Seq> theSeqQ = theQ->QGet<Seq>())
			{
			theMat /= sTween_ScaleX(sTween<Rat>(theSeqQ->QGet(0)));
			theMat /= sTween_ScaleY(sTween<Rat>(theSeqQ->QGet(1)));
			theMat /= sTween_ScaleZ(sTween<Rat>(theSeqQ->QGet(2)));
			}
		else if (ZQ<CVec3> theCVecQ = sQCVec3(1, *theQ))
			{
			theMat /= sTween_Const<Mat>(sScale<Rat>(*theCVecQ));
			}
		}
	
	// -----

	if (const ZQ<Val> theQ = sQGetNamed(iMap, "ScaleX", "SX"))
		theMat /= sTween_ScaleX(sTween<Rat>(theQ));

	if (const ZQ<Val> theQ = sQGetNamed(iMap, "ScaleY", "SY"))
		theMat /= sTween_ScaleY(sTween<Rat>(theQ));

	if (const ZQ<Val> theQ = sQGetNamed(iMap, "ScaleZ", "SZ"))
		theMat /= sTween_ScaleZ(sTween<Rat>(theQ));

	// -----

	if (const ZQ<Val> theQ = sQGetNamed(iMap, "ScaleXY", "SXY"))
		{
		if (ZRef<Tween<Rat> > theTween = sTween<Rat>(theQ))
			theMat /= sTween_ScaleX(theTween) / sTween_ScaleY(theTween);
		}

	if (const ZQ<Val> theQ = sQGetNamed(iMap, "ScaleXZ", "SXZ"))
		{
		if (ZRef<Tween<Rat> > theTween = sTween<Rat>(theQ))
			theMat /= sTween_ScaleX(theTween) / sTween_ScaleZ(theTween);
		}

	if (const ZQ<Val> theQ = sQGetNamed(iMap, "ScaleYZ", "SYZ"))
		{
		if (ZRef<Tween<Rat> > theTween = sTween<Rat>(theQ))
			theMat /= sTween_ScaleY(theTween) / sTween_ScaleZ(theTween);
		}

	if (const ZQ<Val> theQ = sQGetNamed(iMap, "ScaleXYZ", "SXYZ"))
		{
		if (ZRef<Tween<Rat> > theTween = sTween<Rat>(theQ))
			{
			theMat /= sTween_ScaleX(theTween)
				/ sTween_ScaleY(theTween) / sTween_ScaleZ(theTween);
			}
		}

	// -------------------------------------------------------------------------

	if (ZQ<Seq> theSeqQ = sGetNamed(iMap, "Rotate", "R").QGet<Seq>())
		{
		theMat /= sTween_RotateX(sTween<Rat>(theSeqQ->QGet(0)));
		theMat /= sTween_RotateY(sTween<Rat>(theSeqQ->QGet(1)));
		theMat /= sTween_RotateZ(sTween<Rat>(theSeqQ->QGet(2)));
		}
	
	// -----

	if (const ZQ<Val> theQ = sQGetNamed(iMap, "RotateX", "RX"))
		theMat /= sTween_RotateX(sTween<Rat>(theQ));
	
	if (const ZQ<Val> theQ = sQGetNamed(iMap, "RotateY", "RY"))
		theMat /= sTween_RotateY(sTween<Rat>(theQ));
	
	if (const ZQ<Val> theQ = sQGetNamed(iMap, "RotateZ", "RZ"))
		theMat /= sTween_RotateZ(sTween<Rat>(theQ));


	// -------------------------------------------------------------------------

	if (ZQ<Val> theQ = sQGetNamed(iMap, "Translate", "T", "Pos"))
		{
		if (ZQ<Seq> theSeqQ = theQ->QGet<Seq>())
			{
			theMat /= sTween_TranslateX(sTween<Rat>(theSeqQ->QGet(0)));
			theMat /= sTween_TranslateY(sTween<Rat>(theSeqQ->QGet(1)));
			theMat /= sTween_TranslateZ(sTween<Rat>(theSeqQ->QGet(2)));
			}
		else if (ZQ<CVec3> theCVecQ = sQCVec3(0, *theQ))
			{
			theMat /= sTween_Const<Mat>(sTranslate<Rat>(*theCVecQ));
			}
		}

	// -----

	if (const ZQ<Val> theQ = sQGetNamed(iMap, "TranslateX", "TX"))
		theMat /= sTween_TranslateX(sTween<Rat>(theQ));

	if (const ZQ<Val> theQ = sQGetNamed(iMap, "TranslateY", "TY"))
		theMat /= sTween_TranslateY(sTween<Rat>(theQ));

	if (const ZQ<Val> theQ = sQGetNamed(iMap, "TranslateZ", "TZ"))
		theMat /= sTween_TranslateZ(sTween<Rat>(theQ));


	// -----

	if (const ZQ<Val> theQ = sQGetNamed(iMap, "TranslateXY", "TXY"))
		{
		if (ZRef<Tween<Rat> > theTween = sTween<Rat>(theQ))
			theMat /= sTween_TranslateX(theTween) / sTween_TranslateY(theTween);
		}

	if (const ZQ<Val> theQ = sQGetNamed(iMap, "TranslateXZ", "TXZ"))
		{
		if (ZRef<Tween<Rat> > theTween = sTween<Rat>(theQ))
			theMat /= sTween_TranslateX(theTween) / sTween_TranslateZ(theTween);
		}

	if (const ZQ<Val> theQ = sQGetNamed(iMap, "TranslateYZ", "TYZ"))
		{
		if (ZRef<Tween<Rat> > theTween = sTween<Rat>(theQ))
			theMat /= sTween_TranslateY(theTween) / sTween_TranslateZ(theTween);
		}

	if (const ZQ<Val> theQ = sQGetNamed(iMap, "TranslateXYZ", "TXYZ"))
		{
		if (ZRef<Tween<Rat> > theTween = sTween<Rat>(theQ))
			{
			theMat /= sTween_TranslateX(theTween)
				/ sTween_TranslateY(theTween) / sTween_TranslateZ(theTween);
			}
		}

	// ---------------------------------------------------------------------------------------------

	if (theMat == sTween_Mat_Identity())
		theMat.Clear();

	return sTween_AlphaGainMat(theAlpha, theGain, theMat);
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
// MARK: - AlphaGainMatRegistration

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
