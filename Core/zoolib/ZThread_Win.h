// Copyright (c) 2008 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZThread_Win_h__
#define __ZThread_Win_h__ 1
#include "zconfig.h"

#include "zoolib/ZCONFIG_API.h"
#include "zoolib/ZCONFIG_SPI.h"

#ifndef ZCONFIG_API_Avail__Thread_Win
	#define ZCONFIG_API_Avail__Thread_Win ZCONFIG_SPI_Enabled(Win)
#endif

#ifndef ZCONFIG_API_Desired__Thread_Win
	#define ZCONFIG_API_Desired__Thread_Win 1
#endif

#if ZCONFIG_API_Enabled(Thread_Win)

#include "zoolib/ZCompat_NonCopyable.h"
#include "zoolib/ZThread_T.h"

// ZThread_Win.h gets pulled in by ZThread.h, which is referenced in quite a few places. We want
// to avoid pulling in the full Windows headers if possible, so we define dummy versions of HANDLE
// and CRITICAL_SECTION, and cast in the .cpp file.

typedef unsigned long DWORD;
typedef void* LPVOID;

namespace ZooLib {

class ZMtx_Win;
class ZSem_Win;

// =================================================================================================
#pragma mark - ZThread_Win

namespace ZThread_Win {

typedef unsigned ProcResult_t;
typedef void* ProcParam_t;

typedef ProcResult_t (__stdcall *ProcRaw_t)(ProcParam_t iParam);

typedef unsigned int ID;

void sStartRaw(size_t iStackSize, ProcRaw_t iProc, void* iParam);
ID sID();
void sSleep(double iDuration);

void sSetName(const char* iName);

struct Dummy_CRITICAL_SECTION
	{
	void* DebugInfo;
	long LockCount;
	long RecursionCount;
	void* OwningThread;
	void* LockSemaphore;
	void* SpinCount;
	};

typedef void* Dummy_HANDLE;

} // namespace ZThread_Win

// =================================================================================================
#pragma mark - ZTSS_Win

namespace ZTSS_Win {

typedef DWORD Key;
typedef LPVOID Value;

Key sCreate();
void sFree(Key iKey);

void sSet(Key iKey, Value iValue);
Value sGet(Key iKey);

} // namespace ZTSS_Win

// =================================================================================================
#pragma mark - ZMtx_Win

class ZMtx_Win : NonCopyable
	{
public:
	ZMtx_Win();
	~ZMtx_Win();

	void Acquire();
	void Release();

protected:
	ZThread_Win::Dummy_CRITICAL_SECTION fCRITICAL_SECTION;
	};

// =================================================================================================
#pragma mark - ZCnd_Win

typedef ZCndBase_T<ZMtx_Win, ZSem_Win> ZCnd_Win;

// =================================================================================================
#pragma mark - ZSem_Win

class ZSem_Win : NonCopyable
	{
public:
	ZSem_Win();
	~ZSem_Win();

	void Procure();
	bool TryProcureFor(double iTimeout);
	bool TryProcureUntil(double iDeadline);
	void Vacate();

protected:
	ZThread_Win::Dummy_HANDLE fHANDLE;
	};

} // namespace ZooLib

#endif // ZCONFIG_API_Enabled(Thread_Win)

#endif // __ZThread_Win_h__
