// Copyright (c) 2003 Andrew Green and Learning in Motion, Inc.
// MIT License. http://www.zoolib.org

#ifndef __ZooLib_Apple_Chan_Bin_CFStream_h__
#define __ZooLib_Apple_Chan_Bin_CFStream_h__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_SPI.h"

#include "zoolib/ChanR_Bin.h"
#include "zoolib/ChanW_Bin.h"

#if ZCONFIG_SPI_Enabled(CoreFoundation)

#include <CoreFoundation/CFStream.h>

#include "zoolib/Apple/ZP_CF.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - ChanR_Bin_CFStream

class ChanR_Bin_CFStream
:	public ChanR_Bin
	{
public:
	ChanR_Bin_CFStream(CFReadStreamRef iCFStream);
	~ChanR_Bin_CFStream();

// From ChanR_Bin
	virtual size_t Read(byte* oDest, size_t iCount);
	virtual size_t CountReadable();

private:
	ZP<CFReadStreamRef> fCFStream;
	};

// =================================================================================================
#pragma mark - ChanW_Bin_CFStream

class ChanW_Bin_CFStream
:	public ChanW_Bin
	{
public:
	ChanW_Bin_CFStream(CFWriteStreamRef iCFStream);
	~ChanW_Bin_CFStream();

// From ChanW_Bin
	virtual size_t Write(const byte* iSource, size_t iCount);

private:
	ZP<CFWriteStreamRef> fCFStream;
	};

} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(CoreFoundation)

#endif // __ZooLib_Apple_Chan_Bin_CFStream_h__
