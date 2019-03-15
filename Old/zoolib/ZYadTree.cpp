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

#include "zoolib/ZYadTree.h"

#include "zoolib/Trail.h"
#include "zoolib/Util_STL_map.h"
#include "zoolib/ValueOnce.h"

using std::map;
using std::string;
using std::vector;

namespace ZooLib {

struct Tombstone_t {};

using namespace Util_STL;

// =================================================================================================
// MARK: - Link definition

Link::Link(const ZRef<CountedName>& iProtoName, const Map_Any& iMap)
:	fProtoName(iProtoName)
,	fMap(iMap)
	{}

Link::Link(const ZRef<Link>& iParent, const Map_Any& iMap)
:	fProtoName(iParent->fProtoName)
,	fParent(iParent)
,	fMap(iMap)
	{}

ZQ<ZVal_Yad> Link::QReadAt(const Name& iName)
	{
	if (ZRef<Link> theLink = sGet(fChildren, iName))
		return ZMap_Yad(theLink);//??

	if (const Val_Any* theVal = sPGet(fMap, iName))
		{
		if (const Map_Any* theMap = theVal->PGet<Map_Any>())
			{
			ZRef<Link> theLink = new Link(this, *theMap);
			sInsertMust(fChildren, iName, theLink);
			return ZMap_Yad(theLink);
			}
		else if (const Seq_Any* theSeq_Any = theVal->PGet<Seq_Any>())
			{
			return ZSeq_Yad(this, *theSeq_Any);
			}
		else
			{
			return ZVal_Yad(theVal->AsAny());
			}
		}

	if (not fCheckedProto())
		{
		if (const string8* theString = fMap.PGet<string8>(fProtoName->Get()))
			{
			const string& theTrailString = *theString;

			if (theTrailString.size())
				{
				// Our Yad has a non-empty proto trail.
				size_t index = 0;
				const Trail theTrail = Trail(theTrailString).Normalized();

				ZRef<Link> cur = this;

				if (theTrailString[0] == '/')
					{
					// Walk up to the root.
					for (ZRef<Link> next = null; (next = cur->fParent); cur = next)
						{}
					}
				else
					{
					// Follow any leading bounces.
					for (/*no init*/;
						cur && index < theTrail.Count() && sIsEmpty(theTrail.At(index));
						++index, cur = cur->fParent)
						{}
					}

				// Walk down the tree.
				for (;;)
					{
					if (ZQ<ZVal_Yad> theValQ = cur->QReadAt(theTrail.At(index)))
						{
						if (const ZMap_Yad* theMap_Yad = theValQ->PGet<ZMap_Yad>())
							{
							cur = theMap_Yad->GetLink();
							if (++index == theTrail.Count())
								{
								fProto = cur;
								break;
								}
							continue;
							}
						}
					break;
					}
				}
			}
		}

	if (fProto)
		return fProto->QReadAt(iName);

	return null;
	}

ZRef<Link> Link::WithRootAugment(const string& iRootAugmentName, const ZRef<Link>& iRootAugment)
	{
	if (fParent)
		{
		ZRef<Link> newParent = fParent->WithRootAugment(iRootAugmentName, iRootAugment);
		ZRef<Link> newSelf = new Link(newParent, fMap);
		return newSelf;
		}

	ZRef<Link> newSelf = new Link(fProtoName, fMap);
	sInsertMust(newSelf->fChildren, iRootAugmentName, iRootAugment);
	return newSelf;
	}

// =================================================================================================
#pragma mark - spCoerceValPtr

static const ZVal_Yad* spCoerceValPtr(const Any* iAny)
	{ return static_cast<const ZVal_Yad*>(static_cast<const Any*>(iAny)); }

static ZVal_Yad* spCoerceValPtr(Any* iAny)
	{ return static_cast<ZVal_Yad*>(static_cast<Any*>(iAny)); }

// =================================================================================================
#pragma mark - ZSeq_Yad

ZSeq_Yad::ZSeq_Yad()
	{}

ZSeq_Yad::ZSeq_Yad(const ZSeq_Yad& iOther)
:	fLink(iOther.fLink)
,	fSeq(iOther.fSeq)
	{}

ZSeq_Yad::~ZSeq_Yad()
	{}

ZSeq_Yad& ZSeq_Yad::operator=(const ZSeq_Yad& iOther)
	{
	fLink = iOther.fLink;
	fSeq = iOther.fSeq;
	return *this;
	}

ZSeq_Yad::ZSeq_Yad(const ZRef<Link>& iLink, const Seq_Any& iSeq)
:	fLink(iLink)
,	fSeq(iSeq)
	{}

size_t ZSeq_Yad::Size() const
	{ return fSeq.Count(); }

void ZSeq_Yad::Clear()
	{
	fLink.Clear();
	fSeq.Clear();
	}

const ZVal_Yad* ZSeq_Yad::PGet(size_t iIndex) const
	{
	if (Val_Any* theValP = fSeq.PMut(iIndex))
		{
		if (const Map_Any* theMap_Any = theValP->PGet<Map_Any>())
			{
			*theValP = ZMap_Yad(new Link(fLink, *theMap_Any));
			}
		else if (const Seq_Any* theSeq_Any = theValP->PGet<Seq_Any>())
			{
			*theValP = ZSeq_Yad(fLink, *theSeq_Any);
			}
		return spCoerceValPtr(theValP);
		}
	return nullptr;
	}

ZQ<ZVal_Yad> ZSeq_Yad::QGet(size_t iIndex) const
	{
	if (const ZVal_Yad* theVal = this->PGet(iIndex))
		return *theVal;
	return null;
	}

const ZVal_Yad& ZSeq_Yad::DGet(const ZVal_Yad& iDefault, size_t iIndex) const
	{
	if (const ZVal_Yad* theVal = this->PGet(iIndex))
		return *theVal;
	return iDefault;
	}

const ZVal_Yad& ZSeq_Yad::Get(size_t iIndex) const
	{
	if (const ZVal_Yad* theVal = this->PGet(iIndex))
		return *theVal;
	return sDefault<ZVal_Yad>();
	}

ZVal_Yad* ZSeq_Yad::PMut(size_t iIndex)
	{
	if (Val_Any* theValP = fSeq.PMut(iIndex))
		{
		if (Map_Any* theMap_Any = theValP->PMut<Map_Any>())
			{
			*theValP = ZMap_Yad(new Link(fLink, *theMap_Any));
			}
		else if (Seq_Any* theSeq_Any = theValP->PMut<Seq_Any>())
			{
			*theValP = ZSeq_Yad(fLink, *theSeq_Any);
			}
		return spCoerceValPtr(theValP);
		}
	return nullptr;
	}

ZVal_Yad& ZSeq_Yad::Mut(size_t iIndex)
	{
	ZVal_Yad* theP = this->PMut(iIndex);
	ZAssert(theP);
	return *theP;
	}

ZSeq_Yad& ZSeq_Yad::Set(size_t iIndex, const ZVal_Yad& iVal)
	{
	fSeq.Set(iIndex, iVal.AsAny());
	return *this;
	}

ZSeq_Yad& ZSeq_Yad::Erase(size_t iIndex)
	{
	fSeq.Erase(iIndex);
	return *this;
	}

ZSeq_Yad& ZSeq_Yad::Insert(size_t iIndex, const ZVal_Yad& iVal)
	{
	fSeq.Insert(iIndex, iVal.AsAny());
	return *this;
	}

ZSeq_Yad& ZSeq_Yad::Append(const ZVal_Yad& iVal)
	{
	fSeq.Append(iVal.AsAny());
	return *this;
	}

ZVal_Yad& ZSeq_Yad::operator[](size_t iIndex)
	{ return this->Mut(iIndex); }

const ZVal_Yad& ZSeq_Yad::operator[](size_t iIndex) const
	{
	if (const ZVal_Yad* theVal = this->PGet(iIndex))
		return *theVal;
	return sDefault<ZVal_Yad>();
	}

Seq_Any ZSeq_Yad::GetSeq() const
	{ return fSeq; }

// =================================================================================================
#pragma mark - ZMap_Yad

ZMap_Yad::ZMap_Yad()
	{}

ZMap_Yad::ZMap_Yad(const ZMap_Yad& iOther)
:	fLink(iOther.fLink)
,	fMap(iOther.fMap)
	{}

ZMap_Yad::~ZMap_Yad()
	{}

ZMap_Yad& ZMap_Yad::operator=(const ZMap_Yad& iOther)
	{
	fLink = iOther.fLink;
	fMap = iOther.fMap;
	return *this;
	}

ZMap_Yad::ZMap_Yad(const ZRef<Link>& iLink)
:	fLink(iLink)
	{}

void ZMap_Yad::Clear()
	{
	fLink.Clear();
	fMap.Clear();
	}

const ZVal_Yad* ZMap_Yad::PGet(const Name_t& iName) const
	{
	Map_Any::Index_t theIndex = fMap.IndexOf(iName);
	if (theIndex != fMap.End())
		{
		if (const Any* theAny = fMap.PGet(theIndex))
			{
			if (theAny->PGet<Tombstone_t>())
				return nullptr;
			return spCoerceValPtr(theAny);
			}
		}

	if (fLink)
		{
		if (ZQ<ZVal_Yad> theValQ = fLink->QReadAt(iName))
			{
			fMap.Set(iName, theValQ->AsAny());
			return spCoerceValPtr(fMap.PGet(iName));
			}
		}

	fMap.Set(iName, Tombstone_t());
	return nullptr;
	}

ZQ<ZVal_Yad> ZMap_Yad::QGet(const Name_t& iName) const
	{
	if (const ZVal_Yad* theVal = this->PGet(iName))
		return *theVal;
	return null;
	}

const ZVal_Yad& ZMap_Yad::DGet(const ZVal_Yad& iDefault, const Name_t& iName) const
	{
	if (const ZVal_Yad* theVal = this->PGet(iName))
		return *theVal;
	return iDefault;
	}

const ZVal_Yad& ZMap_Yad::Get(const Name_t& iName) const
	{
	if (const ZVal_Yad* theVal = this->PGet(iName))
		return *theVal;
	return sDefault<ZVal_Yad>();
	}

ZVal_Yad* ZMap_Yad::PMut(const Name_t& iName)
	{
	Map_Any::Index_t theIndex = fMap.IndexOf(iName);
	if (theIndex != fMap.End())
		{
		if (Any* theAny = fMap.PMut(theIndex))
			{
			if (theAny->PGet<Tombstone_t>())
				return nullptr;
			return spCoerceValPtr(theAny);
			}
		}

	if (fLink)
		{
		if (ZQ<ZVal_Yad> theValQ = fLink->QReadAt(iName))
			{
			fMap.Set(iName, theValQ->AsAny());
			return spCoerceValPtr(fMap.PMut(iName));
			}
		}

	fMap.Set(iName, Tombstone_t());
	return nullptr;
	}

ZVal_Yad& ZMap_Yad::Mut(const Name_t& iName)
	{
	if (Any* theP = fMap.PMut(iName))
		{
		if (theP->PGet<Tombstone_t>())
			*theP = Any();
		return *spCoerceValPtr(theP);
		}

	Any& theMutable = fMap.Mut(iName);
	if (fLink)
		{
		if (ZQ<ZVal_Yad> theValQ = fLink->QReadAt(iName))
			theMutable = theValQ->AsAny();
		}

	return static_cast<ZVal_Yad&>(theMutable);
	}

ZMap_Yad& ZMap_Yad::Set(const Name_t& iName, const ZVal_Yad& iVal)
	{
	fMap.Set(iName, iVal.AsAny());
	return *this;
	}

ZMap_Yad& ZMap_Yad::Erase(const Name_t& iName)
	{
	fMap.Set(iName, Tombstone_t());
	return *this;
	}

ZVal_Yad& ZMap_Yad::operator[](const Name_t& iName)
	{ return this->Mut(iName); }

const ZVal_Yad& ZMap_Yad::operator[](const Name_t& iName) const
	{
	if (const ZVal_Yad* theVal = this->PGet(iName))
		return *theVal;
	return sDefault<ZVal_Yad>();
	}

ZRef<Link> ZMap_Yad::GetLink() const
	{ return fLink; }

Map_Any ZMap_Yad::GetMap() const
	{ return fMap; }

// =================================================================================================
// MARK: -

ZMap_Yad sYadTree(const Map_Any& iMap_Any, const std::string& iProtoName)
	{ return ZMap_Yad(new Link(new CountedName(iProtoName), iMap_Any)); }

ZMap_Yad sParameterizedYadTree(const ZMap_Yad& iBase,
	const std::string& iRootAugmentName, const ZMap_Yad& iRootAugment)
	{ return ZMap_Yad(iBase.GetLink()->WithRootAugment(iRootAugmentName, iRootAugment.GetLink())); }

} // namespace ZooLib
