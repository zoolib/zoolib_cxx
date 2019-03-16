/* -------------------------------------------------------------------------------------------------
Copyright (c) 2019 Andrew Green
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

#include "zoolib/GameEngine/Val.h"

#include "zoolib/Trail.h"
#include "zoolib/Util_STL_map.h"

namespace ZooLib {
namespace GameEngine {

using std::map;
using std::string;
using std::vector;

using namespace Util_STL;

struct Tombstone_t {};

// =================================================================================================
#pragma mark - Link

Link::Link(const ZRef<CountedName>& iProtoName, const Map_Any& iMap)
:	fProtoName(iProtoName)
,	fMap(iMap)
	{}

Link::Link(const ZRef<Link>& iParent, const Map_Any& iMap)
:	fProtoName(iParent->fProtoName)
,	fParent(iParent)
,	fMap(iMap)
	{}

ZQ<Val> Link::QReadAt(const Name& iName)
	{
	if (ZRef<Link> theLink = sGet(fChildren, iName))
		return Map(theLink);//??

	if (const Val_Any* theVal = sPGet(fMap, iName))
		{
		if (const Map_Any* theMap = theVal->PGet<Map_Any>())
			{
			ZRef<Link> theLink = new Link(this, *theMap);
			sInsertMust(fChildren, iName, theLink);
			return Map(theLink);
			}
		else if (const Seq_Any* theSeq_Any = theVal->PGet<Seq_Any>())
			{
			return Seq(this, *theSeq_Any);
			}
		else
			{
			return Val(theVal->AsAny());
			}
		}

	if (not fCheckedProto())
		{
		if (const string8* theString = fMap.PGet<string8>(fProtoName->Get()))
			{
			const string& theTrailString = *theString;

			if (theTrailString.size())
				{
				// We have a non-empty proto trail.
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
					if (ZQ<Val> theValQ = cur->QReadAt(theTrail.At(index)))
						{
						if (const Map* theMapP = theValQ->PGet<Map>())
							{
							cur = theMapP->GetLink();
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

static const Val* spCoerceValPtr(const Any* iAny)
	{ return static_cast<const Val*>(static_cast<const Any*>(iAny)); }

static Val* spCoerceValPtr(Any* iAny)
	{ return static_cast<Val*>(static_cast<Any*>(iAny)); }

// =================================================================================================
#pragma mark - Seq

Seq::Seq()
	{}

Seq::Seq(const Seq& iOther)
:	fLink(iOther.fLink)
,	fSeq(iOther.fSeq)
	{}

Seq::~Seq()
	{}

Seq& Seq::operator=(const Seq& iOther)
	{
	fLink = iOther.fLink;
	fSeq = iOther.fSeq;
	return *this;
	}

Seq::Seq(const ZRef<Link>& iLink, const Seq_Any& iSeq)
:	fLink(iLink)
,	fSeq(iSeq)
	{}

size_t Seq::Size() const
	{ return fSeq.Count(); }

void Seq::Clear()
	{
	fLink.Clear();
	fSeq.Clear();
	}

const Val* Seq::PGet(size_t iIndex) const
	{
	if (Val_Any* theValP = fSeq.PMut(iIndex))
		{
		if (const Map_Any* theMap_Any = theValP->PGet<Map_Any>())
			{
			*theValP = Map(new Link(fLink, *theMap_Any));
			}
		else if (const Seq_Any* theSeq_Any = theValP->PGet<Seq_Any>())
			{
			*theValP = Seq(fLink, *theSeq_Any);
			}
		return spCoerceValPtr(theValP);
		}
	return nullptr;
	}

ZQ<Val> Seq::QGet(size_t iIndex) const
	{
	if (const Val* theVal = this->PGet(iIndex))
		return *theVal;
	return null;
	}

const Val& Seq::DGet(const Val& iDefault, size_t iIndex) const
	{
	if (const Val* theVal = this->PGet(iIndex))
		return *theVal;
	return iDefault;
	}

const Val& Seq::Get(size_t iIndex) const
	{
	if (const Val* theVal = this->PGet(iIndex))
		return *theVal;
	return sDefault<Val>();
	}

Val* Seq::PMut(size_t iIndex)
	{
	if (Val_Any* theValP = fSeq.PMut(iIndex))
		{
		if (Map_Any* theMap_Any = theValP->PMut<Map_Any>())
			{
			*theValP = Map(new Link(fLink, *theMap_Any));
			}
		else if (Seq_Any* theSeq_Any = theValP->PMut<Seq_Any>())
			{
			*theValP = Seq(fLink, *theSeq_Any);
			}
		return spCoerceValPtr(theValP);
		}
	return nullptr;
	}

Val& Seq::Mut(size_t iIndex)
	{
	Val* theP = this->PMut(iIndex);
	ZAssert(theP);
	return *theP;
	}

Seq& Seq::Set(size_t iIndex, const Val& iVal)
	{
	fSeq.Set(iIndex, iVal.AsAny());
	return *this;
	}

Seq& Seq::Erase(size_t iIndex)
	{
	fSeq.Erase(iIndex);
	return *this;
	}

Seq& Seq::Insert(size_t iIndex, const Val& iVal)
	{
	fSeq.Insert(iIndex, iVal.AsAny());
	return *this;
	}

Seq& Seq::Append(const Val& iVal)
	{
	fSeq.Append(iVal.AsAny());
	return *this;
	}

Val& Seq::operator[](size_t iIndex)
	{ return this->Mut(iIndex); }

const Val& Seq::operator[](size_t iIndex) const
	{
	if (const Val* theVal = this->PGet(iIndex))
		return *theVal;
	return sDefault<Val>();
	}

Seq_Any Seq::GetSeq() const
	{ return fSeq; }

// =================================================================================================
#pragma mark - Map

Map::Map()
	{}

Map::Map(const Map& iOther)
:	fLink(iOther.fLink)
,	fMap(iOther.fMap)
	{}

Map::~Map()
	{}

Map& Map::operator=(const Map& iOther)
	{
	fLink = iOther.fLink;
	fMap = iOther.fMap;
	return *this;
	}

Map::Map(const ZRef<Link>& iLink)
:	fLink(iLink)
	{}

void Map::Clear()
	{
	fLink.Clear();
	fMap.Clear();
	}

const Val* Map::PGet(const Name_t& iName) const
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
		if (ZQ<Val> theValQ = fLink->QReadAt(iName))
			{
			fMap.Set(iName, theValQ->AsAny());
			return spCoerceValPtr(fMap.PGet(iName));
			}
		}

	fMap.Set(iName, Tombstone_t());
	return nullptr;
	}

ZQ<Val> Map::QGet(const Name_t& iName) const
	{
	if (const Val* theVal = this->PGet(iName))
		return *theVal;
	return null;
	}

const Val& Map::DGet(const Val& iDefault, const Name_t& iName) const
	{
	if (const Val* theVal = this->PGet(iName))
		return *theVal;
	return iDefault;
	}

const Val& Map::Get(const Name_t& iName) const
	{
	if (const Val* theVal = this->PGet(iName))
		return *theVal;
	return sDefault<Val>();
	}

Val* Map::PMut(const Name_t& iName)
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
		if (ZQ<Val> theValQ = fLink->QReadAt(iName))
			{
			fMap.Set(iName, theValQ->AsAny());
			return spCoerceValPtr(fMap.PMut(iName));
			}
		}

	fMap.Set(iName, Tombstone_t());
	return nullptr;
	}

Val& Map::Mut(const Name_t& iName)
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
		if (ZQ<Val> theValQ = fLink->QReadAt(iName))
			theMutable = theValQ->AsAny();
		}

	return static_cast<Val&>(theMutable);
	}

Map& Map::Set(const Name_t& iName, const Val& iVal)
	{
	fMap.Set(iName, iVal.AsAny());
	return *this;
	}

Map& Map::Erase(const Name_t& iName)
	{
	fMap.Set(iName, Tombstone_t());
	return *this;
	}

Val& Map::operator[](const Name_t& iName)
	{ return this->Mut(iName); }

const Val& Map::operator[](const Name_t& iName) const
	{
	if (const Val* theVal = this->PGet(iName))
		return *theVal;
	return sDefault<Val>();
	}

ZRef<Link> Map::GetLink() const
	{ return fLink; }

Map_Any Map::GetMap() const
	{ return fMap; }

// =================================================================================================
// MARK: -

Map sYadTree(const Map_Any& iMap_Any, const std::string& iProtoName)
	{ return Map(new Link(new CountedName(iProtoName), iMap_Any)); }

Map sParameterizedYadTree(const Map& iBase,
	const std::string& iRootAugmentName, const Map& iRootAugment)
	{ return Map(iBase.GetLink()->WithRootAugment(iRootAugmentName, iRootAugment.GetLink())); }

} // namespace GameEngine
} // namespace ZooLib
