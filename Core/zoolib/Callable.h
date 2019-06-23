/* -------------------------------------------------------------------------------------------------
Copyright (c) 2010 Andrew Green
http://www.zoolib.org

Permission is hereby granted, free of charge, to any person obtaining a copy of this software
and associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge,Publish, distribute,
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

#ifndef __ZooLib_Callable_h__
#define __ZooLib_Callable_h__ 1
#include "zconfig.h"

#include "zoolib/Callable_Macros.h"

#include "zoolib/ZCounted.h"
#include "zoolib/ZQ.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - QTraits

template <class T> struct QTraits { typedef ZQ<T> Type_t; };

template <> struct QTraits<void> { typedef bool Type_t; };

template <class T> struct QTraits<ZP<T>> { typedef ZP<T> Type_t; };

template <class T> using QRet = typename QTraits<T>::Type_t;

// =================================================================================================
#pragma mark - Callable

template <class Signature_p> class Callable;

// =================================================================================================
#pragma mark - Callable (specialization for 0 params)

template <class R_p>
class Callable<R_p(void)>
:	public ZCounted
	{
public:
	typedef R_p R;
	typedef R (Signature)(void);

	virtual QRet<R> QCall() = 0;

	inline R DCall(const R& iDefault)
		{ return this->QCall().DGet(iDefault); }
	
	inline R Call()
		{ return this->QCall().Get(); }
	};

// =================================================================================================
#pragma mark - Callable (specialization for 0 params, ZP return)

template <class T>
class Callable<ZP<T>(void)>
:	public ZCounted
	{
public:
	typedef ZP<T> R;
	typedef ZP<T>(Signature)(void);

	virtual ZP<T> QCall() = 0;

	inline ZP<T> Call()
		{ return this->QCall(); }
	};

// =================================================================================================
#pragma mark - Callable (specialization for 0 params, void return)

template <>
class Callable<void(void)>
:	public ZCounted
	{
public:
	typedef void R;
	typedef void(Signature)(void);
	typedef void A_t;

	virtual bool QCall() = 0;

	inline void Call()
		{ this->QCall(); }
	};

// =================================================================================================
#pragma mark - Callable variants

#define ZMACRO_Callable_Callable(X) \
\
template <class R_p, ZMACRO_Callable_Class_P##X> \
class Callable<R_p(ZMACRO_Callable_P##X)> \
:	public ZCounted \
	{ \
public: \
	typedef R_p R; \
	ZMACRO_Callable_typedef_##X; \
	typedef R (Signature)(ZMACRO_Callable_P##X); \
	typedef ZMACRO_Callable_P##X A_t; \
\
	virtual QRet<R> QCall(ZMACRO_Callable_P##X) = 0; \
\
	inline R DCall(const R& iDefault, ZMACRO_Callable_VT##X) \
		{ return this->QCall(ZMACRO_Callable_i##X).DGet(iDefault); } \
\
	inline R Call(ZMACRO_Callable_VT##X) \
		{ return this->QCall(ZMACRO_Callable_i##X).Get(); } \
	}; \
\
template <class T, ZMACRO_Callable_Class_P##X> \
class Callable<ZP<T>(ZMACRO_Callable_P##X)> \
:	public ZCounted \
	{ \
public: \
	typedef void R; \
	ZMACRO_Callable_typedef_##X; \
	typedef void(Signature)(ZMACRO_Callable_P##X); \
	typedef ZMACRO_Callable_P##X A_t; \
\
	virtual ZP<T> QCall(ZMACRO_Callable_P##X) = 0; \
\
	inline ZP<T> Call(ZMACRO_Callable_VT##X) \
		{ return this->QCall(ZMACRO_Callable_i##X); } \
	}; \
\
template <ZMACRO_Callable_Class_P##X> \
class Callable<void(ZMACRO_Callable_P##X)> \
:	public ZCounted \
	{ \
public: \
	typedef void R; \
	ZMACRO_Callable_typedef_##X; \
	typedef void(Signature)(ZMACRO_Callable_P##X); \
	typedef ZMACRO_Callable_P##X A_t; \
\
	virtual QRet<void> QCall(ZMACRO_Callable_P##X) = 0; \
\
	inline void Call(ZMACRO_Callable_VT##X) \
		{ this->QCall(ZMACRO_Callable_i##X); } \
	}; \

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
#pragma mark - sCall variants (specialization for 0 params)

template <typename R_p, typename... A_p>
R_p sCallImp(const ZP<Callable<R_p(A_p...)>>& iCallable, A_p... iParams)
	{
	if (iCallable)
		return iCallable->Call(std::forward(iParams...));
	return R_p();
	}


template <typename... A_p>
void sCallImp(const ZP<Callable<void(A_p...)>>& iCallable, A_p... iParams)
	{
	if (iCallable)
		iCallable->Call(std::forward(iParams...));
	}

template <typename Type_p, typename R=typename Type_p::Type_t::R, typename A_p=typename Type_p::Type_t::A_t>
R sCall(const Type_P& iCallable, A_p... iParams)
	{ return sCallImp<R,A_p>(iCallable, std::forward(iParams...)); }


//template <typename R_p, typename... A_p>
//R_p sCall(const ZP<Callable<R_p(A_p...)>>& iCallable, A_p... iParams)
//	{
//	if (iCallable)
//		return iCallable->Call(iParams...);
//	return R_p();
//	}

//template <typename R_p, typename... A_p>
//R_p sDCall(const ZP<Callable<R_p(A_p...)>>& iCallable, const R_p& iDefault, A_p... iParams)
//	{
//	if (iCallable)
//		return iCallable->DCall(iDefault, iParams...);
//	return iDefault;
//	}
//
//template <typename R_p, typename... A_p>
//QRet<R_p> sQCall(const ZP<Callable<R_p(A_p...)>>& iCallable, A_p... iParams)
//	{
//	if (iCallable)
//		return iCallable->QCall(iParams...);
//	return QRet<R_p>();
//	}
//
//template <typename... A_p>
//void sCall(const ZP<Callable<void(A_p...)>>& iCallable, A_p... iParams)
//	{
//	if (iCallable)
//		iCallable->Call(iParams...);
//	}
//
//template <typename... A_p>
//bool sQCall(const ZP<Callable<void(A_p...)>>& iCallable, A_p... iParams)
//	{
//	if (iCallable)
//		return iCallable->QCall(iParams...);
//	return false;
//	}

//// =================================================================================================
//#pragma mark - sCall variants (specialization for 0 params)
//
//template <class Type_p>
//QRet<typename Type_p::Type_t::R> sQCall(
//	const Type_p& iCallable)
//	{
//	if (iCallable)
//		return iCallable->QCall();
//	return QRet<typename Type_p::Type_t::R>();
//	}
//
//template <class Type_p>
//typename Type_p::Type_t::R sDCall(
//	typename CallableUtil::VT<typename Type_p::Type_t::R>::P iDefault,
//	const Type_p& iCallable)
//	{
//	if (iCallable)
//		return iCallable->DCall()(iDefault);
//	return iDefault;
//	}
//
//template <class Type_p>
//typename Type_p::Type_t::R sCall(
//	const Type_p& iCallable)
//	{
//	if (iCallable)
//		return iCallable->Call();
//	return sDefault<typename Type_p::Type_t::R>();
//	}
//
//// =================================================================================================
//#pragma mark - sCall variants
//
//#define ZMACRO_Callable_Call(X) \
//\
//template <class Type_p, ZMACRO_Callable_Class_P##X> \
//QRet<typename Type_p::Type_t::R> sQCall( \
//	const Type_p& iCallable, \
//	ZMACRO_Callable_ConstRef_Pi##X) \
//	{ \
//	if (iCallable) \
//		return iCallable->QCall(ZMACRO_Callable_i##X); \
//	return QRet<typename Type_p::Type_t::R>(); \
//	} \
//\
//template <class Type_p, ZMACRO_Callable_Class_P##X> \
//typename Type_p::Type_t::R sDCall( \
//	typename CallableUtil::VT<typename Type_p::Type_t::R>::P iDefault, \
//	const Type_p& iCallable, \
//	ZMACRO_Callable_ConstRef_Pi##X) \
//	{ \
//	if (iCallable) \
//		return iCallable->DCall(iDefault, ZMACRO_Callable_i##X); \
//	return iDefault; \
//	} \
//\
//template <class Type_p, ZMACRO_Callable_Class_P##X> \
//typename Type_p::Type_t::R sCall( \
//	const Type_p& iCallable, \
//	ZMACRO_Callable_ConstRef_Pi##X) \
//	{ \
//	if (iCallable) \
//		return iCallable->Call(ZMACRO_Callable_i##X); \
//	return sDefault<typename Type_p::Type_t::R>(); \
//	}
//
//ZMACRO_Callable_Call(0)
//ZMACRO_Callable_Call(1)
//ZMACRO_Callable_Call(2)
//ZMACRO_Callable_Call(3)
//ZMACRO_Callable_Call(4)
//ZMACRO_Callable_Call(5)
//ZMACRO_Callable_Call(6)
//ZMACRO_Callable_Call(7)
//ZMACRO_Callable_Call(8)
//ZMACRO_Callable_Call(9)
//ZMACRO_Callable_Call(A)
//ZMACRO_Callable_Call(B)
//ZMACRO_Callable_Call(C)
//ZMACRO_Callable_Call(D)
//ZMACRO_Callable_Call(E)
//ZMACRO_Callable_Call(F)
//
//#undef ZMACRO_Callable_Call

// =================================================================================================
#pragma mark - sCallable

template <class Signature_p>
const ZRef<Callable<Signature_p> >& sCallable(const ZRef<Callable<Signature_p> >& iCallable)
	{ return iCallable; }

template <class ZRef_p>
const ZRef<Callable<typename ZRef_p::Type_t::Signature> >& sCallable(const ZRef_p& iCandidate)
	{ return (const ZRef<Callable<typename ZRef_p::Type_t::Signature> >&)(iCandidate); }

template <class Callable_p>
ZRef<Callable<typename Callable_p::Signature> > sCallable(Callable_p iCandidate)
	{ return ZRef<Callable<typename Callable_p::Signature> >(iCandidate); }

// =================================================================================================
#pragma mark - sCallVoid

template <class T>
void sCallVoid(ZRef<Callable<T(void)> > iCallable)
	{ sCall(iCallable); }

// =================================================================================================
#pragma mark - Useful typedefs

typedef Callable<void(void)> Callable_Void;

typedef Callable<bool(void)> Callable_Bool;

// =================================================================================================
#pragma mark - Callable_Null

class Callable_Null
:	public Callable_Void
	{
public:
// From Callable
	virtual bool QCall()
		{ return true; }
	};

} // namespace ZooLib

#endif // __ZooLib_Callable_h__
