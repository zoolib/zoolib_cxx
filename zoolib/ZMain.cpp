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

#include "zoolib/ZMain.h"
#include "zoolib/ZDebug.h"

#include <stdexcept>

int ZMainNS::sArgC;
char** ZMainNS::sArgV;

/** sMain puts argc and argv into the globals ZMainNS::sArgC and
ZMain::sArgV, and then invokes the application-defined ZMain, wrapping
the call in in a try/catch block. Ideally your ZMain will not allow
exceptions to propogate, and if it does this code will report them
using ZDebugLogf. */

static int sMain(int argc, char **argv)
	{
	ZMainNS::sArgC = argc;
	ZMainNS::sArgV = argv;

	try
		{
		return ZMain(argc, argv);
		}
	catch (std::exception& theEx)
		{
		ZDebugLogf(2, ("Exception \"%s\" not caught by ZMain", theEx.what()));
		}
	catch (...)
		{
		ZDebugLogf(0, ("Exception not caught by ZMain"));
		}

	return 1;
	}

// =================================================================================================

#if ZCONFIG_SPI_Enabled(MacClassic)\
	|| (ZCONFIG_SPI_Enabled(Carbon) && !ZCONFIG_SPI_Enabled(MacOSX))

#include "zoolib/ZMacOSX.h"
#include "zoolib/ZThreadSimple.h"

/*
If WaitNextEvent is called from a thread other than the main thread then the Finder shows us as
using our entire heap, which is deceptive. The Finder bases its memory usage reporting on the
difference between the start of the heap and HiHeapMark. The MacOS Thread Manager and ZFiber
setjmp/longjmp both manipulate the value of HiHeapMark to prevent certain ToolBox routines from
bailing with a "stack moved into heap" DS error.

The solution is to invoke the application's ZMain function from a new thread. Meanwhile the real
main blocks, waiting for ZMainNS::sInvokeAsMainThread to be called. The function passed to
sInvokeAsMainThread is then run from the real main. Usually it will be ZOSApp_Mac::sRunEventLoop,
which is what actually calls GetNextEvent and thus when we switch out to the Finder it's from the
real main, and our memory usage looks more sensible.

On MacOS X this is not an issue. But we still present a similar architecture to the application.
In this situation we simply invoke ZMain from main(), and ZMainNS::sInvokeAsMainThread spawns the
new thread.
*/

static void (*sMainThreadProc)(void*);
static void* sMainThreadParam;
static bool sMainFinished;
static bool sPseudoMainThread_Started;
static bool sPseudoMainThread_Finished;
static ZMutexNR sMutex_MainThread;
static ZCondition sCondition_MainThread;

static void sInvokePseudoMainThread()
	{
	try
		{
		sMainThreadProc(sMainThreadParam);
		}
	catch (...)
		{}

	// Let main() know that we're finished.
	sMutex_MainThread.Acquire();
	sPseudoMainThread_Finished = true;
	sCondition_MainThread.Broadcast();
	sMutex_MainThread.Release();
	}

void ZMainNS::sInvokeAsMainThread(void (*iProc)(void*), void* iParam)
	{
	sMutex_MainThread.Acquire();
	// Stuff our globals so that main or sInvokePseudoMainThread know
	// what function to invoke.
	sMainThreadProc = iProc;
	sMainThreadParam = iParam;
	if (ZMacOSX::sIsMacOSX())
		{
		// We're running on MacOSX.
		sPseudoMainThread_Started = true;
		// Spawn a new thread to execute iProc.
		(new ZThreadSimple<>(sInvokePseudoMainThread))->Start();
		}
	else
		{
		// We're not running on MacOSX.
		// Broadcast sCondition_MainThread to wake main.
		sCondition_MainThread.Broadcast();
		}
	sMutex_MainThread.Release();
	}

struct ZMainArgs_t
	{
	int fArgc;
	char** fArgv;
	int fResult;
	};

static void sInvokeMain(ZMainArgs_t& ioStruct)
	{
	ioStruct.fResult = sMain(ioStruct.fArgc, ioStruct.fArgv);

	sMutex_MainThread.Acquire();
	sMainFinished = true;
	sCondition_MainThread.Broadcast();
	sMutex_MainThread.Release();
	}

extern "C"int main(int argc, char **argv)
	{
	if (ZMacOSX::sIsMacOSX())
		{
		// Invoke sMain, which will in turn safely invoke ZMain.
		int result = sMain(argc, argv);

		// If ZMainNS::sInvokeAsMainThread was called, wait till the
		// proc in question has returned.
		sMutex_MainThread.Acquire();
		while (sPseudoMainThread_Started && !sPseudoMainThread_Finished)
			sCondition_MainThread.Wait(sMutex_MainThread);
		sMutex_MainThread.Release();
		return result;
		}
	else
		{
		ZMainArgs_t theStruct;
		theStruct.fArgc = argc;
		theStruct.fArgv = argv;

		// Fire off a new thread in which ZMain will be invoked.
		(new ZThreadSimple<ZMainArgs_t&>(sInvokeMain, theStruct))->Start();

		sMutex_MainThread.Acquire();
		while (true)
			{
			if (sMainFinished)
				{
				// sMain has returned. We can bail.
				break;
				}
			else if (sMainThreadProc)
				{
				// Something has called sInvokeAsMainThread.
				void (*theProc)(void*) = sMainThreadProc;
				void* theParam = sMainThreadParam;
				sMainThreadProc = nil;
				sMainThreadParam = nil;
				sMutex_MainThread.Release();
				theProc(theParam);
				sMutex_MainThread.Acquire();
				}
			else
				{
				// Nothing interesting has happened.
				sCondition_MainThread.Wait(sMutex_MainThread);
				}
			}
		sMutex_MainThread.Release();

		return theStruct.fResult;
		}
	}
 
// =================================================================================================

#elif ZCONFIG_SPI_Enabled(Win)

#include "zoolib/ZWinHeader.h"

#if ZCONFIG(Compiler, CodeWarrior) || ZCONFIG(Compiler, MSVC) || defined(__MINGW32__)
// MSL and the Visual C++ runtime put argc and argv into the globals __argc and __argv.
// (see setupargs() in the MSL mslcrt runtime library).

extern "C" int __argc;
extern "C" char** __argv;

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine,int nShowCmd)
	{
	// There are several places where we need not only to be running on Win95 or later, but also
	// require that our executable is *marked* as needing to run on Win95 or later, so that the
	// host OS knows to send us messages that we're expecting to receive. This assertion will
	// trip if our executable is marked as needing an OS version below 4.0. This is not detectable
	// by ZooLib at compile or link time, so we have to check it at runtime. It's a considered a
	// fatal error and can be corrected in CodeWarrior by ensuring that the SubSystem ID field of
	// the x86 Linker panel contains 4.00 (or greater).
	DWORD theProcessVersion = ::GetProcessVersion(0);
	ZAssertStopf(0, theProcessVersion >= 0x00040000,
		("theProcessVersion == 0x08x", theProcessVersion));
	return sMain(__argc, __argv);
	}

// This entry point is used when we're building as a console application.
extern "C" int main(int argc, char** argv)
	{
	return sMain(argc, argv);
	}

#else // ZCONFIG_Compiler

#error Unsupported compiler

#endif // ZCONFIG_Compiler

// =================================================================================================

#else

extern "C"int main(int argc, char **argv)
	{
	return sMain(argc, argv);
	}


// =================================================================================================

#endif

// =================================================================================================

#if ZCONFIG_SPI_Enabled(POSIX)
#	include <cstdlib>
#	include <unistd.h>
#	include <fcntl.h>
#endif

void ZMainNS::sDaemonize(bool iForceFDClose)
	{
	#if ZCONFIG_SPI_Enabled(POSIX)
		switch (::fork())
			{
			case -1:
				{
				ZDebugStopf(0,("cannot fork"));
				break;
				}
			case 0:
				{
				// stdin
				if (iForceFDClose || ::isatty(0))
					{
					close(0);
					::open("/dev/null",O_RDONLY);
					}
				// stdout
				if (iForceFDClose || ::isatty(1))
					{
					close(1);
					::open("/dev/null",O_WRONLY);
					}
				// stderr
				if (iForceFDClose || ::isatty(2))
					{
					close(2);
					::open("/dev/null",O_WRONLY);
					}
				// Previously we'd used setpgrp. Changed per Chris Teplov's suggestion.
				::setsid();
				break;
				}
			default:
				{
				exit(0);
				}
			}
	#endif // ZCONFIG_SPI_Enabled(POSIX)
	}
