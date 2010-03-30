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

#include "zoolib/ZCompat_Win.h"
#include "zoolib/ZDebug.h"
#include "zoolib/ZFunctionChain.h"
#include "zoolib/ZCONFIG_SPI.h"
#include "zoolib/ZTypes.h"

#include <stdio.h>
#include <string.h>

#if ZCONFIG(Compiler, MSVC)
#	define vsnprintf _vsnprintf
#	define snprintf _snprintf
#endif

// =================================================================================================
#pragma mark -
#pragma mark * ZDebug

NAMESPACE_ZOOLIB_BEGIN

namespace ZDebug {

Function_t thing;

extern void sInvoke(int iLevel, bool iStop,
	const char* iFileName, const char* iFunctionName, int iLine,
	const char* iConditionMessage, const char* iUserMessage, ...)
	{
	Params_t theParams;
	theParams.fLevel = iLevel;
	theParams.fStop = iStop;
	theParams.fFileName = iFileName;
	theParams.fFunctionName = iFunctionName;
	theParams.fLine = iLine;
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
			iParams.fFunctionName,
			spTruncateFileName(iParams.fFileName),
			iParams.fLine);
		}
	else
		{
		return snprintf(iBuf, iBufSize,
			"%s[%s:%d]",
			iParams.fFunctionName,
			spTruncateFileName(iParams.fFileName),
			iParams.fLine);
		}
	}

// =================================================================================================
#pragma mark -
#pragma mark * POSIX

#if ZCONFIG_SPI_Enabled(POSIX)

static void spHandleDebug_POSIX(const Params_t& iParams, va_list iArgs)
	{
	char theBuf[4096];
	sFormatStandardMessage(theBuf, sizeof(theBuf), iParams);
	fputs("  *", stderr);
	::fputs(theBuf, stderr);
	::fputs("\n", stderr);
	fputs("  *", stderr);
	::vfprintf(stderr, iParams.fUserMessage, iArgs);
	::fputs("\n", stderr);
	if (iParams.fStop)
		{
		// Force a segfault
		*reinterpret_cast<double*>(1) = 0;
//		abort();
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

bool IsDebuggerPresent() {
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

void OutputDebugString(const char *restrict fmt, ...) {
    if( !IsDebuggerPresent() )
        return;

    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
}
#endif
// =================================================================================================
#pragma mark -
#pragma mark * Win

#if ZCONFIG_SPI_Enabled(Win)

#if ZCONFIG(Compiler, CodeWarrior)
#	define __asm asm
#endif

// sIsDebuggerPresent was taken from Whisper 1.3, which was adpated
// from code in Windows Developer Journal, March 1999.

static bool sIsDebuggerPresent()
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
				xor	eax, [threadID]
				mov	[obfuscator], eax
				}

			const uint32* processDatabase = reinterpret_cast<const uint32*>(processID ^ obfuscator);
			if (!IsBadReadPtr(processDatabase, kProcessDatabaseBytes)) 
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
		if (sIsDebuggerPresent())
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
			int result = ::MessageBoxA(nullptr, theBuf, "DebugBreak -- Application will exit",
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

NAMESPACE_ZOOLIB_END

