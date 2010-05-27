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

#ifndef __ZStream_MacOSX__
#define __ZStream_MacOSX__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_SPI.h"

#include "zoolib/ZStreamer.h"

#if ZCONFIG_SPI_Enabled(CoreFoundation)

#include <CoreFoundation/CFStream.h>

#if ZCONFIG_SPI_Enabled(CoreGraphics)
#	include ZMACINCLUDE3(ApplicationServices,CoreGraphics,CGDataProvider.h)
#	include ZMACINCLUDE3(ApplicationServices,CoreGraphics,CGDataConsumer.h)
#endif

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamR_CFStream

class ZStreamR_CFStream : public ZStreamR
	{
public:
	ZStreamR_CFStream(CFReadStreamRef iCFStream);
	~ZStreamR_CFStream();

// From ZStreamR
	virtual void Imp_Read(void* oDest, size_t iCount, size_t* oCountRead);
	virtual size_t Imp_CountReadable();

private:
	ZRef<CFReadStreamRef> fCFStream;
	};

typedef ZStreamerR_T<ZStreamR_CFStream> ZStreamerR_CFStream;

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamW_CFStream

class ZStreamW_CFStream : public ZStreamW
	{
public:
	ZStreamW_CFStream(CFWriteStreamRef iCFStream);
	~ZStreamW_CFStream();

// From ZStreamW
	virtual void Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten);

private:
	ZRef<CFWriteStreamRef> fCFStream;
	};

typedef ZStreamerW_T<ZStreamW_CFStream> ZStreamerW_CFStream;

} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(CoreFoundation)

// =================================================================================================
#pragma mark -
#pragma mark * ZStream_MacOSX

#if ZCONFIG_SPI_Enabled(CoreGraphics)

namespace ZooLib {

namespace ZStream_MacOSX {

CGDataProviderRef sCGDataProviderCreate(ZRef<ZStreamerR> iStreamer);

CGDataProviderRef sCGDataProviderCreateRewind(ZRef<ZStreamerRPos> iStreamer);

CGDataConsumerRef sCGDataConsumerCreate(ZRef<ZStreamerW> iStreamer);

} // namespace ZStream_MacOSX

} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(CoreGraphics)

#endif // __ZStream_MacOSX__
