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

#include "zoolib/CountedVal.h"
#include "zoolib/Log.h"
#include "zoolib/Trail.h"
#include "zoolib/Util_STL_map.h"
#include "zoolib/ValueOnce.h"

//#include "zoolib/Visitor_Do_T.h"
#include "zoolib/ZYadTree.h"

using std::map;
using std::string;
using std::vector;

namespace ZooLib {
namespace YadTree {
namespace { // anonymous

using namespace Util_STL;

typedef CountedVal<Name> CountedString;

class Link;

// =================================================================================================
// MARK: - Helpers

static ZRef<YadR> spWrap(const ZRef<Link>& iLink, const ZRef<YadR>& iYad);

// =================================================================================================
// MARK: - Link declaration

class Link
:	public ZCounted
	{
public:
// ctor that establishes a tree
	Link(const ZRef<CountedString>& iProtoName, const ZRef<YadMapAtR>& iYadMapAtR);

// ctor used as we walk down a tree.
	Link(const ZRef<Link>& iParent, const ZRef<YadMapAtR>& iYadMapAtR);

	ZRef<YadMapAtR> GetYadMapAtR();

	ZRef<YadR> ReadAt(const Name& iName);

	ZRef<Link> WithRootAugment(const string& iRootAugmentName, const ZRef<Link>& iRootAugment);

private:
	const ZRef<CountedString> fProtoName;
	const ZRef<Link> fParent;
	const ZRef<YadMapAtR> fYadMapAtR;
	map<string,ZRef<Link> > fChildren;
	FalseOnce fCheckedProto;
	ZRef<Link> fProto;
	};

// =================================================================================================
// MARK: - ChanR_RefYad_WithLink

class ChanR_RefYad_WithLink
:	public ChanR_RefYad
	{
public:
	ChanR_RefYad_WithLink(const ZRef<Link>& iLink, ZRef<YadSeqR> iYadSeqR)
	:	fLink(iLink)
	,	fYadSeqR(iYadSeqR)
		{}

// From ChanR_RefYad
	virtual size_t Read(RefYad* oDest, size_t iCount)
		{
		if (iCount)
			{
			if (ZRef<YadR> theYadR = sReadInc(fYadSeqR))
				{
				*oDest = spWrap(fLink, theYadR);
				return 1;
				}
			}
		return 0;
		}

private:
	const ZRef<Link> fLink;
	ZRef<YadSeqR> fYadSeqR;
	};

// =================================================================================================
// MARK: - ChanAtR_RefYad_WithLink

class ChanAtR_RefYad_WithLink
:	public ChanAtR_RefYad
	{
public:
	ChanAtR_RefYad_WithLink(const ZRef<Link>& iLink, ZRef<YadSeqAtR> iYadSeqAtR)
	:	fLink(iLink)
	,	fYadSeqAtR(iYadSeqAtR)
	,	fChildren(sSize(*fYadSeqAtR), null)
		{}

// From ChanAtR_RefYad
	virtual size_t ReadAt(const uint64& iLoc, RefYad* oDest, size_t iCount)
		{
		if (iCount)
			{
			if (iLoc < fChildren.size())
				{
				ZRef<YadR> theYad = fChildren[iLoc];
				if (not theYad)
					{
					theYad = sReadAt(*fYadSeqAtR, iLoc);
					if (theYad)
						{
						theYad = spWrap(fLink, theYad);
						fChildren[iLoc] = theYad;
						}
					}
				if (theYad)
					{
					*oDest = theYad;
					return 1;
					}
				}
			}
		return 0;
		}

	virtual uint64 Size()
		{ return fChildren.size(); }

private:
	const ZRef<Link> fLink;
	ZRef<YadSeqAtR> fYadSeqAtR;
	vector<ZRef<YadR> > fChildren;
	};

// =================================================================================================
// MARK: - YadMapAtR_WithLink

class ChanAtR_NameRefYad_WithLink
:	public ChanAtR_NameRefYad
	{
public:
	ChanAtR_NameRefYad_WithLink(const ZRef<Link>& iLink)
	:	fLink(iLink)
		{}

// From ChanAtR_NameRefYad
	virtual size_t ReadAt(const Name& iLoc, RefYad* oDest, size_t iCount)
		{
		if (iCount)
			{
			if (ZRef<YadR> theYad = fLink->ReadAt(iLoc))
				{
				*oDest = theYad;
				return 1;
				}
			}
		return 0;
		}

// Our protocol
	ZRef<Link> GetLink()
		{ return fLink; }

private:
	const ZRef<Link> fLink;
	};

using YadMapAtR_WithLink = Channer_T<ChanAtR_NameRefYad_WithLink>;

// =================================================================================================
// MARK: - Link definition

Link::Link(const ZRef<CountedString>& iProtoName, const ZRef<YadMapAtR>& iYadMapAtR)
:	fProtoName(iProtoName)
,	fYadMapAtR(iYadMapAtR)
	{}

Link::Link(const ZRef<Link>& iParent, const ZRef<YadMapAtR>& iYadMapAtR)
:	fProtoName(iParent->fProtoName)
,	fParent(iParent)
,	fYadMapAtR(iYadMapAtR)
	{}

ZRef<YadMapAtR> Link::GetYadMapAtR()
	{ return fYadMapAtR; }

ZRef<YadR> Link::ReadAt(const Name& iName)
	{
	if (ZRef<Link> theLink = sGet(fChildren, iName))
		return new YadMapAtR_WithLink(theLink);

	if (ZRef<YadR> theYad = sReadAt(*fYadMapAtR, iName))
		{
		if (ZRef<YadMapAtR> asYadMapAtR = theYad.DynamicCast<YadMapAtR>())
			{
			ZRef<Link> theLink = new Link(this, asYadMapAtR);
			sInsertMust(fChildren, iName, theLink);
			return new YadMapAtR_WithLink(theLink);
			}
		else
			{
			return spWrap(this, theYad);
			}
		}

	if (not fCheckedProto())
		{
		if (ZRef<YadStrimmerR> theProtoYad =
			sReadAt(*fYadMapAtR, fProtoName->Get()).DynamicCast<YadStrimmerR>())
			{
			const string theTrailString = sReadAllUTF8(*theProtoYad);

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
					if (ZRef<YadR> theYadR = cur->ReadAt(theTrail.At(index)))
						{
						if (ZRef<YadMapAtR_WithLink> theYMARPos = theYadR.DynamicCast<YadMapAtR_WithLink>())
							{
							cur = theYMARPos->GetLink();
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
		return fProto->ReadAt(iName);

	return null;
	}

ZRef<Link> Link::WithRootAugment(const string& iRootAugmentName, const ZRef<Link>& iRootAugment)
	{
	if (fParent)
		{
		ZRef<Link> newParent = fParent->WithRootAugment(iRootAugmentName, iRootAugment);
		ZRef<Link> newSelf = new Link(newParent, fYadMapAtR);
		return newSelf;
		}

	ZRef<Link> newSelf = new Link(fProtoName, fYadMapAtR);
	sInsertMust(newSelf->fChildren, iRootAugmentName, iRootAugment);
	return newSelf;
	}

// =================================================================================================
// MARK: - Helpers

ZRef<YadR> spWrap(const ZRef<Link>& iLink, const ZRef<YadR>& iYad)
	{
	if (ZRef<YadMapAtR> theYadMapAtR = iYad.DynamicCast<YadMapAtR>())
		return new YadMapAtR_WithLink(new Link(iLink, theYadMapAtR));

	if (ZRef<YadSeqAtR> theYadSeqAtR = iYad.DynamicCast<YadSeqAtR>())
		return new Channer_T<ChanAtR_RefYad_WithLink>(iLink, theYadSeqAtR);

	if (ZRef<YadSeqR> theYadSeqR = iYad.DynamicCast<YadSeqR>())
		return new Channer_T<ChanR_RefYad_WithLink>(iLink, theYadSeqR);

	return iYad;
	}

} // anonymous namespace
} // namespace YadTree

// =================================================================================================
// MARK: -

using namespace YadTree;

ZRef<YadMapAtR> sYadTree(
	const ZRef<YadMapAtR>& iYadMapAtR, const string& iProtoName)
	{ return new YadMapAtR_WithLink(new Link(new CountedString(iProtoName), iYadMapAtR)); }

ZRef<YadMapAtR> sYadTree(const ZRef<YadMapAtR>& iYadMapAtR)
	{ return sYadTree(iYadMapAtR, "_"); }

ZRef<YadMapAtR> sParameterizedYadTree(const ZRef<YadMapAtR>& iBase,
	const string& iRootAugmentName, const ZRef<YadMapAtR>& iRootAugment)
	{
	if (ZRef<YadMapAtR_WithLink,false> theBase = iBase.DynamicCast<YadMapAtR_WithLink>())
		{}
	else if (ZRef<YadMapAtR_WithLink,false> theRootAugment =
		iRootAugment.DynamicCast<YadMapAtR_WithLink>())
		{}
	else
		{
		ZRef<Link> newLink =
			theBase->GetLink()->WithRootAugment(iRootAugmentName, theRootAugment->GetLink());
		return new YadMapAtR_WithLink(newLink);
		}
	return iBase;
	}

} // namespace ZooLib
