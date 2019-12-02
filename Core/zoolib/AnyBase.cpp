// Copyright (c) 2009-2019 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/AnyBase.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - AnyBase

static inline 
const std::type_info* spPODTypeInfo(const void* iPtr)
	{ return (const std::type_info*)(((intptr_t)iPtr) ^ 1); }

bool AnyBase::spTypesMatch(const std::type_info& a, const std::type_info& b)
	{
	#if defined(ZCONFIG_typeinfo_comparison_broken)
		return 0 == strcmp(a.name(), b.name());
	#else
		return a == b;
	#endif
	}

inline
AnyBase::InPlace& AnyBase::pAsInPlace()
	{ return *sFetch_T<InPlace>(&fDistinguisher); }

inline
const AnyBase::InPlace& AnyBase::pAsInPlace() const
	{ return *sFetch_T<InPlace>(&fDistinguisher); }

inline
ZP<AnyBase::OnHeap>& AnyBase::pAsOnHeap()
	{ return *sFetch_T<ZP<OnHeap>>(&fPayload); }

inline
const ZP<AnyBase::OnHeap>& AnyBase::pAsOnHeap() const
	{ return *sFetch_T<ZP<OnHeap>>(&fPayload); }

const std::type_info& AnyBase::Type() const
	{
	if (const std::type_info* theType = this->TypeIfNotVoid())
		return *theType;
	return typeid(void);
	}

const std::type_info* AnyBase::TypeIfNotVoid() const
	{
	if (fDistinguisher)
		{
		if (spIsPOD(fDistinguisher))
			return spPODTypeInfo(fDistinguisher);
		return &pAsInPlace().Type();
		}
	else if (const ZP<OnHeap>& theOnHeap = pAsOnHeap())
		{
		return &theOnHeap->Type();
		}
	else
		{
		return nullptr;
		}
	}

void* AnyBase::MutableVoidStar()
	{
	if (fDistinguisher)
		{
		if (spIsPOD(fDistinguisher))
			return &fPayload;
		return pAsInPlace().MutableVoidStar();
		}
	else if (ZP<OnHeap>& theOnHeap = pAsOnHeap())
		{
		return theOnHeap->FreshMutableVoidStar(theOnHeap);
		}
	else
		{
		return 0;
		}
	}

const void* AnyBase::ConstVoidStar() const
	{
	if (fDistinguisher)
		{
		if (spIsPOD(fDistinguisher))
			return &fPayload;
		return pAsInPlace().ConstVoidStar();
		}
	else if (const ZP<OnHeap>& theOnHeap = pAsOnHeap())
		{
		return theOnHeap->ConstVoidStar();
		}
	else
		{
		return 0;
		}
	}

void AnyBase::swap(AnyBase& ioOther)
	{
	if (fDistinguisher || ioOther.fDistinguisher)
		{
		// Trivial implementation for now
		const AnyBase temp = *this;
		*this = ioOther;
		ioOther = temp;
		}
	else
		{
		pAsOnHeap().swap(ioOther.pAsOnHeap());
		}
	}

bool AnyBase::IsNull() const
	{ return not fDistinguisher && not fPayload.fAsPtr; }

void AnyBase::Clear()
	{
	if (fDistinguisher)
		{
		if (spNotPOD(fDistinguisher))
			sDtor_T<InPlace>(&fDistinguisher);
		fDistinguisher = 0;
		}
	else
		{
		sDtor_T<ZP<OnHeap>>(&fPayload);
		}
	fPayload.fAsPtr = 0;
	}

const void* AnyBase::pFetchConst(const std::type_info& iTypeInfo) const
	{
	if (fDistinguisher)
		{
		if (spIsPOD(fDistinguisher))
			{
			if (iTypeInfo == *spPODTypeInfo(fDistinguisher))
				return &fPayload;
			}
		else
			{
			return pAsInPlace().ConstVoidStarIfTypeMatch(iTypeInfo);
			}
		}
	else if (const ZP<OnHeap>& theOnHeap = pAsOnHeap())
		{
		return theOnHeap->ConstVoidStarIfTypeMatch(iTypeInfo);
		}

	return 0;
	}

void* AnyBase::pFetchMutable(const std::type_info& iTypeInfo)
	{
	if (fDistinguisher)
		{
		if (spIsPOD(fDistinguisher))
			{
			if (iTypeInfo == *spPODTypeInfo(fDistinguisher))
				return &fPayload;
			}
		else
			{
			return pAsInPlace().MutableVoidStarIfTypeMatch(iTypeInfo);
			}
		}
	else if (ZP<OnHeap>& theOnHeap = pAsOnHeap())
		{
		return theOnHeap->FreshMutableVoidStarIfTypeMatch(theOnHeap, iTypeInfo);
		}

	return 0;
	}

void AnyBase::pCtor_NonPOD(const AnyBase& iOther)
	{
	if (iOther.fDistinguisher)
		{
		iOther.pAsInPlace().CtorInto(&fDistinguisher);
		}
	else
		{
		fDistinguisher = 0;
		sCtor_T<ZP<OnHeap>>(&fPayload, iOther.pAsOnHeap());
		}
	}

void AnyBase::pDtor_NonPOD()
	{
	if (fDistinguisher)
		sDtor_T<InPlace>(&fDistinguisher);
	else
		sDtor_T<ZP<OnHeap>>(&fPayload);
	}

} // namespace ZooLib
