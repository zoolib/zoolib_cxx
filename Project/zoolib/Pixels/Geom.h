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

#ifndef __ZooLib_Pixels_Geom_h__
#define __ZooLib_Pixels_Geom_h__ 1
#include "zconfig.h"


namespace ZooLib {
namespace Pixels {

// =================================================================================================
#pragma mark -

typedef int Ord;

// =================================================================================================
#pragma mark - PointPOD

struct PointPOD
	{
	Ord h;
	Ord v;
	};

inline PointPOD sPointPOD(Ord iH, Ord iV)
	{
	const PointPOD result = {iH, iV};
	return result;
	}

inline PointPOD sPointPOD(Ord iHV)
	{ return sPointPOD(iHV, iHV); }

// =================================================================================================
#pragma mark - RectPOD

struct RectPOD
	{
	Ord left;
	Ord top;
	Ord right;
	Ord bottom;
	};

inline RectPOD sRectPOD(Ord iLeft, Ord iTop, Ord iRight, Ord iBottom)
	{
	const RectPOD result = {iLeft, iTop, iRight, iBottom};
	return result;
	}

inline RectPOD sRectPOD(Ord iWidth, Ord iHeight)
	{ return sRectPOD(0, 0, iWidth, iHeight); }

} // namespace Pixels
} // namespace ZooLib

#endif // __ZooLib_Pixels_Geom_h__
