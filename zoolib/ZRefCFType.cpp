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

#include "zoolib/ZRefCFType.h"

#include "zoolib/ZCONFIG_SPI.h"

#if ZCONFIG_SPI_Enabled(CFType)

#include ZMACINCLUDE2(CoreFoundation,CFBase.h) // For CFRetain and CFRelease

#define ZOOLIB_RETAIN_RELEASE(a) \
NAMESPACE_ZOOLIB_BEGIN \
template<> void sRetain_T(a iRef) { if (iRef) ::CFRetain(iRef); } \
template<> void sRelease_T(a iRef) { if (iRef) ::CFRelease(iRef); } \
NAMESPACE_ZOOLIB_END

// =================================================================================================

#define ZOOLIB_CF_RETAIN_RELEASE(a) \
ZOOLIB_RETAIN_RELEASE(CF##a##Ref)

#define ZOOLIB_CF(a) \
typedef struct __CF##a * CF##a##Ref; \
ZOOLIB_CF_RETAIN_RELEASE(a)

#define ZOOLIB_CF_CONST(a) \
typedef const struct __CF##a * CF##a##Ref; \
ZOOLIB_CF_RETAIN_RELEASE(a)

#define ZOOLIB_CF_WITH_MUTABLE(a) \
typedef const struct __CF##a * CF##a##Ref; \
typedef struct __CF##a * CFMutable##a##Ref; \
ZOOLIB_CF_RETAIN_RELEASE(a) \
ZOOLIB_CF_RETAIN_RELEASE(Mutable##a)

// CFTypeRef declared in CFBase.h
ZOOLIB_CF_RETAIN_RELEASE(Type)

ZOOLIB_CF_WITH_MUTABLE(String)
ZOOLIB_CF_WITH_MUTABLE(Array)
ZOOLIB_CF_WITH_MUTABLE(Dictionary)
ZOOLIB_CF_WITH_MUTABLE(Number)
ZOOLIB_CF_WITH_MUTABLE(Data)
ZOOLIB_CF_WITH_MUTABLE(Boolean)

// Plugin and Bundle are usually the same type
//ZOOLIB_CF(PlugIn)
ZOOLIB_CF(Bundle)
ZOOLIB_CF(ReadStream)

ZOOLIB_CF_CONST(URL)

// =================================================================================================

#define ZOOLIB_CG_RETAIN_RELEASE(a) \
ZOOLIB_RETAIN_RELEASE(CG##a##Ref)

#define ZOOLIB_CG(a) \
typedef struct CG##a * CG##a##Ref; \
ZOOLIB_CG_RETAIN_RELEASE(a)

ZOOLIB_CG(Image)
ZOOLIB_CG(ColorSpace)
ZOOLIB_CG(Context)
ZOOLIB_CG(DataProvider)


#endif // ZCONFIG_SPI_Enabled(CFType)
