/* ------------------------------------------------------------
Copyright (c) 2000 Andrew Green and Learning in Motion, Inc.
http://www.zoolib.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
COPYRIGHT HOLDER(S) BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
------------------------------------------------------------ */

#include "ZThreadTM.h"

#if ZCONFIG(OS, MacOS7) || ZCONFIG(OS, Carbon) // || ZCONFIG(OS, MacOSX)

// ==================================================
#pragma mark -
#pragma mark * kDebug

#define kDebug_ThreadTM 2

// ==================================================
#pragma mark -
#pragma mark * Profiling

#if __profile__
#	include <Profiler.h>
#	define ZCONFIG_ThreadTM_Profile 1
#else
#	define ZCONFIG_ThreadTM_Profile 0
#endif

// ==================================================
#pragma mark -
#pragma mark * Includes, type definitions and static variables

#if ZCONFIG(OS, MacOSX)
#	include <CarbonCore/DriverServices.h>
#	include <CarbonCore/LowMem.h>
#	include <CarbonCore/OSUtils.h>
#	include <HIServices/Processes.h>
#	include <CarbonCore/Timer.h>
#else
#	include <DriverServices.h> // For UpTime and AbsoluteToNanoseconds
#	include <LowMem.h> // For all the low memory accessors
#	include <OSUtils.h> // For SetA5
#	include <Processes.h> // For ProcessSerialNumber
#	include <Timer.h> // For timer stuff
#endif

#include <climits> // For LONG_LONG_MAX
#ifndef LONG_LONG_MAX // Note that not everyone defines LONG_LONG_MAX
#	define LONG_LONG_MAX LLONG_MAX
#endif

#include <new> // For bad_alloc

#include "ZAtomic.h"
#include "ZDebug_Mac.h"

#if ZCONFIG(Compiler, CodeWarrior) && ZCONFIG(Processor, 68K)
	// As we rely on sSavedA5 being addressed with an absolute 32 bit
	// address, ensure far_data is active.
	ZAssertCompile(__option(far_data));
#endif

static inline bigtime_t sNow()
	{
	Nanoseconds temp = AbsoluteToNanoseconds(UpTime());
	return *reinterpret_cast<bigtime_t*>(&temp) / 1000;
	}

// ==================================================

struct ZThreadTM_State
	{
	// Link to next and previous allocated threads.
	ZThreadTM_State* fThread_Previous;
	ZThreadTM_State* fThread_Next;

	// Address of procedure at head of our call chain.
	ZThreadTM_EntryProc* fEntryProc;

	// Argument to be passed to our EntryProc.
	void* fArgument;

	// ID of this thread
	ThreadID fID;

	// Is the ThreadTM eligible for scheduling. This is a thread safe variable
	// to allow us to keep sReadyCount correct.
	ZAtomic_t fEnabled;

	// If fSuspendCount > 0 then the thread is disabled.
	ZAtomic_t fSuspendCount;

	// Time the ThreadTM was last yielded to.
	bigtime_t fStartTime;

	// Cumulative run time for this ThreadTM.
	bigtime_t fCumulativeTime;

	// Should this ThreadTM yield to others when its time slice expires.
	bool fAllowTimeSlice;

	// Flag for managing multiple completions.
	bool fInMultiComplete;

	// Thread-specific storage.
	ZThreadTM_TLSData_t fTLSData[ZCONFIG_ThreadTM_TLSMaxKeys];

	// Switch callbacks
	ZThreadTM_SwitchInfo* fSwitchInfo_Head;
	ZThreadTM_SwitchInfo* fSwitchInfo_Tail;

	// Finally, support for profiling.
	#if ZCONFIG_ThreadTM_Profile
		ProfilerThreadRef fProfilerThreadRef;
	#endif
	};

// =================================================================================================

static bool sTLSDataAllocated[ZCONFIG_ThreadTM_TLSMaxKeys];

// We do not explicitly nil these static variables. They'll start off with
// zero contents, and anything placed in them will persist, even through
// the execution of this file's static initializers.
static ZThreadTM_State* sThread_Current;
static ZThreadTM_State* sThread_Main;
static ZThreadTM_State* sThread_Zombie;

static ZThreadTM_State* sThread_Solo;

static ZThreadTM_Sem sSemZombie;

static ZAtomic_t sCriticalSectionFlag;

static ZAtomic_t sReadyCount;

static ProcessSerialNumber sPSN;

static bool sInWaitNextEvent;

static bigtime_t sTimeEnteredProcess;
static bigtime_t sTimeAccumulated;

static int32 sSavedA5;

static void sEnableThread(ZThreadTM_State* inThread);
static void sDisableThread(ZThreadTM_State* inThread);
static void sEnterCritical();
static void sExitCritical();
static void sInternal_Yield(ZThreadTM_State* inNextThread);
static ZThreadTM_Error sInternal_SemWait(ZThreadTM_Sem* iSem, int32 inCount, bigtime_t inTimeout);
static void sInternal_SemSignal(ZThreadTM_Sem* iSem, int32 inCount);

// =================================================================================================
#pragma mark -
#pragma mark * Thread Manager based

static void sEnableThread(ZThreadTM_State* inThread)
	{
	ZAssertStop(kDebug_ThreadTM, inThread);
	if (ZAtomic_Swap(&inThread->fEnabled, 1) == 0)
		{
		ZAtomic_Inc(&sReadyCount);

		if (sInWaitNextEvent)
			::WakeUpProcess(&sPSN);
		}
	}

static void sDisableThread(ZThreadTM_State* inThread)
	{
	ZAssertStop(kDebug_ThreadTM, inThread);
	if (ZAtomic_Swap(&inThread->fEnabled, 0) == 1)
		ZAtomic_Dec(&sReadyCount);
	}

static void sEnterCritical()
	{
	while (1 == ZAtomic_Swap(&sCriticalSectionFlag, 1))
		{}
	}

static void sExitCritical()
	{ ZAtomic_Set(&sCriticalSectionFlag, 0); }

// Forward declaration of sThreadEntry so that ZThreadTM_Initialize can initialize
// sThreadEntryTPP, which otherwise will not get set until static construction
// has finished, which may be too late if we spawn a thread before that time. Unlikely,
// and not recommended, but possible and a pain to debug.
static DEFINE_API(void*) sThreadEntry(void* arg);
static ThreadEntryTPP sThreadEntryTPP;

void ZThreadTM_Initialize()
	{
	ZAssertStop(kDebug_ThreadTM, sThread_Main == nil);

	sThreadEntryTPP = NewThreadEntryUPP(sThreadEntry);

#if !TARGET_API_MAC_CARBON
	// It's important that MaxApplZone() be called before non main threads are allocated
	// when using Thread Manager, and it should be called before setting up even the main
	// threads when using setjmp/longjmp. So we call it here to be sure the app's heap zone
	// is fully expanded prior to allocating new stacks or recording the stack base of the main threads.
	::MaxApplZone();
#endif // TARGET_API_MAC_CARBON

	sSavedA5 = ::SetCurrentA5();

	// Remember the app's PSN
	::GetCurrentProcess(&sPSN);

	// Initialize our process times
	sTimeAccumulated = 0;
	sTimeEnteredProcess = sNow();

	// Set up the zombie semaphore
	ZThreadTM_SemInit(&sSemZombie, 1);

	// Ensure the TLS flags are clear
	for (size_t x = 0; x < ZCONFIG_ThreadTM_TLSMaxKeys; ++x)
		sTLSDataAllocated[x] = false;

	// Allocate the ZThreadTM_State for the main thread.
	ZThreadTM_State* mainThread = new ZThreadTM_State;
	mainThread->fEntryProc = nil;
	mainThread->fStartTime = sTimeEnteredProcess;
	mainThread->fCumulativeTime = 0;
	mainThread->fAllowTimeSlice = true;
	ZAtomic_Set(&mainThread->fEnabled, 1);
	ZAtomic_Set(&mainThread->fSuspendCount, 0);
	ZAtomic_Inc(&sReadyCount);
	mainThread->fInMultiComplete = false;
	for (size_t x = 0; x < ZCONFIG_ThreadTM_TLSMaxKeys; ++x)
		mainThread->fTLSData[x] = 0;

	mainThread->fSwitchInfo_Head = nil;
	mainThread->fSwitchInfo_Tail = nil;

	::GetCurrentThread(&mainThread->fID);

	// Get profiling switched on *now* -- thread-specific profiling blows up otherwise
	#if ZCONFIG_ThreadTM_Profile
		short result = ::ProfilerInit(collectDetailed, bestTimeBase, 30000, 100);
		ZAssertStop(kDebug_ThreadTM, result == 0);
		::ProfilerSetStatus(0);
		::ProfilerClear();
		mainThread->fProfilerThreadRef = ::ProfilerGetMainThreadRef();
	#endif

	// And get the linked list stuff underway
	mainThread->fThread_Next = mainThread;
	mainThread->fThread_Previous = mainThread;
	sThread_Current = mainThread;

	sThread_Main = mainThread;
	}

void ZThreadTM_Exit()
	{
	ZAssertStop(kDebug_ThreadTM, sThread_Main != nil);

	// We must be called from the main thread.
	ZAssertStop(kDebug_ThreadTM, sThread_Current == sThread_Main);

	// Wait till the list of threads goes empty
	#if !ZCONFIG_ThreadTM_Profile
		sEnterCritical();
		while (sThread_Current->fThread_Next != sThread_Current)
			sInternal_Yield(sThread_Current->fThread_Next);
		sExitCritical();
	#endif

	#if ZCONFIG_ThreadTM_Profile
		::ProfilerTerm();
	#endif

	ZThreadTM_SemDestroy(&sSemZombie);
	delete sThread_Main;
	sThread_Main = nil;
	}

// ==================================================
static void sInternal_Yield(ZThreadTM_State* iStartThread)
	{
	bigtime_t now = sNow();
	ZThreadTM_State* nextThread = iStartThread;

	// Loop continuously until an enabled thread becomes available.
	while (true)
		{
		if (ZAtomic_Get(&nextThread->fEnabled))
			break;
		nextThread = nextThread->fThread_Next;
		}

	if (nextThread != sThread_Current)
		{
		// Note that later versions of the Thread Manager do not always yield to the thread
		// we tell it. We allow for this by recording sThread_Current in a *local* variable,
		// hence we're able to determine which thread we actually switched *to* by examining
		// the local, and only completing the yield if the thread in question is actually
		// enabled. With this revised scheme the loop at the beginning of this method is
		// superfluous iff the Thread Manager *never* yields to the thread we want. If it does
		// at least some of the time, then finding an appropriate target is a useful optimization.

		// Call all the switch procs
		ZThreadTM_SwitchInfo* currentInfo = sThread_Current->fSwitchInfo_Head;
		while (currentInfo)
			{
			currentInfo->fSwitchProc(false, currentInfo);
			currentInfo = currentInfo->fNext;
			}

		sThread_Current->fCumulativeTime += now - sThread_Current->fStartTime;
		ZThreadTM_State* currentThread = sThread_Current;
		do
			{
			OSErr yieldErr = ::YieldToThread((ThreadID)nextThread->fID);
			} while (!(ZAtomic_Get(&currentThread->fEnabled) && (sThread_Solo == nil || currentThread == sThread_Solo)));
		sThread_Current = currentThread;

		// Record the time we started running again
		sThread_Current->fStartTime = sNow();

		#if ZCONFIG_ThreadTM_Profile
			::ProfilerSwitchToThread(sThread_Current->fProfilerThreadRef);
		#endif

		// Call the switch procs
		currentInfo = sThread_Current->fSwitchInfo_Tail;
		while (currentInfo)
			{
			currentInfo->fSwitchProc(true, currentInfo);
			currentInfo = currentInfo->fPrev;
			}
		}

	if (sThread_Zombie)
		{
		sInternal_SemWait(&sSemZombie, 1, LONG_LONG_MAX);
		while (sThread_Zombie)
			{
			ZThreadTM_State* currentZombie = sThread_Zombie;
			sThread_Zombie = sThread_Zombie->fThread_Next;
			sInternal_SemSignal(&sSemZombie, 1);
			sExitCritical();

			#if ZCONFIG_ThreadTM_Profile
				::ProfilerDeleteThread(currentZombie->fProfilerThreadRef);
			#endif

			::DisposeThread(currentZombie->fID, nil, false);
			delete currentZombie;

			sEnterCritical();
			sInternal_SemWait(&sSemZombie, 1, LONG_LONG_MAX);
			}
		sInternal_SemSignal(&sSemZombie, 1);
		}
	}

// ==================================================

static DEFINE_API(void*) sThreadEntry(void* arg)
	{
	sThread_Current = reinterpret_cast<ZThreadTM_State*>(arg);

	while (ZAtomic_Get(&sThread_Current->fEnabled) == 0)
		sInternal_Yield(sThread_Current->fThread_Next);

	sThread_Current->fStartTime = sNow();

	#if ZCONFIG_ThreadTM_Profile
		::ProfilerSwitchToThread(sThread_Current->fProfilerThreadRef);
	#endif

	sExitCritical();

	try
		{
		sThread_Current->fEntryProc(sThread_Current->fArgument);
		}
	catch (...)
		{}

	sEnterCritical();

	ZAssertStop(kDebug_ThreadTM, sThread_Current != sThread_Current->fThread_Next);

	ZThreadTM_State* currentThread = sThread_Current;
	ZThreadTM_State* yieldToThread = sThread_Current->fThread_Next;

	sThread_Current->fThread_Next->fThread_Previous = sThread_Current->fThread_Previous;
	sThread_Current->fThread_Previous->fThread_Next = sThread_Current->fThread_Next;

	currentThread->fThread_Next = nil;
	currentThread->fThread_Previous = nil;

	currentThread->fThread_Next = sThread_Zombie;
	sThread_Zombie = currentThread;

	ZAtomic_Dec(&sReadyCount);

	sInternal_Yield(yieldToThread);
	// We never get to here
	ZAssertStop(kDebug_ThreadTM, false);
	return nil;
	}

bool ZThreadTM_NewThread(ZThreadTM_EntryProc* iProc, void* iArgument, ThreadID& oID, ZThreadTM_State*& oState)
	{
	ZAssertStop(kDebug_ThreadTM, sThread_Current != nil);
	ZAssertStop(kDebug_ThreadTM, iProc);

	oID = 0;

	ZThreadTM_State* newThread = nil;
	try
		{
		newThread = new ZThreadTM_State;
		newThread->fEntryProc = iProc;
		newThread->fArgument = iArgument;
		newThread->fStartTime = 0;
		newThread->fCumulativeTime = 0;
		newThread->fAllowTimeSlice = true;
		ZAtomic_Set(&newThread->fEnabled, 0);
		ZAtomic_Set(&newThread->fSuspendCount, 1);

		newThread->fInMultiComplete = false;

		for (size_t x = 0; x < ZCONFIG_ThreadTM_TLSMaxKeys; ++x)
			newThread->fTLSData[x] = 0;

		newThread->fSwitchInfo_Head = nil;
		newThread->fSwitchInfo_Tail = nil;

		if (noErr != ::NewThread(kCooperativeThread, sThreadEntryTPP, newThread, 32 * 1024, 0, nil, &newThread->fID))
			throw bad_alloc();

		sEnterCritical();

		// -----
		#if ZCONFIG_ThreadTM_Profile
			unsigned long profilerStackSize;
			::ThreadCurrentStackSpace(newThread->fID, &profilerStackSize);
			OSErr profileErr = ::ProfilerCreateThread(profilerStackSize / 64, profilerStackSize, &newThread->fProfilerThreadRef);
			ZAssertStop(kDebug_ThreadTM, profileErr == noErr);
		#endif
		// -----

		newThread->fThread_Next = sThread_Current;
		newThread->fThread_Previous = sThread_Current->fThread_Previous;
		sThread_Current->fThread_Previous->fThread_Next = newThread;
		sThread_Current->fThread_Previous = newThread;

		oID = newThread->fID;
		oState = newThread;

		sExitCritical();
		}
	catch (...)
		{
		delete newThread;
		newThread = nil;
		}

	return newThread != nil;
	}

int32 ZThreadTM_ReadyCountNormal()
	{ return ZAtomic_Get(&sReadyCount); }

// =================================================================================================
#pragma mark -
#pragma mark * Generic

#if 1//ZCONFIG(OS, MacOS7)
static bigtime_t sLastWaitNextEvent;
void ZThreadTM_WaitNextEvent(bool inAvoidWait, EventRecord& outEventRecord, RgnHandle inCursorRgn)
	{
	ZAssertStop(kDebug_ThreadTM, sThread_Current);
	bigtime_t now;
	bigtime_t lastEventAvail = 0;
	while (true)
		{
		// Let other threads have some run time
		sEnterCritical();
		sInternal_Yield(sThread_Current->fThread_Next);
		sExitCritical();

		// If there's only one enabled thread, then it's us, and we might as well
		// call WaitNextEvent and let other apps have some run time.
		if (ZThreadTM_ReadyCountNormal() <= 1)
			break;

		// What time is it
		now = sNow();

		// If it's been more than 1 second since we called WaitNextEvent, then break.
		if ((now - sLastWaitNextEvent) >= 1000000LL)
			break;

		// Has it been more than 1/20 second since we called EventAvail
		if (now - lastEventAvail >= 50000LL)
			{
			// If so, then find out if we have any events pending, and if so then break.
			lastEventAvail = now;
			if (::EventAvail(everyEvent, &outEventRecord))
				break;
			}
		}

	// For timers and async operations, we mark the fact that we're calling WaitNextEvent, so
	// they can call WakeUpProcess and get us running again more quickly when they complete.
	sEnterCritical();
	sInWaitNextEvent = true;
	now = sNow();
	sTimeAccumulated += now - sTimeEnteredProcess;
	sExitCritical();

	if ((ZThreadTM_ReadyCountNormal() <= 1) && !inAvoidWait)
		{
		// If we don't have any other threads that can run, and we weren't asked to avoid
		// waiting (generally because we've captured the mouse somehow), then wait for 10 ticks.
		::WaitNextEvent(everyEvent, &outEventRecord, 10, inCursorRgn);
		}
	else
		{
		// Otherwise we wait for the minimum possible time.
		::WaitNextEvent(everyEvent, &outEventRecord, 1, inCursorRgn);
		}

	sEnterCritical();
	// Record the fact that we're back from WaitNextEvent.
	sInWaitNextEvent = false;
	// Update our notion of when we reentered the process, both for use in tracking
	// our use of time and so that the head of this method can know when it next needs
	// to call WaitNextEvent.
	now = sNow();
	sTimeEnteredProcess = now;
	sLastWaitNextEvent = now;
	sExitCritical();
	}
#endif // ZCONFIG(OS, MacOS7)


#if ZCONFIG(OS, Carbon) || ZCONFIG(OS, MacOSX)

static bigtime_t sLastReceiveNextEvent;
static OSStatus sReceiveNextEvent(uint32 inNumTypes, const EventTypeSpec* inList, EventTimeout inTimeout, EventRef* outEvent)
	{
	// For timers and async operations, we mark the fact that we're calling ReceiveNextEvent, so
	// they can call WakeUpProcess and get us running again more quickly when they complete.
	sEnterCritical();
	sInWaitNextEvent = true;
	bigtime_t now = sNow();
	sTimeAccumulated += now - sTimeEnteredProcess;
	sExitCritical();

	OSStatus theResult = ::ReceiveNextEvent(inNumTypes, inList, inTimeout, true, outEvent);

	sEnterCritical();
	// Record the fact that we're back from WaitNextEvent.
	sInWaitNextEvent = false;
	// Update our notion of when we reentered the process, both for use in tracking
	// our use of time and so that the head of this method can know when it next needs
	// to call WaitNextEvent.
	now = sNow();
	sTimeEnteredProcess = now;
	sLastReceiveNextEvent = now;
	sExitCritical();

	return theResult;
	}

OSStatus ZThreadTM_ReceiveNextEvent(bool inAvoidWait, uint32 inNumTypes, const EventTypeSpec* inList, EventRef* outEvent)
	{
	bigtime_t now;
	bigtime_t lastNonBlockingReceiveNextEvent = 0;
	while (true)
		{
		// Let other threads have some run time
		sEnterCritical();
		sInternal_Yield(sThread_Current->fThread_Next);
		sExitCritical();

		// If there's only one enabled thread, then it's us, and we might as well
		// call WaitNextEvent and let other apps have some run time.
		if (ZThreadTM_ReadyCountNormal() <= 1)
			break;

		// What time is it
		now = sNow();

		// If it's been more than 1 second since we called ReceiveNextEvent, then break.
		if ((now - sLastReceiveNextEvent) >= 1000000LL)
			break;

		// Has it been more than 1/20 second since we called ReceiveNextEvent without blocking.
		if (now - lastNonBlockingReceiveNextEvent >= 50000LL)
			{
			lastNonBlockingReceiveNextEvent = now;
			// If so then make a non-blocking call to ReceiveNextEvent.
			OSStatus result = sReceiveNextEvent(inNumTypes, inList, kEventDurationNoWait, outEvent);
			if (result == noErr)
				return noErr;
			}
		}

	if ((ZThreadTM_ReadyCountNormal() <= 1) && !inAvoidWait)
		{
		// If we don't have any other threads that can run, and we weren't asked to avoid
		// waiting (generally because we've captured the mouse somehow), then wait for 1/5 second.
		return sReceiveNextEvent(inNumTypes, inList, kEventDurationSecond / 5, outEvent);
		}
	else
		{
		// Otherwise we wait for what used to be one tick.
		return sReceiveNextEvent(inNumTypes, inList, kEventDurationSecond / 5, outEvent);
		}
	}

#endif // ZCONFIG(OS, Carbon) || ZCONFIG(OS, MacOSX)

struct ZThreadTM_TMTask
	{
	TMTask fTMTask;
	ZThreadTM_State* fThread;
	};

static DEFINE_API(void) sTimerTaskSleep(TMTask* iTMTask ZPARAM_A1)
	{
	long priorA5 = ::SetA5(sSavedA5);
	bool priorInvokeLoweLevelDebugger = sDebug_InvokeLowLevelDebugger;
	sDebug_InvokeLowLevelDebugger = true;

	// We don't have to work hard to find the fThread member of theTMTask -- we defined its structure,
	// unlike the case in sIOCompletion where the parameter block could be of any length depending on
	// what operation was invoked, and hence have to stick fThread *before* the rest of the data
	ZThreadTM_TMTask* theTMTask = reinterpret_cast<ZThreadTM_TMTask*>(iTMTask);

	sEnableThread(theTMTask->fThread);

	sDebug_InvokeLowLevelDebugger = priorInvokeLoweLevelDebugger;
	::SetA5(priorA5);
	}

static TimerUPP sTimerTaskSleepUPP = NewTimerUPP(sTimerTaskSleep);

void ZThreadTM_Sleep(bigtime_t iMicroseconds)
	{
	if (iMicroseconds <= 0)
		{
		// Just yield if there's no real wait time
		sEnterCritical();
		sInternal_Yield(sThread_Current->fThread_Next);
		sExitCritical();
		return;
		}

	while (iMicroseconds > 0)
		{
		int32 currentDelay = 0x40000000;
		if (currentDelay > iMicroseconds)
			currentDelay = iMicroseconds;
		iMicroseconds -= currentDelay;

		ZThreadTM_TMTask theTMTask;
		theTMTask.fTMTask.qLink = nil;
		theTMTask.fTMTask.qType = 0;
		theTMTask.fTMTask.tmAddr = sTimerTaskSleepUPP;
		theTMTask.fTMTask.tmCount = 0;
		theTMTask.fTMTask.tmWakeUp = 0;
		theTMTask.fTMTask.tmReserved = 0;
		theTMTask.fThread = sThread_Current;
		::InsTime(reinterpret_cast<QElem*>(&theTMTask.fTMTask));

		sEnterCritical();
		sDisableThread(sThread_Current);
		::PrimeTime(reinterpret_cast<QElem*>(&theTMTask.fTMTask), -currentDelay);
		sInternal_Yield(sThread_Current->fThread_Next);
		sExitCritical();

		::RmvTime(reinterpret_cast<QElem*>(&theTMTask.fTMTask));
		}
	}

void ZThreadTM_Yield()
	{
	sEnterCritical();
	sInternal_Yield(sThread_Current->fThread_Next);
	sExitCritical();
	}

static void sInternal_CheckExpiredTimeSlice()
	{
	if (sThread_Current->fAllowTimeSlice)
		{
		bigtime_t currentTime = sNow();

		// Yield if this thread has been running for 50ms or more.
		if (currentTime - sThread_Current->fStartTime >= 50000LL)
			sInternal_Yield(sThread_Current->fThread_Next);
		}
	}

void ZThreadTM_YieldIfTimeSliceExpired()
	{
	sEnterCritical();
	sInternal_CheckExpiredTimeSlice();
	sExitCritical();
	}

void ZThreadTM_GetProcessTimes(bigtime_t& oRealTime, bigtime_t& oCumulativeRunTime)
	{
	sEnterCritical();
	oRealTime = sNow();
	oCumulativeRunTime = oRealTime - sTimeEnteredProcess + sTimeAccumulated;
	sExitCritical();
	}

void ZThreadTM_GetThreadTimes(ThreadID iID, bigtime_t& oRealTime, bigtime_t& oCumulativeRunTime)
	{
	sEnterCritical();
	ZThreadTM_State* currentThread = sThread_Current;
	oRealTime = sNow();
	oCumulativeRunTime = oRealTime;
	do
		{
		if (currentThread->fID == iID)
			{
			oCumulativeRunTime = currentThread->fCumulativeTime;
			if (currentThread == sThread_Current)
				oCumulativeRunTime += oRealTime - currentThread->fStartTime;
			break;
			}
		currentThread = currentThread->fThread_Next;
		} while (currentThread != sThread_Current);
	sExitCritical();
	}

int32 ZThreadTM_ReadyCount()
	{ return ZAtomic_Get(&sReadyCount); }

bool ZThreadTM_SetCurrentAllowTimeSlice(bool iAllowTimeSlice)
	{
	bool result = sThread_Current->fAllowTimeSlice;
	sThread_Current->fAllowTimeSlice = iAllowTimeSlice;
	return result;
	}

bool ZThreadTM_SetCurrentSolo(bool iSolo)
	{
	// Because we're probably being called by the debugging gear we don't use
	// ZAssert or ZDebug to report the serious error that a non-soloed thread
	// is trying to change the soloed state -- which just shouldn't be possible -- the
	// only thread executing will be the soloed thread, so how could some other
	// thread get a chance to execute and call us?
	sEnterCritical();
	bool result;
	if (iSolo)
		{
		if (sThread_Solo == nil)
			{
			sThread_Solo = sThread_Current;
			result = false;
			}
		else if (sThread_Solo == sThread_Current)
			result = true;
		else
			::DebugStr("\pZThreadTM_SetCurrentZThreadMacSolo, serious fuck up.");
		}
	else
		{
		if (sThread_Solo == nil)
			result = false;
		else if (sThread_Solo == sThread_Current)
			{
			sThread_Solo = nil;
			result = true;
			}
		else
			::DebugStr("\pZThreadTM_SetCurrentZThreadMacSolo, serious fuck up.");
		}
	sExitCritical();
	return result;
	}

// ==================================================
// Thread Local Storage (TLS), aka Thread Specific Data (TSD)

ZThreadTM_TLSKey_t ZThreadTM_TLSAllocate()
	{
	sEnterCritical();
	ZAssertStop(kDebug_ThreadTM, sThread_Current);
	for (size_t x = 0; x < ZCONFIG_ThreadTM_TLSMaxKeys; ++x)
		{
		if (!sTLSDataAllocated[x])
			{
			sTLSDataAllocated[x] = true;
			sExitCritical();
			return reinterpret_cast<ZThreadTM_TLSKey_t>(x);
			}
		}
	sExitCritical();
	ZDebugStopf(0, ("ZThreadTM_TLSAllocate, no free keys"));
	return 0;
	}

void ZThreadTM_TLSFree(ZThreadTM_TLSKey_t iKey)
	{
	sEnterCritical();
	ZAssertStop(kDebug_ThreadTM, sThread_Current);
	ZAssertStop(kDebug_ThreadTM, reinterpret_cast<int>(iKey) < ZCONFIG_ThreadTM_TLSMaxKeys);
	ZAssertStop(kDebug_ThreadTM, sTLSDataAllocated[reinterpret_cast<int>(iKey)]);
	sTLSDataAllocated[reinterpret_cast<int>(iKey)] = false;
	sExitCritical();
	}

ZThreadTM_TLSData_t ZThreadTM_TLSGet(ZThreadTM_TLSKey_t iKey)
	{
	ZAssertStop(kDebug_ThreadTM, sThread_Current);
	ZAssertStop(kDebug_ThreadTM, reinterpret_cast<int>(iKey) < ZCONFIG_ThreadTM_TLSMaxKeys);
	ZAssertStop(kDebug_ThreadTM, sTLSDataAllocated[reinterpret_cast<int>(iKey)]);
	return sThread_Current->fTLSData[reinterpret_cast<int>(iKey)];
	}

void ZThreadTM_TLSSet(ZThreadTM_TLSKey_t iKey, ZThreadTM_TLSData_t iData)
	{
	ZAssertStop(kDebug_ThreadTM, sThread_Current);
	ZAssertStop(kDebug_ThreadTM, reinterpret_cast<int>(iKey) < ZCONFIG_ThreadTM_TLSMaxKeys);
	ZAssertStop(kDebug_ThreadTM, sTLSDataAllocated[reinterpret_cast<int>(iKey)]);
	sThread_Current->fTLSData[reinterpret_cast<int>(iKey)] = iData;
	}

// ==================================================
// Asynchronous I/O support

static DEFINE_API(void) sIOCompletion(ParamBlockRec* iParamBlockRec ZPARAM_A0)
	{
	long priorA5 = ::SetA5(sSavedA5);
	bool priorInvokeLoweLevelDebugger = sDebug_InvokeLowLevelDebugger;
	sDebug_InvokeLowLevelDebugger = true;

	// We've been handed a pointer to a ParamBlockRec, which is embedded inside
	// a ZThreadMacIOParamBlock (or a struct with equivalent layout). So we need to
	// move back a bit to find the PBHeader stuff which tells us which thread to re-enable
	ZThreadTM_IOParamBlock* threadIOParamBlock = reinterpret_cast<ZThreadTM_IOParamBlock*>(reinterpret_cast<char*>(iParamBlockRec) - offsetof(ZThreadTM_IOParamBlock, fIOParam));
	sEnableThread(threadIOParamBlock->fThread);

	sDebug_InvokeLowLevelDebugger = priorInvokeLoweLevelDebugger;
	::SetA5(priorA5);
	}

static IOCompletionUPP sIOCompletionUPP = NewIOCompletionUPP(sIOCompletion);

void ZThreadTM_SetupForAsyncCompletion(ZThreadTM_PBHeader* inParamBlock)
	{
	ZThreadTM_IOParamBlock* threadParamBlock = reinterpret_cast<ZThreadTM_IOParamBlock*>(inParamBlock);
	threadParamBlock->fThread = sThread_Current;
	threadParamBlock->fIOParam.ioCompletion = sIOCompletionUPP;
	sEnterCritical();
	// Although we call sDisableThread here, we won't exit its thread of execution
	// until sInternal_Yield is called. As we're inside the critical section, our
	// deferred task won't run either (if it needs to run, the next call to sExitCritical
	// will install it).
	sDisableThread(sThread_Current);
	}


OSErr ZThreadTM_WaitForAsyncCompletion(ZThreadTM_PBHeader* inParamBlock, OSErr err)
	{
	if (err == noErr)
		{
		// We internal yield to the *current* thread. If the async operation has
		// already completed then sThread_Current->fEnabled will have been set
		// and we won't yield at all. Otherwise we'll skip sThread_Current
		// and move on to the next enabled thread.
		sInternal_Yield(sThread_Current);
		ZThreadTM_IOParamBlock* threadParamBlock = reinterpret_cast<ZThreadTM_IOParamBlock*>(inParamBlock);
		err = threadParamBlock->fIOParam.ioResult;
		}
	else
		{
		sEnableThread(sThread_Current);
		}

	sInternal_CheckExpiredTimeSlice();

	sExitCritical();
	return err;
	}

static DEFINE_API(void) sIOCompletionMulti(ParamBlockRec* iParamBlockRec ZPARAM_A0)
	{
	long priorA5 = ::SetA5(sSavedA5);
	// And ensure that the low level debugger gets invoked, rather than the source level debugger
	bool priorInvokeLoweLevelDebugger = sDebug_InvokeLowLevelDebugger;
	sDebug_InvokeLowLevelDebugger = true;

	ZThreadTM_IOParamBlock* threadIOParamBlock = reinterpret_cast<ZThreadTM_IOParamBlock*>(reinterpret_cast<char*>(iParamBlockRec)-offsetof(ZThreadTM_IOParamBlock, fIOParam));

	if (threadIOParamBlock->fThread->fInMultiComplete && threadIOParamBlock->fThread)
		sEnableThread(threadIOParamBlock->fThread);

	sDebug_InvokeLowLevelDebugger = priorInvokeLoweLevelDebugger;
	::SetA5(priorA5);
	}

static IOCompletionUPP sIOCompletionMultiUPP = NewIOCompletionUPP(sIOCompletionMulti);

void ZThreadTM_SetupForAsyncCompletionMulti(ZThreadTM_PBHeader* inParamBlock)
	{
	ZThreadTM_IOParamBlock* threadParamBlock = reinterpret_cast<ZThreadTM_IOParamBlock*>(inParamBlock);
	threadParamBlock->fThread = nil;
	threadParamBlock->fIOParam.ioCompletion = sIOCompletionMultiUPP;
	}

void ZThreadTM_WaitForAsyncCompletionMulti(ZThreadTM_PBHeader** inParamBlocks, size_t count)
	{
	// A single thread could easily have more than one multi-completion async call
	// outstanding. Here we walk the list of parameter blocks passed to us and only
	// return if one of *them* is marked as completed. So if another multi-complete call
	// does have one of its async calls complete it could wake this thread, but then
	// we'll disable ourselves again as it won't be one of the current param blocks that
	// is marked as completed.
	ZAssertStop(kDebug_ThreadTM, count > 0);

	sEnterCritical();

	sThread_Current->fInMultiComplete = true;
	register bool gotOne = false;
	while (!gotOne)
		{
		sDisableThread(sThread_Current);

		register ZThreadTM_IOParamBlock** localPtr = reinterpret_cast<ZThreadTM_IOParamBlock**>(inParamBlocks);
		register size_t localCount = count + 1;
		while (--localCount && !gotOne)
			{
			localPtr[0]->fThread = sThread_Current;
			if (localPtr[0]->fIOParam.ioResult != 1)
				{
				sEnableThread(sThread_Current);
				gotOne = true;
				}
			++localPtr;
			}

		sInternal_Yield(sThread_Current);
		}
	sThread_Current->fInMultiComplete = false;

	sInternal_CheckExpiredTimeSlice();

	sExitCritical();
	}

#if !TARGET_API_MAC_CARBON
OSErr ZThreadTM_PBControlAndBlock(ZThreadTM_PBHeader* inParamBlock)
	{
	ZThreadTM_SetupForAsyncCompletion(inParamBlock);
	ZThreadTM_IOParamBlock* threadPB = reinterpret_cast<ZThreadTM_IOParamBlock*>(inParamBlock);
	OSErr err = ::PBControlAsync(reinterpret_cast<ParamBlockRec*>(&threadPB->fIOParam));
	return ZThreadTM_WaitForAsyncCompletion(threadPB, err);
	}
#endif // TARGET_API_MAC_CARBON

extern "C" ZThreadTM_State* ZThreadTM_Current()
	{
	return sThread_Current;
	}

extern "C" void ZThreadTM_Suspend()
	{
	sEnterCritical();
	sDisableThread(sThread_Current);
	if (ZAtomic_Add(&sThread_Current->fSuspendCount, 1) < 0)
		{
		sEnableThread(sThread_Current);
		sInternal_CheckExpiredTimeSlice();
		}
	else
		{
		sInternal_Yield(sThread_Current);
		}
	sExitCritical();
	}

extern "C" void ZThreadTM_Resume(ZThreadTM_State* iThread)
	{
	if (ZAtomic_Add(&iThread->fSuspendCount, -1) == 1)
		sEnableThread(iThread);
	}

// ==================================================

void ZThreadTM_InstallSwitchCallback(ZThreadTM_SwitchInfo* inSwitchInfo)
	{
	ZAssertStop(kDebug_ThreadTM, inSwitchInfo);
	ZAssertStop(kDebug_ThreadTM, inSwitchInfo->fSwitchProc != nil && inSwitchInfo->fPrev == nil && inSwitchInfo->fNext == nil);
	if (sThread_Current->fSwitchInfo_Head)
		{
		inSwitchInfo->fNext = sThread_Current->fSwitchInfo_Head;
		sThread_Current->fSwitchInfo_Head->fPrev = inSwitchInfo;
		sThread_Current->fSwitchInfo_Head = inSwitchInfo;
		}
	else
		{
		sThread_Current->fSwitchInfo_Head = inSwitchInfo;
		sThread_Current->fSwitchInfo_Tail = inSwitchInfo;
		}
	}

void ZThreadTM_RemoveSwitchCallback(ZThreadTM_SwitchInfo* inSwitchInfo)
	{
	if (inSwitchInfo->fPrev)
		inSwitchInfo->fPrev->fNext = inSwitchInfo->fNext;
	if (inSwitchInfo->fNext)
		inSwitchInfo->fNext->fPrev = inSwitchInfo->fPrev;
	if (sThread_Current->fSwitchInfo_Head == inSwitchInfo)
		sThread_Current->fSwitchInfo_Head = inSwitchInfo->fNext;
	if (sThread_Current->fSwitchInfo_Tail == inSwitchInfo)
		sThread_Current->fSwitchInfo_Tail = inSwitchInfo->fPrev;
	inSwitchInfo->fPrev = nil;
	inSwitchInfo->fNext = nil;
	}

// ==================================================

struct ZThreadTM_Waiter
	{
	ZThreadTM_Waiter* fPrev;
	ZThreadTM_Waiter* fNext;
	ZThreadTM_State* fThread;
	int32 fCount;
	bool fSemaphoreDisposed;
	};

void ZThreadTM_SemInit(ZThreadTM_Sem* iSem, int32 inInitialCount)
	{
	ZAssertStop(kDebug_ThreadTM, iSem);
	iSem->fWaiter_Head = nil;
	iSem->fWaiter_Tail = nil;
	iSem->fAvailable = inInitialCount;
	}

void ZThreadTM_SemDestroy(ZThreadTM_Sem* iSem)
	{
	ZAssertStop(kDebug_ThreadTM, iSem);
	sEnterCritical();
	while (iSem->fWaiter_Head)
		{
		ZAssertStop(kDebug_ThreadTM, iSem->fWaiter_Head->fPrev == nil);

		iSem->fWaiter_Head->fSemaphoreDisposed = true;
		sEnableThread(iSem->fWaiter_Head->fThread);
		if (iSem->fWaiter_Tail == iSem->fWaiter_Head)
			iSem->fWaiter_Tail = nil;
		if (iSem->fWaiter_Head->fNext)
			{
			iSem->fWaiter_Head = iSem->fWaiter_Head->fNext;
			iSem->fWaiter_Head->fPrev->fNext = nil;
			iSem->fWaiter_Head->fPrev = nil;
			}
		else
			{
			iSem->fWaiter_Head = nil;
			}
		}
	sExitCritical();
	}

ZThreadTM_Error ZThreadTM_SemWait(ZThreadTM_Sem* iSem, int32 inCount)
	{
	ZAssertStop(kDebug_ThreadTM, iSem);
	ZAssertStop(kDebug_ThreadTM, inCount >= 0);

	if (inCount == 0)
		return ZThreadTM_ErrorNone;

	sEnterCritical();
	ZThreadTM_Error result = sInternal_SemWait(iSem, inCount, LONG_LONG_MAX);
	sExitCritical();
	return result;
	}

ZThreadTM_Error ZThreadTM_SemWait(ZThreadTM_Sem* iSem, int32 inCount, bigtime_t iMicroseconds)
	{
	ZAssertStop(kDebug_ThreadTM, iSem);
	ZAssertStop(kDebug_ThreadTM, inCount >= 0);

	if (inCount == 0)
		return ZThreadTM_ErrorNone;

	sEnterCritical();
	ZThreadTM_Error result = sInternal_SemWait(iSem, inCount, iMicroseconds);
	sExitCritical();
	return result;
	}

void ZThreadTM_SemSignal(ZThreadTM_Sem* iSem, int32 inCount)
	{
	ZAssertStop(kDebug_ThreadTM, iSem);
	ZAssertStop(kDebug_ThreadTM, inCount >= 0);

	sEnterCritical();
	sInternal_SemSignal(iSem, inCount);
	sExitCritical();
	}

static ZThreadTM_Error sInternal_SemWait(ZThreadTM_Sem* iSem, int32 inCount, bigtime_t iMicroseconds)
	{
	ZThreadTM_Waiter theWaiter;

	if (iSem->fWaiter_Tail == nil)
		{
		// Nothing else is waiting on the semaphore
		if (iSem->fAvailable >= inCount)
			{
			// And the count is adequate, so decrement and return.
			iSem->fAvailable -= inCount;
			return ZThreadTM_ErrorNone;
			}
		// Bail if our timeout was zero
		if (iMicroseconds <= 0)
			return ZThreadTM_ErrorTimeout;

		// The count was not adequate, we're going to have to block. We know the list is empty, so just put ourselves at the head
		ZAssertStop(kDebug_ThreadTM, iSem->fWaiter_Head == nil);
		iSem->fWaiter_Head = &theWaiter;
		theWaiter.fPrev = nil;
		// Record how many more signals will be needed, and take the available count down to zero
		theWaiter.fCount = inCount - iSem->fAvailable;
		iSem->fAvailable = 0;
		}
	else
		{
		// We've already got stuff waiting on this semaphore
		// Bail if our timeout was zero
		if (iMicroseconds <= 0)
			return ZThreadTM_ErrorTimeout;

		// Attach to the tail
		theWaiter.fPrev = iSem->fWaiter_Tail;
		iSem->fWaiter_Tail->fNext = &theWaiter;
		theWaiter.fCount = inCount;
		}
	iSem->fWaiter_Tail = &theWaiter;
	theWaiter.fNext = nil;
	theWaiter.fThread = sThread_Current;
	theWaiter.fSemaphoreDisposed = false;

	if (iMicroseconds < LONG_LONG_MAX)
		{
		while (iMicroseconds > 0 && theWaiter.fCount != 0)
			{
			int32 currentDelay = 0x40000000;
			if (currentDelay > iMicroseconds)
				currentDelay = iMicroseconds;
			iMicroseconds -= currentDelay;

			ZThreadTM_TMTask theTMTask;
			theTMTask.fTMTask.qLink = nil;
			theTMTask.fTMTask.qType = 0;
			theTMTask.fTMTask.tmAddr = sTimerTaskSleepUPP;
			theTMTask.fTMTask.tmCount = 0;
			theTMTask.fTMTask.tmWakeUp = 0;
			theTMTask.fTMTask.tmReserved = 0;
			theTMTask.fThread = sThread_Current;
			::InsTime(reinterpret_cast<QElem*>(&theTMTask.fTMTask));
			//::InstallTimeTask(reinterpret_cast<QElem*>(&theTMTask.fTMTask)); // -ec 01.08.31
			sDisableThread(sThread_Current);
			::PrimeTime(reinterpret_cast<QElem*>(&theTMTask.fTMTask), -currentDelay);
			sInternal_Yield(sThread_Current->fThread_Next);
			::RmvTime(reinterpret_cast<QElem*>(&theTMTask.fTMTask));

			if (theWaiter.fSemaphoreDisposed)
				return ZThreadTM_ErrorDisposed;
			}

		// If our count is non-zero then we must have timed out, in which case we're still on the list
		if (theWaiter.fCount != 0)
			{
			if (theWaiter.fPrev)
				theWaiter.fPrev->fNext = theWaiter.fNext;
			if (theWaiter.fNext)
				theWaiter.fNext->fPrev = theWaiter.fPrev;
			if (iSem->fWaiter_Tail == &theWaiter)
				iSem->fWaiter_Tail = theWaiter.fPrev;
			if (iSem->fWaiter_Head == &theWaiter)
				{
				iSem->fWaiter_Head = theWaiter.fNext;
				sInternal_SemSignal(iSem, inCount - theWaiter.fCount);
				}
			return ZThreadTM_ErrorTimeout;
			}
		}
	else
		{
		sDisableThread(sThread_Current);
		sInternal_Yield(sThread_Current->fThread_Next);
		if (theWaiter.fSemaphoreDisposed)
			return ZThreadTM_ErrorDisposed;
		ZAssertStop(kDebug_ThreadTM, theWaiter.fCount == 0);
		ZAssertStop(kDebug_ThreadTM, theWaiter.fPrev == nil);
		ZAssertStop(kDebug_ThreadTM, theWaiter.fNext == nil);
		}

	return ZThreadTM_ErrorNone;
	}

static void sInternal_SemSignal(ZThreadTM_Sem* iSem, int32 inCount)
	{
	while (iSem->fWaiter_Head && inCount > 0)
		{
		if (inCount >= iSem->fWaiter_Head->fCount)
			{
			inCount -= iSem->fWaiter_Head->fCount;
			iSem->fWaiter_Head->fCount = 0;
			sEnableThread(iSem->fWaiter_Head->fThread);

			if (iSem->fWaiter_Head->fNext)
				{
				iSem->fWaiter_Head = iSem->fWaiter_Head->fNext;
				iSem->fWaiter_Head->fPrev->fNext = nil;
				iSem->fWaiter_Head->fPrev = nil;
				}
			else
				{
				ZAssertStop(kDebug_ThreadTM, iSem->fWaiter_Head == iSem->fWaiter_Tail);
				iSem->fWaiter_Head = nil;
				iSem->fWaiter_Tail = nil;
				}
			}
		else
			{
			iSem->fWaiter_Head->fCount -= inCount;
			inCount = 0;
			}
		}
	iSem->fAvailable += inCount;
	}

// =================================================================================================
#pragma mark -
#pragma mark * Profiler Support

#if ZCONFIG_ThreadTM_Profile
#	include "ZString.h" // For ZString::sAsPString
#endif

bool ZThreadTM_ProfilerSetStatus(bool enabled)
	{
	#if ZCONFIG_ThreadTM_Profile
		bool oldState = (::ProfilerGetStatus() != 0);
		::ProfilerSetStatus(enabled ? 1 : 0);
		return oldState;
	#else
		return false;
	#endif
	}

bool ZThreadTM_ProfilerGetStatus()
	{
	#if ZCONFIG_ThreadTM_Profile
		return ::ProfilerGetStatus() != 0;
	#else
		return false;
	#endif
	}

void ZThreadTM_ProfilerDump(const char* filename)
	{
	#if ZCONFIG_ThreadTM_Profile
		::ProfilerDump((StringPtr)ZString::sAsPString(filename));
	#endif
	}

void ZThreadTM_ProfilerClear()
	{
	#if ZCONFIG_ThreadTM_Profile
		::ProfilerClear();
	#endif
	}

#endif // ZCONFIG(OS, MacOS7) || ZCONFIG(OS, Carbon) || ZCONFIG(OS, MacOSX)
