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

#include "zoolib/Any.h"

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
ZP<AnyBase::Reffed>& AnyBase::pAsReffed()
	{ return *sFetch_T<ZP<Reffed> >(&fPayload); }

inline
const ZP<AnyBase::Reffed>& AnyBase::pAsReffed() const
	{ return *sFetch_T<ZP<Reffed> >(&fPayload); }

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
	else if (const ZP<Reffed>& theReffed = pAsReffed())
		{
		return &theReffed->Type();
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
	else if (ZP<Reffed>& theReffed = pAsReffed())
		{
		return theReffed->FreshMutableVoidStar(theReffed);
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
	else if (const ZP<Reffed>& theReffed = pAsReffed())
		{
		return theReffed->ConstVoidStar();
		}
	else
		{
		return 0;
		}
	}

void AnyBase::pSwap(AnyBase& ioOther)
	{
	if (fDistinguisher || ioOther.fDistinguisher)
		{
		// Trivial implementation for now
		const Any temp = *this;
		*this = ioOther;
		ioOther = temp;
		}
	else
		{
		pAsReffed().swap(ioOther.pAsReffed());
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
		sDtor_T<ZP<Reffed> >(&fPayload);
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
			return pAsInPlace().ConstVoidStarIf(iTypeInfo);
			}
		}
	else if (const ZP<Reffed>& theReffed = pAsReffed())
		{
		return theReffed->ConstVoidStarIf(iTypeInfo);
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
			return pAsInPlace().MutableVoidStarIf(iTypeInfo);
			}
		}
	else if (ZP<Reffed>& theReffed = pAsReffed())
		{
		return theReffed->FreshMutableVoidStarIf(theReffed, iTypeInfo);
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
		sCtor_T<ZP<Reffed> >(&fPayload, iOther.pAsReffed());
		}
	}

void AnyBase::pDtor_NonPOD()
	{
	if (fDistinguisher)
		sDtor_T<InPlace>(&fDistinguisher);
	else
		sDtor_T<ZP<Reffed> >(&fPayload);
	}

} // namespace ZooLib
