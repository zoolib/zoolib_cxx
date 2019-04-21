/* -------------------------------------------------------------------------------------------------
Copyright (c) 2019 Andrew Green
http://www.zoolib.org

Permission is hereby granted, free of charge, to any person obtaining a copy of this software
and associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software
is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE COPYRIGHT HOLDER(S) BE LIABLE FOR ANY CLAIM, DAMAGES
OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
------------------------------------------------------------------------------------------------- */

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

inline float& sRed(RGBA& iRGBA)
	{ return iRGBA[0]; }

inline float sRed(const RGBA& iRGBA)
	{ return iRGBA[0]; }

inline float& sGreen(RGBA& iRGBA)
	{ return iRGBA[1]; }

inline float sGreen(const RGBA& iRGBA)
	{ return iRGBA[1]; }

inline float& sBlue(RGBA& iRGBA)
	{ return iRGBA[2]; }

inline float sBlue(const RGBA& iRGBA)
	{ return iRGBA[2]; }

inline float& sAlpha(RGBA& iRGBA)
	{ return iRGBA[3]; }

inline float sAlpha(const RGBA& iRGBA)
	{ return iRGBA[3]; }

} // namespace Pixels
} // namespace ZooLib

#endif // __ZooLib_Pixels_RGBA_h__
