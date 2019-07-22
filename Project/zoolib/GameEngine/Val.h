// Copyright (c) 2019 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_GameEngine_Val_h__
#define __ZooLib_GameEngine_Val_h__ 1
#include "zconfig.h"

#include "zoolib/CountedVal.h"
#include "zoolib/Val_Any.h"
#include "zoolib/Val_T.h"
#include "zoolib/ValueOnce.h"

namespace ZooLib {
namespace GameEngine {

typedef CountedVal<Name> CountedName;

class Seq;
class Map;

typedef Val_T<Map,Seq> Val;

class Link;

// =================================================================================================
#pragma mark - Seq

class Seq
	{
public:
	typedef Val Val_t;

	Seq();
	Seq(const Seq& iOther);
	~Seq();
	Seq& operator=(const Seq& iOther);

	Seq(const ZP<Link>& iLink, const Seq_Any& iSeq);

// ZSeq protocol
	size_t Size() const;
	size_t Count() const
		{ return this->Size(); }

	void Clear();

	const Val* PGet(size_t iIndex) const;
	ZQ<Val> QGet(size_t iIndex) const;
	const Val& DGet(const Val& iDefault, size_t iIndex) const;
	const Val& Get(size_t iIndex) const;
	Val* PMut(size_t iIndex);
	Val& Mut(size_t iIndex);

	template <class S>
	const S* PGet(size_t iIndex) const
		{
		if (const Val* theVal = this->PGet(iIndex))
			return theVal->PGet<S>();
		return nullptr;
		}

	template <class S>
	ZQ<S> QGet(size_t iIndex) const
		{
		if (const S* theP = this->PGet<S>(iIndex))
			return *theP;
		return null;
		}

	template <class S>
	const S DGet(const S& iDefault, size_t iIndex) const
		{
		if (const S* theP = this->PGet<S>(iIndex))
			return *theP;
		return iDefault;
		}

	template <class S>
	const S Get(size_t iIndex) const
		{
		if (const S* theP = this->PGet<S>(iIndex))
			return *theP;
		return sDefault<S>();
		}

	template <class S>
	S* PMut(size_t iIndex)
		{
		if (Val* theVal = this->PMut(iIndex))
			return theVal->PMut<S>();
		return nullptr;
		}

	template <class S>
	S& Mut(size_t iIndex)
		{ return this->Mut(iIndex).Mut<S>(); }

	Seq& Set(size_t iIndex, const Val& iVal);

	Seq& Erase(size_t iIndex);

	Seq& Insert(size_t iIndex, const Val& iVal);

	Seq& Append(const Val& iVal);

// Our protocol
	Val& operator[](size_t iIndex);
	const Val& operator[](size_t iIndex) const;

private:
	void pGenSeq();

	ZP<Link> fLink;
	mutable Seq_Any fSeq_Any;
	};

// =================================================================================================
#pragma mark - Map

class Map
	{
public:
	typedef Val Val_t;
	typedef Map_Any::Name_t Name_t;

	Map();
	Map(const Map& iOther);
	~Map();
	Map& operator=(const Map& iOther);

	explicit Map(const ZP<Link>& iLink);

	explicit Map(const Map& iParent, const Map_Any& iMap);

// ZMap protocol
	void Clear();

	const Val* PGet(const Name_t& iName) const;
	ZQ<Val> QGet(const Name_t& iName) const;
	const Val& DGet(const Val& iDefault, const Name_t& iName) const;
	const Val& Get(const Name_t& iName) const;
	Val* PMut(const Name_t& iName);
	Val& Mut(const Name_t& iName);

	template <class S>
	const S* PGet(const Name_t& iName) const
		{
		if (const Val* theVal = this->PGet(iName))
			return theVal->PGet<S>();
		return nullptr;
		}

	template <class S>
	ZQ<S> QGet(const Name_t& iName) const
		{
		if (const S* theP = this->PGet<S>(iName))
			return *theP;
		return null;
		}

	template <class S>
	const S& DGet(const S& iDefault, const Name_t& iName) const
		{
		if (const S* theP = this->PGet<S>(iName))
			return *theP;
		return iDefault;
		}

	template <class S>
	const S& Get(const Name_t& iName) const
		{
		if (const S* theP = this->PGet<S>(iName))
			return *theP;
		return sDefault<S>();
		}

	template <class S>
	S* PMut(const Name_t& iName)
		{
		if (Val* theVal = this->PMut(iName))
			return theVal->PMut<S>();
		return nullptr;
		}

	template <class S>
	S& Mut(const Name_t& iName)
		{ return this->Mut(iName).Mut<S>(); }

	Map& Set(const Name_t& iName, const Val& iVal);

	template <class S>
	Map& Set(const Name_t& iName, const S& iVal)
		{ return this->Set(iName, Val(iVal)); }

	Map& Erase(const Name_t& iName);

// Our protocol
	Val& operator[](const Name_t& iName);
	const Val& operator[](const Name_t& iName) const;

	ZP<Link> GetLink() const;

private:
	ZP<Link> fLink;
	mutable Map_Any fMap_Any;
	};

// =================================================================================================
#pragma mark - Map

Map sYadTree(const Map_Any& iMap_Any, const std::string& iProtoName);

Map sParameterizedYadTree(const Map& iBase,
	const std::string& iRootAugmentName, const Map& iRootAugment);

} // namespace GameEngine
} // namespace ZooLib

#endif // __ZooLib_GameEngine_Val_h__
