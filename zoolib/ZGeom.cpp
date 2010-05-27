/* -------------------------------------------------------------------------------------------------
Copyright (c) 2000 Andrew Green and Learning in Motion, Inc.
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

#include "zoolib/ZGeom.h"

// AG 98-05-04. Major rework. Because it's possible to have more than one graphics
// API available simultaneously on the same machine (QD and GDI when using QuickTime
// on Windows) we have to be more circumspect about allowing the passing of ZPoints and
// ZRects to OS routines. These types *used* to have conversion operators that could
// implicitly return a pointer to a Point (for example) when a ZPoint was used as a parameter
// to a routine. Handy, but not easy to support when more than one API is active at once.

namespace ZooLib {

template<> const ZPoint_T<int16> ZPoint_T<int16>::sZero(0, 0);
template<> const ZRect_T<int16> ZRect_T<int16>::sZero(0, 0, 0, 0);

template<> const ZPoint_T<int32> ZPoint_T<int32>::sZero(0, 0);
template<> const ZRect_T<int32> ZRect_T<int32>::sZero(0, 0, 0, 0);

template<> const ZPoint_T<float> ZPoint_T<float>::sZero(0, 0);
template<> const ZRect_T<float> ZRect_T<float>::sZero(0, 0, 0, 0);

template<> const ZPoint_T<double> ZPoint_T<double>::sZero(0, 0);
template<> const ZRect_T<double> ZRect_T<double>::sZero(0, 0, 0, 0);

} // namespace ZooLib
