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

namespace { // anonymous

class Tombstone_t {};

ZVal_Yad spVal_Null;

ZVal_Yad spAsVal_Yad(const ZRef<ZYadR>& iYadR)
	{
	if (not iYadR)
		return spVal_Null;

	if (ZRef<ZYadSeqAtR> theYad = iYadR.DynamicCast<ZYadSeqAtR>())
		return ZSeq_Yad(theYad);

	if (ZRef<ZYadMapAtR> theYad = iYadR.DynamicCast<ZYadMapAtR>())
		return ZMap_Yad(theYad);

	return sFromYadR(ZVal_Any(), iYadR).AsAny();
	}

} // anonymous namespace

// =================================================================================================
// MARK: - ZSeq_Yad

ZSeq_Yad::ZSeq_Yad()
	{}

ZSeq_Yad::ZSeq_Yad(const ZSeq_Yad& iOther)
:	fYad(iOther.fYad)
,	fSeq(iOther.fSeq)
	{}

ZSeq_Yad::~ZSeq_Yad()
	{}

ZSeq_Yad& ZSeq_Yad::operator=(const ZSeq_Yad& iOther)
	{
	fYad = iOther.fYad;
	fSeq = iOther.fSeq;
	return *this;
	}

ZSeq_Yad::ZSeq_Yad(const ZRef<ZYadSeqAtR>& iYad)
:	fYad(iYad)
	{}

ZSeq_Yad& ZSeq_Yad::operator=(const ZRef<ZYadSeqAtR>& iYad)
	{
	fYad = iYad;
	fSeq.Clear();
	return *this;
	}

size_t ZSeq_Yad::Count() const
	{
	if (fYad)
		return fYad->Count();
	return fSeq.Count();
	}

void ZSeq_Yad::Clear()
	{
	fYad.Clear();
	fSeq.Clear();
	}

ZQ<ZVal_Yad> ZSeq_Yad::QGet(size_t iIndex) const
	{
	if (fYad)
		{
		if (ZRef<ZYadR> theYad = fYad->ReadAt(iIndex))
			return spAsVal_Yad(theYad);
		return null;
		}

	if (ZQ<ZVal_Any> theQ = fSeq.QGet(iIndex))
		return theQ->AsAny();
		
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

ZSeq_Yad& ZSeq_Yad::Set(size_t iIndex, const ZVal_Yad& iVal)
	{
	this->pGenSeq();
	fSeq.Set(iIndex, iVal.AsAny());
	return *this;
	}

ZSeq_Yad& ZSeq_Yad::Erase(size_t iIndex)
	{
	this->pGenSeq();
	fSeq.Erase(iIndex);
	return *this;
	}

ZSeq_Yad& ZSeq_Yad::Insert(size_t iIndex, const ZVal_Yad& iVal)
	{
	this->pGenSeq();
	fSeq.Insert(iIndex, iVal.AsAny());
	return *this;
	}

ZSeq_Yad& ZSeq_Yad::Append(const ZVal_Yad& iVal)
	{
	this->pGenSeq();
	fSeq.Append(iVal.AsAny());
	return *this;
	}

ZRef<ZYadSeqAtR> ZSeq_Yad::GetYad() const
	{ return fYad; }

void ZSeq_Yad::pGenSeq()
	{
	if (not fYad)
		return;

	for (size_t x = 0, count = fYad->Count(); x < count ; ++x)
		fSeq.Append(spAsVal_Yad(fYad->ReadAt(x)));

	fYad.Clear();
	}

// =================================================================================================
// MARK: - ZMap_Yad

ZMap_Yad::ZMap_Yad()
	{}

ZMap_Yad::ZMap_Yad(const ZMap_Yad& iOther)
:	fYad(iOther.fYad)
,	fMap(iOther.fMap)
	{}

ZMap_Yad::~ZMap_Yad()
	{}

ZMap_Yad& ZMap_Yad::operator=(const ZMap_Yad& iOther)
	{
	fYad = iOther.fYad;
	fMap = iOther.fMap;
	return *this;
	}

ZMap_Yad::ZMap_Yad(const ZRef<ZYadMapAtR>& iYad)
:	fYad(iYad)
	{}

ZMap_Yad& ZMap_Yad::operator=(const ZRef<ZYadMapAtR>& iYad)
	{
	fYad = iYad;
	fMap.Clear();
	return *this;
	}

void ZMap_Yad::Clear()
	{
	fYad.Clear();
	fMap.Clear();
	}

ZQ<ZVal_Yad> ZMap_Yad::QGet(const string8& iName) const
	{
	ZMap_Any::Index_t theIndex = fMap.IndexOf(iName);
	if (theIndex != fMap.End())
		{
		const ZAny& theVal = fMap.Get(theIndex);
		if (theVal.PGet<Tombstone_t>())
			return null;
		return ZVal_Yad(theVal);
		}

	if (fYad)
		{
		if (ZRef<ZYadR> theYad = fYad->ReadAt(iName))
			{
			const ZVal_Yad theVal_Yad = spAsVal_Yad(theYad);
			fMap.Set(iName, ZVal_Any(theVal_Yad.AsAny()));
			return theVal_Yad;
			}
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

ZMap_Yad& ZMap_Yad::Set(const string8& iName, const ZVal_Yad& iVal)
	{
	fMap.Set(iName, iVal.AsAny());
	return *this;
	}

ZMap_Yad& ZMap_Yad::Erase(const string8& iName)
	{
	fMap.Set(iName, Tombstone_t());
	return *this;
	}

// Our protocol
ZVal_Yad& ZMap_Yad::Mutable(const string8& iName)
	{
	if (ZAny* theP = fMap.PGetMutable(iName))
		return *static_cast<ZVal_Yad*>(theP);
	
	ZAny& theMutable = fMap.Mutable(iName);
	if (fYad)
		{
		if (ZRef<ZYadR> theYad = fYad->ReadAt(iName))
			theMutable = spAsVal_Yad(theYad);
		}
	return static_cast<ZVal_Yad&>(theMutable);
	}

ZVal_Yad& ZMap_Yad::operator[](const string8& iName)
	{ return this->Mutable(iName); }

const ZVal_Yad& ZMap_Yad::operator[](const string8& iName) const
	{
	if (ZAny* theP = fMap.PGetMutable(iName))
		return *static_cast<ZVal_Yad*>(theP);
	
	if (fYad)
		{
		if (ZRef<ZYadR> theYad = fYad->ReadAt(iName))
			{
			ZAny& theMutable = fMap.Mutable(iName);
			theMutable = spAsVal_Yad(theYad);
			return static_cast<ZVal_Yad&>(theMutable);
			}
		}

	return spVal_Null;
	}

ZRef<ZYadMapAtR> ZMap_Yad::GetYad() const
	{ return fYad; }

} // namespace ZooLib
