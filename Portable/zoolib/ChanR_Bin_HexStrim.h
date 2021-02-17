// Copyright (c) 2009 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_ChanR_Bin_HexStrim_h__
#define __ZooLib_ChanR_Bin_HexStrim_h__ 1
#include "zconfig.h"

#include "zoolib/ChanR_Bin.h"
#include "zoolib/ChanRU_UTF.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - ChanR_Bin_HexStrim

/// A read filter stream that reads byte values from a strim, where they're encoded as hex digits.

class ChanR_Bin_HexStrim
:	public ChanR_Bin
	{
public:
	ChanR_Bin_HexStrim(const ChanRU_UTF& iChanRU);
	ChanR_Bin_HexStrim(bool iAllowUnderscore, const ChanRU_UTF& iChanRU);
	~ChanR_Bin_HexStrim();

// From ChanAspect_Read
	virtual size_t Read(byte* oDest, size_t iCount);

private:
	const ChanRU_UTF& fChanRU;
	bool fAllowUnderscore;
	};

} // namespace ZooLib

#endif // __ZooLib_ChanR_Bin_HexStrim_h__
