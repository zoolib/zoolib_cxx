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

#ifndef __ZThread_Win__
#define __ZThread_Win__ 1
#include "zconfig.h"

#include "zoolib/ZCONFIG_API.h"
#include "zoolib/ZCONFIG_SPI.h"

#ifndef ZCONFIG_API_Avail__Thread_Win
#	define ZCONFIG_API_Avail__Thread_Win ZCONFIG_SPI_Enabled(Win)
#endif

#ifndef ZCONFIG_API_Desired__Thread_Win
#	define ZCONFIG_API_Desired__Thread_Win 1
#endif

#if ZCONFIG_API_Enabled(Thread_Win)

#include "zoolib/ZCompat_NonCopyable.h"
#include "zoolib/ZThread_T.h"

// ZThread_Win.h gets pulled in by ZThread.h, which is referenced in quite
// a few places. We want to avoid pulling in the full Windows headers if
// possible, so in ZMtx_Win and ZSem_Win we define dummy versions of
// HANDLE and CRITICAL_SECTION, and cast in the .cpp file.

typedef unsigned long DWORD;
typedef void* LPVOID;

NAMESPACE_ZOOLIB_BEGIN

class ZMtx_Win;
class ZSem_Win;

// =================================================================================================
#pragma mark -
#pragma mark * ZTSS_Win

namespace ZTSS_Win {

typedef DWORD Key;
typedef LPVOID Value;

Key sCreate();
void sFree(Key iKey);

void sSet(Key iKey, Value iValue);
Value sGet(Key iKey);

} // namespace ZTSS_Win

// =================================================================================================
#pragma mark -
#pragma mark * ZCnd_Win

typedef ZCnd_T<ZMtx_Win, ZSem_Win> ZCnd_Win;

// =================================================================================================
#pragma mark -
#pragma mark * ZMtx_Win

class ZMtx_Win : NonCopyable
	{
public:
	ZMtx_Win(const char* iName = nullptr);
	~ZMtx_Win();

	void Acquire();
	void Release();

protected:
	struct Dummy_CRITICAL_SECTION
		{
		void* DebugInfo;
		long LockCount;
	    long RecursionCount;
	    void* OwningThread;
	    void* LockSemaphore;
	    void* SpinCount;
		};

	Dummy_CRITICAL_SECTION fCRITICAL_SECTION;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZSem_Win

class ZSem_Win : NonCopyable
	{
public:
	ZSem_Win();

	~ZSem_Win();

	void Wait();
	bool WaitFor(double iTimeout);
	bool WaitUntil(ZTime iDeadline);
	void Signal();

protected:
	typedef void* Dummy_HANDLE;
	Dummy_HANDLE fHANDLE;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZThread_Win

namespace ZThread_Win {

typedef unsigned ProcResult_t;
typedef void* ProcParam_t;

typedef ProcResult_t (__stdcall *ProcRaw_t)(ProcParam_t iParam);

typedef unsigned int ID;

void sCreateRaw(size_t iStackSize, ProcRaw_t iProc, void* iParam);
ID sID();
void sSleep(double iDuration);

} // namespace ZThread_Win

NAMESPACE_ZOOLIB_END

#endif // ZCONFIG_API_Enabled(Thread_Win)

#endif // __ZThread_Win__
