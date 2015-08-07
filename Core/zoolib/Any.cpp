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
#pragma mark -
#pragma mark Any

static inline 
const std::type_info* spPODTypeInfo(const void* iPtr)
	{ return (const std::type_info*)(((intptr_t)iPtr) ^ 1); }

inline
Any::InPlace& Any::pAsInPlace()
	{ return *sFetch_T<InPlace>(&fDistinguisher); }

inline
const Any::InPlace& Any::pAsInPlace() const
	{ return *sFetch_T<InPlace>(&fDistinguisher); }

inline
ZRef<Any::Reffed>& Any::pAsReffed()
	{ return *sFetch_T<ZRef<Reffed> >(&fPayload); }

inline
const ZRef<Any::Reffed>& Any::pAsReffed() const
	{ return *sFetch_T<ZRef<Reffed> >(&fPayload); }

const std::type_info& Any::Type() const
	{
	if (const std::type_info* theType = this->TypeIfNotVoid())
		return *theType;
	return typeid(void);
	}

const std::type_info* Any::TypeIfNotVoid() const
	{
	if (fDistinguisher)
		{
		if (spIsPOD(fDistinguisher))
			return spPODTypeInfo(fDistinguisher);
		return &pAsInPlace().Type();
		}
	else if (const ZRef<Reffed>& theReffed = pAsReffed())
		{
		return &theReffed->Type();
		}
	else
		{
		return nullptr;
		}
	}

void* Any::MutableVoidStar()
	{
	if (fDistinguisher)
		{
		if (spIsPOD(fDistinguisher))
			return &fPayload;
		return pAsInPlace().MutableVoidStar();
		}
	else if (ZRef<Reffed>& theReffed = pAsReffed())
		{
		return theReffed->FreshMutableVoidStar(theReffed);
		}
	else
		{
		return 0;
		}
	}

const void* Any::ConstVoidStar() const
	{
	if (fDistinguisher)
		{
		if (spIsPOD(fDistinguisher))
			return &fPayload;
		return pAsInPlace().ConstVoidStar();
		}
	else if (const ZRef<Reffed>& theReffed = pAsReffed())
		{
		return theReffed->ConstVoidStar();
		}
	else
		{
		return 0;
		}
	}

void Any::swap(Any& ioOther)
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

bool Any::IsNull() const
	{ return not fDistinguisher && not fPayload.fAsPtr; }

void Any::Clear()
	{
	if (fDistinguisher)
		{
		if (spNotPOD(fDistinguisher))
			sDtor_T<InPlace>(&fDistinguisher);
		fDistinguisher = 0;
		}
	else
		{
		sDtor_T<ZRef<Reffed> >(&fPayload);
		}
	fPayload.fAsPtr = 0;
	}

const void* Any::pFetchConst(const std::type_info& iTypeInfo) const
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
	else if (const ZRef<Reffed>& theReffed = pAsReffed())
		{
		return theReffed->ConstVoidStarIf(iTypeInfo);
		}

	return 0;
	}

void* Any::pFetchMutable(const std::type_info& iTypeInfo)
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
	else if (ZRef<Reffed>& theReffed = pAsReffed())
		{
		return theReffed->FreshMutableVoidStarIf(theReffed, iTypeInfo);
		}

	return 0;
	}

void Any::pCtor_NonPOD(const Any& iOther)
	{
	if (iOther.fDistinguisher)
		{
		iOther.pAsInPlace().CtorInto(&fDistinguisher);
		}
	else
		{
		fDistinguisher = 0;
		sCtor_T<ZRef<Reffed> >(&fPayload, iOther.pAsReffed());
		}
	}

void Any::pDtor_NonPOD()
	{
	if (fDistinguisher)
		sDtor_T<InPlace>(&fDistinguisher);
	else
		sDtor_T<ZRef<Reffed> >(&fPayload);
	}

} // namespace ZooLib
