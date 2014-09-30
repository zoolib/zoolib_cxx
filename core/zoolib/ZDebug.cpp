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

#include "zoolib/size_t.h" // For size_t

#include "zoolib/ZCompat_Win.h"
#include "zoolib/ZCONFIG_SPI.h"
#include "zoolib/ZDebug.h"
#include "zoolib/ZFunctionChain.h"

#include <stdio.h>
#include <stdlib.h> // For abort
#include <string.h>

#if ZCONFIG(Compiler, MSVC)
	#define vsnprintf _vsnprintf
	#define snprintf _snprintf
#endif

#if defined(__ANDROID__)
	#include <android/log.h>
#endif

// =================================================================================================
// MARK: - ZDebug

namespace ZooLib {
namespace ZDebug {

using std::va_list;

extern void sInvoke(int iLevel, bool iStop,
	const ZFileFunctionLine& iFFL,
	const char* iConditionMessage, const char* iUserMessage, ...)
	{
	Params_t theParams;
	theParams.fLevel = iLevel;
	theParams.fStop = iStop;
	theParams.fFFL = iFFL;
	theParams.fConditionMessage = iConditionMessage;
	theParams.fUserMessage = iUserMessage;

	Function_t theFunction;
	if (ZFunctionChain_T<Function_t, const Params_t&>::sInvoke(theFunction, theParams))
		{
		va_list args;
		va_start(args, iUserMessage);
		theFunction(theParams, args);
		va_end(args);
		}
	}

static const char* spTruncateFileName(const char* inFilename)
	{
	if (const char* truncatedFileName = strrchr(inFilename, '/'))
		return truncatedFileName + 1;
	return inFilename;
	}

size_t sFormatStandardMessage(char* iBuf, int iBufSize, const Params_t& iParams)
	{
	if (iParams.fConditionMessage)
		{
		return snprintf(iBuf, iBufSize,
			"Assertion failed: (%s), in %s[%s:%d]",
			iParams.fConditionMessage,
			iParams.fFFL.fFunctionName_Long,
			spTruncateFileName(iParams.fFFL.fFileName),
			iParams.fFFL.fLine);
		}
	else
		{
		return snprintf(iBuf, iBufSize,
			"%s[%s:%d]",
			iParams.fFFL.fFunctionName_Long,
			spTruncateFileName(iParams.fFFL.fFileName),
			iParams.fFFL.fLine);
		}
	}

// =================================================================================================
// MARK: - POSIX

#if ZCONFIG_SPI_Enabled(POSIX)

static void spHandleDebug_POSIX(const Params_t& iParams, va_list iArgs)
	{
	char theBuf[4096];
	sFormatStandardMessage(theBuf, sizeof(theBuf), iParams);
	fputs("  *", stderr);
	::fputs(theBuf, stderr);
	::fputs("\n", stderr);
	fputs("  *", stderr);
	if (iParams.fUserMessage)
		::vfprintf(stderr, iParams.fUserMessage, iArgs);
	if (iParams.fStop)
		{
		::fputs("\n", stderr);
		// Force a segfault
		*reinterpret_cast<double*>(1) = 0;
//		abort();
		}
	else
		{
		::fputs("\n", stderr);
		}
	}

class DebugFunction_POSIX
:	public ZFunctionChain_T<Function_t, const Params_t&>
	{
public:
	DebugFunction_POSIX() : Base_t(false) {}

	virtual bool Invoke(Result_t& oResult, Param_t iParam)
		{
		oResult = spHandleDebug_POSIX;
		return true;
		}
	} sDebugFunction_POSIX;

#endif // ZCONFIG_SPI_Enabled(POSIX)
#if 0
// Mac IDP.

extern "C" {

bool IsDebuggerPresent()
	{
	int mib[4];
	struct kinfo_proc info;
	size_t size;

	info.kp_proc.p_flag = 0;
	mib[0] = CTL_KERN;
	mib[1] = KERN_PROC;
	mib[2] = KERN_PROC_PID;
	mib[3] = getpid();

	size = sizeof(info);
	sysctl(mib, sizeof(mib) / sizeof(*mib), &info, &size, NULL, 0);

	return ((info.kp_proc.p_flag & P_TRACED) != 0);
	}

void OutputDebugString(const char *restrict fmt, ...)
	{
	if (not IsDebuggerPresent())
		return;

	va_list args;
	va_start(args, fmt);
	vprintf(fmt, args);
	va_end(args);
	}
#endif

// =================================================================================================
// MARK: - Android

#if defined(__ANDROID__)

static void spHandleDebug_Android(const Params_t& iParams, va_list iArgs)
	{
	if (iParams.fUserMessage)
		__android_log_vprint(ANDROID_LOG_ERROR, "ZooLib", iParams.fUserMessage, iArgs);
	if (iParams.fStop)
		abort();
	}

class DebugFunction_Android
:	public ZFunctionChain_T<Function_t, const Params_t&>
	{
public:
	DebugFunction_Android() : Base_t(false) {}

	virtual bool Invoke(Result_t& oResult, Param_t iParam)
		{
		oResult = spHandleDebug_Android;
		return true;
		}
	} sDebugFunction_Android;

#endif // defined(__ANDROID__)

// =================================================================================================
// MARK: - Win

#if ZCONFIG_SPI_Enabled(Win)

#if ZCONFIG(Compiler, CodeWarrior)
	#define __asm asm
#endif

// spIsDebuggerPresent was taken from Whisper 1.3, which was adpated
// from code in Windows Developer Journal, March 1999.

static bool spIsDebuggerPresent()
	{
	#if ZCONFIG(Processor, x86) && !ZCONFIG(Compiler, GCC)

		if (HINSTANCE kernelH = LoadLibraryA("KERNEL32.DLL"))
			{
			// IsDebuggerPresent only exists in NT and Win 98, although its prototype
			// is in headers for Win 95 also. We must manually locate it in Kernel32 and
			// manually invoke it, if found.
			typedef BOOL (WINAPI *IsDebuggerPresentProc)();
			if (IsDebuggerPresentProc proc =
				(IsDebuggerPresentProc) ::GetProcAddress(kernelH, "IsDebuggerPresent"))
				{
				return proc() != 0;
				}
			}

			const uint32 kDebuggerPresentFlag = 0x000000001;
			const uint32 kProcessDatabaseBytes = 190;
			const uint32 kOffsetFlags = 8;

			uint32 threadID = GetCurrentThreadId();
			uint32 processID = GetCurrentProcessId();
			uint32 obfuscator = 0;

			__asm
				{
				mov	ax, fs
				mov	es, ax
				mov	eax, 0x18
				mov	eax, es:[eax]
				sub	eax, 0x10
				#ifdef xor
					#undef xor
				#endif
				xor eax, [threadID]
				mov	[obfuscator], eax
				}

			#pragma warning(disable: 4312)
			const uint32* processDatabase = reinterpret_cast<const uint32*>(processID ^ obfuscator);
			if (not IsBadReadPtr(processDatabase, kProcessDatabaseBytes))
				{
				uint32 flags = processDatabase[kOffsetFlags];
				return (flags & kDebuggerPresentFlag) != 0;
				}

			return false;

	#else

		return ::IsDebuggerPresent();

	#endif
	}

static void spHandleDebug_Win(const Params_t& iParams, va_list iArgs)
	{
	char theBuf[4096];
	size_t theLength = sFormatStandardMessage(theBuf, sizeof(theBuf), iParams);
	if (iParams.fUserMessage)
		::vsnprintf(theBuf + theLength, sizeof(theBuf) - theLength, iParams.fUserMessage, iArgs);
	if (iParams.fStop)
		{
		if (spIsDebuggerPresent())
			{
			::OutputDebugStringA(theBuf);
			#if ZCONFIG(Processor, x86) && !ZCONFIG(Compiler, GCC)
				__asm { int 3 }
			#else
				::DebugBreak();
			#endif
			}
		else
			{
			/*int result = */::MessageBoxA(nullptr, theBuf, "DebugBreak -- Application will exit",
				MB_SETFOREGROUND | MB_SYSTEMMODAL | MB_ICONHAND | MB_OK);

			::ExitProcess(0);
			}
		}
	}

class DebugFunction_Win
:	public ZFunctionChain_T<Function_t, const Params_t&>
	{
public:
	DebugFunction_Win() : Base_t(false) {}

	virtual bool Invoke(Result_t& oResult, Param_t iParam)
		{
		oResult = spHandleDebug_Win;
		return true;
		}
	} sDebugFunction_Win;

#endif // ZCONFIG_SPI_Enabled(POSIX)

} // namespace ZDebug
} // namespace ZooLib

// =================================================================================================
// MARK: - ZUnimplemented

namespace ZooLib {

// I'd like to formalize ZUnimplemented a little more sometime.
// Perhaps it should throw an exception in production code.

void ZUnimplemented()
	{
	ZDebugStop(0);
	abort();
	}

} // namespace ZooLib
