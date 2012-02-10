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

using std::string;

namespace ZooLib {
namespace YadTree {
namespace { // anonymous

typedef ZCountedVal<string> CountedString;

class Link;
class YadMat;

// =================================================================================================
// MARK: - Helpers

ZRef<ZYadR> spWrap(const ZRef<CountedString>& iProtoName,
	const ZRef<Link>& iLink, const ZRef<ZYadR>& iYad);

// =================================================================================================
// MARK: - Link declaration

class Link
:	public ZCounted
	{
public:
	Link(const ZRef<Link>& iParent, const ZRef<ZYadMat>& iYadMat);

	Link(const ZRef<Link>& iOther,
		const std::string& iRootAugmentName,
		const ZRef<ZYadMat>& iYadMat);

	ZRef<ZYadMat> GetYadMat();

	ZRef<ZYadR> ReadAt(const ZRef<CountedString>& iProtoName, const string& iName);

private:
	const ZRef<Link> fParent;
	const ZRef<ZYadMat> fYadMat;
	};

// =================================================================================================
// MARK: - YadSeqR

class YadSeqR
:	public ZYadSeqR
	{
public:
	YadSeqR(const ZRef<CountedString>& iProtoName,
		const ZRef<Link>& iLink,
		const ZRef<ZYadSeqR>& iYad)
	:	fProtoName(iProtoName)
	,	fLink(iLink)
	,	fYad(iYad)
		{}

// From ZYadSeqR
	virtual ZRef<ZYadR> ReadInc()
		{ return spWrap(fProtoName, fLink, fYad->ReadInc()); }

	virtual bool Skip()
		{ return fYad->Skip(); }

	virtual void SkipAll()
		{ fYad->SkipAll(); }

private:
	const ZRef<CountedString> fProtoName;
	const ZRef<Link> fLink;
	const ZRef<ZYadSeqR> fYad;
	};

// =================================================================================================
// MARK: - YadSeqRClone

class YadSeqRClone
:	public ZYadSeqRClone
	{
public:
	YadSeqRClone(const ZRef<CountedString>& iProtoName,
		const ZRef<Link>& iLink,
		const ZRef<ZYadSeqRClone>& iYad)
	:	fProtoName(iProtoName)
	,	fLink(iLink)
	,	fYad(iYad)
		{}

// From ZYadSeqR
	virtual ZRef<ZYadR> ReadInc()
		{ return spWrap(fProtoName, fLink, fYad->ReadInc()); }

	virtual bool Skip()
		{ return fYad->Skip(); }

	virtual void SkipAll()
		{ fYad->SkipAll(); }

// From ZYadSeqRClone
	virtual ZRef<ZYadSeqRClone> Clone()
		{ return new YadSeqRClone(fProtoName, fLink, fYad->Clone()); }

private:
	const ZRef<CountedString> fProtoName;
	const ZRef<Link> fLink;
	const ZRef<ZYadSeqRClone> fYad;
	};

// =================================================================================================
// MARK: - YadSeqRPos

class YadSeqRPos
:	public ZYadSeqRPos
	{
public:
	YadSeqRPos(const ZRef<CountedString>& iProtoName,
		const ZRef<Link>& iLink,
		const ZRef<ZYadSeqRPos>& iYad)
	:	fProtoName(iProtoName)
	,	fLink(iLink)
	,	fYad(iYad)
		{}

// From ZYadSeqR
	virtual ZRef<ZYadR> ReadInc()
		{ return spWrap(fProtoName, fLink, fYad->ReadInc()); }

	virtual bool Skip()
		{ return fYad->Skip(); }

	virtual void SkipAll()
		{ fYad->SkipAll(); }

// From ZYadSeqRClone
	virtual ZRef<ZYadSeqRClone> Clone()
		{ return new YadSeqRPos(fProtoName, fLink, fYad->Clone().DynamicCast<ZYadSeqRPos>()); }

// From ZYadSeqRPos
	virtual uint64 GetPosition()
		{ return fYad->GetPosition(); }

	virtual void SetPosition(uint64 iPosition)
		{ fYad->SetPosition(iPosition); }

	virtual uint64 GetSize()
		{ return fYad->GetSize(); }

private:
	const ZRef<CountedString> fProtoName;
	const ZRef<Link> fLink;
	const ZRef<ZYadSeqRPos> fYad;
	};

// =================================================================================================
// MARK: - YadSat

class YadSat
:	public ZYadSat
	{
public:
	YadSat(const ZRef<CountedString>& iProtoName,
		const ZRef<Link>& iLink,
		const ZRef<ZYadSat>& iYad)
	:	fProtoName(iProtoName)
	,	fLink(iLink)
	,	fYad(iYad)
		{}

	virtual bool IsSimple(const ZYadOptions& iOptions)
		{ return false; }

// From ZYadSat
	virtual uint64 Count()
		{ return fYad->Count(); }

	virtual ZRef<ZYadR> ReadAt(uint64 iPosition)
		{ return spWrap(fProtoName, fLink, fYad->ReadAt(iPosition)); }

// Our protocol
	ZRef<ZYadSat> GetYad()
		{ return fYad; }

private:
	const ZRef<CountedString> fProtoName;
	const ZRef<Link> fLink;
	const ZRef<ZYadSat> fYad;
	};

// =================================================================================================
// MARK: - YadSatRPos

class YadSatRPos
:	public ZYadSatRPos
	{
public:
	YadSatRPos(const ZRef<CountedString>& iProtoName,
		const ZRef<Link>& iLink,
		const ZRef<ZYadSatRPos>& iYad)
	:	fProtoName(iProtoName)
	,	fLink(iLink)
	,	fYad(iYad)
		{}

// From ZYadSeqR
	virtual ZRef<ZYadR> ReadInc()
		{ return spWrap(fProtoName, fLink, fYad->ReadInc()); }

	virtual bool Skip()
		{ return fYad->Skip(); }

	virtual void SkipAll()
		{ fYad->SkipAll(); }

// From ZYadSeqRClone
	virtual ZRef<ZYadSeqRClone> Clone()
		{ return new YadSeqRPos(fProtoName, fLink, fYad->Clone().DynamicCast<ZYadSatRPos>()); }

// From ZYadSeqRPos
	virtual uint64 GetPosition()
		{ return fYad->GetPosition(); }

	virtual void SetPosition(uint64 iPosition)
		{ fYad->SetPosition(iPosition); }

	virtual uint64 GetSize()
		{ return fYad->GetSize(); }

// From ZYadSat
	virtual uint64 Count()
		{ return fYad->Count(); }

	virtual ZRef<ZYadR> ReadAt(uint64 iPosition)
		{ return spWrap(fProtoName, fLink, fYad->ReadAt(iPosition)); }

private:
	const ZRef<CountedString> fProtoName;
	const ZRef<Link> fLink;
	const ZRef<ZYadSatRPos> fYad;
	};

// =================================================================================================
// MARK: - YadMat

class YadMat
:	public ZYadMat
	{
public:
	YadMat(const ZRef<CountedString>& iProtoName,
		const ZRef<Link>& iLink)
	:	fProtoName(iProtoName)
	,	fLink(iLink)
		{}

// From ZYadMat
	virtual bool IsSimple(const ZYadOptions& iOptions)
		{ return false; }

	virtual ZRef<ZYadR> ReadAt(const string& iName)
		{ return fLink->ReadAt(fProtoName, iName); }

// Our protocol
	const ZRef<CountedString> GetProtoName()
		{ return fProtoName; }

	ZRef<Link> GetLink()
		{ return fLink; }

	ZRef<ZYadMat> WithRootAugment
		(const std::string& iRootAugmentName, const ZRef<ZYadMat>& iRootAugment)
		{
#if 0

		ZRef<Link> newLink = new Link
			(fLink, iRootAugmentName, theRootAugment->GetLink()->GetYadMat());
		return new YadMat(theBase->GetProtoName(), newLink);
#endif
		return null;
		}

private:
	const ZRef<CountedString> fProtoName;
	const ZRef<Link> fLink;
	};

// =================================================================================================
// MARK: - Link definition

Link::Link(const ZRef<Link>& iParent, const ZRef<ZYadMat>& iYadMat)
:	fParent(iParent)
,	fYadMat(iYadMat)
	{
	// It's an error and a problem if iYadMat is a YadTree::YadMat.
	ZAssert(not iYadMat.DynamicCast<YadMat>());
	}

Link::Link(const ZRef<Link>& iOther,
	const std::string& iRootAugmentName,
	const ZRef<ZYadMat>& iYadMat)
:	fParent(iOther->fParent)
,	fYadMat(iOther->fYadMat)
	{
	}

ZRef<ZYadMat> Link::GetYadMat()
	{ return fYadMat; }

ZRef<ZYadR> Link::ReadAt(const ZRef<CountedString>& iProtoName, const string& iName)
	{
	using namespace ZUtil_STL;

	if (ZRef<ZYadR> theYad = fYadMat->ReadAt(iName))
		return spWrap(iProtoName, this, theYad);

	if (ZRef<ZYadStrimR> theProtoYad = fYadMat->ReadAt(iProtoName->Get()).DynamicCast<ZYadStrimR>())
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
						
			for (;;)
				{
				if (ZRef<ZYadR> theYadR = cur->ReadAt(iProtoName, theTrail.At(index)))
					{
					if (++index == theTrail.Count())
						return theYadR;

					if (ZRef<YadMat> theYadMat = theYadR.DynamicCast<YadMat>())
						{
						cur = theYadMat->GetLink();
						continue;
						}
					}
				break;
				}
			}
		}
	return null;
	}

// =================================================================================================
// MARK: - Helpers

class Visitor_Wrap
:	public ZVisitor_Do_T<ZRef<ZYadR> >
,	ZVisitor_Yad_PreferAt
	{
public:
	Visitor_Wrap(const ZRef<CountedString>& iProtoName, const ZRef<Link>& iLink)
	:	fProtoName(iProtoName)
	,	fLink(iLink)
		{}

	virtual void Visit_YadR(const ZRef<ZYadR>& iYadR)
		{ pSetResult(iYadR); }

	virtual void Visit_YadSeqR(const ZRef<ZYadSeqR>& iYadSeqR)
		{ pSetResult(new YadSeqR(fProtoName, fLink, iYadSeqR)); }

	virtual void Visit_YadSeqRClone(const ZRef<ZYadSeqRClone>& iYadSeqRClone)
		{ pSetResult(new YadSeqRClone(fProtoName, fLink, iYadSeqRClone)); }

	virtual void Visit_YadSeqRPos(const ZRef<ZYadSeqRPos>& iYadSeqRPos)
		{ pSetResult(new YadSeqRPos(fProtoName, fLink, iYadSeqRPos)); }

	virtual void Visit_YadSat(const ZRef<ZYadSat>& iYadSat)
		{ pSetResult(new YadSat(fProtoName, fLink, iYadSat)); }

	virtual void Visit_YadSatRPos(const ZRef<ZYadSatRPos>& iYadSatRPos)
		{ pSetResult(new YadSatRPos(fProtoName, fLink, iYadSatRPos)); }

	virtual void Visit_YadMat(const ZRef<ZYadMat>& iYadMat)
		{ pSetResult(new YadMat(fProtoName, new Link(fLink, iYadMat))); }

	const ZRef<CountedString>& fProtoName;
	const ZRef<Link>& fLink;
	};

ZRef<ZYadR> spWrap(const ZRef<CountedString>& iProtoName,
	const ZRef<Link>& iLink, const ZRef<ZYadR>& iYad)
	{ return Visitor_Wrap(iProtoName, iLink).Do(iYad); }

} // anonymous namespace
} // namespace YadTree

// =================================================================================================
// MARK: -

using namespace YadTree;

ZRef<ZYadMat> sYadTree(const ZRef<ZYadMat>& iYadMat, const string& iProtoName)
	{ return new YadMat(new CountedString(iProtoName), new Link(null, iYadMat)); }

ZRef<ZYadMat> sYadTree(const ZRef<ZYadMat>& iYadMat)
	{ return sYadTree(iYadMat, "_"); }

ZRef<ZYadMat> sParameterizedYadTree(const ZRef<ZYadMat>& iBase,
	const std::string& iRootAugmentName, const ZRef<ZYadMat>& iRootAugment)
	{
	if (ZRef<YadMat> theBase = iBase.DynamicCast<YadMat>())
		{
//##		if (ZRef<YadMat> theRootAugment = iRootAugment.DynamicCast<YadMat>())
//##			return theBase->WithRootAugment(iRootAugmentName, theRootAugment);
		}
	return iBase;
	}

} // namespace ZooLib
