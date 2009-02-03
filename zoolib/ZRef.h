/* -------------------------------------------------------------------------------------------------
Copyright (c) 2009 Andrew Green
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

#ifndef __ZRef__
#define __ZRef__ 1
#include "zconfig.h"

#include "zoolib/ZCompat_algorithm.h"
#include "zoolib/ZCompat_operator_bool.h"

NAMESPACE_ZOOLIB_BEGIN

template <class T> void sRetain_T(T);
template <class T> void sRelease_T(T);

// =================================================================================================
#pragma mark -
#pragma mark * ZRef

template <typename T>
class ZRef
	{
private:
	typedef ZRef ThisType;

	ZOOLIB_DEFINE_OPERATOR_BOOL_TYPES_T(ThisType,
		operator_bool_generator_type, operator_bool_type);

	template <typename Ti>
	struct AsPtr_T
		{ typedef Ti* Type_t; };

	template <typename Ti>
	struct AsPtr_T<Ti*>
		{ typedef Ti* Type_t; };

	typedef typename AsPtr_T<T>::Type_t P;


	template <typename Ti, typename P>
	struct Retainable_T
		{
		static void sRetainIt(P iVal) { sRetain(*iVal); }
		static void sReleaseIt(P iVal) { sRelease(*iVal); }
		};

	template <typename Ti, typename Pi>
	struct Retainable_T<Ti*, Pi>
		{
		static void sRetainIt(Pi iVal) { sRetain_T(iVal); }
		static void sReleaseIt(Pi iVal) { sRelease_T(iVal); }
		};

	static void spRetain(P iP)
		{
		if (iP)
			Retainable_T<T, P>::sRetainIt(iP);
		}

	static void spRelease(P iP)
		{
		if (iP)
			Retainable_T<T, P>::sReleaseIt(iP);
		}

	static void spCheck(P iP)
		{
//		if (sCheckAccessEnabled(Retainable_T<T, P>::sGet(iP))
//			sCheckAccess(Retainable_T<T, P>::sGet(iP))
		}

public:
	ZRef()
	:	fP(nil)
		{}

	~ZRef()
		{ spRelease(fP); } 

	ZRef(P iP)
	:	fP(iP)
		{ spRetain(fP); }
	
	ZRef(bool iRetain, P iP)
	:	fP(iP)
		{
		if (iRetain)
			spRetain(fP);
		}

	ZRef& operator=(P iP)
		{
		// Important to do the retain after we've set fP,
		// so that ZRefCountedWithFinalization's Initialize is
		// called *after* we've taken the reference to it.
		std::swap(iP, fP);
		spRetain(fP);
		spRelease(iP);	
		return *this;
		}
	
	bool operator==(const P iP) const
		{ return fP == iP; }

	bool operator!=(const P iP) const
		{ return fP != iP; }

	ZRef(const ZRef& iOther)
	:	fP(iOther.Get())
		{ spRetain(fP); }

	ZRef& operator=(const ZRef& iOther)
		{
		ThisType(iOther).swap(*this);
		return *this;
		}

	bool operator==(const ZRef& iOther) const
		{ return fP == iOther.Get(); }

	bool operator!=(const ZRef& iOther) const
		{ return fP != iOther.Get(); }

	template <class O>
	ZRef(const ZRef<O>& iOther)
	:	fP(iOther.Get())
		{ spRetain(fP); }

	template <class O>
	ZRef& operator=(const ZRef<O>& iOther)
		{
		ThisType(iOther).swap(*this);
		return *this;
		}

	template <class O>
	bool operator==(const ZRef<O>& iOther) const
		{ return fP == iOther.Get(); }

	template <class O>
	bool operator!=(const ZRef<O>& iOther) const
		{ return fP != iOther.Get(); }

	template <class O>
	bool operator<(const ZRef<O>& iOther) const
		{ return fP < iOther.Get(); }

	operator P()
		{ return fP; }

	T operator*() const
		{ return fP; }

	P operator->() const
		{
		spCheck(fP);
		return fP;
		}

	operator operator_bool_type() const
		{ return operator_bool_generator_type::translate(fP); }

	void Clear()
		{
		ThisType().swap(*this);
		}

	P Get() const { return fP; }
	P GetObject() const { return fP; }

	void swap(ZRef& iOther) { std::swap(fP, iOther.fP); }

private:
	P fP;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZRef inline definitions

// =================================================================================================
#pragma mark -
#pragma mark * ZRef casts

template <class O, class T> inline O* ZRefDynamicCast(const ZRef<T>& iVal)
	{
	return dynamic_cast<O*>(iVal.Get());
	}

template <class O, class T> inline O* ZRefStaticCast(const ZRef<T>& iVal)
	{ return static_cast<O*>(iVal.Get()); }

// =================================================================================================

NAMESPACE_ZOOLIB_END

namespace std {
template <class T>
inline void swap(ZOOLIB_PREFIX::ZRef<T>& a, ZOOLIB_PREFIX::ZRef<T>& b)
	{ a.swap(b); }
}

#endif // __ZRef__
