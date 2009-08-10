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
#include "zoolib/ZData_Any.h"
#include "zoolib/ZRef.h"
#include "zoolib/ZRef_Counted.h"
#include "zoolib/ZUnicodeString.h"
#include "zoolib/ZVal.h"
#include "zoolib/ZValAccessors.h"

#include <map>
#include <vector>

NAMESPACE_ZOOLIB_BEGIN

using std::map;
using std::pair;
using std::string;
using std::vector;

class ZVal_Any;
class ZList_Any;
class ZMap_Any;

// =================================================================================================
#pragma mark -
#pragma mark * ZVal_Any

class ZVal_Any
	{
	ZOOLIB_DEFINE_OPERATOR_BOOL_TYPES(ZVal_Any,
		operator_bool_generator_type, operator_bool_type);

public:
	ZVal_Any AsVal_Any();
	ZVal_Any AsVal_Any(const ZVal_Any& iDefault);

	const ZAny& AsAny() const;

	operator operator_bool_type() const;

	ZVal_Any();
	ZVal_Any(const ZVal_Any& iOther);
	~ZVal_Any();
	ZVal_Any& operator=(const ZVal_Any& iOther);

	ZVal_Any(const ZAny& iVal);

	ZVal_Any& operator=(const ZAny& iVal);

	template <class S>
	ZVal_Any(const S& iVal)
	:	fAny(iVal)
		{}

	template <class S>
	ZVal_Any& operator=(const S& iVal)
		{
		fAny = iVal;
		return *this;
		}

// ZVal protocol
	void Clear();

	template <class S>
	S* PGet_T()
		{ return ZAnyCast<S>(&fAny); }

	template <class S>
	const S* PGet_T() const
		{ return ZAnyCast<S>(&fAny); }

	template <class S>
	bool QGet_T(S& oVal) const
		{
		if (const S* theVal = this->PGet_T<S>())
			{
			oVal = *theVal;
			return true;
			}
		return false;
		}

	template <class S>
	S DGet_T(const S& iDefault) const
		{
		if (const S* theVal = this->PGet_T<S>())
			return *theVal;
		return iDefault;
		}

	template <class S>
	S Get_T() const
		{
		if (const S* theVal = this->PGet_T<S>())
			return *theVal;
		return S();
		}

	template <class S>
	void Set_T(const S& iVal)
		{ fAny = iVal; }

// Our protocol
	template <class S>
	bool Is_T() const
		{ return ZAnyCast<S>(&fAny); }

// Typename accessors
	ZMACRO_ZValAccessors_Decl_Entry(ZVal_Any, Data, ZData_Any)
	ZMACRO_ZValAccessors_Decl_Entry(ZVal_Any, List, ZList_Any)
	ZMACRO_ZValAccessors_Decl_Entry(ZVal_Any, Map, ZMap_Any)

private:
	ZAny fAny;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZList_Any

class ZList_Any
	{
	ZOOLIB_DEFINE_OPERATOR_BOOL_TYPES(ZList_Any,
		operator_bool_generator_type, operator_bool_type);

	class Rep;

public:
	typedef ZVal_Any Val_t;

	ZList_Any AsList_Any();
	ZList_Any AsList_Any(const ZVal_Any& iDefault);

	operator operator_bool_type() const;

	ZList_Any();
	ZList_Any(const ZList_Any& iOther);
	~ZList_Any();
	ZList_Any& operator=(const ZList_Any& iOther);

	ZList_Any(const vector<ZVal_Any>& iOther);

	ZList_Any& operator=(const vector<ZVal_Any>& iOther);

	template <class Iterator>
	ZList_Any(Iterator begin, Iterator end);

// ZList protocol
	size_t Count() const;

	void Clear();

	ZVal_Any* PGet(size_t iIndex);
	const ZVal_Any* PGet(size_t iIndex) const;
	bool QGet(size_t iIndex, ZVal_Any& oVal) const;
	ZVal_Any DGet(const ZVal_Any& iDefault, size_t iIndex) const;
	ZVal_Any Get(size_t iIndex) const;

	void Set(size_t iIndex, const ZVal_Any& iVal);

	void Erase(size_t iIndex);

	void Insert(size_t iIndex, const ZVal_Any& iVal);

	void Append(const ZVal_Any& iVal);

private:
	void pTouch();

	ZRef<Rep> fRep;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZList_Any::Rep

class ZList_Any::Rep
:	public ZRefCounted
	{
private:
	Rep();
	virtual ~Rep();
	
	Rep(const vector<ZVal_Any>& iVector);

	template <class Iterator>
	Rep(Iterator begin, Iterator end)
	:	fVector(begin, end)
		{}

	vector<ZVal_Any> fVector;
	friend class ZList_Any;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZList_Any, inline templated constructor

template <class Iterator>
ZList_Any::ZList_Any(Iterator begin, Iterator end)
:	fRep(new Rep(begin, end))
	{}

// =================================================================================================
#pragma mark -
#pragma mark * ZMap_Any

class ZMap_Any
	{
	ZOOLIB_DEFINE_OPERATOR_BOOL_TYPES(ZMap_Any,
		operator_bool_generator_type, operator_bool_type);

	class Rep;

public:
	typedef map<string, ZVal_Any>::iterator Index_t;
	typedef ZVal_Any Val_t;

	ZMap_Any AsMap_Any();
	ZMap_Any AsMap_Any(const ZVal_Any& iDefault);

	operator operator_bool_type() const;

	ZMap_Any();
	ZMap_Any(const ZMap_Any& iOther);
	~ZMap_Any();
	ZMap_Any& operator=(const ZMap_Any& iOther);

	ZMap_Any(const map<string, ZVal_Any>& iOther);
	ZMap_Any& operator=(map<string, ZVal_Any>& iOther);

	template <class Container>
	ZMap_Any(const Container& iContainer);

	template <class Iterator>
	ZMap_Any(Iterator begin, Iterator end);

// ZMap protocol
	void Clear();

	ZVal_Any* PGet(const string8& iName);
	ZVal_Any* PGet(const Index_t& iIndex);

	const ZVal_Any* PGet(const string8& iName) const;
	const ZVal_Any* PGet(const Index_t& iIndex) const;

	bool QGet(const string8& iName, ZVal_Any& oVal) const;
	bool QGet(const Index_t& iIndex, ZVal_Any& oVal) const;

	ZVal_Any DGet(const ZVal_Any& iDefault, const string8& iName) const;
	ZVal_Any DGet(const ZVal_Any& iDefault, const Index_t& iIndex) const;

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

	Index_t IndexOf(const ZMap_Any& iOther, const Index_t& iOtherIndex) const;

private:
	void pTouch();
	map<string, ZVal_Any>::iterator pTouch(const Index_t& iIndex);

	ZRef<Rep> fRep;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZMap_Any::Rep

class ZMap_Any::Rep
:	public ZRefCounted
	{
private:
	Rep();
	virtual ~Rep();
	
	Rep(const map<string, ZVal_Any>& iMap);

	template <class Iterator>
	Rep(Iterator begin, Iterator end)
	:	fMap(begin, end)
		{}

	map<string, ZVal_Any> fMap;
	friend class ZMap_Any;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZMap_Any, inline templated constructor

template <class Container>
ZMap_Any::ZMap_Any(const Container& iContainer)
:	fRep(new Rep(iContainer.begin(), iContainer.end()))
	{}

template <class Iterator>
ZMap_Any::ZMap_Any(Iterator begin, Iterator end)
:	fRep(new Rep(begin, end))
	{}

NAMESPACE_ZOOLIB_END

#endif // __ZVal_Any__
