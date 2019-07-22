// Copyright (c) 2019 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Pixels_PixvalDesc_h__
#define __ZooLib_Pixels_PixvalDesc_h__ 1
#include "zconfig.h"

namespace ZooLib {
namespace Pixels {

// =================================================================================================
#pragma mark - PixvalDesc

/** PixvalDesc describes how a single pixval is laid out. */

class PixvalDesc
	{
public:
	PixvalDesc() {}

	PixvalDesc(int iDepth, bool iBigEndian)
	:	fDepth(iDepth)
	,	fBigEndian(iBigEndian)
		{}

	bool operator==(const PixvalDesc& iOther) const
		{ return fDepth == iOther.fDepth && fBigEndian == iOther.fBigEndian; }

	int fDepth;
	bool fBigEndian;
	};

} // namespace Pixels
} // namespace ZooLib

#endif // __ZooLib_Pixels_PixvalDesc_h__
