/* -------------------------------------------------------------------------------------------------
Copyright (c) 2010 Andrew Green
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

#include "zoolib/ZDelegate.h"

// =================================================================================================
#pragma mark -
#pragma mark * ZooLib_ZDelegate_Proxy

@interface ZooLib_ZDelegate_Proxy : NSObject
	{
@public
	ZooLib::ZDelegate* fDelegate;
	};
@end

@implementation ZooLib_ZDelegate_Proxy

- (BOOL)respondsToSelector:(SEL)aSelector
	{
	if (fDelegate)
		return fDelegate->pRespondsToSelector(aSelector);
	return false;
	}

- (void)forwardInvocation:(NSInvocation *)anInvocation
	{
	if (fDelegate)
		fDelegate->pForwardInvocation(anInvocation);
	}

- (NSMethodSignature *)methodSignatureForSelector:(SEL)aSelector;
	{
	if (fDelegate)
		return fDelegate->pMethodSignatureForSelector(aSelector);
	return nil;
	}

@end

namespace ZooLib {

using std::map;

// =================================================================================================
#pragma mark -
#pragma mark * ZDelegate

ZDelegate::ZDelegate()
	{
	fProxy = [[ZooLib_ZDelegate_Proxy alloc] init];
	fProxy->fDelegate = this;
	}

ZDelegate::~ZDelegate()
	{
	fProxy->fDelegate = nil;
	[fProxy release];
	}

ZDelegate::operator id()
	{ return fProxy; }

BOOL ZDelegate::pRespondsToSelector(SEL aSelector)
	{ return fWrappers.end() != fWrappers.find(aSelector); }

void ZDelegate::pForwardInvocation(NSInvocation* anInvocation)
	{
	map<SEL, Wrapper*>::iterator i = fWrappers.find([anInvocation selector]);
	if (fWrappers.end() != i)
		i->second->ForwardInvocation(anInvocation);
	}

NSMethodSignature* ZDelegate::pMethodSignatureForSelector(SEL aSelector)
	{
	map<SEL, Wrapper*>::iterator i = fWrappers.find(aSelector);
	if (fWrappers.end() != i)
		return i->second->MethodSignature();
	return nil;
	}

} // namespace ZooLib
