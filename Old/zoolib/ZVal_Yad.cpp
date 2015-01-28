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

#include "zoolib/Util_STL_set.h"

#include "zoolib/ZVal_Yad.h"
#include "zoolib/ZYad_Any.h"

namespace ZooLib {

using std::set;
using std::string;

using namespace Util_STL;

namespace { // anonymous

class Tombstone_t {};

ZVal_Yad spAsVal_Yad(const ZRef<ZYadR>& iYadR)
	{
	if (not iYadR)
		return sDefault<ZVal_Yad>();

	if (ZRef<ZYadSeqAtRPos> theYad = iYadR.DynamicCast<ZYadSeqAtRPos>())
		return ZSeq_Yad(theYad);

	if (ZRef<ZYadMapAtRPos> theYad = iYadR.DynamicCast<ZYadMapAtRPos>())
		return ZMap_Yad(theYad);

	return sFromYadR(ZVal_Any(), iYadR).AsAny();
	}

} // anonymous namespace

// =================================================================================================
#pragma mark -
#pragma mark ZSeq_Yad

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

ZSeq_Yad::ZSeq_Yad(const ZRef<ZYadSeqAtRPos>& iYad)
:	fYad(iYad)
	{}

ZSeq_Yad& ZSeq_Yad::operator=(const ZRef<ZYadSeqAtRPos>& iYad)
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

const ZVal_Yad* ZSeq_Yad::PGet(size_t iIndex) const
	{
	if (fYad)
		{
		for (const size_t yadCount = fYad->Count();
			iIndex < yadCount && iIndex >= fSeq.Count();
			/*no inc*/)
			{ fSeq.Append(Tombstone_t()); }
		}

	if (ZAny* theP = fSeq.PMut(iIndex))
		{
		if (theP->PGet<Tombstone_t>())
			*theP = spAsVal_Yad(fYad->ReadAt(iIndex)).AsAny();
		return static_cast<ZVal_Yad*>(theP);
		}

	return nullptr;
	}

ZQ<ZVal_Yad> ZSeq_Yad::QGet(size_t iIndex) const
	{
	if (const ZVal_Yad* theVal = this->PGet(iIndex))
		return *theVal;
	return null;
	}

const ZVal_Yad& ZSeq_Yad::DGet(const ZVal_Yad& iDefault, size_t iIndex) const
	{
	if (const ZVal_Yad* theVal = this->PGet(iIndex))
		return *theVal;
	return iDefault;
	}

const ZVal_Yad& ZSeq_Yad::Get(size_t iIndex) const
	{
	if (const ZVal_Yad* theVal = this->PGet(iIndex))
		return *theVal;
	return sDefault<ZVal_Yad>();
	}

ZVal_Yad* ZSeq_Yad::PMut(size_t iIndex)
	{
	if (fYad)
		{
		for (const size_t yadCount = fYad->Count();
			iIndex < yadCount && iIndex >= fSeq.Count();
			/*no inc*/)
			{ fSeq.Append(Tombstone_t()); }
		}

	if (ZAny* theP = fSeq.PMut(iIndex))
		{
		if (theP->PGet<Tombstone_t>())
			*theP = spAsVal_Yad(fYad->ReadAt(iIndex)).AsAny();
		return static_cast<ZVal_Yad*>(theP);
		}

	return nullptr;
	}

ZVal_Yad& ZSeq_Yad::Mut(size_t iIndex)
	{
	ZVal_Yad* theP = this->PMut(iIndex);
	ZAssert(theP);
	return *theP;
	}

ZSeq_Yad& ZSeq_Yad::Set(size_t iIndex, const ZVal_Yad& iVal)
	{
	if (fYad)
		{
		for (const size_t yadCount = fYad->Count();
			iIndex < yadCount && iIndex >= fSeq.Count();
			/*no inc*/)
			{ fSeq.Append(Tombstone_t()); }
		}
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

ZVal_Yad& ZSeq_Yad::operator[](size_t iIndex)
	{ return this->Mut(iIndex); }

const ZVal_Yad& ZSeq_Yad::operator[](size_t iIndex) const
	{
	if (const ZVal_Yad* theVal = this->PGet(iIndex))
		return *theVal;
	return sDefault<ZVal_Yad>();
	}

ZRef<ZYadSeqAtRPos> ZSeq_Yad::GetYad() const
	{ return fYad; }

ZSeq_Any ZSeq_Yad::GetSeq() const
	{ return fSeq; }

void ZSeq_Yad::pGenSeq()
	{
	if (not fYad)
		return;

	for (size_t x = 0, count = fYad->Count(); x < count ; ++x)
		{
		if (fSeq.Count() == x)
			{
			fSeq.Append(spAsVal_Yad(fYad->ReadAt(x)).AsAny());
			}
		else if (fSeq.PGet<Tombstone_t>(x))
			{
			fSeq.Set(x, spAsVal_Yad(fYad->ReadAt(x)).AsAny());
			}
		}

	fYad.Clear();
	}

// =================================================================================================
#pragma mark -
#pragma mark ZMap_Yad

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

ZMap_Yad::ZMap_Yad(const ZRef<ZYadMapAtRPos>& iYad)
:	fYad(iYad)
	{}

ZMap_Yad& ZMap_Yad::operator=(const ZRef<ZYadMapAtRPos>& iYad)
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

const ZVal_Yad* ZMap_Yad::PGet(const Name_t& iName) const
	{
	ZMap_Any::Index_t theIndex = fMap.IndexOf(iName);
	if (theIndex != fMap.End())
		{
		if (const ZAny* theAny = fMap.PGet(theIndex))
			{
			if (theAny->PGet<Tombstone_t>())
				return nullptr;
			return static_cast<const ZVal_Yad*>(theAny);
			}
		}

	if (fYad)
		{
		if (ZRef<ZYadR> theYad = fYad->ReadAt(iName))
			{
			fMap.Set(iName, ZVal_Any(spAsVal_Yad(theYad).AsAny()));
			return static_cast<const ZVal_Yad*>(static_cast<const ZAny*>(fMap.PGet(iName)));
			}
		}

	fMap.Set(iName, Tombstone_t());
	return nullptr;
	}

ZQ<ZVal_Yad> ZMap_Yad::QGet(const Name_t& iName) const
	{
	if (const ZVal_Yad* theVal = this->PGet(iName))
		return *theVal;
	return null;
	}

const ZVal_Yad& ZMap_Yad::DGet(const ZVal_Yad& iDefault, const Name_t& iName) const
	{
	if (const ZVal_Yad* theVal = this->PGet(iName))
		return *theVal;
	return iDefault;
	}

const ZVal_Yad& ZMap_Yad::Get(const Name_t& iName) const
	{
	if (const ZVal_Yad* theVal = this->PGet(iName))
		return *theVal;
	return sDefault<ZVal_Yad>();
	}

ZVal_Yad* ZMap_Yad::PMut(const Name_t& iName)
	{
	ZMap_Any::Index_t theIndex = fMap.IndexOf(iName);
	if (theIndex != fMap.End())
		{
		if (ZAny* theAny = fMap.PMut(theIndex))
			{
			if (theAny->PGet<Tombstone_t>())
				return nullptr;
			return static_cast<ZVal_Yad*>(theAny);
			}
		}

	if (fYad)
		{
		if (ZRef<ZYadR> theYad = fYad->ReadAt(iName))
			{
			fMap.Set(iName, ZVal_Any(spAsVal_Yad(theYad).AsAny()));
			return static_cast<ZVal_Yad*>(static_cast<ZAny*>(fMap.PMut(iName)));
			}
		}

	fMap.Set(iName, Tombstone_t());
	return nullptr;
	}

ZVal_Yad& ZMap_Yad::Mut(const Name_t& iName)
	{
	if (ZAny* theP = fMap.PMut(iName))
		{
		if (theP->PGet<Tombstone_t>())
			*theP = ZAny();			
		return *static_cast<ZVal_Yad*>(theP);
		}
	
	ZAny& theMutable = fMap.Mut(iName);
	if (fYad)
		{
		if (ZRef<ZYadR> theYad = fYad->ReadAt(iName))
			theMutable = spAsVal_Yad(theYad).AsAny();
		}

	return static_cast<ZVal_Yad&>(theMutable);
	}

ZMap_Yad& ZMap_Yad::Set(const Name_t& iName, const ZVal_Yad& iVal)
	{
	fMap.Set(iName, iVal.AsAny());
	return *this;
	}

ZMap_Yad& ZMap_Yad::Erase(const Name_t& iName)
	{
	fMap.Set(iName, Tombstone_t());
	return *this;
	}

ZVal_Yad& ZMap_Yad::operator[](const Name_t& iName)
	{ return this->Mut(iName); }

const ZVal_Yad& ZMap_Yad::operator[](const Name_t& iName) const
	{
	if (const ZVal_Yad* theVal = this->PGet(iName))
		return *theVal;
	return sDefault<ZVal_Yad>();
	}

ZRef<ZYadMapAtRPos> ZMap_Yad::GetYad() const
	{ return fYad; }

ZMap_Any ZMap_Yad::GetMap() const
	{ return fMap; }

// =================================================================================================
#pragma mark -
#pragma mark YadMapAtRPos

namespace { // anonymous

class YadMapAtRPos
:	public ZYadMapAtRPos
	{
	YadMapAtRPos(const YadMapAtRPos& iOther)
	:	fYad(iOther.fYad->Clone().DynamicCast<ZYadMapAtRPos>())
	,	fMap(iOther.fMap)
	,	fCurrent(fMap.IndexOf(iOther.fMap, iOther.fCurrent))
		{}

public:
	YadMapAtRPos(ZRef<ZYadMapAtRPos> iYad, const ZMap_Any& iMap)
	:	fYad(iYad)
	,	fMap(iMap)
	,	fCurrent(fMap.Begin())
		{}

// From ZYadR
	virtual bool IsSimple(const ZYadOptions& iOptions)
		{ return false; }

// From ZYadMapR
	virtual ZRef<ZYadR> ReadInc(Name& oName)
		{
		if (ZRef<ZYadR> theYad = fYad->ReadInc(oName))
			{
			sInsertMust(fNamesSeen, oName);
			if (const ZVal_Any* theP = fMap.PGet(oName))
				return sYadR(*theP);
			return theYad;
			}

		while (fCurrent != fMap.End())
			{
			oName = fMap.NameOf(fCurrent);
			if (sQInsert(fNamesSeen, oName))
				return sYadR(fMap.Get(fCurrent++));
			++fCurrent;
			}

		return null;
		}

// From ZYadMapRClone
	virtual ZRef<ZYadMapRClone> Clone()
		{ return new YadMapAtRPos(*this); }

// From ZYadMapAtR
	virtual ZRef<ZYadR> ReadAt(const Name& iName)
		{
		if (ZRef<ZYadR> theYad = fYad->ReadAt(iName))
			{
			if (const ZVal_Any* theP = fMap.PGet(iName))
				return sYadR(*theP);
			return theYad;
			}
		
		if (const ZVal_Any* theP = fMap.PGet(iName))
			return sYadR(*theP);
		
		return null;
		}

	virtual ZQ<ZAny> QAsAny()
		{ return null; }

// From ZYadMapRPos
	virtual void SetPosition(const Name& iName)
		{
		// Urg.
		ZUnimplemented();
		}

private:
	ZRef<ZYadMapAtRPos> fYad;
	ZMap_Any fMap;
	ZMap_Any::Index_t fCurrent;
	set<string> fNamesSeen;
	};

} // anonymous namespace

// =================================================================================================
#pragma mark -
#pragma mark sYadR

ZRef<ZYadR> sYadR(const ZVal_Yad& iVal)
	{
	if (const string8* theVal = iVal.PGet<string8>())
		return sYadR(*theVal);

	if (const ZSeq_Yad* theVal = iVal.PGet<ZSeq_Yad>())
		return sYadR(*theVal);

	if (const ZMap_Yad* theVal = iVal.PGet<ZMap_Yad>())
		return sYadR(*theVal);

	return sMake_YadAtomR_Any(iVal.AsAny());
	}

ZRef<ZYadSeqAtRPos> sYadR(const ZSeq_Yad& iSeq)
	{
	if (ZRef<ZYadSeqAtRPos> theYad = iSeq.GetYad())
		return theYad->Clone().DynamicCast<ZYadSeqAtRPos>();
	return sYadR(iSeq.GetSeq());
	}

ZRef<ZYadMapAtRPos> sYadR(const ZMap_Yad& iMap)
	{
	if (ZRef<ZYadMapAtRPos> theYad = iMap.GetYad())
		return new YadMapAtRPos(theYad->Clone().DynamicCast<ZYadMapAtRPos>(), iMap.GetMap());
	return sYadR(iMap.GetMap());
	}

} // namespace ZooLib
