/* -------------------------------------------------------------------------------------------------
Copyright (c) 2008 Andrew Green
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

#ifndef __ZObjC_h__
#define __ZObjC_h__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_SPI.h"

#ifdef __OBJC__
	#include <objc/objc.h>
	#define ZMACRO_ObjCClass(p) @class p
#else
	typedef struct objc_class *Class;
	typedef struct objc_object { Class isa; } *id;
	typedef struct objc_selector *SEL;
	typedef id (*IMP)(id, SEL, ...);
	#define ZMACRO_ObjCClass(p) typedef struct objc_##p p
#endif

#if ZCONFIG_SPI_Enabled(Cocoa) && __cplusplus

ZMACRO_ObjCClass(NSAutoreleasePool);

namespace ZooLib {

// =================================================================================================
// MARK: - ZAutoreleasePool

class ZAutoreleasePool
	{
public:
	ZAutoreleasePool();
	~ZAutoreleasePool();

private:
	NSAutoreleasePool* fPool;
	};

} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(Cocoa) && __cplusplus

#endif // __ZObjC_h__
