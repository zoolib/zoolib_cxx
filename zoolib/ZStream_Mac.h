/* -------------------------------------------------------------------------------------------------
Copyright (c) 2000 Andrew Green and Learning in Motion, Inc.
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

#ifndef __ZStream_Mac__
#define __ZStream_Mac__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_SPI.h"

#if ZCONFIG_SPI_Enabled(Carbon64)

#include "zoolib/ZStreamer.h"

#include ZMACINCLUDE3(CoreServices,CarbonCore,MacMemory.h)

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamRPos_Mac_PartialResource

class ZStreamRPos_Mac_PartialResource : public ZStreamRPos
	{
public:
	ZStreamRPos_Mac_PartialResource(Handle inResourceHandle, bool inAdopt);
	~ZStreamRPos_Mac_PartialResource();

// From ZStreamR via ZStreamRPos
	virtual void Imp_Read(void* inDest, size_t inCount, size_t* outCountRead);
	virtual void Imp_Skip(uint64 inCount, uint64* outCountSkipped);

// From ZStreamRPos
	virtual uint64 Imp_GetPosition();
	virtual void Imp_SetPosition(uint64 inPosition);

	virtual uint64 Imp_GetSize();

private:
	Handle fResourceHandle;
	bool fAdopted;
	uint64 fPosition;
	size_t fSize;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamerRPos_Mac_PartialResource

class ZStreamerRPos_Mac_PartialResource : public ZStreamerRPos
	{
public:
	ZStreamerRPos_Mac_PartialResource(Handle inResourceHandle, bool inAdopt);
	virtual ~ZStreamerRPos_Mac_PartialResource();

// From ZStreamerRPos
	virtual const ZStreamRPos& GetStreamRPos();

protected:
	ZStreamRPos_Mac_PartialResource fStream;
	};

NAMESPACE_ZOOLIB_END

#endif // ZCONFIG_SPI_Enabled(Carbon64)

#endif // __ZStream_Mac__
