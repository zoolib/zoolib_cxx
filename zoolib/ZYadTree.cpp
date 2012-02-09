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
#include "zoolib/ZGetSet.h"
#include "zoolib/ZTrail.h"
#include "zoolib/ZUtil_STL_map.h"
#include "zoolib/ZYadTree.h"

#include "zoolib/ZLog.h"

using std::string;

#if 0
#include "ZYad_Any.h"
namespace GameEngine {
void sDump(const ZooLib::ZAny& iAny);
} // namespace GameEngine
#endif


namespace ZooLib {
namespace YadTree {

typedef ZCountedVal<string> CountedString;

class Link;

// =================================================================================================
// MARK: - Helpers

static ZRef<ZYadR> spWrap(const ZRef<CountedString>& iProtoName,
	const ZRef<Link>& iLink, const ZRef<ZYadR>& iYad);

// =================================================================================================
// MARK: - Link declaration

class Link
:	public ZCounted
	{
private:
	Link(const Link& iOther);

public:
	Link(const ZRef<Link>& iParent, const ZRef<ZYadMapRPos>& iYadMapRPos);
	Link(const std::string& iRootAugmentName, const ZRef<Link>& iRootAugment,
		const ZRef<ZYadMapRPos>& iYadMapRPos);

	ZRef<Link> Clone();

	ZRef<ZYadR> ReadAt(const ZRef<CountedString>& iProtoName, const string& iName);

	ZRef<Link> WithRootAugment
		(const std::string& iRootAugmentName, const ZRef<Link>& iRootAugment);

private:
	ZRef<ZYadR> pReadAt(const string& iName);

	const ZRef<Link> fParent;
	ZRef<ZYadMapRPos> fYadMapRPos;
	
	std::map<string8, ZRef<Link> > fCacheByName;
	std::map<string8, ZRef<Link> > fCacheByTrail;
	};

// =================================================================================================
// MARK: - YadSeqRPos declaration

class YadSeqRPos
:	public ZYadSeqRPos
	{
public:
	YadSeqRPos(const ZRef<CountedString>& iProtoName,
		const ZRef<Link>& iLink, const ZRef<ZYadSeqRPos>& iYadSeqRPos)
	:	fProtoName(iProtoName)
	,	fLink(iLink)
	,	fYadSeqRPos(iYadSeqRPos)
		{}

// From ZYadSeqR via ZYadSeqRPos
	virtual ZRef<ZYadR> ReadInc()
		{ return spWrap(fProtoName, fLink, fYadSeqRPos->ReadInc()); }

// From ZYadSeqRClone via ZYadSeqRPos
	virtual ZRef<ZYadSeqRClone> Clone()
		{ return new YadSeqRPos(fProtoName, fLink, fYadSeqRPos->Clone().DynamicCast<ZYadSeqRPos>()); }

// From ZYadSeqRPos
	virtual uint64 GetPosition()
		{ return fYadSeqRPos->GetPosition(); }

	virtual void SetPosition(uint64 iPosition)
		{ fYadSeqRPos->SetPosition(iPosition); }

	virtual uint64 GetSize()
		{ return fYadSeqRPos->GetSize(); }

	virtual ZRef<ZYadR> ReadAt(uint64 iPosition)
		{ return spWrap(fProtoName, fLink, fYadSeqRPos->ReadAt(iPosition)); }

private:
	const ZRef<CountedString> fProtoName;
	const ZRef<Link> fLink;
	const ZRef<ZYadSeqRPos> fYadSeqRPos;
	};

// =================================================================================================
// MARK: - YadMapRPos declaration

class YadMapRPos
:	public ZYadMapRPos
	{
public:
	YadMapRPos(const ZRef<CountedString>& iProtoName,
		const ZRef<Link>& iLink, const string& iPosition);

	YadMapRPos(const ZRef<CountedString>& iProtoName, const ZRef<ZYadMapRPos>& iYad);

// From ZYadMapR via ZYadMapRPos
	ZRef<ZYadR> ReadInc(string& oName);

// From ZYadMapRClone via ZYadMapRPos
	virtual ZRef<ZYadMapRClone> Clone();

// From ZYadMapRPos
	virtual void SetPosition(const string& iName);
	virtual ZRef<ZYadR> ReadAt(const string& iName);

// Our protocol
	ZRef<Link> GetLink();
	ZRef<YadMapRPos> WithRootAugment
		(const std::string& iRootAugmentName, const ZRef<Link>& iRootAugment);

private:
	const ZRef<CountedString> fProtoName;
	ZRef<Link> fLink;
	string fPosition;
	};

// =================================================================================================
// MARK: - Link definition

Link::Link(const Link& iOther)
:	fParent(iOther.fParent)
,	fYadMapRPos(iOther.fYadMapRPos->Clone().DynamicCast<ZYadMapRPos>())
,	fCacheByName(iOther.fCacheByName)
,	fCacheByTrail(iOther.fCacheByTrail)
	{}

Link::Link(const ZRef<Link>& iParent, const ZRef<ZYadMapRPos>& iYadMapRPos)
:	fParent(iParent)
,	fYadMapRPos(iYadMapRPos)
	{
	// It's an error and a problem if iYadMapRPos is a YadTree::YadMapRPos.
	ZAssert(not iYadMapRPos.DynamicCast<YadMapRPos>());
	}

Link::Link(const std::string& iRootAugmentName, const ZRef<Link>& iRootAugment,
	const ZRef<ZYadMapRPos>& iYadMapRPos)
:	fYadMapRPos(iYadMapRPos->Clone().DynamicCast<ZYadMapRPos>())
	{
	ZUtil_STL::sInsertMustNotContain(fCacheByName, iRootAugmentName, iRootAugment);	
	}

ZRef<Link> Link::Clone()
	{ return new Link(*this); }

ZRef<ZYadR> Link::pReadAt(const string& iName)
	{ return fYadMapRPos->ReadAt(iName); }

ZRef<ZYadR> Link::ReadAt(const ZRef<CountedString>& iProtoName, const string& iName)
	{
	using namespace ZUtil_STL;

	if (ZRef<ZYadR> theYad = this->pReadAt(iName))
		return spWrap(iProtoName, this, theYad);
	
	ZRef<Link> theLink;
	if (ZQ<ZRef<Link> > theByNameQ = sQGet(fCacheByName, iName))
		{
		theLink = *theByNameQ;
		}
	else if (ZRef<ZYadStrimR> theProtoYad = this->pReadAt(iProtoName->Get()).DynamicCast<ZYadStrimR>())
		{
		const string theTrailString = theProtoYad->GetStrimR().ReadAll8();
		if (ZQ<ZRef<Link> > theByTrailQ = sQGet(fCacheByTrail, theTrailString))
			{
			theLink = *theByTrailQ;
			}
		else
			{
			if (theTrailString.size())
				{
				size_t index = 0;
				const ZTrail theTrail = ZTrail(theTrailString).Normalized();
				ZRef<Link> cur = this;

				if (theTrailString[0] == '/')
					{
					// Walk up to the root.
					for (ZRef<Link> next = null; next = cur->fParent; cur = next)
						{}
					}
				else
					{
					// Follow leading bounces.
					for (/*no init*/;
						cur && index < theTrail.Count() && theTrail.At(index).empty();
						++index, cur = cur->fParent)
						{}
					}

				// Walk down the remainder of the trail
				
				if (index < theTrail.Count())
					{
					for (;;)
						{
						const string theComp = theTrail.At(index);
						if (ZQ<ZRef<Link> > aLink = sQGet(cur->fCacheByName, theComp))
							{
							cur = *aLink;
							}
						else if (ZRef<ZYadMapRPos,0> theYadMapRPos =
							cur->pReadAt(theComp).DynamicCast<ZYadMapRPos>())
							{
							break;
							}
						else
							{
//##							if (ZRef<ZYadMapRPos_Any> theM = theYadMapRPos.DynamicCast<ZYadMapRPos_Any>())
//##								GameEngine::sDump(ZAny(theM->GetMap()));
							cur = new Link(cur, theYadMapRPos);
							}
						if (++index == theTrail.Count())
							{
							theLink = cur;
							break;
							}
						}
					}
				}
			sInsertMustNotContain(fCacheByTrail, theTrailString, theLink);
			}
		sInsertMustNotContain(fCacheByName, iName, theLink);
		}

	if (theLink)
		return theLink->ReadAt(iProtoName, iName);

	// Yay, lexical scoping, disabled for now.
	if (false && fParent)
		return spWrap(iProtoName, fParent, fParent->pReadAt(iName));

	return null;
	}

ZRef<Link> Link::WithRootAugment
	(const std::string& iRootAugmentName, const ZRef<Link>& iRootAugment)
	{
	ZRef<ZYadMapRPos> newYad = fYadMapRPos->Clone().DynamicCast<ZYadMapRPos>();
	if (fParent)
		return new Link(fParent->WithRootAugment(iRootAugmentName, iRootAugment), newYad);
	return new Link(iRootAugmentName, iRootAugment, newYad);
	}

// =================================================================================================
// MARK: - YadMapRPos definition

YadMapRPos::YadMapRPos(const ZRef<CountedString>& iProtoName,
	const ZRef<Link>& iLink, const string& iPosition)
:	fProtoName(iProtoName)
,	fLink(iLink)
,	fPosition(iPosition)
	{}

YadMapRPos::YadMapRPos(const ZRef<CountedString>& iProtoName, const ZRef<ZYadMapRPos>& iYad)
:	fProtoName(iProtoName)
,	fLink(new Link(null, iYad))
	{}

ZRef<ZYadR> YadMapRPos::ReadInc(string& oName)
	{ ZUnimplemented(); }

ZRef<ZYadR> YadMapRPos::ReadAt(const string& iName)
	{
	if (fLink->IsShared())
		fLink = fLink->Clone();
	fPosition.clear();
	return fLink->ReadAt(fProtoName, iName);
	}

ZRef<ZYadMapRClone> YadMapRPos::Clone()
	{ return new YadMapRPos(fProtoName, fLink->Clone(), fPosition); }

void YadMapRPos::SetPosition(const string& iName)
	{ fPosition = iName; }

ZRef<Link> YadMapRPos::GetLink()
	{ return fLink; }

ZRef<YadMapRPos> YadMapRPos::WithRootAugment
	(const std::string& iRootAugmentName, const ZRef<Link>& iRootAugment)
	{
	ZRef<Link> newLink = fLink->WithRootAugment(iRootAugmentName, iRootAugment);
	return new YadMapRPos(fProtoName, newLink, string());
	}

// =================================================================================================
// MARK: - Helpers

static ZRef<ZYadR> spWrap(const ZRef<CountedString>& iProtoName,
	const ZRef<Link>& iLink, const ZRef<ZYadR>& iYad)
	{
	if (ZRef<ZYadSeqRPos> theYadSeqRPos = iYad.DynamicCast<ZYadSeqRPos>())
		return new YadSeqRPos(iProtoName, iLink, theYadSeqRPos);

	if (ZRef<ZYadMapRPos> theYadMapRPos = iYad.DynamicCast<ZYadMapRPos>())
		return new YadMapRPos(iProtoName, new Link(iLink, theYadMapRPos), string());

	return iYad;
	}

} // namespace YadTree

// =================================================================================================
// MARK: - YadMapRPos definition

using namespace YadTree;

ZRef<ZYadMapRPos> sYadTree(const ZRef<ZYadMapRPos>& iYadMapRPos, const string& iProtoName)
	{ return new YadMapRPos(new CountedString(iProtoName), iYadMapRPos); }

ZRef<ZYadMapRPos> sYadTree(const ZRef<ZYadMapRPos>& iYadMapRPos)
	{ return sYadTree(iYadMapRPos, "_"); }

ZRef<ZYadMapRPos> sParameterizedYadTree(const ZRef<ZYadMapRPos>& iBase,
	const std::string& iRootAugmentName, const ZRef<ZYadMapRPos>& iRootAugment)
	{
	if (ZRef<YadMapRPos> theBase = iBase.DynamicCast<YadMapRPos>())
		{
		if (ZRef<YadMapRPos> theRootAugment = iRootAugment.DynamicCast<YadMapRPos>())
			return theBase->WithRootAugment(iRootAugmentName, theRootAugment->GetLink());
		}
	return iBase;
	}
} // namespace ZooLib
