// Copyright (c) 2019 Andrew Green. MIT License. http://www.zoolib.org

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

