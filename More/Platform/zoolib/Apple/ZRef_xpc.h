/* -------------------------------------------------------------------------------------------------
Copyright (c) 2012 Andrew Green
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

#ifndef __ZRef_xpc_h__
#define __ZRef_xpc_h__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_SPI.h"
#include "zoolib/ZRef.h"

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
#pragma mark 

namespace ZooLib {

template <>
inline void sRetain_T(xpc_object_t& ioRef)
	{ ioRef = ::xpc_retain(ioRef); }

template <>
inline void sRelease_T(xpc_object_t iRef)
	{ ::xpc_release(iRef); }

} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(xpc)

#endif // __ZRef_xpc_h__
