// Copyright (c) 2009-2019 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_AnyBase_h__
#define __ZooLib_AnyBase_h__
#include "zconfig.h"

#include "zoolib/CountedWithoutFinalize.h"
#include "zoolib/Compare_T.h"

#include "zoolib/ZP.h"
#include "zoolib/ZQ.h"

#include <cstring> // For strcmp
#include <typeinfo> // For std::type_info

namespace ZooLib {

// =================================================================================================
#pragma mark - AnyBase

template <class S>
struct AnyTraits
	{
	enum { eAllowInPlace = 1 };
	};

class AnyBase
	{
public:
	const std::type_info& Type() const;
	const void* ConstVoidStar() const;

	int pCompare(const AnyBase& iOther) const;

// ZVal protocol, generally for use by ZVal derivatives

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

	void Swap(AnyBase& ioOther);

protected:
	AnyBase()
		{
		fDistinguisher = 0;
		fPayload.fAsPtr = 0;
		}

	AnyBase(const AnyBase& iOther)
		{ pCtor(iOther); }

	~AnyBase()
		{ pDtor(); }

	AnyBase& operator=(const AnyBase& iOther)
		{
		if (this != &iOther)
			{
			pDtor();
			pCtor(iOther);
			}
		return *this;
		}

	template <class S>
	AnyBase(const S& iVal)
		{ pCtor_T<S>(iVal); }

	template <class S>
	AnyBase& operator=(const S& iVal)
		{
		pDtor();
		pCtor_T<S>(iVal);
		return *this;
		}

protected:
// Special purpose constructors, called by derived classes.
	typedef IKnowWhatIAmDoing_t IKWIAD_t;

	template <class S>
	AnyBase(const S* dummy,
		const IKWIAD_t&)
		{ this->pCtor_T<S>(); }

	template <class S, class P0>
	AnyBase(const S* dummy,
		const P0& iP0,
		const IKWIAD_t&)
		{ this->pCtor_T<S>(iP0); }

	template <class S, class P0, class P1>
	AnyBase(const S* dummy,
		const P0& iP0, const P1& iP1,
		const IKWIAD_t&)
		{ this->pCtor_T<S>(iP0, iP1); }

	template <class S>
	AnyBase(const S* dummy,
		const IKWIAD_t&, const IKWIAD_t&)
		{ this->pCtor_Counted_T<S>(); }

	template <class S, class P0>
	AnyBase(const S* dummy,
		const P0& iP0,
		const IKWIAD_t&, const IKWIAD_t&)
		{ this->pCtor_Counted_T<S>(iP0); }

	template <class S, class P0, class P1>
	AnyBase(const S* dummy,
		const P0& iP0, const P1& iP1,
		const IKWIAD_t&, const IKWIAD_t&)
		{ this->pCtor_Counted_T<S>(iP0, iP1); }

private:
// -----------------

	static bool spTypesMatch(const std::type_info& a, const std::type_info& b);

	typedef std::pair<const char*, const void*> InfoPair;

	class OnHeap;

	class InPlace
		{
	public:
		virtual ~InPlace() {}

		virtual InfoPair GetInfoPair() const = 0;

		virtual int Compare(const InPlace& iOther) const = 0;

		virtual int Compare(const OnHeap& iOther) const = 0;

		virtual void CtorInto(void* iOther) const = 0;

		virtual const std::type_info& Type() const = 0;

		virtual const void* ConstVoidStar() const = 0;

		virtual const void* ConstVoidStarIfTypeMatch(const std::type_info& iTI) const = 0;

		virtual void* MutableVoidStarIfTypeMatch(const std::type_info& iTI) = 0;
		};

// -----------------

	class OnHeap : public CountedWithoutFinalize
		{
	public:
		virtual InfoPair GetInfoPair() const = 0;

		virtual int Compare(const InPlace& iOther) const = 0;

		virtual int Compare(const OnHeap& iOther) const = 0;

		virtual const std::type_info& Type() const = 0;

		virtual const void* ConstVoidStar() = 0;

		virtual const void* ConstVoidStarIfTypeMatch(const std::type_info& iTI) = 0;

		virtual void* FreshMutableVoidStarIfTypeMatch(ZP<OnHeap>& ioOnHeap, const std::type_info& iTI) = 0;
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

		virtual InfoPair GetInfoPair() const
			{ return InfoPair(typeid(S).name(), &fValue); }

		virtual int Compare(const InPlace& iOther) const
			{
			InfoPair otherInfo = iOther.GetInfoPair();
			if (int compare = strcmp(typeid(S).name(), otherInfo.first))
				return compare;

			return sCompare_T(fValue, *static_cast<const S*>(otherInfo.second));
			}

		virtual int Compare(const OnHeap& iOther) const
			{
			InfoPair otherInfo = iOther.GetInfoPair();
			if (int compare = strcmp(typeid(S).name(), otherInfo.first))
				return compare;

			return sCompare_T(fValue, *static_cast<const S*>(otherInfo.second));
			}

		virtual void CtorInto(void* iOther) const { sCtor_T<InPlace_T>(iOther, fValue); }

		virtual const std::type_info& Type() const { return typeid(S); }

		virtual const void* ConstVoidStar() const { return &fValue; }

		virtual const void* ConstVoidStarIfTypeMatch(const std::type_info& iTI) const
			{
			if (spTypesMatch(iTI, typeid(S)))
				return &fValue;
			return nullptr;
			}

		virtual void* MutableVoidStarIfTypeMatch(const std::type_info& iTI)
			{
			if (spTypesMatch(iTI, typeid(S)))
				return &fValue;
			return nullptr;
			}

		S fValue;
		};

// -----------------

	template <typename S>
	class OnHeap_T : public OnHeap
		{
	public:
		OnHeap_T() {}

		template <class P0>
		OnHeap_T(const P0& iP0) : fValue(iP0) {}

		template <class P0, class P1>
		OnHeap_T(const P0& iP0, const P1& iP1) : fValue(iP0, iP1) {}

		virtual InfoPair GetInfoPair() const
			{ return InfoPair(typeid(S).name(), &fValue); }

		virtual int Compare(const InPlace& iOther) const
			{
			InfoPair otherInfo = iOther.GetInfoPair();
			if (int compare = strcmp(typeid(S).name(), otherInfo.first))
				return compare;

			return sCompare_T(fValue, *static_cast<const S*>(otherInfo.second));
			}

		virtual int Compare(const OnHeap& iOther) const
			{
			InfoPair otherInfo = iOther.GetInfoPair();
			if (int compare = strcmp(typeid(S).name(), otherInfo.first))
				return compare;

			return sCompare_T(fValue, *static_cast<const S*>(otherInfo.second));
			}

		virtual const std::type_info& Type() const { return typeid(S); }

		virtual const void* ConstVoidStar()
			{ return &fValue; }

		virtual const void* ConstVoidStarIfTypeMatch(const std::type_info& iTI)
			{
			if (spTypesMatch(iTI, typeid(S)))
				return &fValue;
			return nullptr;
			}

		virtual void* FreshMutableVoidStarIfTypeMatch(ZP<OnHeap>& ioOnHeap, const std::type_info& iTI)
			{
			if (spTypesMatch(iTI, typeid(S)))
				{
				if (this->IsShared())
					{
					OnHeap_T* theOnHeap = new OnHeap_T(fValue);
					ioOnHeap = theOnHeap;
					return &theOnHeap->fValue;
					}
				return &fValue;
				}
			return nullptr;
			}

		S fValue;
		};

// -----------------

	InPlace& pAsInPlace();
	const InPlace& pAsInPlace() const;

	ZP<OnHeap>& pAsOnHeap();
	const ZP<OnHeap>& pAsOnHeap() const;

// -----------------

	const void* pFetchConst(const std::type_info& iTypeInfo) const;
	void* pFetchMutable(const std::type_info& iTypeInfo);

	void pCtor(const AnyBase& iOther)
		{
		pCtor_NonPOD(iOther);
		}

	void pCtor_NonPOD(const AnyBase& iOther);

	void pDtor()
		{
		pDtor_NonPOD();
		}

	void pDtor_NonPOD();

// -----------------
	static long long countOfPod;
	static long long countOfNonPod;
	static long long countInPlace;
	static long long countOnHeap;


	template <class S>
	void pCtor_T()
		{
		if (std::is_pod<S>::value)
			++countOfPod;
		else
			++countOfNonPod;

		if (AnyTraits<S>::eAllowInPlace && sizeof(S) <= sizeof(fPayload))
			{
			++countInPlace;
			sCtor_T<InPlace_T<S>>(&fDistinguisher);
			}
		else
			{
			++countOnHeap;
			fDistinguisher = 0;
			sCtor_T<ZP<OnHeap>>(&fPayload, new OnHeap_T<S>());
			}
		}

	template <class S>
	void pCtor_Counted_T()
		{
		fDistinguisher = 0;
		sCtor_T<ZP<OnHeap>>(&fPayload, new OnHeap_T<S>());
		}

// -----

	template <class S, class P0>
	void pCtor_T(const P0& iP0)
		{
		if (std::is_pod<S>::value)
			++countOfPod;
		else
			++countOfNonPod;

		if (AnyTraits<S>::eAllowInPlace && sizeof(S) <= sizeof(fPayload))
			{
			++countInPlace;
			sCtor_T<InPlace_T<S>>(&fDistinguisher, iP0);
			}
		else
			{
			++countOnHeap;
			fDistinguisher = 0;
			sCtor_T<ZP<OnHeap>>(&fPayload, new OnHeap_T<S>(iP0));
			}
		}

	template <class S, class P0>
	void pCtor_Counted_T(const P0& iP0)
		{
		fDistinguisher = 0;
		sCtor_T<ZP<OnHeap>>(&fPayload, new OnHeap_T<S>(iP0));
		}

// -----

	template <class S, class P0, class P1>
	void pCtor_T(const P0& iP0, const P1& iP1)
		{
		if (std::is_pod<S>::value)
			++countOfPod;
		else
			++countOfNonPod;

		if (AnyTraits<S>::eAllowInPlace && sizeof(S) <= sizeof(fPayload))
			{
			++countInPlace;
			sCtor_T<InPlace_T<S>>(&fDistinguisher, iP0, iP1);
			}
		else
			{
			++countOnHeap;
			fDistinguisher = 0;
			sCtor_T<ZP<OnHeap>>(&fPayload, new OnHeap_T<S>(iP0, iP1));
			}
		}

	template <class S, class P0, class P1>
	void pCtor_Counted_T(const P0& iP0, const P1& iP1)
		{
		fDistinguisher = 0;
		sCtor_T<ZP<OnHeap>>(&fPayload, new OnHeap_T<S>(iP0, iP1));
		}

// -----------------

	template <class S, class P0>
	S& pCtorRet_T(const P0& iP0)
		{
		if (std::is_pod<S>::value)
			++countOfPod;
		else
			++countOfNonPod;

		if (AnyTraits<S>::eAllowInPlace && sizeof(S) <= sizeof(fPayload))
			{
			++countInPlace;
			return sCtor_T<InPlace_T<S>>(&fDistinguisher, iP0)->fValue;
			}
		else
			{
			++countOnHeap;
			fDistinguisher = 0;
			OnHeap_T<S>* theOnHeap = new OnHeap_T<S>(iP0);
			sCtor_T<ZP<OnHeap>>(&fPayload, theOnHeap);
			return theOnHeap->fValue;
			}
		}

	template <class S>
	S& pCtorRet_T()
		{
		if (AnyTraits<S>::eAllowInPlace && sizeof(S) <= sizeof(fPayload))
			{
			++countInPlace;
			return sCtor_T<InPlace_T<S>>(&fDistinguisher)->fValue;
			}
		else
			{
			++countOnHeap;
			fDistinguisher = 0;
			OnHeap_T<S>* theOnHeap = new OnHeap_T<S>;
			sCtor_T<ZP<OnHeap>>(&fPayload, theOnHeap);
			return theOnHeap->fValue;
			}
		}

// -----------------
	// There are three situations indicated by the value in fDistinguisher.
	// 1. It's zero. fPayload.fAsPtr points to an instance of an OnHeap subclass. If
	//    fPayload.fAsPtr is also null then the AnyBase is itself a null object.
	// 2. LSB is one. It points one byte past a typeid, and fPayload holds a POD value.
	// 3. LSB is zero. It's the vptr of an InPlace, the fields of the object itself
	//    spilling over into fPayload.
	
	void* fDistinguisher;

	union
		{
		// This union provides space for a refcounted pointer to an OnHeap, space
		// for the most common in-place values, and makes some values legible in a debugger.
		// It has its own name so that sizeof has something on which to operate.
		void* fAsPtr;

		bool fAsBool;
		char fAsChar;
		short fAsShort;
		int fAsInt;
		long fAsLong;
		__int64 fAsLongLong;
		float fAsFloat;
		double fAsDouble;
		} fPayload;
	};

// =================================================================================================
#pragma mark - Accessor functions

template <class S>
const S* sPGet(const AnyBase& iAny)
	{ return iAny.template PGet<S>(); }

template <class S>
const ZQ<S> sQGet(const AnyBase& iAny)
	{ return iAny.template QGet<S>(); }

template <class S>
const S& sDGet(const S& iDefault, const AnyBase& iAny)
	{ return iAny.template DGet<S>(iDefault); }

template <class S>
const S& sGet(const AnyBase& iAny)
	{ return iAny.template Get<S>(); }

template <class S>
S* sPMut(AnyBase& ioAny)
	{ return ioAny.template PMut<S>(); }

template <class S>
S& sDMut(const S& iDefault, AnyBase& ioAny)
	{ return ioAny.template DMut<S>(iDefault); }

template <class S>
S& sMut(AnyBase& ioAny)
	{ return ioAny.template Mut<S>(); }

template <class S>
S& sSet(AnyBase& ioAny, const S& iVal)
	{ return ioAny.template Set<S>(iVal); }

} // namespace ZooLib

#endif // __ZooLib_AnyBase_h__
