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

#include ZMACINCLUDE2(CoreFoundation,CFBase.h) // For CFRetain and CFRelease

// =================================================================================================

#define ZMACRO_Retain_Release(a) \
	namespace ZooLib { \
	template <> void sRetain_T(a& ioRef) { if (ioRef) ::CFRetain(ioRef); } \
	template <> void sRelease_T(a iRef) { if (iRef) ::CFRelease(iRef); } \
	} // namespace ZooLib

// =================================================================================================
// CFTypeRef declared in CFBase.h
ZMACRO_Retain_Release(CFTypeRef)

// =================================================================================================
#define ZMACRO_CF_Mutable(a) \
	typedef struct __CF##a * CF##a##Ref; \
	ZMACRO_Retain_Release(CF##a##Ref)

ZMACRO_CF_Mutable(Bundle)
ZMACRO_CF_Mutable(Error)
ZMACRO_CF_Mutable(ReadStream)
ZMACRO_CF_Mutable(RunLoop)
ZMACRO_CF_Mutable(RunLoopSource)
ZMACRO_CF_Mutable(RunLoopTimer)
ZMACRO_CF_Mutable(WriteStream)

// =================================================================================================
#define ZMACRO_CF_Const(a) \
	typedef const struct __CF##a * CF##a##Ref; \
	ZMACRO_Retain_Release(CF##a##Ref)

ZMACRO_CF_Const(URL)
ZMACRO_CF_Const(Null)

// =================================================================================================
#define ZMACRO_CF(a) \
	typedef const struct __CF##a * CF##a##Ref; \
	typedef struct __CF##a * CFMutable##a##Ref; \
	ZMACRO_Retain_Release(CF##a##Ref) \
	ZMACRO_Retain_Release(CFMutable##a##Ref)

ZMACRO_CF(Array)
ZMACRO_CF(Boolean)
ZMACRO_CF(Data)
ZMACRO_CF(Dictionary)
ZMACRO_CF(Number)
ZMACRO_CF(String)

// =================================================================================================
#define ZMACRO_CG(a) \
	typedef struct CG##a * CG##a##Ref; \
	ZMACRO_Retain_Release(CG##a##Ref)

ZMACRO_CG(Color)
ZMACRO_CG(ColorSpace)
ZMACRO_CG(Context)
ZMACRO_CG(DataProvider)
ZMACRO_CG(Gradient)
ZMACRO_CG(Image)

// =================================================================================================
typedef struct OpaqueSecKeychainRef *SecKeychainRef;
ZMACRO_Retain_Release(SecKeychainRef)

typedef struct OpaqueSecKeychainItemRef *SecKeychainItemRef;
ZMACRO_Retain_Release(SecKeychainItemRef)

typedef struct OpaqueSecKeychainSearchRef *SecKeychainSearchRef;
ZMACRO_Retain_Release(SecKeychainSearchRef)

typedef const struct __SCDynamicStore *SCDynamicStoreRef;
ZMACRO_Retain_Release(SCDynamicStoreRef)

typedef const struct __SCPreferences *SCPreferencesRef;
ZMACRO_Retain_Release(SCPreferencesRef)

// =================================================================================================
typedef const struct __AXUIElement *AXUIElementRef;
ZMACRO_Retain_Release(AXUIElementRef)

// =================================================================================================
typedef struct __CVBuffer *CVBufferRef;
ZMACRO_Retain_Release(CVBufferRef)

// =================================================================================================
typedef struct OpaquePasteboardRef* PasteboardRef;
ZMACRO_Retain_Release(PasteboardRef)

// =================================================================================================
typedef struct __CFUserNotification * CFUserNotificationRef;
ZMACRO_Retain_Release(CFUserNotificationRef)

#endif // ZCONFIG_SPI_Enabled(CFType)
