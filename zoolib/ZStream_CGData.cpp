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

#include "zoolib/ZStream_CGData.h"

#if ZCONFIG_SPI_Enabled(CoreGraphics)

#include "zoolib/ZDebug.h"

namespace ZooLib {

// =================================================================================================
// MARK: - ZStream_CGData::sProvider

static size_t spGetBytes_R(void* iInfo, void* oBuffer, size_t iCount)
	{
	size_t countRead;
	static_cast<ZStreamerR*>(iInfo)->GetStreamR().Read(oBuffer, iCount, &countRead);
	return countRead;
	}

static void spReleaseProvider_R(void* iInfo)
	{ static_cast<ZStreamerR*>(iInfo)->Release(); }

#if ZMACRO_IOS || MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_5

static off_t spSkipForward_R(void* iInfo, off_t iCount)
	{
	uint64 countSkipped;
	static_cast<ZStreamerR*>(iInfo)->GetStreamR().Skip(iCount, &countSkipped);
	return countSkipped;
	}

static CGDataProviderSequentialCallbacks spCallbacksSequential =
	{
	0,
	spGetBytes_R,
	spSkipForward_R,
	0,//spUnimplemented,
	spReleaseProvider_R
	};

ZRef<CGDataProviderRef> ZStream_CGData::sProvider(ZRef<ZStreamerR> iStreamer)
	{
	if (iStreamer)
		{
		iStreamer->Retain();
		return sAdopt& ::CGDataProviderCreateSequential(iStreamer.Get(), &spCallbacksSequential);
		}
	return null;
	}

#else // ZMACRO_IOS || MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_5

static void spSkipBytes_R(void* iInfo, size_t iCount)
	{ static_cast<ZStreamerR*>(iInfo)->GetStreamR().Skip(iCount); }

static CGDataProviderCallbacks spCallbacks_R =
	{
	spGetBytes_R,
	spSkipBytes_R,
	0,//spUnimplemented,
	spReleaseProvider_R
	};

ZRef<CGDataProviderRef> ZStream_CGData::sProvider(ZRef<ZStreamerR> iStreamer)
	{
	if (iStreamer)
		{
		iStreamer->Retain();
		return sAdopt& ::CGDataProviderCreate(iStreamer.Get(), &spCallbacks_R);
		}
	return null;
	}

#endif // ZMACRO_IOS || MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_5

// =================================================================================================
// MARK: - ZStream_CGData::sProvider

static size_t spGetBytes_RPos(void* iInfo, void* oBuffer, size_t iCount)
	{
	size_t countRead;
	static_cast<ZStreamerRPos*>(iInfo)->GetStreamR().Read(oBuffer, iCount, &countRead);
	return countRead;
	}

static void spRewind_RPos(void* iInfo)
	{ static_cast<ZStreamerRPos*>(iInfo)->GetStreamRPos().SetPosition(0); }

static void spReleaseProvider_RPos(void* iInfo)
	{ static_cast<ZStreamerRPos*>(iInfo)->Release(); }

#if ZMACRO_IOS || MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_5

static off_t spSkipForward_RPos(void* iInfo, off_t iCount)
	{
	uint64 countSkipped;
	static_cast<ZStreamerR*>(iInfo)->GetStreamR().Skip(iCount, &countSkipped);
	return countSkipped;
	}

static CGDataProviderSequentialCallbacks spCallbacksSequential_RPos =
	{
	0,
	spGetBytes_RPos,
	spSkipForward_RPos,
	spRewind_RPos,
	spReleaseProvider_RPos
	};

ZRef<CGDataProviderRef> ZStream_CGData::sProvider(ZRef<ZStreamerRPos> iStreamer)
	{
	if (iStreamer)
		{
		iStreamer->Retain();
		return sAdopt& ::CGDataProviderCreateSequential(iStreamer.Get(), &spCallbacksSequential_RPos);
		}
	return null;
	}

#else // ZMACRO_IOS || MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_5

static void spSkipBytes_RPos(void* iInfo, size_t iCount)
	{ static_cast<ZStreamerRPos*>(iInfo)->GetStreamR().Skip(iCount); }

static CGDataProviderCallbacks spCallbacks_RPos =
	{
	spGetBytes_RPos,
	spSkipBytes_RPos,
	spRewind_RPos,
	spReleaseProvider_RPos
	};

ZRef<CGDataProviderRef> ZStream_CGData::sProvider(ZRef<ZStreamerRPos> iStreamer)
	{
	if (iStreamer)
		{
		iStreamer->Retain();
		return sAdopt& ::CGDataProviderCreate(iStreamer.Get(), &spCallbacks_RPos);
		}
	return null;
	}

#endif // ZMACRO_IOS || MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_5

// =================================================================================================
// MARK: - CGDataConsumerRef, ZStreamerW

namespace { // anonymous

size_t spPutBytes_W(void* iInfo, const void* iBuffer, size_t iCount)
	{
	size_t countWritten;
	static_cast<ZRef<ZStreamerW>*>(iInfo)[0]->GetStreamW().Write(iBuffer, iCount, &countWritten);
	return countWritten;
	}

void spReleaseConsumer_W(void* iInfo)
	{ static_cast<ZStreamerW*>(iInfo)->Release(); }

CGDataConsumerCallbacks spCallbacks_W =
	{
	spPutBytes_W,
	spReleaseConsumer_W
	};

} // anonymous namespace

ZRef<CGDataConsumerRef> ZStream_CGData::sConsumer(ZRef<ZStreamerW> iStreamer)
	{
	if (iStreamer)
		{
		iStreamer->Retain();
		return sAdopt& ::CGDataConsumerCreate(iStreamer.Get(), &spCallbacks_W);
		}
	return null;
	}

} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(CoreGraphics)
