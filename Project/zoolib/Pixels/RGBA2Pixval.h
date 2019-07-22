// Copyright (c) 2019 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Pixels_RGBA2Pixval_h__
#define __ZooLib_Pixels_RGBA2Pixval_h__ 1
#include "zconfig.h"

#include "zoolib/Pixels/Pixval.h"
#include "zoolib/Pixels/RGBA.h"

namespace ZooLib {
namespace Pixels {

// =================================================================================================
#pragma mark -

class RGBA2Pixval_Indexed
	{
protected:
	RGBA2Pixval_Indexed() {}
	RGBA2Pixval_Indexed(const RGBA2Pixval_Indexed&) {}
	~RGBA2Pixval_Indexed() {}
	RGBA2Pixval_Indexed& operator=(const RGBA2Pixval_Indexed&) { return *this; }

public:
	uint32 AsPixval(const RGBA& iRGBA) const
		{
		uint8 index = fReverseLookup[
			int(16 * sBlue(iRGBA)) + int(256 * sGreen(iRGBA)) + int(4096 * sRed(iRGBA))];
		if (fPixvals)
			return fPixvals[index];
		return index;
		}

	void AsPixvals(const RGBA* iColors, size_t iCount, uint32* oPixvals) const;

protected:
	mutable uint8* fReverseLookup;
	uint32* fPixvals;
	};

// =================================================================================================
#pragma mark -

class RGBA2Pixval_Gray
	{
protected:
	RGBA2Pixval_Gray() {}
	RGBA2Pixval_Gray(const RGBA2Pixval_Gray&) {}
	~RGBA2Pixval_Gray() {}
	RGBA2Pixval_Gray& operator=(const RGBA2Pixval_Gray&) { return *this; }

public:
	uint32 AsPixval(const RGBA& iRGBA) const
		{
		uint32 theGray = (sRed(iRGBA) + sGreen(iRGBA) + sBlue(iRGBA)) / 3.0 * 65535.0;

		uint32 theValue = (theGray >> fShiftRightL) << fShiftLeftL;
		theValue |= ((uint32(sAlpha(iRGBA) * 65535) >> fShiftRightA) << fShiftLeftA);
		return theValue;
		}

	void AsPixvals(const RGBA* iColors, size_t iCount, uint32* oPixvals) const;

protected:
	int fShiftRightL;
	int fShiftLeftL;

	int fShiftRightA;
	int fShiftLeftA;
	};

// =================================================================================================
#pragma mark -

class RGBA2Pixval_Color
	{
protected:
	RGBA2Pixval_Color() {}
	RGBA2Pixval_Color(const RGBA2Pixval_Color&) {}
	~RGBA2Pixval_Color() {}
	RGBA2Pixval_Color& operator=(const RGBA2Pixval_Color&) { return *this; }

public:
	uint32 AsPixval(const RGBA& iRGBA) const
		{
		uint32 theValue = 0;
		theValue |= ((uint32(sRed(iRGBA) * 65535) >> fShiftRightR) << fShiftLeftR);
		theValue |= ((uint32(sGreen(iRGBA) * 65535) >> fShiftRightG) << fShiftLeftG);
		theValue |= ((uint32(sBlue(iRGBA) * 65535) >> fShiftRightB) << fShiftLeftB);
		theValue |= ((uint32(sAlpha(iRGBA) * 65535) >> fShiftRightA) << fShiftLeftA);
		return theValue;
		}

	void AsPixvals(const RGBA* iColors, size_t iCount, uint32* oPixvals) const;

protected:
	int fShiftRightR;
	int fShiftLeftR;

	int fShiftRightG;
	int fShiftLeftG;

	int fShiftRightB;
	int fShiftLeftB;

	int fShiftRightA;
	int fShiftLeftA;
	};

} // namespace Pixels
} // namespace ZooLib

#endif // __ZooLib_Pixels_RGBA2Pixval_h__
