// Copyright (c) 2008 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/ZThread.h"

#if ZCONFIG_SPI_Enabled(Win)
	#include "zoolib/ZCompat_Win.h" // For GetCurrentThreadId
#endif

#if __MACH__
	#include <mach/mach_init.h> // For mach_thread_self
#endif

namespace ZooLib {
namespace ZThread {

// =================================================================================================
#pragma mark - ZThread::sID

#if ZCONFIG_SPI_Enabled(Win)

ID sID() { return ::GetCurrentThreadId(); }

#else // ZCONFIG_SPI_Enabled(Win)

// Assume pthread for now

ID sID() { return ::pthread_self(); }

#endif // ZCONFIG_SPI_Enabled(Win)

// ================================================================================================
#pragma mark - ZThread_pthread::sSetName

#if ZCONFIG_SPI_Enabled(MacOSX)
	#if defined(MAC_OS_X_VERSION_MIN_REQUIRED)
		#if defined(MAC_OS_X_VERSION_10_6) && MAC_OS_X_VERSION_10_6 <= MAC_OS_X_VERSION_MIN_REQUIRED

			void sSetName(const char* iName)
				{
				char buffer[256];
				snprintf(buffer, sizeof(buffer), "%4x:%s", ((int)mach_thread_self()), iName);
				::pthread_setname_np(buffer);
				}

		#else

			extern int pthread_setname_np(const char*) __attribute__((weak_import));

			void sSetName(const char* iName)
				{
				if (pthread_setname_np
					&& 0 != ::pthread_setname_np(iName)
					&& 0 != pthread_setname_np("FailedToSetName"))
					{
					::pthread_setname_np("FTSN");
					}
				}

		#endif
	#endif

#elif ZCONFIG_SPI_Enabled(iPhone)

	void sSetName(const char* iName)
		{ ::pthread_setname_np(iName); }

#elif ! defined(__ANDROID_API__) || __ANDROID_API__ >= 16

	void sSetName(const char* iName)
		{
		if (not iName)
			iName = "NullName";

		if (0 != ::pthread_setname_np(::pthread_self(), iName)
			&& 0 != pthread_setname_np(::pthread_self(), "FailedToSetName"))
			{
			::pthread_setname_np(::pthread_self(), "FTSN");
			}
		}

#else

	void sSetName(const char* iName)
		{ /* no-op */ }

#endif

} // namespace ZThread
} // namespace ZooLib
