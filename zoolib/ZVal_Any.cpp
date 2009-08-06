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

#include "zoolib/ZVal_Any.h"

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * ZVal_Any

ZVal_Any ZVal_Any::AsVal_Any()
	{ return *this; }

ZVal_Any ZVal_Any::AsVal_Any(const ZVal_Any& iDefault)
	{ return *this; }

ZVal_Any::operator operator_bool_type() const
	{ return operator_bool_generator_type::translate(this->type() != typeid(void)); }

ZVal_Any::ZVal_Any()
	{}

ZVal_Any::ZVal_Any(const ZVal_Any& iOther)
:	inherited(static_cast<const ZAny&>(iOther))
	{}

ZVal_Any::~ZVal_Any()
	{}

ZVal_Any& ZVal_Any::operator=(const ZVal_Any& iOther)
	{
	inherited::operator=(static_cast<const ZAny&>(iOther));
	return *this;
	}

ZVal_Any::ZVal_Any(const ZAny& iVal)
:	inherited(iVal)
	{}

ZVal_Any& ZVal_Any::operator=(const ZAny& iVal)
	{
	inherited::operator=(iVal);
	return *this;
	}

void ZVal_Any::Clear()
	{ *this = ZAny(); }

// =================================================================================================
#pragma mark -
#pragma mark * ZVal_Any typename accessors

ZMACRO_ZValAccessors_Def_Entry(ZVal_Any, Data, ZData_Any)
ZMACRO_ZValAccessors_Def_Entry(ZVal_Any, List, ZList_Any)
ZMACRO_ZValAccessors_Def_Entry(ZVal_Any, Map, ZMap_Any)

// =================================================================================================
#pragma mark -
#pragma mark * ZList_Any::Rep

ZList_Any::Rep::Rep()
	{}

ZList_Any::Rep::~Rep()
	{}
	
ZList_Any::Rep::Rep(const vector<ZAny>& iVector)
:	fVector(iVector)
	{}

// =================================================================================================
#pragma mark -
#pragma mark * ZList_Any

ZList_Any ZList_Any::AsList_Any()
	{ return *this; }

ZList_Any ZList_Any::AsList_Any(const ZVal_Any& iDefault)
	{ return *this; }

ZList_Any::operator operator_bool_type() const
	{ return operator_bool_generator_type::translate(fRep && !fRep->fVector.empty()); }

ZList_Any::ZList_Any()
	{}

ZList_Any::ZList_Any(const ZList_Any& iOther)
:	fRep(iOther.fRep)
	{}

ZList_Any::~ZList_Any()
	{}

ZList_Any& ZList_Any::operator=(const ZList_Any& iOther)
	{
	fRep = iOther.fRep;
	return *this;
	}

ZList_Any::ZList_Any(vector<ZAny>& iOther)
:	fRep(new Rep(iOther))
	{}

ZList_Any& ZList_Any::operator=(vector<ZAny>& iOther)
	{
	fRep = new Rep(iOther);
	return *this;
	}

size_t ZList_Any::Count() const
	{
	if (fRep)
		return fRep->fVector.size();
	return 0;
	}

void ZList_Any::Clear()
	{ fRep.Clear(); }

bool ZList_Any::QGet(size_t iIndex, ZVal_Any& oVal) const
	{
	if (fRep && iIndex < fRep->fVector.size())
		{
		oVal = fRep->fVector[iIndex];
		return true;
		}
	return false;
	}

ZVal_Any ZList_Any::DGet(const ZVal_Any& iDefault, size_t iIndex) const
	{
	if (fRep && iIndex < fRep->fVector.size())
		return fRep->fVector[iIndex];

	return iDefault;
	}

ZVal_Any ZList_Any::Get(size_t iIndex) const
	{
	if (fRep && iIndex < fRep->fVector.size())
		return fRep->fVector[iIndex];

	return ZVal_Any();
	}

ZVal_Any* ZList_Any::PGet(size_t iIndex)
	{
	if (fRep && iIndex < fRep->fVector.size())
		{
		this->pTouch();
		return static_cast<ZVal_Any*>(&fRep->fVector[iIndex]);
		}
	return nullptr;
	}

void ZList_Any::Set(size_t iIndex, const ZVal_Any& iVal)
	{
	if (fRep && iIndex < fRep->fVector.size())
		{
		this->pTouch();
		fRep->fVector[iIndex] = static_cast<const ZAny&>(iVal);
		}
	}

void ZList_Any::Erase(size_t iIndex)
	{
	if (fRep && iIndex < fRep->fVector.size())
		{
		this->pTouch();
		fRep->fVector.erase(fRep->fVector.begin() + iIndex);
		}
	}

void ZList_Any::Insert(size_t iIndex, const ZVal_Any& iVal)
	{
	this->pTouch();
	if (iIndex <= fRep->fVector.size())
		fRep->fVector.insert((fRep->fVector.begin() + iIndex), static_cast<const ZAny&>(iVal));
	}

void ZList_Any::Append(const ZVal_Any& iVal)
	{
	this->pTouch();
	fRep->fVector.push_back(static_cast<const ZAny&>(iVal));
	}

void ZList_Any::pTouch()
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

ZMap_Any::Rep::Rep(const map<string, ZAny>& iMap)
:	fMap(iMap)
	{}

// =================================================================================================
#pragma mark -
#pragma mark * ZMap_Any

static map<string, ZAny> spEmptyMap;

ZMap_Any ZMap_Any::AsMap_Any()
	{ return *this; }

ZMap_Any ZMap_Any::AsMap_Any(const ZVal_Any& iDefault)
	{ return *this; }

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

ZMap_Any::ZMap_Any(const map<string, ZAny>& iOther)
:	fRep(new Rep(iOther.begin(), iOther.end()))
	{}

ZMap_Any& ZMap_Any::operator=(map<string, ZAny>& iOther)
	{
	fRep = new Rep(iOther.begin(), iOther.end());
	return *this;
	}

void ZMap_Any::Clear()
	{ fRep.Clear(); }

bool ZMap_Any::QGet(const string8& iName, ZVal_Any& oVal) const
	{
	if (fRep)
		{
		Index_t theIndex = fRep->fMap.find(iName);
		if (theIndex != fRep->fMap.end())
			{
			oVal = (*theIndex).second;
			return true;
			}
		}
	return false;
	}

bool ZMap_Any::QGet(const Index_t& iIndex, ZVal_Any& oVal) const
	{
	if (fRep && iIndex != fRep->fMap.end())
		{
		oVal = (*iIndex).second;
		return true;
		}
	return false;
	}

ZVal_Any ZMap_Any::DGet(const ZVal_Any& iDefault, const string8& iName) const
	{
	if (fRep)
		{
		Index_t theIndex = fRep->fMap.find(iName);
		if (theIndex != fRep->fMap.end())
			return (*theIndex).second;
		}
	return iDefault;
	}

ZVal_Any ZMap_Any::DGet(const ZVal_Any& iDefault, const Index_t& iIndex) const
	{
	if (fRep && iIndex != fRep->fMap.end())
		return (*iIndex).second;
	return iDefault;
	}

ZVal_Any ZMap_Any::Get(const string8& iName) const
	{
	if (fRep)
		{
		Index_t theIndex = fRep->fMap.find(iName);
		if (theIndex != fRep->fMap.end())
			return (*theIndex).second;
		}
	return ZVal_Any();
	}

ZVal_Any ZMap_Any::Get(const Index_t& iIndex) const
	{
	if (fRep && iIndex != fRep->fMap.end())
		return (*iIndex).second;
	return ZVal_Any();
	}

ZVal_Any* ZMap_Any::PGet(const string8& iName)
	{
	if (fRep)
		{
		this->pTouch();
		Index_t theIndex = fRep->fMap.find(iName);
		if (theIndex != fRep->fMap.end())
			return static_cast<ZVal_Any*>(&(*theIndex).second);
		}
	return nullptr;
	}

ZVal_Any* ZMap_Any::PGet(const Index_t& iIndex)
	{
	map<string, ZAny>::iterator theIndex = this->pTouch(iIndex);
	if (theIndex != this->End())
		return static_cast<ZVal_Any*>(&(*theIndex).second);
	return nullptr;
	}

void ZMap_Any::Set(const string8& iName, const ZVal_Any& iVal)
	{
	this->pTouch();
	fRep->fMap.insert(pair<string8, ZAny>(iName, static_cast<const ZAny&>(iVal)));
	}

void ZMap_Any::Set(const Index_t& iIndex, const ZVal_Any& iVal)
	{
	map<string, ZAny>::iterator theIndex = this->pTouch(iIndex);
	if (theIndex != this->End())
		(*theIndex).second = static_cast<const ZAny&>(iVal);
	}

void ZMap_Any::Erase(const Index_t& iIndex)
	{
	map<string, ZAny>::iterator theIndex = this->pTouch(iIndex);
	if (theIndex != this->End())
		fRep->fMap.erase(theIndex);
	}

void ZMap_Any::Erase(const string8& iName)
	{
	if (fRep)
		{
		this->pTouch();
		fRep->fMap.erase(iName);
		}
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

map<string, ZAny>::iterator ZMap_Any::pTouch(const Index_t& iIndex)
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
