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

#ifndef __ZUtil_Graphics_h__
#define __ZUtil_Graphics_h__ 1
#include "zconfig.h"

#include "zoolib/ZGeom.h"
#include "zoolib/ZDCRgn.h"

namespace ZooLib {

class ZDC;
class ZDCPattern;
class ZDCPixmap;

class ZStreamNP;

namespace ZUtil_Graphics {

void sCalculateRgnDifferences(const ZDCRgn& inOldRgn, const ZDCRgn& inNewRgn,
	ZDCRgn* outOldRgnToPaint, ZDCRgn* outNewRgnToPaint, ZDCRgn* outDiffRgnToPaint);

void sCalculatePatternDifference(const ZDCPattern& inOldPattern, const ZDCPattern& inNewPattern,
	ZDCPattern* outDiffPattern, bool* outPatternChanged);

void sMarchAnts(ZDCPattern& ioPattern);

void sDrawDragDropHilite(const ZDC& inDC, const ZDCRgn& inRgn);

ZDCRgn sCalculateRgnStrokedByLine(ZCoord inStartH, ZCoord inStartV,
	ZCoord inEndH, ZCoord inEndV, ZCoord inPenWidth);

ZDCRgn sCalculateRgnStrokedByLine(ZPoint inStart, ZPoint inEnd, ZCoord inPenWidth);

typedef void (*BresenhamProc)(ZPoint inPoint, void* inRefcon);
void sBresenham(ZPoint inStartPoint, ZPoint inEndPoint, BresenhamProc inProc, void* inRefcon);

ZDCRgn sCalculateRgnStrokedByFatLine(const ZDCRgn& inPenRgn,
	ZPoint inStartPoint, ZPoint inEndPoint);

void sDrawFatLine(const ZDC& inDC, const ZDCRgn& inPenRgn,
	ZPoint inStartPoint, ZPoint inEndPoint);

ZRect sCalcHandleBounds9(const ZRect& inBounds, ZCoord inHandleSize, size_t inHandleIndex);

} // namespace ZUtil_Graphics

} // namespace ZooLib

#endif // __ZUtil_Graphics_h__
