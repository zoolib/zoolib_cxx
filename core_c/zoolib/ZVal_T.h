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

#ifndef __ZVal_T_h__
#define __ZVal_T_h__ 1
#include "zconfig.h"

#include "zoolib/ZAny.h"
#include "zoolib/ZCompare.h"
#include "zoolib/ZCompat_string.h" // For strcmp
#include "zoolib/ZName.h"
#include "zoolib/ZUnicodeString.h" // For string8 etc.
#include "zoolib/ZUtil_Relops.h"
#include "zoolib/ZVal_T.h"

namespace ZooLib {

template <>
struct ZAnyTraits<string8>
	{
	enum { eAllowInPlace = 0 };
	};

// =================================================================================================
// MARK: - ZVal_T

template <class Map_p, class Seq_p>
class ZVal_T
:	public ZAny
	{
// private and unimplemented, to catch the common mistake of passing a ZQ.
	template <class S> ZVal_T(const ZQ<S>&);
	template <class S> ZVal_T& operator=(const ZQ<S>&);

public:
	typedef ZName Name_t;

	ZVal_T()
		{}

	ZVal_T(const ZVal_T& iOther)
	:	ZAny(iOther.AsAny())
		{}

	~ZVal_T()
		{}

	ZVal_T& operator=(const ZVal_T& iOther)
		{
		ZAny::operator=((const ZAny&)(iOther));
		return *this;
		}

// Overload, so we don't pack a ZAny inside a ZAny
	ZVal_T(const ZAny& iOther)
	:	ZAny(iOther)
		{}

	ZVal_T& operator=(const ZAny& rhs)
		{
		ZAny::operator=(rhs);
		return *this;
		}

// Overload, so a null becomes a ZVal_T
	ZVal_T(const null_t&)
	:	ZAny()
		{}

	ZVal_T& operator=(const null_t&)
		{
		ZAny::Clear();
		return *this;
		}

// Overload, so we can init/assign from a string constant
	ZVal_T(const UTF8* iVal)
	:	ZAny(sAny<string8>(iVal))
		{}

	ZVal_T& operator=(const UTF8* iVal)
		{
		ZAny::operator=(sAny<string8>(iVal));
		return *this;
		}

	ZVal_T(const UTF16* iVal)
	:	ZAny(sAny<string16>(iVal))
		{}

	ZVal_T& operator=(const UTF16* iVal)
		{
		ZAny::operator=(sAny<string16>(iVal));
		return *this;
		}

	ZVal_T(const UTF32* iVal)
	:	ZAny(string32(iVal))
		{}

	ZVal_T& operator=(const UTF32* iVal)
		{
		ZAny::operator=(sAny<string32>(iVal));
		return *this;
		}

// Overload, as ZAny's templated constructor is explicit.
	template <class S>
	ZVal_T(const S& iVal)
	:	ZAny(iVal)
		{}

	template <class S>
	ZVal_T& operator=(const S& iVal)
		{
		ZAny::operator=(iVal);
		return *this;
		}

	int Compare(const ZVal_T& iOther) const
		{
		const char* typeName = this->Type().name();
		if (int compare = strcmp(typeName, iOther.Type().name()))
			return compare;
		return ZCompare::sCompare(typeName, this->ConstVoidStar(), iOther.ConstVoidStar());
		}
	
	using ZAny::PGet;
	using ZAny::QGet;
	using ZAny::DGet;
	using ZAny::PMut;
	using ZAny::Mut;
	using ZAny::Get;
	using ZAny::Set;

// Shortcut access to values in an enclosed Seq.
	const ZVal_T* PGet(size_t iIndex) const
		{
		if (const Seq_p* asSeq = this->PGet<Seq_p>())
			return asSeq->PGet(iIndex);
		return nullptr;
		}

	const ZQ<ZVal_T> QGet(size_t iIndex) const
		{ return this->Get<Seq_p>().QGet(iIndex); }

	const ZVal_T& Get(size_t iIndex) const
		{ return this->Get<Seq_p>().Get(iIndex); }

	ZVal_T* PMut(size_t iIndex)
		{
		if (Seq_p* asSeq = this->PMut<Seq_p>())
			return asSeq->PMut(iIndex);
		return nullptr;
		}

	ZVal_T& Mut(size_t iIndex)
		{ return this->Mut<Seq_p>().Mut(iIndex); }

	template <class S>
	const S* PGet(size_t iIndex) const
		{
		if (const ZVal_T* theVal = this->PGet(iIndex))
			return theVal->PGet<S>();
		return nullptr;
		}

	template <class S>
	const ZQ<S> QGet(size_t iIndex) const
		{ return this->Get(iIndex).template QGet<S>(); }

	template <class S>
	const S& Get(size_t iIndex) const
		{ return this->Get(iIndex).template Get<S>(); }

	template <class S>
	S* PMut(size_t iIndex)
		{
		if (ZVal_T* theVal = this->PMut(iIndex))
			return theVal->PMut<S>();
		return nullptr;
		}

	template <class S>
	S& Mut(size_t iIndex)
		{ return this->Mut(iIndex).template Mut<S>(); }

	const ZVal_T& operator[](size_t iIndex) const
		{ return this->Get(iIndex); }

// Shortcut access to values in an enclosed Map.
	const ZVal_T* PGet(const Name_t& iName) const
		{
		if (const Map_p* asMap = this->PGet<Map_p>())
			return asMap->PGet(iName);
		return nullptr;
		}

	const ZQ<ZVal_T> QGet(const Name_t& iName) const
		{ return this->Get<Map_p>().QGet(iName); }

	const ZVal_T& Get(const Name_t& iName) const
		{ return this->Get<Map_p>().Get(iName); }

	ZVal_T* PMut(const Name_t& iName)
		{
		if (Map_p* asMap = this->PMut<Map_p>())
			return asMap->PMut(iName);
		return nullptr;
		}

	ZVal_T& Mut(const Name_t& iName)
		{ return this->Mut<Map_p>().Mut(iName); }

	template <class S>
	const S* PGet(const Name_t& iName) const
		{
		if (const ZVal_T* theVal = this->PGet(iName))
			return theVal->PGet<S>();
		return nullptr;
		}

	template <class S>
	const ZQ<S> QGet(const Name_t& iName) const
		{ return this->Get(iName).template QGet<S>(); }

	template <class S>
	const S& Get(const Name_t& iName) const
		{ return this->Get(iName).template Get<S>(); }

	template <class S>
	S* PMut(const Name_t& iName)
		{
		if (ZVal_T* theVal = this->PMut(iName))
			return theVal->PMut<S>();
		return nullptr;
		}

	template <class S>
	S& Mut(const Name_t& iName) const
		{ return this->Mut(iName).template Mut<S>(); }

	ZVal_T& operator[](const Name_t& iName)
		{ return this->Mut(iName); }

	const ZVal_T& operator[](const Name_t& iName) const
		{ return this->Get(iName); }

	// If these are free functions then our template constructor will
	// be tried for any free use of operator== and operator<, leading
	// to perplexing errors all over the place.
	bool operator==(const ZVal_T& r) const
		{ return this->Compare(r) == 0; }

	bool operator<(const ZVal_T& r) const
		{ return this->Compare(r) < 0; }
	};

template <class Map_p, class Seq_p>
struct RelopsTraits_HasEQ<ZVal_T<Map_p,Seq_p> > : public RelopsTraits_Has {};

template <class Map_p, class Seq_p>
struct RelopsTraits_HasLT<ZVal_T<Map_p,Seq_p> > : public RelopsTraits_Has {};

} // namespace ZooLib

#endif // __ZVal_T_h__
