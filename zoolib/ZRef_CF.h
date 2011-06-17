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

#ifndef __ZRef_CF__
#define __ZRef_CF__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_SPI.h"

#if ZCONFIG_SPI_Enabled(CFType)

#include "zoolib/ZRef.h"

typedef const void * CFTypeRef;

typedef const struct __CFNull * CFNullRef;

typedef const struct __CFArray * CFArrayRef;
typedef struct __CFArray * CFMutableArrayRef;

typedef const struct __CFData * CFDataRef;
typedef struct __CFData * CFMutableDataRef;

typedef const struct __CFDictionary * CFDictionaryRef;
typedef struct __CFDictionary * CFMutableDictionaryRef;

typedef const struct __CFString * CFStringRef;
typedef struct __CFString * CFMutableStringRef;

namespace ZooLib {

const struct TempCF_t
	{
	template <class T>
	ZRef<T*,true> operator&(T* iP) const { return ZRef<T*,true>(Adopt_T<T*>(iP)); }

	template <class T>
	ZRef<T*,true> operator()(T* iP) const { return ZRef<T*,true>(Adopt_T<T*>(iP)); }
	} TempCF = {};

} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(CFType)

#endif // __ZRef_CF__
