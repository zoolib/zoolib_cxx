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

#include "zoolib/ZCompare_T.h"
#include "zoolib/ZCompare_Vector.h"
#include "zoolib/ZVal_Any.h"

NAMESPACE_ZOOLIB_BEGIN

template<> int sCompare_T(const ZVal_Any& iL, const ZVal_Any& iR)
	{ return iL.Compare(iR); }

template<> int sCompare_T(const ZSeq_Any& iL, const ZSeq_Any& iR)
	{ return iL.Compare(iR); }

template<> int sCompare_T(const ZMap_Any& iL, const ZMap_Any& iR)
	{ return iL.Compare(iR); }

// =================================================================================================
#pragma mark -
#pragma mark * ZVal_Any typename accessors

int ZVal_Any::Compare(const ZVal_Any& iOther) const
	{
	if (int compare = strcmp(this->type().name(), iOther.type().name()))
		return compare;
	ZUnimplemented();
	return this < &iOther ? -1 : this > &iOther ? 1 : 0;
//	return sCompare_T<ValueType>(held, static_cast<holder*>(iOther)->held);	
	}

ZMACRO_ZValAccessors_Def_Entry(ZVal_Any, Data, ZData_Any)
ZMACRO_ZValAccessors_Def_Entry(ZVal_Any, Seq, ZSeq_Any)
ZMACRO_ZValAccessors_Def_Entry(ZVal_Any, Map, ZMap_Any)

static ZVal_Any sVal_Null;

// =================================================================================================
#pragma mark -
#pragma mark * ZSeq_Any::Rep

ZSeq_Any::Rep::Rep()
	{}

ZSeq_Any::Rep::~Rep()
	{}
	
ZSeq_Any::Rep::Rep(const vector<ZVal_Any>& iVector)
:	fVector(iVector)
	{}

// =================================================================================================
#pragma mark -
#pragma mark * ZSeq_Any

ZAny ZSeq_Any::AsAny() const
	{ return ZAny(*this); }

ZAny ZSeq_Any::AsAny(const ZAny& iDefault) const
	{ return ZAny(*this); }

ZSeq_Any::operator operator_bool_type() const
	{ return operator_bool_generator_type::translate(fRep && !fRep->fVector.empty()); }

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

ZSeq_Any::ZSeq_Any(const vector<ZVal_Any>& iOther)
:	fRep(new Rep(iOther))
	{}

ZSeq_Any& ZSeq_Any::operator=(const vector<ZVal_Any>& iOther)
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

ZVal_Any* ZSeq_Any::PGet(size_t iIndex)
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

bool ZSeq_Any::QGet(size_t iIndex, ZVal_Any& oVal) const
	{
	if (const ZVal_Any* theVal = this->PGet(iIndex))
		{
		oVal = *theVal;
		return true;
		}
	return false;
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
	return sVal_Null;
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
	else if (fRep->GetRefCount() != 1)
		{
		fRep = new Rep(fRep->fVector);
		}
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZMap_Any::Rep

ZMap_Any::Rep::Rep()
	{}

ZMap_Any::Rep::~Rep()
	{}

ZMap_Any::Rep::Rep(const map<string, ZVal_Any>& iMap)
:	fMap(iMap)
	{}

// =================================================================================================
#pragma mark -
#pragma mark * ZMap_Any

static map<string, ZVal_Any> spEmptyMap;

ZAny ZMap_Any::AsAny() const
	{ return ZAny(*this); }

ZAny ZMap_Any::AsAny(const ZAny& iDefault) const
	{ return ZAny(*this); }

ZMap_Any::operator operator_bool_type() const
	{ return operator_bool_generator_type::translate(fRep && !fRep->fMap.empty()); }

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

ZMap_Any::ZMap_Any(const map<string, ZVal_Any>& iOther)
:	fRep(new Rep(iOther.begin(), iOther.end()))
	{}

ZMap_Any& ZMap_Any::operator=(map<string, ZVal_Any>& iOther)
	{
	fRep = new Rep(iOther.begin(), iOther.end());
	return *this;
	}

int ZMap_Any::Compare(const ZMap_Any& iOther) const
	{
	if (fRep == iOther.fRep)
		{
		// We share the same rep, so we're identical.
		return 0;
		}

	if (!fRep)
		{
		// We have no rep, and iOther must have a rep (or fRep would be == iOther.fRep).
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

	if (!iOther.fRep)
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

	for (map<string, ZVal_Any>::const_iterator iterThis = fRep->fMap.begin(),
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
				if (int compare = sCompare_T<string>((*iterThis).first, (*iterOther).first))
					{
					// The names are different.
					return compare;
					}
				if (int compare = sCompare_T<ZVal_Any>((*iterThis).second, (*iterOther).second))
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

void ZMap_Any::Clear()
	{ fRep.Clear(); }

ZVal_Any* ZMap_Any::PGet(const string8& iName)
	{
	if (fRep)
		{
		this->pTouch();
		Index_t theIndex = fRep->fMap.find(iName);
		if (theIndex != fRep->fMap.end())
			return &(*theIndex).second;
		}
	return nullptr;
	}

ZVal_Any* ZMap_Any::PGet(const Index_t& iIndex)
	{
	map<string, ZVal_Any>::iterator theIndex = this->pTouch(iIndex);
	if (theIndex != this->End())
		return &(*theIndex).second;
	return nullptr;
	}

const ZVal_Any* ZMap_Any::PGet(const string8& iName) const
	{
	if (fRep)
		{
		Index_t theIndex = fRep->fMap.find(iName);
		if (theIndex != fRep->fMap.end())
			return &(*theIndex).second;
		}
	return nullptr;
	}

const ZVal_Any* ZMap_Any::PGet(const Index_t& iIndex) const
	{
	if (fRep && iIndex != fRep->fMap.end())
		return &(*iIndex).second;
	return nullptr;
	}

bool ZMap_Any::QGet(const string8& iName, ZVal_Any& oVal) const
	{
	if (const ZVal_Any* theVal = this->PGet(iName))
		{
		oVal = *theVal;
		return true;
		}
	return false;
	}

bool ZMap_Any::QGet(const Index_t& iIndex, ZVal_Any& oVal) const
	{
	if (const ZVal_Any* theVal = this->PGet(iIndex))
		{
		oVal = *theVal;
		return true;
		}
	return false;
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
	return sVal_Null;
	}

const ZVal_Any& ZMap_Any::Get(const Index_t& iIndex) const
	{
	if (const ZVal_Any* theVal = this->PGet(iIndex))
		return *theVal;
	return sVal_Null;
	}

ZMap_Any& ZMap_Any::Set(const string8& iName, const ZVal_Any& iVal)
	{
	this->pTouch();
	fRep->fMap.insert(pair<string8, ZVal_Any>(iName, iVal));
	return *this;
	}

ZMap_Any& ZMap_Any::Set(const Index_t& iIndex, const ZVal_Any& iVal)
	{
	map<string, ZVal_Any>::iterator theIndex = this->pTouch(iIndex);
	if (theIndex != this->End())
		(*theIndex).second = iVal;
	return *this;
	}

ZMap_Any& ZMap_Any::Erase(const Index_t& iIndex)
	{
	map<string, ZVal_Any>::iterator theIndex = this->pTouch(iIndex);
	if (theIndex != this->End())
		fRep->fMap.erase(theIndex);
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

string8 ZMap_Any::NameOf(const Index_t& iIndex) const
	{
	if (fRep && iIndex != fRep->fMap.end())
		return (*iIndex).first;
	return string8();
	}

ZMap_Any::Index_t ZMap_Any::IndexOf(const string8& iName) const
	{
	if (fRep)
		return fRep->fMap.find(iName);
	return spEmptyMap.end();
	}

ZMap_Any::Index_t ZMap_Any::IndexOf(
	const ZMap_Any& iOther, const Index_t& iOtherIndex) const
	{
	if (this == &iOther)
		return iOtherIndex;
	return this->IndexOf(iOther.NameOf(iOtherIndex));
	}

void ZMap_Any::pTouch()
	{
	if (!fRep)
		{
		fRep = new Rep;
		}
	else if (fRep->GetRefCount() != 1)
		{
		fRep = new Rep(fRep->fMap);
		}
	}

map<string, ZVal_Any>::iterator ZMap_Any::pTouch(const Index_t& iIndex)
	{
	if (!fRep)
		{
		return spEmptyMap.end();
		}
	else if (fRep->GetRefCount() != 1)
		{
		const string theName = (*iIndex).first;
		fRep = new Rep(fRep->fMap);
		return fRep->fMap.find(theName);
		}
	else
		{
		return iIndex;
		}
	}

NAMESPACE_ZOOLIB_END
