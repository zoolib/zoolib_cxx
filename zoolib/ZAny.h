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

#ifndef __ZAny__
#define __ZAny__
#include "zconfig.h"

#include "zoolib/ZCountedWithoutFinalize.h"
#include "zoolib/ZQ.h" 
#include "zoolib/ZRef.h"
#include "zoolib/ZTypes.h"

#include <typeinfo> // for std::type_info

#if ZCONFIG(Compiler,GCC)
	#include <tr1/type_traits> // For std::tr1::is_pod
#endif

// =================================================================================================
#pragma mark -
#pragma mark * ZAny

namespace ZooLib {

template <class S>
struct ZAnyTraits
	{
	enum { eAllowInPlace = 1 };
	};

class ZAny
	{
public:
	ZAny();
	ZAny(const ZAny& iOther);
	~ZAny();
	ZAny& operator=(const ZAny& iOther);

	template <class S>
	explicit ZAny(const S& iVal)
		{ pCtor_T<S>(iVal); }

	template <class S>
	ZAny& operator=(const S& iVal);

	const std::type_info& Type() const;

	void* VoidStar();
	const void* ConstVoidStar() const;

// ZVal protocol, generally for use by ZVal derivatives
	void swap(ZAny& ioOther);

	bool IsNull() const;

	void Clear();

	template <class S>
	S* PGetMutable()
		{ return static_cast<S*>(pGetMutable(typeid(S))); }

	template <class S>
	const S* PGet() const
		{ return static_cast<const S*>(pGet(typeid(S))); }

	template <class S>
	ZQ<S> QGet() const
		{
		if (const S* theVal = this->PGet<S>())
			return *theVal;
		return null;
		}

	template <class S>
	S DGet(const S& iDefault) const
		{
		if (const S* theVal = this->PGet<S>())
			return *theVal;
		return iDefault;
		}

	template <class S>
	S Get() const
		{
		if (const S* theVal = this->PGet<S>())
			return *theVal;
		return S();
		}

	template <class S>
	void Set(const S& iVal)
		{
		pDtor();
		pCtor_T<S>(iVal);
		}

// Our protocol
	template <class S>
	bool Is() const
		{ return this->PGet<S>(); }

// Special purpose constructors, called by sAny and sAnyCounted
	template <class S, class P0>
	ZAny(const S* dummy, const P0& iP0)
		{ pCtor_T<S>(iP0); }

	template <class S, class P0, class P1>
	ZAny(const S* dummy, const P0& iP0, const P1& iP1)
		{ pCtor_T<S>(iP0, iP1); }

	template <class S, class P0>
	ZAny(const S* dummy, const P0& iP0, const IKnowWhatIAmDoing_t&)
		{ pCtor_Counted_T<S>(iP0); }

	template <class S, class P0, class P1>
	ZAny(const S* dummy, const P0& iP0, const P1& iP1, const IKnowWhatIAmDoing_t&)
		{ pCtor_Counted_T<S>(iP0, iP1); }

private:
// -----------------

	class Holder_InPlace
		{
	public:
		virtual ~Holder_InPlace() {}

		virtual void CtorInto(void* iOther) const = 0;

		virtual const std::type_info& Type() const = 0;

		virtual void* VoidStar() = 0;
		virtual const void* ConstVoidStar() const = 0;

		virtual void* VoidStarIf(const std::type_info& iTI) = 0;
		virtual const void* ConstVoidStarIf(const std::type_info& iTI) const = 0;
		};

// -----------------

	template<typename S>
	class Holder_InPlace_T : public Holder_InPlace
		{
	public:
		template <class P0>
		Holder_InPlace_T(const P0& iP0) : fValue(iP0) {}

		template <class P0, class P1>
		Holder_InPlace_T(const P0& iP0, const P1& iP1) : fValue(iP0, iP1) {}

		virtual void CtorInto(void* iOther) const { sCtor_T<Holder_InPlace_T>(iOther, fValue); }

		virtual const std::type_info& Type() const { return typeid(S); }

		virtual void* VoidStar() { return &fValue; }
		virtual const void* ConstVoidStar() const { return &fValue; }

		virtual void* VoidStarIf(const std::type_info& iTI)
			{
			if (iTI == typeid(S))
				return &fValue;
			return 0;
			}
		virtual const void* ConstVoidStarIf(const std::type_info& iTI) const
			{
			if (iTI == typeid(S))
				return &fValue;
			return 0;
			}

		S fValue;
		};

// -----------------

	class Holder_Counted : public ZCountedWithoutFinalize
		{
	public:
		virtual const std::type_info& Type() const = 0;
		virtual Holder_Counted* Clone() const = 0;
		virtual void* VoidStar() = 0;
		virtual void* VoidStarIf(const std::type_info& iTI) = 0;
		};

// -----------------

	template<typename S>
	class Holder_Counted_T : public Holder_Counted
		{
	public:
		template <class P0>
		Holder_Counted_T(const P0& iP0) : fValue(iP0) {}

		template <class P0, class P1>
		Holder_Counted_T(const P0& iP0, const P1& iP1) : fValue(iP0, iP1) {}

		virtual const std::type_info& Type() const { return typeid(S); }
		virtual Holder_Counted* Clone() const { return new Holder_Counted_T(fValue); }
		virtual void* VoidStar() { return &fValue; }
		virtual void* VoidStarIf(const std::type_info& iTI)
			{
			if (iTI == typeid(S))
				return &fValue;
			return 0;
			}

		S fValue;
		};

// -----------------

	// Pseudo field accesors, hence the 'f' prefix
	Holder_InPlace& fHolder_InPlace();
	const Holder_InPlace& fHolder_InPlace() const;

	ZRef<Holder_Counted>& fHolder_Ref();
	const ZRef<Holder_Counted>& fHolder_Ref() const;

// -----------------

	void* pGetMutable(const std::type_info& iTypeInfo);
	const void* pGet(const std::type_info& iTypeInfo) const;

	void pCtor(const ZAny& iOther);
	void pCtor_Complex(const ZAny& iOther);

	void pDtor();
	void pDtor_Complex();

	static bool spIsPOD(const void* iPtr);

// -----------------

	template <class S, class P0, class P1>
	void pCtor_T(const P0& iP0, const P1& iP1)
		{
		if (ZAnyTraits<S>::eAllowInPlace && sizeof(S) <= sizeof(fPayload))
			{
			sCtor_T<Holder_InPlace_T<S> >(fBytes_InPlace, iP0, iP1);
			}
		else
			{
			fPtr_InPlace = 0;
			sCtor_T<ZRef<Holder_Counted> >(fBytes_Payload, new Holder_Counted_T<S>(iP0, iP1));
			}
		}

	template <class S, class P0, class P1>
	void pCtor_Counted_T(const P0& iP0, const P1& iP1)
		{
		fPtr_InPlace = 0;
		sCtor_T<ZRef<Holder_Counted> >(fBytes_Payload, new Holder_Counted_T<S>(iP0, iP1));
		}

	template <class S, class P0>
	void pCtor_T(const P0& iP0)
		{
		if (ZAnyTraits<S>::eAllowInPlace && sizeof(S) <= sizeof(fPayload))
			{
			if (false)
				{}
			#if ZCONFIG(Compiler,GCC)
			else if (std::tr1::is_pod<S>::value)
				{
				fPtr_InPlace = (void*)(((intptr_t)&typeid(S)) | 1);
				*((S*)fBytes_Payload) = iP0;
				}
			#endif
			else
				{
				sCtor_T<Holder_InPlace_T<S> >(fBytes_InPlace, iP0);
				}
			}
		else
			{
			fPtr_InPlace = 0;
			sCtor_T<ZRef<Holder_Counted> >(fBytes_Payload, new Holder_Counted_T<S>(iP0));
			}
		}

	template <class S, class P0>
	void pCtor_Counted_T(const P0& iP0)
		{
		fPtr_InPlace = 0;
		sCtor_T<ZRef<Holder_Counted> >(fBytes_Payload, new Holder_Counted_T<S>(iP0));
		}

// -----------------

	union
		{
		char fBytes_InPlace[1];
		void* fPtr_InPlace;
		};

	union
		{
		char fBytes_Payload[1];
		void* fPtr_Counted;
		union
			{
			// This union serves three purposes. It reserves space for in-place
			// values, ensures ZAny has appropriate alignment, and finally
			// makes some types legible when debugging.
			bool fAsBool;
			char fAsChar;
			short fAsShort;
			int fAsInt;
			long fAsLong;
			int64 fAsLongLong;
			float fAsFloat;
			double fAsDouble;
			} fPayload;
		};
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZAny, internal implementation inlines

inline bool ZAny::spIsPOD(const void* iPtr)
	{ return ((intptr_t)iPtr) & 1; }

inline void ZAny::pCtor(const ZAny& iOther)
	{
	if (spIsPOD(iOther.fPtr_InPlace))
		{
		fPtr_InPlace = iOther.fPtr_InPlace;
		fPayload = iOther.fPayload;
		}
	else
		{
		pCtor_Complex(iOther);
		}
	}

inline void ZAny::pDtor()
	{
	if (not spIsPOD(fPtr_InPlace))
		pDtor_Complex();
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZAny, public inlines

inline ZAny::ZAny()
	{
	fPtr_InPlace = 0;
	fPtr_Counted = 0;
	}

inline ZAny::ZAny(const ZAny& iOther)
	{ pCtor(iOther); }

inline ZAny::~ZAny()
	{ pDtor(); }

inline ZAny& ZAny::operator=(const ZAny& iOther)
	{
	if (this != & iOther)
		{
		pDtor();
		pCtor(iOther);
		}
	return *this;
	}

template <class S>
inline ZAny& ZAny::operator=(const S& iVal)
	{
	pDtor();
	pCtor_T<S>(iVal);
	return *this;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZAny, swap and pseudo-constructors

inline void swap(ZAny& a, ZAny& b)
	{ a.swap(b); }

template <class S, class P0>
ZAny sAny(const P0& iP0)
	{ return ZAny(static_cast<S*>(0), iP0); }

template <class S, class P0, class P1>
ZAny sAny(const P0& iP0, const P1& iP1)
	{ return ZAny(static_cast<S*>(0), iP0, iP1); }

template <class S, class P0>
ZAny sAnyCounted(const P0& iP0)
	{ return ZAny(static_cast<S*>(0), iP0, IKnowWhatIAmDoing); }

template <class S, class P0, class P1>
ZAny sAnyCounted(const P0& iP0, const P1& iP1)
	{ return ZAny(static_cast<S*>(0), iP0, iP1, IKnowWhatIAmDoing); }

} // namespace ZooLib

#endif // __ZAny__
