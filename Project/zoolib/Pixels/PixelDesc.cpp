/* -------------------------------------------------------------------------------------------------
Copyright (c) 2019 Andrew Green
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

#include "zoolib/Pixels/Formats.h"

#include <vector>

namespace ZooLib {
namespace Pixels {

using std::vector;

const int kDebug_PixmapNS = 1;

// =================================================================================================
#pragma mark - anonymous

namespace { // anonymous

static void spMaskToShiftsAndMultiplier(uint32 iMask,
	int& oShiftLeft, int& oShiftRight, uint32& oMultiplier, uint32& oAdd)
	{
	oShiftLeft = 0;
	if (iMask == 0)
		{
		// If the mask is empty we use a right shift of 16. When a uint32 with bits set in
		// only its low 16 bits is shifted by 16, the result will always be zero. It is
		// important that the right shift be large enough to do this (ie at least 16), but
		// that it be smaller than the size of a uint32 (ie 32), because the result of a
		// shift by y positions of a value of size x bits is undefined when y >= x (it
		// actually does what we want on PPC, but not on x86).
		oShiftRight = 16;
		oMultiplier = 0;
		oAdd = 0xFFFFFFFFU;
		}
	else
		{
		while (0 == (iMask & 1))
			{
			++oShiftLeft;
			iMask >>= 1;
			}
		uint32 theDepth = 0;
		while (iMask & 1)
			{
			++theDepth;
			iMask >>= 1;
			}
		ZAssertStop(kDebug_PixmapNS, iMask == 0);
		ZAssertStop(kDebug_PixmapNS, theDepth <= 16);
		if (theDepth)
			{
			// theDepth must be non-zero, but Clang's analysis disagrees.
			oMultiplier = 0xFFFFFFFFU / ((1U << theDepth) - 1);
			oShiftRight = 16 - theDepth;
			oAdd = 0;
			}
		else
			{
			ZUnimplemented();
			}
		}
	}

static uint8* spBuildReverseLookup(const RGBA* iColors, size_t iCount)
	{
	ZAssertStop(kDebug_PixmapNS, iCount <= 256);
	uint8* lookupTable = new uint8[4096];
	for (int red = 0; red < 16; ++red)
		{
		for (int green = 0; green < 16; ++green)
			{
			for (int blue = 0; blue < 16; ++blue)
				{
				float bestDistance = 1e9;
				uint8 bestIndex = 0;
				for (size_t xx = 0; xx < iCount; ++xx)
					{
					const float redDelta = red/16.0 - sRed(iColors[xx]);
					const float greenDelta = green/16.0 - sGreen(iColors[xx]);
					const float blueDelta = blue/16.0 - sBlue(iColors[xx]);

					float currentDistance = (redDelta * redDelta)
						+ (greenDelta * greenDelta)
						+ (blueDelta * blueDelta);

					if (bestDistance > currentDistance)
						{
						bestDistance = currentDistance;
						bestIndex = xx;
						}
					}
				lookupTable[blue + 16 * (green + 16 * red)] = bestIndex;
				}
			}
		}
	return lookupTable;
	}

} // anonymous namespace

// =================================================================================================
#pragma mark - PixelDesc

PixelDesc::PixelDesc()
	{}

PixelDesc::PixelDesc(const PixelDesc& iOther)
:	fRep(iOther.fRep)
	{}

PixelDesc::~PixelDesc()
	{}

PixelDesc& PixelDesc::operator=(const PixelDesc& iOther)
	{
	fRep = iOther.fRep;
	return *this;
	}

PixelDesc::PixelDesc(const ZRef<PixelDescRep>& iPixelDescRep)
:	fRep(iPixelDescRep)
	{}

PixelDesc& PixelDesc::operator=(const ZRef<PixelDescRep>& iPixelDescRep)
	{
	fRep = iPixelDescRep;
	return *this;
	}

ZRef<PixelDescRep> PixelDesc::GetRep() const
	{ return fRep; }

bool PixelDesc::HasAlpha() const
	{ return fRep->HasAlpha(); }

PixelDesc PixelDesc::WithoutAlpha() const
	{ return fRep->WithoutAlpha(); }

RGBA PixelDesc::AsRGBA(Pixval iPixval) const
	{ return fRep->Imp_AsRGBA(iPixval); }

void PixelDesc::AsRGBAs(const Pixval* iPixvals, size_t iCount, RGBA* oColors) const
	{ return fRep->Imp_AsRGBAs(iPixvals, iCount, oColors); }

Comp PixelDesc::AsAlpha(Pixval iPixval) const
	{ return fRep->Imp_AsAlpha(iPixval); }

Pixval PixelDesc::AsPixval(const RGBA& iRGBA) const
	{ return fRep->Imp_AsPixval(iRGBA); }

void PixelDesc::AsPixvals(const RGBA* iColors, size_t iCount, Pixval* oPixvals) const
	{ return fRep->Imp_AsPixvals(iColors, iCount, oPixvals); }

// =================================================================================================
#pragma mark - PixelDescRep_Indexed

PixelDescRep_Indexed::PixelDescRep_Indexed(const RGBA* iColors, size_t iCount)
:	fCheckedAlpha(false)
	{
	ZAssertStop(kDebug_PixmapNS, iCount <= 256);

	Pixval2RGBA_Indexed::fPixvals = nullptr;
	RGBA2Pixval_Indexed::fPixvals = nullptr;
	fReverseLookup = nullptr;

	fCount = iCount;
	fColors = new RGBA[iCount];
	sMemCopy(fColors, iColors, iCount * sizeof(RGBA));
	}

PixelDescRep_Indexed::PixelDescRep_Indexed(const RGBA* iColors,
	size_t iCount, bool iStripAlpha)
:	fCheckedAlpha(true),
	fHasAlpha(false)
	{
	ZAssertStop(kDebug_PixmapNS, iCount <= 256);

	ZAssertStopf(1, iStripAlpha, "This constructor is only used when stripping alpha");

	Pixval2RGBA_Indexed::fPixvals = nullptr;
	RGBA2Pixval_Indexed::fPixvals = nullptr;
	fReverseLookup = nullptr;

	fCount = iCount;
	fColors = new RGBA[iCount];
	sMemCopy(fColors, iColors, iCount * sizeof(RGBA));

	// Set the alpha value to be completely opaque.
	for (size_t x = 0; x < iCount; ++x)
		sAlpha(fColors[x]) = 1.0;
	}

PixelDescRep_Indexed::PixelDescRep_Indexed(const RGBA* iColors,
	uint32* iPixvals, size_t iCount)
:	fCheckedAlpha(false)
	{
	ZAssertStop(kDebug_PixmapNS, iCount <= 256);

	Pixval2RGBA_Indexed::fPixvals = nullptr;
	RGBA2Pixval_Indexed::fPixvals = nullptr;
	fReverseLookup = nullptr;

	vector<RGBA> vectorColors;
	vector<uint32> vectorPixvals;
	for (size_t x = 0; x < iCount; ++x)
		{
		vector<uint32>::iterator insertIter =
			lower_bound(vectorPixvals.begin(), vectorPixvals.end(), iPixvals[x]);

		insertIter = vectorPixvals.insert(insertIter, iPixvals[x]);
		vectorColors.insert(
			vectorColors.begin() + (insertIter - vectorPixvals.begin()), iColors[x]);
		}

	fCount = iCount;
	fColors = new RGBA[iCount];

	try
		{
		Pixval2RGBA_Indexed::fPixvals = new uint32[iCount];
		RGBA2Pixval_Indexed::fPixvals = Pixval2RGBA_Indexed::fPixvals;
		}
	catch (...)
		{
		delete fColors;
		throw;
		}

	sMemCopy(fColors, &vectorColors[0], iCount * sizeof(RGBA));
	sMemCopy(Pixval2RGBA_Indexed::fPixvals, &vectorPixvals[0], iCount * sizeof(uint32));
	}

PixelDescRep_Indexed::PixelDescRep_Indexed(const RGBA* iColors,
	uint32* iPixvals, size_t iCount, bool iStripAlpha)
:	fCheckedAlpha(true),
	fHasAlpha(false)
	{
	ZAssertStop(kDebug_PixmapNS, iCount <= 256);

	ZAssertStopf(1, iStripAlpha, ("This constructor is only used when stripping alpha"));

	Pixval2RGBA_Indexed::fPixvals = nullptr;
	RGBA2Pixval_Indexed::fPixvals = nullptr;
	fReverseLookup = nullptr;

	vector<RGBA> vectorColors;
	vector<uint32> vectorPixvals;
	for (size_t x = 0; x < iCount; ++x)
		{
		vector<uint32>::iterator insertIter =
			lower_bound(vectorPixvals.begin(), vectorPixvals.end(), iPixvals[x]);

		insertIter = vectorPixvals.insert(insertIter, iPixvals[x]);
		vectorColors.insert(
			vectorColors.begin() + (insertIter - vectorPixvals.begin()), iColors[x]);
		}

	fCount = iCount;
	fColors = new RGBA[iCount];

	try
		{
		Pixval2RGBA_Indexed::fPixvals = new uint32[iCount];
		RGBA2Pixval_Indexed::fPixvals = Pixval2RGBA_Indexed::fPixvals;
		}
	catch (...)
		{
		delete fColors;
		throw;
		}

	sMemCopy(fColors, &vectorColors[0], iCount * sizeof(RGBA));
	sMemCopy(Pixval2RGBA_Indexed::fPixvals, &vectorPixvals[0], iCount * sizeof(uint32));

	// Set the alpha value to be completely opaque.
	for (size_t x = 0; x < iCount; ++x)
		sAlpha(fColors[x]) = 1.0;
	}

//PixelDescRep_Indexed::PixelDescRep_Indexed(
//	const ZRGBAMap* iColorMap, size_t iCount)
//:	fCheckedAlpha(false)
//	{
//	ZAssertStop(kDebug_PixmapNS, iCount <= 256);
//
//	Pixval2RGBA_Indexed::fPixvals = nullptr;
//	RGBA2Pixval_Indexed::fPixvals = nullptr;
//	fReverseLookup = nullptr;
//
//	fCount = 256;
//	fColors = new RGBA[256];
//	for (size_t xx = 0; xx < 256; ++xx)
//		fColors[xx] = RGBA(0, 1);
//
//	for (size_t x = 0; x < iCount; ++x)
//		fColors[iColorMap[x].fPixval] = iColorMap[x].fColor;
//	}

PixelDescRep_Indexed::~PixelDescRep_Indexed()
	{
	delete[] fColors;
	delete[] Pixval2RGBA_Indexed::fPixvals;
	delete[] fReverseLookup;
	}

bool PixelDescRep_Indexed::HasAlpha()
	{
	if (!fCheckedAlpha)
		{
		fHasAlpha = false;
		RGBA* current = fColors;
		size_t count = fCount + 1;
		while (--count)
			{
			if (sAlpha(*current) < 1.0)
				{
				fHasAlpha = true;
				break;
				}
			++current;
			}
		fCheckedAlpha = true;
		}
	return fHasAlpha;
	}

ZRef<PixelDescRep> PixelDescRep_Indexed::WithoutAlpha()
	{
	if (fCheckedAlpha && !fHasAlpha)
		return this;

	if (Pixval2RGBA_Indexed::fPixvals)
		return new PixelDescRep_Indexed(fColors, Pixval2RGBA_Indexed::fPixvals, fCount, true);
	else
		return new PixelDescRep_Indexed(fColors, fCount);
	}

void PixelDescRep_Indexed::BuildReverseLookupIfNeeded() const
	{
	if (!fReverseLookup)
		fReverseLookup = spBuildReverseLookup(fColors, fCount);
	}

void PixelDescRep_Indexed::GetColors(
	const RGBA*& oColors, size_t& oCount) const
	{
	oColors = fColors;
	oCount = fCount;
	}

bool PixelDescRep_Indexed::Matches(const PixelDescRep_Indexed* iOther)
	{
	return this == iOther;
	}

// =================================================================================================
#pragma mark - PixelDescRep_Gray

PixelDescRep_Gray::PixelDescRep_Gray(uint32 iMaskL, uint32 iMaskA)
	{
	spMaskToShiftsAndMultiplier(iMaskL, fShiftLeftL, fShiftRightL, fMultiplierL, fAddL);
	fShiftL = fShiftLeftL;
	fMaskL = iMaskL;

	spMaskToShiftsAndMultiplier(iMaskA, fShiftLeftA, fShiftRightA, fMultiplierA, fAddA);
	fShiftA = fShiftLeftA;
	fMaskA = iMaskA;
	}

bool PixelDescRep_Gray::HasAlpha()
	{ return fMaskA; }

ZRef<PixelDescRep> PixelDescRep_Gray::WithoutAlpha()
	{
	if (!fMaskA)
		return this;
	return new PixelDescRep_Gray(fMaskL, 0);
	}

void PixelDescRep_Gray::GetMasks(uint32& oMaskL, uint32& oMaskA) const
	{
	oMaskL = fMaskL;
	oMaskA = fMaskA;
	}

bool PixelDescRep_Gray::Matches(const PixelDescRep_Gray* iOther)
	{
	if (fMaskL && iOther->fMaskL && fMaskL != iOther->fMaskL)
		return false;
	if (fMaskA && iOther->fMaskA && fMaskA != iOther->fMaskA)
		return false;
	return true;
	}

// =================================================================================================
#pragma mark - PixelDescRep_Color

PixelDescRep_Color::PixelDescRep_Color(
	uint32 iMaskR, uint32 iMaskG, uint32 iMaskB, uint32 iMaskA)
	{
	// Ensure that the masks are all non-overlapping
	ZAssertStop(kDebug_PixmapNS, 0 == ((iMaskR & iMaskG) | (iMaskR & iMaskB) | (iMaskR & iMaskA)
		| (iMaskG & iMaskB) | (iMaskG & iMaskA) | (iMaskB & iMaskA)));

	spMaskToShiftsAndMultiplier(iMaskR, fShiftLeftR, fShiftRightR, fMultiplierR, fAddR);
	fShiftR = fShiftLeftR;
	fMaskR = iMaskR;

	spMaskToShiftsAndMultiplier(iMaskG, fShiftLeftG, fShiftRightG, fMultiplierG, fAddG);
	fShiftG = fShiftLeftG;
	fMaskG = iMaskG;

	spMaskToShiftsAndMultiplier(iMaskB, fShiftLeftB, fShiftRightB, fMultiplierB, fAddB);
	fShiftB = fShiftLeftB;
	fMaskB = iMaskB;

	spMaskToShiftsAndMultiplier(iMaskA, fShiftLeftA, fShiftRightA, fMultiplierA, fAddA);
	fShiftA = fShiftLeftA;
	fMaskA = iMaskA;
	}

bool PixelDescRep_Color::HasAlpha()
	{ return fMaskA; }

ZRef<PixelDescRep> PixelDescRep_Color::WithoutAlpha()
	{
	if (!fMaskA)
		return this;
	return new PixelDescRep_Color(fMaskR, fMaskG, fMaskB, 0);
	}

void PixelDescRep_Color::GetMasks(
	uint32& oMaskR, uint32& oMaskG, uint32& oMaskB, uint32& oMaskA) const
	{
	oMaskR = fMaskR;
	oMaskG = fMaskG;
	oMaskB = fMaskB;
	oMaskA = fMaskA;
	}

bool PixelDescRep_Color::Matches(const PixelDescRep_Color* iOther)
	{
	if (fMaskR && iOther->fMaskR && fMaskR != iOther->fMaskR)
		return false;
	if (fMaskG && iOther->fMaskG && fMaskG != iOther->fMaskG)
		return false;
	if (fMaskB && iOther->fMaskB && fMaskB != iOther->fMaskB)
		return false;
	if (fMaskA && iOther->fMaskA && fMaskA != iOther->fMaskA)
		return false;
	return true;
	}

// =================================================================================================
#pragma mark - PixelDesc

PixelDesc sPixelDesc(const RGBA* iColors, size_t iCount)
	{ return PixelDesc(new PixelDescRep_Indexed(iColors, iCount)); }

//PixelDesc sPixelDesc(const RGBAMap* iColorMap, size_t iCount)

PixelDesc sPixelDesc(uint32 iMaskGray, uint32 iMaskAlpha)
	{ return PixelDesc(new PixelDescRep_Gray(iMaskGray, iMaskAlpha)); }

PixelDesc sPixelDesc(uint32 iMaskRed, uint32 iMaskGreen, uint32 iMaskBlue, uint32 iMaskAlpha)
	{ return PixelDesc(new PixelDescRep_Color(iMaskRed, iMaskGreen, iMaskBlue, iMaskAlpha)); }

} // namespace Pixels
} // namespace ZooLib
