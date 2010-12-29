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
#include "zoolib/ZCountedWithoutFinalize.h"
#include "zoolib/ZData_Any.h"
#include "zoolib/ZUnicodeString.h"
#include "zoolib/ZVal.h"
#include "zoolib/ZValAccessors.h"

#include <map>
#include <vector>

namespace ZooLib {

class ZVal_Any;
class ZSeq_Any;
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
	:	ZAny(iOther.AsAny())
		{}

	~ZVal_Any()
		{}

	ZVal_Any& operator=(const ZVal_Any& iOther)
		{
		ZAny::operator=(iOther.AsAny());
		return *this;
		}

// Overload, so we don't pack a ZAny inside a ZAny
	ZVal_Any(const ZAny& iOther)
	:	ZAny(iOther)
		{}

	ZVal_Any& operator=(const ZAny& rhs)
		{
		ZAny::operator=(rhs);
		return *this;
		}

// Overload, so we can init/assign from a string constant
	ZVal_Any(const char* iVal)
	:	ZAny(string8(iVal))
		{}

	ZVal_Any& operator=(const char* iVal)
		{
		ZAny::operator=(string8(iVal));
		return *this;
		}

// Overload, so a null becomes a ZVal_Any()
	ZVal_Any(const null_t&)
	:	ZAny()
		{}

	ZVal_Any& operator=(const null_t&)
		{
		ZAny::Clear();
		return *this;
		}

// Overload, as ZAny's templated constructor is explicit.
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

	int Compare(const ZVal_Any& iOther) const;

	using ZAny::PGet;
	using ZAny::QGet;
	using ZAny::DGet;
	using ZAny::Get;

// Shortcut access to values in an enclosed Map.
	ZVal_Any* PGet(const string8& iName);
	const ZVal_Any* PGet(const string8& iName) const;
	ZVal_Any Get(const string8& iName) const;

	template <class S>
	S* PGet(const string8& iName)
		{
		if (ZVal_Any* theVal = this->PGet(iName))
			return theVal->PGet<S>();
		return nullptr;
		}

	template <class S>
	const S* PGet(const string8& iName) const
		{
		if (const ZVal_Any* theVal = this->PGet(iName))
			return theVal->PGet<S>();
		return nullptr;
		}

	template <class S>
	ZQ<S> QGet(const string8& iName) const
		{ return this->Get(iName).QGet<S>(); }

	template <class S>
	S Get(const string8& iName) const
		{ return this->Get(iName).Get<S>(); }

// Shortcut access to values in an enclosed Seq.
	ZVal_Any* PGet(size_t iIndex);
	const ZVal_Any* PGet(size_t iIndex) const;
	ZVal_Any Get(size_t iIndex) const;

	template <class S>
	S* PGet(size_t iIndex)
		{
		if (ZVal_Any* theVal = this->PGet(iIndex))
			return theVal->PGet<S>();
		return nullptr;
		}

	template <class S>
	const S* PGet(size_t iIndex) const
		{
		if (const ZVal_Any* theVal = this->PGet(iIndex))
			return theVal->PGet<S>();
		return nullptr;
		}

	template <class S>
	ZQ<S> QGet(size_t iIndex) const
		{ return this->Get(iIndex).QGet<S>(); }

	template <class S>
	S Get(size_t iIndex) const
		{ return this->Get(iIndex).Get<S>(); }

// Typename accessors
/// \cond DoxygenIgnore
	ZMACRO_ZValAccessors_Decl_Entry(ZVal_Any, Data, ZData_Any)
	ZMACRO_ZValAccessors_Decl_Entry(ZVal_Any, Seq, ZSeq_Any)
	ZMACRO_ZValAccessors_Decl_Entry(ZVal_Any, Map, ZMap_Any)
/// \endcond DoxygenIgnore

	// If these are free functions then our template constructor will
	// be tried for any free use of operator== and operator<, leading
	// to perplexing errors all over the place.
	bool operator==(const ZVal_Any& r) const
		{ return this->Compare(r) == 0; }

	bool operator<(const ZVal_Any& r) const
		{ return this->Compare(r) < 0; }
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZSeq_Any

class ZSeq_Any
	{
	class Rep;

public:
	typedef std::vector<ZVal_Any> Vector_t;
	typedef ZVal_Any Val_t;

	ZOOLIB_DEFINE_OPERATOR_BOOL_TYPES(ZSeq_Any,
		operator_bool_generator_type, operator_bool_type);

	operator operator_bool_type() const;

	ZSeq_Any();
	ZSeq_Any(const ZSeq_Any& iOther);
	~ZSeq_Any();
	ZSeq_Any& operator=(const ZSeq_Any& iOther);

	ZSeq_Any(const Vector_t& iOther);
	ZSeq_Any& operator=(const Vector_t& iOther);

	template <class Iterator>
	ZSeq_Any(Iterator begin, Iterator end);

	int Compare(const ZSeq_Any& iOther) const;

// ZSeq protocol
	size_t Count() const;

	void Clear();

	ZVal_Any* PGet(size_t iIndex);
	const ZVal_Any* PGet(size_t iIndex) const;
	ZQ<ZVal_Any> QGet(size_t iIndex) const;
	ZVal_Any DGet(const ZVal_Any& iDefault, size_t iIndex) const;
	const ZVal_Any& Get(size_t iIndex) const;

	template <class S>
	S* PGet(size_t iIndex)
		{
		if (ZVal_Any* theVal = this->PGet(iIndex))
			return theVal->PGet<S>();
		return nullptr;
		}

	template <class S>
	const S* PGet(size_t iIndex) const
		{
		if (const ZVal_Any* theVal = this->PGet(iIndex))
			return theVal->PGet<S>();
		return nullptr;
		}

	template <class S>
	ZQ<S> QGet(size_t iIndex) const
		{
		if (const ZVal_Any* theVal = this->PGet(iIndex))
			return theVal->QGet<S>();
		return null;
		}

	template <class S>
	S DGet(const S& iDefault, size_t iIndex) const
		{
		if (const S* theVal = this->PGet<S>(iIndex))
			return *theVal;
		return iDefault;
		}

	template <class S>
	S Get(size_t iIndex) const
		{
		if (const S* theVal = this->PGet<S>(iIndex))
			return *theVal;
		return S();
		}

	ZSeq_Any& Set(size_t iIndex, const ZVal_Any& iVal);

	ZSeq_Any& Erase(size_t iIndex);

	ZSeq_Any& Insert(size_t iIndex, const ZVal_Any& iVal);

	ZSeq_Any& Append(const ZVal_Any& iVal);

	bool operator==(const ZSeq_Any& r) const
		{ return this->Compare(r) == 0; }

	bool operator<(const ZSeq_Any& r) const
		{ return this->Compare(r) < 0; }

private:
	void pTouch();

	ZRef<Rep> fRep;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZSeq_Any::Rep

class ZSeq_Any::Rep
:	public ZCountedWithoutFinalize
	{
private:
	Rep();
	virtual ~Rep();

	Rep(const Vector_t& iVector);

	template <class Iterator>
	Rep(Iterator begin, Iterator end)
	:	fVector(begin, end)
		{}

	Vector_t fVector;
	friend class ZSeq_Any;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZSeq_Any, inline templated constructor

template <class Iterator>
ZSeq_Any::ZSeq_Any(Iterator begin, Iterator end)
:	fRep(new Rep(begin, end))
	{}

// =================================================================================================
#pragma mark -
#pragma mark * ZNameVal

typedef std::pair<string8, ZVal_Any> ZNameVal;

// =================================================================================================
#pragma mark -
#pragma mark * ZMap_Any

class ZMap_Any
	{
	class Rep;

public:
	typedef std::map<string8, ZVal_Any> Map_t;
	typedef Map_t::iterator Index_t;
	typedef ZVal_Any Val_t;

	ZOOLIB_DEFINE_OPERATOR_BOOL_TYPES(ZMap_Any,
		operator_bool_generator_type, operator_bool_type);

	operator operator_bool_type() const;

	ZMap_Any();
	ZMap_Any(const ZMap_Any& iOther);
	~ZMap_Any();
	ZMap_Any& operator=(const ZMap_Any& iOther);

	ZMap_Any(const Map_t& iOther);
	ZMap_Any& operator=(Map_t& iOther);

	ZMap_Any(const ZNameVal& iNV);
	ZMap_Any(const char* iName, const char* iVal);
	ZMap_Any(const string8& iName, const ZVal_Any& iVal);

	template <class Container>
	ZMap_Any(const Container& iContainer);

	template <class Iterator>
	ZMap_Any(Iterator begin, Iterator end);

	int Compare(const ZMap_Any& iOther) const;

// ZMap protocol
	void Clear();

	ZVal_Any* PGet(const string8& iName);
	ZVal_Any* PGet(const Index_t& iIndex);

	const ZVal_Any* PGet(const string8& iName) const;
	const ZVal_Any* PGet(const Index_t& iIndex) const;

	ZQ<ZVal_Any> QGet(const string8& iName) const;
	ZQ<ZVal_Any> QGet(const Index_t& iIndex) const;

	ZVal_Any DGet(const ZVal_Any& iDefault, const string8& iName) const;
	ZVal_Any DGet(const ZVal_Any& iDefault, const Index_t& iIndex) const;

	const ZVal_Any& Get(const string8& iName) const;
	const ZVal_Any& Get(const Index_t& iIndex) const;

	template <class S>
	S* PGet(const string8& iName)
		{
		if (ZVal_Any* theVal = this->PGet(iName))
			return theVal->PGet<S>();
		return nullptr;
		}

	template <class S>
	S* PGet(const Index_t& iIndex)
		{
		if (ZVal_Any* theVal = this->PGet(iIndex))
			return theVal->PGet<S>();
		return nullptr;
		}

	template <class S>
	const S* PGet(const string8& iName) const
		{
		if (const ZVal_Any* theVal = this->PGet(iName))
			return theVal->PGet<S>();
		return nullptr;
		}

	template <class S>
	const S* PGet(const Index_t& iIndex) const
		{
		if (const ZVal_Any* theVal = this->PGet(iIndex))
			return theVal->PGet<S>();
		return nullptr;
		}

	template <class S>
	ZQ<S> QGet(const string8& iName) const
		{
		if (const ZVal_Any* theVal = this->PGet(iName))
			return theVal->QGet<S>();
		return null;
		}

	template <class S>
	ZQ<S> QGet(const Index_t& iIndex) const
		{
		if (const ZVal_Any* theVal = this->PGet(iIndex))
			return theVal->QGet<S>();
		return null;
		}

	template <class S>
	S DGet(const S& iDefault, const string8& iName) const
		{
		if (const S* theVal = this->PGet<S>(iName))
			return *theVal;
		return iDefault;
		}

	template <class S>
	S DGet(const S& iDefault, const Index_t& iIndex) const
		{
		if (const S* theVal = this->PGet<S>(iIndex))
			return *theVal;
		return iDefault;
		}

	template <class S>
	S Get(const string8& iName) const
		{
		if (const S* theVal = this->PGet<S>(iName))
			return *theVal;
		return S();
		}

	template <class S>
	S Get(const Index_t& iIndex) const
		{
		if (const S* theVal = this->PGet<S>(iIndex))
			return *theVal;
		return S();
		}

	ZMap_Any& Set(const string8& iName, const ZVal_Any& iVal);
	ZMap_Any& Set(const Index_t& iIndex, const ZVal_Any& iVal);

	ZMap_Any& Erase(const string8& iName);
	ZMap_Any& Erase(const Index_t& iIndex);

// Our protocol
	Index_t Begin() const;
	Index_t End() const;

	string8 NameOf(const Index_t& iIndex) const;
	Index_t IndexOf(const string8& iName) const;

	Index_t IndexOf(const ZMap_Any& iOther, const Index_t& iOtherIndex) const;

	ZMap_Any& Set(const ZNameVal& iNV);

	bool operator==(const ZMap_Any& r) const
		{ return this->Compare(r) == 0; }

	bool operator<(const ZMap_Any& r) const
		{ return this->Compare(r) < 0; }

private:
	void pTouch();
	Map_t::iterator pTouch(const Index_t& iIndex);

	ZRef<Rep> fRep;
	};

ZMap_Any operator*(const ZNameVal& iNV0, const ZNameVal& iNV1);

ZMap_Any& operator*=(ZMap_Any& ioMap, const ZNameVal& iNV);
ZMap_Any operator*(const ZMap_Any& iMap, const ZNameVal& iNV);

ZMap_Any operator*(const ZNameVal& iNV, const ZMap_Any& iMap);

ZMap_Any& operator*=(ZMap_Any& ioMap0, const ZMap_Any& iMap1);
ZMap_Any operator*(const ZMap_Any& iMap0, const ZMap_Any& iMap1);

// =================================================================================================
#pragma mark -
#pragma mark * ZMap_Any::Rep

class ZMap_Any::Rep
:	public ZCountedWithoutFinalize
	{
private:
	Rep();
	virtual ~Rep();

	Rep(const ZNameVal& iNV);
	Rep(const Map_t& iMap);

	template <class Iterator>
	Rep(Iterator begin, Iterator end)
	:	fMap(begin, end)
		{}

	Map_t fMap;
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

} // namespace ZooLib

#endif // __ZVal_Any__
