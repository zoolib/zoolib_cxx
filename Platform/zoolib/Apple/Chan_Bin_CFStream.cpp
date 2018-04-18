/* -------------------------------------------------------------------------------------------------
Copyright (c) 2003 Andrew Green and Learning in Motion, Inc.
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

#include "zoolib/Apple/Chan_Bin_CFStream.h"

#if ZCONFIG_SPI_Enabled(CoreFoundation)

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark ChanR_Bin_CFStream

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
#pragma mark -
#pragma mark ChanW_Bin_CFStream

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
