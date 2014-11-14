/* -------------------------------------------------------------------------------------------------
Copyright (c) 2014 Andrew Green
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

#include "zoolib/UIKit/UIControl+Callable.h"

#include "objc/runtime.h"

using namespace ZooLib;

// =================================================================================================
// MARK: - ZooLib_UIControl_Callable_Proxy

@interface ZooLib_UIControl_Callable_Proxy : NSObject
	{
@public
	ZRef<Callable_Void> fCallable;
	};
@end // interface ZooLib_UIControl_Callable_Proxy

@implementation ZooLib_UIControl_Callable_Proxy

- (void)selector_void
	{ fCallable->QCall(); }

@end // implementation ZooLib_UIControl_Callable_Proxy

// =================================================================================================
// MARK: - UIControl+Callable

// This is a neat way to get a unique void* for use as a key.
// static void* spKey = &spKey;

@implementation UIControl (Callable)

- (void)addCallable:(ZRef<Callable_Void>)callable forControlEvents:(UIControlEvents)controlEvents
	{
	if (not callable)
		return;

	ZooLib_UIControl_Callable_Proxy* theProxy = [[ZooLib_UIControl_Callable_Proxy alloc] init];
	theProxy->fCallable = callable;

	objc_setAssociatedObject(self, theProxy, theProxy, OBJC_ASSOCIATION_RETAIN);

	[theProxy release];

	[self
		addTarget:theProxy
		action:@selector(selector_void)
		forControlEvents:controlEvents];
	}

@end // implementation UIControl (Callable)
