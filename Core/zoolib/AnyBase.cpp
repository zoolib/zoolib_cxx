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
	if (fDistinguisher)
		return pAsInPlace().Type();

	if (const ZP<OnHeap>& theOnHeap = pAsOnHeap())
		return theOnHeap->Type();

	return typeid(void);
	}

const void* AnyBase::ConstVoidStar() const
	{
	if (fDistinguisher)
		return pAsInPlace().ConstVoidStar();

	if (const ZP<OnHeap>& theOnHeap = pAsOnHeap())
		return theOnHeap->ConstVoidStar();

	return nullptr;
	}

int AnyBase::pCompare(const AnyBase& iOther) const
	{
	if (fDistinguisher)
		{
		if (iOther.fDistinguisher)
			return pAsInPlace().Compare(iOther.pAsInPlace());

		if (const ZP<OnHeap>& otherOnHeap = iOther.pAsOnHeap())
			return pAsInPlace().Compare(*otherOnHeap.Get());

		return 1;
		}

	if (const ZP<OnHeap>& theOnHeap = pAsOnHeap())
		{
		if (iOther.fDistinguisher)
			return theOnHeap->Compare(iOther.pAsInPlace());

		if (const ZP<OnHeap>& otherOnHeap = iOther.pAsOnHeap())
			return theOnHeap->Compare(*otherOnHeap.Get());

		return 1;
		}

	if (iOther.fDistinguisher || iOther.pAsOnHeap())
		return -1;

	return 0;
	}


void AnyBase::Swap(AnyBase& ioOther)
	{
	if (fDistinguisher || ioOther.fDistinguisher)
		{
		// Trivial implementation for now
		AnyBase temp = std::move(*this);
		*this = std::move(ioOther);
		ioOther = std::move(temp);
		}
	else
		{
		swap(pAsOnHeap(), ioOther.pAsOnHeap());
		}
	}

bool AnyBase::IsNull() const
	{ return not fDistinguisher && not fPayload.fAsPtr; }

void AnyBase::Clear()
	{
	if (fDistinguisher)
		{
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
		return pAsInPlace().ConstVoidStarIfTypeMatch(iTypeInfo);

	if (const ZP<OnHeap>& theOnHeap = pAsOnHeap())
		return theOnHeap->ConstVoidStarIfTypeMatch(iTypeInfo);

	return nullptr;
	}

void* AnyBase::pFetchMutable(const std::type_info& iTypeInfo)
	{
	if (fDistinguisher)
		return pAsInPlace().MutableVoidStarIfTypeMatch(iTypeInfo);

	if (ZP<OnHeap>& theOnHeap = pAsOnHeap())
		return theOnHeap->FreshMutableVoidStarIfTypeMatch(theOnHeap, iTypeInfo);

	return nullptr;
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
