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

#include "zoolib/ZThread_MacMP.h"

#if ZCONFIG_API_Enabled(Thread_MacMP)

#include "zoolib/ZCompat_algorithm.h" // For min

#include <new> // for bad_alloc

// For UpTime etc
#include ZMACINCLUDE3(CoreServices,CarbonCore,DriverServices.h)

namespace ZooLib {

// =================================================================================================
// MARK: - ZThread_MacMP

namespace ZThread_MacMP {

void sCreateRaw(size_t iStackSize, ProcRaw_t iProc, void* iParam)
	{
	if (iStackSize == 0)
		iStackSize = 1024 * 1024;

	ID theID;
	if (noErr != ::MPCreateTask(iProc, iParam, iStackSize, 0, nullptr, nullptr, 0, &theID))
		throw std::bad_alloc();
	}

ID sID()
	{ return ::MPCurrentTaskID(); }

void sSleep(double iDuration)
	{
	const Nanoseconds nowU = ::AbsoluteToNanoseconds(::UpTime());
	const double targetDouble =
		double(*reinterpret_cast<const uint64*>(&nowU)) / 1e9 + iDuration;
	const uint64 targetU = uint64(targetDouble * 1e9);
	AbsoluteTime targetA =
		::NanosecondsToAbsolute(*reinterpret_cast<const Nanoseconds*>(&targetU));
	::MPDelayUntil(&targetA);
	}

} // namespace ZThread_MacMP

// =================================================================================================
// MARK: - ZTSS_MacMP

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
// MARK: - ZMtx_MacMP

/*
MPCriticalRegionID is recursive, but we want to ensure that ZMtx_MacMP
asserts or deadlocks when recursive acquisition is attempted so that we
match the behavior of other non-recursive mutexes.
*/

ZMtx_MacMP::ZMtx_MacMP()
	{ ::MPCreateCriticalRegion(&fMPCriticalRegionID); }

ZMtx_MacMP::~ZMtx_MacMP()
	{ ::MPDeleteCriticalRegion(fMPCriticalRegionID); }

void ZMtx_MacMP::Acquire()
	{
	ZAssert(fOwner != ZThread_MacMP::sID());
	::MPEnterCriticalRegion(fMPCriticalRegionID, kDurationForever);
	fOwner = ZThread_MacMP::sID();
	}

void ZMtx_MacMP::Release()
	{
	ZAssert(fOwner == ZThread_MacMP::sID());
	fOwner = 0;
	::MPExitCriticalRegion(fMPCriticalRegionID);
	}

// =================================================================================================
// MARK: - ZSem_MacMP

ZSem_MacMP::ZSem_MacMP()
	{ ::MPCreateSemaphore(0xFFFFFFFFU, 0, &fMPSemaphoreID); }

ZSem_MacMP::~ZSem_MacMP()
	{ ::MPDeleteSemaphore(fMPSemaphoreID); }

void ZSem_MacMP::Procure()
	{ ::MPWaitOnSemaphore(fMPSemaphoreID, kDurationForever); }

bool ZSem_MacMP::TryProcureFor(double iTimeout)
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
		// Otherwise do a millisecond wait, staying well short of the SInt32 limit.
		return noErr == ::MPWaitOnSemaphore(fMPSemaphoreID, std::min(1e9, iTimeout * 1e3));
		}
	}

bool ZSem_MacMP::TryProcureUntil(ZTime iDeadline)
	{ return this->TryProcureFor(iDeadline - ZTime::sSystem()); }

void ZSem_MacMP::Vacate()
	{ ::MPSignalSemaphore(fMPSemaphoreID); }

} // namespace ZooLib

#endif // ZCONFIG_API_Enabled(Thread_MacMP)
