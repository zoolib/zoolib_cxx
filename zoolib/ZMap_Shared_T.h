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

#include "zoolib/ZCountedVal.h"
#include "zoolib/ZQ.h"

#include <string>

namespace ZooLib {

// =================================================================================================
// MARK: - ZMap_Shared_T

template <class Map_p, class Val_p = typename Map_p::Val>
class ZMap_Shared_T
	{
public:
	typedef Map_p Map;
	typedef Val_p Val;

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
	explicit ZMap_Shared_T(const Map& iMap)
	:	fRep(sCountedVal(iMap))
		{}

// ZMap protocol
	ZQ<Val> QGet(const std::string& iName) const
		{
		if (fRep)
			return fRep->Get().QGet(iName);
		return null;
		}

	Val DGet(const Val& iDefault, const std::string& iName) const
		{
		if (ZQ<Val> theVal = this->QGet(iName))
			return *theVal;
		return iDefault;
		}

	Val Get(const std::string& iName) const
		{
		if (ZQ<Val> theVal = this->QGet(iName))
			return *theVal;
		return Val();
		}

	template <class S>
	ZQ<S> QGet(const std::string& iName) const
		{
		if (ZQ<Val> theQ = this->QGet(iName))
			return theQ->QGet<S>();
		return null;
		}

	template <class S>
	S DGet(const S& iDefault, const std::string& iName) const
		{
		if (ZQ<Val> theQ = this->QGet(iName))
			{
			if (ZQ<S> theQ2 = theQ->QGet<S>())
				return *theQ2;
			}
		return iDefault;
		}

	template <class S>
	S Get(const std::string& iName) const
		{
		if (ZQ<Val> theQ = this->QGet(iName))
			{
			if (ZQ<S> theQ2 = theQ->QGet<S>())
				return *theQ2;
			}
		return S();
		}

// Our protocol
	const Val operator[](const std::string& iName) const
		{ return this->Get(iName); }

protected:
	ZRef<ZCountedVal<Map> > fRep;
	};

// =================================================================================================
// MARK: - ZMap_Shared_Mutable_T

template <class Map_p, class Val_p = typename Map_p::Val>
class ZMap_Shared_Mutable_T
:	public ZMap_Shared_T<Map_p,Val_p>
	{
	typedef ZMap_Shared_T<Map_p,Val_p> inherited;
	using inherited::fRep;

public:
	typedef Map_p Map;
	typedef Val_p Val;

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
	explicit ZMap_Shared_Mutable_T(const Map& iMap)
	:	inherited(iMap)
		{}

// Pick up stuff from inherited
	using inherited::operator[];

// ZMap mutable protocol
	void Clear()
		{
		if (fRep)
			fRep->GetMutable().Clear();
		}

	ZMap_Shared_Mutable_T& Set(const std::string& iName, const Val& iVal)
		{
		if (not fRep)
			fRep = sCountedVal<Map>();
		fRep->GetMutable().Set(iName, iVal);
		return *this;
		}

	template <class S>
	ZMap_Shared_Mutable_T& Set(const std::string& iName, const S& iVal)
		{ return this->Set(iName, Val(iVal)); }

	ZMap_Shared_Mutable_T& Erase(const std::string& iName)
		{
		if (fRep)
			fRep->GetMutable().Erase(iName);
		return *this;
		}

// Our protocol
	Val& Mutable(const std::string& iName)
		{
		if (not fRep)
			fRep = sCountedVal<Map>();
		return fRep->GetMutable().Mutable(iName);
		}

	template <class S>
	S& Mutable(const std::string& iName)
		{
		if (not fRep)
			fRep = sCountedVal<Map>();
		return fRep->GetMutable().Mutable<S>(iName);
		}

	Val& operator[](const std::string& iName)
		{ return this->Mutable(iName); }
	};

} // namespace ZooLib

#endif // __ZMap_Shared_T_h__
