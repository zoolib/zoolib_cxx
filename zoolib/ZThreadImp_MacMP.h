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

#ifndef __ZThreadImp_MacMP__
#define __ZThreadImp_MacMP__ 1
#include "zconfig.h"

#include "zoolib/ZCONFIG_API.h"
#include "zoolib/ZCONFIG_SPI.h"

#ifndef ZCONFIG_API_Avail__ThreadImp_MacMP
#	define ZCONFIG_API_Avail__ThreadImp_MacMP ZCONFIG_SPI_Enabled(MacMP)
#endif

#ifndef ZCONFIG_API_Desired__ThreadImp_MacMP
#	define ZCONFIG_API_Desired__ThreadImp_MacMP 1
#endif

#if ZCONFIG_API_Enabled(ThreadImp_MacMP)

#include "zoolib/ZCompat_NonCopyable.h"
#include "zoolib/ZThreadImp_T.h"

#if __MACH__
// This is tricky. We need Multiprocessing.h, but don't want to pull in other
// stuff like OpenTransport, which has conflicts with tcp.h.
#	if __MWERKS__
#		include <CarbonCore/CarbonCore.h>
#	else
#		include <CoreServices/CoreServices.h>
#	endif
#else
#	include <Multiprocessing.h>
#endif

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * ZTSS_MacMP

namespace ZTSS_MacMP {

typedef TaskStorageIndex Key;
typedef TaskStorageValue Value;

Key sCreate();
void sFree(Key iKey);

void sSet(Key iKey, Value iValue);
Value sGet(Key iKey);

} // namespace ZTSS_MacMP

// =================================================================================================
#pragma mark -
#pragma mark * ZSem_MacMP

class ZSem_MacMP : NonCopyable
	{
public:
	ZSem_MacMP();

	~ZSem_MacMP();

	void Wait();
	bool Wait(double iTimeout);
	void Signal();

protected:
	MPSemaphoreID fMPSemaphoreID;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZMtx_MacMP

class ZMtx_MacMP : NonCopyable
	{
public:
	ZMtx_MacMP(const char* iName = nil);
	~ZMtx_MacMP();

	void Acquire();
	void Release();

protected:
	MPCriticalRegionID fMPCriticalRegionID;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZCnd_MacMP

class ZCnd_MacMP : ZCnd_T<ZMtx_MacMP, ZSem_MacMP>
	{
public:
	ZCnd_MacMP();

	~ZCnd_MacMP();

	void Wait(ZMtx_MacMP& iMtx);
	void Wait(ZMtx_MacMP& iMtx, double iTimeout);
	void Signal();
	void Broadcast();
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZThreadImp_MacMP

namespace ZThreadImp_MacMP {

typedef OSStatus ProcResult_t;
typedef void* ProcParam_t;

typedef TaskProc Proc_t;

typedef MPTaskID ID;

ID sCreate(size_t iStackSize, Proc_t iProc, void* iParam);
ID sID();
void sSleep(double iDuration);

} // namespace ZThreadImp_MacMP

} // namespace ZooLib


#endif // ZCONFIG_API_Enabled(ThreadImp_MacMP)

#endif // __ZThreadImp_MacMP__
