// Copyright (c) 2019 Andrew Green. MIT License. http://www.zoolib.org

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
#pragma mark - spCoerceXXX

static const Val* spCoerceValPtr(const Any* iAny)
	{ return static_cast<const Val*>(iAny); }

static Val* spCoerceValPtr(Any* iAny)
	{ return static_cast<Val*>(iAny); }

static const Val_Any& spCoerce_Val_Any(const Val& iVal)
	{ return static_cast<const Val_Any&>(static_cast<const Any&>(iVal)); }

// =================================================================================================
#pragma mark - Link, declaration

class Link
:	public Counted
	{
public:
// ctor that establishes a tree
	Link(const ZP<CountedName>& iProtoName, const Map_Any& iMap);

// ctor used as we walk down a tree.
	Link(const ZP<Link>& iParent, const Map_Any& iMap);

	ZQ<Any> QReadAt(const Name& iName);

	ZP<Link> WithRootAugment(const std::string& iRootAugmentName, const ZP<Link>& iRootAugment);

private:
	const ZP<CountedName> fProtoName;
	const ZP<Link> fParent;
	const Map_Any fMap;

	std::map<std::string,ZP<Link>> fChildren;
	FalseOnce fCheckedProto;
	ZP<Link> fProto;
	};

// =================================================================================================
#pragma mark - Link, definition

Link::Link(const ZP<CountedName>& iProtoName, const Map_Any& iMap)
:	fProtoName(iProtoName)
,	fMap(iMap)
	{}

Link::Link(const ZP<Link>& iParent, const Map_Any& iMap)
:	fProtoName(iParent->fProtoName)
,	fParent(iParent)
,	fMap(iMap)
	{}

ZQ<Any> Link::QReadAt(const Name& iName)
	{
	if (ZP<Link> theLink = sGet(fChildren, iName))
		return Map(theLink);//??

	if (const Any* theAnyP = sPGet(fMap, iName))
		{
		if (const Map_Any* theMap = theAnyP->PGet<Map_Any>())
			{
			ZP<Link> theLink = new Link(this, *theMap);
			sInsertMust(fChildren, iName, theLink);
			return Map(theLink);
			}
		else if (const Seq_Any* theSeq_AnyP = theAnyP->PGet<Seq_Any>())
			{
			return Seq(this, *theSeq_AnyP);
			}
		else
			{
			return *theAnyP;
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

				ZP<Link> cur = this;

				if (theTrailString[0] == '/')
					{
					// Walk up to the root.
					for (ZP<Link> next = null; (next = cur->fParent); cur = next)
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

				for (;;++index)
					{
					if (index == theTrail.Count())
						{
						fProto = cur;
						break;
						}
					else if (ZQ<Any> theAnyQ = cur->QReadAt(theTrail.At(index)))
						{
						if (const Map* theMapP = theAnyQ->PGet<Map>())
							{
							cur = theMapP->GetLink();
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

ZP<Link> Link::WithRootAugment(const string& iRootAugmentName, const ZP<Link>& iRootAugment)
	{
	if (fParent)
		{
		ZP<Link> newParent = fParent->WithRootAugment(iRootAugmentName, iRootAugment);
		ZP<Link> newSelf = new Link(newParent, fMap);
		return newSelf;
		}

	ZP<Link> newSelf = new Link(fProtoName, fMap);
	sInsertMust(newSelf->fChildren, iRootAugmentName, iRootAugment);
	return newSelf;
	}

// =================================================================================================
#pragma mark - Seq

Seq::Seq()
	{}

Seq::Seq(const Seq& iOther)
:	fLink(iOther.fLink)
,	fSeq_Any(iOther.fSeq_Any)
	{}

Seq::~Seq()
	{}

Seq& Seq::operator=(const Seq& iOther)
	{
	fLink = iOther.fLink;
	fSeq_Any = iOther.fSeq_Any;
	return *this;
	}

Seq::Seq(const ZP<Link>& iLink, const Seq_Any& iSeq)
:	fLink(iLink)
,	fSeq_Any(iSeq)
	{}

size_t Seq::Size() const
	{ return fSeq_Any.Count(); }

void Seq::Clear()
	{
	fLink.Clear();
	fSeq_Any.Clear();
	}

const Val* Seq::PGet(size_t iIndex) const
	{
	if (Any* theAnyP = fSeq_Any.PMut(iIndex))
		{
		if (const Map_Any* theMap_AnyP = theAnyP->PGet<Map_Any>())
			{
			*theAnyP = Map(new Link(fLink, *theMap_AnyP));
			}
		else if (const Seq_Any* theSeq_AnyP = theAnyP->PGet<Seq_Any>())
			{
			*theAnyP = Seq(fLink, *theSeq_AnyP);
			}
		return spCoerceValPtr(theAnyP);
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
	if (Any* theAnyP = fSeq_Any.PMut(iIndex))
		{
		if (Map_Any* theMap_AnyP = theAnyP->PMut<Map_Any>())
			{
			*theAnyP = Map(new Link(fLink, *theMap_AnyP));
			}
		else if (Seq_Any* theSeq_AnyP = theAnyP->PMut<Seq_Any>())
			{
			*theAnyP = Seq(fLink, *theSeq_AnyP);
			}
		return spCoerceValPtr(theAnyP);
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
	fSeq_Any.Set(iIndex, spCoerce_Val_Any(iVal));
	return *this;
	}

Seq& Seq::Erase(size_t iIndex)
	{
	fSeq_Any.Erase(iIndex);
	return *this;
	}

Seq& Seq::Insert(size_t iIndex, const Val& iVal)
	{
	fSeq_Any.Insert(iIndex, spCoerce_Val_Any(iVal));
	return *this;
	}

Seq& Seq::Append(const Val& iVal)
	{
	fSeq_Any.Append(spCoerce_Val_Any(iVal));
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

// =================================================================================================
#pragma mark - Map

Map::Map()
	{}

Map::Map(const Map& iOther)
:	fLink(iOther.fLink)
,	fMap_Any(iOther.fMap_Any)
	{}

Map::~Map()
	{}

Map& Map::operator=(const Map& iOther)
	{
	fLink = iOther.fLink;
	fMap_Any = iOther.fMap_Any;
	return *this;
	}

Map::Map(const ZP<Link>& iLink)
:	fLink(iLink)
	{}

Map::Map(const Map& iParent, const Map_Any& iMap)
:	fLink(new Link(iParent.GetLink(), iMap))
	{}

void Map::Clear()
	{
	fLink.Clear();
	fMap_Any.Clear();
	}

const Val* Map::PGet(const Name_t& iName) const
	{
	Map_Any::Index_t theIndex = fMap_Any.IndexOf(iName);
	if (theIndex != fMap_Any.End())
		{
		if (const Any* theAny = fMap_Any.PGet(theIndex))
			{
			if (theAny->PGet<Tombstone_t>())
				return nullptr;
			return spCoerceValPtr(theAny);
			}
		}

	if (fLink)
		{
		if (ZQ<Any> theAnyQ = fLink->QReadAt(iName))
			{
			fMap_Any.Set(iName, *theAnyQ);
			return spCoerceValPtr(fMap_Any.PGet(iName));
			}
		}

	fMap_Any.Set(iName, Tombstone_t());
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
	Map_Any::Index_t theIndex = fMap_Any.IndexOf(iName);
	if (theIndex != fMap_Any.End())
		{
		if (Any* theAny = fMap_Any.PMut(theIndex))
			{
			if (theAny->PGet<Tombstone_t>())
				return nullptr;
			return spCoerceValPtr(theAny);
			}
		}

	if (fLink)
		{
		if (ZQ<Any> theAnyQ = fLink->QReadAt(iName))
			{
			fMap_Any.Set(iName, *theAnyQ);
			return spCoerceValPtr(fMap_Any.PMut(iName));
			}
		}

	fMap_Any.Set(iName, Tombstone_t());
	return nullptr;
	}

Val& Map::Mut(const Name_t& iName)
	{
	if (Any* theP = fMap_Any.PMut(iName))
		{
		if (theP->PGet<Tombstone_t>())
			*theP = Any();
		return *spCoerceValPtr(theP);
		}

	Any& theMutable = fMap_Any.Mut(iName);
	if (fLink)
		{
		if (ZQ<Any> theAnyQ = fLink->QReadAt(iName))
			theMutable = *theAnyQ;
		}

	return static_cast<Val&>(theMutable);
	}

Map& Map::Set(const Name_t& iName, const Val& iVal)
	{
	fMap_Any.Set(iName, spCoerce_Val_Any(iVal));
	return *this;
	}

Map& Map::Erase(const Name_t& iName)
	{
	fMap_Any.Set(iName, Tombstone_t());
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

ZP<Link> Map::GetLink() const
	{ return fLink; }

//Map_Any Map::GetMap() const
//	{ return fMap; }

// =================================================================================================
#pragma mark -

Map sYadTree(const Map_Any& iMap_Any, const std::string& iProtoName)
	{ return Map(new Link(new CountedName(iProtoName), iMap_Any)); }

Map sParameterizedYadTree(const Map& iBase,
	const std::string& iRootAugmentName, const Map& iRootAugment)
	{ return Map(iBase.GetLink()->WithRootAugment(iRootAugmentName, iRootAugment.GetLink())); }

} // namespace GameEngine
} // namespace ZooLib
