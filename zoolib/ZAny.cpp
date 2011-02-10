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
#pragma mark -
#pragma mark * ZAny

ZAny::ZAny()
	{
	fPtr_InPlace = 0;
	fPtr_Counted = 0;
	}

ZAny::ZAny(const ZAny& iOther)
	{ pCtorFrom(iOther); }

ZAny::~ZAny()
	{ pDtor(); }

ZAny& ZAny::operator=(const ZAny& iOther)
	{
	if (this != & iOther)
		{
		pDtor();
		pCtorFrom(iOther);
		}
	return *this;
	}

ZAny::operator operator_bool() const
	{ return operator_bool_gen::translate(pIsInPlace() || fHolder_Counted()); }

const std::type_info& ZAny::Type() const
	{
	if (pIsInPlace())
		return fHolder_InPlace().Type();
	else if (const ZRef<Holder_Counted>& theHolderRef = fHolder_Counted())
		return theHolderRef->Type();
	return typeid(void);
	}

void* ZAny::VoidStar()
	{
	if (pIsInPlace())
		return fHolder_InPlace().VoidStar();

	if (ZRef<Holder_Counted>& theHolderRef = fHolder_Counted())
		{
		if (theHolderRef->IsShared())
			theHolderRef = theHolderRef->Clone();
		return theHolderRef->VoidStar();
		}

	return 0;
	}

const void* ZAny::ConstVoidStar() const
	{
	if (pIsInPlace())
		return fHolder_InPlace().ConstVoidStar();

	if (const ZRef<Holder_Counted>& theHolderRef = fHolder_Counted())
		return theHolderRef->VoidStar();

	return 0;
	}

void ZAny::swap(ZAny& ioOther)
	{
	if (pIsInPlace())
		{
		if (ioOther.pIsInPlace())
			{
			ZAny temp = *this;
			*this = ioOther;
			ioOther = *this;
			}
		else
			{
			ZRef<Holder_Counted> theRep = ioOther.fHolder_Counted();
			ioOther.pDtor();
			fHolder_InPlace().CtorInto(ioOther.fBytes_InPlace);
			pDtor();
			fPtr_InPlace = 0;
			sCtor_T<ZRef<Holder_Counted> >(fBytes_Counted, theRep);
			}
		}
	else if (ioOther.pIsInPlace())
		{
		ZRef<Holder_Counted> theRep = fHolder_Counted();
		pDtor();
		ioOther.fHolder_InPlace().CtorInto(fBytes_InPlace);
		ioOther.pDtor();
		ioOther.fPtr_InPlace = 0;
		sCtor_T<ZRef<Holder_Counted> >(ioOther.fBytes_Counted, theRep);
		}
	else
		{
		fHolder_Counted().swap(ioOther.fHolder_Counted());
		}
	}

void ZAny::Clear()
	{
	if (fPtr_InPlace)
		{
		sDtor_T<Holder_InPlace>(fBytes_InPlace);
		fPtr_InPlace = 0;
		}
	else
		{
		sDtor_T<ZRef<Holder_Counted> >(fBytes_Counted);
		}
	fPtr_Counted = 0;
	}

void* ZAny::pGetMutable(const std::type_info& iTypeInfo)
	{
	if (pIsInPlace())
		{
		Holder_InPlace& theHolderRef = fHolder_InPlace();
		if (theHolderRef.Type() == iTypeInfo)
			return theHolderRef.VoidStar();
		}
	else
		{
		if (ZRef<Holder_Counted>& theHolderRef = fHolder_Counted())
			{
			if (theHolderRef->Type() == iTypeInfo)
				{
				if (theHolderRef->IsShared())
					theHolderRef = theHolderRef->Clone();
				return theHolderRef->VoidStar();
				}
			}
		}
	return 0;
	}

const void* ZAny::pGet(const std::type_info& iTypeInfo) const
	{
	if (pIsInPlace())
		{
		const Holder_InPlace& theHolderRef = fHolder_InPlace();
		if (theHolderRef.Type() == iTypeInfo)
			return theHolderRef.ConstVoidStar();
		}
	else
		{
		if (const ZRef<Holder_Counted>& theHolderRef = fHolder_Counted())
			{
			if (theHolderRef->Type() == iTypeInfo)
				return theHolderRef->VoidStar();
			}
		}
	return 0;
	}

void ZAny::pCtorFrom(const ZAny& iOther)
	{
	if (iOther.pIsInPlace())
		{
		iOther.fHolder_InPlace().CtorInto(fBytes_InPlace);
		}
	else
		{
		fPtr_InPlace = 0;
		sCtor_T<ZRef<Holder_Counted> >(fBytes_Counted, iOther.fHolder_Counted());
		}
	}

void ZAny::pDtor()
	{
	if (pIsInPlace())
		sDtor_T<Holder_InPlace>(fBytes_InPlace);
	else
		sDtor_T<ZRef<Holder_Counted> >(fBytes_Counted);
	}

bool ZAny::pIsInPlace() const
	{ return fPtr_InPlace; }

ZAny::Holder_InPlace& ZAny::fHolder_InPlace()
	{ return *sFetch_T<Holder_InPlace>(fBytes_InPlace); }

const ZAny::Holder_InPlace& ZAny::fHolder_InPlace() const
	{ return *sFetch_T<Holder_InPlace>(fBytes_InPlace); }

ZRef<ZAny::Holder_Counted>& ZAny::fHolder_Counted()
	{ return *sFetch_T<ZRef<Holder_Counted> >(fBytes_Counted); }

const ZRef<ZAny::Holder_Counted>& ZAny::fHolder_Counted() const
	{ return *sFetch_T<ZRef<Holder_Counted> >(fBytes_Counted); }

} // namespace ZooLib
