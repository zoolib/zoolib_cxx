// Copyright (c) 2009-2019 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Val_ZZ_h__
#define __ZooLib_Val_ZZ_h__ 1
#include "zconfig.h"

#include "zoolib/Compat_unordered_map.h"
#include "zoolib/Counted.h"
#include "zoolib/Data_ZZ.h"
#include "zoolib/Name.h"
#include "zoolib/SafePtrStack.h"
#include "zoolib/Util_Relops.h"
#include "zoolib/Val.h"
#include "zoolib/Val_T.h"

#include <map>
#include <vector>

namespace ZooLib {

class Seq_ZZ;
class Map_ZZ;

typedef Val_T<Map_ZZ,Seq_ZZ> Val_ZZ;

// =================================================================================================
#pragma mark - Seq_ZZ

class Seq_ZZ
	{
	class Rep;

public:
	typedef std::vector<Val_ZZ> Vector_t;
	typedef Val_ZZ Val;

	Seq_ZZ();
	Seq_ZZ(const Seq_ZZ& iOther);
	~Seq_ZZ();
	Seq_ZZ& operator=(const Seq_ZZ& iOther);

	Seq_ZZ(const Vector_t& iOther);
	Seq_ZZ& operator=(const Vector_t& iOther);

	template <class Iterator>
	Seq_ZZ(Iterator begin, Iterator end);

	int Compare(const Seq_ZZ& iOther) const;

// ZSeq protocol
	size_t Size() const;
	size_t Count() const { return this->Size(); }

	void Clear();

	Val_ZZ* PMut(size_t iIndex);
	const Val_ZZ* PGet(size_t iIndex) const;
	const ZQ<Val_ZZ> QGet(size_t iIndex) const;
	const Val_ZZ DGet(const Val_ZZ& iDefault, size_t iIndex) const;
	const Val_ZZ& Get(size_t iIndex) const;

	template <class S>
	const S* PGet(size_t iIndex) const
		{
		if (const Val_ZZ* theVal = this->PGet(iIndex))
			return theVal->PGet<S>();
		return nullptr;
		}

	template <class S>
	const ZQ<S> QGet(size_t iIndex) const
		{
		if (const Val_ZZ* theVal = this->PGet(iIndex))
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
		if (Val_ZZ* theVal = this->PMut(iIndex))
			return theVal->PMut<S>();
		return nullptr;
		}

	Seq_ZZ& Set(size_t iIndex, const Val_ZZ& iVal);

	Seq_ZZ& Erase(size_t iIndex);

	Seq_ZZ& Insert(size_t iIndex, const Val_ZZ& iVal);

	Seq_ZZ& Append(const Val_ZZ& iVal);

// Our protocol
	Val_ZZ& Mut(size_t iIndex);

	template <class S>
	S& Mut(size_t iIndex)
		{ return this->Mut(iIndex).Mut<S>(); }

	template <class S>
	S& DMut(const S& iDefault, size_t iIndex)
		{ return this->Mut(iIndex).DMut(iDefault); }

// Operators and STL-style
	bool operator==(const Seq_ZZ& r) const
		{ return this->Compare(r) == 0; }

	bool operator<(const Seq_ZZ& r) const
		{ return this->Compare(r) < 0; }

	Val_ZZ& operator[](size_t iIndex);
	const Val_ZZ& operator[](size_t iIndex) const;

	typedef Vector_t::iterator iterator;
	iterator begin();
	iterator end();

	typedef Vector_t::const_iterator const_iterator;
	const_iterator begin() const;
	const_iterator end() const;

private:
	void pTouch();

	ZP<Rep> fRep;
	};

template <> struct RelopsTraits_HasEQ<Seq_ZZ> : public RelopsTraits_Has {};
template <> struct RelopsTraits_HasLT<Seq_ZZ> : public RelopsTraits_Has {};

template <>
inline int sCompare_T(const Seq_ZZ& iL, const Seq_ZZ& iR)
	{ return iL.Compare(iR); }

// =================================================================================================
#pragma mark - Seq_ZZ::Rep

class Seq_ZZ::Rep
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
	friend class Seq_ZZ;
	};

// =================================================================================================
#pragma mark - Seq_ZZ, inline templated constructor

template <class Iterator>
Seq_ZZ::Seq_ZZ(Iterator begin, Iterator end)
:	fRep(new Rep(begin, end))
	{}

// =================================================================================================
#pragma mark - NameVal

typedef std::pair<Name, Val_ZZ> NameVal;

// =================================================================================================
#pragma mark - Map_ZZ

class Map_ZZ
	{
public:
	class Rep;
	typedef Name Name_t;

#if 0 && ZCONFIG_SPI_Enabled(unordered_map)
	typedef unordered_map<Name_t, Val_ZZ> Map_t;
#else
	typedef std::map<Name_t, Val_ZZ> Map_t;
#endif

	typedef Map_t::iterator Index_t;
	typedef Val_ZZ Val_t;

	Map_ZZ();
	Map_ZZ(const Map_ZZ& iOther);
	~Map_ZZ();
	Map_ZZ& operator=(const Map_ZZ& iOther);

	Map_ZZ(const Map_t& iOther);
	Map_ZZ& operator=(Map_t& iOther);

	Map_ZZ(const NameVal& iNV);
	Map_ZZ(const char* iName, const char* iVal);
	Map_ZZ(const Name_t& iName, const Val_ZZ& iVal);

	int Compare(const Map_ZZ& iOther) const;

// ZMap protocol
	bool IsEmpty() const;

	void Clear();

	const Val_ZZ* PGet(const Name_t& iName) const;
	const Val_ZZ* PGet(const Index_t& iIndex) const;

	const ZQ<Val_ZZ> QGet(const Name_t& iName) const;
	const ZQ<Val_ZZ> QGet(const Index_t& iIndex) const;

	const Val_ZZ& DGet(const Val_ZZ& iDefault, const Name_t& iName) const;
	const Val_ZZ& DGet(const Val_ZZ& iDefault, const Index_t& iIndex) const;

	const Val_ZZ& Get(const Name_t& iName) const;
	const Val_ZZ& Get(const Index_t& iIndex) const;

	Val_ZZ* PMut(const Name_t& iName);
	Val_ZZ* PMut(const Index_t& iIndex);

	template <class S>
	const S* PGet(const Name_t& iName) const
		{
		if (const Val_ZZ* theVal = this->PGet(iName))
			return theVal->PGet<S>();
		return nullptr;
		}

	template <class S>
	const S* PGet(const Index_t& iIndex) const
		{
		if (const Val_ZZ* theVal = this->PGet(iIndex))
			return theVal->PGet<S>();
		return nullptr;
		}

	template <class S>
	const ZQ<S> QGet(const Name_t& iName) const
		{
		if (const Val_ZZ* theVal = this->PGet(iName))
			return theVal->QGet<S>();
		return null;
		}

	template <class S>
	const ZQ<S> QGet(const Index_t& iIndex) const
		{
		if (const Val_ZZ* theVal = this->PGet(iIndex))
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
		if (Val_ZZ* theVal = this->PMut(iName))
			return theVal->PMut<S>();
		return nullptr;
		}

	template <class S>
	S* PMut(const Index_t& iIndex)
		{
		if (Val_ZZ* theVal = this->PMut(iIndex))
			return theVal->PMut<S>();
		return nullptr;
		}

	Map_ZZ& Set(const Name_t& iName, const Val_ZZ& iVal);
	Map_ZZ& Set(const Index_t& iIndex, const Val_ZZ& iVal);

	template <class S>
	Map_ZZ& Set(const Name_t& iName, const S& iVal)
		{ return this->Set(iName, Val_ZZ(iVal)); }

	template <class S>
	Map_ZZ& Set(const Index_t& iIndex, const S& iVal)
		{ return this->Set(iIndex, Val_ZZ(iVal)); }

	Map_ZZ& Erase(const Name_t& iName);
	Map_ZZ& Erase(const Index_t& iIndex);

// Our protocol
	Val_ZZ& Mut(const Name_t& iName);

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

	typedef Map_t::iterator iterator;
	iterator begin();
	iterator end();

	typedef Map_t::const_iterator const_iterator;
	const_iterator begin() const;
	const_iterator end() const;

	const Name_t& NameOf(const Index_t& iIndex) const;
	Index_t IndexOf(const Name_t& iName) const;

	Index_t IndexOf(const Map_ZZ& iOther, const Index_t& iOtherIndex) const;

	Map_ZZ& Set(const NameVal& iNV);

	bool operator==(const Map_ZZ& r) const
		{ return this->Compare(r) == 0; }

	bool operator<(const Map_ZZ& r) const
		{ return this->Compare(r) < 0; }

	Val_ZZ& operator[](const Name_t& iName);
	const Val_ZZ& operator[](const Name_t& iName) const;

	Val_ZZ& operator[](const Index_t& iIndex);
	const Val_ZZ& operator[](const Index_t& iIndex) const;

private:
	void pTouch();
	Map_t::iterator pTouch(const Index_t& iIndex);

	ZP<Rep> fRep;
	};

template <> struct RelopsTraits_HasEQ<Map_ZZ> : public RelopsTraits_Has {};
template <> struct RelopsTraits_HasLT<Map_ZZ> : public RelopsTraits_Has {};

template <>
inline int sCompare_T(const Map_ZZ& iL, const Map_ZZ& iR)
	{ return iL.Compare(iR); }

Map_ZZ operator*(const NameVal& iNV0, const NameVal& iNV1);

Map_ZZ& operator*=(Map_ZZ& ioMap, const NameVal& iNV);
Map_ZZ operator*(const Map_ZZ& iMap, const NameVal& iNV);

Map_ZZ operator*(const NameVal& iNV, const Map_ZZ& iMap);

Map_ZZ& operator*=(Map_ZZ& ioMap0, const Map_ZZ& iMap1);
Map_ZZ operator*(const Map_ZZ& iMap0, const Map_ZZ& iMap1);

// -----

inline const Val_ZZ* sPGet(const Map_ZZ& iMap, const Map_ZZ::Name_t& iName)
	{ return iMap.PGet(iName); }

inline const ZQ<Val_ZZ> sQGet(const Map_ZZ& iMap, const Map_ZZ::Name_t& iName)
	{ return iMap.QGet(iName); }

inline const Val_ZZ& sDGet(const Val_ZZ& iDefault, const Map_ZZ& iMap, const Map_ZZ::Name_t& iName)
	{ return iMap.DGet(iDefault, iName); }

inline const Val_ZZ& sGet(const Map_ZZ& iMap, const Map_ZZ::Name_t& iName)
	{ return iMap.Get(iName); }

inline Val_ZZ* sPMut(Map_ZZ& ioMap, const Map_ZZ::Name_t& iName)
	{ return ioMap.PMut(iName); }

inline Val_ZZ& sDMut(const Val_ZZ& iDefault, Map_ZZ& ioMap, const Map_ZZ::Name_t& iName)
	{ return ioMap.DMut(iDefault, iName); }

inline Val_ZZ& sMut(Map_ZZ& ioMap, const Map_ZZ::Name_t& iName)
	{ return ioMap.Mut(iName); }

// inline Val_ZZ& sSet(Map_ZZ& ioMap, const Map_ZZ::Name_t& iName, const Val_ZZ& iVal)
//	{ return ioMap.Set(iName, iVal); }

// -----

template <class S>
const S* sPGet(const Map_ZZ& iMap, const Map_ZZ::Name_t& iName)
	{ return iMap.PGet<S>(iName); }

template <class S>
const ZQ<S> sQGet(const Map_ZZ& iMap, const Map_ZZ::Name_t& iName)
	{ return iMap.QGet<S>(iName); }

template <class S>
const S& sDGet(const S& iDefault, const Map_ZZ& iMap, const Map_ZZ::Name_t& iName)
	{ return iMap.DGet<S>(iDefault, iName); }

template <class S>
const S& sGet(const Map_ZZ& iMap, const Map_ZZ::Name_t& iName)
	{ return iMap.Get<S>(iName); }

template <class S>
S* sPMut(Map_ZZ& ioMap, const Map_ZZ::Name_t& iName)
	{ return ioMap.PMut<S>(iName); }

template <class S>
S& sDMut(const S& iDefault, Map_ZZ& ioMap, const Map_ZZ::Name_t& iName)
	{ return ioMap.DMut<S>(iDefault, iName); }

template <class S>
S& sMut(Map_ZZ& ioMap, const Map_ZZ::Name_t& iName)
	{ return ioMap.Mut<S>(iName); }

template <class S>
void sSet(Map_ZZ& ioMap, const Map_ZZ::Name_t& iName, const S& iVal)
	{ ioMap.Set<S>(iName, iVal); }

// =================================================================================================
#pragma mark - Map_ZZ::Rep

class SafePtrStackLink_Map_ZZ_Rep
:	public SafePtrStackLink<Map_ZZ::Rep,SafePtrStackLink_Map_ZZ_Rep>
	{};

class Map_ZZ::Rep
:	public Counted
,	public SafePtrStackLink_Map_ZZ_Rep
	{
public:
	virtual ~Rep();

private:
	Rep();

	Rep(const Map_t& iMap);

// From Counted
	virtual void Finalize();

// Our protocol
	static ZP<Rep> spMake();
	static ZP<Rep> spMake(const Map_t& iMap);

	Map_t fMap;
	friend class Map_ZZ;
	};

// =================================================================================================
#pragma mark -

Map_ZZ sAugmented(const Map_ZZ& iUnder, const Map_ZZ& iOver);

} // namespace ZooLib

#endif // __ZooLib_Val_ZZ_h__
