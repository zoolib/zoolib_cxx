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

#include "zoolib/Pixels/RasterDesc.h"

namespace ZooLib {
namespace Pixels {

// =================================================================================================
#pragma mark -

int sCalcRowBytes(int iWidth, int iDepth, int iByteRound)
	{
	const int bitRound = iByteRound * 8;
	return ((iWidth * iDepth + bitRound - 1) / bitRound) * iByteRound;
	}

const void* sCalcRowAddress(const RasterDesc& iRD, const void* iBaseAddress, int iRow)
	{
	return static_cast<const char*>(iBaseAddress)
		+ iRD.fRowBytes * (iRD.fFlipped ? iRD.fRowCount - iRow - 1 : iRow);
	}
	
void* sCalcRowAddress(const RasterDesc& iRD, void* iBaseAddress, int iRow)
	{
	return static_cast<char*>(iBaseAddress)
		+ iRD.fRowBytes * (iRD.fFlipped ? iRD.fRowCount - iRow - 1 : iRow);
	}
	
} // namespace Pixels
} // namespace ZooLib

