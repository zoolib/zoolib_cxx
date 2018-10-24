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

#include "zoolib/Apple/ZRef_NS.h"

#include "zoolib/ZDebug.h"

#if ZCONFIG_SPI_Enabled(CocoaFoundation)

#import <Foundation/NSObject.h>

// =================================================================================================
#pragma mark -
#pragma mark id and NSObject, sRetain and sRelease

void sRetain(struct objc_object& iOb)
	{
	#if ZCONFIG(Compiler,Clang) && __has_feature(objc_arc)
		CFRetain(&iOb);
	#else
		[((id)&iOb) retain];
	#endif
	}

void sRetain(NSObject& iNSObject)
	{
	#if ZCONFIG(Compiler,Clang) && __has_feature(objc_arc)
		CFRetain((__bridge CFTypeRef)&iNSObject);
	#else
		[&iNSObject retain];
	#endif
	}

void sRelease(struct objc_object& iOb)
	{
	#if ZCONFIG(Compiler,Clang) && __has_feature(objc_arc)
		CFRelease(&iOb);
	#else
		[((id)&iOb) release];
	#endif
	}

void sRelease(NSObject& iNSObject)
	{
	#if ZCONFIG(Compiler,Clang) && __has_feature(objc_arc)
		CFRelease((__bridge CFTypeRef)&iNSObject);
	#else
		[&iNSObject release];
	#endif
	}

void sCheck(struct objc_object* iP)
	{ ZAssertStop(1, iP); }

void sCheck(NSObject* iP)
	{ ZAssertStop(1, iP); }

int sRetainCount(struct objc_object* iP)
	{
	#if ZCONFIG(Compiler,Clang) && __has_feature(objc_arc)
		return (int)CFGetRetainCount(iP);
	#else
		return [((id)iP) retainCount];
	#endif
	}

int sRetainCount(NSObject* iP)
	{
	#if ZCONFIG(Compiler,Clang) && __has_feature(objc_arc)
		return (int)CFGetRetainCount((__bridge CFTypeRef)iP);
	#else
		return [iP retainCount];
	#endif
	}

#endif // ZCONFIG_SPI_Enabled(CocoaFoundation)
