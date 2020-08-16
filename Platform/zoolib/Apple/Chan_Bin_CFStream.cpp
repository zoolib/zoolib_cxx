// Copyright (c) 2003 Andrew Green and Learning in Motion, Inc.
// MIT License. http://www.zoolib.org

#include "zoolib/Apple/Chan_Bin_CFStream.h"

#if ZCONFIG_SPI_Enabled(CoreFoundation)

namespace ZooLib {

// =================================================================================================
#pragma mark - ChanR_Bin_CFStream

ChanR_Bin_CFStream::ChanR_Bin_CFStream(CFReadStreamRef iCFStream)
:	fCFStream(iCFStream)
	{}

ChanR_Bin_CFStream::~ChanR_Bin_CFStream()
	{}

size_t ChanR_Bin_CFStream::Read(byte* oDest, size_t iCount)
	{
	if (fCFStream)
		{
		CFIndex result = ::CFReadStreamRead(fCFStream, (byte*)oDest, iCount);
		if (result > 0)
			return result;
		}
	return 0;
	}

size_t ChanR_Bin_CFStream::CountReadable()
	{
	if (fCFStream && ::CFReadStreamHasBytesAvailable(fCFStream))
		return 1;
	return 0;
	}

// =================================================================================================
#pragma mark - ChanW_Bin_CFStream

ChanW_Bin_CFStream::ChanW_Bin_CFStream(CFWriteStreamRef iCFStream)
:	fCFStream(iCFStream)
	{}

ChanW_Bin_CFStream::~ChanW_Bin_CFStream()
	{}

size_t ChanW_Bin_CFStream::Write(const byte* iSource, size_t iCount)
	{
	if (fCFStream)
		{
		CFIndex result = ::CFWriteStreamWrite(fCFStream, (byte*)iSource, iCount);
		if (result > 0)
			return result;
		}
	return 0;
	}

} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(CoreFoundation)
