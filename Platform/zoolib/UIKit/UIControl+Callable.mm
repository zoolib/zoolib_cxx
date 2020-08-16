// Copyright (c) 2014 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/UIKit/UIControl+Callable.h"

#include "objc/runtime.h"

using namespace ZooLib;

// =================================================================================================
#pragma mark - ZooLib_UIControl_Callable_Proxy

@interface ZooLib_UIControl_Callable_Proxy : NSObject
	{
@public
	ZP<Callable_Void> fCallable;
	};
@end // interface ZooLib_UIControl_Callable_Proxy

@implementation ZooLib_UIControl_Callable_Proxy

- (void)selector_void
	{ fCallable->QCall(); }

@end // implementation ZooLib_UIControl_Callable_Proxy

// =================================================================================================
#pragma mark - UIControl (Callable)

@implementation UIControl (Callable)

- (void)addCallable:(ZP<Callable_Void>)callable forControlEvents:(UIControlEvents)controlEvents
	{ sAddCallable(self, callable, controlEvents); }

@end // implementation UIControl (Callable)

// =================================================================================================
#pragma mark - ZooLib

namespace ZooLib {

void sAddCallable(UIControl* iUIControl,
	ZP<Callable_Void> iCallable, UIControlEvents iControlEvents)
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
