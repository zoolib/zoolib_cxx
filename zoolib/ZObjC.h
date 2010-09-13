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

#ifndef __ZObjC__
#define __ZObjC__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_SPI.h"

#if ZCONFIG_SPI_Enabled(Cocoa)

// The DESTROY macro from renaissance is useful. At some point, if we have
// renaissance available, then we'll pull the definition from there.
#ifndef DESTROY
#	ifdef __OBJC__
#		define DESTROY(a) do { if (a) {[a release]; a = 0; } } while(0)
#	else
#		define DESTROY(a) $ONLY_USE_IN_OBJECTIVE_C$
#	endif
#endif

#ifndef AUTORELEASE
#	ifdef __OBJC__
#		define AUTORELEASE(a) ([a autorelease])
#	else
#		define AUTORELEASE(a) $ONLY_USE_IN_OBJECTIVE_C$
#	endif
#endif

#ifdef __cplusplus

#ifdef __OBJC__
	@class NSAutoreleasePool;
#else
	typedef struct objc_NSAutoreleasePool NSAutoreleasePool;
#endif

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * ZAutoreleasePool

class ZAutoreleasePool
	{
public:
	ZAutoreleasePool();
	~ZAutoreleasePool();

private:
	NSAutoreleasePool* fPool;
	};

} // namespace ZooLib

#endif // __cplusplus

#endif // ZCONFIG_SPI_Enabled(Cocoa)

#endif // __ZObjC__
