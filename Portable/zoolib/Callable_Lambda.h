// Copyright (c) 2012 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Callable_Lambda_h__
#define __ZooLib_Callable_Lambda_h__ 1
#include "zconfig.h"

#include "zoolib/Callable.h"

#include <type_traits> // For std::remove_reference

// =================================================================================================
#pragma mark - Callable_Lambda

namespace ZooLib {
namespace Callable_Lambda_Util {

// =================================================================================================
#pragma mark - RemoveClass_T

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
#pragma mark - GetSig_T

// Available to be used below and elsewhere, but never proved valuable?

//template<typename T, bool> struct GetSigImpl_T {};
//
//template<typename R_p, typename... A_p>
//struct GetSigImpl_T<R_p(A_p...),true>
//	{ using type = R_p(A_p...); };
//
//template<typename R_p, typename... A_p>
//struct GetSigImpl_T<R_p(*)(A_p...),true>
//	{ using type = R_p(A_p...); };
//
//template<typename T>
//struct GetSigImpl_T<T, true>
//	{
//	using type =
//		typename RemoveClass_T<decltype(&remove_reference<T>::type::operator())>::type;
//	};
//
//template<typename T> using GetSig_T = typename GetSigImpl_T<T,true>::type;

} // namespace Callable_Lambda_Util

// =================================================================================================
#pragma mark - Callable

template <class Lambda, class Sig> class Callable_Lambda;

template <typename Lambda_p, typename R_p, typename... A_p>
class Callable_Lambda<Lambda_p,R_p(A_p...)>
:	public Callable<R_p(A_p...)>
	{
public:
	Callable_Lambda(const Lambda_p& iLambda)
	:	fLambda(iLambda)
		{}

// From Callable
	virtual QRet<R_p> QCall(A_p... iParams)
		{ return fLambda(iParams...); }

private:
	Lambda_p fLambda;
	};

template <typename Lambda_p, typename... A_p>
class Callable_Lambda<Lambda_p,void(A_p...)>
:	public Callable<void(A_p...)>
	{
public:
	Callable_Lambda(const Lambda_p& iLambda)
	:	fLambda(iLambda)
		{}

// From Callable
	virtual bool QCall(A_p... iParams)
		{
		fLambda(iParams...);
		return true;
		}

private:
	Lambda_p fLambda;
	};

template <typename Lambda_p>
ZP<Callable
	<typename Callable_Lambda_Util::RemoveClass_T<decltype(&std::remove_reference<Lambda_p>::type::operator())>::type>
	>
sCallable(const Lambda_p& iLambda)
	{
	typedef typename Callable_Lambda_Util::RemoveClass_T<decltype(&std::remove_reference<Lambda_p>::type::operator())>::type
		Signature;

	return new Callable_Lambda<Lambda_p,Signature>(iLambda);
	}

} // namespace ZooLib

#endif // __ZooLib_Callable_Lambda_h__
