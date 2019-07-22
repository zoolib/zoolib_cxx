// Copyright (c) 2019 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Pixels_PixvalAccessor_h__
#define __ZooLib_Pixels_PixvalAccessor_h__ 1
#include "zconfig.h"

#include "zoolib/Pixels/Pixval.h"
#include "zoolib/Pixels/PixvalDesc.h"

namespace ZooLib {
namespace Pixels {

// =================================================================================================
#pragma mark - PixvalAccessor

/** PixvalAccessor provides for efficient access to
pixvals by precomputing and reusing key information. */

class PixvalAccessor
	{
public:
	PixvalAccessor(const PixvalDesc& iPixvalDesc);
	PixvalAccessor(int iDepth, bool iBigEndian);

	Pixval GetPixval(const void* iRowAddress, int iH) const;
	void SetPixval(void* iRowAddress, int iH, Pixval iPixval) const;

	void GetPixvals(const void* iRowAddress, int iStartH, size_t iCount, Pixval* oPixvals) const;
	void SetPixvals(void* iRowAddress, int iStartH, size_t iCount, const Pixval* iPixvals) const;

protected:
	int fDepth;
	int fDivisorShift;
	int fModMask;
	const uint8* fBitMask;
	const uint8* fBitNotMask;
	const uint8* fBitShift;
	enum ECase
		{
		eCase1Byte, eCase1ByteWithShiftBE, eCase1ByteWithShiftLE,
		eCase2Bytes, eCase2BytesWithSwap,
		eCase3BytesBEHostBE, eCase3BytesBEHostLE, eCase3BytesLEHostBE, eCase3BytesLEHostLE,
		eCase4Bytes, eCase4BytesWithSwap
		};
	ECase fWhichCase;
	};

} // namespace Pixels
} // namespace ZooLib

#endif // __ZooLib_Pixels_PixvalAccessor_h__
