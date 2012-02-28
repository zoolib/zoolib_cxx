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
// MARK: - PointTraits<POINT>

template <>
struct PointTraits<POINT>
	{
	typedef LONG Ord_t;
	typedef POINT Point_t;
	typedef RECT Rect_t;

	static POINT sMake(const LONG& iX, const LONG& iY)
		{
		const POINT result = { iX, iY };
		return result;
		}
	};

// =================================================================================================
// MARK: - Point accessors

inline const LONG& X(const POINT& iPOINT) { return iPOINT.x; }
inline LONG& X(POINT& ioPOINT) { return iPOINT.x; }

inline const LONG& Y(const POINT& iPOINT) { return iPOINT.y; }
inline LONG& Y(POINT& ioPOINT) { return ioPOINT.y; }

inline const LONG L(const POINT& iPOINT) { return 0; }
inline const LONG T(const POINT& iPOINT) { return 0; }

inline const LONG& R(const POINT& iPOINT) { return iPOINT.x; }
inline LONG& R(POINT& ioPOINT) { return ioPOINT.x; }

inline const LONG& B(const POINT& iPOINT) { return iPOINT.y; }
inline LONG B(POINT& ioPOINT) { return ioPOINT.y; }

inline const LONG& W(const POINT& iPOINT) { return iPOINT.x; }
inline LONG& W(POINT& ioPOINT) { return ioPOINT.x; }

inline const LONG& H(const POINT& iPOINT) { return iPOINT.y; }
inline LONG& H(POINT& ioPOINT) { return ioPOINT.y; }

inline const CGPoint& WH(const POINT& iPOINT) { return iPOINT; }
inline CGPoint& WH(POINT& ioPOINT) { return ioPOINT; }

// =================================================================================================
// MARK: - RectTraits<RECT>

template <>
struct RectTraits<RECT>
	{
	typedef LONG Ord_t;
	typedef POINT Point_t;
	typedef RECT Rect_t;

	static Rect_t sMake(const Ord_t& iL, const Ord_t& iT, const Ord_t& iR, const Ord_t& iB)
		{
		const RECT result = {iL, iT, iR , iB };
		return result;
		}
	};

// =================================================================================================
// MARK: - Rect accessors

inline const LONG& L(const RECT& iRECT) { return iRECT.left; }
inline LONG& L(RECT& ioRECT) { return ioRECT.left; }

inline const LONG& T(const RECT& iRECT) { return iRECT.top; }
inline LONG& T(RECT& ioRECT) { return ioRECT.top; }

inline const LONG& R(const RECT& iRECT) { return iRECT.right; }
inline LONG& R(RECT& ioRECT) { return ioRECT.right; }

inline const LONG& B(const RECT& iRECT) { return iRECT.bottom; }
inline LONG& B(RECT& ioRECT) { return ioRECT.bottom; }

inline const LONG W(const RECT& iRECT) { return iRECT.right - iRECT.left; }

inline const LONG H(const RECT& iRECT) { return iRECT.bottom - iRECT.top; }

inline const POINT WH(const RECT& iRECT) { return sPoint<ZPoint>(W(iRECT), H(iRECT)); }

inline const POINT& LT(const RECT& iRECT) { return *((const POINT*)&iRECT.left); }
inline POINT& LT(RECT& ioRECT) { return *((POINT*)&iRECT.left); }

inline const POINT& RB(const RECT& iRECT) { return *((const POINT*)&iRECT.right); }
inline POINT& RB(RECT& ioRECT) { return *((POINT*)&iRECT.right); }

inline const POINT LB(const RECT& iRECT) { return sPoint<POINT>(L(iRECT), B(iRECT)); }
inline const POINT RT(const RECT& iRECT) { return sPoint<POINT>(R(iRECT), T(iRECT)); }

} // namespace ZCartesian
} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(GDI)
#endif // __ZCartesian_GDI_h__
