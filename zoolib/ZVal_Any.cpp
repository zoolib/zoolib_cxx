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

#include "zoolib/ZCompare.h"
#include "zoolib/ZCompare_T.h"
#include "zoolib/ZCompare_Vector.h"
#include "zoolib/ZVal_Any.h"

#include <cstring> // for strcmp

using std::map;
using std::pair;
using std::strcmp;
using std::string;
using std::vector;

namespace ZooLib {

static ZVal_Any spVal_Null;

template <>
int sCompare_T(const ZVal_Any& iL, const ZVal_Any& iR)
	{ return iL.Compare(iR); }

template <>
int sCompare_T(const ZSeq_Any& iL, const ZSeq_Any& iR)
	{ return iL.Compare(iR); }

template <>
int sCompare_T(const ZMap_Any& iL, const ZMap_Any& iR)
	{ return iL.Compare(iR); }

template <>
int sCompare_T(const ZNameVal& iL, const ZNameVal& iR)
	{
	if (int compare = sCompare_T(iL.first, iR.first))
		return compare;
	return sCompare_T(iL.second, iR.second);
	}

ZMACRO_CompareRegistration_T(ZVal_Any)
ZMACRO_CompareRegistration_T(ZSeq_Any)
ZMACRO_CompareRegistration_T(ZMap_Any)

// =================================================================================================
// MARK: - ZVal_Any typename accessors

/**
\class ZVal_Any
\ingroup ZVal

\brief
*/

int ZVal_Any::Compare(const ZVal_Any& iOther) const
	{
	const char* typeName = this->Type().name();
	if (int compare = strcmp(typeName, iOther.Type().name()))
		return compare;
	return ZCompare::sCompare(typeName, this->ConstVoidStar(), iOther.ConstVoidStar());
	}

ZVal_Any* ZVal_Any::PGetMutable(const string8& iName)
	{
	if (ZMap_Any* asMap = this->PGetMutable<ZMap_Any>())
		return asMap->PGetMutable(iName);
	return nullptr;
	}

const ZVal_Any* ZVal_Any::PGet(const string8& iName) const
	{
	if (const ZMap_Any* asMap = this->PGet<ZMap_Any>())
		return asMap->PGet(iName);
	return nullptr;
	}

ZVal_Any ZVal_Any::Get(const string8& iName) const
	{ return this->GetMap().Get(iName); }

ZVal_Any& ZVal_Any::Mutable(const string8& iName)
	{
	if (ZMap_Any* asMap = this->PGetMutable<ZMap_Any>())
		return asMap->Mutable(iName);	
	return this->Mutable<ZMap_Any>().Mutable(iName);
	}

ZVal_Any* ZVal_Any::PGetMutable(size_t iIndex)
	{
	if (ZSeq_Any* asSeq = this->PGetMutable<ZSeq_Any>())
		return asSeq->PGetMutable(iIndex);
	return nullptr;
	}

const ZVal_Any* ZVal_Any::PGet(size_t iIndex) const
	{
	if (const ZSeq_Any* asSeq = this->PGet<ZSeq_Any>())
		return asSeq->PGet(iIndex);
	return nullptr;
	}

ZVal_Any ZVal_Any::Get(size_t iIndex) const
	{ return this->GetSeq().Get(iIndex); }

ZMACRO_ZValAccessors_Def_Entry(ZVal_Any, Data, ZData_Any)
ZMACRO_ZValAccessors_Def_Entry(ZVal_Any, Seq, ZSeq_Any)
ZMACRO_ZValAccessors_Def_Entry(ZVal_Any, Map, ZMap_Any)

// =================================================================================================
// MARK: - ZSeq_Any::Rep

ZSeq_Any::Rep::Rep()
	{}

ZSeq_Any::Rep::~Rep()
	{}

ZSeq_Any::Rep::Rep(const Vector_t& iVector)
:	fVector(iVector)
	{}

// =================================================================================================
// MARK: - ZSeq_Any

ZSeq_Any::ZSeq_Any()
	{}

ZSeq_Any::ZSeq_Any(const ZSeq_Any& iOther)
:	fRep(iOther.fRep)
	{}

ZSeq_Any::~ZSeq_Any()
	{}

ZSeq_Any& ZSeq_Any::operator=(const ZSeq_Any& iOther)
	{
	fRep = iOther.fRep;
	return *this;
	}

ZSeq_Any::ZSeq_Any(const Vector_t& iOther)
:	fRep(new Rep(iOther))
	{}

ZSeq_Any& ZSeq_Any::operator=(const Vector_t& iOther)
	{
	fRep = new Rep(iOther);
	return *this;
	}

size_t ZSeq_Any::Count() const
	{
	if (fRep)
		return fRep->fVector.size();
	return 0;
	}

int ZSeq_Any::Compare(const ZSeq_Any& iOther) const
	{
	if (fRep)
		{
		if (iOther.fRep)
			{
			return sCompare_T(fRep->fVector, iOther.fRep->fVector);
			}
		else
			{
			return 1;
			}
		}
	else if (iOther.fRep)
		{
		return -1;
		}
	else
		{
		return 0;
		}
	}

void ZSeq_Any::Clear()
	{ fRep.Clear(); }

ZVal_Any* ZSeq_Any::PGetMutable(size_t iIndex)
	{
	if (fRep && iIndex < fRep->fVector.size())
		{
		this->pTouch();
		return &fRep->fVector[iIndex];
		}
	return nullptr;
	}

const ZVal_Any* ZSeq_Any::PGet(size_t iIndex) const
	{
	if (fRep && iIndex < fRep->fVector.size())
		return &fRep->fVector[iIndex];
	return nullptr;
	}

ZQ<ZVal_Any> ZSeq_Any::QGet(size_t iIndex) const
	{
	if (const ZVal_Any* theVal = this->PGet(iIndex))
		return *theVal;
	return null;
	}

ZVal_Any ZSeq_Any::DGet(const ZVal_Any& iDefault, size_t iIndex) const
	{
	if (const ZVal_Any* theVal = this->PGet(iIndex))
		return *theVal;
	return iDefault;
	}

const ZVal_Any& ZSeq_Any::Get(size_t iIndex) const
	{
	if (const ZVal_Any* theVal = this->PGet(iIndex))
		return *theVal;
	return spVal_Null;
	}

ZSeq_Any& ZSeq_Any::Set(size_t iIndex, const ZVal_Any& iVal)
	{
	if (fRep && iIndex < fRep->fVector.size())
		{
		this->pTouch();
		fRep->fVector[iIndex] = iVal;
		}
	return *this;
	}

ZSeq_Any& ZSeq_Any::Erase(size_t iIndex)
	{
	if (fRep && iIndex < fRep->fVector.size())
		{
		this->pTouch();
		fRep->fVector.erase(fRep->fVector.begin() + iIndex);
		}
	return *this;
	}

ZSeq_Any& ZSeq_Any::Insert(size_t iIndex, const ZVal_Any& iVal)
	{
	this->pTouch();
	if (iIndex <= fRep->fVector.size())
		fRep->fVector.insert((fRep->fVector.begin() + iIndex), iVal);
	return *this;
	}

ZSeq_Any& ZSeq_Any::Append(const ZVal_Any& iVal)
	{
	this->pTouch();
	fRep->fVector.push_back(iVal);
	return *this;
	}

void ZSeq_Any::pTouch()
	{
	if (!fRep)
		{
		fRep = new Rep;
		}
	else if (fRep->IsShared())
		{
		fRep = new Rep(fRep->fVector);
		}
	}

// =================================================================================================
// MARK: - ZMap_Any::Rep

ZMap_Any::Rep::Rep()
	{}

ZMap_Any::Rep::~Rep()
	{}

ZMap_Any::Rep::Rep(const ZNameVal& iNV)
:	fMap(&iNV, &iNV + 1)
	{}

ZMap_Any::Rep::Rep(const Map_t& iMap)
:	fMap(iMap)
	{}

// =================================================================================================
// MARK: - ZMap_Any

static string8 spEmptyString;
static ZMap_Any::Map_t spEmptyMap;

ZMap_Any::ZMap_Any()
	{}

ZMap_Any::ZMap_Any(const ZMap_Any& iOther)
:	fRep(iOther.fRep)
	{}

ZMap_Any::~ZMap_Any()
	{}

ZMap_Any& ZMap_Any::operator=(const ZMap_Any& iOther)
	{
	fRep = iOther.fRep;
	return *this;
	}

ZMap_Any::ZMap_Any(const Map_t& iOther)
:	fRep(new Rep(iOther.begin(), iOther.end()))
	{}

ZMap_Any& ZMap_Any::operator=(Map_t& iOther)
	{
	fRep = new Rep(iOther.begin(), iOther.end());
	return *this;
	}

ZMap_Any::ZMap_Any(const ZNameVal& iNV)
:	fRep(new Rep(iNV))
	{}

ZMap_Any::ZMap_Any(const char* iName, const char* iVal)
	{ this->Set(iName, iVal); }

ZMap_Any::ZMap_Any(const string8& iName, const ZVal_Any& iVal)
	{ this->Set(iName, iVal); }

int ZMap_Any::Compare(const ZMap_Any& iOther) const
	{
	if (fRep == iOther.fRep)
		{
		// We share the same rep, so we're identical.
		return 0;
		}

	if (!fRep)
		{
		// We have no rep, (iOther must have a rep or fRep would be == iOther.fRep).
		if (iOther.fRep->fMap.empty())
			{
			// And iOther's map is empty, we're equivalent.
			return 0;
			}
		else
			{
			// iOther has properties, so we're less than it.
			return -1;
			}
		}

	if (not iOther.fRep)
		{
		// iOther has no rep.
		if (fRep->fMap.empty())
			{
			// And our map is empty, so we're equivalent.
			return 0;
			}
		else
			{
			// We have properties, so we're greater than iOther.
			return 1;
			}
		}

	for (Map_t::const_iterator iterThis = fRep->fMap.begin(),
		iterOther = iOther.fRep->fMap.begin(),
		endThis = fRep->fMap.end(),
		endOther = iOther.fRep->fMap.end();
		/*no test*/; ++iterThis, ++iterOther)
		{
		if (iterThis != endThis)
			{
			// This is not exhausted.
			if (iterOther != endOther)
				{
				// Other is not exhausted either, so we compare their current values.
				if (int compare = sCompare_T<string>(iterThis->first, iterOther->first))
					{
					// The names are different.
					return compare;
					}
				if (int compare = sCompare_T<ZVal_Any>(iterThis->second, iterOther->second))
					{
					// The values are different.
					return compare;
					}
				}
			else
				{
				// Exhausted other, but still have this
				// remaining, so this is greater than other.
				return 1;
				}
			}
		else
			{
			// Exhausted this.
			if (iterOther != endOther)
				{
				// Still have other remaining, so this is less than other.
				return -1;
				}
			else
				{
				// Exhausted other. And as this is also
				// exhausted this equals other.
				return 0;
				}
			}
		}
	}

bool ZMap_Any::IsEmpty() const
	{ return not fRep || fRep->fMap.empty(); }

void ZMap_Any::Clear()
	{ fRep.Clear(); }

ZVal_Any* ZMap_Any::PGetMutable(const string8& iName)
	{
	if (fRep)
		{
		this->pTouch();
		Index_t theIndex = fRep->fMap.find(iName);
		if (theIndex != fRep->fMap.end())
			return &theIndex->second;
		}
	return nullptr;
	}

ZVal_Any* ZMap_Any::PGetMutable(const Index_t& iIndex)
	{
	Map_t::iterator theIndex = this->pTouch(iIndex);
	if (theIndex != this->End())
		return &theIndex->second;
	return nullptr;
	}

const ZVal_Any* ZMap_Any::PGet(const string8& iName) const
	{
	if (fRep)
		{
		Index_t theIndex = fRep->fMap.find(iName);
		if (theIndex != fRep->fMap.end())
			return &theIndex->second;
		}
	return nullptr;
	}

const ZVal_Any* ZMap_Any::PGet(const Index_t& iIndex) const
	{
	if (fRep && iIndex != fRep->fMap.end())
		return &iIndex->second;
	return nullptr;
	}

ZQ<ZVal_Any> ZMap_Any::QGet(const string8& iName) const
	{
	if (const ZVal_Any* theVal = this->PGet(iName))
		return *theVal;
	return null;
	}

ZQ<ZVal_Any> ZMap_Any::QGet(const Index_t& iIndex) const
	{
	if (const ZVal_Any* theVal = this->PGet(iIndex))
		return *theVal;
	return null;
	}

ZVal_Any ZMap_Any::DGet(const ZVal_Any& iDefault, const string8& iName) const
	{
	if (const ZVal_Any* theVal = this->PGet(iName))
		return *theVal;
	return iDefault;
	}

ZVal_Any ZMap_Any::DGet(const ZVal_Any& iDefault, const Index_t& iIndex) const
	{
	if (const ZVal_Any* theVal = this->PGet(iIndex))
		return *theVal;
	return iDefault;
	}

const ZVal_Any& ZMap_Any::Get(const string8& iName) const
	{
	if (const ZVal_Any* theVal = this->PGet(iName))
		return *theVal;
	return spVal_Null;
	}

const ZVal_Any& ZMap_Any::Get(const Index_t& iIndex) const
	{
	if (const ZVal_Any* theVal = this->PGet(iIndex))
		return *theVal;
	return spVal_Null;
	}

ZMap_Any& ZMap_Any::Set(const string8& iName, const ZVal_Any& iVal)
	{
	this->pTouch();
	fRep->fMap[iName] = iVal;
	return *this;
	}

ZMap_Any& ZMap_Any::Set(const Index_t& iIndex, const ZVal_Any& iVal)
	{
	Map_t::iterator theIndex = this->pTouch(iIndex);
	if (theIndex != this->End())
		theIndex->second = iVal;
	return *this;
	}

ZMap_Any& ZMap_Any::Erase(const string8& iName)
	{
	if (fRep)
		{
		this->pTouch();
		fRep->fMap.erase(iName);
		}
	return *this;
	}

ZMap_Any& ZMap_Any::Erase(const Index_t& iIndex)
	{
	Map_t::iterator theIndex = this->pTouch(iIndex);
	if (theIndex != this->End())
		fRep->fMap.erase(theIndex);
	return *this;
	}

ZVal_Any& ZMap_Any::Mutable(const string8& iName)
	{
	this->pTouch();
	return fRep->fMap[iName];
	}

ZMap_Any::Index_t ZMap_Any::Begin() const
	{
	if (fRep)
		return fRep->fMap.begin();
	return spEmptyMap.begin();
	}

ZMap_Any::Index_t ZMap_Any::End() const
	{
	if (fRep)
		return fRep->fMap.end();
	return spEmptyMap.end();
	}

const string8& ZMap_Any::NameOf(const Index_t& iIndex) const
	{
	if (fRep && iIndex != fRep->fMap.end())
		return iIndex->first;
	return spEmptyString;
	}

ZMap_Any::Index_t ZMap_Any::IndexOf(const string8& iName) const
	{
	if (fRep)
		return fRep->fMap.find(iName);
	return spEmptyMap.end();
	}

ZMap_Any::Index_t ZMap_Any::IndexOf(const ZMap_Any& iOther, const Index_t& iOtherIndex) const
	{
	if (this == &iOther)
		return iOtherIndex;
	return this->IndexOf(iOther.NameOf(iOtherIndex));
	}

ZMap_Any& ZMap_Any::Set(const ZNameVal& iNV)
	{ return this->Set(iNV.first, iNV.second); }

ZVal_Any& ZMap_Any::operator[](const string8& iName)
	{
	if (ZVal_Any* theVal = this->PGetMutable(iName))
		{ return *theVal; }
	else
		{
		this->Set(iName, ZVal_Any());
		return *this->PGetMutable(iName);
		}
	}

const ZVal_Any& ZMap_Any::operator[](const string8& iName) const
	{
	if (const ZVal_Any* theVal = this->PGet(iName))
		return *theVal;
	return spVal_Null;
	}

ZVal_Any& ZMap_Any::operator[](const Index_t& iIndex)
	{
	if (ZVal_Any* theVal = this->PGetMutable(iIndex))
		{ return *theVal; }
	else
		{
		ZDebugStop(0);
		return *theVal;
		}
	}

const ZVal_Any& ZMap_Any::operator[](const Index_t& iIndex) const
	{
	if (const ZVal_Any* theVal = this->PGet(iIndex))
		return *theVal;
	return spVal_Null;
	}

void ZMap_Any::pTouch()
	{
	if (!fRep)
		{
		fRep = new Rep;
		}
	else if (fRep->IsShared())
		{
		fRep = new Rep(fRep->fMap);
		}
	}

ZMap_Any::Map_t::iterator ZMap_Any::pTouch(const Index_t& iIndex)
	{
	if (!fRep)
		{
		return spEmptyMap.end();
		}
	else if (fRep->IsShared())
		{
		const string theName = iIndex->first;
		fRep = new Rep(fRep->fMap);
		return fRep->fMap.find(theName);
		}
	else
		{
		return iIndex;
		}
	}

ZMap_Any operator*(const ZNameVal& iNV0, const ZNameVal& iNV1)
	{
	ZMap_Any result(iNV0);
	result.Set(iNV1);
	return result;
	}

ZMap_Any& operator*=(ZMap_Any& ioMap, const ZNameVal& iNV)
	{ return ioMap.Set(iNV); }

ZMap_Any operator*(const ZMap_Any& iMap, const ZNameVal& iNV)
	{ return ZMap_Any(iMap).Set(iNV); }

ZMap_Any operator*(const ZNameVal& iNV, const ZMap_Any& iMap)
	{ return ZMap_Any(iMap).Set(iNV); }

ZMap_Any& operator*=(ZMap_Any& ioMap0, const ZMap_Any& iMap1)
	{
	for (ZMap_Any::Index_t ii = iMap1.Begin(), end = iMap1.End(); ii != end; ++ii)
		ioMap0.Set(iMap1.NameOf(ii), iMap1.Get(ii));
	return ioMap0;
	}

ZMap_Any operator*(const ZMap_Any& iMap0, const ZMap_Any& iMap1)
	{
	ZMap_Any result = iMap0;
	return result *= iMap1;
	}

// =================================================================================================
// MARK: - 

ZMap_Any sAugment(const ZMap_Any& iUnder, const ZMap_Any& iOver)
	{
	ZMap_Any result = iUnder;
	for (ZMap_Any::Index_t ii = iOver.Begin(); ii != iOver.End(); ++ii)
		{
		const string8& theName = iOver.NameOf(ii);
		const ZVal_Any& theOverVal = iOver.Get(ii);
		if (ZVal_Any* theResultVal = result.PGetMutable(theName))
			{
			// Already have a val in result
			if (const ZMap_Any* theOverMap = theOverVal.PGet<ZMap_Any>())
				{
				// Over is a map
				if (ZMap_Any* theResultMap = theResultVal->PGetMutable<ZMap_Any>())
					{
					// And result is a map, so we augment, and continue so as to skip
					// the default replacement operation below.
					*theResultMap *= *theOverMap;
					continue;
					}
				}
			// If we don't have two maps, replace existing result with over's.
			*theResultVal = theOverVal;
			}
		else
			{
			// No existing val in result, just shove in over's.
			result.Set(theName, theOverVal);
			}
		}
	return result;
	}

} // namespace ZooLib
