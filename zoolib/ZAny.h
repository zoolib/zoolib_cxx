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

#ifndef __ZAny_h__
#define __ZAny_h__
#include "zconfig.h"

#include "zoolib/ZCompat_type_traits.h" // for is_pod
#include "zoolib/ZCountedWithoutFinalize.h"
#include "zoolib/ZQ.h" 
#include "zoolib/ZRef.h"

#include <typeinfo> // For std::type_info

// =================================================================================================
// MARK: - ZAny

namespace ZooLib {

template <class S>
struct ZAnyTraits
	{
	enum { eAllowInPlace = 1 };
	};

class ZAny
	{
public:
	const ZAny& AsAny() const
		{ return *this; }

	ZAny& AsAny()
		{ return *this; }

	ZAny()
		{
		fDistinguisher = 0;
		fPayload.fAsPtr = 0;
		}

	ZAny(const ZAny& iOther)
		{ pCtor(iOther); }

	~ZAny()
		{ pDtor(); }

	ZAny& operator=(const ZAny& iOther)
		{
		if (this != &iOther)
			{
			pDtor();
			pCtor(iOther);
			}
		return *this;
		}

	ZAny(const null_t&)
		{
		fDistinguisher = 0;
		fPayload.fAsPtr = 0;
		}

	ZAny& operator=(const null_t&)
		{
		this->Clear();
		return *this;
		}

	template <class S>
	explicit ZAny(const S& iVal)
		{ pCtor_T<S>(iVal); }

	template <class S>
	ZAny& operator=(const S& iVal)
		{
		pDtor();
		pCtor_T<S>(iVal);
		return *this;
		}

	const std::type_info& Type() const;

	void* MutableVoidStar();
	const void* ConstVoidStar() const;

// ZVal protocol, generally for use by ZVal derivatives
	void swap(ZAny& ioOther);

	bool IsNull() const;

	void Clear();

	template <class S>
	const S* PGet() const
		{ return static_cast<const S*>(pFetchConst(typeid(S))); }

	template <class S>
	const ZQ<S> QGet() const
		{
		if (const S* theP = this->PGet<S>())
			return *theP;
		return null;
		}

	template <class S>
	const S& DGet(const S& iDefault) const
		{
		if (const S* theP = this->PGet<S>())
			return *theP;
		return iDefault;
		}

	template <class S>
	const S& Get() const
		{
		if (const S* theP = this->PGet<S>())
			return *theP;
		return sDefault<S>();
		}

	template <class S>
	S* PMut()
		{ return static_cast<S*>(pFetchMutable(typeid(S))); }

	template <class S>
	S& DMut(const S& iDefault)
		{
		if (const S* theP = this->PGet<S>())
			return *theP;
		pDtor();
		return pCtorRet_T<S>(iDefault);
		}

	template <class S>
	S& Mut()
		{
		if (S* theP = this->PMut<S>())
			return *theP;
		pDtor();
		return pCtorRet_T<S>();
		}

	template <class S>
	S& Set(const S& iVal)
		{
		if (S* theP = this->PMut<S>())
			*theP = iVal;
		pDtor();
		return pCtorRet_T<S>(iVal);
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

	class InPlace
		{
	public:
		virtual ~InPlace() {}

		virtual void CtorInto(void* iOther) const = 0;

		virtual const std::type_info& Type() const = 0;

		virtual const void* ConstVoidStar() const = 0;

		virtual const void* ConstVoidStarIf(const std::type_info& iTI) const = 0;

		virtual void* MutableVoidStar() = 0;

		virtual void* MutableVoidStarIf(const std::type_info& iTI) = 0;
		};

// -----------------

	template <typename S>
	class InPlace_T : public InPlace
		{
	public:
		InPlace_T() {}

		template <class P0>
		InPlace_T(const P0& iP0) : fValue(iP0) {}

		template <class P0, class P1>
		InPlace_T(const P0& iP0, const P1& iP1) : fValue(iP0, iP1) {}

		virtual void CtorInto(void* iOther) const { sCtor_T<InPlace_T>(iOther, fValue); }

		virtual const std::type_info& Type() const { return typeid(S); }

		virtual const void* ConstVoidStar() const { return &fValue; }

		virtual const void* ConstVoidStarIf(const std::type_info& iTI) const
			{
			if (iTI == typeid(S))
				return &fValue;
			return 0;
			}

		virtual void* MutableVoidStar() { return &fValue; }

		virtual void* MutableVoidStarIf(const std::type_info& iTI)
			{
			if (iTI == typeid(S))
				return &fValue;
			return 0;
			}

		S fValue;
		};

// -----------------

	class Reffed : public ZCountedWithoutFinalize
		{
	public:
		virtual const std::type_info& Type() const = 0;

		virtual const void* ConstVoidStar() = 0;

		virtual const void* ConstVoidStarIf(const std::type_info& iTI) = 0;

		virtual void* FreshMutableVoidStar(ZRef<Reffed>& ioReffed) = 0;

		virtual void* FreshMutableVoidStarIf(ZRef<Reffed>& ioReffed, const std::type_info& iTI) = 0;
		};

// -----------------

	template <typename S>
	class Reffed_T : public Reffed
		{
	public:
		Reffed_T() {}

		template <class P0>
		Reffed_T(const P0& iP0) : fValue(iP0) {}

		template <class P0, class P1>
		Reffed_T(const P0& iP0, const P1& iP1) : fValue(iP0, iP1) {}

		virtual const std::type_info& Type() const { return typeid(S); }

		virtual const void* ConstVoidStar()
			{ return &fValue; }

		virtual const void* ConstVoidStarIf(const std::type_info& iTI)
			{
			if (iTI == typeid(S))
				return &fValue;
			return 0;
			}

		virtual void* FreshMutableVoidStar(ZRef<Reffed>& ioReffed)
			{
			if (this->IsShared())
				{
				Reffed_T* theReffed = new Reffed_T(fValue);
				ioReffed = theReffed;
				return &theReffed->fValue;
				}
			return &fValue;
			}

		virtual void* FreshMutableVoidStarIf(ZRef<Reffed>& ioReffed, const std::type_info& iTI)
			{
			if (iTI == typeid(S))
				{
				if (this->IsShared())
					{
					Reffed_T* theReffed = new Reffed_T(fValue);
					ioReffed = theReffed;
					return &theReffed->fValue;
					}
				return &fValue;
				}
			return 0;
			}

		S fValue;
		};

// -----------------

	InPlace& pAsInPlace();
	const InPlace& pAsInPlace() const;

	ZRef<Reffed>& pAsReffed();
	const ZRef<Reffed>& pAsReffed() const;

// -----------------

	const void* pFetchConst(const std::type_info& iTypeInfo) const;
	void* pFetchMutable(const std::type_info& iTypeInfo);

	void pCtor(const ZAny& iOther)
		{
		if (spNotPOD(iOther.fDistinguisher))
			{
			pCtor_NonPOD(iOther);
			}
		else
			{
			fDistinguisher = iOther.fDistinguisher;
			fPayload = iOther.fPayload;
			}
		}

	void pCtor_NonPOD(const ZAny& iOther);

	void pDtor()
		{
		if (spNotPOD(fDistinguisher))
			pDtor_NonPOD();
		}

	void pDtor_NonPOD();

	static bool spIsPOD(const void* iPtr)
		{ return ((intptr_t)iPtr) & 1; }

	static bool spNotPOD(const void* iPtr)
		{ return not spIsPOD(iPtr); }

// -----------------

	template <class S, class P0, class P1>
	void pCtor_T(const P0& iP0, const P1& iP1)
		{
		if (ZAnyTraits<S>::eAllowInPlace && sizeof(S) <= sizeof(fPayload))
			{
			sCtor_T<InPlace_T<S> >(&fDistinguisher, iP0, iP1);
			}
		else
			{
			fDistinguisher = 0;
			sCtor_T<ZRef<Reffed> >(&fPayload, new Reffed_T<S>(iP0, iP1));
			}
		}

	template <class S, class P0, class P1>
	void pCtor_Counted_T(const P0& iP0, const P1& iP1)
		{
		fDistinguisher = 0;
		sCtor_T<ZRef<Reffed> >(&fPayload, new Reffed_T<S>(iP0, iP1));
		}

	template <class S, class P0>
	void pCtor_T(const P0& iP0)
		{
		if (ZAnyTraits<S>::eAllowInPlace && sizeof(S) <= sizeof(fPayload))
			{
			if (false)
				{}
			#if ZCONFIG_SPI_Enabled(type_traits)
			else if (is_pod<S>::value)
				{
				fDistinguisher = (void*)(((intptr_t)&typeid(S)) | 1);
				sCtor_T<S>(&fPayload, iP0);
				}
			#endif
			else
				{
				sCtor_T<InPlace_T<S> >(&fDistinguisher, iP0);
				}
			}
		else
			{
			fDistinguisher = 0;
			sCtor_T<ZRef<Reffed> >(&fPayload, new Reffed_T<S>(iP0));
			}
		}

	template <class S, class P0>
	void pCtor_Counted_T(const P0& iP0)
		{
		fDistinguisher = 0;
		sCtor_T<ZRef<Reffed> >(&fPayload, new Reffed_T<S>(iP0));
		}

// -----------------

	template <class S, class P0>
	S& pCtorRet_T(const P0& iP0)
		{
		if (ZAnyTraits<S>::eAllowInPlace && sizeof(S) <= sizeof(fPayload))
			{
			if (false)
				{}
			#if ZCONFIG_SPI_Enabled(type_traits)
			else if (is_pod<S>::value)
				{
				fDistinguisher = (void*)(((intptr_t)&typeid(S)) | 1);
				return *sCtor_T<S>(&fPayload, iP0);
				}
			#endif
			else
				{
				return sCtor_T<InPlace_T<S> >(&fDistinguisher, iP0)->fValue;
				}
			}
		else
			{
			fDistinguisher = 0;
			Reffed_T<S>* theReffed = new Reffed_T<S>(iP0);
			sCtor_T<ZRef<Reffed> >(&fPayload, theReffed);
			return theReffed->fValue;
			}
		}

	template <class S>
	S& pCtorRet_T()
		{
		if (ZAnyTraits<S>::eAllowInPlace && sizeof(S) <= sizeof(fPayload))
			{
			if (false)
				{}
			#if ZCONFIG_SPI_Enabled(type_traits)
			else if (is_pod<S>::value)
				{
				fDistinguisher = (void*)(((intptr_t)&typeid(S)) | 1);
				return *sCtor_T<S>(&fPayload);
				}
			#endif
			else
				{
				return sCtor_T<InPlace_T<S> >(&fDistinguisher)->fValue;
				}
			}
		else
			{
			fDistinguisher = 0;
			Reffed_T<S>* theReffed = new Reffed_T<S>;
			sCtor_T<ZRef<Reffed> >(&fPayload, theReffed);
			return theReffed->fValue;
			}
		}

// -----------------
	// There are three situations indicated by the value in fDistinguisher.
	// 1. It's zero. fPayload.fAsPtr points to an instance of a Reffed subclass. If
	//    fPayload.fAsPtr is also null then the ZAny is itself a null object.
	// 2. LSB is one. It points one byte past a typeid, and fPayload holds a POD value.
	// 3. LSB is zero. It's the vptr of an InPlace, the fields of the object itself
	//    spilling over into fPayload.
	
	void* fDistinguisher;

	union
		{
		// This union provides space for a refcounted pointer to a Reffed, space
		// for the most common in-place values, and makes some values legible in a debugger.
		// It has its own name so that sizeof has something on which to operate.
		void* fAsPtr;

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

// =================================================================================================
// MARK: - Accessor functions

template <class S>
const S* sPGet(const ZAny& iAny)
	{ return iAny.PGet<S>(); }

template <class S>
const ZQ<S> sQGet(const ZAny& iAny)
	{ return iAny.QGet<S>(); }

template <class S>
const S& sDGet(const S& iDefault, const ZAny& iAny)
	{ return iAny.DGet<S>(iDefault); }

template <class S>
const S& sGet(const ZAny& iAny)
	{ return iAny.Get<S>(); }

template <class S>
S* sPMut(ZAny& ioAny)
	{ return ioAny.PMut<S>(); }

template <class S>
S& sDMut(const S& iDefault, ZAny& ioAny)
	{ return ioAny.DMut<S>(iDefault); }

template <class S>
S& sMut(ZAny& ioAny)
	{ return ioAny.Mut<S>(); }

template <class S>
S& sSet(ZAny& ioAny, const S& iVal)
	{ return ioAny.Set<S>(iVal); }

// =================================================================================================
// MARK: - ZAny, swap and pseudo-constructors

inline void swap(ZAny& a, ZAny& b)
	{ a.swap(b); }

template <class S>
ZAny sAny()
	{ return ZAny(static_cast<S*>(0)); }

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

#endif // __ZAny_h__
