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

#include "zoolib/ZRef_CFType.h"

#if ZCONFIG_SPI_Enabled(CFType)

#include ZMACINCLUDE2(CoreFoundation,CFBase.h) // For CFRetain and CFRelease

// =================================================================================================

#define ZMACRO_RETAIN_RELEASE(a) \
	namespace ZooLib { \
	template <> void sRetain_T(a& ioRef) { if (ioRef) ::CFRetain(ioRef); } \
	template <> void sRelease_T(a iRef) { if (iRef) ::CFRelease(iRef); } \
	} // namespace ZooLib

// =================================================================================================
// CFTypeRef declared in CFBase.h
ZMACRO_RETAIN_RELEASE(CFTypeRef)

// =================================================================================================
#define ZMACRO_CF_MUTABLE(a) \
	typedef struct __CF##a * CF##a##Ref; \
	ZMACRO_RETAIN_RELEASE(CF##a##Ref)

ZMACRO_CF_MUTABLE(Bundle)
ZMACRO_CF_MUTABLE(Error)
ZMACRO_CF_MUTABLE(ReadStream)
ZMACRO_CF_MUTABLE(RunLoop)
ZMACRO_CF_MUTABLE(RunLoopTimer)
ZMACRO_CF_MUTABLE(WriteStream)

// =================================================================================================
#define ZMACRO_CF_CONST(a) \
	typedef const struct __CF##a * CF##a##Ref; \
	ZMACRO_RETAIN_RELEASE(CF##a##Ref)

ZMACRO_CF_CONST(URL)
ZMACRO_CF_CONST(Null)

// =================================================================================================
#define ZMACRO_CF(a) \
	typedef const struct __CF##a * CF##a##Ref; \
	typedef struct __CF##a * CFMutable##a##Ref; \
	ZMACRO_RETAIN_RELEASE(CF##a##Ref) \
	ZMACRO_RETAIN_RELEASE(CFMutable##a##Ref)

ZMACRO_CF(Array)
ZMACRO_CF(Boolean)
ZMACRO_CF(Data)
ZMACRO_CF(Dictionary)
ZMACRO_CF(Number)
ZMACRO_CF(String)

// =================================================================================================
#define ZMACRO_CG(a) \
	typedef struct CG##a * CG##a##Ref; \
	ZMACRO_RETAIN_RELEASE(CG##a##Ref)

ZMACRO_CG(Color)
ZMACRO_CG(ColorSpace)
ZMACRO_CG(Context)
ZMACRO_CG(DataProvider)
ZMACRO_CG(Gradient)
ZMACRO_CG(Image)

// =================================================================================================
typedef struct OpaqueSecKeychainRef *SecKeychainRef;
ZMACRO_RETAIN_RELEASE(SecKeychainRef)

typedef struct OpaqueSecKeychainItemRef *SecKeychainItemRef;
ZMACRO_RETAIN_RELEASE(SecKeychainItemRef)

typedef struct OpaqueSecKeychainSearchRef *SecKeychainSearchRef;
ZMACRO_RETAIN_RELEASE(SecKeychainSearchRef)

typedef const struct __SCDynamicStore *	SCDynamicStoreRef;
ZMACRO_RETAIN_RELEASE(SCDynamicStoreRef)

// =================================================================================================
typedef const struct __AXUIElement *AXUIElementRef;
ZMACRO_RETAIN_RELEASE(AXUIElementRef)

// =================================================================================================
typedef struct __CVBuffer *CVBufferRef;
ZMACRO_RETAIN_RELEASE(CVBufferRef)

// =================================================================================================
typedef struct OpaquePasteboardRef* PasteboardRef;
ZMACRO_RETAIN_RELEASE(PasteboardRef)

#endif // ZCONFIG_SPI_Enabled(CFType)
