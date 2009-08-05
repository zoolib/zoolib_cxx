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

#include "zoolib/ZTuple.h"

NAMESPACE_ZOOLIB_BEGIN

using std::vector;

// =================================================================================================
#pragma mark -
#pragma mark * ZTuple

ZTuple::ZTuple()
	{}

ZTuple::ZTuple(const ZTuple& iTuple)
:	ZMap_ZooLib(iTuple)
	{}

ZTuple::~ZTuple()
	{}

ZTuple ZTuple::operator=(const ZTuple& iTuple)
	{
	ZMap_ZooLib::operator=(iTuple);
	return *this;
	}

ZTuple::ZTuple(const ZMap_ZooLib& iMap)
:	ZMap_ZooLib(iMap)
	{}

ZTuple ZTuple::operator=(const ZMap_ZooLib& iMap)
	{
	ZMap_ZooLib::operator=(iMap);
	return *this;
	}

ZTuple::ZTuple(const ZStreamR& iStreamR)
:	ZMap_ZooLib(iStreamR)
	{}

ZTuple ZTuple::Over(const ZTuple& iUnder) const
	{
	ZTuple result = iUnder;
	if (fRep)
		{
		for (ZTuple::const_iterator i = this->begin(), theEnd = this->end();
			i != theEnd; ++i)
			{
			result.Set((*i).fName, (*i).fVal);
			}
		}
	return result;
	}

ZTuple ZTuple::Under(const ZTuple& iOver) const
	{ return iOver.Over(*this); }

ZMap_ZooLib& ZTuple::SetNull(const ZTName& iPropName)
	{
	this->pTouch();
	*this->pFindOrAllocate(iPropName) = ZVal_ZooLib();
	return *this;
	}

vector<ZVal_ZooLib>& ZTuple::SetMutableVector(const ZTName& iPropName)
	{
	this->pTouch();
	return this->pFindOrAllocate(iPropName)->MutableList().MutableVector();
	}

ZTuple ZTuple::GetTuple(const ZTName& iPropName) const
	{ return this->Get(iPropName).GetMap(); }

void ZTuple::SetTuple(const ZTName& iName, const ZTuple& iTuple)
	{ this->Set(iName, iTuple); }

// =================================================================================================
#pragma mark -
#pragma mark * ZMap_ZooLib typename accessors

#define ZMACRO_ZValMapAccessors_Def_Entry(Name_t, TYPENAME, TYPE) \
	bool ZTuple::QGet##TYPENAME(Name_t iName, TYPE& oVal) const \
		{ \
		ZVal_ZooLib theVal; \
		if (this->QGet(iName, theVal)) \
			return theVal.QGet_T<TYPE>(oVal); \
		return false; \
		} \
	TYPE ZTuple::DGet##TYPENAME(Name_t iName, const TYPE& iDefault) const \
		{ \
		ZVal_ZooLib theVal; \
		if (this->QGet(iName, theVal)) \
			return theVal.DGet_T<TYPE>(iDefault); \
		return iDefault; \
		} \
	TYPE ZTuple::Get##TYPENAME(Name_t iName) const \
		{ return this->Get(iName).Get_T<TYPE>(); } \
	void ZTuple::Set##TYPENAME(Name_t iName, const TYPE& iVal) \
		{ return this->Set(iName, iVal); } \

#define ZMACRO_ZValMapAccessors_Def(Name_t) \
	ZMACRO_ZValMapAccessors_Def_Entry(Name_t, ID, uint64) \
	ZMACRO_ZValMapAccessors_Def_Entry(Name_t, Int8, int8) \
	ZMACRO_ZValMapAccessors_Def_Entry(Name_t, Int16, int16) \
	ZMACRO_ZValMapAccessors_Def_Entry(Name_t, Int32, int32) \
	ZMACRO_ZValMapAccessors_Def_Entry(Name_t, Int64, int64) \
	ZMACRO_ZValMapAccessors_Def_Entry(Name_t, Bool, bool) \
	ZMACRO_ZValMapAccessors_Def_Entry(Name_t, Float, float) \
	ZMACRO_ZValMapAccessors_Def_Entry(Name_t, Double, double) \
	ZMACRO_ZValMapAccessors_Def_Entry(Name_t, String, std::string) \
	ZMACRO_ZValMapAccessors_Def_Entry(Name_t, Time, ZTime) \

ZMACRO_ZValMapAccessors_Def(const char*)
ZMACRO_ZValMapAccessors_Def(const ZTName&)
ZMACRO_ZValMapAccessors_Def(const_iterator)

#undef ZMACRO_ZValMapAccessors_Def_Entry
#undef ZMACRO_ZValMapAccessors_Def

NAMESPACE_ZOOLIB_END
