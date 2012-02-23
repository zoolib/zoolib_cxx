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

#include "zoolib/ZVal_Any.h" // For ZSeq_Any and ZMap_Any
#include "zoolib/ZValAccessors.h"
#include "zoolib/ZYad.h"

#include <string>

namespace ZooLib {

class ZSeq_Yad;
class ZMap_Yad;

typedef ZVal_T<ZMap_Yad,ZSeq_Yad> ZVal_Yad;

// =================================================================================================
// MARK: - ZSeq_Yad

class ZSeq_Yad
	{
public:
	typedef ZVal_Yad Val;

	ZSeq_Yad();
	ZSeq_Yad(const ZSeq_Yad& iOther);
	~ZSeq_Yad();
	ZSeq_Yad& operator=(const ZSeq_Yad& iOther);

	ZSeq_Yad(const ZRef<ZYadSeqAtRPos>& iYad);
	ZSeq_Yad& operator=(const ZRef<ZYadSeqAtRPos>& iYad);

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

	ZSeq_Yad& Set(size_t iIndex, const ZVal_Yad& iVal);

	ZSeq_Yad& Erase(size_t iIndex);

	ZSeq_Yad& Insert(size_t iIndex, const ZVal_Yad& iVal);

	ZSeq_Yad& Append(const ZVal_Yad& iVal);

// Our protocol
//	ZVal_Yad& Mutable(size_t iIndex);
//	ZVal_Yad& operator[](size_t iIndex);
	ZVal_Yad operator[](size_t iIndex) const;

	ZRef<ZYadSeqAtRPos> GetYad() const;
	ZSeq_Any GetSeq() const;

private:
	void pGenSeq();

	mutable ZRef<ZYadSeqAtRPos> fYad;
	mutable ZSeq_Any fSeq;
	};

// =================================================================================================
// MARK: - ZMap_Yad

class ZMap_Yad
	{
public:
	typedef ZVal_Yad Val;

	ZMap_Yad();
	ZMap_Yad(const ZMap_Yad& iOther);
	~ZMap_Yad();
	ZMap_Yad& operator=(const ZMap_Yad& iOther);

	ZMap_Yad(const ZRef<ZYadMapAtRPos>& iYad);
	ZMap_Yad& operator=(const ZRef<ZYadMapAtRPos>& iYad);

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

	ZMap_Yad& Set(const string8& iName, const ZVal_Yad& iVal);

	template <class S>
	ZMap_Yad& Set(const string8& iName, const S& iVal)
		{ return this->Set(iName, ZVal_Yad(iVal)); }

	ZMap_Yad& Erase(const string8& iName);

// Our protocol
	ZVal_Yad& Mutable(const string8& iName);
	ZVal_Yad& operator[](const string8& iName);
	const ZVal_Yad& operator[](const string8& iName) const;

	ZRef<ZYadMapAtRPos> GetYad() const;
	ZMap_Any GetMap() const;

private:
	mutable ZRef<ZYadMapAtRPos> fYad;
	mutable ZMap_Any fMap;
	};

// =================================================================================================
// MARK: - sYadR

ZRef<ZYadR> sYadR(const ZVal_Yad& iVal);

ZRef<ZYadSeqAtRPos> sYadR(const ZSeq_Yad& iSeq);

ZRef<ZYadMapAtRPos> sYadR(const ZMap_Yad& iMap);

} // namespace ZooLib

#endif // __ZVal_Yad_h__
