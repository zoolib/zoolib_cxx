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

#include "zoolib/ZUtil_Win.h"

#if ZCONFIG_SPI_Enabled(Win)

#include "zoolib/ZCompat_Win.h"

namespace ZooLib {

// From Whisper 1.3
static bool spIsWinNT_Inited;
static bool spIsWinNT_Result;
bool ZUtil_Win::sIsWinNT()
	{
	// We unconditionally use the 'A' API because we don't yet know
	// if we're on NT, which is what this method is trying to determine.
	// We could use the W entry point, and a failure used to indicate
	// that we know we're not on NT.
	if (!spIsWinNT_Inited)
		{
		OSVERSIONINFOA info;
		info.dwOSVersionInfoSize = sizeof(info);

		if (::GetVersionExA(&info))
			spIsWinNT_Result = info.dwPlatformId == VER_PLATFORM_WIN32_NT;

		spIsWinNT_Inited = true;
		}
	return spIsWinNT_Result;
	}

static bool spIsWin95OSR2_Inited;
static bool spIsWin95OSR2_Result;
bool ZUtil_Win::sIsWin95OSR2()
	{
	if (!spIsWin95OSR2_Inited)
		{
		OSVERSIONINFOA info;
		info.dwOSVersionInfoSize = sizeof(info);

		if (::GetVersionExA(&info))
			{
			if (info.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
				{
				if (info.dwMinorVersion >= 10)
					spIsWin95OSR2_Result = true;
				else if (LOWORD(info.dwBuildNumber) > 1080)
					spIsWin95OSR2_Result = true;
				}
			}

		spIsWin95OSR2_Inited = true;
		}
	return spIsWin95OSR2_Result;
	}

static bool spFlag_DisallowWAPI;
bool ZUtil_Win::sUseWAPI()
	{
	if (spFlag_DisallowWAPI)
		return false;

	return sIsWinNT();
	}

void ZUtil_Win::sDisallowWAPI()
	{ spFlag_DisallowWAPI = true; }

HINSTANCE ZUtil_Win::sGetModuleHandle()
	{
	#if ZCONFIG(Compiler, CodeWarrior)
		return ::GetModuleHandleW(nullptr);
	#else
		HMODULE theHINSTANCE;
		bool result = ::GetModuleHandleExW(
			GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT | GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
			reinterpret_cast<LPCWSTR>(sGetModuleHandle),
			&theHINSTANCE);
		return theHINSTANCE;
	#endif
	}

} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(Win)
