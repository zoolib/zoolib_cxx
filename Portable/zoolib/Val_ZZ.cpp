// Copyright (c) 2009-2019 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/Val_ZZ.h"

#include "zoolib/Compare_vector.h"
#include "zoolib/Singleton.h"

using std::map;
using std::pair;
using std::string;
using std::vector;

namespace ZooLib {

static const Val_ZZ& spVal_Null()
	{ return sDefault<Val_ZZ>(); }

// =================================================================================================
#pragma mark - Seq_ZZ::Rep

Seq_ZZ::Rep::Rep()
	{
	fVector.reserve(8);
	}

Seq_ZZ::Rep::~Rep()
	{}

Seq_ZZ::Rep::Rep(const Vector_t& iVector)
:	fVector(iVector)
	{}

// =================================================================================================
#pragma mark - Seq_ZZ

Seq_ZZ::Seq_ZZ()
	{}

Seq_ZZ::Seq_ZZ(const Seq_ZZ& iOther)
:	fRep(iOther.fRep)
	{}

Seq_ZZ::~Seq_ZZ()
	{}

Seq_ZZ& Seq_ZZ::operator=(const Seq_ZZ& iOther)
	{
	fRep = iOther.fRep;
	return *this;
	}

Seq_ZZ::Seq_ZZ(const Vector_t& iOther)
:	fRep(new Rep(iOther))
	{}

Seq_ZZ& Seq_ZZ::operator=(const Vector_t& iOther)
	{
	fRep = new Rep(iOther);
	return *this;
	}

int Seq_ZZ::Compare(const Seq_ZZ& iOther) const
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

size_t Seq_ZZ::Size() const
	{
	if (fRep)
		return fRep->fVector.size();
	return 0;
	}

void Seq_ZZ::Clear()
	{ fRep.Clear(); }

const Val_ZZ* Seq_ZZ::PGet(size_t iIndex) const
	{
	if (fRep && iIndex < fRep->fVector.size())
		return &fRep->fVector[iIndex];
	return nullptr;
	}

const ZQ<Val_ZZ> Seq_ZZ::QGet(size_t iIndex) const
	{
	if (const Val_ZZ* theVal = this->PGet(iIndex))
		return *theVal;
	return null;
	}

//const Val_ZZ Seq_ZZ::DGet(const Val_ZZ& iDefault, size_t iIndex) const
//	{
//	if (const Val_ZZ* theVal = this->PGet(iIndex))
//		return *theVal;
//	return iDefault;
//	}

const Val_ZZ& Seq_ZZ::Get(size_t iIndex) const
	{
	if (const Val_ZZ* theVal = this->PGet(iIndex))
		return *theVal;
	return spVal_Null();
	}

Val_ZZ* Seq_ZZ::PMut(size_t iIndex)
	{
	if (fRep && iIndex < fRep->fVector.size())
		{
		this->pTouch();
		return &fRep->fVector[iIndex];
		}
	return nullptr;
	}

Seq_ZZ& Seq_ZZ::Set(size_t iIndex, const Val_ZZ& iVal)
	{
	if (fRep && iIndex < fRep->fVector.size())
		{
		this->pTouch();
		fRep->fVector[iIndex] = iVal;
		}
	return *this;
	}

Seq_ZZ& Seq_ZZ::Erase(size_t iIndex)
	{
	if (fRep && iIndex < fRep->fVector.size())
		{
		this->pTouch();
		fRep->fVector.erase(fRep->fVector.begin() + iIndex);
		}
	return *this;
	}

Seq_ZZ& Seq_ZZ::Insert(size_t iIndex, const Val_ZZ& iVal)
	{
	this->pTouch();
	if (iIndex <= fRep->fVector.size())
		fRep->fVector.insert((fRep->fVector.begin() + iIndex), iVal);
	return *this;
	}

Seq_ZZ& Seq_ZZ::Append(const Val_ZZ& iVal)
	{
	this->pTouch();
	fRep->fVector.push_back(iVal);
	return *this;
	}

Val_ZZ& Seq_ZZ::Append()
	{
	this->pTouch();
	fRep->fVector.resize(fRep->fVector.size() + 1);
	return fRep->fVector.back();
	}

Val_ZZ& Seq_ZZ::Mut(size_t iIndex)
	{
	this->pTouch();
	if (iIndex >= fRep->fVector.size())
		fRep->fVector.resize(iIndex + 1);
	return fRep->fVector[iIndex];
	}

Val_ZZ& Seq_ZZ::operator[](size_t iIndex)
	{
	this->pTouch();
	if (iIndex >= fRep->fVector.size())
		fRep->fVector.resize(iIndex + 1);
	return fRep->fVector[iIndex];
	}

const Val_ZZ& Seq_ZZ::operator[](size_t iIndex) const
	{ return this->Get(iIndex); }

Seq_ZZ::iterator Seq_ZZ::begin()
	{
	if (fRep)
		return fRep->fVector.begin();
	// This is slightly sketchy in that the sDefault instance must remain unchanged. However
	// already illegal to write to this iterator, because it's from an empty vector.
	return sNonConst(sDefault<Vector_t>()).begin();
	}

Seq_ZZ::iterator Seq_ZZ::end()
	{
	if (fRep)
		return fRep->fVector.end();
	return sNonConst(sDefault<Vector_t>()).end();
	}

Seq_ZZ::const_iterator Seq_ZZ::begin() const
	{
	if (fRep)
		return fRep->fVector.begin();
	return sDefault<Vector_t>().begin();
	}

Seq_ZZ::const_iterator Seq_ZZ::end() const
	{
	if (fRep)
		return fRep->fVector.end();
	return sDefault<Vector_t>().end();
	}

Seq_ZZ::const_iterator Seq_ZZ::cbegin() const
	{
	if (fRep)
		return fRep->fVector.cbegin();
	return sDefault<Vector_t>().cbegin();
	}

Seq_ZZ::const_iterator Seq_ZZ::cend() const
	{
	if (fRep)
		return fRep->fVector.cend();
	return sDefault<Vector_t>().cend();
	}

void Seq_ZZ::pTouch()
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
#pragma mark - Map_ZZ::Rep

namespace { // Anonymous

SafePtrStack_WithDestroyer<Map_ZZ::Rep,SafePtrStackLink_Map_ZZ_Rep> spSafePtrStack_Map_ZZ_Rep;

} // anonymous namespace

Map_ZZ::Rep::Rep()
	{}

Map_ZZ::Rep::~Rep()
	{}

Map_ZZ::Rep::Rep(const std::initializer_list<NameVal>& iNameVals)
:	fMap(std::begin(iNameVals), std::end(iNameVals))
	{}

Map_ZZ::Rep::Rep(const Map_t& iMap)
:	fMap(iMap)
	{}

void Map_ZZ::Rep::Finalize()
	{
	bool finalized = this->FinishFinalize();
	ZAssert(finalized);
	ZAssert(not this->IsReferenced());
	fMap.clear();

	spSafePtrStack_Map_ZZ_Rep.Push(this);
	}

ZP<Map_ZZ::Rep> Map_ZZ::Rep::spMake()
	{
	if (Rep* result = spSafePtrStack_Map_ZZ_Rep.PopIfNotEmpty<Rep>())
		return result;

	return new Rep;
	}

ZP<Map_ZZ::Rep> Map_ZZ::Rep::spMake(const Map_t& iMap)
	{
	if (Rep* result = spSafePtrStack_Map_ZZ_Rep.PopIfNotEmpty<Rep>())
		{
		result->fMap = iMap;
		return result;
		}

	return new Rep(iMap);
	}

// =================================================================================================
#pragma mark - Map_ZZ

static Map_ZZ::Name_t spEmptyString;

static Map_ZZ::Map_t spEmptyMap;

Map_ZZ::Map_ZZ()
	{}

Map_ZZ::Map_ZZ(const Map_ZZ& iOther)
:	fRep(iOther.fRep)
	{}

Map_ZZ::~Map_ZZ()
	{}

Map_ZZ& Map_ZZ::operator=(const Map_ZZ& iOther)
	{
	fRep = iOther.fRep;
	return *this;
	}

Map_ZZ::Map_ZZ(const Map_t& iOther)
:	fRep(Rep::spMake(iOther))
	{}

Map_ZZ& Map_ZZ::operator=(Map_t& iOther)
	{
	fRep = Rep::spMake(iOther);
	return *this;
	}

Map_ZZ::Map_ZZ(const NameVal& iNV)
	{
	this->Set(iNV);
	}

Map_ZZ::Map_ZZ(const char* iName, const char* iVal)
	{ this->Set(iName, iVal); }

Map_ZZ::Map_ZZ(const Name_t& iName, const Val_ZZ& iVal)
	{ this->Set(iName, iVal); }

Map_ZZ::Map_ZZ(const std::initializer_list<NameVal>& iNameVals)
:	fRep(new Rep(iNameVals))
	{}

int Map_ZZ::Compare(const Map_ZZ& iOther) const
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
				if (int compare = sCompare_T<Val_ZZ>(iterThis->second, iterOther->second))
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

bool Map_ZZ::IsEmpty() const
	{ return not fRep || fRep->fMap.empty(); }

void Map_ZZ::Clear()
	{ fRep.Clear(); }

const Val_ZZ* Map_ZZ::PGet(const Name_t& iName) const
	{
	if (fRep)
		{
		Index_t theIndex = fRep->fMap.find(iName);
		if (theIndex != fRep->fMap.end())
			return &theIndex->second;
		}
	return nullptr;
	}

const Val_ZZ* Map_ZZ::PGet(const Index_t& iIndex) const
	{
	if (fRep && iIndex != fRep->fMap.end())
		return &iIndex->second;
	return nullptr;
	}

const ZQ<Val_ZZ> Map_ZZ::QGet(const Name_t& iName) const
	{
	if (const Val_ZZ* theVal = this->PGet(iName))
		return *theVal;
	return null;
	}

const ZQ<Val_ZZ> Map_ZZ::QGet(const Index_t& iIndex) const
	{
	if (const Val_ZZ* theVal = this->PGet(iIndex))
		return *theVal;
	return null;
	}

//const Val_ZZ& Map_ZZ::DGet(const Val_ZZ& iDefault, const Name_t& iName) const
//	{
//	if (const Val_ZZ* theVal = this->PGet(iName))
//		return *theVal;
//	return iDefault;
//	}

//const Val_ZZ& Map_ZZ::DGet(const Val_ZZ& iDefault, const Index_t& iIndex) const
//	{
//	if (const Val_ZZ* theVal = this->PGet(iIndex))
//		return *theVal;
//	return iDefault;
//	}

const Val_ZZ& Map_ZZ::Get(const Name_t& iName) const
	{
	if (const Val_ZZ* theVal = this->PGet(iName))
		return *theVal;
	return spVal_Null();
	}

const Val_ZZ& Map_ZZ::Get(const Index_t& iIndex) const
	{
	if (const Val_ZZ* theVal = this->PGet(iIndex))
		return *theVal;
	return spVal_Null();
	}

Val_ZZ* Map_ZZ::PMut(const Name_t& iName)
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

Val_ZZ* Map_ZZ::PMut(const Index_t& iIndex)
	{
	Map_t::iterator theIndex = this->pTouch(iIndex);
	if (theIndex != this->End())
		return &theIndex->second;
	return nullptr;
	}

Val_ZZ& Map_ZZ::Mut(const Name_t& iName)
	{
	this->pTouch();
	return fRep->fMap[iName];
	}

Map_ZZ& Map_ZZ::Set(const Name_t& iName, const Val_ZZ& iVal)
	{
	this->pTouch();
	fRep->fMap[iName] = iVal;
	return *this;
	}

Map_ZZ& Map_ZZ::Set(const Index_t& iIndex, const Val_ZZ& iVal)
	{
	Map_t::iterator theIndex = this->pTouch(iIndex);
	if (theIndex != this->End())
		theIndex->second = iVal;
	return *this;
	}

Map_ZZ& Map_ZZ::Erase(const Name_t& iName)
	{
	if (fRep)
		{
		this->pTouch();
		fRep->fMap.erase(iName);
		}
	return *this;
	}

Map_ZZ& Map_ZZ::Erase(const Index_t& iIndex)
	{
	Map_t::iterator theIndex = this->pTouch(iIndex);
	if (theIndex != this->End())
		fRep->fMap.erase(theIndex);
	return *this;
	}

Map_ZZ::Index_t Map_ZZ::Begin() const
	{
	if (fRep)
		return fRep->fMap.begin();
	return spEmptyMap.begin();
	}

Map_ZZ::Index_t Map_ZZ::End() const
	{
	if (fRep)
		return fRep->fMap.end();
	return spEmptyMap.end();
	}

const Map_ZZ::Name_t& Map_ZZ::NameOf(const Index_t& iIndex) const
	{
	if (fRep && iIndex != fRep->fMap.end())
		return iIndex->first;
	return spEmptyString;
	}

Map_ZZ::Index_t Map_ZZ::IndexOf(const Name_t& iName) const
	{
	if (fRep)
		return fRep->fMap.find(iName);
	return spEmptyMap.end();
	}

Map_ZZ::Index_t Map_ZZ::IndexOf(const Map_ZZ& iOther, const Index_t& iOtherIndex) const
	{
	if (this == &iOther)
		return iOtherIndex;
	return this->IndexOf(iOther.NameOf(iOtherIndex));
	}

Map_ZZ& Map_ZZ::Set(const NameVal& iNV)
	{ return this->Set(iNV.first, iNV.second); }

Val_ZZ& Map_ZZ::operator[](const Name_t& iName)
	{ return this->Mut(iName); }

const Val_ZZ& Map_ZZ::operator[](const Name_t& iName) const
	{
	if (const Val_ZZ* theVal = this->PGet(iName))
		return *theVal;
	return spVal_Null();
	}

Val_ZZ& Map_ZZ::operator[](const Index_t& iIndex)
	{
	if (Val_ZZ* theVal = this->PMut(iIndex))
		{ return *theVal; }
	else
		{
		ZDebugStop(0);
		return *theVal;
		}
	}

const Val_ZZ& Map_ZZ::operator[](const Index_t& iIndex) const
	{
	if (const Val_ZZ* theVal = this->PGet(iIndex))
		return *theVal;
	return spVal_Null();
	}

Map_ZZ::iterator Map_ZZ::begin()
	{
	if (fRep)
		return fRep->fMap.begin();
	return spEmptyMap.begin();
	}

Map_ZZ::iterator Map_ZZ::end()
	{
	if (fRep)
		return fRep->fMap.end();
	return spEmptyMap.end();
	}

Map_ZZ::const_iterator Map_ZZ::begin() const
	{
	if (fRep)
		return fRep->fMap.begin();
	return spEmptyMap.begin();
	}

Map_ZZ::const_iterator Map_ZZ::end() const
	{
	if (fRep)
		return fRep->fMap.end();
	return spEmptyMap.end();
	}

Map_ZZ::const_iterator Map_ZZ::cbegin() const
	{
	if (fRep)
		return fRep->fMap.begin();
	return spEmptyMap.begin();
	}

Map_ZZ::const_iterator Map_ZZ::cend() const
	{
	if (fRep)
		return fRep->fMap.end();
	return spEmptyMap.end();
	}

void Map_ZZ::pTouch()
	{
	if (not fRep)
		{
		fRep = Rep::spMake();
		}
	else if (fRep->IsShared())
		{
		fRep = Rep::spMake(fRep->fMap);
		}
	}

Map_ZZ::Map_t::iterator Map_ZZ::pTouch(const Index_t& iIndex)
	{
	if (not fRep)
		{
		return spEmptyMap.end();
		}
	else if (fRep->IsShared())
		{
		const Name_t theName = iIndex->first;
		fRep = Rep::spMake(fRep->fMap);
		return fRep->fMap.find(theName);
		}
	else
		{
		return iIndex;
		}
	}

Map_ZZ operator*(const NameVal& iNV0, const NameVal& iNV1)
	{
	Map_ZZ result;
	result.Set(iNV0);
	result.Set(iNV1);
	return result;
	}

Map_ZZ& operator*=(Map_ZZ& ioMap, const NameVal& iNV)
	{ return ioMap.Set(iNV); }

Map_ZZ operator*(const Map_ZZ& iMap, const NameVal& iNV)
	{ return Map_ZZ(iMap).Set(iNV); }

Map_ZZ operator*(const NameVal& iNV, const Map_ZZ& iMap)
	{ return Map_ZZ(iMap).Set(iNV); }

Map_ZZ& operator*=(Map_ZZ& ioMap0, const Map_ZZ& iMap1)
	{
	for (Map_ZZ::Index_t ii = iMap1.Begin(), end = iMap1.End(); ii != end; ++ii)
		ioMap0.Set(iMap1.NameOf(ii), iMap1.Get(ii));
	return ioMap0;
	}

Map_ZZ operator*(const Map_ZZ& iMap0, const Map_ZZ& iMap1)
	{
	Map_ZZ result = iMap0;
	return result *= iMap1;
	}

// =================================================================================================
#pragma mark -

Map_ZZ sAugmented(const Map_ZZ& iUnder, const Map_ZZ& iOver)
	{
	Map_ZZ result = iUnder;
	for (Map_ZZ::Index_t ii = iOver.Begin(); ii != iOver.End(); ++ii)
		{
		const Map_ZZ::Name_t& theName = iOver.NameOf(ii);
		const Val_ZZ& theOverVal = iOver.Get(ii);
		if (Val_ZZ* theResultVal = result.PMut(theName))
			{
			// Already have a val in result
			if (const Map_ZZ* theOverMap = theOverVal.PGet<Map_ZZ>())
				{
				// Over is a map
				if (Map_ZZ* theResultMap = theResultVal->PMut<Map_ZZ>())
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
