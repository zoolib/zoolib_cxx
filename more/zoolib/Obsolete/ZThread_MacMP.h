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

#ifndef __ZThread_MacMP_h__
#define __ZThread_MacMP_h__ 1
#include "zconfig.h"

#include "zoolib/ZCONFIG_API.h"
#include "zoolib/ZCONFIG_SPI.h"

#ifndef ZCONFIG_API_Avail__Thread_MacMP
	#define ZCONFIG_API_Avail__Thread_MacMP ZCONFIG_SPI_Enabled(MacMP)
#endif

#ifndef ZCONFIG_API_Desired__Thread_MacMP
	#define ZCONFIG_API_Desired__Thread_MacMP 1
#endif

#if ZCONFIG_API_Enabled(Thread_MacMP)

#include "zoolib/ZCompat_NonCopyable.h"
#include "zoolib/ZThread_T.h"

#include ZMACINCLUDE3(CoreServices,CarbonCore,Multiprocessing.h)

namespace ZooLib {

class ZMtx_MacMP;
class ZSem_MacMP;

// =================================================================================================
// MARK: - ZThread_MacMP

namespace ZThread_MacMP {

typedef OSStatus ProcResult_t;
typedef void* ProcParam_t;

typedef TaskProc ProcRaw_t;

typedef MPTaskID ID;

void sCreateRaw(size_t iStackSize, ProcRaw_t iProc, void* iParam);
ID sID();
void sSleep(double iDuration);

} // namespace ZThread_MacMP

// =================================================================================================
// MARK: - ZTSS_MacMP

namespace ZTSS_MacMP {

typedef TaskStorageIndex Key;
typedef TaskStorageValue Value;

Key sCreate();
void sFree(Key iKey);

void sSet(Key iKey, Value iValue);
Value sGet(Key iKey);

} // namespace ZTSS_MacMP

// =================================================================================================
// MARK: - ZMtx_MacMP

class ZMtx_MacMP : NonCopyable
	{
public:
	ZMtx_MacMP();
	~ZMtx_MacMP();

	void Acquire();
	void Release();

protected:
	MPCriticalRegionID fMPCriticalRegionID;
	MPTaskID fOwner;
	};

// =================================================================================================
// MARK: - ZCndBase_MacMP

typedef ZCndBase_T<ZMtx_MacMP, ZSem_MacMP> ZCndBase_MacMP;

// =================================================================================================
// MARK: - ZMtxR_MacMP

typedef ZMtxR_T<ZMtx_MacMP, ZCndBase_MacMP, ZThread_MacMP::ID, ZThread_MacMP::sID> ZMtxR_MacMP;

// =================================================================================================
// MARK: - ZCnd_MacMP

typedef ZCndR_T<ZMtxR_MacMP, ZCndBase_MacMP> ZCnd_MacMP;

// =================================================================================================
// MARK: - ZSem_MacMP

class ZSem_MacMP : NonCopyable
	{
public:
	ZSem_MacMP();
	~ZSem_MacMP();

	void Procure();
	bool TryProcureFor(double iTimeout);
	bool TryProcureUntil(ZTime iDeadline);
	void Vacate();

protected:
	MPSemaphoreID fMPSemaphoreID;
	};

} // namespace ZooLib

#endif // ZCONFIG_API_Enabled(Thread_MacMP)

#endif // __ZThread_MacMP_h__
