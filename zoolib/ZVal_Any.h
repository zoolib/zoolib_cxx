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

class ZVal_Any : public ZAny
	{
public:
	const ZAny& AsAny() const
		{ return *this; }

	ZVal_Any()
		{}

	ZVal_Any(const ZVal_Any& iOther)
	:	ZAny((const ZAny&)iOther)
		{}

	~ZVal_Any()
		{}

	ZVal_Any& operator=(const ZVal_Any& iOther)
		{
		ZAny::operator=((const ZAny&)iOther);
		return *this;
		}

	ZVal_Any(const ZAny& iOther)
	:	ZAny(iOther)
		{}

	ZVal_Any& operator=(const ZAny& rhs)
		{
		ZAny::operator=(rhs);
		return *this;
		}

	template <class S>
	ZVal_Any(const S& iVal)
	:	ZAny(iVal)
		{}

	template <class S>
	ZVal_Any& operator=(const S& iVal)
		{
		ZAny::operator=(iVal);
		return *this;
		}

// Typename accessors
	ZMACRO_ZValAccessors_Decl_Entry(ZVal_Any, Data, ZData_Any)
	ZMACRO_ZValAccessors_Decl_Entry(ZVal_Any, List, ZList_Any)
	ZMACRO_ZValAccessors_Decl_Entry(ZVal_Any, Map, ZMap_Any)
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

	ZAny AsAny() const;
	ZAny AsAny(const ZAny& iDefault) const;

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

	template <class S>
	S* PGet_T(size_t iIndex);

	template <class S>
	const S* PGet_T(size_t iIndex) const;

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

template <class S>
inline
S* ZList_Any::PGet_T(size_t iIndex)
	{
	if (ZVal_Any* theVal = this->PGet(iIndex))
		return theVal->PGet_T<S>();
	return nullptr;
	}

template <class S>
inline
const S* ZList_Any::PGet_T(size_t iIndex) const
	{
	if (const ZVal_Any* theVal = this->PGet(iIndex))
		return theVal->PGet_T<S>();
	return nullptr;
	}

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

	ZAny AsAny() const;
	ZAny AsAny(const ZAny& iDefault) const;

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

	template <class S>
	S* PGet_T(const string8& iName);

	template <class S>
	S* PGet_T(const Index_t& iIndex);

	template <class S>
	const S* PGet_T(const string8& iName) const;

	template <class S>
	const S* PGet_T(const Index_t& iIndex) const;

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

template <class S>
inline
S* ZMap_Any::PGet_T(const string8& iName)
	{
	if (ZVal_Any* theVal = this->PGet(iName))
		return theVal->PGet_T<S>();
	return nullptr;
	}

template <class S>
inline
S* ZMap_Any::PGet_T(const Index_t& iIndex)
	{
	if (ZVal_Any* theVal = this->PGet(iIndex))
		return theVal->PGet_T<S>();
	return nullptr;
	}

template <class S>
inline
const S* ZMap_Any::PGet_T(const string8& iName) const
	{
	if (const ZVal_Any* theVal = this->PGet(iName))
		return theVal->PGet_T<S>();
	return nullptr;
	}

template <class S>
inline
const S* ZMap_Any::PGet_T(const Index_t& iIndex) const
	{
	if (const ZVal_Any* theVal = this->PGet(iIndex))
		return theVal->PGet_T<S>();
	return nullptr;
	}

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
