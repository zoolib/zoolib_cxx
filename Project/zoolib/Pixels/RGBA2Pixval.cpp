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

#include "zoolib/Pixels/RGBA2Pixval.h"

namespace ZooLib {
namespace Pixels {

// =================================================================================================
#pragma mark - RGBA2Pixval_Indexed

void RGBA2Pixval_Indexed::AsPixvals(const RGBA* iColors,
	size_t iCount, uint32* oPixvals) const
	{
	size_t localCount = iCount + 1;
	if (fPixvals)
		{
		while (--localCount)
			{
			uint8 index = fReverseLookup[
				int(16 * sBlue(*iColors)) + int(256 * sGreen(*iColors)) + int(4096 * sRed(*iColors))];
			*oPixvals++ = fPixvals[index];
			++iColors;
			}
		}
	else
		{
		while (--localCount)
			{
			*oPixvals++ = fReverseLookup[
				int(16 * sBlue(*iColors)) + int(256 * sGreen(*iColors)) + int(4096 * sRed(*iColors))];
			++iColors;
			}
		}
	}

// =================================================================================================
#pragma mark - RGBA2Pixval_Gray

void RGBA2Pixval_Gray::AsPixvals(const RGBA* iColors,
	size_t iCount, uint32* oPixvals) const
	{
	size_t localCount = iCount + 1;
	while (--localCount)
		{
		uint32 theGray = (sRed(*iColors) + sGreen(*iColors) + sBlue(*iColors)) / 3.0 * 65535.0;
		uint32 theValue = (theGray >> fShiftRightL) << fShiftLeftL;
		theValue |= ((uint32(sAlpha(*iColors) * 65535) >> fShiftRightA) << fShiftLeftA);
		*oPixvals++ = theValue;
		++iColors;
		}
	}

// =================================================================================================
#pragma mark - RGBA2Pixval_Color

void RGBA2Pixval_Color::AsPixvals(const RGBA* iColors,
	size_t iCount, uint32* oPixvals) const
	{
	size_t localCount = iCount + 1;
	while (--localCount)
		{
		uint32 theValue = 0;
		theValue |= ((uint32(sRed(*iColors) * 65535) >> fShiftRightR) << fShiftLeftR);
		theValue |= ((uint32(sGreen(*iColors) * 65535) >> fShiftRightG) << fShiftLeftG);
		theValue |= ((uint32(sBlue(*iColors) * 65535) >> fShiftRightB) << fShiftLeftB);
		theValue |= ((uint32(sAlpha(*iColors) * 65535) >> fShiftRightA) << fShiftLeftA);
		*oPixvals++ = theValue;
		++iColors;
		}
	}

} // namespace Pixels
} // namespace ZooLib
