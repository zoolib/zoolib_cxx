// Copyright (c) 2014 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_UIKit_UIControl_Callable_h__
#define __ZooLib_UIKit_UIControl_Callable_h__ 1
#include "zconfig.h"

#import <UIKit/UIControl.h>

#include "zoolib/Callable.h"

// =================================================================================================
#pragma mark - UIControl (Callable)

@interface UIControl (Callable)

- (void)addCallable:(ZooLib::ZP<ZooLib::Callable_Void>)callable
	forControlEvents:(UIControlEvents)controlEvents;

@end // interface UIControl (Callable)

// =================================================================================================
#pragma mark - ZooLib

namespace ZooLib {

void sAddCallable(UIControl* iUIControl,
	ZP<Callable_Void> iCallable, UIControlEvents iControlEvents);

} // namespace ZooLib

#endif // __ZooLib_UIKit_UIControl_Callable_h__
