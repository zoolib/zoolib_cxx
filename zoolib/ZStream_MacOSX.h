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
#include "ZCONFIG_SPI.h"

#if ZCONFIG_SPI_Enabled(CoreFoundation)
#include <CoreFoundation/CFStream.h>

#include "ZStreamer.h"

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamR_CFStream

class ZStreamR_CFStream : public ZStreamR
	{
public:
	ZStreamR_CFStream(CFReadStreamRef iCFStream);
	~ZStreamR_CFStream();

// From ZStreamR
	virtual void Imp_Read(void* iDest, size_t iCount, size_t* oCountRead);
	virtual size_t Imp_CountReadable();

private:
	CFReadStreamRef fCFStream;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamerR_CFStream

class ZStreamerR_CFStream : public ZStreamerR
	{
public:
	ZStreamerR_CFStream(CFReadStreamRef iCFStream);
	virtual ~ZStreamerR_CFStream();

// From ZStreamerR
	virtual const ZStreamR& GetStreamR();

protected:
	ZStreamR_CFStream fStream;
	};

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
	CFWriteStreamRef fCFStream;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamerW_CFStream

class ZStreamerW_CFStream : public ZStreamerW
	{
public:
	ZStreamerW_CFStream(CFWriteStreamRef iCFStream);
	virtual ~ZStreamerW_CFStream();

// From ZStreamerW
	virtual const ZStreamW& GetStreamW();

protected:
	ZStreamW_CFStream fStream;
	};

#endif // ZCONFIG_SPI_Enabled(CoreFoundation)

// =================================================================================================
#pragma mark -
#pragma mark * ZStream_MacOSX

#if ZCONFIG_SPI_Enabled(CoreGraphics)

#include <CoreGraphics/CGDataProvider.h>
#include <CoreGraphics/CGDataConsumer.h>

namespace ZStream_MacOSX {

CGDataProviderRef sCGDataProviderCreate(ZRef<ZStreamerR> iStreamer);

CGDataProviderRef sCGDataProviderCreateRewind(ZRef<ZStreamerRPos> iStreamer);

CGDataConsumerRef sCGDataConsumerCreate(ZRef<ZStreamerW> iStreamer);

} // ZStream_MacOSX

#endif // ZCONFIG_SPI_Enabled(CoreGraphics)

#endif // __ZStream_MacOSX__
