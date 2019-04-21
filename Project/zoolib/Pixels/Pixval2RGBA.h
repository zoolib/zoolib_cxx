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

#ifndef __ZooLib_Pixels_Pixval2RGBA_h__
#define __ZooLib_Pixels_Pixval2RGBA_h__ 1
#include "zconfig.h"

#include "zoolib/Pixels/Pixval.h"
#include "zoolib/Pixels/RGBA.h"

namespace ZooLib {
namespace Pixels {

// =================================================================================================
#pragma mark -

class Pixval2RGBA_Indexed
	{             
protected:
	Pixval2RGBA_Indexed() {}
	Pixval2RGBA_Indexed(const Pixval2RGBA_Indexed&) {}
	~Pixval2RGBA_Indexed() {}
	Pixval2RGBA_Indexed& operator=(const Pixval2RGBA_Indexed&) { return *this; }

public:
	RGBA AsRGBA(uint32 iPixval) const
		{
		if (fPixvals)
			return this->pAsRGBA(iPixval);
		return fColors[iPixval];
		}

	Comp AsAlpha(uint32 iPixval) const
		{
		if (fPixvals)
			return this->pAsAlpha(iPixval);
		else
			return sAlpha(fColors[iPixval]);
		}

	void AsRGBAs(const uint32* iPixvals, size_t iCount, RGBA* oColors) const;

protected:
	RGBA pAsRGBA(uint32 iPixval) const;
	Comp pAsAlpha(uint32 iPixval) const;

	RGBA* fColors;
	size_t fCount;
	uint32* fPixvals;
	};

// =================================================================================================
#pragma mark -

class Pixval2RGBA_Gray
	{
protected:
	Pixval2RGBA_Gray() {}
	Pixval2RGBA_Gray(const Pixval2RGBA_Gray&) {}
	~Pixval2RGBA_Gray() {}
	Pixval2RGBA_Gray& operator=(const Pixval2RGBA_Gray&) { return *this; }

public:
	static Comp sConvert(
		uint32 iPixval, uint32 iMask, int32 iShift, uint32 iMultiplier, uint32 iAdd)
		{
		return (iAdd + (((iPixval & iMask) >> iShift) * iMultiplier)) / Comp(0xFFFFFFFF);
		}

	RGBA AsRGBA(uint32 iPixval) const
		{
		return sRGBA(
			sConvert(iPixval, fMaskL, fShiftL, fMultiplierL, fAddL),
			sConvert(iPixval, fMaskA, fShiftA, fMultiplierA, fAddA));
		}

	Comp AsAlpha(uint32 iPixval) const
		{
		return sConvert(iPixval, fMaskA, fShiftA, fMultiplierA, fAddA);
		}

	void AsRGBAs(const uint32* iPixvals, size_t iCount, RGBA* oColors) const;

protected:
	int fShiftL;
	uint32 fMultiplierL;
	uint32 fMaskL;
	uint32 fAddL;

	int fShiftA;
	uint32 fMultiplierA;
	uint32 fMaskA;
	uint32 fAddA;
	};

// =================================================================================================
#pragma mark -

class Pixval2RGBA_Color
	{
protected:
	Pixval2RGBA_Color() {}
	Pixval2RGBA_Color(const Pixval2RGBA_Color&) {}
	~Pixval2RGBA_Color() {}
	Pixval2RGBA_Color& operator=(const Pixval2RGBA_Color&) { return *this; }

public:
	static Comp sConvert(
		uint32 iPixval, uint32 iMask, int32 iShift, uint32 iMultiplier, uint32 iAdd)
		{
		return iAdd + (((iPixval & iMask) >> iShift) * iMultiplier) / Comp(0xFFFFFFFF);
		}

	RGBA AsRGBA(uint32 iPixval) const
		{
		return sRGBA(
			sConvert(iPixval, fMaskR, fShiftR, fMultiplierR, fAddR),
			sConvert(iPixval, fMaskG, fShiftG, fMultiplierG, fAddG),
			sConvert(iPixval, fMaskB, fShiftB, fMultiplierB, fAddB),
			sConvert(iPixval, fMaskA, fShiftA, fMultiplierA, fAddA));
		}

	void AsRGBAs(const uint32* iPixvals, size_t iCount, RGBA* oColors) const;

	Comp AsAlpha(uint32 iPixval) const
		{
		return sConvert(iPixval, fMaskA, fShiftA, fMultiplierA, fAddA);;
		}

protected:
	int fShiftR;
	uint32 fMultiplierR;
	uint32 fMaskR;
	uint32 fAddR;

	int fShiftG;
	uint32 fMultiplierG;
	uint32 fMaskG;
	uint32 fAddG;

	int fShiftB;
	uint32 fMultiplierB;
	uint32 fMaskB;
	uint32 fAddB;

	int fShiftA;
	uint32 fMultiplierA;
	uint32 fMaskA;
	uint32 fAddA;
	};

} // namespace Pixels
} // namespace ZooLib

#endif // __ZooLib_Pixels_Pixval2RGBA_h__
