/* -------------------------------------------------------------------------------------------------
Copyright (c) 2000 Andrew Green and Learning in Motion, Inc.
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

#ifndef __ZTuple__
#define __ZTuple__ 1
#include "zconfig.h"

#include "zoolib/ZVal_ZooLib.h"

NAMESPACE_ZOOLIB_BEGIN

typedef ZVal_ZooLib ZTValue;

// =================================================================================================
#pragma mark -
#pragma mark * ZTuple

#define ZMACRO_ZMapAccessors_Decl_Entry(T, Name_t, TYPENAME, TYPE) \
	bool QGet##TYPENAME(Name_t iName, TYPE& oVal) const; \
	TYPE DGet##TYPENAME(const TYPE& iDefault, Name_t iName) const; \
	TYPE Get##TYPENAME(Name_t iName) const; \
	ZTuple& Set##TYPENAME(Name_t iName, const TYPE& iVal); \

#define ZMACRO_ZMapAccessors_Decl(T, Name_t) \
	ZMACRO_ZMapAccessors_Decl_Entry(T, Name_t, ID, uint64) \
	ZMACRO_ZMapAccessors_Decl_Entry(T, Name_t, Int8, int8) \
	ZMACRO_ZMapAccessors_Decl_Entry(T, Name_t, Int16, int16) \
	ZMACRO_ZMapAccessors_Decl_Entry(T, Name_t, Int32, int32) \
	ZMACRO_ZMapAccessors_Decl_Entry(T, Name_t, Int64, int64) \
	ZMACRO_ZMapAccessors_Decl_Entry(T, Name_t, Bool, bool) \
	ZMACRO_ZMapAccessors_Decl_Entry(T, Name_t, Float, float) \
	ZMACRO_ZMapAccessors_Decl_Entry(T, Name_t, Double, double) \
	ZMACRO_ZMapAccessors_Decl_Entry(T, Name_t, String, std::string) \
	ZMACRO_ZMapAccessors_Decl_Entry(T, Name_t, Time, ZTime) \

class ZTuple : public ZMap_ZooLib
	{
public:
	ZTuple();
	ZTuple(const ZTuple& iTuple);
	~ZTuple();
	ZTuple operator=(const ZTuple& iTuple);

	ZTuple(const ZMap_ZooLib& iMap);
	ZTuple operator=(const ZMap_ZooLib& iMap);

	explicit ZTuple(const ZStreamR& iStreamR);

// Backwards compatibility
	ZTuple Over(const ZTuple& iUnder) const;
	ZTuple Under(const ZTuple& iOver) const;

	ZTuple GetTuple(const ZTName& iName) const;
	void SetTuple(const ZTName& iName, const ZTuple& iTuple);

	ZMap_ZooLib& SetNull(const ZTName& iPropName);
	std::vector<ZVal_ZooLib>& SetMutableVector(const ZTName& iPropName);

	Index_t begin() const { return this->Begin(); }
	Index_t end() const { return this->End(); }
	
	typedef Index_t const_iterator;

	const_iterator IteratorOf(const char* iPropName) const
		{ return this->IndexOf(iPropName); }

	const_iterator IteratorOf(const ZTName& iPropName) const
		{ return this->IndexOf(iPropName); }

// Typename accessors
	ZMACRO_ZMapAccessors_Decl(ZMap_ZooLib, const char*)
	ZMACRO_ZMapAccessors_Decl(ZMap_ZooLib, const ZTName&)
	ZMACRO_ZMapAccessors_Decl(ZMap_ZooLib, const_iterator)
	};

#undef ZMACRO_ZMapAccessors_Decl_Entry
#undef ZMACRO_ZMapAccessors_Decl

NAMESPACE_ZOOLIB_END

#endif // __ZTuple__
