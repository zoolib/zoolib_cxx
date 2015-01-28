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

#ifndef __ZooLib_Val_Any_h__
#define __ZooLib_Val_Any_h__ 1
#include "zconfig.h"

#include "zoolib/Compat_unordered_map.h"
#include "zoolib/Data_Any.h"
#include "zoolib/Name.h"
#include "zoolib/SafePtrStack.h"
#include "zoolib/Util_Relops.h"
#include "zoolib/Val.h"
#include "zoolib/Val_T.h"

#include "zoolib/ZCounted.h"

#include <map>
#include <vector>

namespace ZooLib {

class Seq_Any;
class Map_Any;

typedef Val_T<Map_Any,Seq_Any> Val_Any;

// =================================================================================================
#pragma mark -
#pragma mark Seq_Any

class Seq_Any
	{
	class Rep;

public:
	typedef std::vector<Val_Any> Vector_t;
	typedef Val_Any Val;

	Seq_Any();
	Seq_Any(const Seq_Any& iOther);
	~Seq_Any();
	Seq_Any& operator=(const Seq_Any& iOther);

	Seq_Any(const Vector_t& iOther);
	Seq_Any& operator=(const Vector_t& iOther);

	template <class Iterator>
	Seq_Any(Iterator begin, Iterator end);

	int Compare(const Seq_Any& iOther) const;

// ZSeq protocol
	size_t Count() const;

	void Clear();

	Val_Any* PMut(size_t iIndex);
	const Val_Any* PGet(size_t iIndex) const;
	const ZQ<Val_Any> QGet(size_t iIndex) const;
	const Val_Any DGet(const Val_Any& iDefault, size_t iIndex) const;
	const Val_Any& Get(size_t iIndex) const;

	template <class S>
	const S* PGet(size_t iIndex) const
		{
		if (const Val_Any* theVal = this->PGet(iIndex))
			return theVal->PGet<S>();
		return nullptr;
		}

	template <class S>
	const ZQ<S> QGet(size_t iIndex) const
		{
		if (const Val_Any* theVal = this->PGet(iIndex))
			return theVal->QGet<S>();
		return null;
		}

	template <class S>
	const S& DGet(const S& iDefault, size_t iIndex) const
		{
		if (const S* theVal = this->PGet<S>(iIndex))
			return *theVal;
		return iDefault;
		}

	template <class S>
	const S& Get(size_t iIndex) const
		{
		if (const S* theVal = this->PGet<S>(iIndex))
			return *theVal;
		return sDefault<S>();
		}

	template <class S>
	S* PMut(size_t iIndex)
		{
		if (Val_Any* theVal = this->PMut(iIndex))
			return theVal->PMut<S>();
		return nullptr;
		}

	Seq_Any& Set(size_t iIndex, const Val_Any& iVal);

	Seq_Any& Erase(size_t iIndex);

	Seq_Any& Insert(size_t iIndex, const Val_Any& iVal);

	Seq_Any& Append(const Val_Any& iVal);

// Our protocol
	Val_Any& Mut(size_t iIndex);

	template <class S>
	S& Mut(size_t iIndex)
		{ return this->Mut(iIndex).Mut<S>(); }

	template <class S>
	S& DMut(const S& iDefault, size_t iIndex)
		{ return this->Mut(iIndex).DMut(iDefault); }

// Operators and STL-style
	bool operator==(const Seq_Any& r) const
		{ return this->Compare(r) == 0; }

	bool operator<(const Seq_Any& r) const
		{ return this->Compare(r) < 0; }

	Val_Any& operator[](size_t iIndex);
	const Val_Any& operator[](size_t iIndex) const;

	typedef Vector_t::iterator iterator;
	iterator begin();
	iterator end();

	typedef Vector_t::const_iterator const_iterator;
	const_iterator begin() const;
	const_iterator end() const;

private:
	void pTouch();

	ZRef<Rep> fRep;
	};

template <> struct RelopsTraits_HasEQ<Seq_Any> : public RelopsTraits_Has {};
template <> struct RelopsTraits_HasLT<Seq_Any> : public RelopsTraits_Has {};

// =================================================================================================
#pragma mark -
#pragma mark Seq_Any::Rep

class Seq_Any::Rep
:	public CountedWithoutFinalize
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
	friend class Seq_Any;
	};

// =================================================================================================
#pragma mark -
#pragma mark Seq_Any, inline templated constructor

template <class Iterator>
Seq_Any::Seq_Any(Iterator begin, Iterator end)
:	fRep(new Rep(begin, end))
	{}

// =================================================================================================
#pragma mark -
#pragma mark NameVal

typedef std::pair<Name, Val_Any> NameVal;

// =================================================================================================
#pragma mark -
#pragma mark Map_Any

class Map_Any
	{
public:
	class Rep;
	typedef Name Name_t;

#if 0 && ZCONFIG_SPI_Enabled(unordered_map)
	typedef unordered_map<Name_t, Val_Any> Map_t;
#else
	typedef std::map<Name_t, Val_Any> Map_t;
#endif

	typedef Map_t::iterator Index_t;
	typedef Val_Any Val_t;

	Map_Any();
	Map_Any(const Map_Any& iOther);
	~Map_Any();
	Map_Any& operator=(const Map_Any& iOther);

	Map_Any(const Map_t& iOther);
	Map_Any& operator=(Map_t& iOther);

	Map_Any(const NameVal& iNV);
	Map_Any(const char* iName, const char* iVal);
	Map_Any(const Name_t& iName, const Val_Any& iVal);

	int Compare(const Map_Any& iOther) const;

// ZMap protocol
	bool IsEmpty() const;

	void Clear();

	const Val_Any* PGet(const Name_t& iName) const;
	const Val_Any* PGet(const Index_t& iIndex) const;

	const ZQ<Val_Any> QGet(const Name_t& iName) const;
	const ZQ<Val_Any> QGet(const Index_t& iIndex) const;

	const Val_Any& DGet(const Val_Any& iDefault, const Name_t& iName) const;
	const Val_Any& DGet(const Val_Any& iDefault, const Index_t& iIndex) const;

	const Val_Any& Get(const Name_t& iName) const;
	const Val_Any& Get(const Index_t& iIndex) const;

	Val_Any* PMut(const Name_t& iName);
	Val_Any* PMut(const Index_t& iIndex);

	template <class S>
	const S* PGet(const Name_t& iName) const
		{
		if (const Val_Any* theVal = this->PGet(iName))
			return theVal->PGet<S>();
		return nullptr;
		}

	template <class S>
	const S* PGet(const Index_t& iIndex) const
		{
		if (const Val_Any* theVal = this->PGet(iIndex))
			return theVal->PGet<S>();
		return nullptr;
		}

	template <class S>
	const ZQ<S> QGet(const Name_t& iName) const
		{
		if (const Val_Any* theVal = this->PGet(iName))
			return theVal->QGet<S>();
		return null;
		}

	template <class S>
	const ZQ<S> QGet(const Index_t& iIndex) const
		{
		if (const Val_Any* theVal = this->PGet(iIndex))
			return theVal->QGet<S>();
		return null;
		}

	template <class S>
	const S& DGet(const S& iDefault, const Name_t& iName) const
		{
		if (const S* theVal = this->PGet<S>(iName))
			return *theVal;
		return iDefault;
		}

	template <class S>
	const S& DGet(const S& iDefault, const Index_t& iIndex) const
		{
		if (const S* theVal = this->PGet<S>(iIndex))
			return *theVal;
		return iDefault;
		}

	template <class S>
	const S& Get(const Name_t& iName) const
		{
		if (const S* theVal = this->PGet<S>(iName))
			return *theVal;
		return sDefault<S>();
		}

	template <class S>
	const S& Get(const Index_t& iIndex) const
		{
		if (const S* theVal = this->PGet<S>(iIndex))
			return *theVal;
		return sDefault<S>();
		}

	template <class S>
	S* PMut(const Name_t& iName)
		{
		if (Val_Any* theVal = this->PMut(iName))
			return theVal->PMut<S>();
		return nullptr;
		}

	template <class S>
	S* PMut(const Index_t& iIndex)
		{
		if (Val_Any* theVal = this->PMut(iIndex))
			return theVal->PMut<S>();
		return nullptr;
		}

	Map_Any& Set(const Name_t& iName, const Val_Any& iVal);
	Map_Any& Set(const Index_t& iIndex, const Val_Any& iVal);

	template <class S>
	Map_Any& Set(const Name_t& iName, const S& iVal)
		{ return this->Set(iName, Val_Any(iVal)); }

	template <class S>
	Map_Any& Set(const Index_t& iIndex, const S& iVal)
		{ return this->Set(iIndex, Val_Any(iVal)); }

	Map_Any& Erase(const Name_t& iName);
	Map_Any& Erase(const Index_t& iIndex);

// Our protocol
	Val_Any& Mut(const Name_t& iName);

	template <class S>
	S& Mut(const Name_t& iName)
		{ return this->Mut(iName).Mut<S>(); }

	template <class S>
	S& DMut(const S& iDefault, const Name_t& iName)
		{
		if (S* theVal = this->PMut<S>(iName))
			return *theVal;
		this->Set(iName, iDefault);
		return *this->PMut<S>(iName);
		}
	
	Index_t Begin() const;
	Index_t End() const;

	const Name_t& NameOf(const Index_t& iIndex) const;
	Index_t IndexOf(const Name_t& iName) const;

	Index_t IndexOf(const Map_Any& iOther, const Index_t& iOtherIndex) const;

	Map_Any& Set(const NameVal& iNV);

	bool operator==(const Map_Any& r) const
		{ return this->Compare(r) == 0; }

	bool operator<(const Map_Any& r) const
		{ return this->Compare(r) < 0; }

	Val_Any& operator[](const Name_t& iName);
	const Val_Any& operator[](const Name_t& iName) const;

	Val_Any& operator[](const Index_t& iIndex);
	const Val_Any& operator[](const Index_t& iIndex) const;

private:
	void pTouch();
	Map_t::iterator pTouch(const Index_t& iIndex);

	ZRef<Rep> fRep;
	};

template <> struct RelopsTraits_HasEQ<Map_Any> : public RelopsTraits_Has {};
template <> struct RelopsTraits_HasLT<Map_Any> : public RelopsTraits_Has {};

Map_Any operator*(const NameVal& iNV0, const NameVal& iNV1);

Map_Any& operator*=(Map_Any& ioMap, const NameVal& iNV);
Map_Any operator*(const Map_Any& iMap, const NameVal& iNV);

Map_Any operator*(const NameVal& iNV, const Map_Any& iMap);

Map_Any& operator*=(Map_Any& ioMap0, const Map_Any& iMap1);
Map_Any operator*(const Map_Any& iMap0, const Map_Any& iMap1);

// -----

inline const Val_Any* sPGet(const Map_Any& iMap, const Map_Any::Name_t& iName)
	{ return iMap.PGet(iName); }

inline const ZQ<Val_Any> sQGet(const Map_Any& iMap, const Map_Any::Name_t& iName)
	{ return iMap.QGet(iName); }

inline const Val_Any& sDGet(const Val_Any& iDefault, const Map_Any& iMap, const Map_Any::Name_t& iName)
	{ return iMap.DGet(iDefault, iName); }

inline const Val_Any& sGet(const Map_Any& iMap, const Map_Any::Name_t& iName)
	{ return iMap.Get(iName); }

inline Val_Any* sPMut(Map_Any& ioMap, const Map_Any::Name_t& iName)
	{ return ioMap.PMut(iName); }

inline Val_Any& sDMut(const Val_Any& iDefault, Map_Any& ioMap, const Map_Any::Name_t& iName)
	{ return ioMap.DMut(iDefault, iName); }

inline Val_Any& sMut(Map_Any& ioMap, const Map_Any::Name_t& iName)
	{ return ioMap.Mut(iName); }

// inline Val_Any& sSet(Map_Any& ioMap, const Map_Any::Name_t& iName, const Val_Any& iVal)
//	{ return ioMap.Set(iName, iVal); }

// -----

template <class S>
const S* sPGet(const Map_Any& iMap, const Map_Any::Name_t& iName)
	{ return iMap.PGet<S>(iName); }

template <class S>
const ZQ<S> sQGet(const Map_Any& iMap, const Map_Any::Name_t& iName)
	{ return iMap.QGet<S>(iName); }

template <class S>
const S& sDGet(const S& iDefault, const Map_Any& iMap, const Map_Any::Name_t& iName)
	{ return iMap.DGet<S>(iDefault, iName); }

template <class S>
const S& sGet(const Map_Any& iMap, const Map_Any::Name_t& iName)
	{ return iMap.Get<S>(iName); }

template <class S>
S* sPMut(Map_Any& ioMap, const Map_Any::Name_t& iName)
	{ return ioMap.PMut<S>(iName); }

template <class S>
S& sDMut(const S& iDefault, Map_Any& ioMap, const Map_Any::Name_t& iName)
	{ return ioMap.DMut<S>(iDefault, iName); }

template <class S>
S& sMut(Map_Any& ioMap, const Map_Any::Name_t& iName)
	{ return ioMap.Mut<S>(iName); }

template <class S>
S& sSet(Map_Any& ioMap, const Map_Any::Name_t& iName, const S& iVal)
	{ return ioMap.Set<S>(iName, iVal); }

// =================================================================================================
#pragma mark -
#pragma mark Map_Any::Rep

class SafePtrStackLink_Map_Any_Rep
:	public SafePtrStackLink<Map_Any::Rep,SafePtrStackLink_Map_Any_Rep>
	{};

class Map_Any::Rep
:	public ZCounted
,	public SafePtrStackLink_Map_Any_Rep
	{
public:
	virtual ~Rep();

private:
	Rep();

	Rep(const Map_t& iMap);

// From ZCounted
	virtual void Finalize();

// Our protocol
	static ZRef<Rep> sMake();
	static ZRef<Rep> sMake(const Map_t& iMap);

	Map_t fMap;
	friend class Map_Any;
	};

// =================================================================================================
#pragma mark -
#pragma mark 

Map_Any sAugmented(const Map_Any& iUnder, const Map_Any& iOver);

} // namespace ZooLib

#endif // __ZooLib_Val_Any_h__
