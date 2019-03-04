#ifndef __ZooLib_GameEngine_Types_h__
#define __ZooLib_GameEngine_Types_h__ 1

#include "zoolib/Cartesian_Matrix.h"
#include "zoolib/ChanW_UTF.h"
#include "zoolib/Map_Shared_T.h"
#include "zoolib/Name.h"
#include "zoolib/TagVal.h"
#include "zoolib/Val_Any.h"
#include "zoolib/ZVal_Yad.h"

#include "zoolib/ARM/Compat_arm.h"

#include "zoolib/GameEngine/ZRGBA.h"
#include "zoolib/GameEngine/Geometry3D.h"

#include "zoolib/Channer_Bin.h"

#include "zoolib/ZMACRO_auto.h"

#define GEMACRO_Callable(callable, function) \
	static const ZMACRO_auto(callable, sCallable(function));

namespace ZooLib {
namespace GameEngine {

typedef ZooLib::ChannerR_Bin ZStreamerR;
typedef ZooLib::ChannerW_Bin ZStreamerW;

// =================================================================================================

typedef ZMtx MtxF;
typedef ZAcqMtx AcqMtxF;

// =================================================================================================
// MARK: - Fundamental types

typedef ZVal_Yad Val;
typedef ZSeq_Yad Seq;
typedef ZMap_Yad Map;

typedef float Rat;

typedef Matrix<Rat,4,4> Mat;

typedef Matrix<Rat,1,2> CVec2;
typedef Matrix<Rat,1,3> CVec3;

typedef MatrixArray<Rat,1,2,2> GRect;
typedef Matrix<Rat,1,2> GPoint;

typedef Map_Shared_T<Map> SharedState;
typedef Map_Shared_Mutable_T<Map> SharedState_Mutable;

// =================================================================================================
// MARK: - GRect pseudo-ctors

inline GRect sGRect()
	{ return GRect(); }

inline GRect sGRect(Rat iX, Rat iY)
	{ return sRect<GRect>(iX, iY); }

inline GRect sGRect(Rat iL, Rat iT, Rat iR, Rat iB)
	{ return sRect<GRect>(iL, iT, iR, iB); }

// =================================================================================================
// MARK: - Alpha

typedef TagVal<float, struct Tag_Alpha> Alpha;

inline Alpha operator*(const Alpha& iLHS, const Alpha& iRHS)
	{ return Alpha(sGet(iLHS) * sGet(iRHS)); }

inline Alpha& operator*=(Alpha& ioLHS, const Alpha& iRHS)
	{
	sSet(ioLHS, sGet(ioLHS) * sGet(iRHS));
	return ioLHS;
	}

// =================================================================================================
// MARK: - Gain

struct Gain
	{
	Gain()
	:	fL(1.0)
	,	fR(1.0)
		{}

	Gain(const Gain& iOther)
	:	fL(iOther.fL)
	,	fR(iOther.fR)
		{}

	Gain(Rat iGain)
	:	fL(iGain)
	,	fR(iGain)
		{}

	Gain(Rat iL, Rat iR)
	:	fL(iL)
	,	fR(iR)
		{}
	
	Rat fL;
	Rat fR;
	};

inline Gain operator*(const Gain& iLHS, const Gain& iRHS)
	{ return Gain(iLHS.fL * iRHS.fL, iLHS.fR * iRHS.fR); }

inline Gain& operator*=(Gain& ioLHS, const Gain& iRHS)
	{
	ioLHS.fL *= iRHS.fL;
	ioLHS.fR *= iRHS.fR;
	return ioLHS;
	}

// =================================================================================================
// MARK: - AlphaMat

struct AlphaMat
	{
	AlphaMat()
	:	fAlpha(1)
	,	fMat(1)
		{}

	AlphaMat(const AlphaMat& iOther)
	:	fAlpha(iOther.fAlpha)
	,	fMat(iOther.fMat)
		{}

	AlphaMat(Alpha iAlpha)
	:	fAlpha(iAlpha)
	,	fMat(1)
		{}

	AlphaMat(const Mat& iMat)
	:	fAlpha(1)
	,	fMat(iMat)
		{}

	AlphaMat(Alpha iAlpha, const Mat& iMat)
	:	fAlpha(iAlpha)
	,	fMat(iMat)
		{}

	Alpha fAlpha;
	Mat fMat;
	};

inline AlphaMat operator*(const AlphaMat& iLHS, const AlphaMat& iRHS)
	{ return AlphaMat(iLHS.fAlpha * iRHS.fAlpha, iLHS.fMat * iRHS.fMat); }

inline AlphaMat& operator*=(AlphaMat& ioLHS, const AlphaMat& iRHS)
	{
	ioLHS.fAlpha *= iRHS.fAlpha;
	ioLHS.fMat *= iRHS.fMat;
	return ioLHS;
	}

// =================================================================================================
// MARK: - AlphaGainMat

struct AlphaGainMat
	{
	AlphaGainMat()
	:	fAlpha(1)
	,	fGain(1)
	,	fMat(1)
		{}

	AlphaGainMat(const AlphaGainMat& iOther)
	:	fAlpha(iOther.fAlpha)
	,	fGain(iOther.fGain)
	,	fMat(iOther.fMat)
		{}

	AlphaGainMat(Alpha iAlpha)
	:	fAlpha(iAlpha)
	,	fGain(1)
	,	fMat(1)
		{}

	AlphaGainMat(const Gain& iGain)
	:	fAlpha(1)
	,	fGain(iGain)
	,	fMat(1)
		{}

	AlphaGainMat(const Mat& iMat)
	:	fAlpha(1)
	,	fGain(1)
	,	fMat(iMat)
		{}

	AlphaGainMat(Alpha iAlpha, const Gain& iGain)
	:	fAlpha(iAlpha)
	,	fGain(iGain)
	,	fMat(1)
		{}

	AlphaGainMat(const Gain& iGain, const Mat& iMat)
	:	fAlpha(1)
	,	fGain(iGain)
	,	fMat(iMat)
		{}

	AlphaGainMat(Alpha iAlpha, const Mat& iMat)
	:	fAlpha(iAlpha)
	,	fMat(iMat)
		{}

	AlphaGainMat(Alpha iAlpha, const Gain& iGain, const Mat& iMat)
	:	fAlpha(iAlpha)
	,	fGain(iGain)
	,	fMat(iMat)
		{}

	Alpha fAlpha;
	Gain fGain;
	Mat fMat;
	};

inline AlphaGainMat operator*(const AlphaGainMat& iLHS, const AlphaGainMat& iRHS)
	{
	return AlphaGainMat(
		iLHS.fAlpha * iRHS.fAlpha,
		iLHS.fGain * iRHS.fGain,
		iLHS.fMat * iRHS.fMat);
	}

inline AlphaGainMat& operator*=(AlphaGainMat& ioLHS, const AlphaGainMat& iRHS)
	{
	ioLHS.fAlpha *= iRHS.fAlpha;
	ioLHS.fGain *= iRHS.fGain;
	ioLHS.fMat *= iRHS.fMat;
	return ioLHS;
	}

// =================================================================================================
// MARK: - Pseudo-ctors, AlphaGainMat --> Alpha, Gain, Mat

inline Alpha sAlpha(const AlphaGainMat& iAlphaGainMat)
	{ return iAlphaGainMat.fAlpha; }

inline Gain sGain(const AlphaGainMat& iAlphaGainMat)
	{ return iAlphaGainMat.fGain; }

inline Mat sMat(const AlphaGainMat& iAlphaGainMat)
	{ return iAlphaGainMat.fMat; }

inline AlphaMat sAlphaMat(const AlphaGainMat& iAlphaGainMat)
	{ return AlphaMat(iAlphaGainMat.fAlpha, iAlphaGainMat.fMat); }

// =================================================================================================
// MARK: -

inline AlphaMat operator*(const Alpha& iAlpha, const Mat& iMat)
	{ return AlphaMat(iAlpha, iMat); }

inline AlphaMat operator*(const Mat& iMat, const Alpha& iAlpha)
	{ return AlphaMat(iAlpha, iMat); }

inline AlphaGainMat operator*(const AlphaMat& iAlphaMat, const Gain& iGain)
	{ return AlphaGainMat(iAlphaMat.fAlpha, iGain, iAlphaMat.fMat); }

inline AlphaGainMat operator*(const Gain& iGain, const AlphaMat& iAlphaMat)
	{ return AlphaGainMat(iAlphaMat.fAlpha, iGain, iAlphaMat.fMat); }

// =================================================================================================
// MARK: -

inline AlphaMat operator*(const AlphaMat& iAlphaMat, const Alpha& iAlpha)
	{ return AlphaMat(iAlphaMat.fAlpha * iAlpha, iAlphaMat.fMat); }

inline AlphaMat& operator*=(AlphaMat& ioLHS, const Alpha& iRHS)
	{
	ioLHS.fAlpha *= iRHS;
	return ioLHS;
	}

inline AlphaMat operator*(const AlphaMat& iAlphaMat, const Mat& iMat)
	{ return AlphaMat(iAlphaMat.fAlpha, iAlphaMat.fMat * iMat); }

inline AlphaMat& operator*=(AlphaMat& ioLHS, const Mat& iRHS)
	{
	ioLHS.fMat *= iRHS;
	return ioLHS;
	}

// =================================================================================================
// MARK: -

inline AlphaGainMat operator*(const AlphaGainMat& iLHS, const Alpha& iRHS)
	{ return AlphaGainMat(iLHS.fAlpha * iRHS, iLHS.fGain, iLHS.fMat); }

inline AlphaGainMat& operator*=(AlphaGainMat& ioLHS, const Alpha& iRHS)
	{
	ioLHS.fAlpha *= iRHS;
	return ioLHS;
	}

inline AlphaGainMat operator*(const AlphaGainMat& iLHS, const Gain& iRHS)
	{ return AlphaGainMat(iLHS.fAlpha, iLHS.fGain * iRHS, iLHS.fMat); }

inline AlphaGainMat& operator*=(AlphaGainMat& ioLHS, const Gain& iRHS)
	{
	ioLHS.fGain *= iRHS;
	return ioLHS;
	}

inline AlphaGainMat operator*(const AlphaGainMat& iLHS, const Mat& iRHS)
	{ return AlphaGainMat(iLHS.fAlpha, iLHS.fGain, iLHS.fMat * iRHS); }

inline AlphaGainMat& operator*=(AlphaGainMat& ioLHS, const Mat& iRHS)
	{
	ioLHS.fMat *= iRHS;
	return ioLHS;
	}

// =================================================================================================
// MARK: - NameFrame

struct NameFrame
	{
public:
	NameFrame();
	NameFrame(const Name& iName);
	NameFrame(const Name& iName, size_t iFrame);

	Name fName;
	size_t fFrame;
	};

// =================================================================================================
// MARK: - Cel

struct Cel
	{
	Cel();
	Cel(const NameFrame& iNameFrame, const AlphaMat& iAlphaMat);

	NameFrame fNameFrame;
	AlphaMat fAlphaMat;
	};

// =================================================================================================
// MARK: - Map Get

ZQ<Val> sQGetNamed(const Map& iMap, const Name& iName0);

ZQ<Val> sQGetNamed(const Map& iMap, const Name& iName0, const Name& iName1);

ZQ<Val> sQGetNamed(const Map& iMap,
	const Name& iName0, const Name& iName1, const Name& iName2);

Val sGetNamed(const Map& iMap, const Name& iName0, const Name& iName1);

// =================================================================================================
// MARK: - Rat, from ZQ<Val>

ZQ<Rat> sQRat(const ZQ<Val>& iValQ);
Rat sDRat(Rat iDefault, const ZQ<Val>& iValQ);
Rat sRat(const ZQ<Val>& iValQ);

// =================================================================================================
// MARK: - Rat, from Any

ZQ<Rat> sQRat(const Any& iAny);
Rat sDRat(Rat iDefault, const Any& iAny);
Rat sRat(const Any& iAny);

// =================================================================================================
// MARK: - CVec

ZQ<CVec3> sQCVec3(Rat iIdentity, const ZQ<Val>& iValQ);
CVec3 sDCVec3(const CVec3& iDefault, Rat iIdentity, const ZQ<Val>& iValQ);
CVec3 sCVec3(Rat iIdentity, const ZQ<Val>& iValQ);

ZQ<CVec3> sQCVec3(Rat iIdentity, const Any& iVal);
CVec3 sDCVec3(const CVec3& iDefault, Rat iIdentity, const Any& iVal);
CVec3 sCVec3(Rat iIdentity, const Any& iVal);

CVec3 sCVec3(Rat iX, Rat iY, Rat iZ);

// =================================================================================================
// MARK: - RGBA

ZQ<ZRGBA> sQRGBA(const ZQ<Val>& iValQ);

// =================================================================================================
// MARK: - sRandomInRange

double sRandomInRange(double iMin, double iMax);

// =================================================================================================
// MARK: - operator<<

typedef ChanW_UTF ZStrimW;

const ZStrimW& operator<<(const ZStrimW& w, const Name& iName);
const ZStrimW& operator<<(const ZStrimW& w, const Any& iAny);
const ZStrimW& operator<<(const ZStrimW& w, const Map& iMap);
const ZStrimW& operator<<(const ZStrimW& w, const Seq& iSeq);

} // namespace GameEngine
} // namespace ZooLib

#endif // __GameEngine_Types_h__
