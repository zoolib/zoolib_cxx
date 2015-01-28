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

#include "zoolib/iOS/UIControl+Callable.h"

#include "objc/runtime.h"

using namespace ZooLib;

// =================================================================================================
#pragma mark -
#pragma mark ZooLib_UIControl_Callable_Proxy

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
#pragma mark -
#pragma mark UIControl (Callable)

@implementation UIControl (Callable)

- (void)addCallable:(ZRef<Callable_Void>)callable forControlEvents:(UIControlEvents)controlEvents
	{ sAddCallable(self, callable, controlEvents); }

@end // implementation UIControl (Callable)

// =================================================================================================
#pragma mark -
#pragma mark ZooLib

namespace ZooLib {

void sAddCallable(UIControl* iUIControl,
	ZRef<Callable_Void> iCallable, UIControlEvents iControlEvents)
	{
	if (not iCallable)
		return;

	ZooLib_UIControl_Callable_Proxy* theProxy = [[ZooLib_UIControl_Callable_Proxy alloc] init];
	theProxy->fCallable = iCallable;

	// Associate theProxy under its own address, which allows unlimited
	// callables to be safely attached and released when self is itelf disposed.
	objc_setAssociatedObject(iUIControl, theProxy, theProxy, OBJC_ASSOCIATION_RETAIN);

	[theProxy release];

	[iUIControl
		addTarget:theProxy
		action:@selector(selector_void)
		forControlEvents:iControlEvents
	];
	}

} // namespace ZooLib
