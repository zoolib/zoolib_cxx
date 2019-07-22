// Copyright (c) 2019 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/Pixels/Pixval2RGBA.h"

#include "zoolib/Compat_algorithm.h" // for lower_bound

namespace ZooLib {
namespace Pixels {

using std::lower_bound;

// =================================================================================================
#pragma mark - Pixval2RGBA_Indexed

void Pixval2RGBA_Indexed::AsRGBAs(const uint32* iPixvals,
	size_t iCount, RGBA* oColors) const
	{
	size_t localCount = iCount + 1;
	if (fPixvals)
		{
		uint32* pixValsEnd = fPixvals + fCount;
		while (--localCount)
			{
			size_t offset = lower_bound(fPixvals, pixValsEnd, *iPixvals++) - fPixvals;
			*oColors++ = fColors[offset];
			}
		}
	else
		{
		while (--localCount)
			*oColors++ = fColors[*iPixvals++];
		}
	}

RGBA Pixval2RGBA_Indexed::pAsRGBA(uint32 iPixval) const
	{
	uint32* iter = lower_bound(fPixvals, fPixvals + fCount, iPixval);
	return fColors[iter - fPixvals];
	}

Comp Pixval2RGBA_Indexed::pAsAlpha(uint32 iPixval) const
	{
	uint32* iter = lower_bound(fPixvals, fPixvals + fCount, iPixval);
	return sAlpha(fColors[iter - fPixvals]);
	}

// =================================================================================================
#pragma mark - Pixval2RGBA_Gray

void Pixval2RGBA_Gray::AsRGBAs(const uint32* iPixvals,
	size_t iCount, RGBA* oColors) const
	{
	size_t localCount = iCount + 1;
	while (--localCount)
		{
		uint32 pixVal = *iPixvals++;
		*oColors++ = sRGBA(sConvert(pixVal, fMaskL, fShiftL, fMultiplierL, fAddL),
			sConvert(pixVal, fMaskA, fShiftA, fMultiplierA, fAddA));
		}
	}

// =================================================================================================
#pragma mark - Pixval2RGBA_Color

void Pixval2RGBA_Color::AsRGBAs(const uint32* iPixvals,
	size_t iCount, RGBA* oColors) const
	{
	size_t localCount = iCount + 1;
	while (--localCount)
		{
		uint32 pixVal = *iPixvals++;
		*oColors++ = sRGBA(
			sConvert(pixVal, fMaskR, fShiftR, fMultiplierR, fAddR),
			sConvert(pixVal, fMaskG, fShiftG, fMultiplierG, fAddG),
			sConvert(pixVal, fMaskB, fShiftB, fMultiplierB, fAddB),
			sConvert(pixVal, fMaskA, fShiftA, fMultiplierA, fAddA));
		}
	}

} // namespace Pixels
} // namespace ZooLib
