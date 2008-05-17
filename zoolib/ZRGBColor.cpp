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

#include "ZRGBColor.h"
#include "ZCompat_algorithm.h"

using std::min;
using std::max;

const ZRGBColorPOD ZRGBColorPOD::sBlack =   { 0x0000, 0x0000, 0x0000, 0xFFFF };
const ZRGBColorPOD ZRGBColorPOD::sRed =     { 0xFFFF, 0x0000, 0x0000, 0xFFFF };
const ZRGBColorPOD ZRGBColorPOD::sGreen =   { 0x0000, 0xFFFF, 0x0000, 0xFFFF };
const ZRGBColorPOD ZRGBColorPOD::sBlue =    { 0x0000, 0x0000, 0xFFFF, 0xFFFF };
const ZRGBColorPOD ZRGBColorPOD::sYellow =  { 0xFFFF, 0xFFFF, 0x0000, 0xFFFF };
const ZRGBColorPOD ZRGBColorPOD::sCyan =    { 0x0000, 0xFFFF, 0xFFFF, 0xFFFF };
const ZRGBColorPOD ZRGBColorPOD::sMagenta = { 0xFFFF, 0x0000, 0xFFFF, 0xFFFF };
const ZRGBColorPOD ZRGBColorPOD::sWhite =   { 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF };

const ZRGBColorSmallPOD ZRGBColorSmallPOD::sBlack =   { 0x00, 0x00, 0x00, 0xFF };
const ZRGBColorSmallPOD ZRGBColorSmallPOD::sRed =     { 0xFF, 0x00, 0x00, 0xFF };
const ZRGBColorSmallPOD ZRGBColorSmallPOD::sGreen =   { 0x00, 0xFF, 0x00, 0xFF };
const ZRGBColorSmallPOD ZRGBColorSmallPOD::sBlue =    { 0x00, 0x00, 0xFF, 0xFF };
const ZRGBColorSmallPOD ZRGBColorSmallPOD::sYellow =  { 0xFF, 0xFF, 0x00, 0xFF };
const ZRGBColorSmallPOD ZRGBColorSmallPOD::sCyan =    { 0x00, 0xFF, 0xFF, 0xFF };
const ZRGBColorSmallPOD ZRGBColorSmallPOD::sMagenta = { 0xFF, 0x00, 0xFF, 0xFF };
const ZRGBColorSmallPOD ZRGBColorSmallPOD::sWhite =   { 0xFF, 0xFF, 0xFF, 0xFF };

// =================================================================================================
#pragma mark -
#pragma mark * ZRGBColorPOD

ZRGBColorPOD& ZRGBColorPOD::operator*=(double iMultiplier)
	{
	red = uint16(max(0.0, min(65535.0, red * iMultiplier)));
	green = uint16(max(0.0, min(65535.0, green * iMultiplier)));
	blue = uint16(max(0.0, min(65535.0, blue * iMultiplier)));
	alpha = uint16(max(0.0, min(65535.0, alpha * iMultiplier)));

	return *this;
	}

ZRGBColorPOD& ZRGBColorPOD::operator/=(double iDivisor)
	{
	red = uint16(max(0.0, min(65535.0, red / iDivisor)));
	green = uint16(max(0.0, min(65535.0, green / iDivisor)));
	blue = uint16(max(0.0, min(65535.0, blue / iDivisor)));
	alpha = uint16(max(0.0, min(65535.0, alpha / iDivisor)));

	return *this;
	}

ZRGBColorPOD& ZRGBColorPOD::operator*=(const ZRGBColorPOD& other)
	{
	red = uint16((red * other.red) / 65535);
	green = uint16((green * other.green) / 65535);
	blue = uint16((blue * other.blue) / 65535);
	alpha = uint16((alpha * other.alpha) / 65535);

	return *this;
	}

ZRGBColorPOD& ZRGBColorPOD::operator+=(const ZRGBColorPOD& other)
	{
	red = uint16(max(0, min(65535, red + other.red)));
	green = uint16(max(0, min(65535, green + other.green)));
	blue = uint16(max(0, min(65535, blue + other.blue)));
	alpha = uint16(max(0, min(65535, alpha + other.alpha)));

	return *this;
	}

ZRGBColorPOD& ZRGBColorPOD::operator-=(const ZRGBColorPOD& other)
	{
	red = uint16(max(0, min(65535, red - other.red)));
	green = uint16(max(0, min(65535, green - other.green)));
	blue = uint16(max(0, min(65535, blue - other.blue)));
	alpha = uint16(max(0, min(65535, alpha - other.alpha)));

	return *this;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZRGBColor

#if ZCONFIG_SPI_Enabled(GDI)
ZRGBColor ZRGBColor::sFromCOLORREF(COLORREF iColorRef)
	{
	ZRGBColor theColor;
	theColor.red = (iColorRef & 0x000000FF) * 0x101;
	theColor.green = ((iColorRef & 0x0000FF00) >> 8) * 0x101;
	theColor.blue = ((iColorRef & 0x00FF0000) >> 16) * 0x101;
	theColor.alpha = 0xFFFFU;
	return theColor;
	}
#endif // ZCONFIG_SPI_Enabled(GDI)

// =================================================================================================
#pragma mark -
#pragma mark * ZRGBColorSmallPOD

ZRGBColorSmallPOD& ZRGBColorSmallPOD::operator*=(double iMultiplier)
	{
	red = uint8(max(0.0, min(255.0, red * iMultiplier)));
	green = uint8(max(0.0, min(255.0, green * iMultiplier)));
	blue = uint8(max(0.0, min(255.0, blue * iMultiplier)));
	alpha = uint8(max(0.0, min(255.0, alpha * iMultiplier)));

	return *this;
	}

ZRGBColorSmallPOD& ZRGBColorSmallPOD::operator/=(double iDivisor)
	{
	red = uint8(max(0.0, min(255.0, red / iDivisor)));
	green = uint8(max(0.0, min(255.0, green / iDivisor)));
	blue = uint8(max(0.0, min(255.0, blue / iDivisor)));
	alpha = uint8(max(0.0, min(255.0, alpha / iDivisor)));

	return *this;
	}

ZRGBColorSmallPOD& ZRGBColorSmallPOD::operator*=(const ZRGBColorSmallPOD& other)
	{
	red = uint8((red * other.red) / 255);
	green = uint8((green * other.green) / 255);
	blue = uint8((blue * other.blue) / 255);
	alpha = uint8((alpha * other.alpha) / 255);

	return *this;
	}

ZRGBColorSmallPOD& ZRGBColorSmallPOD::operator+=(const ZRGBColorSmallPOD& other)
	{
	red = uint8(max(0, min(255, red + other.red)));
	green = uint8(max(0, min(255, green + other.green)));
	blue = uint8(max(0, min(255, blue + other.blue)));
	alpha = uint8(max(0, min(255, alpha + other.alpha)));

	return *this;
	}

ZRGBColorSmallPOD& ZRGBColorSmallPOD::operator-=(const ZRGBColorSmallPOD& other)
	{
	red = uint8(max(0, min(255, red - other.red)));
	green = uint8(max(0, min(255, green - other.green)));
	blue = uint8(max(0, min(255, blue - other.blue)));
	alpha = uint8(max(0, min(255, alpha - other.alpha)));

	return *this;
	}


// =================================================================================================
#pragma mark -
#pragma mark * ZRGBColorSmall

#if ZCONFIG_SPI_Enabled(GDI)
ZRGBColorSmall ZRGBColorSmall::sFromCOLORREF(COLORREF iColorRef)
	{
	ZRGBColorSmall theColor;
	theColor.red = (iColorRef & 0x000000FF);
	theColor.green = iColorRef & 0x0000FF00 >> 8;
	theColor.blue = (iColorRef & 0x00FF0000) >> 16;
	theColor.alpha = 0xFFU;
	return theColor;
	}
#endif // ZCONFIG_SPI_Enabled(GDI)


