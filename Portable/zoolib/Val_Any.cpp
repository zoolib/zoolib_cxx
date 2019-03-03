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

#include "zoolib/Val_Any.h"

#include "zoolib/Compare.h"
#include "zoolib/Compare_T.h"
#include "zoolib/Compare_vector.h"
#include "zoolib/Singleton.h"

using std::map;
using std::pair;
using std::string;
using std::vector;

namespace ZooLib {

static const Val_Any& spVal_Null()
	{ return sDefault<Val_Any>(); }

template <>
int sCompare_T(const Val_Any& iL, const Val_Any& iR)
	{ return iL.Compare(iR); }

template <>
int sCompare_T(const Seq_Any& iL, const Seq_Any& iR)
	{ return iL.Compare(iR); }

template <>
int sCompare_T(const Map_Any& iL, const Map_Any& iR)
	{ return iL.Compare(iR); }

template <>
int sCompare_T(const NameVal& iL, const NameVal& iR)
	{
	if (int compare = sCompare_T(iL.first, iR.first))
		return compare;
	return sCompare_T(iL.second, iR.second);
	}

ZMACRO_CompareRegistration_T(Val_Any)
ZMACRO_CompareRegistration_T(Seq_Any)
ZMACRO_CompareRegistration_T(Map_Any)

// =================================================================================================
#pragma mark - Seq_Any::Rep

Seq_Any::Rep::Rep()
	{
	fVector.reserve(8);
	}

Seq_Any::Rep::~Rep()
	{}

Seq_Any::Rep::Rep(const Vector_t& iVector)
:	fVector(iVector)
	{}

// =================================================================================================
#pragma mark - Seq_Any

Seq_Any::Seq_Any()
	{}

Seq_Any::Seq_Any(const Seq_Any& iOther)
:	fRep(iOther.fRep)
	{}

Seq_Any::~Seq_Any()
	{}

Seq_Any& Seq_Any::operator=(const Seq_Any& iOther)
	{
	fRep = iOther.fRep;
	return *this;
	}

Seq_Any::Seq_Any(const Vector_t& iOther)
:	fRep(new Rep(iOther))
	{}

Seq_Any& Seq_Any::operator=(const Vector_t& iOther)
	{
	fRep = new Rep(iOther);
	return *this;
	}

int Seq_Any::Compare(const Seq_Any& iOther) const
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

size_t Seq_Any::Size() const
	{
	if (fRep)
		return fRep->fVector.size();
	return 0;
	}

void Seq_Any::Clear()
	{ fRep.Clear(); }

const Val_Any* Seq_Any::PGet(size_t iIndex) const
	{
	if (fRep && iIndex < fRep->fVector.size())
		return &fRep->fVector[iIndex];
	return nullptr;
	}

const ZQ<Val_Any> Seq_Any::QGet(size_t iIndex) const
	{
	if (const Val_Any* theVal = this->PGet(iIndex))
		return *theVal;
	return null;
	}

const Val_Any Seq_Any::DGet(const Val_Any& iDefault, size_t iIndex) const
	{
	if (const Val_Any* theVal = this->PGet(iIndex))
		return *theVal;
	return iDefault;
	}

const Val_Any& Seq_Any::Get(size_t iIndex) const
	{
	if (const Val_Any* theVal = this->PGet(iIndex))
		return *theVal;
	return spVal_Null();
	}

Val_Any* Seq_Any::PMut(size_t iIndex)
	{
	if (fRep && iIndex < fRep->fVector.size())
		{
		this->pTouch();
		return &fRep->fVector[iIndex];
		}
	return nullptr;
	}

Seq_Any& Seq_Any::Set(size_t iIndex, const Val_Any& iVal)
	{
	if (fRep && iIndex < fRep->fVector.size())
		{
		this->pTouch();
		fRep->fVector[iIndex] = iVal;
		}
	return *this;
	}

Seq_Any& Seq_Any::Erase(size_t iIndex)
	{
	if (fRep && iIndex < fRep->fVector.size())
		{
		this->pTouch();
		fRep->fVector.erase(fRep->fVector.begin() + iIndex);
		}
	return *this;
	}

Seq_Any& Seq_Any::Insert(size_t iIndex, const Val_Any& iVal)
	{
	this->pTouch();
	if (iIndex <= fRep->fVector.size())
		fRep->fVector.insert((fRep->fVector.begin() + iIndex), iVal);
	return *this;
	}

Seq_Any& Seq_Any::Append(const Val_Any& iVal)
	{
	this->pTouch();
	fRep->fVector.push_back(iVal);
	return *this;
	}

Val_Any& Seq_Any::Mut(size_t iIndex)
	{
	this->pTouch();
	if (iIndex >= fRep->fVector.size())
		fRep->fVector.resize(iIndex + 1);
	return fRep->fVector[iIndex];
	}

Val_Any& Seq_Any::operator[](size_t iIndex)
	{
	this->pTouch();
	if (iIndex >= fRep->fVector.size())
		fRep->fVector.resize(iIndex + 1);
	return fRep->fVector[iIndex];
	}

const Val_Any& Seq_Any::operator[](size_t iIndex) const
	{ return this->Get(iIndex); }

Seq_Any::iterator Seq_Any::begin()
	{
	if (fRep)
		return fRep->fVector.begin();
	return const_cast<Vector_t&>(sDefault<Vector_t>()).begin();
	}

Seq_Any::iterator Seq_Any::end()
	{
	if (fRep)
		return fRep->fVector.end();
	return const_cast<Vector_t&>(sDefault<Vector_t>()).end();
	}

Seq_Any::const_iterator Seq_Any::begin() const
	{
	if (fRep)
		return fRep->fVector.begin();
	return sDefault<Vector_t>().begin();
	}

Seq_Any::const_iterator Seq_Any::end() const
	{
	if (fRep)
		return fRep->fVector.end();
	return sDefault<Vector_t>().end();
	}

void Seq_Any::pTouch()
	{
	if (not fRep)
		{
		fRep = new Rep;
		}
	else if (fRep->IsShared())
		{
		fRep = new Rep(fRep->fVector);
		}
	}

// =================================================================================================
#pragma mark - Map_Any::Rep

namespace {

SafePtrStack_WithDestroyer<Map_Any::Rep,SafePtrStackLink_Map_Any_Rep> spSafePtrStack_Map_Any_Rep;

} // anonymous namespace

Map_Any::Rep::Rep()
	{}

Map_Any::Rep::~Rep()
	{}

Map_Any::Rep::Rep(const Map_t& iMap)
:	fMap(iMap)
	{}

void Map_Any::Rep::Finalize()
	{
	bool finalized = this->FinishFinalize();
	ZAssert(finalized);
	ZAssert(not this->IsReferenced());
	fMap.clear();

	spSafePtrStack_Map_Any_Rep.Push(this);
	}

ZRef<Map_Any::Rep> Map_Any::Rep::sMake()
	{
	if (Rep* result = spSafePtrStack_Map_Any_Rep.PopIfNotEmpty<Rep>())
		return result;

	return new Rep;
	}

ZRef<Map_Any::Rep> Map_Any::Rep::sMake(const Map_t& iMap)
	{
	if (Rep* result = spSafePtrStack_Map_Any_Rep.PopIfNotEmpty<Rep>())
		{
		result->fMap = iMap;
		return result;
		}

	return new Rep(iMap);
	}

// =================================================================================================
#pragma mark - Map_Any

static Map_Any::Name_t spEmptyString;

static Map_Any::Map_t spEmptyMap;

Map_Any::Map_Any()
	{}

Map_Any::Map_Any(const Map_Any& iOther)
:	fRep(iOther.fRep)
	{}

Map_Any::~Map_Any()
	{}

Map_Any& Map_Any::operator=(const Map_Any& iOther)
	{
	fRep = iOther.fRep;
	return *this;
	}

Map_Any::Map_Any(const Map_t& iOther)
:	fRep(Rep::sMake(iOther))
	{}

Map_Any& Map_Any::operator=(Map_t& iOther)
	{
	fRep = Rep::sMake(iOther);
	return *this;
	}

Map_Any::Map_Any(const NameVal& iNV)
	{
	this->Set(iNV);
	}

Map_Any::Map_Any(const char* iName, const char* iVal)
	{ this->Set(iName, iVal); }

Map_Any::Map_Any(const Name_t& iName, const Val_Any& iVal)
	{ this->Set(iName, iVal); }

int Map_Any::Compare(const Map_Any& iOther) const
	{
	if (fRep == iOther.fRep)
		{
		// We share the same rep, so we're identical.
		return 0;
		}

	if (not fRep)
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
				if (int compare = sCompare_T(iterThis->first, iterOther->first))
					{
					// The names are different.
					return compare;
					}
				if (int compare = sCompare_T<Val_Any>(iterThis->second, iterOther->second))
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

bool Map_Any::IsEmpty() const
	{ return not fRep || fRep->fMap.empty(); }

void Map_Any::Clear()
	{ fRep.Clear(); }

const Val_Any* Map_Any::PGet(const Name_t& iName) const
	{
	if (fRep)
		{
		Index_t theIndex = fRep->fMap.find(iName);
		if (theIndex != fRep->fMap.end())
			return &theIndex->second;
		}
	return nullptr;
	}

const Val_Any* Map_Any::PGet(const Index_t& iIndex) const
	{
	if (fRep && iIndex != fRep->fMap.end())
		return &iIndex->second;
	return nullptr;
	}

const ZQ<Val_Any> Map_Any::QGet(const Name_t& iName) const
	{
	if (const Val_Any* theVal = this->PGet(iName))
		return *theVal;
	return null;
	}

const ZQ<Val_Any> Map_Any::QGet(const Index_t& iIndex) const
	{
	if (const Val_Any* theVal = this->PGet(iIndex))
		return *theVal;
	return null;
	}

const Val_Any& Map_Any::DGet(const Val_Any& iDefault, const Name_t& iName) const
	{
	if (const Val_Any* theVal = this->PGet(iName))
		return *theVal;
	return iDefault;
	}

const Val_Any& Map_Any::DGet(const Val_Any& iDefault, const Index_t& iIndex) const
	{
	if (const Val_Any* theVal = this->PGet(iIndex))
		return *theVal;
	return iDefault;
	}

const Val_Any& Map_Any::Get(const Name_t& iName) const
	{
	if (const Val_Any* theVal = this->PGet(iName))
		return *theVal;
	return spVal_Null();
	}

const Val_Any& Map_Any::Get(const Index_t& iIndex) const
	{
	if (const Val_Any* theVal = this->PGet(iIndex))
		return *theVal;
	return spVal_Null();
	}

Val_Any* Map_Any::PMut(const Name_t& iName)
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

Val_Any* Map_Any::PMut(const Index_t& iIndex)
	{
	Map_t::iterator theIndex = this->pTouch(iIndex);
	if (theIndex != this->End())
		return &theIndex->second;
	return nullptr;
	}

Val_Any& Map_Any::Mut(const Name_t& iName)
	{
	this->pTouch();
	return fRep->fMap[iName];
	}

Map_Any& Map_Any::Set(const Name_t& iName, const Val_Any& iVal)
	{
	this->pTouch();
	fRep->fMap[iName] = iVal;
	return *this;
	}

Map_Any& Map_Any::Set(const Index_t& iIndex, const Val_Any& iVal)
	{
	Map_t::iterator theIndex = this->pTouch(iIndex);
	if (theIndex != this->End())
		theIndex->second = iVal;
	return *this;
	}

Map_Any& Map_Any::Erase(const Name_t& iName)
	{
	if (fRep)
		{
		this->pTouch();
		fRep->fMap.erase(iName);
		}
	return *this;
	}

Map_Any& Map_Any::Erase(const Index_t& iIndex)
	{
	Map_t::iterator theIndex = this->pTouch(iIndex);
	if (theIndex != this->End())
		fRep->fMap.erase(theIndex);
	return *this;
	}

Map_Any::Index_t Map_Any::Begin() const
	{
	if (fRep)
		return fRep->fMap.begin();
	return spEmptyMap.begin();
	}

Map_Any::Index_t Map_Any::End() const
	{
	if (fRep)
		return fRep->fMap.end();
	return spEmptyMap.end();
	}

Map_Any::iterator Map_Any::begin()
	{
	if (fRep)
		return fRep->fMap.begin();
	return spEmptyMap.begin();
	}

Map_Any::iterator Map_Any::end()
	{
	if (fRep)
		return fRep->fMap.end();
	return spEmptyMap.end();
	}


Map_Any::const_iterator Map_Any::begin() const
	{
	if (fRep)
		return fRep->fMap.begin();
	return spEmptyMap.begin();
	}

Map_Any::const_iterator Map_Any::end() const
	{
	if (fRep)
		return fRep->fMap.end();
	return spEmptyMap.end();
	}

const Map_Any::Name_t& Map_Any::NameOf(const Index_t& iIndex) const
	{
	if (fRep && iIndex != fRep->fMap.end())
		return iIndex->first;
	return spEmptyString;
	}

Map_Any::Index_t Map_Any::IndexOf(const Name_t& iName) const
	{
	if (fRep)
		return fRep->fMap.find(iName);
	return spEmptyMap.end();
	}

Map_Any::Index_t Map_Any::IndexOf(const Map_Any& iOther, const Index_t& iOtherIndex) const
	{
	if (this == &iOther)
		return iOtherIndex;
	return this->IndexOf(iOther.NameOf(iOtherIndex));
	}

Map_Any& Map_Any::Set(const NameVal& iNV)
	{ return this->Set(iNV.first, iNV.second); }

Val_Any& Map_Any::operator[](const Name_t& iName)
	{ return this->Mut(iName); }

const Val_Any& Map_Any::operator[](const Name_t& iName) const
	{
	if (const Val_Any* theVal = this->PGet(iName))
		return *theVal;
	return spVal_Null();
	}

Val_Any& Map_Any::operator[](const Index_t& iIndex)
	{
	if (Val_Any* theVal = this->PMut(iIndex))
		{ return *theVal; }
	else
		{
		ZDebugStop(0);
		return *theVal;
		}
	}

const Val_Any& Map_Any::operator[](const Index_t& iIndex) const
	{
	if (const Val_Any* theVal = this->PGet(iIndex))
		return *theVal;
	return spVal_Null();
	}

void Map_Any::pTouch()
	{
	if (not fRep)
		{
		fRep = Rep::sMake();
		}
	else if (fRep->IsShared())
		{
		fRep = Rep::sMake(fRep->fMap);
		}
	}

Map_Any::Map_t::iterator Map_Any::pTouch(const Index_t& iIndex)
	{
	if (not fRep)
		{
		return spEmptyMap.end();
		}
	else if (fRep->IsShared())
		{
		const Name_t theName = iIndex->first;
		fRep = Rep::sMake(fRep->fMap);
		return fRep->fMap.find(theName);
		}
	else
		{
		return iIndex;
		}
	}

Map_Any operator*(const NameVal& iNV0, const NameVal& iNV1)
	{
	Map_Any result;
	result.Set(iNV0);
	result.Set(iNV1);
	return result;
	}

Map_Any& operator*=(Map_Any& ioMap, const NameVal& iNV)
	{ return ioMap.Set(iNV); }

Map_Any operator*(const Map_Any& iMap, const NameVal& iNV)
	{ return Map_Any(iMap).Set(iNV); }

Map_Any operator*(const NameVal& iNV, const Map_Any& iMap)
	{ return Map_Any(iMap).Set(iNV); }

Map_Any& operator*=(Map_Any& ioMap0, const Map_Any& iMap1)
	{
	for (Map_Any::Index_t ii = iMap1.Begin(), end = iMap1.End(); ii != end; ++ii)
		ioMap0.Set(iMap1.NameOf(ii), iMap1.Get(ii));
	return ioMap0;
	}

Map_Any operator*(const Map_Any& iMap0, const Map_Any& iMap1)
	{
	Map_Any result = iMap0;
	return result *= iMap1;
	}

// =================================================================================================
#pragma mark -

Map_Any sAugmented(const Map_Any& iUnder, const Map_Any& iOver)
	{
	Map_Any result = iUnder;
	for (Map_Any::Index_t ii = iOver.Begin(); ii != iOver.End(); ++ii)
		{
		const Map_Any::Name_t& theName = iOver.NameOf(ii);
		const Val_Any& theOverVal = iOver.Get(ii);
		if (Val_Any* theResultVal = result.PMut(theName))
			{
			// Already have a val in result
			if (const Map_Any* theOverMap = theOverVal.PGet<Map_Any>())
				{
				// Over is a map
				if (Map_Any* theResultMap = theResultVal->PMut<Map_Any>())
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
