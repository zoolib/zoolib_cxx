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

#include "zoolib/ZStream_CFStream.h"

#if ZCONFIG_SPI_Enabled(CoreFoundation)

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamR_CFStream

ZStreamR_CFStream::ZStreamR_CFStream(CFReadStreamRef iCFStream)
:	fCFStream(iCFStream)
	{}

ZStreamR_CFStream::~ZStreamR_CFStream()
	{}

void ZStreamR_CFStream::Imp_Read(void* oDest, size_t iCount, size_t* oCountRead)
	{
	if (oCountRead)
		*oCountRead = 0;

	if (fCFStream)
		{
		CFIndex result = ::CFReadStreamRead(fCFStream, static_cast<UInt8*>(oDest), iCount);

		if (result > 0 && oCountRead)
			*oCountRead = result;
		}
	}

size_t ZStreamR_CFStream::Imp_CountReadable()
	{
	if (fCFStream && ::CFReadStreamHasBytesAvailable(fCFStream))
		return 1;
	return 0;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamW_CFStream

ZStreamW_CFStream::ZStreamW_CFStream(CFWriteStreamRef iCFStream)
:	fCFStream(iCFStream)
	{}

ZStreamW_CFStream::~ZStreamW_CFStream()
	{}

void ZStreamW_CFStream::Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten)
	{
	if (oCountWritten)
		*oCountWritten = 0;

	if (fCFStream)
		{
		CFIndex result = ::CFWriteStreamWrite(fCFStream,
			static_cast<const UInt8*>(iSource), iCount);

		if (result > 0 && oCountWritten)
			*oCountWritten = result;
		}
	}

#endif // ZCONFIG_SPI_Enabled(CoreFoundation)

} // namespace ZooLib
