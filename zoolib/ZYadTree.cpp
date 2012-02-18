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

typedef ZCountedVal<string> CountedString;

class Link;
class YadMapAtR;

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
	Link(const ZRef<CountedString>& iProtoName, const ZRef<ZYadMapAtR>& iYadMapAtR);

// ctor used as we walk down a tree.
	Link(const ZRef<Link>& iParent, const ZRef<ZYadMapAtR>& iYadMapAtR);

	ZRef<ZYadMapAtR> GetYadMapAtR();

	ZRef<ZYadR> ReadAt(const string& iName);

	ZRef<Link> WithRootAugment(const string& iRootAugmentName, const ZRef<Link>& iRootAugment);

private:
	const ZRef<CountedString> fProtoName;
	const ZRef<Link> fParent;
	const ZRef<ZYadMapAtR> fYadMapAtR;
	map<string,ZRef<Link> > fChildren;
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
// MARK: - YadMapAtR

class YadMapAtR
:	public ZYadMapAtR
	{
public:
	YadMapAtR(const ZRef<Link>& iLink)
	:	fLink(iLink)
		{}

// From ZYadMapAtR
	virtual bool IsSimple(const ZYadOptions& iOptions)
		{ return false; }

	virtual ZRef<ZYadR> ReadAt(const string& iName)
		{ return fLink->ReadAt(iName); }

	virtual ZQ<ZAny> QAsAny()
		{
		if (ZRef<ZYadMapAtR> theYadMapAtR = fLink->GetYadMapAtR())
			return theYadMapAtR->QAsAny();
		return null;
		}

// Our protocol
	ZRef<Link> GetLink()
		{ return fLink; }

private:
	const ZRef<Link> fLink;
	};

// =================================================================================================
// MARK: - Link definition

Link::Link(const ZRef<CountedString>& iProtoName, const ZRef<ZYadMapAtR>& iYadMapAtR)
:	fProtoName(iProtoName)
,	fYadMapAtR(iYadMapAtR)
	{}

Link::Link(const ZRef<Link>& iParent, const ZRef<ZYadMapAtR>& iYadMapAtR)
:	fProtoName(iParent->fProtoName)
,	fParent(iParent)
,	fYadMapAtR(iYadMapAtR)
	{}

ZRef<ZYadMapAtR> Link::GetYadMapAtR()
	{ return fYadMapAtR; }

ZRef<ZYadR> Link::ReadAt(const string& iName)
	{
	if (ZQ<ZRef<Link> > theQ = sQGet(fChildren, iName))
		return new YadMapAtR(*theQ);

	if (ZRef<ZYadR> theYad = fYadMapAtR->ReadAt(iName))
		{
		if (ZRef<ZYadMapAtR> asYadMapAtR = theYad.DynamicCast<ZYadMapAtR>())
			{
			ZRef<Link> theLink = new Link(this, asYadMapAtR);
			sInsertMustNotContain(fChildren, iName, theLink);
			return new YadMapAtR(theLink);
			}
		else
			{
			return spWrap_NoAt(this, theYad);
			}
		}

	if (ZRef<ZYadStrimmerR> theProtoYad = fYadMapAtR->ReadAt(fProtoName->Get()).DynamicCast<ZYadStrimmerR>())
		{
		const string theTrailString = theProtoYad->GetStrimR().ReadAll8();

		if (theTrailString.size())
			{
			// Our Yad has a non-empty proto trail.
			size_t index = 0;
			const ZTrail theTrail = ZTrail(theTrailString).Normalized() + iName;
			
			ZRef<Link> cur = this;

			if (theTrailString[0] == '/')
				{
				// Walk up to the root.
				for (ZRef<Link> next = null; next = cur->fParent; cur = next)
					{}
				}
			else
				{
				// Follow any leading bounces.
				for (/*no init*/;
					cur && index < theTrail.Count() && theTrail.At(index).empty();
					++index, cur = cur->fParent)
					{}
				}
						
			// Walk down the tree.
			for (;;)
				{
				if (ZRef<ZYadR> theYadR = cur->ReadAt(theTrail.At(index)))
					{
					if (++index == theTrail.Count())
						return theYadR;

					if (ZRef<YadMapAtR> theYadMapAtR = theYadR.DynamicCast<YadMapAtR>())
						{
						cur = theYadMapAtR->GetLink();
						continue;
						}
					}
				break;
				}
			}
		}
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
	sInsertMustNotContain(newSelf->fChildren, iRootAugmentName, iRootAugment);
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

	virtual void Visit_YadMapAtR(const ZRef<ZYadMapAtR>& iYadMapAtR)
		{ pSetResult(new YadMapAtR(new Link(fLink, iYadMapAtR))); }

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

	virtual void Visit_YadMapAtR(const ZRef<ZYadMapAtR>& iYadMapAtR)
		{ ZUnimplemented(); }
	};

ZRef<ZYadR> spWrap_NoAt(const ZRef<Link>& iLink, const ZRef<ZYadR>& iYad)
	{ return Visitor_Wrap_NoAt(iLink).Do(iYad); }

} // anonymous namespace
} // namespace YadTree

// =================================================================================================
// MARK: -

using namespace YadTree;

ZRef<ZYadMapAtR> sYadTree(const ZRef<ZYadMapAtR>& iYadMapAtR, const string& iProtoName)
	{ return new YadMapAtR(new Link(new CountedString(iProtoName), iYadMapAtR)); }

ZRef<ZYadMapAtR> sYadTree(const ZRef<ZYadMapAtR>& iYadMapAtR)
	{ return sYadTree(iYadMapAtR, "_"); }

ZRef<ZYadMapAtR> sParameterizedYadTree(const ZRef<ZYadMapAtR>& iBase,
	const string& iRootAugmentName, const ZRef<ZYadMapAtR>& iRootAugment)
	{
	if (ZRef<YadMapAtR> theBase = iBase.DynamicCast<YadMapAtR>())
		{
		if (ZRef<YadMapAtR> theRootAugment = iRootAugment.DynamicCast<YadMapAtR>())
			{
			ZRef<Link> newLink =
				theBase->GetLink()->WithRootAugment(iRootAugmentName, theRootAugment->GetLink());
			return new YadMapAtR(newLink);
			}
		}
	return iBase;
	}

} // namespace ZooLib
