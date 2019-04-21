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
