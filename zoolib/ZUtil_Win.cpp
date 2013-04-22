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

#include <vector>

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
	if (not spIsWinNT_Inited)
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
	if (not spIsWin95OSR2_Inited)
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

static bool spIsVistaOrLater_Inited;
static bool spIsVistaOrLater_Result;
bool ZUtil_Win::sIsVistaOrLater()
	{
	if (not spIsVistaOrLater_Inited)
		{
		OSVERSIONINFOEX osvi = { 0 };
		osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
		osvi.dwMajorVersion = 6;
		DWORDLONG conditional = 0;
		VER_SET_CONDITION(conditional, VER_MAJORVERSION, VER_GREATER_EQUAL);
		spIsVistaOrLater_Result = true && ::VerifyVersionInfo(&osvi, VER_MAJORVERSION, conditional);
		spIsVistaOrLater_Inited = true;
		}
	return spIsVistaOrLater_Result;
	}

bool ZUtil_Win::sIsUserAdmin()
	{
	// Determine if the user is part of the adminstators group. This will return
	// true in case of XP and 2K if the user belongs to admin group. In case of
	// Vista, it only returns true if the admin is running elevated.
	SID_IDENTIFIER_AUTHORITY nt_authority = SECURITY_NT_AUTHORITY;
	PSID administrators_group = nullptr;
	BOOL result = ::AllocateAndInitializeSid
		(&nt_authority,
		2,
		SECURITY_BUILTIN_DOMAIN_RID,
		DOMAIN_ALIAS_RID_ADMINS,
		0, 0, 0, 0, 0, 0,
		&administrators_group);

	if (result)
		{
		if (not ::CheckTokenMembership(NULL, administrators_group, &result))
			result = false;
		::FreeSid(administrators_group);
		}

	return true && result;
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
		bool result = ::GetModuleHandleExW
			(GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT | GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
			reinterpret_cast<LPCWSTR>(sGetModuleHandle),
			&theHINSTANCE);
		return theHINSTANCE;
	#endif
	}

ZQ<uint64> ZUtil_Win::sQGetVersion_File(const WCHAR* iPath)
	{
	DWORD dummy;
	if (DWORD theSize = ::GetFileVersionInfoSizeW(iPath, &dummy))
		{
		std::vector<char> buffer(theSize);
		if (::GetFileVersionInfoW(iPath, 0, theSize, &buffer[0]))
			{
			VS_FIXEDFILEINFO* info;
			UINT infoSize;
			if (::VerQueryValueW(&buffer[0], const_cast<WCHAR*>(L"\\"), (void**)&info, &infoSize)
				&& infoSize >= sizeof(VS_FIXEDFILEINFO))
				{ return (uint64(info->dwFileVersionMS) << 32) | info->dwFileVersionLS; }
			}
		}
	return null;
	}

} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(Win)
