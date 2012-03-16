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

#ifndef __ZCartesian_Netscape_h__
#define __ZCartesian_Netscape_h__ 1
#include "zconfig.h"

#include "zoolib/ZCartesian.h"
#include "zoolib/netscape/ZNetscape.h"

namespace ZooLib {
namespace ZCartesian {

// =================================================================================================
// MARK: - PointTraits<ZNetscape::NPPoint_Z>

template <>
struct PointTraits<ZNetscape::NPPoint_Z>
:	public PointTraits_Std_HV<uint16,ZNetscape::NPPoint_Z,NPRect>
	{
	static Point_t sMake(const Ord_t& iX, const Ord_t& iY)
		{
		const Point_t result = { iY, iX };
		return result;
		}
	};

// =================================================================================================
// MARK: - RectTraits<NPRect>

template <>
struct RectTraits<NPRect>
:	public RectTraits_Std_LeftTopRightBottom<uint16,ZNetscape::NPPoint_Z,NPRect>
	{
	static Rect_t sMake(const Ord_t& iL, const Ord_t& iT, const Ord_t& iR, const Ord_t& iB)
		{
		const Rect_t result = { iT, iL, iB, iR };
		return result;
		}
	};

} // namespace ZCartesian
} // namespace ZooLib


#endif // __ZCartesian_Netscape_h__
