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

#include "zoolib/ZMacFixup.h"

// ZDCPoly::sDecompose and its helper methods are derived from X Windows source.

/************************************************************************

Copyright (c) 1987, 1988 X Consortium

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
X CONSORTIUM BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of the X Consortium shall not be
used in advertising or otherwise to promote the sale, use or other dealings
in this Software without prior written authorization from the X Consortium.


Copyright 1987, 1988 by Digital Equipment Corporation, Maynard, Massachusetts.

			All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the name of Digital not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.

DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

************************************************************************/

#include "zoolib/ZDCPoly.h"
#include "zoolib/ZDebug.h"
#include "zoolib/ZMemory.h" // For ZMemCopy

#if ZCONFIG_SPI_Enabled(QuickDraw)
	#include ZMACINCLUDE3(CoreServices,CarbonCore,MacMemory.h) // For HLock etc.
#endif

using ZooLib::ZCoord;

// Linux and Be headers currently don't provide <limits>, so we have to futz around.
#if ZCONFIG_SPI_Enabled(BeOS)
	#include <climits>
	static const ZCoord sCoord_Max = LONG_MAX;
	static const ZCoord sCoord_Min = LONG_MIN;
//#elif ZCONFIG(OS, POSIX)
//	#include <climits>
//	static const ZCoord sCoord_Max = SHRT_MAX;
//	static const ZCoord sCoord_Min = SHRT_MIN;
#else
	#include <limits>
	static const ZCoord sCoord_Max = std::numeric_limits<ZCoord>::max();
	static const ZCoord sCoord_Min = std::numeric_limits<ZCoord>::min();
#endif

namespace ZooLib {

using std::min;
using std::max;

// =================================================================================================
// MARK: - ZDCPoly

ZDCPoly::ZDCPoly()
	{
	fOrigin = ZPoint::sZero;

	fCachedPoints = nullptr;

#if ZCONFIG_SPI_Enabled(QuickDraw)
	fCachedPolyHandle = nullptr;
#endif

#if ZCONFIG_SPI_Enabled(GDI)
	fCachedPOINTs = nullptr;
#endif

#if ZCONFIG_SPI_Enabled(X11)
	fCachedXPoints = nullptr;
#endif

#if ZCONFIG_SPI_Enabled(BeOS)
	fCachedBPoints = nullptr;
#endif
	}

ZDCPoly::ZDCPoly(const ZPoint& iStartPoint)
	{
	fOrigin = ZPoint::sZero;
	fPoints.push_back(iStartPoint);

	fCachedPoints = nullptr;

#if ZCONFIG_SPI_Enabled(QuickDraw)
	fCachedPolyHandle = nullptr;
#endif

#if ZCONFIG_SPI_Enabled(GDI)
	fCachedPOINTs = nullptr;
#endif

#if ZCONFIG_SPI_Enabled(X11)
	fCachedXPoints = nullptr;
#endif

#if ZCONFIG_SPI_Enabled(BeOS)
	fCachedBPoints = nullptr;
#endif
	}

ZDCPoly::ZDCPoly(ZCoord iPointH, ZCoord iPointV)
	{
	fOrigin = ZPoint::sZero;
	fPoints.push_back(ZPoint(iPointH, iPointV));

	fCachedPoints = nullptr;

#if ZCONFIG_SPI_Enabled(QuickDraw)
	fCachedPolyHandle = nullptr;
#endif

#if ZCONFIG_SPI_Enabled(GDI)
	fCachedPOINTs = nullptr;
#endif

#if ZCONFIG_SPI_Enabled(X11)
	fCachedXPoints = nullptr;
#endif

#if ZCONFIG_SPI_Enabled(BeOS)
	fCachedBPoints = nullptr;
#endif
	}

ZDCPoly::ZDCPoly(const ZDCPoly& iOther)
:	fPoints(iOther.fPoints),
	fOrigin(iOther.fOrigin)
	{
	fCachedPoints = nullptr;

#if ZCONFIG_SPI_Enabled(QuickDraw)
	fCachedPolyHandle = nullptr;
#endif

#if ZCONFIG_SPI_Enabled(GDI)
	fCachedPOINTs = nullptr;
#endif

#if ZCONFIG_SPI_Enabled(X11)
	fCachedXPoints = nullptr;
#endif

#if ZCONFIG_SPI_Enabled(BeOS)
	fCachedBPoints = nullptr;
#endif
	}

ZDCPoly::~ZDCPoly()
	{ this->pEmptyCache(); }

ZDCPoly& ZDCPoly::operator=(const ZDCPoly& iOther)
	{
	if (&iOther == this)
		return *this;
	this->pEmptyCache();
	fPoints = iOther.fPoints;
	fOrigin = iOther.fOrigin;
	return *this;
	}

void ZDCPoly::Add(const ZPoint& iPoint)
	{
	this->pEmptyCache();
	fPoints.push_back(iPoint - fOrigin);
	}

ZDCPoly ZDCPoly::operator+(const ZPoint& iPoint) const
	{
	ZDCPoly newPoly(*this);
	return newPoly += iPoint;
	}

ZDCPoly ZDCPoly::operator-(const ZPoint& iPoint) const
	{
	ZDCPoly newPoly(*this);
	return newPoly -= iPoint;
	}

ZDCPoly& ZDCPoly::operator+=(const ZPoint& iPoint)
	{
	fOrigin += iPoint;
	return *this;
	}

ZDCPoly& ZDCPoly::operator-=(const ZPoint& iPoint)
	{
	fOrigin -= iPoint;
	return *this;
	}

void ZDCPoly::GetPoints(const ZPoint& iOrigin, ZPoint*& oPoints, size_t& oCount) const
	{
	ZDCPoly* nonConstThis = const_cast<ZDCPoly*>(this);
	ZPoint localOrigin = fOrigin + iOrigin;
	if (fCachedPoints == nullptr || fCachedOrigin != localOrigin)
		{
		if (fPoints.size() > 0)
			{
			ZPoint* thePoints = fCachedPoints;
			if (thePoints == nullptr)
				thePoints = new ZPoint[fPoints.size()];
			for (size_t x = 0; x < fPoints.size(); ++x)
				thePoints[x] = fPoints[x] + localOrigin;
			nonConstThis->fCachedPoints = thePoints;
			nonConstThis->fCachedOrigin = iOrigin;
			}
		}
	oPoints = nonConstThis->fCachedPoints;
	oCount = fPoints.size();
	}

#if ZCONFIG_SPI_Enabled(QuickDraw)
PolyHandle ZDCPoly::GetPolyHandle(const ZPoint& iOrigin) const
	{
	ZDCPoly* nonConstThis = const_cast<ZDCPoly*>(this);
	ZPoint localOrigin = fOrigin + iOrigin;
	if (fCachedPolyHandle == nullptr || fCachedOrigin != localOrigin)
		{
		if (fPoints.size() > 0)
			{
			PolyHandle thePolyHandle = fCachedPolyHandle;
		
			size_t pointsCount = fPoints.size();
			bool isOpen = true;
			if (fPoints[0].h != fPoints[pointsCount-1].h
				|| fPoints[0].v != fPoints[pointsCount-1].v)
				{
				isOpen = false;
				}

			if (thePolyHandle == nullptr)
				{
				size_t theSize = sizeof(short) + sizeof(Rect) + sizeof(Point) * pointsCount;
				if (isOpen)
					theSize += sizeof(Point);
				thePolyHandle = (PolyHandle)::NewHandle(theSize);
				thePolyHandle[0]->polySize = theSize;
				}

			::HLock((Handle)thePolyHandle);
			Rect* theBounds = &thePolyHandle[0]->polyBBox;
			theBounds->left = 32767;
			theBounds->top = 32767;
			theBounds->right = -32768;
			theBounds->bottom = -32768;
			Point* destPtr = thePolyHandle[0]->polyPoints;
			for (size_t x = 0; x < pointsCount; ++x)
				{
				ZPoint currentPoint = fPoints[x] + localOrigin;
				if (theBounds->left > currentPoint.h)
					theBounds->left = currentPoint.h;
				if (theBounds->top > currentPoint.v)
					theBounds->top = currentPoint.v;
				if (theBounds->right < currentPoint.h)
					theBounds->right = currentPoint.h;
				if (theBounds->bottom < currentPoint.v)
					theBounds->bottom = currentPoint.v;
				*destPtr++ = currentPoint;
				}
			if (isOpen)
				{
				destPtr->h = fPoints[0].h + localOrigin.h;
				destPtr->v = fPoints[0].v + localOrigin.v;
				}
			::HUnlock((Handle)thePolyHandle);
			nonConstThis->fCachedPolyHandle = thePolyHandle;
			nonConstThis->fCachedOrigin = localOrigin;
			}
		}
	return nonConstThis->fCachedPolyHandle;
	}
#endif // ZCONFIG_SPI_Enabled(QuickDraw)

#if ZCONFIG_SPI_Enabled(GDI)
void ZDCPoly::GetPOINTs(const ZPoint& iOrigin, POINT*& oPOINTs, size_t& oCount) const
	{
	ZDCPoly* nonConstThis = const_cast<ZDCPoly*>(this);
	ZPoint localOrigin = fOrigin + iOrigin;
	if (fCachedPOINTs == nullptr || fCachedOrigin != localOrigin)
		{
		if (fPoints.size() > 0)
			{
			POINT* thePOINTs = fCachedPOINTs;
			if (thePOINTs == nullptr)
				thePOINTs = new POINT[fPoints.size()];
			for (size_t x = 0; x < fPoints.size(); ++x)
				thePOINTs[x] = fPoints[x] + localOrigin;
			nonConstThis->fCachedPOINTs = thePOINTs;
			nonConstThis->fCachedOrigin = localOrigin;
			}
		}
	oPOINTs = nonConstThis->fCachedPOINTs;
	oCount = fPoints.size();
	}
#endif // ZCONFIG_SPI_Enabled(GDI)

#if ZCONFIG_SPI_Enabled(X11)
void ZDCPoly::GetXPoints(const ZPoint& iOrigin, XPoint*& oXPoints, size_t& oCount) const
	{
	ZDCPoly* nonConstThis = const_cast<ZDCPoly*>(this);
	ZPoint localOrigin = fOrigin + iOrigin;
	if (fCachedXPoints == nullptr || fCachedOrigin != localOrigin)
		{
		if (fPoints.size() > 0)
			{
			XPoint* theXPoints = fCachedXPoints;
			if (theXPoints == nullptr)
				theXPoints = new XPoint[fPoints.size()];
			for (size_t x = 0; x < fPoints.size(); ++x)
				theXPoints[x] = fPoints[x] + localOrigin;
			nonConstThis->fCachedXPoints = theXPoints;
			nonConstThis->fCachedOrigin = localOrigin;
			}
		}
	oXPoints = nonConstThis->fCachedXPoints;
	oCount = fPoints.size();
	}
#endif // ZCONFIG_SPI_Enabled(X11)

#if ZCONFIG_SPI_Enabled(BeOS)
void ZDCPoly::GetBPoints(const ZPoint& iOrigin, BPoint*& oBPoints, size_t& oCount) const
	{
	ZDCPoly* nonConstThis = const_cast<ZDCPoly*>(this);
	ZPoint localOrigin = fOrigin + iOrigin;
	if (fCachedBPoints == nullptr || fCachedOrigin != localOrigin)
		{
		if (fPoints.size() > 0)
			{
			BPoint* theBPoints = fCachedBPoints;
			if (theBPoints == nullptr)
				theBPoints = new BPoint[fPoints.size()];
			for (size_t x = 0; x < fPoints.size(); ++x)
				theBPoints[x] = fPoints[x]+localOrigin;
			nonConstThis->fCachedBPoints = theBPoints;
			nonConstThis->fCachedOrigin = localOrigin;
			}
		}
	oBPoints = nonConstThis->fCachedBPoints;
	oCount = fPoints.size();
	}
#endif // ZCONFIG_SPI_Enabled(BeOS)

void ZDCPoly::pEmptyCache()
	{
	delete[] fCachedPoints;
	fCachedPoints = nullptr;
	#if ZCONFIG_SPI_Enabled(QuickDraw)
		if (fCachedPolyHandle)
			::KillPoly(fCachedPolyHandle);
		fCachedPolyHandle = nullptr;
	#endif

	#if ZCONFIG_SPI_Enabled(GDI)
		delete[] fCachedPOINTs;
		fCachedPOINTs = nullptr;
	#endif

	#if ZCONFIG_SPI_Enabled(X11)
		delete[] fCachedXPoints;
		fCachedXPoints = nullptr;
	#endif

	#if ZCONFIG_SPI_Enabled(BeOS)
		delete[] fCachedBPoints;
		fCachedBPoints = nullptr;
	#endif
	}

// =================================================================================================

void ZDCPoly::Decompose(bool iEvenOdd, ZCoord iOffsetH, ZCoord iOffsetV,
	DecomposeProc iDecomposeProc, void* iRefcon) const
	{
	if (fPoints.size() > 2)
		{
		ZDCPoly::sDecompose(&fPoints[0], fPoints.size(),
		iEvenOdd, fOrigin.h + iOffsetH, fOrigin.v + iOffsetV, iDecomposeProc, iRefcon);
		}
	}

struct BresenhamInfo
	{
	ZCoord fMinorAxis; // minor axis
	ZCoord fD; // decision variable
	ZCoord fM; // slope and slope+1
	ZCoord fM1;
	ZCoord fIncr1; // error increments
	ZCoord fIncr2;

	void Init(ZCoord iDeltaMajor, ZCoord iMinor1, ZCoord iMinor2);
	void Step();
	};

void BresenhamInfo::Init(ZCoord iDeltaMajor, ZCoord iMinor1, ZCoord iMinor2)
	{
	ZAssertStop(1, iDeltaMajor != 0);

	fMinorAxis = iMinor1;
	ZCoord dx = iMinor2 - iMinor1;
	if (dx < 0)
		{
		fM = dx / iDeltaMajor;
		fM1 = fM - 1;
		fIncr1 = -2 * dx + 2 * iDeltaMajor * fM1;
		fIncr2 = -2 * dx + 2 * iDeltaMajor * fM;
		fD = 2 * fM * iDeltaMajor - 2 * dx - 2 * iDeltaMajor;
		}
	else
		{
		fM = dx / iDeltaMajor;
		fM1 = fM + 1;
		fIncr1 = 2 * dx - 2 * iDeltaMajor * fM1;
		fIncr2 = 2 * dx - 2 * iDeltaMajor * fM;
		fD = -2 * fM * iDeltaMajor + 2 * dx;
		}
	}

void BresenhamInfo::Step()
	{
	if (fM1 > 0)
		{
		if (fD > 0)
			{
			fMinorAxis += fM1;
			fD += fIncr1;
			}
		else
			{
			fMinorAxis += fM;
			fD += fIncr2;
			}
		}
	else
		{
		if (fD >= 0)
			{
			fMinorAxis += fM1;
			fD += fIncr1;
			}
		else
			{
			fMinorAxis += fM;
			fD += fIncr2;
			}
		}
	}

struct Edge
	{
	// Used to generate x coords when we walk this edge through its scanlines.
	BresenhamInfo fBresenhamInfo; 

	// Y coord at which we no longer exist.
	ZCoord fExitV;

	// Next edge starting on same scanline.
	Edge* fEdge_Next; 

	// For insertion sort.
	Edge* fEdge_Back;

	// For winding number rule.
	Edge* fEdge_NextForWinding;

	// Flag for winding number rule.
	bool fClockwise;
	};

struct ScanLine
	{
	// The scanline represented.
	ZCoord fVCoord;

	// First edge on this scanline.
	Edge* fEdge_First;

	// Next in the list.
	ScanLine* fScanLine_Next;
	};

static const size_t sSLLsPerBlock = 25;

struct ScanLineBlock
	{
	ScanLine fScanLines[sSLLsPerBlock];
	ScanLineBlock* fNextAllocatedBlock;
	};

struct EdgeTable
	{
	ZCoord fMaxV; // ymax for the polygon
	ZCoord fMinV; // ymin for the polygon
	ScanLine* fScanLine_First;
	};

static void sBuildTable(const ZPoint* iPoints, size_t iCount,
	ZCoord iOffsetH, ZCoord iOffsetV,
	EdgeTable& ioEdgeTable, Edge* iEdges,
	ScanLineBlock* iFirstScanLineBlock);

static void sUpdateActiveEdgeTable(Edge* iFirstActiveEdge, Edge* iEdgesToAdd);

static bool sInsertionSort(Edge* iEdge);

static void sInsertEdge(EdgeTable& ioEdgeTable, Edge* iEdgeToInsert,
	ZCoord iVCoord, ScanLineBlock*& ioScanLineBlock, size_t& ioIndexInCurrentScanLineBlock);

static void sLinkWindingEdgeList(Edge* iFirstActiveEdge);

void ZDCPoly::sDecompose(const ZPoint* iPoints, size_t iCount, bool iEvenOdd,
	ZCoord iOffsetH, ZCoord iOffsetV, DecomposeProc iDecomposeProc, void* iRefcon)
	{
	if (iCount < 3)
		return;

	// Special case a rectangle
	if (iCount == 4
		|| (iCount == 5
		&& (iPoints[0].h == iPoints[4].h)
		&& (iPoints[0].v == iPoints[4].v)))
		{
		if (((iPoints[0].v == iPoints[1].v) && (iPoints[1].h == iPoints[2].h)
			&& (iPoints[2].v == iPoints[3].v) && (iPoints[3].h == iPoints[0].h))
			|| ((iPoints[0].h == iPoints[1].h) && (iPoints[1].v == iPoints[2].v)
			&& (iPoints[2].h == iPoints[3].h) && (iPoints[3].v == iPoints[0].v)))
			{
			ZCoord temp[2];
			temp[0] = min(iPoints[0].h, iPoints[2].h) + iOffsetH;
			temp[1] = max(iPoints[0].h, iPoints[2].h) + iOffsetH;
			ZCoord top = min(iPoints[0].v, iPoints[2].v) + iOffsetV;
			ZCoord bottom = max(iPoints[0].v, iPoints[2].v) + iOffsetV;

			// Call once with the rectangle's information.
			if (iDecomposeProc(top, bottom, temp, 2, iRefcon))
				return;

			// And once with no data. This is needed for some client
			// code that builds data structures that track the deltas
			// rather than an absolute list of rectangles (ie QD regions).
			iDecomposeProc(bottom, bottom, nullptr, 0, iRefcon);
			return;
			}
		}

	EdgeTable theEdgeTable;
	Edge* theEdges = new Edge[iCount];
	ScanLineBlock* firstScanLineBlock = new ScanLineBlock;
	sBuildTable(iPoints, iCount, iOffsetH, iOffsetV,
		theEdgeTable, theEdges, firstScanLineBlock);

	ZCoord* transitionCoords = new ZCoord[16];
	size_t transitionCoordsAllocatedSize = 16;
	bool aborted = false;

	if (iEvenOdd)
		{
		// EvenOdd Rule
		Edge dummyEdge;
		dummyEdge.fEdge_Next = nullptr;
		dummyEdge.fEdge_Back = nullptr;
		dummyEdge.fBresenhamInfo.fMinorAxis = sCoord_Min;
		ScanLine* currentScanLine = theEdgeTable.fScanLine_First;
		for (ZCoord currentVCoord = theEdgeTable.fMinV;
			currentVCoord < theEdgeTable.fMaxV; ++currentVCoord)
			{
			// Have we reached the v coord corresponding to the next ScanLine object?
			if (currentScanLine != nullptr && currentVCoord == currentScanLine->fVCoord)
				{
				// If so, augment the list of active edges
				// with the edges attached to this ScanLine.
				sUpdateActiveEdgeTable(&dummyEdge, currentScanLine->fEdge_First);

				// And update our notion of which ScanLine object is the next to be watched for.
				currentScanLine = currentScanLine->fScanLine_Next;
				}

			Edge* previousEdge = &dummyEdge;
			Edge* currentEdge = dummyEdge.fEdge_Next;
			size_t transitionCoordsUsedSize = 0;
			// For each active edge
			while (currentEdge)
				{
				ZAssert(transitionCoordsUsedSize <= transitionCoordsAllocatedSize);
				if (transitionCoordsUsedSize == transitionCoordsAllocatedSize)
					{
					transitionCoordsAllocatedSize *= 2;
					ZCoord* newTransitionCoords = new ZCoord[transitionCoordsAllocatedSize];

					ZMemCopy(newTransitionCoords,
						transitionCoords, sizeof(ZCoord) * transitionCoordsUsedSize);

					delete[] transitionCoords;
					transitionCoords = newTransitionCoords;
					}

				transitionCoords[transitionCoordsUsedSize++]
					= currentEdge->fBresenhamInfo.fMinorAxis;

				if (currentEdge->fExitV == currentVCoord)
					{
					// If we've reached the V coord that this edge expires at,
					// then remove it from the list of active edges
					if (previousEdge)
						previousEdge->fEdge_Next = currentEdge->fEdge_Next;
					currentEdge = currentEdge->fEdge_Next;

					// Make sure that the back links are updated too.
					if (currentEdge)
						currentEdge->fEdge_Back = previousEdge;
					}
				else
					{
					// Otherwise just step the Bresenham algorithm along
					currentEdge->fBresenhamInfo.Step();

					// And move on to the next edge
					previousEdge = currentEdge;
					currentEdge = currentEdge->fEdge_Next;
					}
				}
			aborted = iDecomposeProc(currentVCoord, currentVCoord + 1,
				transitionCoords, transitionCoordsUsedSize, iRefcon);

			if (aborted)
				break;

			// Reorder the edges so they're still in left to right order after having been stepped.
			sInsertionSort(dummyEdge.fEdge_Next);
			}
		}
	else
		{
		// Winding Rule
		Edge dummyEdge;
		dummyEdge.fEdge_Next = nullptr;
		dummyEdge.fEdge_Back = nullptr;
		dummyEdge.fBresenhamInfo.fMinorAxis = sCoord_Min;
		ScanLine* currentScanLine = theEdgeTable.fScanLine_First;
		for (ZCoord currentVCoord = theEdgeTable.fMinV;
			currentVCoord < theEdgeTable.fMaxV; ++currentVCoord)
			{
			// Have we reached the v coord corresponding to the next ScanLine object?
			if (currentScanLine != nullptr && currentVCoord == currentScanLine->fVCoord)
				{
				// If so, augment the list of active edges
				// with the edges attached to this ScanLine
				sUpdateActiveEdgeTable(&dummyEdge, currentScanLine->fEdge_First);

				// And relink the winding edge list
				sLinkWindingEdgeList(&dummyEdge);

				// And update our notion of which ScanLine object is the next to be watched for.
				currentScanLine = currentScanLine->fScanLine_Next;
				}
			Edge* previousEdge = &dummyEdge;
			Edge* currentEdge = dummyEdge.fEdge_Next;
			Edge* currentWindingEdge = dummyEdge.fEdge_Next;
			size_t transitionCoordsUsedSize = 0;
			bool fixWindingEdges = false;
			// For each active edge
			while (currentEdge)
				{
				// Only output data for edges that are in the winding linked list
				if (currentEdge == currentWindingEdge)
					{
					ZAssert(transitionCoordsUsedSize <= transitionCoordsAllocatedSize);
					if (transitionCoordsUsedSize == transitionCoordsAllocatedSize)
						{
						transitionCoordsAllocatedSize *= 2;
						ZCoord* newTransitionCoords = new ZCoord[transitionCoordsAllocatedSize];

						ZMemCopy(newTransitionCoords,
							transitionCoords, sizeof(ZCoord) * transitionCoordsUsedSize);

						delete[] transitionCoords;
						transitionCoords = newTransitionCoords;
						}
					transitionCoords[transitionCoordsUsedSize++]
						= currentEdge->fBresenhamInfo.fMinorAxis;

					currentWindingEdge = currentWindingEdge->fEdge_NextForWinding;
					}
				if (currentEdge->fExitV == currentVCoord)
					{
					// If we've reached the V coord that this edge expires at,
					// then remove it from the list of active edges
					if (previousEdge)
						previousEdge->fEdge_Next = currentEdge->fEdge_Next;
					currentEdge = currentEdge->fEdge_Next;

					// Make sure that the back links are updated too.
					if (currentEdge)
						currentEdge->fEdge_Back = previousEdge;
					fixWindingEdges = true;
					}
				else
					{
					// Otherwise just step the Bresenham algorithm along
					currentEdge->fBresenhamInfo.Step();
					// And move on to the next edge
					previousEdge = currentEdge;
					currentEdge = currentEdge->fEdge_Next;
					}
				}
			aborted = iDecomposeProc(currentVCoord, currentVCoord + 1,
				transitionCoords, transitionCoordsUsedSize, iRefcon);

			if (aborted)
				break;

			// Reorder the edges so they're still in left to right order after having been stepped.
			if (sInsertionSort(dummyEdge.fEdge_Next) || fixWindingEdges)
				{
				// If sInsertionSort changed the order, or fixWindingEdges
				// was set by an edge going out of scope, then we
				// need to rebuild the list of active winding edges
				sLinkWindingEdgeList(&dummyEdge);
				}
			}
		}
	// ==================================================

	delete[] transitionCoords;
	// Make the final callback with no data.
	if (!aborted)
		iDecomposeProc(theEdgeTable.fMaxV, theEdgeTable.fMaxV, nullptr, 0, iRefcon);

	// Free the ScanLineBlocks
	while (firstScanLineBlock)
		{
		ScanLineBlock* next = firstScanLineBlock->fNextAllocatedBlock;
		delete firstScanLineBlock;
		firstScanLineBlock = next;
		}
	// And the edges
	delete[] theEdges;
	}

static void sBuildTable(const ZPoint* iPoints, size_t iCount,
	ZCoord iOffsetH, ZCoord iOffsetV,
	EdgeTable& ioEdgeTable, Edge* iEdges, ScanLineBlock* iFirstScanLineBlock)
	{
	// Initialize the Edge Table.
	ioEdgeTable.fScanLine_First = nullptr;
	ioEdgeTable.fMaxV = sCoord_Min;
	ioEdgeTable.fMinV = sCoord_Max;

	ScanLineBlock* currentScanLineBlock = iFirstScanLineBlock;
	// Ensure the ScanLineBlock's next pointer is nil
	currentScanLineBlock->fNextAllocatedBlock = nullptr;

	// Set up the index of which entry in the current ScanLineBlock is to be used
	size_t indexInCurrentScanLineBlock = 0;

	// Set up the pointer to the entry we're working on
	Edge* currentEdge = iEdges;

	// Polygons are closed figures. The point prior to the first point is the last point.
	ZPoint previousPoint = iPoints[iCount - 1];
	previousPoint.h += iOffsetH;
	previousPoint.v += iOffsetV;

	while (iCount--)
		{
		// Grab the current point, and update our reference to which point we'll work with next.
		ZPoint currentPoint = *iPoints++;
		currentPoint.h += iOffsetH;
		currentPoint.v += iOffsetV;

		// If current and previous have different v coords then they don't form
		// a horizontal edge and we'll handle them
		if (currentPoint.v != previousPoint.v)
			{
			ZPoint topPoint, bottomPoint;
			// Which point is above and which is below?
			if (previousPoint.v > currentPoint.v)
				{
				topPoint = currentPoint;
				bottomPoint = previousPoint;
				currentEdge->fClockwise = false;
				}
			else
				{
				topPoint = previousPoint;
				bottomPoint = currentPoint;
				currentEdge->fClockwise = true;
				}
			currentEdge->fBresenhamInfo.Init(bottomPoint.v - topPoint.v, topPoint.h, bottomPoint.h);
			currentEdge->fExitV = bottomPoint.v - 1;
			currentEdge->fEdge_Next = nullptr;
			currentEdge->fEdge_Back = nullptr;
			currentEdge->fEdge_NextForWinding = nullptr;

			// Hook this edge onto the appropriate scanline
			sInsertEdge(ioEdgeTable, currentEdge, topPoint.v,
				currentScanLineBlock, indexInCurrentScanLineBlock);

			// Update our notion of which edge is current
			++currentEdge;

			// Update the polygon's min and max v coordinates
			if (ioEdgeTable.fMaxV < previousPoint.v)
				ioEdgeTable.fMaxV = previousPoint.v;
			if (ioEdgeTable.fMinV > previousPoint.v)
				ioEdgeTable.fMinV = previousPoint.v;
			}
		previousPoint = currentPoint;
		}
	}

static void sUpdateActiveEdgeTable(Edge* iFirstActiveEdge, Edge* iEdgesToAdd)
	{
	// This is a merge sort, and thus assumes that iEdgesToAdd are sorted by fMinorAxis.
	Edge* previousActiveEdge = nullptr;
	Edge* currentActiveEdge = iFirstActiveEdge;

	Edge* currentEdgeToAdd = iEdgesToAdd;

	while (currentEdgeToAdd) 
		{
		while (currentActiveEdge
			&& (currentActiveEdge->fBresenhamInfo.fMinorAxis
			< currentEdgeToAdd->fBresenhamInfo.fMinorAxis)) 
			{
			previousActiveEdge = currentActiveEdge;
			currentActiveEdge = currentActiveEdge->fEdge_Next;
			}

		Edge* temp = currentEdgeToAdd->fEdge_Next;

		currentEdgeToAdd->fEdge_Next = currentActiveEdge;

		if (currentActiveEdge)
			currentActiveEdge->fEdge_Back = currentEdgeToAdd;

		currentEdgeToAdd->fEdge_Back = previousActiveEdge;

		if (previousActiveEdge)
			previousActiveEdge->fEdge_Next = currentEdgeToAdd;

		previousActiveEdge = currentEdgeToAdd;

		currentEdgeToAdd = temp;
		}
	}

static void sInsertEdge(EdgeTable& ioEdgeTable, Edge* iEdgeToInsert,
	ZCoord iVCoord, ScanLineBlock*& ioScanLineBlock, size_t& ioIndexInCurrentScanLineBlock)
	{
	// Find the ScanLine to which the edge should be attached
	ScanLine* previousScanLine = nullptr;
	ScanLine* currentScanLine = ioEdgeTable.fScanLine_First;
	while (currentScanLine && currentScanLine->fVCoord < iVCoord)
		{
		previousScanLine = currentScanLine;
		currentScanLine = currentScanLine->fScanLine_Next;
		}

	if (currentScanLine == nullptr || currentScanLine->fVCoord > iVCoord)
		{
		if (ioIndexInCurrentScanLineBlock >= sSLLsPerBlock)
			{
			ScanLineBlock* newBlock = new ScanLineBlock;
			newBlock->fNextAllocatedBlock = nullptr;
			ioScanLineBlock->fNextAllocatedBlock = newBlock;
			ioScanLineBlock = newBlock;
			ioIndexInCurrentScanLineBlock = 0;
			}
		currentScanLine = &ioScanLineBlock->fScanLines[ioIndexInCurrentScanLineBlock];
		++ioIndexInCurrentScanLineBlock;
		currentScanLine->fEdge_First = nullptr;
		currentScanLine->fVCoord = iVCoord;
	
		if (previousScanLine)
			{
			currentScanLine->fScanLine_Next = previousScanLine->fScanLine_Next;
			previousScanLine->fScanLine_Next = currentScanLine;
			}
		else
			{
			currentScanLine->fScanLine_Next = ioEdgeTable.fScanLine_First;
			ioEdgeTable.fScanLine_First = currentScanLine;
			}
		}

	Edge* previousEdge = nullptr;
	Edge* currentEdge = currentScanLine->fEdge_First;

	while (currentEdge
		&& currentEdge->fBresenhamInfo.fMinorAxis < iEdgeToInsert->fBresenhamInfo.fMinorAxis)
		{
		previousEdge = currentEdge;
		currentEdge = currentEdge->fEdge_Next;
		}
	iEdgeToInsert->fEdge_Next = currentEdge;

	if (previousEdge)
		previousEdge->fEdge_Next = iEdgeToInsert;
	else
		currentScanLine->fEdge_First = iEdgeToInsert;
	}

static bool sInsertionSort(Edge* iEdge)
	{
	bool changed = false;
	Edge* currentEdge = iEdge;

	while (currentEdge) 
		{
		Edge* chaseEdge = currentEdge;

		while (chaseEdge->fEdge_Back->fBresenhamInfo.fMinorAxis
			> currentEdge->fBresenhamInfo.fMinorAxis)
			{
			chaseEdge = chaseEdge->fEdge_Back;
			}
	
		Edge* insertEdge = currentEdge;
		currentEdge = currentEdge->fEdge_Next;
		if (chaseEdge != insertEdge) 
			{
			Edge* temp = chaseEdge->fEdge_Back;
			insertEdge->fEdge_Back->fEdge_Next = currentEdge;
			if (currentEdge)
				currentEdge->fEdge_Back = insertEdge->fEdge_Back;
			insertEdge->fEdge_Next = chaseEdge;
			chaseEdge->fEdge_Back->fEdge_Next = insertEdge;
			chaseEdge->fEdge_Back = insertEdge;
			insertEdge->fEdge_Back = temp;
			changed = true;
			}
		}
	return changed;
	}

static void sLinkWindingEdgeList(Edge* iFirstActiveEdge)
	{
	bool inside = true;
	int isInsideCount = 0;

	iFirstActiveEdge->fEdge_NextForWinding = nullptr;
	Edge* currentWindingEdge = iFirstActiveEdge;
	Edge* currentActiveEdge = iFirstActiveEdge->fEdge_Next;
	while (currentActiveEdge) 
		{
		if (currentActiveEdge->fClockwise)
			++isInsideCount;
		else
			--isInsideCount;
	
		if ((!inside && !isInsideCount) || (inside && isInsideCount)) 
			{
			currentWindingEdge->fEdge_NextForWinding = currentActiveEdge;
			currentWindingEdge = currentActiveEdge;
			inside = !inside;
			}
		currentActiveEdge = currentActiveEdge->fEdge_Next;
		}
	currentWindingEdge->fEdge_NextForWinding = nullptr;
	}

} // namespace ZooLib
