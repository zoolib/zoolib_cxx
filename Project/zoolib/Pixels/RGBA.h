// Copyright (c) 2019 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Pixels_RGBA_h__
#define __ZooLib_Pixels_RGBA_h__ 1
#include "zconfig.h"

#include "zoolib/Matrix.h"

namespace ZooLib {
namespace Pixels {

typedef float Comp;

typedef Matrix<Comp,1,4> RGBA;

inline RGBA sRGBA(Comp iRed, Comp iGreen, Comp iBlue, Comp iAlpha)
	{ return RGBA({iRed, iGreen, iBlue, iAlpha}); }

inline RGBA sRGBA(Comp iRed, Comp iGreen, Comp iBlue)
	{ return sRGBA(iRed, iGreen, iBlue, 1.0); }

inline RGBA sRGBA(Comp iGray, Comp iAlpha)
	{ return sRGBA(iGray, iGray, iGray, iAlpha); }

inline RGBA sRGBA(Comp iGray)
	{ return sRGBA(iGray, iGray, iGray, 1.0); }

inline Comp& sRed(RGBA& iRGBA)
	{ return iRGBA[0]; }

inline Comp sRed(const RGBA& iRGBA)
	{ return iRGBA[0]; }

inline Comp& sGreen(RGBA& iRGBA)
	{ return iRGBA[1]; }

inline Comp sGreen(const RGBA& iRGBA)
	{ return iRGBA[1]; }

inline Comp& sBlue(RGBA& iRGBA)
	{ return iRGBA[2]; }

inline Comp sBlue(const RGBA& iRGBA)
	{ return iRGBA[2]; }

inline Comp& sAlpha(RGBA& iRGBA)
	{ return iRGBA[3]; }

inline Comp sAlpha(const RGBA& iRGBA)
	{ return iRGBA[3]; }

} // namespace Pixels
} // namespace ZooLib

#endif // __ZooLib_Pixels_RGBA_h__
