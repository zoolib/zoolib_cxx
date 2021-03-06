// Copyright (c) 2012 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Map_Shared_T_h__
#define __ZooLib_Map_Shared_T_h__ 1
#include "zconfig.h"

#include "zoolib/CountedVal.h"

#include "zoolib/Default.h" // For sDefault
#include "zoolib/ZQ.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - Map_Shared_T

template <class Map_p, class Val_p = typename Map_p::Val_t, class Name_p = typename Map_p::Name_t>
class Map_Shared_T
	{
public:
	typedef Map_p Map_t;
	typedef Val_p Val_t;
	typedef Name_p Name_t;

	Map_Shared_T()
		{}

	Map_Shared_T(const Map_Shared_T& iOther)
	:	fRep(iOther.fRep)
		{}

	~Map_Shared_T()
		{}

	Map_Shared_T& operator=(const Map_Shared_T& iOther)
		{
		fRep = iOther.fRep;
		return *this;
		}

// Our ctor protocol
	explicit Map_Shared_T(const Map_t& iMap)
	:	fRep(sCountedVal(iMap))
		{}

// ZMap protocol
	const Val_t* PGet(const Name_t& iName) const
		{
		if (fRep)
			return sGet(fRep).PGet(iName);
		return nullptr;
		}

	ZQ<Val_t> QGet(const Name_t& iName) const
		{
		if (const Val_t* theVal = this->PGet(iName))
			return *theVal;
		return null;
		}

	const Val_t& Get(const Name_t& iName) const
		{
		if (const Val_t* theVal = this->PGet(iName))
			return *theVal;
		return sDefault<Val_t>();
		}

	template <class S>
	const S* PGet(const Name_t& iName) const
		{
		if (fRep)
			return sGet(fRep).template PGet<S>(iName);
		return nullptr;
		}

	template <class S>
	ZQ<S> QGet(const Name_t& iName) const
		{
		if (const S* theVal = this->PGet<S>(iName))
			return *theVal;
		return null;
		}

	template <class S>
	const S& Get(const Name_t& iName) const
		{
		if (const S* theVal = this->PGet<S>(iName))
			return *theVal;
		return sDefault<S>();
		}

// Our protocol
	const Val_t& operator[](const Name_t& iName) const
		{ return this->Get(iName); }

protected:
	ZP<CountedVal<Map_t>> fRep;
	};

// =================================================================================================
#pragma mark - Map_Shared_Mutable_T

template <class Map_p, class Val_p = typename Map_p::Val_t, class Name_p = typename Map_p::Name_t>
class Map_Shared_Mutable_T
:	public Map_Shared_T<Map_p,Val_p,Name_p>
	{
	typedef Map_Shared_T<Map_p,Val_p,Name_p> inherited;
	using inherited::fRep;

public:
	typedef Map_p Map_t;
	typedef Val_p Val_t;
	typedef Name_p Name_t;

	Map_Shared_Mutable_T()
		{}

	Map_Shared_Mutable_T(const Map_Shared_Mutable_T& iOther)
	:	inherited(iOther)
		{}

	~Map_Shared_Mutable_T()
		{}

	Map_Shared_Mutable_T& operator=(const Map_Shared_Mutable_T& iOther)
		{
		inherited::operator=(iOther);
		return *this;
		}

// Our ctor protocol
	explicit Map_Shared_Mutable_T(const Map_t& iMap)
	:	inherited(iMap)
		{}

// Pick up stuff from inherited
	using inherited::operator[];

// ZMap mutable protocol
	void Clear()
		{
		if (fRep)
			sMut(fRep).Clear();
		}

	Val_t* PMut(const Name_t& iName)
		{
		if (fRep)
			return sMut(fRep).PMut(iName);
		return nullptr;
		}

	Val_t& Mut(const Name_t& iName)
		{
		if (not fRep)
			sNonConst(this)->fRep = sCountedVal<Map_t>();
		return sMut(fRep).Mut(iName);
		}

	template <class S>
	S* PMut(const Name_t& iName)
		{
		if (fRep)
			return sMut(fRep).template PMut<S>(iName);
		return nullptr;
		}

	template <class S>
	S& Mut(const Name_t& iName)
		{
		if (not fRep)
			sNonConst(this)->fRep = sCountedVal<Map_t>();
		return sMut(fRep).template Mut<S>(iName);
		}

	Map_Shared_Mutable_T& Set(const Name_t& iName, const Val_t& iVal)
		{
		if (not fRep)
			sNonConst(this)->fRep = sCountedVal<Map_t>();
		sMut(fRep).Set(iName, iVal);
		return *this;
		}

	template <class S>
	Map_Shared_Mutable_T& Set(const Name_t& iName, const S& iVal)
		{ return this->Set(iName, Val_t(iVal)); }

	Map_Shared_Mutable_T& Erase(const Name_t& iName)
		{
		if (fRep)
			sMut(fRep).Erase(iName);
		return *this;
		}

	Val_t& operator[](const Name_t& iName)
		{ return this->Mut(iName); }
	};

} // namespace ZooLib

#endif // __ZooLib_Map_Shared_T_h__
