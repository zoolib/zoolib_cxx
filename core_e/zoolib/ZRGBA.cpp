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

#include "zoolib/ZRGBA.h"
#include "zoolib/ZCompat_algorithm.h" // For min/max

namespace ZooLib {

using std::min;
using std::max;

const ZRGBA_POD ZRGBA_POD::sBlack =   { 0x0000, 0x0000, 0x0000, 0xFFFF };
const ZRGBA_POD ZRGBA_POD::sRed =     { 0xFFFF, 0x0000, 0x0000, 0xFFFF };
const ZRGBA_POD ZRGBA_POD::sGreen =   { 0x0000, 0xFFFF, 0x0000, 0xFFFF };
const ZRGBA_POD ZRGBA_POD::sBlue =    { 0x0000, 0x0000, 0xFFFF, 0xFFFF };
const ZRGBA_POD ZRGBA_POD::sYellow =  { 0xFFFF, 0xFFFF, 0x0000, 0xFFFF };
const ZRGBA_POD ZRGBA_POD::sCyan =    { 0x0000, 0xFFFF, 0xFFFF, 0xFFFF };
const ZRGBA_POD ZRGBA_POD::sMagenta = { 0xFFFF, 0x0000, 0xFFFF, 0xFFFF };
const ZRGBA_POD ZRGBA_POD::sWhite =   { 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF };

// =================================================================================================
// MARK: - ZRGBA_POD

#if ZCONFIG_SPI_Enabled(GDI)
ZRGBA_POD ZRGBA_POD::sFromCOLORREF(COLORREF iColorRef)
	{
	ZRGBA_POD theColor;
	theColor.red = (iColorRef & 0x000000FF) * 0x101;
	theColor.green = ((iColorRef & 0x0000FF00) >> 8) * 0x101;
	theColor.blue = ((iColorRef & 0x00FF0000) >> 16) * 0x101;
	theColor.alpha = 0xFFFFU;
	return theColor;
	}
#endif // ZCONFIG_SPI_Enabled(GDI)

ZRGBA_POD& ZRGBA_POD::operator*=(double iMultiplier)
	{
	red = uint16(max(0.0, min(65535.0, red * iMultiplier)));
	green = uint16(max(0.0, min(65535.0, green * iMultiplier)));
	blue = uint16(max(0.0, min(65535.0, blue * iMultiplier)));
	alpha = uint16(max(0.0, min(65535.0, alpha * iMultiplier)));

	return *this;
	}

ZRGBA_POD& ZRGBA_POD::operator/=(double iDivisor)
	{
	red = uint16(max(0.0, min(65535.0, red / iDivisor)));
	green = uint16(max(0.0, min(65535.0, green / iDivisor)));
	blue = uint16(max(0.0, min(65535.0, blue / iDivisor)));
	alpha = uint16(max(0.0, min(65535.0, alpha / iDivisor)));

	return *this;
	}

ZRGBA_POD& ZRGBA_POD::operator*=(const ZRGBA_POD& other)
	{
	red = uint16((red * other.red) / 65535);
	green = uint16((green * other.green) / 65535);
	blue = uint16((blue * other.blue) / 65535);
	alpha = uint16((alpha * other.alpha) / 65535);

	return *this;
	}

ZRGBA_POD& ZRGBA_POD::operator+=(const ZRGBA_POD& other)
	{
	red = uint16(max(0, min(65535, red + other.red)));
	green = uint16(max(0, min(65535, green + other.green)));
	blue = uint16(max(0, min(65535, blue + other.blue)));
	alpha = uint16(max(0, min(65535, alpha + other.alpha)));

	return *this;
	}

ZRGBA_POD& ZRGBA_POD::operator-=(const ZRGBA_POD& other)
	{
	red = uint16(max(0, min(65535, red - other.red)));
	green = uint16(max(0, min(65535, green - other.green)));
	blue = uint16(max(0, min(65535, blue - other.blue)));
	alpha = uint16(max(0, min(65535, alpha - other.alpha)));

	return *this;
	}

} // namespace ZooLib
