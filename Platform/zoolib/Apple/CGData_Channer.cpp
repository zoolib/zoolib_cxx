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

#include "zoolib/Apple/CGData_Channer.h"

#if ZCONFIG_SPI_Enabled(CoreGraphics)

#include "zoolib/ZDebug.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - CGData_Channer::sProvider

static size_t spGetBytes_R(void* iInfo, void* oBuffer, size_t iCount)
	{ return sReadMem(*static_cast<ChannerR_Bin*>(iInfo), oBuffer, iCount); }

static void spReleaseProvider_R(void* iInfo)
	{ static_cast<ChannerR_Bin*>(iInfo)->Release(); }

#if ZMACRO_IOS || MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_5

static off_t spSkipForward_R(void* iInfo, off_t iCount)
	{ return sSkip(*static_cast<ChannerR_Bin*>(iInfo), iCount); }

static CGDataProviderSequentialCallbacks spCallbacksSequential =
	{
	0,
	spGetBytes_R,
	spSkipForward_R,
	0,//spUnimplemented,
	spReleaseProvider_R
	};

ZP<CGDataProviderRef> CGData_Channer::sProvider(ZP<ChannerR_Bin> iChanner)
	{
	if (iChanner)
		{
		iChanner->Retain();
		return sAdopt& ::CGDataProviderCreateSequential(iChanner.Get(), &spCallbacksSequential);
		}
	return null;
	}

#else // ZMACRO_IOS || MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_5

static void spSkipBytes_R(void* iInfo, size_t iCount)
	{ static_cast<ChannerR_Bin*>(iInfo)->GetStreamR().Skip(iCount); }

static CGDataProviderCallbacks spCallbacks_R =
	{
	spGetBytes_R,
	spSkipBytes_R,
	0,//spUnimplemented,
	spReleaseProvider_R
	};

ZP<CGDataProviderRef> CGData_Channer::sProvider(ZP<ChannerR_Bin> iChanner)
	{
	if (iChanner)
		{
		iChanner->Retain();
		return sAdopt& ::CGDataProviderCreate(iChanner.Get(), &spCallbacks_R);
		}
	return null;
	}

#endif // ZMACRO_IOS || MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_5

// =================================================================================================
#pragma mark - CGData_Channer::sProvider

static size_t spGetBytes_RPos(void* iInfo, void* oBuffer, size_t iCount)
	{ return sReadMem(*static_cast<ChannerRPos_Bin*>(iInfo), oBuffer, iCount); }

static void spRewind_RPos(void* iInfo)
	{ sPosSet(*static_cast<ChannerRPos_Bin*>(iInfo), 0); }

static void spReleaseProvider_RPos(void* iInfo)
	{ static_cast<ChannerRPos_Bin*>(iInfo)->Release(); }

#if ZMACRO_IOS || MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_5

static off_t spSkipForward_RPos(void* iInfo, off_t iCount)
	{ return sSkip(*static_cast<ChannerRPos_Bin*>(iInfo), iCount); }

static CGDataProviderSequentialCallbacks spCallbacksSequential_RPos =
	{
	0,
	spGetBytes_RPos,
	spSkipForward_RPos,
	spRewind_RPos,
	spReleaseProvider_RPos
	};

ZP<CGDataProviderRef> CGData_Channer::sProvider(ZP<ChannerRPos_Bin> iChanner)
	{
	if (iChanner)
		{
		iChanner->Retain();
		return sAdopt& ::CGDataProviderCreateSequential(iChanner.Get(), &spCallbacksSequential_RPos);
		}
	return null;
	}

#else // ZMACRO_IOS || MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_5

static void spSkipBytes_RPos(void* iInfo, size_t iCount)
	{ static_cast<ChannerRPos_Bin*>(iInfo)->GetStreamR().Skip(iCount); }

static CGDataProviderCallbacks spCallbacks_RPos =
	{
	spGetBytes_RPos,
	spSkipBytes_RPos,
	spRewind_RPos,
	spReleaseProvider_RPos
	};

ZP<CGDataProviderRef> CGData_Channer::sProvider(ZP<ChannerRPos_Bin> iChanner)
	{
	if (iChanner)
		{
		iChanner->Retain();
		return sAdopt& ::CGDataProviderCreate(iChanner.Get(), &spCallbacks_RPos);
		}
	return null;
	}

#endif // ZMACRO_IOS || MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_5

// =================================================================================================
#pragma mark - CGDataConsumerRef, ChannerW_Bin

namespace { // anonymous

size_t spPutBytes_W(void* iInfo, const void* iBuffer, size_t iCount)
	{ return sWriteMem(*static_cast<ZP<ChannerW_Bin>*>(iInfo)[0], iBuffer, iCount); }

void spReleaseConsumer_W(void* iInfo)
	{ static_cast<ChannerW_Bin*>(iInfo)->Release(); }

CGDataConsumerCallbacks spCallbacks_W =
	{
	spPutBytes_W,
	spReleaseConsumer_W
	};

} // anonymous namespace

ZP<CGDataConsumerRef> CGData_Channer::sConsumer(ZP<ChannerW_Bin> iChanner)
	{
	if (iChanner)
		{
		iChanner->Retain();
		return sAdopt& ::CGDataConsumerCreate(iChanner.Get(), &spCallbacks_W);
		}
	return null;
	}

} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(CoreGraphics)
