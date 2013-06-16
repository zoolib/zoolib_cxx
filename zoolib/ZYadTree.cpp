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

#include "zoolib/ZCountedVal.h"
#include "zoolib/ZLog.h"
#include "zoolib/ZTrail.h"
#include "zoolib/ZUtil_STL_map.h"
#include "zoolib/ZVisitor_Do_T.h"
#include "zoolib/ZYadTree.h"

using std::map;
using std::string;
using std::vector;

namespace ZooLib {
namespace YadTree {
namespace { // anonymous

using namespace ZUtil_STL;

typedef ZCountedVal<ZName> CountedString;

class Link;
class YadMapAtRPos;

// =================================================================================================
// MARK: - Helpers

ZRef<ZYadR> spWrap(const ZRef<Link>& iLink, const ZRef<ZYadR>& iYad);
ZRef<ZYadR> spWrap_NoAt(const ZRef<Link>& iLink, const ZRef<ZYadR>& iYad);

// =================================================================================================
// MARK: - Link declaration

class Link
:	public ZCounted
	{
public:
// ctor that establishes a tree
	Link(const ZRef<CountedString>& iProtoName, const ZRef<ZYadMapAtRPos>& iYadMapAtRPos);

// ctor used as we walk down a tree.
	Link(const ZRef<Link>& iParent, const ZRef<ZYadMapAtRPos>& iYadMapAtRPos);

	ZRef<ZYadMapAtRPos> GetYadMapAtRPos();

	ZRef<ZYadR> ReadAt(const ZName& iName);

	ZRef<Link> WithRootAugment(const string& iRootAugmentName, const ZRef<Link>& iRootAugment);

private:
	const ZRef<CountedString> fProtoName;
	const ZRef<Link> fParent;
	const ZRef<ZYadMapAtRPos> fYadMapAtRPos;
	map<string,ZRef<Link> > fChildren;
	bool fCheckedProto;
	ZRef<Link> fProto;
	};

// =================================================================================================
// MARK: - YadSeqR

class YadSeqR
:	public ZYadSeqR
	{
public:
	YadSeqR(const ZRef<Link>& iLink, const ZRef<ZYadSeqR>& iYad)
	:	fLink(iLink)
	,	fYad(iYad)
		{}

// From ZYadSeqR
	virtual ZRef<ZYadR> ReadInc()
		{ return spWrap(fLink, fYad->ReadInc()); }

	virtual bool Skip()
		{ return fYad->Skip(); }

	virtual void SkipAll()
		{ fYad->SkipAll(); }

private:
	const ZRef<Link> fLink;
	const ZRef<ZYadSeqR> fYad;
	};

// =================================================================================================
// MARK: - YadSeqRClone

class YadSeqRClone
:	public ZYadSeqRClone
	{
public:
	YadSeqRClone(const ZRef<Link>& iLink, const ZRef<ZYadSeqRClone>& iYad)
	:	fLink(iLink)
	,	fYad(iYad)
		{}

// From ZYadSeqR
	virtual ZRef<ZYadR> ReadInc()
		{ return spWrap(fLink, fYad->ReadInc()); }

	virtual bool Skip()
		{ return fYad->Skip(); }

	virtual void SkipAll()
		{ fYad->SkipAll(); }

// From ZYadSeqRClone
	virtual ZRef<ZYadSeqRClone> Clone()
		{ return new YadSeqRClone(fLink, fYad->Clone()); }

private:
	const ZRef<Link> fLink;
	const ZRef<ZYadSeqRClone> fYad;
	};

// =================================================================================================
// MARK: - YadSeqRPos

class YadSeqRPos
:	public ZYadSeqRPos
	{
public:
	YadSeqRPos(const ZRef<Link>& iLink, const ZRef<ZYadSeqRPos>& iYad)
	:	fLink(iLink)
	,	fYad(iYad)
		{}

// From ZYadSeqR
	virtual ZRef<ZYadR> ReadInc()
		{ return spWrap(fLink, fYad->ReadInc()); }

	virtual bool Skip()
		{ return fYad->Skip(); }

	virtual void SkipAll()
		{ fYad->SkipAll(); }

// From ZYadSeqRClone
	virtual ZRef<ZYadSeqRClone> Clone()
		{ return new YadSeqRPos(fLink, fYad->Clone().DynamicCast<ZYadSeqRPos>()); }

// From ZYadSeqRPos
	virtual uint64 GetPosition()
		{ return fYad->GetPosition(); }

	virtual void SetPosition(uint64 iPosition)
		{ fYad->SetPosition(iPosition); }

	virtual uint64 GetSize()
		{ return fYad->GetSize(); }

private:
	const ZRef<Link> fLink;
	const ZRef<ZYadSeqRPos> fYad;
	};

// =================================================================================================
// MARK: - YadSeqAtR

class YadSeqAtR
:	public ZYadSeqAtR
	{
public:
	YadSeqAtR(const ZRef<Link>& iLink, const ZRef<ZYadSeqAtR>& iYad)
	:	fLink(iLink)
	,	fYad(iYad)
	,	fChildren(fYad->Count(), null)
		{}

	virtual bool IsSimple(const ZYadOptions& iOptions)
		{ return false; }

// From ZYadSeqAtR
	virtual uint64 Count()
		{ return fYad->Count(); }

	virtual ZRef<ZYadR> ReadAt(uint64 iPosition)
		{
		ZRef<ZYadR> theYad;
		if (iPosition < fChildren.size())
			{
			theYad = fChildren[iPosition];
			if (not theYad)
				{
				theYad = spWrap(fLink, fYad->ReadAt(iPosition));
				fChildren[iPosition] = theYad;
				}
			}
		return theYad;
		}

	virtual ZQ<ZAny> QAsAny()
		{ return fYad->QAsAny(); }

// Our protocol
	ZRef<ZYadSeqAtR> GetYad()
		{ return fYad; }

private:
	const ZRef<Link> fLink;
	const ZRef<ZYadSeqAtR> fYad;
	vector<ZRef<ZYadR> > fChildren;
	};

// =================================================================================================
// MARK: - YadSeqAtRPos

class YadSeqAtRPos
:	public ZYadSeqAtRPos
	{
public:
	YadSeqAtRPos(const ZRef<Link>& iLink, const ZRef<ZYadSeqAtRPos>& iYad)
	:	fLink(iLink)
	,	fYad(iYad)
	,	fChildren(fYad->Count(), null)
		{}

// From ZYadSeqR
	virtual ZRef<ZYadR> ReadInc()
		{ return spWrap(fLink, fYad->ReadInc()); }

	virtual bool Skip()
		{ return fYad->Skip(); }

	virtual void SkipAll()
		{ fYad->SkipAll(); }

// From ZYadSeqRClone
	virtual ZRef<ZYadSeqRClone> Clone()
		{ return new YadSeqRPos(fLink, fYad->Clone().DynamicCast<ZYadSeqAtRPos>()); }

// From ZYadSeqRPos
	virtual uint64 GetPosition()
		{ return fYad->GetPosition(); }

	virtual void SetPosition(uint64 iPosition)
		{ fYad->SetPosition(iPosition); }

	virtual uint64 GetSize()
		{ return fYad->GetSize(); }

// From ZYadSeqAtR
	virtual uint64 Count()
		{ return fYad->Count(); }

	virtual ZRef<ZYadR> ReadAt(uint64 iPosition)
		{
		ZRef<ZYadR> theYad;
		if (iPosition < fChildren.size())
			{
			theYad = fChildren[iPosition];
			if (not theYad)
				{
				theYad = spWrap(fLink, fYad->ReadAt(iPosition));
				fChildren[iPosition] = theYad;
				}
			}
		return theYad;
		}

	virtual ZQ<ZAny> QAsAny()
		{ return fYad->QAsAny(); }

private:
	const ZRef<Link> fLink;
	const ZRef<ZYadSeqAtRPos> fYad;
	vector<ZRef<ZYadR> > fChildren;
	};

// =================================================================================================
// MARK: - YadMapAtRPos

class YadMapAtRPos
:	public ZYadMapAtRPos
	{
	YadMapAtRPos(const YadMapAtRPos& iOther)
	:	fLink(iOther.fLink)
		{
		if (iOther.fYadMapAtRPos)
			fYadMapAtRPos = iOther.fYadMapAtRPos->Clone().DynamicCast<ZYadMapAtRPos>();
		}

public:
	YadMapAtRPos(const ZRef<Link>& iLink)
	:	fLink(iLink)
		{}

// From ZYadR
	virtual bool IsSimple(const ZYadOptions& iOptions)
		{ return false; }

// From ZYadMapR
	virtual ZRef<ZYadR> ReadInc(ZName& oName)
		{
		this->pGenMap();
		return fYadMapAtRPos->ReadInc(oName);
		}

// From ZYadMapRClone
	virtual ZRef<ZYadMapRClone> Clone()
		{ return new YadMapAtRPos(*this); }

// From ZYadMapAtR
	virtual ZRef<ZYadR> ReadAt(const ZName& iName)
		{ return fLink->ReadAt(iName); }

	virtual ZQ<ZAny> QAsAny()
		{
		if (ZRef<ZYadMapAtRPos> theYadMapAtRPos = fLink->GetYadMapAtRPos())
			return theYadMapAtRPos->QAsAny();
		return null;
		}

// From ZYadMapRPos
	virtual void SetPosition(const ZName& iName)
		{
		this->pGenMap();
		fYadMapAtRPos->SetPosition(iName);
		}

// Our protocol
	ZRef<Link> GetLink()
		{ return fLink; }

private:
	void pGenMap()
		{
		if (not fYadMapAtRPos)
			fYadMapAtRPos = fLink->GetYadMapAtRPos()->Clone().DynamicCast<ZYadMapAtRPos>();
		}

	const ZRef<Link> fLink;
	ZRef<ZYadMapAtRPos> fYadMapAtRPos;
	};

// =================================================================================================
// MARK: - Link definition

Link::Link(const ZRef<CountedString>& iProtoName, const ZRef<ZYadMapAtRPos>& iYadMapAtRPos)
:	fCheckedProto(false)
,	fProtoName(iProtoName)
,	fYadMapAtRPos(iYadMapAtRPos)
	{}

Link::Link(const ZRef<Link>& iParent, const ZRef<ZYadMapAtRPos>& iYadMapAtRPos)
:	fCheckedProto(false)
,	fProtoName(iParent->fProtoName)
,	fParent(iParent)
,	fYadMapAtRPos(iYadMapAtRPos)
	{}

ZRef<ZYadMapAtRPos> Link::GetYadMapAtRPos()
	{ return fYadMapAtRPos; }

ZRef<ZYadR> Link::ReadAt(const ZName& iName)
	{
	if (ZQ<ZRef<Link> > theQ = sQGet(fChildren, iName))
		return new YadMapAtRPos(*theQ);

	if (ZRef<ZYadR> theYad = fYadMapAtRPos->ReadAt(iName))
		{
		if (ZRef<ZYadMapAtRPos> asYadMapAtRPos = theYad.DynamicCast<ZYadMapAtRPos>())
			{
			ZRef<Link> theLink = new Link(this, asYadMapAtRPos);
			sInsertMust(fChildren, iName, theLink);
			return new YadMapAtRPos(theLink);
			}
		else
			{
			return spWrap_NoAt(this, theYad);
			}
		}

	if (not sGetSet(fCheckedProto, true))
		{
		if (ZRef<ZYadStrimmerR> theProtoYad =
			fYadMapAtRPos->ReadAt(fProtoName->Get()).DynamicCast<ZYadStrimmerR>())
			{
			const string theTrailString = theProtoYad->GetStrimR().ReadAll8();

			if (theTrailString.size())
				{
				// Our Yad has a non-empty proto trail.
				size_t index = 0;
				const ZTrail theTrail = ZTrail(theTrailString).Normalized();
				
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
					if (ZRef<ZYadR> theYadR = cur->ReadAt(theTrail.At(index)))
						{
						if (ZRef<YadMapAtRPos> theYMARPos = theYadR.DynamicCast<YadMapAtRPos>())
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
		ZRef<Link> newSelf = new Link(newParent, fYadMapAtRPos);
		return newSelf;
		}

	ZRef<Link> newSelf = new Link(fProtoName, fYadMapAtRPos);
	sInsertMust(newSelf->fChildren, iRootAugmentName, iRootAugment);
	return newSelf;
	}

// =================================================================================================
// MARK: - Helpers

class Visitor_Wrap
:	public ZVisitor_Do_T<ZRef<ZYadR> >
,	ZVisitor_Yad_PreferAt
	{
public:
	Visitor_Wrap(const ZRef<Link>& iLink)
	:	fLink(iLink)
		{}

	virtual void Visit_YadR(const ZRef<ZYadR>& iYadR)
		{ pSetResult(iYadR); }

	virtual void Visit_YadSeqR(const ZRef<ZYadSeqR>& iYadSeqR)
		{ pSetResult(new YadSeqR(fLink, iYadSeqR)); }

	virtual void Visit_YadSeqRClone(const ZRef<ZYadSeqRClone>& iYadSeqRClone)
		{ pSetResult(new YadSeqRClone(fLink, iYadSeqRClone)); }

	virtual void Visit_YadSeqRPos(const ZRef<ZYadSeqRPos>& iYadSeqRPos)
		{ pSetResult(new YadSeqRPos(fLink, iYadSeqRPos)); }

	virtual void Visit_YadSeqAtR(const ZRef<ZYadSeqAtR>& iYadSeqAtR)
		{ pSetResult(new YadSeqAtR(fLink, iYadSeqAtR)); }

	virtual void Visit_YadSeqAtRPos(const ZRef<ZYadSeqAtRPos>& iYadSeqAtRPos)
		{ pSetResult(new YadSeqAtRPos(fLink, iYadSeqAtRPos)); }

	virtual void Visit_YadMapAtRPos(const ZRef<ZYadMapAtRPos>& iYadMapAtRPos)
		{ pSetResult(new YadMapAtRPos(new Link(fLink, iYadMapAtRPos))); }

	const ZRef<Link>& fLink;
	};

ZRef<ZYadR> spWrap(const ZRef<Link>& iLink, const ZRef<ZYadR>& iYad)
	{ return Visitor_Wrap(iLink).Do(iYad); }

class Visitor_Wrap_NoAt
:	public Visitor_Wrap
	{
public:
	Visitor_Wrap_NoAt(const ZRef<Link>& iLink)
	:	Visitor_Wrap(iLink)
		{}

	virtual void Visit_YadMapAtRPos(const ZRef<ZYadMapAtRPos>& iYadMapAtRPos)
		{ ZUnimplemented(); }
	};

ZRef<ZYadR> spWrap_NoAt(const ZRef<Link>& iLink, const ZRef<ZYadR>& iYad)
	{ return Visitor_Wrap_NoAt(iLink).Do(iYad); }

} // anonymous namespace
} // namespace YadTree

// =================================================================================================
// MARK: -

using namespace YadTree;

ZRef<ZYadMapAtRPos> sYadTree(
	const ZRef<ZYadMapAtRPos>& iYadMapAtRPos, const string& iProtoName)
	{ return new YadMapAtRPos(new Link(new CountedString(iProtoName), iYadMapAtRPos)); }

ZRef<ZYadMapAtRPos> sYadTree(const ZRef<ZYadMapAtRPos>& iYadMapAtRPos)
	{ return sYadTree(iYadMapAtRPos, "_"); }

ZRef<ZYadMapAtRPos> sParameterizedYadTree(const ZRef<ZYadMapAtRPos>& iBase,
	const string& iRootAugmentName, const ZRef<ZYadMapAtRPos>& iRootAugment)
	{
	if (ZRef<YadMapAtRPos> theBase = iBase.DynamicCast<YadMapAtRPos>())
		{
		if (ZRef<YadMapAtRPos> theRootAugment = iRootAugment.DynamicCast<YadMapAtRPos>())
			{
			ZRef<Link> newLink =
				theBase->GetLink()->WithRootAugment(iRootAugmentName, theRootAugment->GetLink());
			return new YadMapAtRPos(newLink);
			}
		}
	return iBase;
	}

} // namespace ZooLib
