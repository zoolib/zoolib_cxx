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

#if 0
//#ifndef __ZVal_Yad_h__
#define __ZVal_Yad_h__ 1
#include "zconfig.h"

#include "zoolib/Val_Any.h" // For Seq_Any and Map_Any
//#include "zoolib/ZValAccessors.h"
#include "zoolib/Yad.h"

#include <string>

namespace ZooLib {

class ZSeq_Yad;
class ZMap_Yad;

typedef Val_T<ZMap_Yad,ZSeq_Yad> ZVal_Yad;

// =================================================================================================
#pragma mark -
#pragma mark ZSeq_Yad

class ZSeq_Yad
	{
public:
	typedef ZVal_Yad Val_t;

	ZSeq_Yad();
	ZSeq_Yad(const ZSeq_Yad& iOther);
	~ZSeq_Yad();
	ZSeq_Yad& operator=(const ZSeq_Yad& iOther);

	ZSeq_Yad(const ZRef<ZYadSeqAtRPos>& iYad);
	ZSeq_Yad& operator=(const ZRef<ZYadSeqAtRPos>& iYad);

// ZSeq protocol
	size_t Count() const;

	void Clear();

	const ZVal_Yad* PGet(size_t iIndex) const;
	ZQ<ZVal_Yad> QGet(size_t iIndex) const;
	const ZVal_Yad& DGet(const ZVal_Yad& iDefault, size_t iIndex) const;
	const ZVal_Yad& Get(size_t iIndex) const;
	ZVal_Yad* PMut(size_t iIndex);
	ZVal_Yad& Mut(size_t iIndex);

	template <class S>
	const S* PGet(size_t iIndex) const
		{
		if (const ZVal_Yad* theVal = this->PGet(iIndex))
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
		if (ZVal_Yad* theVal = this->PMut(iIndex))
			return theVal->PMut<S>();
		return nullptr;
		}

	template <class S>
	S& Mut(size_t iIndex)
		{ return this->Mut(iIndex).Mut<S>(); }

	ZSeq_Yad& Set(size_t iIndex, const ZVal_Yad& iVal);

	ZSeq_Yad& Erase(size_t iIndex);

	ZSeq_Yad& Insert(size_t iIndex, const ZVal_Yad& iVal);

	ZSeq_Yad& Append(const ZVal_Yad& iVal);

// Our protocol
	ZVal_Yad& operator[](size_t iIndex);
	const ZVal_Yad& operator[](size_t iIndex) const;

	ZRef<ZYadSeqAtRPos> GetYad() const;
	Seq_Any GetSeq() const;

private:
	void pGenSeq();

	mutable ZRef<ZYadSeqAtRPos> fYad;
	mutable Seq_Any fSeq;
	};

// =================================================================================================
#pragma mark -
#pragma mark ZMap_Yad

class ZMap_Yad
	{
public:
	typedef ZVal_Yad Val_t;
	typedef Map_Any::Name_t Name_t;

	ZMap_Yad();
	ZMap_Yad(const ZMap_Yad& iOther);
	~ZMap_Yad();
	ZMap_Yad& operator=(const ZMap_Yad& iOther);

	ZMap_Yad(const ZRef<ZYadMapAtRPos>& iYad);
	ZMap_Yad& operator=(const ZRef<ZYadMapAtRPos>& iYad);

// ZMap protocol
	void Clear();

	const ZVal_Yad* PGet(const Name_t& iName) const;
	ZQ<ZVal_Yad> QGet(const Name_t& iName) const;
	const ZVal_Yad& DGet(const ZVal_Yad& iDefault, const Name_t& iName) const;
	const ZVal_Yad& Get(const Name_t& iName) const;
	ZVal_Yad* PMut(const Name_t& iName);
	ZVal_Yad& Mut(const Name_t& iName);

	template <class S>
	const S* PGet(const Name_t& iName) const
		{
		if (const ZVal_Yad* theVal = this->PGet(iName))
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
		if (ZVal_Yad* theVal = this->PMut(iName))
			return theVal->PMut<S>();
		return nullptr;
		}

	template <class S>
	S& Mut(const Name_t& iName)
		{ return this->Mut(iName).Mut<S>(); }

	ZMap_Yad& Set(const Name_t& iName, const ZVal_Yad& iVal);

	template <class S>
	ZMap_Yad& Set(const Name_t& iName, const S& iVal)
		{ return this->Set(iName, ZVal_Yad(iVal)); }

	ZMap_Yad& Erase(const Name_t& iName);

// Our protocol
	ZVal_Yad& operator[](const Name_t& iName);
	const ZVal_Yad& operator[](const Name_t& iName) const;

	ZRef<ZYadMapAtRPos> GetYad() const;
	Map_Any GetMap() const;

private:
	mutable ZRef<ZYadMapAtRPos> fYad;
	mutable Map_Any fMap;
	};

// =================================================================================================
#pragma mark -
#pragma mark sYadR

ZRef<YadR> sYadR(const ZVal_Yad& iVal);

ZRef<ZYadSeqAtRPos> sYadR(const ZSeq_Yad& iSeq);

ZRef<ZYadMapAtRPos> sYadR(const ZMap_Yad& iMap);

} // namespace ZooLib

#endif // __ZVal_Yad_h__
