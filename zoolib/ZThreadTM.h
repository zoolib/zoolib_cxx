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

#ifndef __ZThreadTM__
#define __ZThreadTM__ 1
#include "zconfig.h"

// =================================================================================================
#if ZCONFIG(OS, MacOS7) || ZCONFIG(OS, Carbon) || ZCONFIG(OS, MacOSX)

// ==================================================
// This tiny subset of the API is exposed to C++ _and_ C so that OTMP can call it.
// ZThreadTM_State is exposed only so that the very rare cases where clients need
// to enable a fiber can do so efficiently, without us having to map an ID to a struct.
struct ZThreadTM_State;

// Suspend/resume of a fiber is provided to allow for the implementation of blocking calls
// to the OS using completion procs. DO NOT MISUSE.
#ifdef __cplusplus
extern "C" {
#endif

struct ZThreadTM_State* ZThreadTM_Current();
void ZThreadTM_Suspend();
void ZThreadTM_Resume(struct ZThreadTM_State* iThread);

#ifdef __cplusplus
} // extern "C"
#endif

// ==================================================
#ifdef __cplusplus

#include "ZTypes.h"

#if ZCONFIG(OS, MacOS7)
#	include <Events.h> // For EventRecord
#endif

#if ZCONFIG(OS, Carbon)
#	include <CarbonEvents.h>
#endif

#if ZCONFIG(OS, MacOSX)
#	include <HIToolbox/CarbonEvents.h>
#endif

#include <cstddef> // For size_t

#if ZCONFIG(OS, MacOSX)
#	include <CarbonCore/Devices.h>
#	include <CarbonCore/Threads.h>
#else
#	include <Devices.h> // For IOParam
#	include <Threads.h> // For ThreadID
#endif

// ==================================================

typedef int32 ZThreadTM_Error;
enum { ZThreadTM_ErrorNone, ZThreadTM_ErrorDisposed, ZThreadTM_ErrorTimeout };

// Set up and tear down of the fiber infrastructure -- these are called at
// static construction time by ZThread::InitHelper__.
void ZThreadTM_Initialize();
void ZThreadTM_Exit();

// Prototype of a fiber's entry proc. Declared as returning void*, although that
// return value is not not accessible in any fashion.
typedef void (ZThreadTM_EntryProc)(void* iArgument);

// Create a new thread
bool ZThreadTM_NewThread(ZThreadTM_EntryProc* iProc, void* iArgument, ThreadID& oID, ZThreadTM_State*& oState);

// To allow a process to be a good cooperative citizen, and to be responsive to user events,
// but also to ensure fibers use as much CPU time as is really available we provide a wrapper
// around WaitNextEvent that tunes the parameters to that method based on what's happening in the process.
void ZThreadTM_WaitNextEvent(bool iAvoidWait, EventRecord& oEventRecord, RgnHandle iCursorRgn);

#if ZCONFIG(OS, Carbon) || ZCONFIG(OS, MacOSX)
OSStatus ZThreadTM_ReceiveNextEvent(bool inAvoidWait, uint32 inNumTypes, const EventTypeSpec* inList, EventRef* outEvent);
#endif

// Just what you'd expect.
void ZThreadTM_Sleep(bigtime_t iMicroseconds);
// Although yielding is frowned upon, as it's most commonly used when code is polling in
// some fashion, there are still some limited  circumstances where it's the right thing to do.
void ZThreadTM_Yield();

// ZThreadTM doesn't preemptively time slice, it will yield periodically when semaphores are accessed or
// when doing asynchronous IO. If you have a tight loop that uses no synchronization primitives and
// does no IO, calling ZThreadTM_YieldIfTimeSliceExpired will allow other fibers a chance to run.
void ZThreadTM_YieldIfTimeSliceExpired();

// These methods return the current time in outRealTime, and the cumulative time in outCumulativeRunTime.
void ZThreadTM_GetProcessTimes(bigtime_t& oRealTime, bigtime_t& oCumulativeRunTime);
void ZThreadTM_GetThreadTimes(ThreadID iID, bigtime_t& oRealTime, bigtime_t& oCumulativeRunTime);

// These methods are used internally by ZThreadTM_WaitNextEvent, and are exposed in case you
// need to implement a replacement for ZThreadTM_WaitNextEvent.
int32 ZThreadTM_ReadyCount();
int32 ZThreadTM_ReadyCountNormal();

// ZThreadTM_SetCurrentAllowTimeSlice disables yielding of the current fiber due to a time
// slice having expired -- it's useful when working with toolbox entities that are managed
// on a process-wide basis, like the current GrafPort, and which must thus be saved and restored on
// context switches. This doesn't prevent switching to other fibers, but does reduce the
// frequency with which it will happen unexpectedly.
bool ZThreadTM_SetCurrentAllowTimeSlice(bool iAllowTimeSlice);

// ZThreadTM_SetCurrentSolo is not for general use -- it ensures that the current fiber is
// the only one to execute, necessary when dumping stack crawls to disk at deferred task time.
bool ZThreadTM_SetCurrentSolo(bool inSolo);

// ==================================================
// Thread Local Storage (TLS), aka Thread Specific Data (TSD)
#ifndef ZCONFIG_ThreadTM_TLSMaxKeys
#	define ZCONFIG_ThreadTM_TLSMaxKeys 8
#endif

typedef struct ZThreadTM_TLSKey_t_Opqaue* ZThreadTM_TLSKey_t;
typedef struct ZThreadTM_TLSData_t_Opqaue* ZThreadTM_TLSData_t;

ZThreadTM_TLSKey_t ZThreadTM_TLSAllocate();
void ZThreadTM_TLSFree(ZThreadTM_TLSKey_t iKey);
ZThreadTM_TLSData_t ZThreadTM_TLSGet(ZThreadTM_TLSKey_t iKey);
void ZThreadTM_TLSSet(ZThreadTM_TLSKey_t iKey, ZThreadTM_TLSData_t iData);

// ==================================================
// Asynchronous I/O support
struct ZThreadTM_PBHeader
	{
	ZThreadTM_State* fThread;
	};

struct ZThreadTM_IOParamBlock : public ZThreadTM_PBHeader
	{
	IOParam fIOParam;
	};

void ZThreadTM_SetupForAsyncCompletion(ZThreadTM_PBHeader* iParamBlock);
OSErr ZThreadTM_WaitForAsyncCompletion(ZThreadTM_PBHeader* iParamBlock, OSErr iErr);
void ZThreadTM_SetupForAsyncCompletionMulti(ZThreadTM_PBHeader* iParamBlock);
void ZThreadTM_WaitForAsyncCompletionMulti(ZThreadTM_PBHeader** iParamBlocks, size_t iCount);

#if !ZCONFIG(OS, Carbon) && !ZCONFIG(OS, MacOSX)
// PBControl is not supported under Carbon.
OSErr ZThreadTM_PBControlAndBlock(ZThreadTM_PBHeader* iParamBlock);
#endif

// ==================================================
// Switching callbacks -- A fiber can install as many as it needs, each installed switch
// proc will be called with the address of the SwitchInfo it passed in when the fiber
// ceases execution, and again when it resumes execution. Switch callbacks are called
// in reverse order of installation when switching out, and in order of installation when
// switching in. They are called with the critical section held, so you are *severely*
// constrained in what you are allowed to do -- no use of semaphores for example.
struct ZThreadTM_SwitchInfo;
typedef void (ZThreadTM_SwitchProc)(bool iSwitchingIn, ZThreadTM_SwitchInfo* iSwitchInfo);
struct ZThreadTM_SwitchInfo
	{
	ZThreadTM_SwitchProc* fSwitchProc;
	ZThreadTM_SwitchInfo* fPrev;
	ZThreadTM_SwitchInfo* fNext;
	};

void ZThreadTM_InstallSwitchCallback(ZThreadTM_SwitchInfo* iSwitchInfo);
void ZThreadTM_RemoveSwitchCallback(ZThreadTM_SwitchInfo* iSwitchInfo);

// ==================================================
// Semaphore

struct ZThreadTM_Waiter;
struct ZThreadTM_Sem
	{
	ZThreadTM_Waiter* fWaiter_Head;
	ZThreadTM_Waiter* fWaiter_Tail;
	int32 fAvailable;
	};

void ZThreadTM_SemInit(ZThreadTM_Sem* iSem, int32 iInitialCount);
void ZThreadTM_SemDestroy(ZThreadTM_Sem* iSem);
ZThreadTM_Error ZThreadTM_SemWait(ZThreadTM_Sem* iSem, int32 iCount);
ZThreadTM_Error ZThreadTM_SemWait(ZThreadTM_Sem* iSem, int32 iCount, bigtime_t iMicroseconds);
void ZThreadTM_SemSignal(ZThreadTM_Sem* iSem, int32 iCount);

#endif // __cplusplus

// ==================================================
// Profiling support functions that do that right thing in a preemptive environment

bool ZThreadTM_ProfilerSetStatus(bool iEnabled);
bool ZThreadTM_ProfilerGetStatus();
void ZThreadTM_ProfilerDump(const char* iFilename);
void ZThreadTM_ProfilerClear();

#endif // ZCONFIG(OS, MacOS7) || ZCONFIG(OS, Carbon) || ZCONFIG(OS, MacOSX)
// =================================================================================================

#endif // __ZThreadTM__
