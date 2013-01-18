/* -------------------------------------------------------------------------------------------------
Copyright (c) 2009 Andrew Green
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

#include "zoolib/ZRef_CF.h"

#if ZCONFIG_SPI_Enabled(CFType)

#include ZMACINCLUDE2(CoreFoundation,CFBase.h) // For CFRetain, CFRelease and CFTypeRef

// =================================================================================================

#define ZMACRO_Retain_Release(a) \
	namespace ZooLib { \
	template <> void sRetain_T(a& ioRef) { ::CFRetain(ioRef); } \
	template <> void sRelease_T(a iRef) { ::CFRelease(iRef); } \
	} // namespace ZooLib

// =================================================================================================

#define ZMACRO_Mutable(a) \
	typedef struct __##a * a##Ref; \
	ZMACRO_Retain_Release(a##Ref)

// =================================================================================================

#define ZMACRO_Const(a) \
	typedef const struct __##a * a##Ref; \
	ZMACRO_Retain_Release(a##Ref)

// =================================================================================================

#define ZMACRO_Opaque(a) \
	typedef struct Opaque##a##Ref * a##Ref; \
	ZMACRO_Retain_Release(a##Ref)

// =================================================================================================

// CFTypeRef declared in CFBase.h
ZMACRO_Retain_Release(CFTypeRef)

// =================================================================================================

#define ZMACRO_CF(a) \
	ZMACRO_Const(CF##a) \
	typedef struct __CF##a * CFMutable##a##Ref; \
	ZMACRO_Retain_Release(CFMutable##a##Ref)

ZMACRO_CF(Array)
ZMACRO_CF(Boolean)
ZMACRO_CF(Data)
ZMACRO_CF(Dictionary)
ZMACRO_CF(Number)
ZMACRO_CF(String)

// =================================================================================================

ZMACRO_Const(CFNull)
ZMACRO_Const(CFURL)
ZMACRO_Const(CFUUID)

// =================================================================================================

ZMACRO_Mutable(CFBundle)
ZMACRO_Mutable(CFError)
ZMACRO_Mutable(CFReadStream)
ZMACRO_Mutable(CFRunLoop)
ZMACRO_Mutable(CFRunLoopObserver)
ZMACRO_Mutable(CFRunLoopSource)
ZMACRO_Mutable(CFRunLoopTimer)
ZMACRO_Mutable(CFWriteStream)

ZMACRO_Mutable(CFUserNotification)

// =================================================================================================

#define ZMACRO_CG(a) \
	typedef struct a * a##Ref; \
	ZMACRO_Retain_Release(a##Ref)

ZMACRO_CG(CGColor)
ZMACRO_CG(CGColorSpace)
ZMACRO_CG(CGContext)
ZMACRO_CG(CGDataProvider)
ZMACRO_CG(CGGradient)
ZMACRO_CG(CGImage)

// =================================================================================================

ZMACRO_Mutable(CVBuffer)

// =================================================================================================

ZMACRO_Const(SCDynamicStore)
ZMACRO_Const(SCPreferences)
ZMACRO_Const(SCNetworkReachability)

// =================================================================================================

ZMACRO_Opaque(SecKeychain)
ZMACRO_Opaque(SecKeychainItem)
ZMACRO_Opaque(SecKeychainSearch)

// =================================================================================================

ZMACRO_Const(AXUIElement)

// =================================================================================================

ZMACRO_Opaque(Pasteboard)

#endif // ZCONFIG_SPI_Enabled(CFType)
