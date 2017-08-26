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

#ifndef __ZooLib_Val_T_h__
#define __ZooLib_Val_T_h__ 1
#include "zconfig.h"

#include "zoolib/Any.h"
#include "zoolib/Compare.h"
#include "zoolib/Compat_string.h" // For strcmp
#include "zoolib/Name.h"
#include "zoolib/UnicodeString.h" // For string8 etc.
#include "zoolib/Util_Relops.h"

namespace ZooLib {

template <>
struct AnyTraits<string8>
	{
	enum { eAllowInPlace = 0 };
	};

// =================================================================================================
#pragma mark -
#pragma mark Val_T

template <class Map_p, class Seq_p>
class Val_T
:	public Any
	{
// private and unimplemented, to catch the common mistake of passing a ZQ.
	template <class S> Val_T(const ZQ<S>&);
	template <class S> Val_T& operator=(const ZQ<S>&);

public:
	typedef Name Name_t;

	Val_T()
		{}

	Val_T(const Val_T& iOther)
	:	Any(iOther.AsAny())
		{}

	~Val_T()
		{}

	Val_T& operator=(const Val_T& iOther)
		{
		Any::operator=((const Any&)(iOther));
		return *this;
		}

// Overload, so we don't pack a Any inside a Any
	Val_T(const Any& iOther)
	:	Any(iOther)
		{}

	Val_T& operator=(const Any& rhs)
		{
		Any::operator=(rhs);
		return *this;
		}

// Overload, so a null becomes a Val_T
	Val_T(const null_t&)
	:	Any()
		{}

	Val_T& operator=(const null_t&)
		{
		Any::Clear();
		return *this;
		}

// Overload, so we can init/assign from a string constant
	Val_T(const UTF8* iVal)
	:	Any(sAny<string8>(iVal))
		{}

	Val_T& operator=(const UTF8* iVal)
		{
		Any::operator=(sAny<string8>(iVal));
		return *this;
		}

	Val_T(const UTF16* iVal)
	:	Any(sAny<string16>(iVal))
		{}

	Val_T& operator=(const UTF16* iVal)
		{
		Any::operator=(sAny<string16>(iVal));
		return *this;
		}

	Val_T(const UTF32* iVal)
	:	Any(string32(iVal))
		{}

	Val_T& operator=(const UTF32* iVal)
		{
		Any::operator=(sAny<string32>(iVal));
		return *this;
		}

// Overload, as Any's templated constructor is explicit.
	template <class S>
	Val_T(const S& iVal)
	:	Any(iVal)
		{}

	template <class S>
	Val_T& operator=(const S& iVal)
		{
		Any::operator=(iVal);
		return *this;
		}

	int Compare(const Val_T& iOther) const
		{
		const char* typeName = this->Type().name();
		if (int compare = strcmp(typeName, iOther.Type().name()))
			return compare;
		return Comparer::sCompare(typeName, this->ConstVoidStar(), iOther.ConstVoidStar());
		}
	
	using Any::PGet;
	using Any::QGet;
	using Any::DGet;
	using Any::PMut;
	using Any::Mut;
	using Any::Get;
	using Any::Set;

// Shortcut access to values in an enclosed Seq.
	const Val_T* PGet(size_t iIndex) const
		{
		if (const Seq_p* asSeq = this->PGet<Seq_p>())
			return asSeq->PGet(iIndex);
		return nullptr;
		}

	const ZQ<Val_T> QGet(size_t iIndex) const
		{ return this->Get<Seq_p>().QGet(iIndex); }

	const Val_T& Get(size_t iIndex) const
		{ return this->Get<Seq_p>().Get(iIndex); }

	Val_T* PMut(size_t iIndex)
		{
		if (Seq_p* asSeq = this->PMut<Seq_p>())
			return asSeq->PMut(iIndex);
		return nullptr;
		}

	Val_T& Mut(size_t iIndex)
		{ return this->Mut<Seq_p>().Mut(iIndex); }

	template <class S>
	const S* PGet(size_t iIndex) const
		{
		if (const Val_T* theVal = this->PGet(iIndex))
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
		if (Val_T* theVal = this->PMut(iIndex))
			return theVal->PMut<S>();
		return nullptr;
		}

	template <class S>
	S& Mut(size_t iIndex)
		{ return this->Mut(iIndex).template Mut<S>(); }

	const Val_T& operator[](size_t iIndex) const
		{ return this->Get(iIndex); }

// Shortcut access to values in an enclosed Map.
	const Val_T* PGet(const Name_t& iName) const
		{
		if (const Map_p* asMap = this->PGet<Map_p>())
			return asMap->PGet(iName);
		return nullptr;
		}

	const ZQ<Val_T> QGet(const Name_t& iName) const
		{ return this->Get<Map_p>().QGet(iName); }

	const Val_T& Get(const Name_t& iName) const
		{ return this->Get<Map_p>().Get(iName); }

	Val_T* PMut(const Name_t& iName)
		{
		if (Map_p* asMap = this->PMut<Map_p>())
			return asMap->PMut(iName);
		return nullptr;
		}

	Val_T& Mut(const Name_t& iName)
		{ return this->Mut<Map_p>().Mut(iName); }

	template <class S>
	const S* PGet(const Name_t& iName) const
		{
		if (const Val_T* theVal = this->PGet(iName))
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
		if (Val_T* theVal = this->PMut(iName))
			return theVal->PMut<S>();
		return nullptr;
		}

	template <class S>
	S& Mut(const Name_t& iName) const
		{ return this->Mut(iName).template Mut<S>(); }

	Val_T& operator[](const Name_t& iName)
		{ return this->Mut(iName); }

	const Val_T& operator[](const Name_t& iName) const
		{ return this->Get(iName); }

	// If these are free functions then our template constructor will
	// be tried for any free use of operator== and operator<, leading
	// to perplexing errors all over the place.
	bool operator==(const Val_T& r) const
		{ return this->Compare(r) == 0; }

	bool operator<(const Val_T& r) const
		{ return this->Compare(r) < 0; }
	};

template <class Map_p, class Seq_p>
struct RelopsTraits_HasEQ<Val_T<Map_p,Seq_p> > : public RelopsTraits_Has {};

template <class Map_p, class Seq_p>
struct RelopsTraits_HasLT<Val_T<Map_p,Seq_p> > : public RelopsTraits_Has {};

} // namespace ZooLib

#endif // __ZooLib_Val_T_h__
