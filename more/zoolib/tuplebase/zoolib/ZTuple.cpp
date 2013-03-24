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

#include "zoolib/ZStrim_Stream.h"
#include "zoolib/ZStrimU_Unreader.h"
#include "zoolib/ZStrimmerFromStrim.h"
#include "zoolib/ZYad_Any.h"
#include "zoolib/ZYad_ZooLibStrim.h"

namespace ZooLib {

using std::vector;

// =================================================================================================
#pragma mark -
#pragma mark * ZTuple

ZTuple::ZTuple()
	{}

ZTuple::ZTuple(const ZTuple& iTuple)
:	ZMap_Any(iTuple)
	{}

ZTuple::~ZTuple()
	{}

ZTuple ZTuple::operator=(const ZTuple& iTuple)
	{
	ZMap_Any::operator=(iTuple);
	return *this;
	}

ZTuple::ZTuple(const ZMap_Any& iMap)
:	ZMap_Any(iMap)
	{}

ZTuple ZTuple::operator=(const ZMap_Any& iMap)
	{
	ZMap_Any::operator=(iMap);
	return *this;
	}

ZTuple::ZTuple(const ZStreamR& iStreamR)
	{
	ZStrimR_StreamUTF8 theStrimR(iStreamR);
	ZStrimU_Unreader theStrimU(theStrimR);

	*this = sFromYadR(ZVal_Any(),
		ZYad_ZooLibStrim::sYadR(ZStrimmerFromStrimU(theStrimU))).Get<ZMap_Any>();
	}

void ZTuple::ToStream(const ZStreamW& iStreamW) const
	{ ZYad_ZooLibStrim::sToStrim(sYadR(*this), ZStrimW_StreamUTF8(iStreamW)); }

ZTuple ZTuple::GetTuple(const ZTName& iPropName) const
	{ return this->Get<ZMap_Any>(iPropName); }

void ZTuple::SetTuple(const ZTName& iName, const ZTuple& iTuple)
	{ this->Set(iName, iTuple); }

// =================================================================================================
#pragma mark -
#pragma mark * ZMap_Any typename accessors

#define ZMACRO_ZMapAccessors_Def_Entry(Name_t, TYPENAME, TYPE) \
	bool ZTuple::QGet##TYPENAME(Name_t iName, TYPE& oVal) const \
		{ \
		if (ZQ<TYPE> theQ = this->QGet<TYPE>(iName)) \
			{ \
			oVal = *theQ; \
			return true; \
			} \
		return false; \
		} \
	TYPE ZTuple::DGet##TYPENAME(const TYPE& iDefault, Name_t iName) const \
		{ return this->DGet<TYPE>(iDefault, iName); } \
	TYPE ZTuple::Get##TYPENAME(Name_t iName) const \
		{ return this->Get<TYPE>(iName); } \
	ZTuple& ZTuple::Set##TYPENAME(Name_t iName, const TYPE& iVal) \
		{ \
		this->Set(iName, iVal); \
		return *this; \
		} \

#define ZMACRO_ZMapAccessors_Def(Name_t) \
	ZMACRO_ZMapAccessors_Def_Entry(Name_t, ID, uint64) \
	ZMACRO_ZMapAccessors_Def_Entry(Name_t, Int8, int8) \
	ZMACRO_ZMapAccessors_Def_Entry(Name_t, Int16, int16) \
	ZMACRO_ZMapAccessors_Def_Entry(Name_t, Int32, int32) \
	ZMACRO_ZMapAccessors_Def_Entry(Name_t, Int64, int64) \
	ZMACRO_ZMapAccessors_Def_Entry(Name_t, Bool, bool) \
	ZMACRO_ZMapAccessors_Def_Entry(Name_t, Float, float) \
	ZMACRO_ZMapAccessors_Def_Entry(Name_t, Double, double) \
	ZMACRO_ZMapAccessors_Def_Entry(Name_t, String, std::string) \
	ZMACRO_ZMapAccessors_Def_Entry(Name_t, Time, ZTime) \

ZMACRO_ZMapAccessors_Def(const char*)
ZMACRO_ZMapAccessors_Def(const ZTName&)
ZMACRO_ZMapAccessors_Def(const_iterator)

#undef ZMACRO_ZMapAccessors_Def_Entry
#undef ZMACRO_ZMapAccessors_Def

} // namespace ZooLib
