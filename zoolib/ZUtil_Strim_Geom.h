/* -------------------------------------------------------------------------------------------------
Copyright (c) 2009 Andrew Green
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

#ifndef __ZUtil_Strim_Geom__
#define __ZUtil_Strim_Geom__ 1
#include "zconfig.h"

#include "zoolib/ZGeom.h"
#include "zoolib/ZGeometry.h"
#include "zoolib/ZStrim.h"

namespace ZooLib {

template <class C>
inline const ZStrimW& operator<<(const ZStrimW& s, const ZRect_T<C>& val)
	{
	return s.Writef("Rect(%g, %g, %g, %g)",
		double(val.left), double(val.top),
		double(val.right), double(val.bottom));
	}

template <class C>
inline const ZStrimW& operator<<(const ZStrimW& s, const ZPoint_T<C>& val)
	{ return s.Writef("(%g, %g)", double(val.h), double(val.v)); }

template <class C>
inline const ZStrimW& operator<<(const ZStrimW& s, const ZGPointPOD_T<C>& val)
	{ return s.Writef("(%g, %g)", double(val.x), double(val.y)); }

template <class C>
inline const ZStrimW& operator<<(const ZStrimW& s, const ZGExtentPOD_T<C>& val)
	{ return s.Writef("(%g, %g)", double(val.h), double(val.v)); }

template <class C>
inline const ZStrimW& operator<<(const ZStrimW& s, const ZGRectPOD_T<C>& val)
	{
	s << "(" << val.origin << ", " << val.extent << ")";
	return s;
	}

} // namespace ZooLib

#endif // __ZUtil_Strim_Geom__
