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

static const Val* spCoerceValPtr(const Val_ZZ* iVal)
	{ return &iVal->As<Val>(); }

static Val* spCoerceValPtr(Val_ZZ* iVal)
	{ return &iVal->As<Val>(); }

static const Val_ZZ& spCoerce_Val_ZZ(const Val& iVal)
	{ return iVal.As<Val_ZZ>(); }

// =================================================================================================
#pragma mark - Link, declaration

class Link
:	public Counted
	{
public:
// ctor that establishes a tree
	Link(const ZP<CountedName>& iProtoName, const Map_ZZ& iMap_ZZ);

// ctor used as we walk down a tree.
	Link(const ZP<Link>& iParent, const Map_ZZ& iMap_ZZ);

	ZQ<Val_ZZ> QReadAt(const Name& iName);

	ZP<Link> WithRootAugment(const std::string& iRootAugmentName, const ZP<Link>& iRootAugment);

	ZP<Link> GetProto();

	void InsertNames(std::set<Map::Name_t>& ioNames) const;

private:
	void pCheckProto();

	const ZP<CountedName> fProtoName;
	const ZP<Link> fParent;
	const Map_ZZ fMap_ZZ;

	std::map<Name,ZP<Link>> fChildren;
	FalseOnce fCheckedProto;
	ZP<Link> fProto;
	};

// =================================================================================================
#pragma mark - Link, definition

Link::Link(const ZP<CountedName>& iProtoName, const Map_ZZ& iMap_ZZ)
:	fProtoName(iProtoName)
,	fMap_ZZ(iMap_ZZ)
	{}

Link::Link(const ZP<Link>& iParent, const Map_ZZ& iMap_ZZ)
:	fProtoName(iParent->fProtoName)
,	fParent(iParent)
,	fMap_ZZ(iMap_ZZ)
	{}

ZQ<Val_ZZ> Link::QReadAt(const Name& iName)
	{
	if (ZP<Link> theLink = sGet(fChildren, iName))
		return Map(theLink);

	if (const Val_ZZ* theP = sPGet(fMap_ZZ, iName))
		{
		if (const Map_ZZ* theMap = theP->PGet<Map_ZZ>())
			{
			ZP<Link> theLink = new Link(this, *theMap);
			sInsertMust(fChildren, iName, theLink);
			return Map(theLink);
			}
		else if (const Seq_ZZ* theSeq_ZZP = theP->PGet<Seq_ZZ>())
			{
			return Seq(this, *theSeq_ZZP);
			}
		else
			{
			return *theP;
			}
		}

	pCheckProto();
	if (fProto)
		return fProto->QReadAt(iName);

	return null;
	}

ZP<Link> Link::WithRootAugment(const string& iRootAugmentName, const ZP<Link>& iRootAugment)
	{
	if (fParent)
		{
		ZP<Link> newParent = fParent->WithRootAugment(iRootAugmentName, iRootAugment);
		ZP<Link> newSelf = new Link(newParent, fMap_ZZ);
		return newSelf;
		}

	ZP<Link> newSelf = new Link(fProtoName, fMap_ZZ);
	sInsertMust(newSelf->fChildren, iRootAugmentName, iRootAugment);
	return newSelf;
	}

ZP<Link> Link::GetProto()
	{
	this->pCheckProto();
	return fProto;
	}

void Link::InsertNames(std::set<Map::Name_t>& ioNames) const
	{
	for (auto ii = fMap_ZZ.cbegin(); ii != fMap_ZZ.cend(); ++ii)
		ioNames.insert(ii->first);
	for (auto ii = fChildren.cbegin(); ii != fChildren.cend(); ++ii)
		ioNames.insert(ii->first);
	}

void Link::pCheckProto()
	{
	if (fCheckedProto())
		return;

	if (const string8* theString = fMap_ZZ.PGet<string8>(fProtoName->Get()))
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
				else if (ZQ<Val_ZZ> theZZQ = cur->QReadAt(theTrail.At(index)))
					{
					if (const Map* theMapP = theZZQ->PGet<Map>())
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

// =================================================================================================
#pragma mark - Seq

Seq::Seq()
	{}

Seq::Seq(const Seq& iOther)
:	fLink(iOther.fLink)
,	fSeq_ZZ(iOther.fSeq_ZZ)
	{}

Seq::~Seq()
	{}

Seq& Seq::operator=(const Seq& iOther)
	{
	fLink = iOther.fLink;
	fSeq_ZZ = iOther.fSeq_ZZ;
	return *this;
	}

Seq::Seq(const ZP<Link>& iLink, const Seq_ZZ& iSeq)
:	fLink(iLink)
,	fSeq_ZZ(iSeq)
	{}

size_t Seq::Size() const
	{ return fSeq_ZZ.Count(); }

void Seq::Clear()
	{
	fLink.Clear();
	fSeq_ZZ.Clear();
	}

const Val* Seq::PGet(size_t iIndex) const
	{
	if (Val_ZZ* theP = fSeq_ZZ.PMut(iIndex))
		{
		if (const Map_ZZ* theMap_ZZP = theP->PGet<Map_ZZ>())
			{
			*theP = Map(new Link(fLink, *theMap_ZZP));
			}
		else if (const Seq_ZZ* theSeq_ZZP = theP->PGet<Seq_ZZ>())
			{
			*theP = Seq(fLink, *theSeq_ZZP);
			}
		return spCoerceValPtr(theP);
		}
	return nullptr;
	}

ZQ<Val> Seq::QGet(size_t iIndex) const
	{
	if (const Val* theVal = this->PGet(iIndex))
		return *theVal;
	return null;
	}

const Val& Seq::Get(size_t iIndex) const
	{
	if (const Val* theVal = this->PGet(iIndex))
		return *theVal;
	return sDefault<Val>();
	}

Val* Seq::PMut(size_t iIndex)
	{
	if (Val_ZZ* theP = fSeq_ZZ.PMut(iIndex))
		{
		if (Map_ZZ* theMap_ZZP = theP->PMut<Map_ZZ>())
			{
			*theP = Map(new Link(fLink, *theMap_ZZP));
			}
		else if (Seq_ZZ* theSeq_ZZP = theP->PMut<Seq_ZZ>())
			{
			*theP = Seq(fLink, *theSeq_ZZP);
			}
		return spCoerceValPtr(theP);
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
	fSeq_ZZ.Set(iIndex, spCoerce_Val_ZZ(iVal));
	return *this;
	}

Seq& Seq::Erase(size_t iIndex)
	{
	fSeq_ZZ.Erase(iIndex);
	return *this;
	}

Seq& Seq::Insert(size_t iIndex, const Val& iVal)
	{
	fSeq_ZZ.Insert(iIndex, spCoerce_Val_ZZ(iVal));
	return *this;
	}

Seq& Seq::Append(const Val& iVal)
	{
	fSeq_ZZ.Append(spCoerce_Val_ZZ(iVal));
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
,	fMap_ZZ(iOther.fMap_ZZ)
	{}

Map::~Map()
	{}

Map& Map::operator=(const Map& iOther)
	{
	fLink = iOther.fLink;
	fMap_ZZ = iOther.fMap_ZZ;
	return *this;
	}

Map::Map(const ZP<Link>& iLink)
:	fLink(iLink)
	{}

Map::Map(const Map& iParent, const Map_ZZ& iMap)
:	fLink(new Link(iParent.GetLink(), iMap))
	{}

void Map::Clear()
	{
	fLink.Clear();
	fMap_ZZ.Clear();
	}

const Val* Map::PGet(const Name_t& iName) const
	{
	Map_ZZ::Index_t theIndex = fMap_ZZ.IndexOf(iName);
	if (theIndex != fMap_ZZ.End())
		{
		if (const Val_ZZ* theZZ = fMap_ZZ.PGet(theIndex))
			{
			if (theZZ->PGet<Tombstone_t>())
				return nullptr;
			return spCoerceValPtr(theZZ);
			}
		}

	if (fLink)
		{
		if (ZQ<Val_ZZ> theZZQ = fLink->QReadAt(iName))
			{
			fMap_ZZ.Set(iName, *theZZQ);
			return spCoerceValPtr(fMap_ZZ.PGet(iName));
			}
		}

	fMap_ZZ.Set(iName, Tombstone_t());
	return nullptr;
	}

ZQ<Val> Map::QGet(const Name_t& iName) const
	{
	if (const Val* theVal = this->PGet(iName))
		return *theVal;
	return null;
	}

const Val& Map::Get(const Name_t& iName) const
	{
	if (const Val* theVal = this->PGet(iName))
		return *theVal;
	return sDefault<Val>();
	}

Val* Map::PMut(const Name_t& iName)
	{
	Map_ZZ::Index_t theIndex = fMap_ZZ.IndexOf(iName);
	if (theIndex != fMap_ZZ.End())
		{
		if (Val_ZZ* theZZ = fMap_ZZ.PMut(theIndex))
			{
			if (theZZ->PGet<Tombstone_t>())
				return nullptr;
			return spCoerceValPtr(theZZ);
			}
		}

	if (fLink)
		{
		if (ZQ<Val_ZZ> theZZQ = fLink->QReadAt(iName))
			{
			fMap_ZZ.Set(iName, *theZZQ);
			return spCoerceValPtr(fMap_ZZ.PMut(iName));
			}
		}

	fMap_ZZ.Set(iName, Tombstone_t());
	return nullptr;
	}

Val& Map::Mut(const Name_t& iName)
	{
	if (Val_ZZ* theP = fMap_ZZ.PMut(iName))
		{
		if (theP->PGet<Tombstone_t>())
			theP->Clear();
		return *spCoerceValPtr(theP);
		}

	Val_ZZ& theMutable = fMap_ZZ.Mut(iName);
	if (fLink)
		{
		if (ZQ<Val_ZZ> theZZQ = fLink->QReadAt(iName))
			theMutable = *theZZQ;
		}

	return theMutable.As<Val>();
	}

Map& Map::Set(const Name_t& iName, const Val& iVal)
	{
	fMap_ZZ.Set(iName, spCoerce_Val_ZZ(iVal));
	return *this;
	}

Map& Map::Erase(const Name_t& iName)
	{
	fMap_ZZ.Set(iName, Tombstone_t());
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

void Map::InsertNames(std::set<Map::Name_t>& ioNames) const
	{
	for (ZP<Link> cur = fLink; cur; cur = cur->GetProto())
		cur->InsertNames(ioNames);

	for (auto ii = fMap_ZZ.cbegin(); ii != fMap_ZZ.cend(); ++ii)
		{
		if (ii->second.PGet<Tombstone_t>())
			ioNames.erase(ii->first);
		else
			ioNames.insert(ii->first);
		}
	}

// =================================================================================================
#pragma mark -

Map sYadTree(const Map_ZZ& iMap_ZZ, const std::string& iProtoName)
	{ return Map(new Link(new CountedName(iProtoName), iMap_ZZ)); }

Map sParameterizedYadTree(const Map& iBase,
	const std::string& iRootAugmentName, const Map& iRootAugment)
	{ return Map(iBase.GetLink()->WithRootAugment(iRootAugmentName, iRootAugment.GetLink())); }

std::set<Map::Name_t> sAllNamesIn(Map iMap)
	{
	std::set<Map::Name_t> result;
	iMap.InsertNames(result);
	return result;
	}

Val_ZZ sAsZZ(const Val& iVal)
	{
	if (const Map* asMap = iVal.PGet<Map>())
		{
		Map_ZZ result;
		for (Name theName: sAllNamesIn(*asMap))
			result.Set(theName, sAsZZ(asMap->Get(theName)));
		return result;
		}
	else if (const Seq* asSeq = iVal.PGet<Seq>())
		{
		Seq_ZZ result;
		for (size_t xx = 0; xx < asSeq->Count(); ++xx)
			result.Append(sAsZZ(asSeq->Get(xx)));
		return result;
		}
	else
		{
		return iVal.As<Val_ZZ>();
		}
	}

} // namespace GameEngine
} // namespace ZooLib
