// Copyright (c) 2019 Andrew Green. MIT License. http://www.zoolib.org

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
