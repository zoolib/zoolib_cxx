// Copyright (c) 2015 Andrew Green. MIT License. http://www.zoolib.org

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
	:	fT(iOther.fT)
		{}

	~Not() {}

	Not& operator=(const Not& iOther)
		{
		fT = iOther.fT;
		return *this;
		}

// --

	Not(const T& iT)
	:	fT(iT)
		{}

	Not& operator=(const T& iT)
		{
		fT = iT;
		return *this;
		}

// --

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

// --

	explicit operator bool()
		{ return not (true && fT); }

	explicit operator bool() const
		{ return not (true && fT); }

// --

	operator T&()
		{ return fT; }

	operator const T&() const
		{ return fT; }

// --

	decltype(*(*(T*)0)) operator*()
		{ return *fT; }

	decltype(*(*(const T*)0)) operator*() const
		{ return *fT; }

// --

	T& operator->()
		{ return fT; }

	const T& operator->() const
		{ return fT; }

// --


// --

private:
	T fT;
	};

// =================================================================================================
#pragma mark - Pseudo-ctor

template <class T>
Not<T> sNot()
	{ return Not<T>(); }

template <class T>
const T& sNot(const Not<T>& iNot)
	{ return iNot; }

template <class T>
Not<T> sNot(const T& iT)
	{ return Not<T>(iT); }

template <class T, class P0>
Not<T> sNot(const P0& i0)
	{ return Not<T>(i0); }

} // namespace ZooLib

#endif // __ZooLib_Not_h__
