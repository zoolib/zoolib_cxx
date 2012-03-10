/* -------------------------------------------------------------------------------------------------
Copyright (c) 2012 Andrew Green
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

#ifndef __ZCartesian_CG_h__
#define __ZCartesian_CG_h__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_SPI.h"

#include "zoolib/ZCartesian.h"

#if ZCONFIG_SPI_Enabled(CoreGraphics)

#if ZCONFIG_SPI_Enabled(iPhone)
	#include ZMACINCLUDE2(CoreGraphics,CGGeometry.h)
#else
	#include ZMACINCLUDE3(ApplicationServices,CoreGraphics,CGGeometry.h)
#endif

namespace ZooLib {
namespace ZCartesian {

// =================================================================================================
// MARK: - PointTraits<CGPoint>

template <>
struct PointTraits<CGPoint>
:	public PointTraits_Std_XY<CGFloat,CGPoint,CGRect>
	{
	static Point_t sMake(const Ord_t& iX, const Ord_t& iY)
		{ return CGPointMake(iX, iY); }
	};

// =================================================================================================
// MARK: - PointTraits<CGSize>

template <>
struct PointTraits<CGSize>
:	public PointTraits_Std_WidthHeight<CGFloat,CGSize,CGRect>
	{
	static Point_t sMake(const Ord_t& iX, const Ord_t& iY)
		{ return CGSizeMake(iX, iY); }
	};

// =================================================================================================
// MARK: - RectTraits<CGRect>

template <>
struct RectTraits<CGRect>
:	public RectTraits_Std_OriginSize<CGFloat,CGPoint,CGSize,CGRect>
	{
	static Rect_t sMake(const Ord_t& iL, const Ord_t& iT, const Ord_t& iR, const Ord_t& iB)
		{ return CGRectMake(iL, iT, iR - iL, iB - iT); }
	};

} // namespace ZCartesian
} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(CoreGraphics)
#endif // __ZCartesian_CG_h__
