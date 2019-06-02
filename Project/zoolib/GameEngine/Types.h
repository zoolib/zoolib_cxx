#ifndef __ZooLib_GameEngine_Types_h__
#define __ZooLib_GameEngine_Types_h__ 1

#include "zoolib/Cartesian_Matrix.h"
#include "zoolib/ChanW_UTF.h"
#include "zoolib/Map_Shared_T.h"
#include "zoolib/Name.h"
#include "zoolib/TagVal.h"
#include "zoolib/Val_Any.h"

#include "zoolib/ARM/Compat_arm.h"

#include "zoolib/GameEngine/Geometry3D.h"
#include "zoolib/GameEngine/Val.h"

#include "zoolib/Pixels/Cartesian_Geom.h"
#include "zoolib/Pixels/RGBA.h"

#include "zoolib/ZMACRO_auto.h"

#define GEMACRO_Callable(callable, function) \
	static const ZMACRO_auto(callable, sCallable(function));

namespace ZooLib {
namespace GameEngine {

using Pixels::PointPOD;
using Pixels::sPointPOD;

using Pixels::RectPOD;
using Pixels::sRectPOD;

using Pixels::RGBA;
using Pixels::sRGBA;
using Pixels::sRed;
using Pixels::sGreen;
using Pixels::sBlue;
using Pixels::sAlpha;

// =================================================================================================

typedef ZMtx MtxF;
typedef ZAcqMtx AcqMtxF;

// =================================================================================================
#pragma mark - Fundamental types

typedef float Rat;

typedef Matrix<Rat,4,4> Mat;

typedef Matrix<Rat,1,2> CVec2;
typedef Matrix<Rat,1,3> CVec3;
typedef Matrix<Rat,1,4> CVec4;

typedef MatrixArray<Rat,1,2,2> GRect;
typedef Matrix<Rat,1,2> GPoint;

typedef Map_Shared_T<Map> SharedState;
typedef Map_Shared_Mutable_T<Map> SharedState_Mutable;

// =================================================================================================
#pragma mark - GRect pseudo-ctors

inline GRect sGRect()
	{ return GRect(); }

inline GRect sGRect(Rat iX, Rat iY)
	{ return sRect<GRect>(iX, iY); }

inline GRect sGRect(Rat iL, Rat iT, Rat iR, Rat iB)
	{ return sRect<GRect>(iL, iT, iR, iB); }

inline GPoint sGPoint()
	{ return GPoint(); }

inline GPoint sGPoint(Rat iX, Rat iY)
	{ return sPoint<GPoint>(iX, iY); }

// =================================================================================================
#pragma mark - Blush

typedef RGBA Blush;

// =================================================================================================
#pragma mark - Gain

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
#pragma mark - BlushMat

struct BlushMat
	{
	BlushMat()
	:	fBlush(1)
	,	fMat(1)
		{}

	BlushMat(const BlushMat& iOther)
	:	fBlush(iOther.fBlush)
	,	fMat(iOther.fMat)
		{}

	BlushMat(Blush iBlush)
	:	fBlush(iBlush)
	,	fMat(1)
		{}

	BlushMat(const Mat& iMat)
	:	fBlush(1)
	,	fMat(iMat)
		{}

	BlushMat(Blush iBlush, const Mat& iMat)
	:	fBlush(iBlush)
	,	fMat(iMat)
		{}

	Blush fBlush;
	Mat fMat;
	};

inline BlushMat operator*(const BlushMat& iLHS, const BlushMat& iRHS)
	{ return BlushMat(iLHS.fBlush * iRHS.fBlush, iLHS.fMat * iRHS.fMat); }

inline BlushMat& operator*=(BlushMat& ioLHS, const BlushMat& iRHS)
	{
	ioLHS.fBlush *= iRHS.fBlush;
	ioLHS.fMat *= iRHS.fMat;
	return ioLHS;
	}

// =================================================================================================
#pragma mark - BlushGainMat

struct BlushGainMat
	{
	BlushGainMat()
	:	fBlush(1)
	,	fGain(1)
	,	fMat(1)
		{}

	BlushGainMat(const BlushGainMat& iOther)
	:	fBlush(iOther.fBlush)
	,	fGain(iOther.fGain)
	,	fMat(iOther.fMat)
		{}

	BlushGainMat(Blush iBlush)
	:	fBlush(iBlush)
	,	fGain(1)
	,	fMat(1)
		{}

	BlushGainMat(const Gain& iGain)
	:	fBlush(1)
	,	fGain(iGain)
	,	fMat(1)
		{}

	BlushGainMat(const Mat& iMat)
	:	fBlush(1)
	,	fGain(1)
	,	fMat(iMat)
		{}

	BlushGainMat(Blush iBlush, const Gain& iGain)
	:	fBlush(iBlush)
	,	fGain(iGain)
	,	fMat(1)
		{}

	BlushGainMat(const Gain& iGain, const Mat& iMat)
	:	fBlush(1)
	,	fGain(iGain)
	,	fMat(iMat)
		{}

	BlushGainMat(Blush iBlush, const Mat& iMat)
	:	fBlush(iBlush)
	,	fMat(iMat)
		{}

	BlushGainMat(Blush iBlush, const Gain& iGain, const Mat& iMat)
	:	fBlush(iBlush)
	,	fGain(iGain)
	,	fMat(iMat)
		{}

	BlushGainMat(const BlushMat& iBlushMat)
	:	fBlush(iBlushMat.fBlush)
	,	fGain(1)
	,	fMat(iBlushMat.fMat)
		{}


	Blush fBlush;
	Gain fGain;
	Mat fMat;
	};

inline BlushGainMat operator*(const BlushGainMat& iLHS, const BlushGainMat& iRHS)
	{
	return BlushGainMat(
		iLHS.fBlush * iRHS.fBlush,
		iLHS.fGain * iRHS.fGain,
		iLHS.fMat * iRHS.fMat);
	}

inline BlushGainMat& operator*=(BlushGainMat& ioLHS, const BlushGainMat& iRHS)
	{
	ioLHS.fBlush *= iRHS.fBlush;
	ioLHS.fGain *= iRHS.fGain;
	ioLHS.fMat *= iRHS.fMat;
	return ioLHS;
	}

// =================================================================================================
#pragma mark - Pseudo-ctors, BlushGainMat --> Blush, Gain, Mat

inline Blush sBlush(const BlushGainMat& iBlushGainMat)
	{ return iBlushGainMat.fBlush; }

inline Gain sGain(const BlushGainMat& iBlushGainMat)
	{ return iBlushGainMat.fGain; }

inline Mat sMat(const BlushGainMat& iBlushGainMat)
	{ return iBlushGainMat.fMat; }

inline Blush sBlush(const BlushMat& iBlushMat)
	{ return iBlushMat.fBlush; }

inline Mat sMat(const BlushMat& iBlushMat)
	{ return iBlushMat.fMat; }

inline BlushMat sBlushMat(const BlushGainMat& iBlushGainMat)
	{ return BlushMat(iBlushGainMat.fBlush, iBlushGainMat.fMat); }

// =================================================================================================
#pragma mark -

inline BlushMat operator*(const Blush& iBlush, const Mat& iMat)
	{ return BlushMat(iBlush, iMat); }

inline BlushMat operator*(const Mat& iMat, const Blush& iBlush)
	{ return BlushMat(iBlush, iMat); }

inline BlushGainMat operator*(const BlushMat& iBlushMat, const Gain& iGain)
	{ return BlushGainMat(iBlushMat.fBlush, iGain, iBlushMat.fMat); }

inline BlushGainMat operator*(const Gain& iGain, const BlushMat& iBlushMat)
	{ return BlushGainMat(iBlushMat.fBlush, iGain, iBlushMat.fMat); }

// =================================================================================================
#pragma mark -

inline BlushMat operator*(const BlushMat& iBlushMat, const Blush& iBlush)
	{ return BlushMat(iBlushMat.fBlush * iBlush, iBlushMat.fMat); }

inline BlushMat& operator*=(BlushMat& ioLHS, const Blush& iRHS)
	{
	ioLHS.fBlush *= iRHS;
	return ioLHS;
	}

inline BlushMat operator*(const BlushMat& iBlushMat, const Mat& iMat)
	{ return BlushMat(iBlushMat.fBlush, iBlushMat.fMat * iMat); }

inline BlushMat& operator*=(BlushMat& ioLHS, const Mat& iRHS)
	{
	ioLHS.fMat *= iRHS;
	return ioLHS;
	}

// =================================================================================================
#pragma mark -

inline BlushGainMat operator*(const BlushGainMat& iLHS, const Blush& iRHS)
	{ return BlushGainMat(iLHS.fBlush * iRHS, iLHS.fGain, iLHS.fMat); }

inline BlushGainMat& operator*=(BlushGainMat& ioLHS, const Blush& iRHS)
	{
	ioLHS.fBlush *= iRHS;
	return ioLHS;
	}

inline BlushGainMat operator*(const BlushGainMat& iLHS, const Gain& iRHS)
	{ return BlushGainMat(iLHS.fBlush, iLHS.fGain * iRHS, iLHS.fMat); }

inline BlushGainMat& operator*=(BlushGainMat& ioLHS, const Gain& iRHS)
	{
	ioLHS.fGain *= iRHS;
	return ioLHS;
	}

inline BlushGainMat operator*(const BlushGainMat& iLHS, const Mat& iRHS)
	{ return BlushGainMat(iLHS.fBlush, iLHS.fGain, iLHS.fMat * iRHS); }

inline BlushGainMat& operator*=(BlushGainMat& ioLHS, const Mat& iRHS)
	{
	ioLHS.fMat *= iRHS;
	return ioLHS;
	}

// =================================================================================================
#pragma mark - NameFrame

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
#pragma mark - Cel

struct Cel
	{
	Cel();
	Cel(const NameFrame& iNameFrame, const BlushMat& iBlushMat);

	NameFrame fNameFrame;
	BlushMat fBlushMat;
	};

// =================================================================================================
#pragma mark - Map Get

ZQ<Val> sQGetNamed(const Map& iMap, const Name& iName0);

ZQ<Val> sQGetNamed(const Map& iMap, const Name& iName0, const Name& iName1);

ZQ<Val> sQGetNamed(const Map& iMap,
	const Name& iName0, const Name& iName1, const Name& iName2);

Val sGetNamed(const Map& iMap, const Name& iName0, const Name& iName1);

// =================================================================================================
#pragma mark - Rat, from Any

ZQ<Rat> sQRat(const Any& iAny);
Rat sDRat(Rat iDefault, const Any& iAny);
Rat sRat(const Any& iAny);

// =================================================================================================
#pragma mark - Rat, from ZQ<Val>

ZQ<Rat> sQRat(const ZQ<Val>& iValQ);
Rat sDRat(Rat iDefault, const ZQ<Val>& iValQ);
Rat sRat(const ZQ<Val>& iValQ);

// =================================================================================================
#pragma mark - CVec3, from Any

ZQ<CVec3> sQCVec3(Rat iIdentity, const Any& iVal);
CVec3 sDCVec3(const CVec3& iDefault, Rat iIdentity, const Any& iVal);
CVec3 sCVec3(Rat iIdentity, const Any& iVal);

// =================================================================================================
#pragma mark - CVec3, from ZQ<Val>

ZQ<CVec3> sQCVec3(Rat iIdentity, const ZQ<Val>& iValQ);
CVec3 sDCVec3(const CVec3& iDefault, Rat iIdentity, const ZQ<Val>& iValQ);
CVec3 sCVec3(Rat iIdentity, const ZQ<Val>& iValQ);

CVec3 sCVec3(Rat iX, Rat iY, Rat iZ);

// =================================================================================================
#pragma mark - RGBA

ZQ<RGBA> sQRGBA(const string8& iString);

ZQ<RGBA> sQRGBA(const Any& iVal);

ZQ<RGBA> sQRGBA(const ZQ<Val>& iValQ);

// =================================================================================================
#pragma mark - sRandomInRange

double sRandomInRange(double iMin, double iMax);

} // namespace GameEngine
} // namespace ZooLib

#endif // __GameEngine_Types_h__
