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

#ifndef __ZRGBA_h__
#define __ZRGBA_h__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_SPI.h"

#include "zoolib/ZTypes.h"

#if ZCONFIG_SPI_Enabled(QuickDraw)
	#include ZMACINCLUDE3(ApplicationServices,QD,QuickDraw.h)
#endif

#if ZCONFIG_SPI_Enabled(GDI)
	typedef unsigned long COLORREF;
#endif

#if ZCONFIG_SPI_Enabled(X11)
	#include "zoolib/ZCompat_Xlib.h"
#endif

#if ZCONFIG_SPI_Enabled(BeOS)
	#include <GraphicsDefs.h>
#endif

namespace ZooLib {

// =================================================================================================
// MARK: - ZRGBA_POD

class ColorComp
	{
public:
	operator float() const
		{ return fVal; }

	operator uint8() const
		{ return uint8(0xFFu * fVal); }

	operator uint16() const
		{ return uint16(0xFFFFu * fVal); }

	operator uint32() const
		{ return uint32(0xFFFFFFFFu * fVal); }

	ColorComp& operator=(float iVal)
		{
		fVal = iVal;
		return *this;
		}

	ColorComp& operator=(uint8 iVal)
		{
		fVal = iVal / 255.0;
		return *this;
		}

	ColorComp& operator=(uint16 iVal)
		{
		fVal = iVal / 65535.0;
		return *this;
		}

	ColorComp& operator=(uint32 iVal)
		{
		fVal = iVal / 4294967295.0;
		return *this;
		}

private:
	float fVal;
	};

class ZRGBA_POD
	{
public:
	static const ZRGBA_POD sBlack;
	static const ZRGBA_POD sRed;
	static const ZRGBA_POD sGreen;
	static const ZRGBA_POD sBlue;
	static const ZRGBA_POD sYellow;
	static const ZRGBA_POD sCyan;
	static const ZRGBA_POD sMagenta;
	static const ZRGBA_POD sWhite;

	uint16 red;
	uint16 green;
	uint16 blue;
	uint16 alpha;

	#if ZCONFIG_SPI_Enabled(QuickDraw)
	operator RGBColor() const
		{
		RGBColor theRGB;
		theRGB.red = red;
		theRGB.green = green;
		theRGB.blue = blue;
		return theRGB;
		}
	#endif

	#if ZCONFIG_SPI_Enabled(X11)
	operator XColor() const
		{
		XColor theXColor;
		theXColor.red = red;
		theXColor.blue = blue;
		theXColor.green = green;
		theXColor.pixel = 0;
		return theXColor;
		}
	#endif

	#if ZCONFIG_SPI_Enabled(BeOS)
	operator rgb_color() const
		{
		rgb_color theRGB;
		theRGB.red = red >> 8;
		theRGB.green = green >> 8;
		theRGB.blue = blue >> 8;
		theRGB.alpha = alpha >> 8;
		return theRGB;
		}
	#endif

	#if ZCONFIG_SPI_Enabled(GDI)
	static ZRGBA_POD sFromCOLORREF(COLORREF iColorRef);
	#endif

	float floatRed() const
		{ return red / 65535.0; }

	float floatGreen() const
		{ return green / 65535.0; }

	float floatBlue() const
		{ return blue / 65535.0; }

	float floatAlpha() const
		{ return alpha / 65535.0; }

	uint16 Luminance() const
		{ return uint16((uint32(red) + uint32(green) + uint32(blue)) / 3); }

	double NTSCLuminance() const
		{ return ((0.3 * red) + (0.59 * green) + (0.11 * blue)) / 65535.0; }

	bool operator==(const ZRGBA_POD& other) const
		{ return red == other.red && green == other.green && blue == other.blue; }

	bool operator!=(const ZRGBA_POD& other) const
		{ return ! (*this == other); }

	ZRGBA_POD& operator*=(double iMultiplier);
	ZRGBA_POD operator*(double iMultiplier) const
		{
		ZRGBA_POD newColor(*this);
		newColor *= iMultiplier;
		return newColor;
		}

	ZRGBA_POD& operator/=(double iDivisor);
	ZRGBA_POD operator/(double iDivisor) const
		{
		ZRGBA_POD newColor(*this);
		newColor /= iDivisor;
		return newColor;
		}

	ZRGBA_POD& operator*=(const ZRGBA_POD& other);
	ZRGBA_POD operator*(const ZRGBA_POD& other) const
		{
		ZRGBA_POD newColor(*this);
		newColor *= other;
		return newColor;
		}

	ZRGBA_POD& operator+=(const ZRGBA_POD& other);
	ZRGBA_POD operator+(const ZRGBA_POD& other) const
		{
		ZRGBA_POD newColor(*this);
		newColor += other;
		return newColor;
		}

	ZRGBA_POD& operator-=(const ZRGBA_POD& other);
	ZRGBA_POD operator-(const ZRGBA_POD& other) const
		{
		ZRGBA_POD newColor(*this);
		newColor -= other;
		return newColor;
		}

	bool operator<(const ZRGBA_POD& other) const
		{
		return (red < other.red)
			|| (red == other.red && green < other.green)
			|| (red == other.red && green == other.green && blue < other.blue);
		}
	};

// =================================================================================================
// MARK: - ZRGBA

class ZRGBA : public ZRGBA_POD
	{
public:
	ZRGBA(const ZRGBA_POD& other)
		{
		red = other.red;
		green = other.green;
		blue = other.blue;
		alpha = other.alpha;
		}

	ZRGBA& operator=(const ZRGBA_POD& other)
		{
		red = other.red;
		green = other.green;
		blue = other.blue;
		alpha = other.alpha;
		return *this;
		}

	#if ZCONFIG_SPI_Enabled(QuickDraw)
	ZRGBA(const RGBColor& iRGBColor)
		{
		red = iRGBColor.red;
		green = iRGBColor.green;
		blue = iRGBColor.blue;
		alpha = 0xFFFFU;
		}

	ZRGBA& operator=(const RGBColor& iRGBColor)
		{
		red = iRGBColor.red;
		green = iRGBColor.green;
		blue = iRGBColor.blue;
		alpha = 0xFFFFU;
		return *this;
		}
	#endif

	#if ZCONFIG_SPI_Enabled(X11)
	ZRGBA(const XColor& iXColor)
		{
		red = iXColor.red;
		green = iXColor.green;
		blue = iXColor.blue;
		alpha = 0xFFFFU;
		}

	ZRGBA& operator=(const XColor& iXColor)
		{
		red = iXColor.red;
		green = iXColor.green;
		blue = iXColor.blue;
		alpha = 0xFFFFU;
		return *this;
		}
	#endif

	#if ZCONFIG_SPI_Enabled(BeOS)
	ZRGBA(const rgb_color& other)
		{
		red = uint16(other.red * 0x101);
		green = uint16(other.green * 0x101);
		blue = uint16(other.blue * 0x101);
		alpha = uint16(other.alpha * 0x101);
		return *this;
		}

	ZRGBA& operator=(const rgb_color& other)
		{
		red = uint16(other.red * 0x101);
		green = uint16(other.green * 0x101);
		blue = uint16(other.blue * 0x101);
		alpha = uint16(other.alpha * 0x101);
		return *this;
		}
	#endif

	ZRGBA() {}

	explicit ZRGBA(uint16 iGray)
		{
		red = green = blue = iGray;
		alpha = 0xFFFFU;
		}

	ZRGBA(uint16 iGray, uint16 iAlpha)
		{
		red = green = blue = iGray;
		alpha = iAlpha;
		}

	ZRGBA(uint16 iRed, uint16 iGreen, uint16 iBlue)
		{
		red = iRed;
		green = iGreen;
		blue = iBlue;
		alpha = 0xFFFFU;
		}

	ZRGBA(uint16 iRed, uint16 iGreen, uint16 iBlue, uint16 iAlpha)
		{
		red = iRed;
		green = iGreen;
		blue = iBlue;
		alpha = iAlpha;
		}

	explicit ZRGBA(int iGray)
		{
		red = green = blue = uint16(iGray);
		alpha = 0xFFFFU;
		}

	ZRGBA(int iGray, int iAlpha)
		{
		red = green = blue = uint16(iGray);
		alpha = uint16(iAlpha);
		}

	ZRGBA(int iRed, int iGreen, int iBlue)
		{
		red = uint16(iRed);
		green = uint16(iGreen);
		blue = uint16(iBlue);
		alpha = 0xFFFFU;
		}

	ZRGBA(int iRed, int iGreen, int iBlue, int iAlpha)
		{
		red = uint16(iRed);
		green = uint16(iGreen);
		blue = uint16(iBlue);
		alpha = uint16(iAlpha);
		}

	explicit ZRGBA(unsigned int iGray)
		{
		red = green = blue = uint16(iGray);
		alpha = 0xFFFFU;
		}

	ZRGBA(unsigned int iGray, unsigned int iAlpha)
		{
		red = green = blue = uint16(iGray);
		alpha = uint16(iAlpha);
		}

	ZRGBA(unsigned int iRed, unsigned int iGreen, unsigned int iBlue)
		{
		red = uint16(iRed);
		green = uint16(iGreen);
		blue = uint16(iBlue);
		alpha = 0xFFFFU;
		}

	ZRGBA(unsigned int iRed, unsigned int iGreen, unsigned int iBlue, unsigned int iAlpha)
		{
		red = uint16(iRed);
		green = uint16(iGreen);
		blue = uint16(iBlue);
		alpha = uint16(iAlpha);
		}

	explicit ZRGBA(float iGray)
		{
		red = green = blue = uint16(iGray * 65535);
		alpha = 0xFFFFU;
		}

	ZRGBA(float iGray, float iAlpha)
		{
		red = green = blue = uint16(iGray * 65535);
		alpha = uint16(iAlpha * 65535);
		}

	ZRGBA(float iRed, float iGreen, float iBlue)
		{
		red = uint16(iRed * 65535);
		green = uint16(iGreen * 65535);
		blue = uint16(iBlue * 65535);
		alpha = 0xFFFFU;
		}

	ZRGBA(float iRed, float iGreen, float iBlue, float iAlpha)
		{
		red = uint16(iRed * 65535);
		green = uint16(iGreen * 65535);
		blue = uint16(iBlue * 65535);
		alpha = uint16(iAlpha * 65535);
		}

	explicit ZRGBA(double iGray)
		{
		red = green = blue = uint16(iGray * 65535);
		alpha = 0xFFFFU;
		}

	ZRGBA(double iGray, double iAlpha)
		{
		red = green = blue = uint16(iGray * 65535);
		alpha = uint16(iAlpha * 65535);
		}

	ZRGBA(double iRed, double iGreen, double iBlue)
		{
		red = uint16(iRed * 65535);
		green = uint16(iGreen * 65535);
		blue = uint16(iBlue * 65535);
		alpha = 0xFFFFU;
		}

	ZRGBA(double iRed, double iGreen, double iBlue, double iAlpha)
		{
		red = uint16(iRed * 65535);
		green = uint16(iGreen * 65535);
		blue = uint16(iBlue * 65535);
		alpha = uint16(iAlpha * 65535);
		}
	};

// =================================================================================================
// MARK: - ZRGBAMap

class ZRGBAMap
	{
public:
	uint8 fPixval;
	ZRGBA_POD fColor;
	};

} // namespace ZooLib

#endif // __ZRGBA_h__
