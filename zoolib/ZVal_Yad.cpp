/* -------------------------------------------------------------------------------------------------
Copyright (c) 2011 Andrew Green
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

#include "zoolib/ZVal_Yad.h"
#include "zoolib/ZYad_Any.h"

namespace ZooLib {

// =================================================================================================
// MARK: - ZVal_Yad

ZAny ZVal_Yad::AsAny() const
	{ return this->pAsAny(); }

ZVal_Yad::ZVal_Yad()
	{}

ZVal_Yad::ZVal_Yad(const ZVal_Yad& iOther)
:	fYad(iOther.fYad)
	{}

ZVal_Yad::~ZVal_Yad()
	{}

ZVal_Yad& ZVal_Yad::operator=(const ZVal_Yad& iOther)
	{
	fYad = iOther.fYad;
	return *this;
	}

ZVal_Yad::ZVal_Yad(const ZRef<ZYadR>& iYad)
:	fYad(iYad)
	{}

ZVal_Yad& ZVal_Yad::operator=(const ZRef<ZYadR>& iYad)
	{
	fYad = iYad;
	return *this;
	}

ZVal_Yad::ZVal_Yad(const ZSeq_Yad& iSeq)
:	fYad(iSeq.GetYad())
	{}

ZVal_Yad& ZVal_Yad::operator=(const ZSeq_Yad& iSeq)
	{
	fYad = iSeq.GetYad();
	return *this;
	}

ZVal_Yad::ZVal_Yad(const ZMap_Yad& iMap)
:	fYad(iMap.GetYad())
	{}

ZVal_Yad& ZVal_Yad::operator=(const ZMap_Yad& iMap)
	{
	fYad = iMap.GetYad();
	return *this;
	}

bool ZVal_Yad::IsNull() const
	{ return not fYad; }

ZQ<ZVal_Yad> ZVal_Yad::QGet(const string8& iName) const
	{
	if (ZRef<ZYadMapRPos> theYad = fYad.DynamicCast<ZYadMapRPos>())
		return ZMap_Yad(theYad).QGet(iName);
	return null;
	}

ZVal_Yad ZVal_Yad::Get(const string8& iName) const
	{
	if (ZRef<ZYadMapRPos> theYad = fYad.DynamicCast<ZYadMapRPos>())
		return ZMap_Yad(theYad).Get(iName);
	return ZVal_Yad();
	}

ZQ<ZVal_Yad> ZVal_Yad::QGet(const size_t& iIndex) const
	{
	if (ZRef<ZYadSeqRPos> theYad = fYad.DynamicCast<ZYadSeqRPos>())
		return ZSeq_Yad(theYad).QGet(iIndex);
	return null;
	}

ZVal_Yad ZVal_Yad::Get(const size_t& iIndex) const
	{
	if (ZRef<ZYadSeqRPos> theYad = fYad.DynamicCast<ZYadSeqRPos>())
		return ZSeq_Yad(theYad).Get(iIndex);
	return ZVal_Yad();
	}

template <>
ZQ<ZSeq_Yad> ZVal_Yad::QGet() const
	{
	if (ZRef<ZYadSeqRPos> theYad = fYad.DynamicCast<ZYadSeqRPos>())
		return ZSeq_Yad(theYad);
	return null;
	}

template <>
ZQ<ZMap_Yad> ZVal_Yad::QGet() const
	{
	if (ZRef<ZYadMapRPos> theYad = fYad.DynamicCast<ZYadMapRPos>())
		return ZMap_Yad(theYad);
	return null;
	}

ZRef<ZYadR> ZVal_Yad::GetYad() const
	{ return fYad; }

ZAny ZVal_Yad::pAsAny() const
	{
	if (fYad)
		return sFromYadR(false, ZVal_Any(), fYad);
	return ZAny();
	}

ZMACRO_ZValAccessors_Def_Get(ZVal_Yad, Seq, ZSeq_Yad)
ZMACRO_ZValAccessors_Def_Get(ZVal_Yad, Map, ZMap_Yad)

// =================================================================================================
// MARK: - ZSeq_Yad

ZSeq_Yad::ZSeq_Yad()
	{}

ZSeq_Yad::ZSeq_Yad(const ZSeq_Yad& iOther)
:	fYad(iOther.fYad)
	{}

ZSeq_Yad::~ZSeq_Yad()
	{}

ZSeq_Yad& ZSeq_Yad::operator=(const ZSeq_Yad& iOther)
	{
	fYad = iOther.fYad;
	return *this;
	}

ZSeq_Yad::ZSeq_Yad(const ZRef<ZYadSeqRPos>& iYad)
:	fYad(iYad)
	{}

ZSeq_Yad& ZSeq_Yad::operator=(const ZRef<ZYadSeqRPos>& iYad)
	{
	fYad = iYad;
	return *this;
	}

size_t ZSeq_Yad::Count() const
	{
	if (fYad)
		return fYad->GetSize();
	return 0;
	}

void ZSeq_Yad::Clear()
	{ fYad.Clear(); }

ZQ<ZVal_Yad> ZSeq_Yad::QGet(size_t iIndex) const
	{
	if (fYad)
		{
		if (fYad->IsShared())
			fYad = fYad->Clone().DynamicCast<ZYadSeqRPos>();
		if (ZRef<ZYadR> theYad = fYad->ReadAt(iIndex))
			return ZVal_Yad(theYad);
		}
	return null;
	}

ZVal_Yad ZSeq_Yad::DGet(const ZVal_Yad& iDefault, size_t iIndex) const
	{
	if (ZQ<ZVal_Yad> theQ = this->QGet(iIndex))
		return theQ.Get();
	return iDefault;
	}

ZVal_Yad ZSeq_Yad::Get(size_t iIndex) const
	{
	if (ZQ<ZVal_Yad> theQ = this->QGet(iIndex))
		return theQ.Get();
	return ZVal_Yad();
	}

ZRef<ZYadSeqRPos> ZSeq_Yad::GetYad() const
	{ return fYad; }

// =================================================================================================
// MARK: - ZMap_Yad

ZMap_Yad::ZMap_Yad()
	{}

ZMap_Yad::ZMap_Yad(const ZMap_Yad& iOther)
:	fYad(iOther.fYad)
	{}

ZMap_Yad::~ZMap_Yad()
	{}

ZMap_Yad& ZMap_Yad::operator=(const ZMap_Yad& iOther)
	{
	fYad = iOther.fYad;
	return *this;
	}

ZMap_Yad::ZMap_Yad(const ZRef<ZYadMapRPos>& iYad)
:	fYad(iYad)
	{}

ZMap_Yad& ZMap_Yad::operator=(const ZRef<ZYadMapRPos>& iYad)
	{
	fYad = iYad;
	return *this;
	}

void ZMap_Yad::Clear()
	{ fYad.Clear(); }

ZQ<ZVal_Yad> ZMap_Yad::QGet(const string8& iName) const
	{
	if (fYad)
		{
		if (fYad->IsShared())
			fYad = fYad->Clone().DynamicCast<ZYadMapRPos>();
		if (ZRef<ZYadR> theYad = fYad->ReadAt(iName))
			return ZVal_Yad(theYad);
		}
	return null;
	}

ZVal_Yad ZMap_Yad::DGet(const ZVal_Yad& iDefault, const string8& iName) const
	{
	if (ZQ<ZVal_Yad> theQ = this->QGet(iName))
		return theQ.Get();
	return iDefault;
	}

ZVal_Yad ZMap_Yad::Get(const string8& iName) const
	{
	if (ZQ<ZVal_Yad> theQ = this->QGet(iName))
		return theQ.Get();
	return ZVal_Yad();
	}

ZRef<ZYadMapRPos> ZMap_Yad::GetYad() const
	{ return fYad; }

} // namespace ZooLib
