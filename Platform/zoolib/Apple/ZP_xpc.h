// Copyright (c) 2012 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Apple_ZP_xpc_h__
#define __ZooLib_Apple_ZP_xpc_h__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_SPI.h"
#include "zoolib/ZP.h"

#ifndef ZCONFIG_SPI_Avail__xpc
	#if (defined(MAC_OS_X_VERSION_MIN_REQUIRED) \
			&& MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_7)
		#define ZCONFIG_SPI_Avail__xpc 1
	#endif
#endif

#ifndef ZCONFIG_SPI_Avail__xpc
	#if (defined(__IPHONE_OS_VERSION_MIN_REQUIRED) \
			&& __IPHONE_OS_VERSION_MIN_REQUIRED >= __IPHONE_5_0)
		#define ZCONFIG_SPI_Avail__xpc 1
	#endif
#endif

#ifndef ZCONFIG_SPI_Avail__xpc
	#define ZCONFIG_SPI_Avail__xpc 0
#endif

#ifndef ZCONFIG_SPI_Desired__xpc
	#define ZCONFIG_SPI_Desired__xpc 1
#endif

#if ZCONFIG_SPI_Enabled(xpc)

#include <xpc/xpc.h>

// =================================================================================================
#pragma mark -

namespace ZooLib {

template <>
inline void sRetain_T(xpc_object_t& ioRef)
	{ ioRef = ::xpc_retain(ioRef); }

template <>
inline void sRelease_T(xpc_object_t iRef)
	{ ::xpc_release(iRef); }

} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(xpc)

#endif // __ZooLib_Apple_ZP_xpc_h__
