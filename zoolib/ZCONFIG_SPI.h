/* -------------------------------------------------------------------------------------------------
Copyright (c) 2005 Andrew Green and Learning in Motion, Inc.
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

#ifndef __ZCONFIG_SPI__
#define __ZCONFIG_SPI__ 1
#include "zconfig.h"

#include "zoolib/ZCONFIG_SPI_Definitions.h"

// Pick up Apple's config macros if they're available
#if __MACH__

#	include <AvailabilityMacros.h>
#	include <TargetConditionals.h>

#	if TARGET_IPHONE_SIMULATOR
#		define ZMACRO_IOS 1
#		define ZMACRO_IOS_Device 0
#		define ZMACRO_IOS_Simulator 1
#	elif TARGET_OS_IPHONE
#		define ZMACRO_IOS 1
#		define ZMACRO_IOS_Device 1
#		define ZMACRO_IOS_Simulator 0
#	endif

#endif

// Although it seems that this formulation should work in this header, as it
// does in application code:
//#define ZCONFIG_SPI_Avail__pthread (ZCONFIG_SPI_Enabled(POSIX))
// instead it is safer to do this:
//#define ZCONFIG_SPI_Avail__pthread (ZCONFIG_SPI_Avail__POSIX && ZCONFIG_SPI_Desired__POSIX)

// =================================================================================================
#pragma mark AppleEvent
#ifndef ZCONFIG_SPI_Avail__AppleEvent
#	define ZCONFIG_SPI_Avail__AppleEvent \
		(ZCONFIG_SPI_Avail__Carbon64 && ZCONFIG_SPI_Desired__Carbon64)
#endif

#ifndef ZCONFIG_SPI_Desired__AppleEvent
#	define ZCONFIG_SPI_Desired__AppleEvent 1
#endif


// =================================================================================================
#pragma mark BeOS
#ifndef ZCONFIG_SPI_Avail__BeOS
#	define ZCONFIG_SPI_Avail__BeOS 0
#endif

#ifndef ZCONFIG_SPI_Desired__BeOS
#	define ZCONFIG_SPI_Desired__BeOS 1
#endif


// =================================================================================================
#pragma mark BerkeleyDB
#ifndef ZCONFIG_SPI_Avail__BerkeleyDB
#	define ZCONFIG_SPI_Avail__BerkeleyDB (ZCONFIG_SPI_Avail__POSIX && ZCONFIG_SPI_Desired__POSIX)
#endif

#ifndef ZCONFIG_SPI_Avail__BerkeleyDB
#	define ZCONFIG_SPI_Avail__BerkeleyDB 0
#endif

#ifndef ZCONFIG_SPI_Desired__BerkeleyDB
#	define ZCONFIG_SPI_Desired__BerkeleyDB 1
#endif


// =================================================================================================
#pragma mark boost
#ifndef ZCONFIG_SPI_Avail__boost
#	define ZCONFIG_SPI_Avail__boost 0
#endif

#ifndef ZCONFIG_SPI_Desired__boost
#	define ZCONFIG_SPI_Desired__boost 1
#endif


// =================================================================================================
#pragma mark BSD
#ifndef ZCONFIG_SPI_Avail__BSD
#	if defined(__APPLE__) \
	|| defined(__NetBSD__) \
	|| defined(__FreeBSD__) \
	|| defined(__OpenBSD__)
#		define ZCONFIG_SPI_Avail__BSD 1
#	endif
#endif

#ifndef ZCONFIG_SPI_Avail__BSD
#	define ZCONFIG_SPI_Avail__BSD 0
#endif

#ifndef ZCONFIG_SPI_Desired__BSD
#	define ZCONFIG_SPI_Desired__BSD 1
#endif


// =================================================================================================
#pragma mark bzip2
#ifndef ZCONFIG_SPI_Avail__bzip2
#	define ZCONFIG_SPI_Avail__bzip2 0
#endif

#ifndef ZCONFIG_SPI_Desired__bzip2
#	define ZCONFIG_SPI_Desired__bzip2 1
#endif


// =================================================================================================
#pragma mark Carbon
#ifndef ZCONFIG_SPI_Avail__Carbon
#	if __MACH__
#		if ! ZMACRO_IOS && ! __LP64__
#			define ZCONFIG_SPI_Avail__Carbon 1
#		endif
#	elif macintosh
#		if TARGET_API_MAC_CARBON
#			define ZCONFIG_SPI_Avail__Carbon 1
#		elif TARGET_CARBON
#			define ZCONFIG_SPI_Avail__Carbon 1
#		endif
#	endif
#endif

#ifndef ZCONFIG_SPI_Avail__Carbon
#	define ZCONFIG_SPI_Avail__Carbon 0
#endif


#ifndef ZCONFIG_SPI_Desired__Carbon
#	define ZCONFIG_SPI_Desired__Carbon 1
#endif


// =================================================================================================
#pragma mark Carbon64
#ifndef ZCONFIG_SPI_Avail__Carbon64
#	if ZCONFIG_SPI_Avail__Carbon
#		define ZCONFIG_SPI_Avail__Carbon64 1
#	elif __MACH__
#		if ! ZMACRO_IOS
#			define ZCONFIG_SPI_Avail__Carbon64 1
#		endif
#	endif
#endif

#ifndef ZCONFIG_SPI_Avail__Carbon64
#	define ZCONFIG_SPI_Avail__Carbon64 0
#endif


#ifndef ZCONFIG_SPI_Desired__Carbon64
#	define ZCONFIG_SPI_Desired__Carbon64 1
#endif


// =================================================================================================
#pragma mark CFType
#ifndef ZCONFIG_SPI_Avail__CFType
#	if __MACH__
#		define ZCONFIG_SPI_Avail__CFType 1
#	elif ZCONFIG_SPI_Avail__Carbon64
#		define ZCONFIG_SPI_Avail__CFType 1
#	endif
#endif

#ifndef ZCONFIG_SPI_Avail__CFType
#	define ZCONFIG_SPI_Avail__CFType 0
#endif

#ifndef ZCONFIG_SPI_Desired__CFType
#	define ZCONFIG_SPI_Desired__CFType 1
#endif


// =================================================================================================
#pragma mark Cocoa
#ifndef ZCONFIG_SPI_Avail__Cocoa
#	if __MACH__
#		define ZCONFIG_SPI_Avail__Cocoa 1
#	elif __COCOTRON__
#		define ZCONFIG_SPI_Avail__Cocoa 1
#	endif
#endif

#ifndef ZCONFIG_SPI_Avail__Cocoa
#	define ZCONFIG_SPI_Avail__Cocoa 0
#endif

#ifndef ZCONFIG_SPI_Desired__Cocoa
#	define ZCONFIG_SPI_Desired__Cocoa 1
#endif


// =================================================================================================
#pragma mark CocoaFoundation
#ifndef ZCONFIG_SPI_Avail__CocoaFoundation
#	if __MACH__
#		define ZCONFIG_SPI_Avail__CocoaFoundation 1
#	elif __COCOTRON__
#		define ZCONFIG_SPI_Avail__CocoaFoundation 1
#	endif
#endif

#ifndef ZCONFIG_SPI_Avail__CocoaFoundation
#	define ZCONFIG_SPI_Avail__CocoaFoundation 0
#endif

#ifndef ZCONFIG_SPI_Desired__CocoaFoundation
#	define ZCONFIG_SPI_Desired__CocoaFoundation 1
#endif


// =================================================================================================
#pragma mark CocoaAppKit
#ifndef ZCONFIG_SPI_Avail__CocoaAppKit
#	if __MACH__
#		define ZCONFIG_SPI_Avail__CocoaAppKit (!ZMACRO_IOS)
#	elif __COCOTRON__
#		define ZCONFIG_SPI_Avail__CocoaAppKit 1
#	endif
#endif

#ifndef ZCONFIG_SPI_Avail__CocoaAppKit
#	define ZCONFIG_SPI_Avail__CocoaAppKit 0
#endif

#ifndef ZCONFIG_SPI_Desired__CocoaAppKit
#	define ZCONFIG_SPI_Desired__CocoaAppKit 1
#endif


// =================================================================================================
#pragma mark CoreFoundation
#ifndef ZCONFIG_SPI_Avail__CoreFoundation
#	if __MACH__
#		define ZCONFIG_SPI_Avail__CoreFoundation 1
#	elif ZCONFIG_SPI_Avail__Carbon64
#		define ZCONFIG_SPI_Avail__CoreFoundation 1
#	endif
#endif

#ifndef ZCONFIG_SPI_Avail__CoreFoundation
#	define ZCONFIG_SPI_Avail__CoreFoundation 0
#endif

#ifndef ZCONFIG_SPI_Desired__CoreFoundation
#	define ZCONFIG_SPI_Desired__CoreFoundation 1
#endif


// =================================================================================================
#pragma mark CoreGraphics
#ifndef ZCONFIG_SPI_Avail__CoreGraphics
#	if __MACH__
#		define ZCONFIG_SPI_Avail__CoreGraphics 1
#	endif
#endif

#ifndef ZCONFIG_SPI_Avail__CoreGraphics
#	define ZCONFIG_SPI_Avail__CoreGraphics 0
#endif

#ifndef ZCONFIG_SPI_Desired__CoreGraphics
#	define ZCONFIG_SPI_Desired__CoreGraphics 1
#endif


// =================================================================================================
#pragma mark DNSSD (aka ZeroConf)
#ifndef ZCONFIG_SPI_Avail__DNSSD
// Not sure about this one. Assumes POSIX for now, may need to drop back
// to MacOSX and Linux only (by default).
#	define ZCONFIG_SPI_Avail__DNSSD (ZCONFIG_SPI_Avail__POSIX && ZCONFIG_SPI_Desired__POSIX)
#endif

#ifndef ZCONFIG_SPI_Avail__DNSSD
#	define ZCONFIG_SPI_Avail__DNSSD 0
#endif

#ifndef ZCONFIG_SPI_Desired__DNSSD
#	define ZCONFIG_SPI_Desired__DNSSD 1
#endif


// =================================================================================================
#pragma mark GDI
#ifndef ZCONFIG_SPI_Avail__GDI
#	define ZCONFIG_SPI_Avail__GDI (ZCONFIG_SPI_Avail__Win && ZCONFIG_SPI_Desired__Win)
#endif

#ifndef ZCONFIG_SPI_Avail__GDI
#	define ZCONFIG_SPI_Avail__GDI 0
#endif

#ifndef ZCONFIG_SPI_Desired__GDI
#	define ZCONFIG_SPI_Desired__GDI 1
#endif

// =================================================================================================
#pragma mark iconv
#ifndef ZCONFIG_SPI_Avail__iconv
#	define ZCONFIG_SPI_Avail__iconv \
		((ZCONFIG_SPI_Avail__MacOSX && ZCONFIG_SPI_Desired__MacOSX) || ZMACRO_IOS)
#endif

#ifndef ZCONFIG_SPI_Desired__iconv
#	define ZCONFIG_SPI_Desired__iconv 1
#endif


// =================================================================================================
#pragma mark ICU
#ifndef ZCONFIG_SPI_Avail__ICU
#	define ZCONFIG_SPI_Avail__ICU 0
#endif

#ifndef ZCONFIG_SPI_Desired__ICU
#	define ZCONFIG_SPI_Desired__ICU 1
#endif


// =================================================================================================
#pragma mark imagehlp
#ifndef ZCONFIG_SPI_Avail__imagehlp
#	define ZCONFIG_SPI_Avail__imagehlp 0
#endif

#ifndef ZCONFIG_SPI_Desired__imagehlp
#	define ZCONFIG_SPI_Desired__imagehlp 1
#endif


// =================================================================================================
#pragma mark iPhone

#ifndef ZCONFIG_SPI_Avail__iPhone
#	if ZMACRO_IOS
#		define ZCONFIG_SPI_Avail__iPhone 1
#	endif
#endif

#ifndef ZCONFIG_SPI_Avail__iPhone
#	define ZCONFIG_SPI_Avail__iPhone 0
#endif

#ifndef ZCONFIG_SPI_Desired__iPhone
#	define ZCONFIG_SPI_Desired__iPhone 1
#endif


// =================================================================================================
#pragma mark Linux
#ifndef ZCONFIG_SPI_Avail__Linux
#	ifdef linux
#		define ZCONFIG_SPI_Avail__Linux 1
#	endif
#endif

#ifndef ZCONFIG_SPI_Avail__Linux
#	define ZCONFIG_SPI_Avail__Linux 0
#endif

#ifndef ZCONFIG_SPI_Desired__Linux
#	define ZCONFIG_SPI_Desired__Linux 1
#endif


// =================================================================================================
#pragma mark MacClassic
#ifndef ZCONFIG_SPI_Avail__MacClassic
#	if macintosh && ! ZCONFIG_SPI_Avail__Carbon
#		define ZCONFIG_SPI_Avail__MacClassic 1
#	endif
#endif

#ifndef ZCONFIG_SPI_Avail__MacClassic
#	define ZCONFIG_SPI_Avail__MacClassic 0
#endif

#ifndef ZCONFIG_SPI_Desired__MacClassic
#	define ZCONFIG_SPI_Desired__MacClassic 1
#endif


// =================================================================================================
#pragma mark MacMP
#ifndef ZCONFIG_SPI_Avail__MacMP
#	if macintosh || ZCONFIG_SPI_Avail__Carbon64
#		define ZCONFIG_SPI_Avail__MacMP 1
#	endif
#endif

#ifndef ZCONFIG_SPI_Avail__MacMP
#	define ZCONFIG_SPI_Avail__MacMP 0
#endif

#ifndef ZCONFIG_SPI_Desired__MacMP
#	define ZCONFIG_SPI_Desired__MacMP 1
#endif


// =================================================================================================
#pragma mark MacOSX
#ifndef ZCONFIG_SPI_Avail__MacOSX
#	if __MACH__ && ! ZMACRO_IOS
#		define ZCONFIG_SPI_Avail__MacOSX 1
#	endif
#endif

#ifndef ZCONFIG_SPI_Avail__MacOSX
#	define ZCONFIG_SPI_Avail__MacOSX 0
#endif

#ifndef ZCONFIG_SPI_Desired__MacOSX
#	define ZCONFIG_SPI_Desired__MacOSX 1
#endif


// =================================================================================================
#pragma mark openssl
#ifndef ZCONFIG_SPI_Avail__openssl
#	define ZCONFIG_SPI_Avail__openssl 0
#endif

#ifndef ZCONFIG_SPI_Desired__openssl
#	define ZCONFIG_SPI_Desired__openssl 1
#endif


// =================================================================================================
#pragma mark POSIX
#ifndef ZCONFIG_SPI_Avail__POSIX
#	if __MACH__
#		define ZCONFIG_SPI_Avail__POSIX 1
#	elif linux
#		define ZCONFIG_SPI_Avail__POSIX 1
#	endif
#endif

#ifndef ZCONFIG_SPI_Avail__POSIX
#	define ZCONFIG_SPI_Avail__POSIX 0
#endif

#ifndef ZCONFIG_SPI_Desired__POSIX
#	define ZCONFIG_SPI_Desired__POSIX 1
#endif


// =================================================================================================
#pragma mark pthread
#ifndef ZCONFIG_SPI_Avail__pthread
#	define ZCONFIG_SPI_Avail__pthread (ZCONFIG_SPI_Avail__POSIX && ZCONFIG_SPI_Desired__POSIX)
#endif

#ifndef ZCONFIG_SPI_Avail__pthread
#	define ZCONFIG_SPI_Avail__pthread 0
#endif

#ifndef ZCONFIG_SPI_Desired__pthread
#	define ZCONFIG_SPI_Desired__pthread 1
#endif


// =================================================================================================
#pragma mark QuickDraw
#ifndef ZCONFIG_SPI_Avail__QuickDraw
#	define ZCONFIG_SPI_Avail__QuickDraw \
		((ZCONFIG_SPI_Avail__Carbon && ZCONFIG_SPI_Desired__Carbon) \
		|| (ZCONFIG_SPI_Avail__MacClassic && ZCONFIG_SPI_Desired__MacClassic))
#endif

#ifndef ZCONFIG_SPI_Avail__QuickDraw
#	define ZCONFIG_SPI_Avail__QuickDraw 0
#endif

#ifndef ZCONFIG_SPI_Desired__QuickDraw
#	define ZCONFIG_SPI_Desired__QuickDraw 1
#endif


// =================================================================================================
#pragma mark stlsoft
#ifndef ZCONFIG_SPI_Avail__stlsoft
#	define ZCONFIG_SPI_Avail__stlsoft 0
#endif

#ifndef ZCONFIG_SPI_Desired__stlsoft
#	define ZCONFIG_SPI_Desired__stlsoft 1
#endif


// =================================================================================================
#pragma mark SystemConfiguration
#ifndef ZCONFIG_SPI_Avail__SystemConfiguration
#	define ZCONFIG_SPI_Avail__SystemConfiguration ZCONFIG_SPI_Avail__MacOSX
#endif

#ifndef ZCONFIG_SPI_Avail__SystemConfiguration
#	define ZCONFIG_SPI_Avail__SystemConfiguration 0
#endif

#ifndef ZCONFIG_SPI_Desired__SystemConfiguration
#	define ZCONFIG_SPI_Desired__SystemConfiguration 1
#endif


// =================================================================================================
#pragma mark Win
#ifndef ZCONFIG_SPI_Avail__Win
#	if defined(WIN32) || defined(_WIN32) || defined(WINDOWS)
#		define ZCONFIG_SPI_Avail__Win 1
#	elif __MWERKS__ && __INTEL__
#		define ZCONFIG_SPI_Avail__Win 1
#	endif
#endif

#ifndef ZCONFIG_SPI_Avail__Win
#	define ZCONFIG_SPI_Avail__Win 0
#endif

#ifndef ZCONFIG_SPI_Desired__Win
#	define ZCONFIG_SPI_Desired__Win 1
#endif


// =================================================================================================
#pragma mark X11
//#ifndef ZCONFIG_SPI_Avail__X11
//#	define ZCONFIG_SPI_Avail__X11 (ZCONFIG_SPI_Avail__POSIX && ZCONFIG_SPI_Desired__POSIX)
//#endif

#ifndef ZCONFIG_SPI_Avail__X11
#	define ZCONFIG_SPI_Avail__X11 0
#endif

#ifndef ZCONFIG_SPI_Desired__X11
#	define ZCONFIG_SPI_Desired__X11 1
#endif


// =================================================================================================
#pragma mark zlib
#ifndef ZCONFIG_SPI_Avail__zlib
#	define ZCONFIG_SPI_Avail__zlib 0
#endif

#ifndef ZCONFIG_SPI_Desired__zlib
#	define ZCONFIG_SPI_Desired__zlib 1
#endif


#endif // __ZCONFIG_SPI__
