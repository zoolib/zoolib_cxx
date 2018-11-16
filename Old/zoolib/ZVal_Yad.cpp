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
#include "zoolib/Yad_Any.h"

#include "zoolib/ZVal_Yad.h"

namespace ZooLib {

using std::set;
using std::string;

using namespace Util_STL;

namespace { // anonymous

class Tombstone_t {};

ZVal_Yad spAsVal_Yad(const ZRef<YadR>& iYadR)
	{
	if (not iYadR)
		return sDefault<ZVal_Yad>();

	if (ZRef<YadSeqAtR> theYad = iYadR.DynamicCast<YadSeqAtR>())
		return ZSeq_Yad(theYad);

	if (ZRef<YadMapAtR> theYad = iYadR.DynamicCast<YadMapAtR>())
		return ZMap_Yad(theYad);

	return sFromYadR(Val_Any(), iYadR).AsAny();
	}

} // anonymous namespace

// =================================================================================================
#pragma mark - ZSeq_Yad

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

ZSeq_Yad::ZSeq_Yad(const ZRef<YadSeqAtR>& iYad)
:	fYad(iYad)
	{}

ZSeq_Yad& ZSeq_Yad::operator=(const ZRef<YadSeqAtR>& iYad)
	{
	fYad = iYad;
	fSeq.Clear();
	return *this;
	}

size_t ZSeq_Yad::Size() const
	{
	if (fYad)
		return sSize(*fYad);
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
		for (const size_t yadCount = sSize(*fYad);
			iIndex < yadCount && iIndex >= fSeq.Count();
			/*no inc*/)
			{ fSeq.Append(Tombstone_t()); }
		}

	if (Any* theP = fSeq.PMut(iIndex))
		{
		if (theP->PGet<Tombstone_t>())
			*theP = spAsVal_Yad(sReadAt(fYad, iIndex)).AsAny();
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
		for (const size_t yadCount = sSize(*fYad);
			iIndex < yadCount && iIndex >= fSeq.Count();
			/*no inc*/)
			{ fSeq.Append(Tombstone_t()); }
		}

	if (Any* theP = fSeq.PMut(iIndex))
		{
		if (theP->PGet<Tombstone_t>())
			*theP = spAsVal_Yad(sReadAt(fYad, iIndex)).AsAny();
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
		for (const size_t yadCount = sSize(*fYad);
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

ZRef<YadSeqAtR> ZSeq_Yad::GetYad() const
	{ return fYad; }

Seq_Any ZSeq_Yad::GetSeq() const
	{ return fSeq; }

void ZSeq_Yad::pGenSeq()
	{
	if (not fYad)
		return;

	for (size_t xx = 0, count = sSize(*fYad); xx < count ; ++xx)
		{
		if (fSeq.Count() == xx)
			{
			fSeq.Append(spAsVal_Yad(sReadAt(fYad, xx)).AsAny());
			}
		else if (fSeq.PGet<Tombstone_t>(xx))
			{
			fSeq.Set(xx, spAsVal_Yad(sReadAt(fYad, xx)).AsAny());
			}
		}

	fYad.Clear();
	}

// =================================================================================================
#pragma mark - ZMap_Yad

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

ZMap_Yad::ZMap_Yad(const ZRef<YadMapAtR>& iYad)
:	fYad(iYad)
	{}

ZMap_Yad& ZMap_Yad::operator=(const ZRef<YadMapAtR>& iYad)
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
	Map_Any::Index_t theIndex = fMap.IndexOf(iName);
	if (theIndex != fMap.End())
		{
		if (const Any* theAny = fMap.PGet(theIndex))
			{
			if (theAny->PGet<Tombstone_t>())
				return nullptr;
			return static_cast<const ZVal_Yad*>(theAny);
			}
		}

	if (fYad)
		{
		if (ZRef<YadR> theYad = sReadAt(fYad, iName))
			{
			fMap.Set(iName, Val_Any(spAsVal_Yad(theYad).AsAny()));
			return static_cast<const ZVal_Yad*>(static_cast<const Any*>(fMap.PGet(iName)));
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
	Map_Any::Index_t theIndex = fMap.IndexOf(iName);
	if (theIndex != fMap.End())
		{
		if (Any* theAny = fMap.PMut(theIndex))
			{
			if (theAny->PGet<Tombstone_t>())
				return nullptr;
			return static_cast<ZVal_Yad*>(theAny);
			}
		}

	if (fYad)
		{
		if (ZRef<YadR> theYad = sReadAt(fYad, iName))
			{
			fMap.Set(iName, Val_Any(spAsVal_Yad(theYad).AsAny()));
			return static_cast<ZVal_Yad*>(static_cast<Any*>(fMap.PMut(iName)));
			}
		}

	fMap.Set(iName, Tombstone_t());
	return nullptr;
	}

ZVal_Yad& ZMap_Yad::Mut(const Name_t& iName)
	{
	if (Any* theP = fMap.PMut(iName))
		{
		if (theP->PGet<Tombstone_t>())
			*theP = Any();
		return *static_cast<ZVal_Yad*>(theP);
		}
	
	Any& theMutable = fMap.Mut(iName);
	if (fYad)
		{
		if (ZRef<YadR> theYad = sReadAt(fYad, iName))
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

ZRef<YadMapAtR> ZMap_Yad::GetYad() const
	{ return fYad; }

Map_Any ZMap_Yad::GetMap() const
	{ return fMap; }

// =================================================================================================
#pragma mark - sYadR

ZRef<YadR> sYadR(const ZVal_Yad& iVal)
	{
	if (const string8* theVal = iVal.PGet<string8>())
		return sYadR(*theVal);

	if (const ZSeq_Yad* theVal = iVal.PGet<ZSeq_Yad>())
		return sYadR(*theVal);

	if (const ZMap_Yad* theVal = iVal.PGet<ZMap_Yad>())
		return sYadR(*theVal);

	return sYadR(iVal.AsAny());
	}

ZRef<YadSeqAtR> sYadR(const ZSeq_Yad& iSeq)
	{
	if (ZRef<YadSeqAtR> theYad = iSeq.GetYad())
		return theYad;
	return sYadR(iSeq.GetSeq());
	}

ZRef<YadMapAtR> sYadR(const ZMap_Yad& iMap)
	{
	if (ZRef<YadMapAtR> theYad = iMap.GetYad())
		return theYad;
	return sYadR(iMap.GetMap());
	}

} // namespace ZooLib
