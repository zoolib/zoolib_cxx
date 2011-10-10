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

#include "zoolib/ZGetSet.h"
#include "zoolib/ZTrail.h"
#include "zoolib/ZYadTree.h"
#include "zoolib/ZCountedVal.h"

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
public:
	Chain
		(const ZRef<Chain>& iParent, const ZRef<ZYadMapRPos>& iYadMapRPos);

	ZRef<Chain> Clone();

	ZRef<ZYadR> ReadInc(string& oName);
	ZRef<ZYadR> ReadAt(const ZRef<CountedString>& iProto, const string& iName);

private:
	const ZRef<Chain> fParent;
	ZRef<ZYadMapRPos> fYadMapRPos;
	};

// =================================================================================================
#pragma mark -
#pragma mark * YadSeqRPos declaration

class YadSeqRPos
:	public ZYadSeqRPos
	{
public:
	YadSeqRPos(const ZRef<CountedString>& iProto,
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
	const ZRef<CountedString> fProto;
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
	YadMapRPos(const ZRef<CountedString>& iProto,
		const ZRef<Chain>& iChain, const string& iPosition);

	YadMapRPos(const ZRef<CountedString>& iProto, const ZRef<ZYadMapRPos>& iYad);

// From ZYadMapR via ZYadMapRPos
	ZRef<ZYadR> ReadInc(string& oName);

// From ZYadMapRClone via ZYadMapRPos
	virtual ZRef<ZYadMapRClone> Clone();

// From ZYadMapRPos
	virtual void SetPosition(const string& iName);
	virtual ZRef<ZYadR> ReadAt(const string& iName);

private:
	const ZRef<CountedString> fProto;
	ZRef<Chain> fChain;
	string fPosition;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Helpers

static ZRef<ZYadR> spWrap(const ZRef<CountedString>& iProto,
	const ZRef<Chain>& iChain, const ZRef<ZYadR>& iYad)
	{
	if (ZRef<ZYadSeqRPos> theYadSeqRPos = iYad.DynamicCast<ZYadSeqRPos>())
		return new YadSeqRPos(iProto, iChain, theYadSeqRPos);

	if (ZRef<ZYadMapRPos> theYadMapRPos = iYad.DynamicCast<ZYadMapRPos>())
		return new YadMapRPos(iProto, new Chain(iChain, theYadMapRPos), string());

	return iYad;
	}

// =================================================================================================
#pragma mark -
#pragma mark * Chain definition

Chain::Chain(const ZRef<Chain>& iParent, const ZRef<ZYadMapRPos>& iYadMapRPos)
:	fParent(iParent)
,	fYadMapRPos(iYadMapRPos)
	{}

ZRef<Chain> Chain::Clone()
	{ return new Chain(fParent, fYadMapRPos->Clone().DynamicCast<ZYadMapRPos>()); }

ZRef<ZYadR> Chain::ReadInc(string& oName)
	{ return fYadMapRPos->ReadInc(oName); }

ZRef<ZYadR> Chain::ReadAt(const ZRef<CountedString>& iProto, const string& iName)
	{
	if (ZRef<ZYadR> theYad = fYadMapRPos->ReadAt(iName))
		return spWrap(iProto, this, theYad);
	
	if (ZRef<ZYadStrimR> theProtoYad = fYadMapRPos->ReadAt(iProto->Get()).DynamicCast<ZYadStrimR>())
		{
		const string theTrailString = theProtoYad->GetStrimR().ReadAll8();
		if (theTrailString.size())
			{
			size_t index = 0;
			const ZTrail theTrail = ZTrail(theTrailString).Normalized();
			ZRef<Chain> cur = this;
			if (theTrailString[0] == '/')
				{
				// Walk up to the root.
				for (;;)
					{
					if (ZRef<Chain> next = cur->fParent)
						cur = next;
					else
						break;
					}
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
			while (index < theTrail.Count())
				{
				if (ZRef<ZYadMapRPos,false> theYadMapRPos =
					cur->ReadAt(iProto, theTrail.At(index)).DynamicCast<ZYadMapRPos>())
					{ break; }
				else
					{
					cur = new Chain(cur, theYadMapRPos);
					if (++index == theTrail.Count())
						return spWrap(iProto, cur, theYadMapRPos->ReadAt(iName));
					}
				}
			}
		}
	return null;
	}

class Chain;


// =================================================================================================
#pragma mark -
#pragma mark * YadSeqRPos definition

YadSeqRPos::YadSeqRPos(const ZRef<CountedString>& iProto,
	const ZRef<Chain>& iChain, const ZRef<ZYadSeqRPos>& iYadSeqRPos)
:	fProto(iProto)
,	fChain(iChain)
,	fYadSeqRPos(iYadSeqRPos)
	{}

ZRef<ZYadR> YadSeqRPos::ReadInc()
	{ return spWrap(fProto, fChain, fYadSeqRPos->ReadInc()); }

ZRef<ZYadSeqRClone> YadSeqRPos::Clone()
	{ return new YadSeqRPos(fProto, fChain, fYadSeqRPos->Clone().DynamicCast<ZYadSeqRPos>()); }

uint64 YadSeqRPos::GetPosition()
	{ return fYadSeqRPos->GetPosition(); }

void YadSeqRPos::SetPosition(uint64 iPosition)
	{ fYadSeqRPos->SetPosition(iPosition); }

uint64 YadSeqRPos::GetSize()
	{ return fYadSeqRPos->GetSize(); }

ZRef<ZYadR> YadSeqRPos::ReadAt(uint64 iPosition)
	{ return spWrap(fProto, fChain, fYadSeqRPos->ReadAt(iPosition)); }

// =================================================================================================
#pragma mark -
#pragma mark * YadMapRPos definition

YadMapRPos::YadMapRPos(const ZRef<CountedString>& iProto,
	const ZRef<Chain>& iChain, const string& iPosition)
:	fProto(iProto)
,	fChain(iChain)
,	fPosition(iPosition)
	{}

YadMapRPos::YadMapRPos(const ZRef<CountedString>& iProto, const ZRef<ZYadMapRPos>& iYad)
:	fProto(iProto)
,	fChain(new Chain(null, iYad))
	{}

ZRef<ZYadR> YadMapRPos::ReadInc(string& oName)
	{
	if (fChain->IsShared())
		fChain = fChain->Clone();

	if (fPosition.empty())
		return fChain->ReadInc(oName);
	
	oName = fPosition;
	return fChain->ReadAt(fProto, sGetSet(fPosition, string()));
	}

ZRef<ZYadR> YadMapRPos::ReadAt(const string& iName)
	{
	if (fChain->IsShared())
		fChain = fChain->Clone();
	fPosition.clear();
	return fChain->ReadAt(fProto, iName);
	}

ZRef<ZYadMapRClone> YadMapRPos::Clone()
	{ return new YadMapRPos(fProto, fChain->Clone(), fPosition); }

void YadMapRPos::SetPosition(const string& iName)
	{ fPosition = iName; }

} // namespace YadTree

// =================================================================================================
#pragma mark -
#pragma mark * YadMapRPos definition

ZRef<ZYadMapRPos> sYadTree(const ZRef<ZYadMapRPos>& iYadMapRPos, const string& iProtoName)
	{ return new YadTree::YadMapRPos(new YadTree::CountedString(iProtoName), iYadMapRPos); }

ZRef<ZYadMapRPos> sYadTree(const ZRef<ZYadMapRPos>& iYadMapRPos)
	{ return sYadTree(iYadMapRPos, "_"); }

} // namespace ZooLib
