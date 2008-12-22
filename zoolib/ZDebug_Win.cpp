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

#include "zoolib/ZDebug_Win.h"

#include "zoolib/ZCONFIG_SPI.h"

// =================================================================================================

#if ZCONFIG_SPI_Enabled(Win)

#include <cstdarg>
#include <cstdio>

#include "zoolib/ZTypes.h"
#include "zoolib/ZWinHeader.h"


#if ZCONFIG(Compiler, MSVC)
#	define asm __asm
#endif

// ==================================================

// Forward declaration of helper function.
static bool sIsDebuggerPresent();
static void sDoOther(const char* inMessage);

// ==================================================

static void ZDebug_HandleActual_Win(int inLevel, ZDebug_Action inAction, const char* inMessage)
	{
	::OutputDebugStringA(inMessage);

	if (inAction == eDebug_ActionStop)
		{
		if (::sIsDebuggerPresent())
			{
			// We invoke the debugger using int 3 so the debugger breaks right here
			// rather than inside DebugBreak.
			#if ZCONFIG(Compiler, GCC)
				::DebugBreak();
			#else
				asm { int 3 }
			#endif
			}
		else
			{
			// sDoOther will call ExitProcess
			::sDoOther(inMessage);
			}
		}
	}

// ==================================================

static void sDoOther(const char* inMessage)
	{
	int result = ::MessageBoxA(nil, inMessage, "DebugBreak -- Application will exit", MB_SETFOREGROUND | MB_SYSTEMMODAL | MB_ICONHAND | MB_OK);
	::ExitProcess(0);
	}

// ==================================================
// sIsDebuggerPresent was taken from Whisper 1.3, which was adpated from code in
// Windows Developer Journal, March 1999.

static bool sIsDebuggerPresent()
	{
	if (HINSTANCE kernelH = LoadLibraryA("KERNEL32.DLL"))
		{
		// IsDebuggerPresent only exists in NT and Win 98, although its prototype
		// is in headers for Win 95 also. We must manually locate it in Kernel32 and
		// manually invoke it, if found.
		typedef BOOL (WINAPI *IsDebuggerPresentProc)();
		if (IsDebuggerPresentProc proc = (IsDebuggerPresentProc) ::GetProcAddress(kernelH, "IsDebuggerPresent"))
			return proc() != 0;
		}
	#if ! ZCONFIG(Compiler, GCC)
		const uint32 kDebuggerPresentFlag = 0x000000001;
		const uint32 kProcessDatabaseBytes = 190;
		const uint32 kOffsetFlags = 8;

		uint32 threadID = GetCurrentThreadId();
		uint32 processID = GetCurrentProcessId();
		uint32 obfuscator = 0;

		asm
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
	#endif // ! ZCONFIG(Compiler, GCC)

	return false;
	}

#endif // ZCONFIG_SPI_Enabled(Win)
