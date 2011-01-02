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

#ifndef __ZDelegate__
#define __ZDelegate__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_SPI.h"

#if ZCONFIG_SPI_Enabled(Cocoa)

#include "zoolib/ZCallable.h"
#include "zoolib/ZCompat_NonCopyable.h"
#include "zoolib/ZRef_NSObject.h"

#include <map>
#include <string>

#ifdef __OBJC__
	#import <Foundation/NSInvocation.h>
	@class NSMethodSignature;
	@class ZooLib_ZDelegate_Proxy;
#else
	typedef struct objc_ZooLib_ZDelegate_Proxy ZooLib_ZDelegate_Proxy;
#endif

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * ZDelegate

class ZDelegate : ZooLib::NonCopyable
	{
public:
	ZDelegate();
	~ZDelegate();

	#ifdef __OBJC__

	template <class Callable>
	void Set(SEL iSEL, ZRef<Callable> iCallable);

	operator id();

	// These can't be physically private, as they must be accessible by ZooLib_ZDelegate_Proxy.
	BOOL pRespondsToSelector(SEL aSelector);
	void pForwardInvocation(NSInvocation* anInvocation);
	NSMethodSignature* pMethodSignatureForSelector(SEL aSelector);

	#endif // __OBJC__

private:
	class Wrapper;

	ZooLib_ZDelegate_Proxy* fProxy;
	std::map<SEL, ZRef<Wrapper> > fWrappers;

	#ifdef __OBJC__

	template <class Signature> class Wrapper_T;

	template <class R>
	static ZRef<Wrapper> spMakeWrapper(ZRef<ZCallable<R(void)> > iCallable);

	template <class R,
		class P0>
	static ZRef<Wrapper> spMakeWrapper(ZRef<ZCallable<R(P0)> > iCallable);
	
	template <class R,
		class P0, class P1>
	static ZRef<Wrapper> spMakeWrapper(ZRef<ZCallable<R(P0,P1)> > iCallable);

	template <class R,
		class P0, class P1, class P2>
	static ZRef<Wrapper> spMakeWrapper(ZRef<ZCallable<R(P0,P1,P2)> > iCallable);

	template <class R,
		class P0, class P1, class P2, class P3>
	static ZRef<Wrapper> spMakeWrapper(ZRef<ZCallable<R(P0,P1,P2,P3)> > iCallable);

	template <class R,
		class P0, class P1, class P2, class P3, class P4>
	static ZRef<Wrapper> spMakeWrapper(ZRef<ZCallable<R(P0,P1,P2,P3,P4)> > iCallable);

	template <class R,
		class P0, class P1, class P2, class P3, class P4, class P5>
	static ZRef<Wrapper> spMakeWrapper(ZRef<ZCallable<R(P0,P1,P2,P3,P4,P5)> > iCallable);

	template <class R,
		class P0, class P1, class P2, class P3, class P4, class P5, class P6>
	static ZRef<Wrapper> spMakeWrapper(ZRef<ZCallable<R(P0,P1,P2,P3,P4,P5,P6)> > iCallable);

	template <class R,
		class P0, class P1, class P2, class P3, class P4, class P5, class P6, class P7>
	static ZRef<Wrapper> spMakeWrapper(ZRef<ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7)> > iCallable);

	#endif // __OBJC__
	};

#ifdef __OBJC__

// =================================================================================================
#pragma mark -
#pragma mark * ZDelegate::Wrapper

class ZDelegate::Wrapper
:	public ZCounted
	{
	friend class ZDelegate;

protected:
	virtual ~Wrapper() {}
	virtual void ForwardInvocation(NSInvocation* anInvocation) = 0;

	void pSetSignature(const char* R);

	void pSetSignature(const char* R,
		const char* P0);

	void pSetSignature(const char* R,
		const char* P0, const char* P1);

	void pSetSignature(const char* R,
		const char* P0, const char* P1, const char* P2);

	void pSetSignature(const char* R,
		const char* P0, const char* P1, const char* P2, const char* P3);

	void pSetSignature(const char* R,
		const char* P0, const char* P1, const char* P2, const char* P3,
		const char* P4);

	void pSetSignature(const char* R,
		const char* P0, const char* P1, const char* P2, const char* P3,
		const char* P4, const char* P5);

	void pSetSignature(const char* R,
		const char* P0, const char* P1, const char* P2, const char* P3,
		const char* P4, const char* P5, const char* P6);

	void pSetSignature(const char* R,
		const char* P0, const char* P1, const char* P2, const char* P3,
		const char* P4, const char* P5, const char* P6, const char* P7);

	ZRef<NSMethodSignature> fNSMethodSignature;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZDelegate::Wrapper_T (specialized for 0 params)

template <class R>
class ZDelegate::Wrapper_T<R(void)> : public ZDelegate::Wrapper
	{
	friend class ZDelegate;

	typedef ZCallable<R(void)> Callable;

	Wrapper_T(ZRef<Callable> iCallable)
	:	fCallable(iCallable)
		{
		this->pSetSignature(@encode(R));
		}
	
	virtual void ForwardInvocation(NSInvocation* anInvocation)
		{
		R result = fCallable->Call();
		[anInvocation setReturnValue:&result];
		}

	ZRef<Callable> fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZDelegate::Wrapper_T (specialized for 0 params, void return)

template <>
class ZDelegate::Wrapper_T<void(void)> : public ZDelegate::Wrapper
	{
	friend class ZDelegate;

	typedef ZCallable<void(void)> Callable;

	Wrapper_T(ZRef<Callable> iCallable)
	:	fCallable(iCallable)
		{
		this->pSetSignature(@encode(void));
		}
	
	virtual void ForwardInvocation(NSInvocation* anInvocation)
		{
		fCallable->Call();
		}

	ZRef<Callable> fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZDelegate::Wrapper_T (specialized for 1 param)

template <class R,
	class P0>
class ZDelegate::Wrapper_T<R(P0)> : public ZDelegate::Wrapper
	{
	friend class ZDelegate;

	typedef ZCallable<R(P0)> Callable;

	Wrapper_T(ZRef<Callable> iCallable)
	:	fCallable(iCallable)
		{
		this->pSetSignature(@encode(R),
			@encode(P0));
		}
	
	virtual void ForwardInvocation(NSInvocation* anInvocation)
		{
		P0 p0;
		[anInvocation getArgument:&p0 atIndex:2];

		R result = fCallable->Call(p0);
		[anInvocation setReturnValue:&result];
		}

	ZRef<Callable> fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZDelegate::Wrapper_T (specialized for 1 param, void return)

template <class P0>
class ZDelegate::Wrapper_T<void(P0)> : public ZDelegate::Wrapper
	{
	friend class ZDelegate;

	typedef ZCallable<void(P0)> Callable;

	Wrapper_T(ZRef<Callable> iCallable)
	:	fCallable(iCallable)
		{
		this->pSetSignature(@encode(void),
			@encode(P0));
		}
	
	virtual void ForwardInvocation(NSInvocation* anInvocation)
		{
		P0 p0;
		[anInvocation getArgument:&p0 atIndex:2];

		fCallable->Call(p0);
		}

	ZRef<Callable> fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZDelegate::Wrapper_T (specialized for 2 params)

template <class R,
	class P0, class P1>
class ZDelegate::Wrapper_T<R(P0,P1)> : public ZDelegate::Wrapper
	{
	friend class ZDelegate;

	typedef ZCallable<R(P0,P1)> Callable;

	Wrapper_T(ZRef<Callable> iCallable)
	:	fCallable(iCallable)
		{
		this->pSetSignature(@encode(R),
			@encode(P0), @encode(P1));
		}

	virtual void ForwardInvocation(NSInvocation* anInvocation)
		{
		P0 p0;
		[anInvocation getArgument:&p0 atIndex:2];
		P1 p1;
		[anInvocation getArgument:&p1 atIndex:3];

		R result = fCallable->Call(p0, p1);
		[anInvocation setReturnValue:&result];
		}

	ZRef<Callable> fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZDelegate::Wrapper_T (specialized for 2 params, void return)

template <class P0, class P1>
class ZDelegate::Wrapper_T<void(P0,P1)> : public ZDelegate::Wrapper
	{
	friend class ZDelegate;

	typedef ZCallable<void(P0,P1)> Callable;

	Wrapper_T(ZRef<Callable> iCallable)
	:	fCallable(iCallable)
		{
		this->pSetSignature(@encode(void),
			@encode(P0), @encode(P1));
		}

	virtual void ForwardInvocation(NSInvocation* anInvocation)
		{
		P0 p0;
		[anInvocation getArgument:&p0 atIndex:2];
		P1 p1;
		[anInvocation getArgument:&p1 atIndex:3];

		fCallable->Call(p0, p1);
		}

	ZRef<Callable> fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZDelegate::Wrapper_T (specialized for 3 params)

template <class R,
	class P0, class P1, class P2>
class ZDelegate::Wrapper_T<R(P0,P1,P2)> : public ZDelegate::Wrapper
	{
	friend class ZDelegate;

	typedef ZCallable<R(P0,P1,P2)> Callable;

	Wrapper_T(ZRef<Callable> iCallable)
	:	fCallable(iCallable)
		{
		this->pSetSignature(@encode(R),
			@encode(P0), @encode(P1), @encode(P2));
		}
	
	virtual void ForwardInvocation(NSInvocation* anInvocation)
		{
		P0 p0;
		[anInvocation getArgument:&p0 atIndex:2];
		P1 p1;
		[anInvocation getArgument:&p1 atIndex:3];
		P2 p2;
		[anInvocation getArgument:&p2 atIndex:4];

		R result = fCallable->Call(p0, p1, p2);
		[anInvocation setReturnValue:&result];
		}

	ZRef<Callable> fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZDelegate::Wrapper_T (specialized for 3 params, void return)

template <class P0, class P1, class P2>
class ZDelegate::Wrapper_T<void(P0,P1,P2)> : public ZDelegate::Wrapper
	{
	friend class ZDelegate;

	typedef ZCallable<void(P0,P1,P2)> Callable;

	Wrapper_T(ZRef<Callable> iCallable)
	:	fCallable(iCallable)
		{
		this->pSetSignature(@encode(void),
			@encode(P0), @encode(P1), @encode(P2));
		}
	
	virtual void ForwardInvocation(NSInvocation* anInvocation)
		{
		P0 p0;
		[anInvocation getArgument:&p0 atIndex:2];
		P1 p1;
		[anInvocation getArgument:&p1 atIndex:3];
		P2 p2;
		[anInvocation getArgument:&p2 atIndex:4];

		fCallable->Call(p0, p1, p2);
		}

	ZRef<Callable> fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZDelegate::Wrapper_T (specialized for 4 params)

template <class R,
	class P0, class P1, class P2, class P3>
class ZDelegate::Wrapper_T<R(P0,P1,P2,P3)> : public ZDelegate::Wrapper
	{
	friend class ZDelegate;

	typedef ZCallable<R(P0,P1,P2,P3)> Callable;

	Wrapper_T(ZRef<Callable> iCallable)
	:	fCallable(iCallable)
		{
		this->pSetSignature(@encode(R),
			@encode(P0), @encode(P1), @encode(P2), @encode(P3));
		}
	
	virtual void ForwardInvocation(NSInvocation* anInvocation)
		{
		P0 p0;
		[anInvocation getArgument:&p0 atIndex:2];
		P1 p1;
		[anInvocation getArgument:&p1 atIndex:3];
		P2 p2;
		[anInvocation getArgument:&p2 atIndex:4];
		P3 p3;
		[anInvocation getArgument:&p3 atIndex:5];

		R result = fCallable->Call(p0, p1, p2, p3);
		[anInvocation setReturnValue:&result];
		}

	ZRef<Callable> fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZDelegate::Wrapper_T (specialized for 4 params, void return)

template <class P0, class P1, class P2, class P3>
class ZDelegate::Wrapper_T<void(P0,P1,P2,P3)> : public ZDelegate::Wrapper
	{
	friend class ZDelegate;

	typedef ZCallable<void(P0,P1,P2,P3)> Callable;

	Wrapper_T(ZRef<Callable> iCallable)
	:	fCallable(iCallable)
		{
		this->pSetSignature(@encode(void),
			@encode(P0), @encode(P1), @encode(P2), @encode(P3));
		}
	
	virtual void ForwardInvocation(NSInvocation* anInvocation)
		{
		P0 p0;
		[anInvocation getArgument:&p0 atIndex:2];
		P1 p1;
		[anInvocation getArgument:&p1 atIndex:3];
		P2 p2;
		[anInvocation getArgument:&p2 atIndex:4];
		P3 p3;
		[anInvocation getArgument:&p3 atIndex:5];

		fCallable->Call(p0, p1, p2, p3);
		}

	ZRef<Callable> fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZDelegate::Wrapper_T (specialized for 5 params)

template <class R,
	class P0, class P1, class P2, class P3, class P4>
class ZDelegate::Wrapper_T<R(P0,P1,P2,P3,P4)> : public ZDelegate::Wrapper
	{
	friend class ZDelegate;

	typedef ZCallable<R(P0,P1,P2,P3,P4)> Callable;

	Wrapper_T(ZRef<Callable> iCallable)
	:	fCallable(iCallable)
		{
		this->pSetSignature(@encode(R),
			@encode(P0), @encode(P1), @encode(P2), @encode(P3),
			@encode(P4));
		}
	
	virtual void ForwardInvocation(NSInvocation* anInvocation)
		{
		P0 p0;
		[anInvocation getArgument:&p0 atIndex:2];
		P1 p1;
		[anInvocation getArgument:&p1 atIndex:3];
		P2 p2;
		[anInvocation getArgument:&p2 atIndex:4];
		P3 p3;
		[anInvocation getArgument:&p3 atIndex:5];
		P4 p4;
		[anInvocation getArgument:&p4 atIndex:6];

		R result = fCallable->Call(p0, p1, p2, p3, p4);
		[anInvocation setReturnValue:&result];
		}

	ZRef<Callable> fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZDelegate::Wrapper_T (specialized for 5 params, void return)

template <class P0, class P1, class P2, class P3, class P4>
class ZDelegate::Wrapper_T<void(P0,P1,P2,P3,P4)> : public ZDelegate::Wrapper
	{
	friend class ZDelegate;

	typedef ZCallable<void(P0,P1,P2,P3,P4)> Callable;

	Wrapper_T(ZRef<Callable> iCallable)
	:	fCallable(iCallable)
		{
		this->pSetSignature(@encode(void),
			@encode(P0), @encode(P1), @encode(P2), @encode(P3),
			@encode(P4));
		}
	
	virtual void ForwardInvocation(NSInvocation* anInvocation)
		{
		P0 p0;
		[anInvocation getArgument:&p0 atIndex:2];
		P1 p1;
		[anInvocation getArgument:&p1 atIndex:3];
		P2 p2;
		[anInvocation getArgument:&p2 atIndex:4];
		P3 p3;
		[anInvocation getArgument:&p3 atIndex:5];
		P4 p4;
		[anInvocation getArgument:&p4 atIndex:6];

		fCallable->Call(p0, p1, p2, p3, p4);
		}

	ZRef<Callable> fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZDelegate::Wrapper_T (specialized for 6 params)

template <class R,
	class P0, class P1, class P2, class P3, class P4, class P5>
class ZDelegate::Wrapper_T<R(P0,P1,P2,P3,P4,P5)> : public ZDelegate::Wrapper
	{
	friend class ZDelegate;

	typedef ZCallable<R(P0,P1,P2,P3,P4,P5)> Callable;

	Wrapper_T(ZRef<Callable> iCallable)
	:	fCallable(iCallable)
		{
		this->pSetSignature(@encode(R),
			@encode(P0), @encode(P1), @encode(P2), @encode(P3),
			@encode(P4), @encode(P5));
		}
	
	virtual void ForwardInvocation(NSInvocation* anInvocation)
		{
		P0 p0;
		[anInvocation getArgument:&p0 atIndex:2];
		P1 p1;
		[anInvocation getArgument:&p1 atIndex:3];
		P2 p2;
		[anInvocation getArgument:&p2 atIndex:4];
		P3 p3;
		[anInvocation getArgument:&p3 atIndex:5];
		P4 p4;
		[anInvocation getArgument:&p4 atIndex:6];
		P5 p5;
		[anInvocation getArgument:&p5 atIndex:7];

		R result = fCallable->Call(p0, p1, p2, p3, p4, p5);
		[anInvocation setReturnValue:&result];
		}

	ZRef<Callable> fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZDelegate::Wrapper_T (specialized for 6 params, void return)

template <class P0, class P1, class P2, class P3, class P4, class P5>
class ZDelegate::Wrapper_T<void(P0,P1,P2,P3,P4,P5)> : public ZDelegate::Wrapper
	{
	friend class ZDelegate;

	typedef ZCallable<void(P0,P1,P2,P3,P4,P5)> Callable;

	Wrapper_T(ZRef<Callable> iCallable)
	:	fCallable(iCallable)
		{
		this->pSetSignature(@encode(void),
			@encode(P0), @encode(P1), @encode(P2), @encode(P3),
			@encode(P4), @encode(P5));
		}
	
	virtual void ForwardInvocation(NSInvocation* anInvocation)
		{
		P0 p0;
		[anInvocation getArgument:&p0 atIndex:2];
		P1 p1;
		[anInvocation getArgument:&p1 atIndex:3];
		P2 p2;
		[anInvocation getArgument:&p2 atIndex:4];
		P3 p3;
		[anInvocation getArgument:&p3 atIndex:5];
		P4 p4;
		[anInvocation getArgument:&p4 atIndex:6];
		P5 p5;
		[anInvocation getArgument:&p5 atIndex:7];

		fCallable->Call(p0, p1, p2, p3, p4, p5);
		}

	ZRef<Callable> fCallable;
	};


// =================================================================================================
#pragma mark -
#pragma mark * ZDelegate::Wrapper_T (specialized for 7 params)

template <class R,
	class P0, class P1, class P2, class P3, class P4, class P5, class P6>
class ZDelegate::Wrapper_T<R(P0,P1,P2,P3,P4,P5,P6)> : public ZDelegate::Wrapper
	{
	friend class ZDelegate;

	typedef ZCallable<R(P0,P1,P2,P3,P4,P5,P6)> Callable;

	Wrapper_T(ZRef<Callable> iCallable)
	:	fCallable(iCallable)
		{
		this->pSetSignature(@encode(R),
			@encode(P0), @encode(P1), @encode(P2), @encode(P3),
			@encode(P4), @encode(P5), @encode(P6));
		}
	
	virtual void ForwardInvocation(NSInvocation* anInvocation)
		{
		P0 p0;
		[anInvocation getArgument:&p0 atIndex:2];
		P1 p1;
		[anInvocation getArgument:&p1 atIndex:3];
		P2 p2;
		[anInvocation getArgument:&p2 atIndex:4];
		P3 p3;
		[anInvocation getArgument:&p3 atIndex:5];
		P4 p4;
		[anInvocation getArgument:&p4 atIndex:6];
		P5 p5;
		[anInvocation getArgument:&p5 atIndex:7];
		P6 p6;
		[anInvocation getArgument:&p6 atIndex:8];

		R result = fCallable->Call(p0, p1, p2, p3, p4, p5, p6);
		[anInvocation setReturnValue:&result];
		}

	ZRef<Callable> fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZDelegate::Wrapper_T (specialized for 7 params, void return)

template <class P0, class P1, class P2, class P3, class P4, class P5, class P6>
class ZDelegate::Wrapper_T<void(P0,P1,P2,P3,P4,P5,P6)> : public ZDelegate::Wrapper
	{
	friend class ZDelegate;

	typedef ZCallable<void(P0,P1,P2,P3,P4,P5,P6)> Callable;

	Wrapper_T(ZRef<Callable> iCallable)
	:	fCallable(iCallable)
		{
		this->pSetSignature(@encode(void),
			@encode(P0), @encode(P1), @encode(P2), @encode(P3),
			@encode(P4), @encode(P5), @encode(P6));
		}
	
	virtual void ForwardInvocation(NSInvocation* anInvocation)
		{
		P0 p0;
		[anInvocation getArgument:&p0 atIndex:2];
		P1 p1;
		[anInvocation getArgument:&p1 atIndex:3];
		P2 p2;
		[anInvocation getArgument:&p2 atIndex:4];
		P3 p3;
		[anInvocation getArgument:&p3 atIndex:5];
		P4 p4;
		[anInvocation getArgument:&p4 atIndex:6];
		P5 p5;
		[anInvocation getArgument:&p5 atIndex:7];
		P6 p6;
		[anInvocation getArgument:&p6 atIndex:8];

		fCallable->Call(p0, p1, p2, p3, p4, p5, p6);
		}

	ZRef<Callable> fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZDelegate::Wrapper_T (specialized for 8 params)

template <class R,
	class P0, class P1, class P2, class P3, class P4, class P5, class P6, class P7>
class ZDelegate::Wrapper_T<R(P0,P1,P2,P3,P4,P5,P6,P7)> : public ZDelegate::Wrapper
	{
	friend class ZDelegate;

	typedef ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7)> Callable;

	Wrapper_T(ZRef<Callable> iCallable)
	:	fCallable(iCallable)
		{
		this->pSetSignature(@encode(R),
			@encode(P0), @encode(P1), @encode(P2), @encode(P3),
			@encode(P4), @encode(P5), @encode(P6), @encode(P7));
		}
	
	virtual void ForwardInvocation(NSInvocation* anInvocation)
		{
		P0 p0;
		[anInvocation getArgument:&p0 atIndex:2];
		P1 p1;
		[anInvocation getArgument:&p1 atIndex:3];
		P2 p2;
		[anInvocation getArgument:&p2 atIndex:4];
		P3 p3;
		[anInvocation getArgument:&p3 atIndex:5];
		P4 p4;
		[anInvocation getArgument:&p4 atIndex:6];
		P5 p5;
		[anInvocation getArgument:&p5 atIndex:7];
		P6 p6;
		[anInvocation getArgument:&p6 atIndex:8];
		P7 p7;
		[anInvocation getArgument:&p7 atIndex:9];

		R result = fCallable->Call(p0, p1, p2, p3, p4, p5, p6, p7);
		[anInvocation setReturnValue:&result];
		}

	ZRef<Callable> fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZDelegate::Wrapper_T (specialized for 8 params, void return)

template <class P0, class P1, class P2, class P3, class P4, class P5, class P6, class P7>
class ZDelegate::Wrapper_T<void(P0,P1,P2,P3,P4,P5,P6,P7)> : public ZDelegate::Wrapper
	{
	friend class ZDelegate;

	typedef ZCallable<void(P0,P1,P2,P3,P4,P5,P6,P7)> Callable;

	Wrapper_T(ZRef<Callable> iCallable)
	:	fCallable(iCallable)
		{
		this->pSetSignature(@encode(void),
			@encode(P0), @encode(P1), @encode(P2), @encode(P3),
			@encode(P4), @encode(P5), @encode(P6), @encode(P7));
		}
	
	virtual void ForwardInvocation(NSInvocation* anInvocation)
		{
		P0 p0;
		[anInvocation getArgument:&p0 atIndex:2];
		P1 p1;
		[anInvocation getArgument:&p1 atIndex:3];
		P2 p2;
		[anInvocation getArgument:&p2 atIndex:4];
		P3 p3;
		[anInvocation getArgument:&p3 atIndex:5];
		P4 p4;
		[anInvocation getArgument:&p4 atIndex:6];
		P5 p5;
		[anInvocation getArgument:&p5 atIndex:7];
		P6 p6;
		[anInvocation getArgument:&p6 atIndex:8];
		P7 p7;
		[anInvocation getArgument:&p6 atIndex:9];

		fCallable->Call(p0, p1, p2, p3, p4, p5, p6, p7);
		}

	ZRef<Callable> fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZDelegate::spMakeWrapper

template <class R,
	class P0>
ZRef<ZDelegate::Wrapper>
ZDelegate::spMakeWrapper(ZRef<ZCallable<R(P0)> > iCallable)
	{ return new ZDelegate::Wrapper_T<R(P0)>(iCallable); }

template <class R,
	class P0, class P1>
ZRef<ZDelegate::Wrapper>
ZDelegate::spMakeWrapper(ZRef<ZCallable<R(P0,P1)> > iCallable)
	{ return new ZDelegate::Wrapper_T<R(P0,P1)>(iCallable); }

template <class R,
	class P0, class P1, class P2>
ZRef<ZDelegate::Wrapper>
ZDelegate::spMakeWrapper(ZRef<ZCallable<R(P0,P1,P2)> > iCallable)
	{ return new ZDelegate::Wrapper_T<R(P0,P1,P2)>(iCallable); }

template <class R,
	class P0, class P1, class P2, class P3>
ZRef<ZDelegate::Wrapper>
ZDelegate::spMakeWrapper(ZRef<ZCallable<R(P0,P1,P2,P3)> > iCallable)
	{ return new ZDelegate::Wrapper_T<R(P0,P1,P2,P3)>(iCallable); }

template <class R,
	class P0, class P1, class P2, class P3, class P4>
ZRef<ZDelegate::Wrapper>
ZDelegate::spMakeWrapper(ZRef<ZCallable<R(P0,P1,P2,P3,P4)> > iCallable)
	{ return new ZDelegate::Wrapper_T<R(P0,P1,P2,P3,P4)>(iCallable); }

template <class R,
	class P0, class P1, class P2, class P3, class P4, class P5>
ZRef<ZDelegate::Wrapper>
ZDelegate::spMakeWrapper(ZRef<ZCallable<R(P0,P1,P2,P3,P4,P5)> > iCallable)
	{ return new ZDelegate::Wrapper_T<R(P0,P1,P2,P3,P4,P5)>(iCallable); }

template <class R,
	class P0, class P1, class P2, class P3, class P4, class P5, class P6>
ZRef<ZDelegate::Wrapper>
ZDelegate::spMakeWrapper(ZRef<ZCallable<R(P0,P1,P2,P3,P4,P5,P6)> > iCallable)
	{ return new ZDelegate::Wrapper_T<R(P0,P1,P2,P3,P4,P5,P6)>(iCallable); }

template <class R,
	class P0, class P1, class P2, class P3, class P4, class P5, class P6, class P7>
ZRef<ZDelegate::Wrapper>
ZDelegate::spMakeWrapper(ZRef<ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7)> > iCallable)
	{ return new ZDelegate::Wrapper_T<R(P0,P1,P2,P3,P4,P5,P6,P7)>(iCallable); }

// =================================================================================================
#pragma mark -
#pragma mark * ZDelegate::Add

template <class Callable>
void ZDelegate::Set(SEL iSEL, ZRef<Callable> iCallable)
	{
	if (iCallable)
		fWrappers[iSEL] = spMakeWrapper(iCallable);
	else
		fWrappers.erase(iSEL);
	}

#endif // __OBJC__

} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(Cocoa)
#endif // __ZDelegate__
