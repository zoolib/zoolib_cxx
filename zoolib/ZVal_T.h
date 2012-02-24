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
#include "zoolib/ZUnicodeString.h" // For string8 etc.

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
	
	using ZAny::PGetMutable;
	using ZAny::PGet;
	using ZAny::QGet;
	using ZAny::DGet;
	using ZAny::Get;
	using ZAny::Set;
	using ZAny::Mutable;

// Shortcut access to values in an enclosed Seq.
	ZVal_T* PGetMutable(size_t iIndex)
		{
		if (Seq_p* asSeq = this->PGetMutable<Seq_p>())
			return asSeq->PGetMutable(iIndex);
		return nullptr;
		}

	const ZVal_T* PGet(size_t iIndex) const
		{
		if (const Seq_p* asSeq = this->PGet<Seq_p>())
			return asSeq->PGet(iIndex);
		return nullptr;
		}

	ZQ<ZVal_T> QGet(size_t iIndex) const
		{ return this->Get<Seq_p>().QGet(iIndex); }

	ZVal_T Get(size_t iIndex) const
		{ return this->Get<Seq_p>().Get(iIndex); }

	template <class S>
	S* PGetMutable(size_t iIndex)
		{
		if (ZVal_T* theVal = this->PGetMutable(iIndex))
			return theVal->PGetMutable<S>();
		return nullptr;
		}

	template <class S>
	const S* PGet(size_t iIndex) const
		{
		if (const ZVal_T* theVal = this->PGet(iIndex))
			return theVal->PGet<S>();
		return nullptr;
		}

	template <class S>
	ZQ<S> QGet(size_t iIndex) const
		{ return this->Get(iIndex).QGet<S>(); }

	template <class S>
	S Get(size_t iIndex) const
		{ return this->Get(iIndex).Get<S>(); }

	ZVal_T operator[](size_t iIndex) const
		{ return this->Get(iIndex); }

// Shortcut access to values in an enclosed Map.
	ZVal_T* PGetMutable(const string8& iName)
		{
		if (Map_p* asMap = this->PGetMutable<Map_p>())
			return asMap->PGetMutable(iName);
		return nullptr;
		}

	const ZVal_T* PGet(const string8& iName) const
		{
		if (const Map_p* asMap = this->PGet<Map_p>())
			return asMap->PGet(iName);
		return nullptr;
		}

	ZQ<ZVal_T> QGet(const string8& iName) const
		{ return this->Get<Map_p>().QGet(iName); }

	ZVal_T Get(const string8& iName) const
		{ return this->Get<Map_p>().Get(iName); }

	ZVal_T& Mutable(const string8& iName)
		{ return this->Mutable<Map_p>().Mutable(iName); }

	template <class S>
	S* PGetMutable(const string8& iName)
		{
		if (ZVal_T* theVal = this->PGetMutable(iName))
			return theVal->PGetMutable<S>();
		return nullptr;
		}

	template <class S>
	const S* PGet(const string8& iName) const
		{
		if (const ZVal_T* theVal = this->PGet(iName))
			return theVal->PGet<S>();
		return nullptr;
		}

	template <class S>
	ZQ<S> QGet(const string8& iName) const
		{ return this->Get(iName).QGet<S>(); }

	template <class S>
	S Get(const string8& iName) const
		{ return this->Get(iName).Get<S>(); }

	template <class S>
	S& Mutable(const string8& iName) const
		{ return this->Mutable(iName).Mutable<S>(); }

	ZVal_T operator[](const string8& iName) const
		{ return this->Get(iName); }

// Typename accessors
/// \cond DoxygenIgnore
//	ZMACRO_ZValAccessors_Decl_GetSet(ZVal_T, Data, ZData_Any)
//	ZMACRO_ZValAccessors_Decl_GetSet(ZVal_T, Seq, Seq_p)
//	ZMACRO_ZValAccessors_Decl_GetSet(ZVal_T, Map, Map_p)
/// \endcond DoxygenIgnore

	// If these are free functions then our template constructor will
	// be tried for any free use of operator== and operator<, leading
	// to perplexing errors all over the place.
	bool operator==(const ZVal_T& r) const
		{ return this->Compare(r) == 0; }

	bool operator<(const ZVal_T& r) const
		{ return this->Compare(r) < 0; }
	};

//ZMACRO_ZValAccessors_Def_GetSet(ZVal_Any, Data, ZData_Any)
//ZMACRO_ZValAccessors_Def_GetSet(ZVal_Any, Seq, ZSeq_Any)
//ZMACRO_ZValAccessors_Def_GetSet(ZVal_Any, Map, ZMap_Any)

} // namespace ZooLib

#endif // __ZVal_T_h__
