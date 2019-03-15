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

#ifndef __ZYadTree_h__
#define __ZYadTree_h__ 1
#include "zconfig.h"

#include "zoolib/CountedVal.h"
#include "zoolib/Val_Any.h"
#include "zoolib/Val_T.h"
#include "zoolib/ValueOnce.h"

namespace ZooLib {

typedef CountedVal<Name> CountedName;

class ZSeq_Yad;
class ZMap_Yad;

typedef Val_T<ZMap_Yad,ZSeq_Yad> ZVal_Yad;

// =================================================================================================
// MARK: - Link declaration

class Link
:	public ZCounted
	{
public:
// ctor that establishes a tree
	Link(const ZRef<CountedName>& iProtoName, const Map_Any& iMap);

// ctor used as we walk down a tree.
	Link(const ZRef<Link>& iParent, const Map_Any& iMap);

	ZQ<ZVal_Yad> QReadAt(const Name& iName);

	ZRef<Link> WithRootAugment(const std::string& iRootAugmentName, const ZRef<Link>& iRootAugment);

private:
	const ZRef<CountedName> fProtoName;
	const ZRef<Link> fParent;
	const Map_Any fMap;

	std::map<std::string,ZRef<Link>> fChildren;
	FalseOnce fCheckedProto;
	ZRef<Link> fProto;
	};

// =================================================================================================
#pragma mark - ZSeq_Yad

class ZSeq_Yad
	{
public:
	typedef ZVal_Yad Val_t;

	ZSeq_Yad();
	ZSeq_Yad(const ZSeq_Yad& iOther);
	~ZSeq_Yad();
	ZSeq_Yad& operator=(const ZSeq_Yad& iOther);

	ZSeq_Yad(const ZRef<Link>& iLink, const Seq_Any& iSeq);

// ZSeq protocol
	size_t Size() const;
	size_t Count() const
		{ return this->Size(); }

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

	Seq_Any GetSeq() const;

private:
	void pGenSeq();

	ZRef<Link> fLink;
	mutable Seq_Any fSeq;
	};

// =================================================================================================
#pragma mark - ZMap_Yad

class ZMap_Yad
	{
public:
	typedef ZVal_Yad Val_t;
	typedef Map_Any::Name_t Name_t;

	ZMap_Yad();
	ZMap_Yad(const ZMap_Yad& iOther);
	~ZMap_Yad();
	ZMap_Yad& operator=(const ZMap_Yad& iOther);

	explicit ZMap_Yad(const ZRef<Link>& iLink);
	explicit ZMap_Yad(const Map_Any& iMap);

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

	ZRef<Link> GetLink() const;
	Map_Any GetMap() const;

private:
	ZRef<Link> fLink;
	mutable Map_Any fMap;
	};

// =================================================================================================
#pragma mark - ZMap_Yad

ZMap_Yad sYadTree(const Map_Any& iMap_Any, const std::string& iProtoName);

ZMap_Yad sParameterizedYadTree(const ZMap_Yad& iBase,
	const std::string& iRootAugmentName, const ZMap_Yad& iRootAugment);

} // namespace ZooLib

#endif // __ZYadTree_h__
