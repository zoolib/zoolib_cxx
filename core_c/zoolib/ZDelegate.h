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

#ifndef __ZDelegate_h__
#define __ZDelegate_h__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_SPI.h"

#if ZCONFIG_SPI_Enabled(Cocoa)

#include "zoolib/Callable.h"

#include "zoolib/ZCompat_NonCopyable.h"
#include "zoolib/ZObjC.h"
#include "zoolib/ZRef_NS.h"

#include <map>

#ifdef __OBJC__
	#import <Foundation/NSInvocation.h>
#endif

ZMACRO_ObjCClass(NSInvocation);
ZMACRO_ObjCClass(NSMethodSignature);
ZMACRO_ObjCClass(ZooLib_ZDelegate_Proxy);

namespace ZooLib {

// =================================================================================================
// MARK: - ZDelegate

class ZDelegate
:	NonCopyable
	{
public:
	ZDelegate();
	~ZDelegate();

	#ifdef __OBJC__

	template <class Callable>
	void Set(SEL iSEL, const ZRef<Callable>& iCallable);

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

	#endif // __OBJC__
	};

#ifdef __OBJC__

// =================================================================================================
// MARK: - ZDelegate::Wrapper

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
// MARK: - ZDelegate::Wrapper_T (specialized for 0 params)

template <class R>
class ZDelegate::Wrapper_T<R(void)>
:	public ZDelegate::Wrapper
	{
	friend class ZDelegate;

	typedef Callable<R(void)> Callable_t;

	Wrapper_T(const ZRef<Callable_t>& iCallable)
	:	fCallable(iCallable)
		{
		this->pSetSignature(@encode(R));
		}

	virtual void ForwardInvocation(NSInvocation* anInvocation)
		{
		R result = fCallable->Call();
		[anInvocation setReturnValue:&result];
		}

	const ZRef<Callable_t> fCallable;
	};

// =================================================================================================
// MARK: - ZDelegate::Wrapper_T (specialized for 0 params, void return)

template <>
class ZDelegate::Wrapper_T<void(void)>
:	public ZDelegate::Wrapper
	{
	friend class ZDelegate;

	typedef Callable<void(void)> Callable_t;

	Wrapper_T(const ZRef<Callable_t>& iCallable)
	:	fCallable(iCallable)
		{
		this->pSetSignature(@encode(void));
		}

	virtual void ForwardInvocation(NSInvocation* anInvocation)
		{
		fCallable->Call();
		}

	const ZRef<Callable_t> fCallable;
	};

// =================================================================================================
// MARK: - ZDelegate::Wrapper_T (specialized for 1 param)

template <class R,
	class P0>
class ZDelegate::Wrapper_T<R(P0)>
:	public ZDelegate::Wrapper
	{
	friend class ZDelegate;

	typedef ZCallable<R(P0)> Callable_t;

	Wrapper_T(const ZRef<Callable_t>& iCallable)
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

	const ZRef<Callable_t> fCallable;
	};

// =================================================================================================
// MARK: - ZDelegate::Wrapper_T (specialized for 1 param, void return)

template <class P0>
class ZDelegate::Wrapper_T<void(P0)>
:	public ZDelegate::Wrapper
	{
	friend class ZDelegate;

	typedef Callable<void(P0)> Callable_t;

	Wrapper_T(const ZRef<Callable_t>& iCallable)
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

	const ZRef<Callable_t> fCallable;
	};

// =================================================================================================
// MARK: - ZDelegate::Wrapper_T (specialized for 2 params)

template <class R,
	class P0, class P1>
class ZDelegate::Wrapper_T<R(P0,P1)>
:	public ZDelegate::Wrapper
	{
	friend class ZDelegate;

	typedef Callable<R(P0,P1)> Callable_t;

	Wrapper_T(const ZRef<Callable_t>& iCallable)
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

	const ZRef<Callable_t> fCallable;
	};

// =================================================================================================
// MARK: - ZDelegate::Wrapper_T (specialized for 2 params, void return)

template <class P0, class P1>
class ZDelegate::Wrapper_T<void(P0,P1)>
:	public ZDelegate::Wrapper
	{
	friend class ZDelegate;

	typedef Callable<void(P0,P1)> Callable_t;

	Wrapper_T(const ZRef<Callable_t>& iCallable)
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

	const ZRef<Callable_t> fCallable;
	};

// =================================================================================================
// MARK: - ZDelegate::Wrapper_T (specialized for 3 params)

template <class R,
	class P0, class P1, class P2>
class ZDelegate::Wrapper_T<R(P0,P1,P2)>
:	public ZDelegate::Wrapper
	{
	friend class ZDelegate;

	typedef Callable<R(P0,P1,P2)> Callable_t;

	Wrapper_T(const ZRef<Callable_t>& iCallable)
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

	const ZRef<Callable_t> fCallable;
	};

// =================================================================================================
// MARK: - ZDelegate::Wrapper_T (specialized for 3 params, void return)

template <class P0, class P1, class P2>
class ZDelegate::Wrapper_T<void(P0,P1,P2)>
:	public ZDelegate::Wrapper
	{
	friend class ZDelegate;

	typedef Callable<void(P0,P1,P2)> Callable_t;

	Wrapper_T(const ZRef<Callable_t>& iCallable)
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

	const ZRef<Callable_t> fCallable;
	};

// =================================================================================================
// MARK: - ZDelegate::Wrapper_T (specialized for 4 params)

template <class R,
	class P0, class P1, class P2, class P3>
class ZDelegate::Wrapper_T<R(P0,P1,P2,P3)>
:	public ZDelegate::Wrapper
	{
	friend class ZDelegate;

	typedef Callable<R(P0,P1,P2,P3)> Callable_t;

	Wrapper_T(const ZRef<Callable_t>& iCallable)
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

	const ZRef<Callable_t> fCallable;
	};

// =================================================================================================
// MARK: - ZDelegate::Wrapper_T (specialized for 4 params, void return)

template <class P0, class P1, class P2, class P3>
class ZDelegate::Wrapper_T<void(P0,P1,P2,P3)>
:	public ZDelegate::Wrapper
	{
	friend class ZDelegate;

	typedef Callable<void(P0,P1,P2,P3)> Callable_t;

	Wrapper_T(const ZRef<Callable_t>& iCallable)
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

	const ZRef<Callable_t> fCallable;
	};

// =================================================================================================
// MARK: - ZDelegate::Wrapper_T (specialized for 5 params)

template <class R,
	class P0, class P1, class P2, class P3, class P4>
class ZDelegate::Wrapper_T<R(P0,P1,P2,P3,P4)>
:	public ZDelegate::Wrapper
	{
	friend class ZDelegate;

	typedef Callable<R(P0,P1,P2,P3,P4)> Callable_t;

	Wrapper_T(const ZRef<Callable_t>& iCallable)
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

	const ZRef<Callable_t> fCallable;
	};

// =================================================================================================
// MARK: - ZDelegate::Wrapper_T (specialized for 5 params, void return)

template <class P0, class P1, class P2, class P3, class P4>
class ZDelegate::Wrapper_T<void(P0,P1,P2,P3,P4)>
:	public ZDelegate::Wrapper
	{
	friend class ZDelegate;

	typedef Callable<void(P0,P1,P2,P3,P4)> Callable_t;

	Wrapper_T(const ZRef<Callable_t>& iCallable)
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

	const ZRef<Callable_t> fCallable;
	};

// =================================================================================================
// MARK: - ZDelegate::Wrapper_T (specialized for 6 params)

template <class R,
	class P0, class P1, class P2, class P3, class P4, class P5>
class ZDelegate::Wrapper_T<R(P0,P1,P2,P3,P4,P5)>
:	public ZDelegate::Wrapper
	{
	friend class ZDelegate;

	typedef Callable<R(P0,P1,P2,P3,P4,P5)> Callable_t;

	Wrapper_T(const ZRef<Callable_t>& iCallable)
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

	const ZRef<Callable_t> fCallable;
	};

// =================================================================================================
// MARK: - ZDelegate::Wrapper_T (specialized for 6 params, void return)

template <class P0, class P1, class P2, class P3, class P4, class P5>
class ZDelegate::Wrapper_T<void(P0,P1,P2,P3,P4,P5)>
:	public ZDelegate::Wrapper
	{
	friend class ZDelegate;

	typedef Callable<void(P0,P1,P2,P3,P4,P5)> Callable_t;

	Wrapper_T(const ZRef<Callable_t>& iCallable)
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

	const ZRef<Callable_t> fCallable;
	};

// =================================================================================================
// MARK: - ZDelegate::Wrapper_T (specialized for 7 params)

template <class R,
	class P0, class P1, class P2, class P3, class P4, class P5, class P6>
class ZDelegate::Wrapper_T<R(P0,P1,P2,P3,P4,P5,P6)>
:	public ZDelegate::Wrapper
	{
	friend class ZDelegate;

	typedef Callable<R(P0,P1,P2,P3,P4,P5,P6)> Callable_t;

	Wrapper_T(const ZRef<Callable_t>& iCallable)
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

	const ZRef<Callable_t> fCallable;
	};

// =================================================================================================
// MARK: - ZDelegate::Wrapper_T (specialized for 7 params, void return)

template <class P0, class P1, class P2, class P3, class P4, class P5, class P6>
class ZDelegate::Wrapper_T<void(P0,P1,P2,P3,P4,P5,P6)>
:	public ZDelegate::Wrapper
	{
	friend class ZDelegate;

	typedef Callable<void(P0,P1,P2,P3,P4,P5,P6)> Callable_t;

	Wrapper_T(const ZRef<Callable_t>& iCallable)
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

	const ZRef<Callable_t> fCallable;
	};

// =================================================================================================
// MARK: - ZDelegate::Wrapper_T (specialized for 8 params)

template <class R,
	class P0, class P1, class P2, class P3, class P4, class P5, class P6, class P7>
class ZDelegate::Wrapper_T<R(P0,P1,P2,P3,P4,P5,P6,P7)>
:	public ZDelegate::Wrapper
	{
	friend class ZDelegate;

	typedef Callable<R(P0,P1,P2,P3,P4,P5,P6,P7)> Callable_t;

	Wrapper_T(const ZRef<Callable_t>& iCallable)
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

	const ZRef<Callable_t> fCallable;
	};

// =================================================================================================
// MARK: - ZDelegate::Wrapper_T (specialized for 8 params, void return)

template <class P0, class P1, class P2, class P3, class P4, class P5, class P6, class P7>
class ZDelegate::Wrapper_T<void(P0,P1,P2,P3,P4,P5,P6,P7)>
:	public ZDelegate::Wrapper
	{
	friend class ZDelegate;

	typedef Callable<void(P0,P1,P2,P3,P4,P5,P6,P7)> Callable_t;

	Wrapper_T(const ZRef<Callable_t>& iCallable)
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

	const ZRef<Callable_t> fCallable;
	};

// =================================================================================================
// MARK: - ZDelegate::Set

template <class Callable_p>
void ZDelegate::Set(SEL iSEL, const ZRef<Callable_p>& iCallable)
	{
	if (iCallable)
		fWrappers[iSEL] = new Wrapper_T<typename Callable_p::Signature>(iCallable);
	else
		fWrappers.erase(iSEL);
	}

#endif // __OBJC__

} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(Cocoa)
#endif // __ZDelegate_h__
