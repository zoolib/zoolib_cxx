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

#ifndef __ZCartesian_GDI_h__
#define __ZCartesian_GDI_h__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_SPI.h"

#include "zoolib/ZCartesian.h"

#if ZCONFIG_SPI_Enabled(GDI)

#include "zoolib/ZCompat_Win.h"

namespace ZooLib {
namespace ZCartesian {

// =================================================================================================
// MARK: - POINT

template <>
struct PointTraits<POINT>
:	public PointTraitsStd_XY<LONG,POINT,RECT>
	{
	static Point_t sMake(const Ord_t& iX, const Ord_t& iY)
		{
		const POINT result = { iX, iY };
		return result;
		}
	};

// =================================================================================================
// MARK: - RECT

template <>
struct RectTraits<CGRect>
:	public RectTraitsStd_LTRB<LONG,POINT,RECT>
	{
	static Rect_t sMake(const Ord_t& iL, const Ord_t& iT, const Ord_t& iR, const Ord_t& iB)
		{
		const RECT result = {iL, iT, iR, iB };
		return result;
		}
	};

} // namespace ZCartesian
} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(GDI)
#endif // __ZCartesian_GDI_h__
