// Copyright (c) 2010 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Callable_Function_h__
#define __ZooLib_Callable_Function_h__ 1
#include "zconfig.h"

#include "zoolib/Callable.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - Callable

template <class Signature> class Callable_Function;

// =================================================================================================
#pragma mark - Callable (specialization for 0 params)

template <class R>
class Callable_Function<R()>
:	public Callable<R()>
	{
public:
	typedef R (*FunctionPtr_t)();

	Callable_Function(FunctionPtr_t iFunctionPtr)
	:	fFunctionPtr(iFunctionPtr)
		{}

// From Callable
	virtual QRet<R> QCall()
		{ return fFunctionPtr(); }

private:
	FunctionPtr_t fFunctionPtr;
	};

// =================================================================================================
#pragma mark - Callable (specialization for 0 params, void return)

template <>
class Callable_Function<void()>
:	public Callable<void()>
	{
public:
	typedef void (*FunctionPtr_t)();

	Callable_Function(FunctionPtr_t iFunctionPtr)
	:	fFunctionPtr(iFunctionPtr)
		{}

// From Callable
	virtual bool QCall()
		{
		fFunctionPtr();
		return true;
		}

private:
	FunctionPtr_t fFunctionPtr;
	};

// =================================================================================================
#pragma mark - Callable (specializations for 1-16 params)

#define ZMACRO_Callable_Callable(X) \
\
template <class R, ZMACRO_Callable_Class_P##X> \
class Callable_Function<R(ZMACRO_Callable_P##X)> \
:	public Callable<R(ZMACRO_Callable_P##X)> \
	{ \
public: \
	typedef R (*FunctionPtr_t)(ZMACRO_Callable_P##X); \
\
	Callable_Function(FunctionPtr_t iFunctionPtr) \
	:	fFunctionPtr(iFunctionPtr) \
		{} \
\
	virtual QRet<R> QCall(ZMACRO_Callable_Pi##X) \
		{ return fFunctionPtr(ZMACRO_Callable_i##X); } \
\
private: \
	FunctionPtr_t fFunctionPtr; \
	}; \
\
template <ZMACRO_Callable_Class_P##X> \
class Callable_Function<void(ZMACRO_Callable_P##X)> \
:	public Callable<void(ZMACRO_Callable_P##X)> \
	{ \
public: \
	typedef void (*FunctionPtr_t)(ZMACRO_Callable_P##X); \
\
	Callable_Function(FunctionPtr_t iFunctionPtr) \
	:	fFunctionPtr(iFunctionPtr) \
		{} \
\
	virtual bool QCall(ZMACRO_Callable_Pi##X) \
		{ \
		fFunctionPtr(ZMACRO_Callable_i##X); \
		return true; \
		} \
\
private: \
	FunctionPtr_t fFunctionPtr; \
	};

ZMACRO_Callable_Callable(0)
ZMACRO_Callable_Callable(1)
ZMACRO_Callable_Callable(2)
ZMACRO_Callable_Callable(3)
ZMACRO_Callable_Callable(4)
ZMACRO_Callable_Callable(5)
ZMACRO_Callable_Callable(6)
ZMACRO_Callable_Callable(7)
ZMACRO_Callable_Callable(8)
ZMACRO_Callable_Callable(9)
ZMACRO_Callable_Callable(A)
ZMACRO_Callable_Callable(B)
ZMACRO_Callable_Callable(C)
ZMACRO_Callable_Callable(D)
ZMACRO_Callable_Callable(E)
ZMACRO_Callable_Callable(F)

#undef ZMACRO_Callable_Callable

// =================================================================================================
#pragma mark - sCallable

template <class R>
ZP<Callable<R()>>
sCallable(R (*iFunctionPtr)())
	{
	if (not iFunctionPtr)
		return null;
	return new Callable_Function<R()>(iFunctionPtr);
	}

#define ZMACRO_Callable_sCallable(X) \
\
template <class R, ZMACRO_Callable_Class_P##X> \
ZP<Callable<R(ZMACRO_Callable_P##X)>> \
sCallable(R (*iFunctionPtr)(ZMACRO_Callable_P##X)) \
	{ \
	if (not iFunctionPtr) \
		return null; \
	return new Callable_Function<R(ZMACRO_Callable_P##X)>(iFunctionPtr); \
	}

ZMACRO_Callable_sCallable(0)
ZMACRO_Callable_sCallable(1)
ZMACRO_Callable_sCallable(2)
ZMACRO_Callable_sCallable(3)
ZMACRO_Callable_sCallable(4)
ZMACRO_Callable_sCallable(5)
ZMACRO_Callable_sCallable(6)
ZMACRO_Callable_sCallable(7)
ZMACRO_Callable_sCallable(8)
ZMACRO_Callable_sCallable(9)
ZMACRO_Callable_sCallable(A)
ZMACRO_Callable_sCallable(B)
ZMACRO_Callable_sCallable(C)
ZMACRO_Callable_sCallable(D)
ZMACRO_Callable_sCallable(E)
ZMACRO_Callable_sCallable(F)

#undef ZMACRO_Callable_sCallable

} // namespace ZooLib

#endif // __ZooLib_Callable_Function_h__
