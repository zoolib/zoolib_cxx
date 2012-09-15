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

#ifndef __ZCallable_h__
#define __ZCallable_h__ 1
#include "zconfig.h"

#include "zoolib/ZCounted.h"
#include "zoolib/ZQ.h"

#define ZMACRO_Callable_PV
#define ZMACRO_Callable_P0 P0
#define ZMACRO_Callable_P1 ZMACRO_Callable_P0,P1
#define ZMACRO_Callable_P2 ZMACRO_Callable_P1,P2
#define ZMACRO_Callable_P3 ZMACRO_Callable_P2,P3
#define ZMACRO_Callable_P4 ZMACRO_Callable_P3,P4
#define ZMACRO_Callable_P5 ZMACRO_Callable_P4,P5
#define ZMACRO_Callable_P6 ZMACRO_Callable_P5,P6
#define ZMACRO_Callable_P7 ZMACRO_Callable_P6,P7
#define ZMACRO_Callable_P8 ZMACRO_Callable_P7,P8
#define ZMACRO_Callable_P9 ZMACRO_Callable_P8,P9
#define ZMACRO_Callable_PA ZMACRO_Callable_P9,PA
#define ZMACRO_Callable_PB ZMACRO_Callable_PA,PB
#define ZMACRO_Callable_PC ZMACRO_Callable_PB,PC
#define ZMACRO_Callable_PD ZMACRO_Callable_PC,PD
#define ZMACRO_Callable_PE ZMACRO_Callable_PD,PE
#define ZMACRO_Callable_PF ZMACRO_Callable_PE,PF

#define ZMACRO_Callable_iV
#define ZMACRO_Callable_i0 i0
#define ZMACRO_Callable_i1 ZMACRO_Callable_i0,i1
#define ZMACRO_Callable_i2 ZMACRO_Callable_i1,i2
#define ZMACRO_Callable_i3 ZMACRO_Callable_i2,i3
#define ZMACRO_Callable_i4 ZMACRO_Callable_i3,i4
#define ZMACRO_Callable_i5 ZMACRO_Callable_i4,i5
#define ZMACRO_Callable_i6 ZMACRO_Callable_i5,i6
#define ZMACRO_Callable_i7 ZMACRO_Callable_i6,i7
#define ZMACRO_Callable_i8 ZMACRO_Callable_i7,i8
#define ZMACRO_Callable_i9 ZMACRO_Callable_i8,i9
#define ZMACRO_Callable_iA ZMACRO_Callable_i9,iA
#define ZMACRO_Callable_iB ZMACRO_Callable_iA,iB
#define ZMACRO_Callable_iC ZMACRO_Callable_iB,iC
#define ZMACRO_Callable_iD ZMACRO_Callable_iC,iD
#define ZMACRO_Callable_iE ZMACRO_Callable_iD,iE
#define ZMACRO_Callable_iF ZMACRO_Callable_iE,iF

#define ZMACRO_Callable_VTV
#define ZMACRO_Callable_VT0 typename ZCallableUtil::VT<P0>::P i0
#define ZMACRO_Callable_VT1 ZMACRO_Callable_VT0,typename ZCallableUtil::VT<P1>::P i1
#define ZMACRO_Callable_VT2 ZMACRO_Callable_VT1,typename ZCallableUtil::VT<P2>::P i2
#define ZMACRO_Callable_VT3 ZMACRO_Callable_VT2,typename ZCallableUtil::VT<P3>::P i3
#define ZMACRO_Callable_VT4 ZMACRO_Callable_VT3,typename ZCallableUtil::VT<P4>::P i4
#define ZMACRO_Callable_VT5 ZMACRO_Callable_VT4,typename ZCallableUtil::VT<P5>::P i5
#define ZMACRO_Callable_VT6 ZMACRO_Callable_VT5,typename ZCallableUtil::VT<P6>::P i6
#define ZMACRO_Callable_VT7 ZMACRO_Callable_VT6,typename ZCallableUtil::VT<P7>::P i7
#define ZMACRO_Callable_VT8 ZMACRO_Callable_VT7,typename ZCallableUtil::VT<P8>::P i8
#define ZMACRO_Callable_VT9 ZMACRO_Callable_VT8,typename ZCallableUtil::VT<P9>::P i9
#define ZMACRO_Callable_VTA ZMACRO_Callable_VT9,typename ZCallableUtil::VT<PA>::P iA
#define ZMACRO_Callable_VTB ZMACRO_Callable_VTA,typename ZCallableUtil::VT<PB>::P iB
#define ZMACRO_Callable_VTC ZMACRO_Callable_VTB,typename ZCallableUtil::VT<PC>::P iC
#define ZMACRO_Callable_VTD ZMACRO_Callable_VTC,typename ZCallableUtil::VT<PD>::P iD
#define ZMACRO_Callable_VTE ZMACRO_Callable_VTD,typename ZCallableUtil::VT<PE>::P iE
#define ZMACRO_Callable_VTF ZMACRO_Callable_VTE,typename ZCallableUtil::VT<PF>::P iF

#define ZMACRO_Callable_Class_PV
#define ZMACRO_Callable_Class_P0 class P0
#define ZMACRO_Callable_Class_P1 ZMACRO_Callable_Class_P0,class P1
#define ZMACRO_Callable_Class_P2 ZMACRO_Callable_Class_P1,class P2
#define ZMACRO_Callable_Class_P3 ZMACRO_Callable_Class_P2,class P3
#define ZMACRO_Callable_Class_P4 ZMACRO_Callable_Class_P3,class P4
#define ZMACRO_Callable_Class_P5 ZMACRO_Callable_Class_P4,class P5
#define ZMACRO_Callable_Class_P6 ZMACRO_Callable_Class_P5,class P6
#define ZMACRO_Callable_Class_P7 ZMACRO_Callable_Class_P6,class P7
#define ZMACRO_Callable_Class_P8 ZMACRO_Callable_Class_P7,class P8
#define ZMACRO_Callable_Class_P9 ZMACRO_Callable_Class_P8,class P9
#define ZMACRO_Callable_Class_PA ZMACRO_Callable_Class_P9,class PA
#define ZMACRO_Callable_Class_PB ZMACRO_Callable_Class_PA,class PB
#define ZMACRO_Callable_Class_PC ZMACRO_Callable_Class_PB,class PC
#define ZMACRO_Callable_Class_PD ZMACRO_Callable_Class_PC,class PD
#define ZMACRO_Callable_Class_PE ZMACRO_Callable_Class_PD,class PE
#define ZMACRO_Callable_Class_PF ZMACRO_Callable_Class_PE,class PF

namespace ZooLib {

// =================================================================================================
// MARK: - ZCallableUtil

namespace ZCallableUtil {

template <class T> struct VT
	{
	typedef const T Field;
	typedef const T& Param;

	typedef Field F;
	typedef Param P;
	};

template <class T> struct VT<const T&>
	{
	typedef const T Field;
	typedef const T& Param;

	typedef Field F;
	typedef Param P;
	};

template <class T> struct VT<T&>
	{
	typedef T Field;
	typedef T& Param;

	typedef Field F;
	typedef Param P;
	};

} // namespace ZCallableUtil

// =================================================================================================
// MARK: - ZCallable

template <class Signature> class ZCallable;

// =================================================================================================
// MARK: - ZCallable (specialization for 0 params)

template <class R>
class ZCallable<R(void)>
:	public ZCounted
	{
public:
	typedef R(Signature)(void);

	virtual ZQ<R> QCall() = 0;

	inline R DCall(const R& iDefault)
		{ return this->QCall() .DGet(iDefault); }
	
	inline R Call()
		{ return this->QCall().Get(); }
	};

// =================================================================================================
// MARK: - ZCallable (specialization for 0 params, void return)

template <>
class ZCallable<void(void)>
:	public ZCounted
	{
public:
	typedef void(Signature)(void);

	virtual ZQ<void> QCall() = 0;

	inline void Call()
		{ this->QCall(); }
	};

// =================================================================================================
// MARK: - ZCallable variants

#define ZMACRO_Callable_Callable(X) \
template <class R, ZMACRO_Callable_Class_P##X> \
class ZCallable<R(ZMACRO_Callable_P##X)> \
:	public ZCounted \
	{ \
public: \
	typedef R(Signature)(ZMACRO_Callable_P##X); \
\
	virtual ZQ<R> QCall(ZMACRO_Callable_P##X) = 0; \
\
	inline R DCall(const R& iDefault, ZMACRO_Callable_VT##X) \
		{ return this->QCall(ZMACRO_Callable_i##X).DGet(iDefault); } \
\
	inline R Call(ZMACRO_Callable_VT##X) \
		{ return this->QCall(ZMACRO_Callable_i##X).Get(); } \
	}; \
\
template <ZMACRO_Callable_Class_P##X> \
class ZCallable<void(ZMACRO_Callable_P##X)> \
:	public ZCounted \
	{ \
public: \
	typedef void(Signature)(ZMACRO_Callable_P##X); \
\
	virtual ZQ<void> QCall(ZMACRO_Callable_P##X) = 0; \
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

// =================================================================================================
// MARK: - sCall variants (specialization for 0 params)

template <class R>
ZQ<R> sQCall
	(const ZRef<ZCallable<R(void)> >& iCallable)
	{
	if (iCallable)
		return iCallable->QCall();
	return null;
	}

template <class R>
R sDCall
	(typename ZCallableUtil::VT<R>::P iDefault,
	const ZRef<ZCallable<R(void)> >& iCallable)
	{
	if (iCallable)
		return iCallable->DCall(iDefault);
	return iDefault;
	}

template <class R>
R sCall
	(const ZRef<ZCallable<R(void)> >& iCallable)
	{
	if (iCallable)
		return iCallable->Call();
	return sDefault<R>();
	}

// =================================================================================================
// MARK: - sCall variants

#define ZMACRO_Callable_Call(X) \
\
template <class R, ZMACRO_Callable_Class_P##X> \
ZQ<R> sQCall \
	(const ZRef<ZCallable<R(ZMACRO_Callable_P##X)> >& iCallable, ZMACRO_Callable_VT##X) \
	{ \
	if (iCallable) \
		return iCallable->QCall(ZMACRO_Callable_i##X); \
	return null; \
	} \
\
template <class R, ZMACRO_Callable_Class_P##X> \
R sDCall \
	(typename ZCallableUtil::VT<R>::P iDefault, \
	const ZRef<ZCallable<R(ZMACRO_Callable_P##X)> >& iCallable, ZMACRO_Callable_VT##X) \
	{ \
	if (iCallable) \
		return iCallable->DCall(iDefault, ZMACRO_Callable_i##X); \
	return iDefault; \
	} \
\
template <class R, ZMACRO_Callable_Class_P##X> \
R sCall \
	(const ZRef<ZCallable<R(ZMACRO_Callable_P##X)> >& iCallable, ZMACRO_Callable_VT##X) \
	{ \
	if (iCallable) \
		return iCallable->Call(ZMACRO_Callable_i##X); \
	return sDefault<R>(); \
	}

ZMACRO_Callable_Call(0)
ZMACRO_Callable_Call(1)
ZMACRO_Callable_Call(2)
ZMACRO_Callable_Call(3)
ZMACRO_Callable_Call(4)
ZMACRO_Callable_Call(5)
ZMACRO_Callable_Call(6)
ZMACRO_Callable_Call(7)
ZMACRO_Callable_Call(8)
ZMACRO_Callable_Call(9)
ZMACRO_Callable_Call(A)
ZMACRO_Callable_Call(B)
ZMACRO_Callable_Call(C)
ZMACRO_Callable_Call(D)
ZMACRO_Callable_Call(E)
ZMACRO_Callable_Call(F)

// =================================================================================================
// MARK: - sCallable

template <class Signature>
ZRef<ZCallable<Signature> > sCallable(const ZRef<ZCallable<Signature> >& iCallable)
	{ return iCallable; }

// =================================================================================================
// MARK: - sCallVoid

template <class T>
void sCallVoid(ZRef<ZCallable<T(void)> > iCallable)
	{ sCall(iCallable); }

// =================================================================================================
// MARK: - Useful typedefs

typedef ZCallable<void(void)> ZCallable_Void;
typedef ZRef<ZCallable_Void> ZRef_ZCallable_Void;

typedef ZCallable<bool(void)> ZCallable_Bool;

// =================================================================================================
// MARK: - ZCallable_Null

class ZCallable_Null
:	public ZCallable_Void
	{
public:
// From ZCallable
	virtual ZQ<void> QCall()
		{ return notnull; }
	};

} // namespace ZooLib

#endif // __ZCallable_h__
