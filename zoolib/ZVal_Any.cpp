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

ZVal_Any::operator operator_bool_type() const
	{ return operator_bool_generator_type::translate(this->type() != typeid(void)); }

ZVal_Any::ZVal_Any()
	{}

ZVal_Any::ZVal_Any(const ZVal_Any& iOther)
:	inherited(iOther)
	{}

ZVal_Any::~ZVal_Any()
	{}

ZVal_Any& ZVal_Any::operator=(const ZVal_Any& iOther)
	{
	inherited::operator=(iOther);
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

ZAny& ZVal_Any::OParam()
	{
	this->Clear();
	return *this;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZVal_Any typename accessors

ZMACRO_ZValAccessors_Def_Entry(ZVal_Any, Data, ZValData_Any)
ZMACRO_ZValAccessors_Def_Entry(ZVal_Any, List, ZValList_Any)
ZMACRO_ZValAccessors_Def_Entry(ZVal_Any, Map, ZValMap_Any)

// =================================================================================================
#pragma mark -
#pragma mark * ZValList_Any

ZValList_Any::operator bool() const
	{ return this->size(); }

ZValList_Any::ZValList_Any()
	{}

ZValList_Any::ZValList_Any(const ZValList_Any& iOther)
:	inherited(iOther)
	{}

ZValList_Any::~ZValList_Any()
	{}

ZValList_Any& ZValList_Any::operator=(const ZValList_Any& iOther)
	{
	inherited::operator=(iOther);
	return *this;
	}

ZValList_Any::ZValList_Any(vector<ZAny>& iOther)
:	inherited(iOther)
	{}

ZValList_Any& ZValList_Any::operator=(vector<ZAny>& iOther)
	{
	inherited::operator=(iOther);
	return *this;
	}

size_t ZValList_Any::Count() const
	{ return inherited::size(); }

void ZValList_Any::Clear()
	{ inherited::clear(); }

bool ZValList_Any::QGet(size_t iIndex, ZVal_Any& oVal) const
	{
	if (iIndex < inherited::size())
		{
		oVal = (*this)[iIndex];
		return true;
		}
	return false;
	}

ZVal_Any ZValList_Any::DGet(size_t iIndex, const ZVal_Any& iDefault) const
	{
	if (iIndex < inherited::size())
		return (*this)[iIndex];
	return iDefault;
	}

ZVal_Any ZValList_Any::Get(size_t iIndex) const
	{
	if (iIndex < inherited::size())
		return (*this)[iIndex];
	return ZVal_Any();
	}

void ZValList_Any::Set(size_t iIndex, const ZVal_Any& iVal)
	{
	if (iIndex < inherited::size())
		(*this)[iIndex] = iVal;
	}

void ZValList_Any::Erase(size_t iIndex)
	{
	if (iIndex < inherited::size())
		inherited::erase(inherited::begin() + iIndex);
	}

void ZValList_Any::Insert(size_t iIndex, const ZVal_Any& iVal)
	{
	if (iIndex <= inherited::size())
		inherited::insert((inherited::begin() + iIndex), iVal);
	}

void ZValList_Any::Append(const ZVal_Any& iVal)
	{ inherited::push_back(iVal); }

vector<ZAny>& ZValList_Any::OParam()
	{
	inherited::clear();
	return *this;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZValMap_Any

ZValMap_Any::operator bool() const
	{ return inherited::size(); }

ZValMap_Any::ZValMap_Any()
	{}

ZValMap_Any::ZValMap_Any(const ZValMap_Any& iOther)
:	inherited(iOther)
	{}

ZValMap_Any::~ZValMap_Any()
	{}

ZValMap_Any& ZValMap_Any::operator=(const ZValMap_Any& iOther)
	{
	inherited::operator=(iOther);
	return *this;
	}

ZValMap_Any::ZValMap_Any(const map<string, ZAny>& iOther)
:	inherited(iOther)
	{}

ZValMap_Any& ZValMap_Any::operator=(map<string, ZAny>& iOther)
	{
	inherited::operator=(iOther);
	return *this;
	}

void ZValMap_Any::Clear()
	{ inherited::clear(); }

bool ZValMap_Any::QGet(const string8& iName, ZVal_Any& oVal) const
	{ return this->QGet(inherited::find(iName), oVal); }

bool ZValMap_Any::QGet(const Index_t& iIndex, ZVal_Any& oVal) const
	{
	if (iIndex != inherited::end())
		{
		oVal = (*iIndex).second;
		return true;
		}
	return false;
	}

ZVal_Any ZValMap_Any::DGet(const string8& iName, const ZVal_Any& iDefault) const
	{ return this->DGet(inherited::find(iName), iDefault); }

ZVal_Any ZValMap_Any::DGet(const Index_t& iIndex, const ZVal_Any& iDefault) const
	{
	if (iIndex != inherited::end())
		return (*iIndex).second;

	return iDefault;
	}

ZVal_Any ZValMap_Any::Get(const string8& iName) const
	{ return this->Get(inherited::find(iName)); }

ZVal_Any ZValMap_Any::Get(const Index_t& iIndex) const
	{
	if (iIndex != inherited::end())
		return (*iIndex).second;

	return ZVal_Any();
	}

void ZValMap_Any::Set(const string8& iName, const ZVal_Any& iVal)
	{
	(*this)[iName] = iVal;
	}

void ZValMap_Any::Set(const Index_t& iIndex, const ZVal_Any& iVal)
	{
	// Urg. Do this so it works, but the point of using
	// the iterator is for performance.
	if (iIndex != inherited::end())
		{
		iterator i = inherited::find((*iIndex).first);
		(*i).second = iVal;
		}
	}

void ZValMap_Any::Erase(const Index_t& iIndex)
	{
	if (iIndex != inherited::end())
		inherited::erase(inherited::find((*iIndex).first));
	}

void ZValMap_Any::Erase(const string8& iName)
	{ inherited::erase(iName); }

ZValMap_Any::Index_t ZValMap_Any::Begin() const
	{ return inherited::begin(); }

ZValMap_Any::Index_t ZValMap_Any::End() const
	{ return inherited::end(); }

string8 ZValMap_Any::NameOf(Index_t iIndex) const
	{
	if (iIndex != inherited::end())
		return (*iIndex).first;
	return string8();
	}

ZValMap_Any::Index_t ZValMap_Any::IndexOf(const string8& iName) const
	{ return inherited::find(iName); }

map<string, ZAny>& ZValMap_Any::OParam()
	{
	inherited::clear();
	return *this;
	}


NAMESPACE_ZOOLIB_END
