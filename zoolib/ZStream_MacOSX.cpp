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

#include "zoolib/ZStream_MacOSX.h"

#if ZCONFIG_SPI_Enabled(CoreFoundation)

NAMESPACE_ZOOLIB_USING

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamR_CFStream

ZStreamR_CFStream::ZStreamR_CFStream(CFReadStreamRef iCFStream)
:	fCFStream(iCFStream)
	{
	if (fCFStream)
		::CFRetain(fCFStream);
	}

ZStreamR_CFStream::~ZStreamR_CFStream()
	{
	if (fCFStream)
		::CFRelease(fCFStream);
	}

void ZStreamR_CFStream::Imp_Read(void* iDest, size_t iCount, size_t* oCountRead)
	{
	if (oCountRead)
		*oCountRead = 0;

	if (fCFStream)
		{
		CFIndex result = ::CFReadStreamRead(fCFStream, static_cast<UInt8*>(iDest), iCount);

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
	{
	if (fCFStream)
		::CFRetain(fCFStream);
	}

ZStreamW_CFStream::~ZStreamW_CFStream()
	{
	if (fCFStream)
		::CFRelease(fCFStream);
	}

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

#if ZCONFIG_SPI_Enabled(CoreGraphics)

// =================================================================================================
#pragma mark -
#pragma mark * ZStream_MacOSX::sCGDataProviderCreate

namespace ZANONYMOUS {

size_t sGetBytesR(void* iInfo, void* iBuffer, size_t iCount)
	{
	size_t countRead;
	static_cast<ZRef<ZStreamerR>*>(iInfo)[0]->GetStreamR().Read(iBuffer, iCount, &countRead);
	return countRead;
	}

void sSkipBytesR(void* iInfo, size_t iCount)
	{
	static_cast<ZRef<ZStreamerR>*>(iInfo)[0]->GetStreamR().Skip(iCount);
	}

void sRewindR(void* iInfo)
	{
	ZUnimplemented();
	}

void sReleaseProviderR(void* iInfo)
	{
	delete static_cast<ZRef<ZStreamerR>*>(iInfo);
	}

CGDataProviderCallbacks sCallbacksR =
	{
	sGetBytesR,
	sSkipBytesR,
	sRewindR,
	sReleaseProviderR
	};

} // anonymous namespace

CGDataProviderRef ZStream_MacOSX::sCGDataProviderCreate(ZRef<ZStreamerR> iStreamer)
	{	
	return ::CGDataProviderCreate(new ZRef<ZStreamerR>(iStreamer), &sCallbacksR);
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZStream_MacOSX::sCGDataProviderCreateRewind

#include "zoolib/ZStreamer.h"

namespace ZANONYMOUS {

size_t sGetBytesRewind(void* iInfo, void* iBuffer, size_t iCount)
	{
	size_t countRead;
	static_cast<ZRef<ZStreamerRPos>*>(iInfo)[0]->GetStreamR().Read(iBuffer, iCount, &countRead);
	return countRead;
	}

void sSkipBytesRewind(void* iInfo, size_t iCount)
	{
	static_cast<ZRef<ZStreamerRPos>*>(iInfo)[0]->GetStreamR().Skip(iCount);
	}

void sRewind(void* iInfo)
	{
	static_cast<ZRef<ZStreamerRPos>*>(iInfo)[0]->GetStreamRPos().SetPosition(0);
	}

void sReleaseProviderRewind(void* iInfo)
	{
	delete static_cast<ZRef<ZStreamerRPos>*>(iInfo);
	}

CGDataProviderCallbacks sCallbacksRewind =
	{
	sGetBytesRewind,
	sSkipBytesRewind,
	sRewind,
	sReleaseProviderRewind
	};

} // anonymous namespace

CGDataProviderRef ZStream_MacOSX::sCGDataProviderCreateRewind(ZRef<ZStreamerRPos> iStreamer)
	{	
	return ::CGDataProviderCreate(new ZRef<ZStreamerRPos>(iStreamer), &sCallbacksRewind);
	}

// =================================================================================================
#pragma mark -
#pragma mark * CGDataConsumerRef, ZStreamerW

namespace ZANONYMOUS {

size_t sPutBytesW(void* iInfo, const void* iBuffer, size_t iCount)
	{
	size_t countWritten;
	static_cast<ZRef<ZStreamerW>*>(iInfo)[0]->GetStreamW().Write(iBuffer, iCount, &countWritten);
	return countWritten;
	}

void sReleaseConsumerW(void* iInfo)
	{
	delete static_cast<ZRef<ZStreamerW>*>(iInfo);
	}

CGDataConsumerCallbacks sCallbacksW =
	{
	sPutBytesW,
	sReleaseConsumerW
	};

} // anonymous namespace

CGDataConsumerRef ZStream_MacOSX::sCGDataConsumerCreate(ZRef<ZStreamerW> iStreamer)
	{
	return ::CGDataConsumerCreate(new ZRef<ZStreamerW>(iStreamer), &sCallbacksW);
	}

#endif // ZCONFIG_SPI_Enabled(CoreGraphics)
