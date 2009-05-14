/* -------------------------------------------------------------------------------------------------
Copyright (c) 2008 Andrew Green
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

#include "zoolib/ZThreadImp_MacMP.h"

#if ZCONFIG_API_Enabled(ThreadImp_MacMP)

#include <new> // for bad_alloc

// For UpTime etc
#include ZMACINCLUDE3(CoreServices,CarbonCore,DriverServices.h)

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * ZTSS_MacMP

ZTSS_MacMP::Key ZTSS_MacMP::sCreate()
	{
	Key theKey;
	::MPAllocateTaskStorageIndex(&theKey);
	return theKey;
	}

void ZTSS_MacMP::sFree(Key iKey)
	{ ::MPDeallocateTaskStorageIndex(iKey); }

void ZTSS_MacMP::sSet(Key iKey, Value iValue)
	{ ::MPSetTaskStorageValue(iKey, iValue); }

ZTSS_MacMP::Value ZTSS_MacMP::sGet(Key iKey)
	{ return ::MPGetTaskStorageValue(iKey); }

// =================================================================================================
#pragma mark -
#pragma mark * ZSem_MacMP

ZSem_MacMP::ZSem_MacMP()
	{ ::MPCreateSemaphore(0xFFFFFFFFU, 0, &fMPSemaphoreID); }

ZSem_MacMP::~ZSem_MacMP()
	{ ::MPDeleteSemaphore(fMPSemaphoreID); }

void ZSem_MacMP::Wait()
	{ ::MPWaitOnSemaphore(fMPSemaphoreID, kDurationForever); }

bool ZSem_MacMP::Wait(double iTimeout)
	{
	if (iTimeout <= 0)
		{
		return noErr == ::MPWaitOnSemaphore(fMPSemaphoreID, kDurationImmediate);
		}
	else if (iTimeout < 1)
		{
		// If it's less than a second, do a microsecond wait.
		return noErr == ::MPWaitOnSemaphore(fMPSemaphoreID, iTimeout * -1e6);
		}
	else
		{
		// Otherwise do a millisecond wait.
		return noErr == ::MPWaitOnSemaphore(fMPSemaphoreID, iTimeout * 1e3);
		}
	}

void ZSem_MacMP::Signal()
	{ ::MPSignalSemaphore(fMPSemaphoreID); }

// =================================================================================================
#pragma mark -
#pragma mark * ZMtx_MacMP

ZMtx_MacMP::ZMtx_MacMP(const char* iName)
	{ ::MPCreateCriticalRegion(&fMPCriticalRegionID); }

ZMtx_MacMP::~ZMtx_MacMP()
	{ ::MPDeleteCriticalRegion(fMPCriticalRegionID); }

void ZMtx_MacMP::Acquire()
	{ ::MPEnterCriticalRegion(fMPCriticalRegionID, kDurationForever); }

void ZMtx_MacMP::Release()
	{ ::MPExitCriticalRegion(fMPCriticalRegionID); }

// =================================================================================================
#pragma mark -
#pragma mark * ZThreadImp_MacMP

ZThreadImp_MacMP::ID ZThreadImp_MacMP::sCreate(size_t iStackSize, Proc_t iProc, void* iParam)
	{
	if (iStackSize == 0)
		iStackSize = 1024 * 1024;

	ID theID;
	if (noErr != ::MPCreateTask(iProc, iParam, iStackSize, 0, nullptr, nullptr, 0, &theID))
		throw std::bad_alloc();
	return theID;
	}

ZThreadImp_MacMP::ID ZThreadImp_MacMP::sID()
	{ return ::MPCurrentTaskID(); }

void ZThreadImp_MacMP::sSleep(double iDuration)
	{
	const Nanoseconds nowU = ::AbsoluteToNanoseconds(::UpTime());
	const double targetDouble
		= double(*reinterpret_cast<const uint64*>(&nowU)) / 1e9 + iDuration;
	const uint64 targetU = uint64(targetDouble * 1e9);
	AbsoluteTime targetA
		= ::NanosecondsToAbsolute(*reinterpret_cast<const Nanoseconds*>(&targetU));
	::MPDelayUntil(&targetA);
	}

NAMESPACE_ZOOLIB_END

#endif // ZCONFIG_API_Enabled(ThreadImp_MacMP)
