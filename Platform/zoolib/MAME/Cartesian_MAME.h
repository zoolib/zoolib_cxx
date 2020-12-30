// Copyright (c) 2020 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_MAME_Cartesian_MAME_h__
#define __ZooLib_MAME_Cartesian_MAME_h__ 1
#include "zconfig.h"

#include "zoolib/Pixels/Cartesian_Geom.h" // For Cartesian, Pixels::Ord, Pixels::PointPOD etc

#include "bitmap.h" // For MAME's rectangle.

namespace ZooLib {
namespace Cartesian {

// =================================================================================================
#pragma mark - RectTraits<rectangle>

// Pixels::Ord is an int, which matches what we want for rectangle

template <>
struct RectTraits<rectangle>
:	public RectTraits_Std<Pixels::Ord,Pixels::PointPOD,rectangle>
	{
	typedef const Ord_t& LC_t;
	static LC_t sL(const Rect_t& iRect) { return iRect.min_x; }

	typedef Ord_t& L_t;
	static L_t sL(Rect_t& ioRect) { return ioRect.min_x; }

	typedef const Ord_t& TC_t;
	static TC_t sT(const Rect_t& iRect) { return iRect.min_y; }

	typedef Ord_t& T_t;
	static T_t sT(Rect_t& ioRect) { return ioRect.min_y; }

	typedef Ord_t RC_t;
	static RC_t sR(const Rect_t& iRect) { return iRect.max_x + 1; }

	typedef const Ord_t BC_t;
	static BC_t sB(const Rect_t& iRect) { return iRect.max_y + 1; }

	static Rect_t sMake(const Ord_t& iL, const Ord_t& iT, const Ord_t& iR, const Ord_t& iB)
		{ return rectangle(iL, iR - 1, iT, iB - 1); }
	};

} // namespace Cartesian

} // namespace ZooLib

#endif // __ZooLib_MAME_Cartesian_MAME_h__
