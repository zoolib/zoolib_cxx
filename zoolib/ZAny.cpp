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

#include "zoolib/ZAny.h"

namespace ZooLib {

// =================================================================================================
// MARK: - ZAny

inline static const std::type_info* spPODTypeInfo(const void* iPtr)
	{ return (const std::type_info*)(((intptr_t)iPtr) ^ 1); }

inline ZAny::Holder_InPlace& ZAny::pAsInPlace()
	{ return *sFetch_T<Holder_InPlace>(&fDistinguisher); }

inline const ZAny::Holder_InPlace& ZAny::pAsInPlace() const
	{ return *sFetch_T<Holder_InPlace>(&fDistinguisher); }

inline ZRef<ZAny::Holder_Counted>& ZAny::pAsCounted()
	{ return *sFetch_T<ZRef<Holder_Counted> >(&fPayload); }

inline const ZRef<ZAny::Holder_Counted>& ZAny::pAsCounted() const
	{ return *sFetch_T<ZRef<Holder_Counted> >(&fPayload); }

const std::type_info& ZAny::Type() const
	{
	if (spIsPOD(fDistinguisher))
		{
		return *spPODTypeInfo(fDistinguisher);
		}
	else if (fDistinguisher)
		{
		return pAsInPlace().Type();
		}
	else if (const ZRef<Holder_Counted>& theHolderRef = pAsCounted())
		{
		return theHolderRef->Type();
		}
	else
		{
		return typeid(void);
		}
	}

void* ZAny::VoidStar()
	{
	if (spIsPOD(fDistinguisher))
		{
		return &fPayload;
		}
	else if (fDistinguisher)
		{
		return pAsInPlace().VoidStar();
		}
	else if (ZRef<Holder_Counted>& theHolderRef = pAsCounted())
		{
		if (theHolderRef->IsShared())
			theHolderRef = theHolderRef->Clone();
		return theHolderRef->VoidStar();
		}
	else
		{
		return 0;
		}
	}

const void* ZAny::ConstVoidStar() const
	{
	if (spIsPOD(fDistinguisher))
		{
		return &fPayload;
		}
	else if (fDistinguisher)
		{
		return pAsInPlace().ConstVoidStar();
		}
	else if (const ZRef<Holder_Counted>& theHolderRef = pAsCounted())
		{
		return theHolderRef->VoidStar();
		}
	else
		{
		return 0;
		}
	}

void ZAny::swap(ZAny& ioOther)
	{
	if (fDistinguisher || ioOther.fDistinguisher)
		{
		// Trivial implementation for now
		const ZAny temp = *this;
		*this = ioOther;
		ioOther = temp;
		}
	else
		{
		pAsCounted().swap(ioOther.pAsCounted());
		}
	}

bool ZAny::IsNull() const
	{ return not fDistinguisher && not fPayload.fAsPtr; }

void ZAny::Clear()
	{
	if (spIsPOD(fDistinguisher))
		{
		fDistinguisher = 0;
		}
	else if (fDistinguisher)
		{
		sDtor_T<Holder_InPlace>(&fDistinguisher);
		fDistinguisher = 0;
		}
	else
		{
		sDtor_T<ZRef<Holder_Counted> >(&fPayload);
		}
	fPayload.fAsPtr = 0;
	}

void* ZAny::pGetMutable(const std::type_info& iTypeInfo)
	{
	if (spIsPOD(fDistinguisher))
		{
		if (iTypeInfo == *spPODTypeInfo(fDistinguisher))
			return &fPayload;
		}
	else if (fDistinguisher)
		{
		return pAsInPlace().VoidStarIf(iTypeInfo);
		}
	else if (ZRef<Holder_Counted>& theHolderRef = pAsCounted())
		{
		if (theHolderRef->Type() == iTypeInfo)
			{
			if (theHolderRef->IsShared())
				theHolderRef = theHolderRef->Clone();
			return theHolderRef->VoidStar();
			}
		}

	return 0;
	}

const void* ZAny::pGet(const std::type_info& iTypeInfo) const
	{
	if (spIsPOD(fDistinguisher))
		{
		if (iTypeInfo == *spPODTypeInfo(fDistinguisher))
			return &fPayload;
		}
	else if (fDistinguisher)
		{
		return pAsInPlace().ConstVoidStarIf(iTypeInfo);
		}
	else if (const ZRef<Holder_Counted>& theHolderRef = pAsCounted())
		{
		return theHolderRef->VoidStarIf(iTypeInfo);
		}

	return 0;
	}

void ZAny::pCtor_Complex(const ZAny& iOther)
	{
	if (iOther.fDistinguisher)
		{
		iOther.pAsInPlace().CtorInto(&fDistinguisher);
		}
	else
		{
		fDistinguisher = 0;
		sCtor_T<ZRef<Holder_Counted> >(&fPayload, iOther.pAsCounted());
		}
	}

void ZAny::pDtor_Complex()
	{
	if (fDistinguisher)
		sDtor_T<Holder_InPlace>(&fDistinguisher);
	else
		sDtor_T<ZRef<Holder_Counted> >(&fPayload);
	}

} // namespace ZooLib
