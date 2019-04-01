/* -------------------------------------------------------------------------------------------------
Copyright (c) 2015 Andrew Green
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

#ifndef __ZooLib_Not_h__
#define __ZooLib_Not_h__ 1
#include "zconfig.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - Not

template <class T>
class Not
	{
public:
	Not() {}

	Not(const Not& iOther)
	:	fT(iOther)
		{}

	~Not() {}

	Not& operator=(const Not& iOther)
		{
		fT = iOther.fT;
		return *this;
		}

// -----------------

	template <class P0>
	Not(const P0& i0)
	:	fT(i0)
		{}

	template <class P0>
	Not& operator=(const P0& i0)
		{
		fT = i0;
		return *this;
		}

// -----------------

	explicit operator bool()
		{ return not (true && fT); }

	explicit operator bool() const
		{ return not (true && fT); }

// -----------------

	operator T&()
		{ return fT; }

	operator const T&() const
		{ return fT; }

// -----------------

//	__typeof__(*(T()))	operator*()
//		{ return *fT; }

//	__typeof__(*(T()))	operator*() const
//		{ return *fT; }

// -----------------

private:
	T fT;
	};

// =================================================================================================
#pragma mark - Pseudo-ctor

template <class T>
Not<T> sNot()
	{ return Not<T>(); }

template <class T>
Not<T> sNot(const T& iT)
	{ return Not<T>(iT); }

template <class T, class P0>
Not<T> sNot(const P0& i0)
	{ return Not<T>(i0); }

} // namespace ZooLib

#endif // __ZooLib_Not_h__
