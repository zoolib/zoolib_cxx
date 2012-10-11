/* -------------------------------------------------------------------------------------------------
Copyright (c) 2012 Andrew Green
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

#ifndef __ZCallable_Lambda_h__
#define __ZCallable_Lambda_h__ 1
#include "zconfig.h"

#include "zoolib/ZCallable.h"

#include "zoolib/ZCompat_type_traits.h" // for std::remove_reference

#if ZCONFIG_SPI_Enabled(type_traits) && ZCONFIG_CPP >= 2011

// =================================================================================================
// MARK: - ZCallable_Lambda

namespace ZooLib {
namespace ZCallable_Lambda {

// =================================================================================================
// MARK: - RemoveClass_T

template<typename T> struct RemoveClass_T {};

template<typename C, typename R_p, typename... A_p>
struct RemoveClass_T<R_p(C::*)(A_p...)>
	{ using type = R_p(A_p...); };

template<typename C, typename R_p, typename... A_p>
struct RemoveClass_T<R_p(C::*)(A_p...) const>
	{ using type = R_p(A_p...); };

template<typename C, typename R_p, typename... A_p>
struct RemoveClass_T<R_p(C::*)(A_p...) volatile>
	{ using type = R_p(A_p...); };

template<typename C, typename R_p, typename... A_p>
struct RemoveClass_T<R_p(C::*)(A_p...) const volatile>
	{ using type = R_p(A_p...); };

// =================================================================================================
// MARK: - GetSig_T

template<typename T, bool> struct GetSigImpl_T {};

template<typename R_p, typename... A_p>
struct GetSigImpl_T<R_p(A_p...),true>
	{ using type = R_p(A_p...); };

template<typename R_p, typename... A_p>
struct GetSigImpl_T<R_p(*)(A_p...),true>
	{ using type = R_p(A_p...); };

template<typename T>
struct GetSigImpl_T<T, true>
	{
	using type =
		typename RemoveClass_T<decltype(&remove_reference<T>::type::operator())>::type;
	};

template<typename T> using GetSig_T = typename GetSigImpl_T<T,true>::type;

// =================================================================================================
// MARK: - Callable

template <class Lambda, class Sig> class Callable;

template <typename Lambda_p, typename R_p, typename... A_p>
class Callable<Lambda_p,R_p(A_p...)>
:	public ZCallable<R_p(A_p...)>
	{
public:
	Callable(const Lambda_p& iLambda)
	:	fLambda(iLambda)
		{}

// From ZCallable
	virtual ZQ<R_p> QCall(A_p... iParams)
		{ return fLambda(iParams...); }

private:
	Lambda_p fLambda;
	};

template <typename Lambda_p, typename... A_p>
class Callable<Lambda_p,void(A_p...)>
:	public ZCallable<void(A_p...)>
	{
public:
	Callable(const Lambda_p& iLambda)
	:	fLambda(iLambda)
		{}

// From ZCallable
	virtual ZQ<void> QCall(A_p... iParams)
		{
		fLambda(iParams...);
		return notnull;
		}

private:
	Lambda_p fLambda;
	};

template <typename Lambda_p>
ZRef<ZCallable
	<typename RemoveClass_T<decltype(&remove_reference<Lambda_p>::type::operator())>::type>
	>
sCallable(const Lambda_p& iLambda)
	{
	typedef typename RemoveClass_T<decltype(&remove_reference<Lambda_p>::type::operator())>::type
		Signature;

	return new Callable<Lambda_p,Signature>(iLambda);
	}

} // namespace ZCallable_Lambda

// =================================================================================================
// MARK: - sCallable

using ZCallable_Lambda::sCallable;

} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(type_traits) && ZCONFIG_CPP >= 2011

#endif // __ZCallable_Lambda_h__
