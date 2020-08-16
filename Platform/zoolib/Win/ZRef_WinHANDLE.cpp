// Copyright (c) 2009 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/ZRef_WinHANDLE.h"
#include "zoolib/ZCONFIG_SPI.h"

#if ZCONFIG_SPI_Enabled(Win)

#include "zoolib/ZCompat_Win.h"

// =================================================================================================

#define ZMACRO_ZRef_WinHANDLE_Decl(HANDLE_t) \
	typedef struct HANDLE_t##__ * HANDLE_t; \
	namespace ZooLib { \
	template <> void sRetain_T(HANDLE_t& ioHANDLE) \
	{ ioHANDLE = static_cast<HANDLE_t>(spDuplicateHandle(ioHANDLE)); } \
	} /* namespace ZooLib */

#define ZMACRO_ZRef_WinHANDLE_WithReleaser(HANDLE_t, Releaser) \
	ZMACRO_ZRef_WinHANDLE_Decl(HANDLE_t) \
	namespace ZooLib { \
	template <> void sRelease_T(HANDLE_t iHANDLE) { Releaser(iHANDLE); } \
	} /* namespace ZooLib */

#define ZMACRO_ZRef_WinHANDLE_WithReleaserDecl(HANDLE_t, Qual, Releaser) \
	Qual WINAPI Releaser(HANDLE_t); \
	ZMACRO_ZRef_WinHANDLE_WithReleaser(HANDLE_t, Releaser)

#define ZMACRO_ZRef_WinHANDLE(HANDLE_t) \
	ZMACRO_ZRef_WinHANDLE_WithReleaser(HANDLE_t, CloseHandle)

// =================================================================================================

static HANDLE spDuplicateHandle(HANDLE iHANDLE)
	{
	if (iHANDLE)
		{
		HANDLE result;
		if (::DuplicateHandle(
			::GetCurrentProcess(), // hSourceProcessHandle
			iHANDLE, // hSourceHandle
			::GetCurrentProcess(), // hTargetProcessHandle(
			(LPHANDLE)&result, // lpTargetHandle
			0, // dwDesiredAccess
			false, // bInheritHandle
			DUPLICATE_SAME_ACCESS // dwOptions
			))
			{ return result; }
		}

	return iHANDLE;
	}

// =================================================================================================

ZMACRO_ZRef_WinHANDLE_WithReleaser(HKEY, RegCloseKey)

//ZMACRO_ZRef_WinHANDLE_WithReleaser(HINSTANCE, FreeLibrary)

//ZMACRO_ZRef_WinHANDLE_WithReleaserDecl(HKEY, WINADVAPI LONG, RegCloseKey)
//ZMACRO_ZRef_WinHANDLE_WithReleaserDecl(HINSTANCE, WINBASEAPI BOOL, FreeLibrary)

namespace ZooLib {

template <>
void sRetain_T(HANDLE& ioHANDLE)
	{
	if (ioHANDLE)
		ioHANDLE = spDuplicateHandle(ioHANDLE);
	}

template <>
void sRelease_T(HANDLE iHANDLE)
	{
	if (iHANDLE)
		::CloseHandle(iHANDLE);
	}

} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(Win)
