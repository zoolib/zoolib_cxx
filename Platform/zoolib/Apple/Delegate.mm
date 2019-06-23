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

#include "zoolib/Apple/Delegate.h"
#include "zoolib/Util_STL_map.h"

#include <string>

#if ZCONFIG_SPI_Enabled(Cocoa)

#import <Foundation/NSInvocation.h>
#import <Foundation/NSMethodSignature.h>

using std::string;

// =================================================================================================
#pragma mark - Workaround

#if !defined(MAC_OS_X_VERSION_10_5) || (MAC_OS_X_VERSION_MAX_ALLOWED < MAC_OS_X_VERSION_10_5)

// signatureWithObjCTypes was not declared before 10.5.
@interface NSMethodSignature (objctypes)
+(NSMethodSignature*)signatureWithObjCTypes:(const char*)types;
@end

#endif // !defined(MAC_OS_X_VERSION_10_5) || (MAC_OS_X_VERSION_MAX_ALLOWED < MAC_OS_X_VERSION_10_5)

// =================================================================================================
#pragma mark - ZooLib_Delegate_Proxy

@interface ZooLib_Delegate_Proxy : NSObject
	{
@public
	ZooLib::Delegate* fDelegate;
	};
@end // interface ZooLib_Delegate_Proxy

@implementation ZooLib_Delegate_Proxy

- (BOOL)respondsToSelector:(SEL)aSelector
	{
	if (fDelegate)
		return fDelegate->pRespondsToSelector(aSelector);
	return false;
	}

- (void)forwardInvocation:(NSInvocation*)anInvocation
	{
	if (fDelegate)
		fDelegate->pForwardInvocation(anInvocation);
	}

- (NSMethodSignature*)methodSignatureForSelector:(SEL)aSelector;
	{
	if (fDelegate)
		return fDelegate->pMethodSignatureForSelector(aSelector);
	return nil;
	}

@end // implementation ZooLib_Delegate_Proxy

// =================================================================================================
#pragma mark - C++

namespace ZooLib {

using namespace Util_STL;

// =================================================================================================
#pragma mark - Delegate

Delegate::Delegate()
	{
	fProxy = [[ZooLib_Delegate_Proxy alloc] init];
	fProxy->fDelegate = this;
	}

Delegate::~Delegate()
	{
	fProxy->fDelegate = nil;
	#if !ZCONFIG(Compiler,Clang) || !__has_feature(objc_arc)
		[fProxy release];
	#endif
	}

Delegate::operator id()
	{ return fProxy; }

BOOL Delegate::pRespondsToSelector(SEL aSelector)
	{ return sContains(fWrappers, aSelector); }

void Delegate::pForwardInvocation(NSInvocation* anInvocation)
	{
	if (ZQ<ZP<Wrapper>> theQ = sQGet(fWrappers, [anInvocation selector]))
		(*theQ)->ForwardInvocation(anInvocation);
	}

NSMethodSignature* Delegate::pMethodSignatureForSelector(SEL aSelector)
	{
	if (ZQ<ZP<Wrapper>> theQ = sQGet(fWrappers, aSelector))
		return (*theQ)->fNSMethodSignature;
	return nil;
	}

// =================================================================================================
#pragma mark - Delegate::Wrapper

// Type code for receiver (an id) and SEL.
static const string kRcvrSEL = "@:";

void Delegate::Wrapper::pSetSignature(const char* R)
	{
	const string signature = R + kRcvrSEL;
	fNSMethodSignature = [NSMethodSignature signatureWithObjCTypes:signature.c_str()];
	}

void Delegate::Wrapper::pSetSignature(const char* R,
	const char* P0)
	{
	const string signature = R + kRcvrSEL + P0;
	fNSMethodSignature = [NSMethodSignature signatureWithObjCTypes:signature.c_str()];
	}

void Delegate::Wrapper::pSetSignature(const char* R,
	const char* P0, const char* P1)
	{
	const string signature = R + kRcvrSEL + P0 + P1;
	fNSMethodSignature = [NSMethodSignature signatureWithObjCTypes:signature.c_str()];
	}

void Delegate::Wrapper::pSetSignature(const char* R,
	const char* P0, const char* P1, const char* P2)
	{
	const string signature = R + kRcvrSEL + P0 + P1 + P2;
	fNSMethodSignature = [NSMethodSignature signatureWithObjCTypes:signature.c_str()];
	}

void Delegate::Wrapper::pSetSignature(const char* R,
	const char* P0, const char* P1, const char* P2, const char* P3)
	{
	const string signature = R + kRcvrSEL + P0 + P1 + P2 + P3;
	fNSMethodSignature = [NSMethodSignature signatureWithObjCTypes:signature.c_str()];
	}

void Delegate::Wrapper::pSetSignature(const char* R,
	const char* P0, const char* P1, const char* P2, const char* P3,
	const char* P4)
	{
	const string signature = R + kRcvrSEL + P0 + P1 + P2 + P3 + P4;
	fNSMethodSignature = [NSMethodSignature signatureWithObjCTypes:signature.c_str()];
	}

void Delegate::Wrapper::pSetSignature(const char* R,
	const char* P0, const char* P1, const char* P2, const char* P3,
	const char* P4, const char* P5)
	{
	const string signature = R + kRcvrSEL + P0 + P1 + P2 + P3 + P4 + P5;
	fNSMethodSignature = [NSMethodSignature signatureWithObjCTypes:signature.c_str()];
	}

void Delegate::Wrapper::pSetSignature(const char* R,
	const char* P0, const char* P1, const char* P2, const char* P3,
	const char* P4, const char* P5, const char* P6)
	{
	const string signature = R + kRcvrSEL + P0 + P1 + P2 + P3 + P4 + P5 + P6;
	fNSMethodSignature = [NSMethodSignature signatureWithObjCTypes:signature.c_str()];
	}

void Delegate::Wrapper::pSetSignature(const char* R,
	const char* P0, const char* P1, const char* P2, const char* P3,
	const char* P4, const char* P5, const char* P6, const char* P7)
	{
	const string signature = R + kRcvrSEL + P0 + P1 + P2 + P3 + P4 + P5 + P6 + P7;
	fNSMethodSignature = [NSMethodSignature signatureWithObjCTypes:signature.c_str()];
	}

} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(Cocoa)
