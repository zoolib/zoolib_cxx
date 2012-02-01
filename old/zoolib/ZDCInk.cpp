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

#include "zoolib/ZDCInk.h"
#include "zoolib/ZDCPixmap.h"
#include "zoolib/ZDebug.h"

namespace ZooLib {

// Because our static sGray, sBlack stuff are constructed before we've been able to call
// InitGraf(), qd.white, qd.gray etc. will not have valid values, so we have our own
// copies of them as follows:
const ZDCPattern ZDCPattern::sBlack = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
const ZDCPattern ZDCPattern::sDkGray = { 0x22, 0x88, 0x22, 0x88, 0x22, 0x88, 0x22, 0x88 };
const ZDCPattern ZDCPattern::sGray = { 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA };
const ZDCPattern ZDCPattern::sLtGray = { 0x77, 0xDD, 0x77, 0xDD, 0x77, 0xDD, 0x77, 0xDD };
const ZDCPattern ZDCPattern::sWhite = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

const ZDCPattern ZDCPattern::sBackOnly = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
const ZDCPattern ZDCPattern::sForeOnly = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

// Same static initialization issue applies to ZRGBColor
static const ZRGBColor spLocalBlack(0x0000);
static const ZRGBColor spLocalWhite(0xFFFF);

const ZDCInk ZDCInk::sBlack(spLocalBlack);
const ZDCInk ZDCInk::sWhite(spLocalWhite);
const ZDCInk ZDCInk::sGray(spLocalWhite, spLocalBlack, ZDCPattern::sGray);
const ZDCInk ZDCInk::sDkGray(spLocalWhite, spLocalBlack, ZDCPattern::sDkGray);
const ZDCInk ZDCInk::sLtGray(spLocalWhite, spLocalBlack, ZDCPattern::sLtGray);

#define ALLONES ZUINT64_C(0xFFFFFFFFFFFFFFFF)

// =================================================================================================
// MARK: - ZDCInk

#if ZCONFIG(Compiler, CodeWarrior)
// Enforce higher level of optimization for this code -- otherwise
// the funky switch stuff is all for naught.
	#pragma push
	#pragma optimization_level 4
	#pragma opt_common_subs on
	#pragma opt_lifetimes on
	#pragma peephole on
	#if ZCONFIG(Processor, x86)
		#pragma register_coloring on
	#endif
#endif

#if ZCONFIG(Compiler, CodeWarrior) && (ZCONFIG(Processor, 68K) || ZCONFIG(Processor, x86))
	#define RotateLeft(a,b,c) __rol(a,b)
#else
	#define RotateLeft(value, distance, eightMinusDistance) \
	(((value) << (distance)) | ((value) >> (eightMinusDistance)))
#endif

void ZDCPattern::sOffset(int32 inOffsetH, int32 inOffsetV, const uint8* inSource, uint8* inDest)
	{
	// Get our offsets into the range [0-7]. We need to use modulus, rather than simply
	// truncating at 3 bits, because we must handle negative offsets correctly.
	register const uint8 offsetH = uint8(((inOffsetH % 8) + 8) % 8);
	register const uint8 offsetV = uint8(((inOffsetV % 8) + 8) % 8);

	register const uint8 eightMinusOffsetH = 8 - offsetH;

	// The switch statements let us take advantage of constant offset instructions. CW's code
	// generation on x86 is is sucky though -- repeatedly loading the value of offsetH into cl.
	if (offsetH && offsetV)
		{
		register uint8* localDest = inDest - offsetV;
		switch (offsetV)
			{
			case 0: localDest[0] = RotateLeft(inSource[0], offsetH, eightMinusOffsetH);
			case 1: localDest[1] = RotateLeft(inSource[1], offsetH, eightMinusOffsetH);
			case 2: localDest[2] = RotateLeft(inSource[2], offsetH, eightMinusOffsetH);
			case 3: localDest[3] = RotateLeft(inSource[3], offsetH, eightMinusOffsetH);
			case 4: localDest[4] = RotateLeft(inSource[4], offsetH, eightMinusOffsetH);
			case 5: localDest[5] = RotateLeft(inSource[5], offsetH, eightMinusOffsetH);
			case 6: localDest[6] = RotateLeft(inSource[6], offsetH, eightMinusOffsetH);
			case 7: localDest[7] = RotateLeft(inSource[7], offsetH, eightMinusOffsetH);
			}
		register const uint8* localSource = inSource + offsetV - 7;
		switch (offsetV)
			{
			case 7: inDest[1] = RotateLeft(localSource[0], offsetH, eightMinusOffsetH);
			case 6: inDest[2] = RotateLeft(localSource[1], offsetH, eightMinusOffsetH);
			case 5: inDest[3] = RotateLeft(localSource[2], offsetH, eightMinusOffsetH);
			case 4: inDest[4] = RotateLeft(localSource[3], offsetH, eightMinusOffsetH);
			case 3: inDest[5] = RotateLeft(localSource[4], offsetH, eightMinusOffsetH);
			case 2: inDest[6] = RotateLeft(localSource[5], offsetH, eightMinusOffsetH);
			case 1: inDest[7] = RotateLeft(localSource[6], offsetH, eightMinusOffsetH);
			}
		}
	else if (offsetH)
		{
		inDest[0] = RotateLeft(inSource[0], offsetH, eightMinusOffsetH);
		inDest[1] = RotateLeft(inSource[1], offsetH, eightMinusOffsetH);
		inDest[2] = RotateLeft(inSource[2], offsetH, eightMinusOffsetH);
		inDest[3] = RotateLeft(inSource[3], offsetH, eightMinusOffsetH);
		inDest[4] = RotateLeft(inSource[4], offsetH, eightMinusOffsetH);
		inDest[5] = RotateLeft(inSource[5], offsetH, eightMinusOffsetH);
		inDest[6] = RotateLeft(inSource[6], offsetH, eightMinusOffsetH);
		inDest[7] = RotateLeft(inSource[7], offsetH, eightMinusOffsetH);
		}
	else if (offsetV)
		{
		register uint8* localDest = inDest - offsetV;
		switch (offsetV)
			{
			case 0: localDest[0] = inSource[0];
			case 1: localDest[1] = inSource[1];
			case 2: localDest[2] = inSource[2];
			case 3: localDest[3] = inSource[3];
			case 4: localDest[4] = inSource[4];
			case 5: localDest[5] = inSource[5];
			case 6: localDest[6] = inSource[6];
			case 7: localDest[7] = inSource[7];
			}
		register const uint8* localSource = inSource + offsetV - 7;
		switch (offsetV)
			{
			case 7: inDest[1] = localSource[0];
			case 6: inDest[2] = localSource[1];
			case 5: inDest[3] = localSource[2];
			case 4: inDest[4] = localSource[3];
			case 3: inDest[5] = localSource[4];
			case 2: inDest[6] = localSource[5];
			case 1: inDest[7] = localSource[6];
			}
		}
	else
		{
		(*(uint32*)(inDest)) = (*(uint32*)(inSource));
		(*(uint32*)(inDest + 4)) = (*(uint32*)(inSource + 4));
		}
	}

#if ZCONFIG(Compiler, CodeWarrior)
	#pragma pop
#endif

// =================================================================================================
// MARK: - ZDCInkRep

ZDCInkRep::ZDCInkRep(const ZDCInkRep& inOther)
	{
	if (inOther.fType == ZDCInk::eTypeSolidColor)
		{
		fAsSolidColor.fColor = inOther.fAsSolidColor.fColor;
		}
	else if (inOther.fType == ZDCInk::eTypeTwoColor)
		{
		fAsTwoColor.fForeColor = inOther.fAsTwoColor.fForeColor;
		fAsTwoColor.fBackColor = inOther.fAsTwoColor.fBackColor;
		fAsTwoColor.fPattern = inOther.fAsTwoColor.fPattern;
		}
	else if (inOther.fType == ZDCInk::eTypeMultiColor)
		{
		fAsMultiColor.fPixmap = new ZDCPixmap(*inOther.fAsMultiColor.fPixmap);
		}
	else
		{
		ZUnimplemented();
		}
	fType = inOther.fType;
	}

ZDCInkRep::ZDCInkRep(const ZRGBColorPOD& inSolidColor)
	{
	fType = ZDCInk::eTypeSolidColor;
	fAsSolidColor.fColor = inSolidColor;
	}

ZDCInkRep::ZDCInkRep(const ZRGBColorPOD& inForeColor,
	const ZRGBColorPOD& inBackColor,
	const ZDCPattern& inPattern)
	{
	fType = ZDCInk::eTypeTwoColor;
	fAsTwoColor.fForeColor = inForeColor;
	fAsTwoColor.fBackColor = inBackColor;
	fAsTwoColor.fPattern = inPattern;
	}

ZDCInkRep::ZDCInkRep(const ZDCPixmap& inPixmap)
	{
	fType = ZDCInk::eTypeMultiColor;
	fAsMultiColor.fPixmap = new ZDCPixmap(inPixmap);
	}

ZDCInkRep::~ZDCInkRep()
	{
	// If we're holding structured data, lose it. Flat data we don't have to worry about
	if (fType == ZDCInk::eTypeMultiColor)
		delete fAsMultiColor.fPixmap;
	}

// =================================================================================================
// MARK: - ZDCInk

ZDCInk::ZDCInk(const ZDCPixmap& inPixmap)
:	fRep(inPixmap ? new ZDCInkRep(inPixmap) : nullptr)
	{}

static void spExaminePattern(const ZDCPattern& inPattern, bool& outSolidFore, bool& outSolidBack)
	{
	outSolidFore = ((*(uint64*)(inPattern.pat)) == ALLONES);
	outSolidBack = ((*(uint64*)(inPattern.pat)) == 0);
	}

static void spExaminePatterns(const ZDCPattern& inPattern1, const ZDCPattern& inPattern2, 
					bool& outIdentical, bool& outIdenticalInverse,
					bool& outPattern1SolidFore, bool& outPattern1SolidBack,
					bool& outPattern2SolidFore, bool& outPattern2SolidBack)
	{
	outIdentical = ((*(uint64*)(inPattern1.pat)) == (*(uint64*)(inPattern2.pat)));
	outIdenticalInverse = ALLONES
		== (((*(uint64*)(inPattern1.pat)) ^ (*(uint64*)(inPattern2.pat))));

	outPattern1SolidFore = ALLONES == (*(uint64*)(inPattern1.pat));
	outPattern2SolidFore = ALLONES == (*(uint64*)(inPattern2.pat));

	outPattern1SolidBack = 0 == (*(uint64*)(inPattern1.pat));
	outPattern2SolidBack = 0 == (*(uint64*)(inPattern2.pat));
	}

static inline bool sSameColors(const ZRGBColorPOD& inColor1, const ZRGBColorPOD& inColor2)
	{
	return (inColor1.red == inColor2.red)
		&& (inColor1.green == inColor2.green)
		&& (inColor1.blue == inColor2.blue);
	}

bool ZDCInk::Internal_IsSameAs(const ZDCInk& inOther) const
	{
	if (!fRep || !inOther.fRep)
		{
		// The easy case. One or other of the reps is nullptr, and thus they can't be equivalent.
		return false;
		}

	// Neither rep is nil. IsSameAs has already determined they're not the same rep.
	if (fRep->fType == inOther.fRep->fType)
		{
		if (fRep->fType == eTypeSolidColor)
			{
			return sSameColors(fRep->fAsSolidColor.fColor, inOther.fRep->fAsSolidColor.fColor);
			}
		else if (fRep->fType == eTypeTwoColor)
			{
			bool identical;
			bool identicalInverse;
			bool pattern1SolidFore;
			bool pattern1SolidBack;
			bool pattern2SolidFore;
			bool pattern2SolidBack;
			spExaminePatterns(fRep->fAsTwoColor.fPattern, inOther.fRep->fAsTwoColor.fPattern,
				identical, identicalInverse,
				pattern1SolidFore, pattern1SolidBack,
				pattern2SolidFore, pattern2SolidBack);

			bool sameForeColors = sSameColors(fRep->fAsTwoColor.fForeColor,
				inOther.fRep->fAsTwoColor.fForeColor);

			bool sameBackColors = sSameColors(fRep->fAsTwoColor.fBackColor,
				inOther.fRep->fAsTwoColor.fBackColor);

			bool sameForeColorsInverse = sSameColors(fRep->fAsTwoColor.fForeColor,
				inOther.fRep->fAsTwoColor.fBackColor);

			bool sameBackColorsInverse = sSameColors(fRep->fAsTwoColor.fBackColor,
				inOther.fRep->fAsTwoColor.fForeColor);

			if (identical && sameForeColors && sameBackColors)
				return true;
			if (identicalInverse && sameForeColorsInverse && sameBackColorsInverse)
				return true;
			if (pattern1SolidFore && pattern2SolidFore && sameForeColors)
				return true;
			if (pattern1SolidBack && pattern2SolidBack && sameBackColors)
				return true;
			if (pattern1SolidFore && pattern2SolidBack && sameForeColorsInverse)
				return true;
			if (pattern1SolidBack && pattern2SolidFore && sameBackColorsInverse)
				return true;
			return false;
			}
		else if (fRep->fType == eTypeMultiColor)
			{
			// I'm not going to bother checking pixmap exact pixel equivalence,
			// just if they share a rep.
			return fRep->fAsMultiColor.fPixmap->GetRep()
				== inOther.fRep->fAsMultiColor.fPixmap->GetRep();
			}
		ZUnimplemented();
		return false;
		}
	else if (fRep->fType == eTypeSolidColor && inOther.fRep->fType == eTypeTwoColor)
		{
		bool solidFore, solidBack;
		spExaminePattern(inOther.fRep->fAsTwoColor.fPattern, solidFore, solidBack);

		if (solidFore
			&& sSameColors(fRep->fAsSolidColor.fColor, inOther.fRep->fAsTwoColor.fForeColor))
			{
			return true;
			}

		if (solidBack
			&& sSameColors(fRep->fAsSolidColor.fColor, inOther.fRep->fAsTwoColor.fBackColor))
			{
			return true;
			}
		}
	else if (fRep->fType == eTypeTwoColor && inOther.fRep->fType == eTypeSolidColor)
		{
		bool solidFore, solidBack;
		spExaminePattern(fRep->fAsTwoColor.fPattern, solidFore, solidBack);

		if (solidFore
			&& sSameColors(fRep->fAsTwoColor.fForeColor, inOther.fRep->fAsSolidColor.fColor))
			{
			return true;
			}

		if (solidBack
			&& sSameColors(fRep->fAsTwoColor.fBackColor, inOther.fRep->fAsSolidColor.fColor))
			{
			return true;
			}
		}

	return false;
	}

} // namespace ZooLib
