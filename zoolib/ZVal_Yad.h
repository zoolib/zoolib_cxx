/* -------------------------------------------------------------------------------------------------
Copyright (c) 2011 Andrew Green
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

#ifndef __ZVal_Yad_h__
#define __ZVal_Yad_h__ 1
#include "zconfig.h"

#include "zoolib/ZVal_Any.h"
#include "zoolib/ZValAccessors.h"
#include "zoolib/ZYad.h"

#include <string>

namespace ZooLib {

class ZVal_Yad;
class ZSeq_Yad;
class ZMap_Yad;

// =================================================================================================
// MARK: - ZVal_Yad

class ZVal_Yad
	{
public:
	ZAny AsAny() const;

	ZVal_Yad();
	ZVal_Yad(const ZVal_Yad& iOther);
	~ZVal_Yad();
	ZVal_Yad& operator=(const ZVal_Yad& iOther);

	ZVal_Yad(const ZRef<ZYadR>& iYad);
	ZVal_Yad& operator=(const ZRef<ZYadR>& iYad);

	ZVal_Yad(const ZSeq_Yad& iSeq);
	ZVal_Yad& operator=(const ZSeq_Yad& iSeq);

	ZVal_Yad(const ZMap_Yad& iMap);
	ZVal_Yad& operator=(const ZMap_Yad& iMap);

// ZVal protocol
	bool IsNull() const;

	template <class S>
	ZQ<S> QGet() const;

	template <class S>
	S DGet(const S& iDefault) const
		{
		if (ZQ<S> theQ = this->QGet<S>())
			return theQ.Get();
		return iDefault;
		}

	template <class S>
	S Get() const
		{
		if (ZQ<S> theQ = this->QGet<S>())
			return theQ.Get();
		return S();
		}

// Shortcut access to values in an enclosed Map.
	ZQ<ZVal_Yad> QGet(const string8& iName) const;
	ZVal_Yad Get(const string8& iName) const;

	template <class S>
	ZQ<S> QGet(const string8& iName) const
		{ return this->Get(iName).QGet<S>(); }

	template <class S>
	S Get(const string8& iName) const
		{ return this->Get(iName).Get<S>(); }

// Shortcut access to values in an enclosed Seq.
	ZQ<ZVal_Yad> QGet(const size_t& iIndex) const;
	ZVal_Yad Get(const size_t& iIndex) const;

	template <class S>
	ZQ<S> QGet(const size_t& iIndex) const
		{ return this->Get(iIndex).QGet<S>(); }

	template <class S>
	S Get(const size_t& iIndex) const
		{ return this->Get(iIndex).Get<S>(); }

// Typename accessors
/// \cond DoxygenIgnore
	ZMACRO_ZValAccessors_Decl_GetSet(ZVal_Yad, Seq, ZSeq_Yad)
	ZMACRO_ZValAccessors_Decl_GetSet(ZVal_Yad, Map, ZMap_Yad)
/// \endcond DoxygenIgnore

// Our protocol
	ZRef<ZYadR> GetYad() const;

private:
	ZQ<ZAny> pQAsAny() const;
	ZRef<ZYadR> fYad;
	};

template <>
ZQ<ZSeq_Yad> ZVal_Yad::QGet() const;

template <>
ZQ<ZMap_Yad> ZVal_Yad::QGet() const;

template <class S>
ZQ<S> ZVal_Yad::QGet() const
	{
	if (ZQ<ZAny> theQ = this->pQAsAny())
   		return theQ->QGet<S>();
	return null;
	}

// =================================================================================================
// MARK: - ZSeq_Yad

class ZSeq_Yad
	{
public:
	ZSeq_Yad();
	ZSeq_Yad(const ZSeq_Yad& iOther);
	~ZSeq_Yad();
	ZSeq_Yad& operator=(const ZSeq_Yad& iOther);

	ZSeq_Yad(const ZRef<ZYadSeqAtR>& iYad);
	ZSeq_Yad& operator=(const ZRef<ZYadSeqAtR>& iYad);

// ZSeq protocol
	size_t Count() const;

	void Clear();

	ZQ<ZVal_Yad> QGet(size_t iIndex) const;
	ZVal_Yad DGet(const ZVal_Yad& iDefault, size_t iIndex) const;
	ZVal_Yad Get(size_t iIndex) const;

	template <class S>
	ZQ<S> QGet(size_t iIndex) const
		{
		if (ZQ<ZVal_Yad> theQ = this->QGet(iIndex))
			return theQ->QGet<S>();
		return null;
		}

	template <class S>
	S DGet(const S& iDefault, size_t iIndex) const
		{
		if (ZQ<ZVal_Yad> theQ = this->QGet(iIndex))
			{
			if (ZQ<S> theQ2 = theQ->QGet<S>())
				return *theQ2;
			}
		return iDefault;
		}

	template <class S>
	S Get(size_t iIndex) const
		{
		if (ZQ<ZVal_Yad> theQ = this->QGet(iIndex))
			{
			if (ZQ<S> theQ2 = theQ->QGet<S>())
				return *theQ2;
			}
		return S();
		}

// Our protocol
	ZRef<ZYadSeqAtR> GetYad() const;

private:
	mutable ZRef<ZYadSeqAtR> fYad;
	};

// =================================================================================================
// MARK: - ZMap_Yad

class ZMap_Yad
	{
public:
	ZMap_Yad();
	ZMap_Yad(const ZMap_Yad& iOther);
	~ZMap_Yad();
	ZMap_Yad& operator=(const ZMap_Yad& iOther);

	ZMap_Yad(const ZRef<ZYadMapAtR>& iYad);
	ZMap_Yad& operator=(const ZRef<ZYadMapAtR>& iYad);

// ZMap protocol
	void Clear();

	ZQ<ZVal_Yad> QGet(const string8& iName) const;
	ZVal_Yad DGet(const ZVal_Yad& iDefault, const string8& iName) const;
	ZVal_Yad Get(const string8& iName) const;

	template <class S>
	ZQ<S> QGet(const string8& iName) const
		{
		if (ZQ<ZVal_Yad> theQ = this->QGet(iName))
			return theQ->QGet<S>();
		return null;
		}

	template <class S>
	S DGet(const S& iDefault, const string8& iName) const
		{
		if (ZQ<ZVal_Yad> theQ = this->QGet(iName))
			{
			if (ZQ<S> theQ2 = theQ->QGet<S>())
				return *theQ2;
			}
		return iDefault;
		}

	template <class S>
	S Get(const string8& iName) const
		{
		if (ZQ<ZVal_Yad> theQ = this->QGet(iName))
			{
			if (ZQ<S> theQ2 = theQ->QGet<S>())
				return *theQ2;
			}
		return S();
		}

// Our protocol
	ZRef<ZYadMapAtR> GetYad() const;

private:
	mutable ZRef<ZYadMapAtR> fYad;
	};

} // namespace ZooLib

#endif // __ZVal_Yad_h__
