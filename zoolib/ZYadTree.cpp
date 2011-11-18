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

using std::string;

namespace ZooLib {
namespace YadTree {

typedef ZCountedVal<string> CountedString;

// =================================================================================================
#pragma mark -
#pragma mark * Chain declaration

class Chain
:	public ZCounted
	{
private:
	Chain(const Chain& iOther);

public:
	Chain
		(const ZRef<Chain>& iParent, const ZRef<ZYadMapRPos>& iYadMapRPos);

	ZRef<Chain> Clone();

	ZRef<ZYadR> ReadInc(string& oName);
	ZRef<ZYadR> ReadAt
		(const ZRef<CountedString>& iProtoName, const ZRef<CountedString>& iLexicalScopeName,
		const string& iName);

private:
	ZRef<ZYadR> pReadAt
		(const ZRef<CountedString>& iProtoName, const ZRef<CountedString>& iLexicalScopeName,
		const string& iName, bool iAllowLexicalScope);

	ZRef<ZYadR> pReadAt(const string& iName);

	const ZRef<Chain> fParent;
	ZRef<ZYadMapRPos> fYadMapRPos;
	
	std::map<string8, ZRef<Chain> > fCacheByName;
	std::map<string8, ZRef<Chain> > fCacheByTrail;
	};

// =================================================================================================
#pragma mark -
#pragma mark * YadSeqRPos declaration

class YadSeqRPos
:	public ZYadSeqRPos
	{
public:
	YadSeqRPos(const ZRef<CountedString>& iProtoName, const ZRef<CountedString>& iLexicalScopeName,
		const ZRef<Chain>& iChain, const ZRef<ZYadSeqRPos>& iYadSeqRPos);

// From ZYadSeqR via ZYadSeqRPos
	virtual ZRef<ZYadR> ReadInc();

// From ZYadSeqRClone via ZYadSeqRPos
	virtual ZRef<ZYadSeqRClone> Clone();

// From ZYadSeqRPos
	virtual uint64 GetPosition();
	virtual void SetPosition(uint64 iPosition);
	virtual uint64 GetSize();

	virtual ZRef<ZYadR> ReadAt(uint64 iPosition);

private:
	const ZRef<CountedString> fProtoName;
	const ZRef<CountedString> fLexicalScopeName;
	const ZRef<Chain> fChain;
	const ZRef<ZYadSeqRPos> fYadSeqRPos;
	};

// =================================================================================================
#pragma mark -
#pragma mark * YadMapRPos declaration

class YadMapRPos
:	public ZYadMapRPos
	{
public:
	YadMapRPos(const ZRef<CountedString>& iProtoName, const ZRef<CountedString>& iLexicalScopeName,
		const ZRef<Chain>& iChain, const string& iPosition);

	YadMapRPos(const ZRef<CountedString>& iProtoName, const ZRef<CountedString>& iLexicalScopeName,
		const ZRef<ZYadMapRPos>& iYad);

// From ZYadMapR via ZYadMapRPos
	ZRef<ZYadR> ReadInc(string& oName);

// From ZYadMapRClone via ZYadMapRPos
	virtual ZRef<ZYadMapRClone> Clone();

// From ZYadMapRPos
	virtual void SetPosition(const string& iName);
	virtual ZRef<ZYadR> ReadAt(const string& iName);

private:
	const ZRef<CountedString> fProtoName;
	const ZRef<CountedString> fLexicalScopeName;
	ZRef<Chain> fChain;
	string fPosition;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Helpers

static ZRef<ZYadR> spWrap
	(const ZRef<CountedString>& iProtoName, const ZRef<CountedString>& iLexicalScopeName,
	const ZRef<Chain>& iChain, const ZRef<ZYadR>& iYad)
	{
	if (ZRef<ZYadSeqRPos> theYadSeqRPos = iYad.DynamicCast<ZYadSeqRPos>())
		return new YadSeqRPos(iProtoName, iLexicalScopeName, iChain, theYadSeqRPos);

	if (ZRef<ZYadMapRPos> theYadMapRPos = iYad.DynamicCast<ZYadMapRPos>())
		{
		return new YadMapRPos
			(iProtoName, iLexicalScopeName, new Chain(iChain, theYadMapRPos), string());
		}

	return iYad;
	}

// =================================================================================================
#pragma mark -
#pragma mark * Chain definition

Chain::Chain(const Chain& iOther)
:	fParent(iOther.fParent)
,	fYadMapRPos(iOther.fYadMapRPos->Clone().DynamicCast<ZYadMapRPos>())
,	fCacheByName(iOther.fCacheByName)
,	fCacheByTrail(iOther.fCacheByTrail)
	{}

Chain::Chain(const ZRef<Chain>& iParent, const ZRef<ZYadMapRPos>& iYadMapRPos)
:	fParent(iParent)
,	fYadMapRPos(iYadMapRPos)
	{
	ZAssert(not iYadMapRPos.DynamicCast<YadMapRPos>());
	}

ZRef<Chain> Chain::Clone()
	{ return new Chain(*this); }

ZRef<ZYadR> Chain::ReadInc(string& oName)
	{ return fYadMapRPos->ReadInc(oName); }

ZRef<ZYadR> Chain::pReadAt(const string& iName)
	{ return fYadMapRPos->ReadAt(iName); }

ZRef<ZYadR> Chain::ReadAt
	(const ZRef<CountedString>& iProtoName, const ZRef<CountedString>& iLexicalScopeName,
	const string& iName)
	{ return this->pReadAt(iProtoName, iLexicalScopeName, iName, false); }

ZRef<ZYadR> Chain::pReadAt
	(const ZRef<CountedString>& iProtoName, const ZRef<CountedString>& iLexicalScopeName,
	const string& iName, bool iAllowLexicalScope)
	{
	if (ZRef<ZYadR> theYad = this->pReadAt(iName))
		return spWrap(iProtoName, iLexicalScopeName, this, theYad);
	
	ZRef<Chain> theChain;
	if (ZQ<ZRef<Chain> > theByNameQ = ZUtil_STL::sQGet(fCacheByName, iName))
		{
		theChain = *theByNameQ;
		}
	else if (ZRef<ZYadStrimR> theProtoYad = this->pReadAt(iProtoName->Get()).DynamicCast<ZYadStrimR>())
		{
		const string theTrailString = theProtoYad->GetStrimR().ReadAll8();
		if (ZQ<ZRef<Chain> > theByTrailQ = ZUtil_STL::sQGet(fCacheByTrail, theTrailString))
			{
			theChain = *theByTrailQ;
			}
		else
			{
			if (theTrailString.size())
				{
				size_t index = 0;
				const ZTrail theTrail = ZTrail(theTrailString).Normalized();
				ZRef<Chain> cur = this;

				if (theTrailString[0] == '/')
					{
					// Walk up to the root.
					for (ZRef<Chain> next = null; next = cur->fParent; cur = next)
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
				ZRef<Chain> result;
				while (index < theTrail.Count())
					{
					if (ZRef<ZYadMapRPos,0> theYadMapRPos =
						cur->pReadAt(theTrail.At(index)).DynamicCast<ZYadMapRPos>())
						{ break; }
					else
						{
						cur = new Chain(cur, theYadMapRPos);
						if (++index == theTrail.Count())
							{
							theChain = cur;
							break;
							}
						}
					}
				}
			ZUtil_STL::sInsertMustNotContain(fCacheByTrail, theTrailString, theChain);
			}
		ZUtil_STL::sInsertMustNotContain(fCacheByName, iName, theChain);
		}

	if (theChain)
		{
		if (ZRef<ZYadR> theYadR =
			theChain->pReadAt(iProtoName, iLexicalScopeName, iName, iAllowLexicalScope))
			{ return theYadR; }
		}

	// Yay, lexical scoping, disabled for now.
	if (iAllowLexicalScope || (iLexicalScopeName && this->pReadAt(iLexicalScopeName->Get())))
		{
		return spWrap
			(iProtoName, iLexicalScopeName,
			fParent, fParent->pReadAt(iProtoName, iLexicalScopeName, iName, true));
		}

	return null;
	}

// =================================================================================================
#pragma mark -
#pragma mark * YadSeqRPos definition

YadSeqRPos::YadSeqRPos
	(const ZRef<CountedString>& iProtoName, const ZRef<CountedString>& iLexicalScopeName,
	const ZRef<Chain>& iChain, const ZRef<ZYadSeqRPos>& iYadSeqRPos)
:	fProtoName(iProtoName)
,	fLexicalScopeName(iLexicalScopeName)
,	fChain(iChain)
,	fYadSeqRPos(iYadSeqRPos)
	{}

ZRef<ZYadR> YadSeqRPos::ReadInc()
	{ return spWrap(fProtoName, fLexicalScopeName, fChain, fYadSeqRPos->ReadInc()); }

ZRef<ZYadSeqRClone> YadSeqRPos::Clone()
	{
	return new YadSeqRPos
		(fProtoName, fLexicalScopeName, fChain, fYadSeqRPos->Clone().DynamicCast<ZYadSeqRPos>());
	}

uint64 YadSeqRPos::GetPosition()
	{ return fYadSeqRPos->GetPosition(); }

void YadSeqRPos::SetPosition(uint64 iPosition)
	{ fYadSeqRPos->SetPosition(iPosition); }

uint64 YadSeqRPos::GetSize()
	{ return fYadSeqRPos->GetSize(); }

ZRef<ZYadR> YadSeqRPos::ReadAt(uint64 iPosition)
	{ return spWrap(fProtoName, fLexicalScopeName, fChain, fYadSeqRPos->ReadAt(iPosition)); }

// =================================================================================================
#pragma mark -
#pragma mark * YadMapRPos definition

YadMapRPos::YadMapRPos
	(const ZRef<CountedString>& iProtoName, const ZRef<CountedString>& iLexicalScopeName,
	const ZRef<Chain>& iChain, const string& iPosition)
:	fProtoName(iProtoName)
,	fLexicalScopeName(iLexicalScopeName)
,	fChain(iChain)
,	fPosition(iPosition)
	{}

YadMapRPos::YadMapRPos
	(const ZRef<CountedString>& iProtoName, const ZRef<CountedString>& iLexicalScopeName,
		const ZRef<ZYadMapRPos>& iYad)
:	fProtoName(iProtoName)
,	fLexicalScopeName(iLexicalScopeName)
,	fChain(new Chain(null, iYad))
	{}

ZRef<ZYadR> YadMapRPos::ReadInc(string& oName)
	{
	if (fChain->IsShared())
		fChain = fChain->Clone();

	if (fPosition.empty())
		return fChain->ReadInc(oName);
	
	oName = fPosition;
	return fChain->ReadAt(fProtoName, fLexicalScopeName, sGetSet(fPosition, string()));
	}

ZRef<ZYadR> YadMapRPos::ReadAt(const string& iName)
	{
	if (fChain->IsShared())
		fChain = fChain->Clone();
	fPosition.clear();
	return fChain->ReadAt(fProtoName, fLexicalScopeName, iName);
	}

ZRef<ZYadMapRClone> YadMapRPos::Clone()
	{ return new YadMapRPos(fProtoName, fLexicalScopeName, fChain->Clone(), fPosition); }

void YadMapRPos::SetPosition(const string& iName)
	{ fPosition = iName; }

} // namespace YadTree

// =================================================================================================
#pragma mark -
#pragma mark * YadMapRPos definition

ZRef<ZYadMapRPos> sYadTree(const ZRef<ZYadMapRPos>& iYadMapRPos,
	const string& iProtoName,
	const string& iLexicalScopeName)
	{
	return new YadTree::YadMapRPos(new YadTree::CountedString(iProtoName),
		new YadTree::CountedString(iLexicalScopeName),
		iYadMapRPos);
	}

ZRef<ZYadMapRPos> sYadTree(const ZRef<ZYadMapRPos>& iYadMapRPos)
	{ return sYadTree(iYadMapRPos, "_", "__"); }

} // namespace ZooLib
