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

#ifndef __ZooLib_Pixels_PixvalAccessor_h__
#define __ZooLib_Pixels_PixvalAccessorl_h__ 1
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
