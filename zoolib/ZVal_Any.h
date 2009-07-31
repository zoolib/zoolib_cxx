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

#ifndef __ZVal_Any__
#define __ZVal_Any__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_SPI.h"

#include "zoolib/ZAny.h"
#include "zoolib/ZCompat_operator_bool.h"
#include "zoolib/ZRef.h"
#include "zoolib/ZRef_Counted.h"
#include "zoolib/ZUnicodeString.h"
#include "zoolib/ZVal.h"
#include "zoolib/ZValAccessors.h"
#include "zoolib/ZValData_Any.h"

#include <map>
#include <vector>

NAMESPACE_ZOOLIB_BEGIN

using std::map;
using std::pair;
using std::string;
using std::vector;

class ZVal_Any;
class ZValList_Any;
class ZValMap_Any;

// =================================================================================================
#pragma mark -
#pragma mark * ZVal_Any

class ZVal_Any
:	public ZAny
	{
	ZOOLIB_DEFINE_OPERATOR_BOOL_TYPES(ZVal_Any,
		operator_bool_generator_type, operator_bool_type);

	typedef ZAny inherited;

public:
	operator operator_bool_type() const;

	ZVal_Any();
	ZVal_Any(const ZVal_Any& iOther);
	~ZVal_Any();
	ZVal_Any& operator=(const ZVal_Any& iOther);

	ZVal_Any(const ZAny& iVal);

	ZVal_Any& operator=(const ZAny& iVal);

	template <class S>
	ZVal_Any(const S& iVal)
	:	inherited(iVal)
		{}

	template <class S>
	ZVal_Any& operator=(const S& iVal)
		{
		inherited::operator=(iVal);
		return *this;
		}

// ZVal protocol
	void Clear();

	template <class S>
	bool QGet_T(S& oVal) const
		{
		if (const S* theVal = ZAnyCast<S>(this))
			{
			oVal = *theVal;
			return true;
			}
		return false;
		}

	template <class S>
	S DGet_T(const S& iDefault) const
		{
		if (const S* theVal = ZAnyCast<S>(this))
			return *theVal;
		return iDefault;
		}

	template <class S>
	S Get_T() const
		{
		if (const S* theVal = ZAnyCast<S>(this))
			return *theVal;
		return S();
		}

	template <class S>
	void Set_T(const S& iVal)
		{
		ZAny::operator=(iVal);
		}

// Our protocol
	ZAny& OParam();

// Typename accessors
	ZMACRO_ZValAccessors_Decl_Entry(ZVal_Any, Data, ZValData_Any)
	ZMACRO_ZValAccessors_Decl_Entry(ZVal_Any, List, ZValList_Any)
	ZMACRO_ZValAccessors_Decl_Entry(ZVal_Any, Map, ZValMap_Any)
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZValList_Any

class ZValList_Any
	{
	ZOOLIB_DEFINE_OPERATOR_BOOL_TYPES(ZValMap_Any,
		operator_bool_generator_type, operator_bool_type);

	class Rep;

public:
	operator operator_bool_type() const;

	ZValList_Any();
	ZValList_Any(const ZValList_Any& iOther);
	~ZValList_Any();
	ZValList_Any& operator=(const ZValList_Any& iOther);

	ZValList_Any(vector<ZAny>& iOther);

	ZValList_Any& operator=(vector<ZAny>& iOther);

// ZValList protocol
	size_t Count() const;

	void Clear();

	bool QGet(size_t iIndex, ZVal_Any& oVal) const;
	ZVal_Any DGet(size_t iIndex, const ZVal_Any& iDefault) const;
	ZVal_Any Get(size_t iIndex) const;

	void Set(size_t iIndex, const ZVal_Any& iVal);

	void Erase(size_t iIndex);

	void Insert(size_t iIndex, const ZVal_Any& iVal);

	void Append(const ZVal_Any& iVal);

// Our protocol
	vector<ZAny>& OParam();

private:
	void pTouch();

	ZRef<Rep> fRep;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZValMap_Any

class ZValMap_Any
	{
	ZOOLIB_DEFINE_OPERATOR_BOOL_TYPES(ZValMap_Any,
		operator_bool_generator_type, operator_bool_type);

	class Rep;

public:
	typedef map<string, ZAny>::iterator Index_t;

	operator operator_bool_type() const;

	ZValMap_Any();
	ZValMap_Any(const ZValMap_Any& iOther);
	~ZValMap_Any();
	ZValMap_Any& operator=(const ZValMap_Any& iOther);

	ZValMap_Any(const map<string, ZAny>& iOther);
	ZValMap_Any& operator=(map<string, ZAny>& iOther);

	template <class T>
	ZValMap_Any(const vector<pair<string, T> >& iOther);

	template <class T>
	ZValMap_Any(const map<string, T>& iOther);

// ZValMap protocol
	void Clear();

	bool QGet(const string8& iName, ZVal_Any& oVal) const;
	bool QGet(const Index_t& iIndex, ZVal_Any& oVal) const;

	ZVal_Any DGet(const string8& iName, const ZVal_Any& iDefault) const;
	ZVal_Any DGet(const Index_t& iIndex, const ZVal_Any& iDefault) const;

	ZVal_Any Get(const string8& iName) const;
	ZVal_Any Get(const Index_t& iIndex) const;

	void Set(const string8& iName, const ZVal_Any& iVal);
	void Set(const Index_t& iIndex, const ZVal_Any& iVal);

	void Erase(const string8& iName);
	void Erase(const Index_t& iIndex);

// Our protocol
	Index_t Begin() const;
	Index_t End() const;

	string8 NameOf(const Index_t& iIndex) const;
	Index_t IndexOf(const string8& iName) const;

	Index_t IndexOf(const ZValMap_Any& iOther, const Index_t& iOtherIndex) const;

	map<string, ZAny>& OParam();

private:
	void pTouch();
	map<string, ZAny>::iterator pTouch(const Index_t& iIndex);

	ZRef<Rep> fRep;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZValMap_Any::Rep

class ZValMap_Any::Rep
:	public ZRefCounted
	{
public:
	Rep();
	virtual ~Rep();
	
	Rep(const map<string, ZAny>& iMap);

	template <class Iterator>
	Rep(Iterator begin, Iterator end)
	:	fMap(begin, end)
		{}

private:
	map<string, ZAny> fMap;
	friend class ZValMap_Any;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZValMap_Any, inline templated constructors

template <class T>
ZValMap_Any::ZValMap_Any(const vector<pair<string, T> >& iOther)
:	fRep(new Rep(iOther.begin(), iOther.end()))
	{}

template <class T>
ZValMap_Any::ZValMap_Any(const map<string, T>& iOther)
:	fRep(new Rep(iOther.begin(), iOther.end()))
	{}

NAMESPACE_ZOOLIB_END

#endif // __ZVal_Any__
