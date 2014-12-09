/* -------------------------------------------------------------------------------------------------
Copyright (c) 2012 Andrew Green
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

#ifndef __ZMap_Shared_T_h__
#define __ZMap_Shared_T_h__ 1
#include "zconfig.h"

#include "zoolib/CountedVal.h"

#include "zoolib/ZAny.h" // For sDefault
#include "zoolib/ZQ.h"

namespace ZooLib {

// =================================================================================================
// MARK: - ZMap_Shared_T

template <class Map_p, class Val_p = typename Map_p::Val_t, class Name_p = typename Map_p::Name_t>
class ZMap_Shared_T
	{
public:
	typedef Map_p Map_t;
	typedef Val_p Val_t;
	typedef Name_p Name_t;

	ZMap_Shared_T()
		{}

	ZMap_Shared_T(const ZMap_Shared_T& iOther)
	:	fRep(iOther.fRep)
		{}

	~ZMap_Shared_T()
		{}

	ZMap_Shared_T& operator=(const ZMap_Shared_T& iOther)
		{
		fRep = iOther.fRep;
		return *this;
		}

// Our ctor protocol
	explicit ZMap_Shared_T(const Map_t& iMap)
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

	const Val_t& DGet(const Val_t& iDefault, const Name_t& iName) const
		{
		if (const Val_t* theVal = this->PGet(iName))
			return *theVal;
		return iDefault;
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
	const S& DGet(const S& iDefault, const Name_t& iName) const
		{
		if (const S* theVal = this->PGet<S>(iName))
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

// Our protocol
	const Val_t& operator[](const Name_t& iName) const
		{ return this->Get(iName); }

protected:
	ZRef<CountedVal<Map_t> > fRep;
	};

// =================================================================================================
// MARK: - ZMap_Shared_Mutable_T

template <class Map_p, class Val_p = typename Map_p::Val_t, class Name_p = typename Map_p::Name_t>
class ZMap_Shared_Mutable_T
:	public ZMap_Shared_T<Map_p,Val_p,Name_p>
	{
	typedef ZMap_Shared_T<Map_p,Val_p,Name_p> inherited;
	using inherited::fRep;

public:
	typedef Map_p Map_t;
	typedef Val_p Val_t;
	typedef Name_p Name_t;

	ZMap_Shared_Mutable_T()
		{}

	ZMap_Shared_Mutable_T(const ZMap_Shared_Mutable_T& iOther)
	:	inherited(iOther)
		{}

	~ZMap_Shared_Mutable_T()
		{}

	ZMap_Shared_Mutable_T& operator=(const ZMap_Shared_Mutable_T& iOther)
		{
		inherited::operator=(iOther);
		return *this;
		}

// Our ctor protocol
	explicit ZMap_Shared_Mutable_T(const Map_t& iMap)
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
			const_cast<ZMap_Shared_Mutable_T*>(this)->fRep = sCountedVal<Map_t>();
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
			const_cast<ZMap_Shared_Mutable_T*>(this)->fRep = sCountedVal<Map_t>();
		return sMut(fRep).template Mut<S>(iName);
		}

	ZMap_Shared_Mutable_T& Set(const Name_t& iName, const Val_t& iVal)
		{
		if (not fRep)
			const_cast<ZMap_Shared_Mutable_T*>(this)->fRep = sCountedVal<Map_t>();
		sMut(fRep).Set(iName, iVal);
		return *this;
		}

	template <class S>
	ZMap_Shared_Mutable_T& Set(const Name_t& iName, const S& iVal)
		{ return this->Set(iName, Val_t(iVal)); }

	ZMap_Shared_Mutable_T& Erase(const Name_t& iName)
		{
		if (fRep)
			sMut(fRep).Erase(iName);
		return *this;
		}

	Val_t& operator[](const Name_t& iName)
		{ return this->Mut(iName); }
	};

} // namespace ZooLib

#endif // __ZMap_Shared_T_h__
