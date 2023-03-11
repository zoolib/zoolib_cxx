// Copyright (c) 2006 Andrew Green and Learning in Motion, Inc.
// MIT License. http://www.zoolib.org

#ifndef __ZooLib_Chan_Bin_ASCIIStrim_h__
#define __ZooLib_Chan_Bin_ASCIIStrim_h__ 1
#include "zconfig.h"

#include "zoolib/ChanR_Bin.h"
#include "zoolib/ChanW_Bin.h"
#include "zoolib/ChanR_UTF.h"
#include "zoolib/ChanW_UTF.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - ChanR_Bin_ASCIIStrim

/// A read filter stream that reads only the ASCII-range code points from a strim.

class ChanR_Bin_ASCIIStrim
:	public virtual ChanR_Bin
	{
public:
	ChanR_Bin_ASCIIStrim(const ChanR_UTF& iChanR);

// From ChanAspect_Read
	virtual size_t Read(byte* oDest, size_t iCount);

private:
	const ChanR_UTF& fChanR_UTF;
	};

// =================================================================================================
#pragma mark - ChanW_Bin_ASCIIStrim

/// A write filter stream that writes only the ASCII-range bytes to a strim.

class ChanW_Bin_ASCIIStrim
:	public virtual ChanW_Bin
	{
public:
	ChanW_Bin_ASCIIStrim(const ChanW_UTF& iChanW);

// From ChanAspect_Write
	virtual size_t Write(const byte* iSource, size_t iCount);

private:
	const ChanW_UTF& fChanW_UTF;
	};

} // namespace ZooLib

#endif // __ZooLib_Chan_Bin_ASCIIStrim_h__
