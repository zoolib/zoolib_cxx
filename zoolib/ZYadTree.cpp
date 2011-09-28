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

namespace ZooLib {
namespace YadTree {

using std::string;

// =================================================================================================
#pragma mark -
#pragma mark * Chain

class Chain
:	public ZCounted
	{
public:
	Chain(const ZRef<Chain>& iParent, const ZRef<ZYadMapRPos>& iYadMapRPos);

	ZRef<Chain> Clone();

	ZRef<ZYadR> ReadInc(string8& oName);
	ZRef<ZYadR> ReadAt(const string8& iName);

private:
	const ZRef<Chain> fParent;
	ZRef<ZYadMapRPos> fYadMapRPos;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Helpers

static ZRef<ZYadR> spWrap(ZRef<Chain> iChain, ZRef<ZYadR> iYad)
	{
	if (ZRef<ZYadSeqRPos> theYadSeqRPos = iYad.DynamicCast<ZYadSeqRPos>())
		return new YadSeqRPos(iChain, theYadSeqRPos);

	if (ZRef<ZYadMapRPos> theYadMapRPos = iYad.DynamicCast<ZYadMapRPos>())
		return new YadMapRPos(new Chain(iChain, theYadMapRPos), string());

	return iYad;
	}

// =================================================================================================
#pragma mark -
#pragma mark * Chain

Chain::Chain(const ZRef<Chain>& iParent, const ZRef<ZYadMapRPos>& iYadMapRPos)
:	fParent(iParent)
,	fYadMapRPos(iYadMapRPos)
	{}

ZRef<Chain> Chain::Clone()
	{ return new Chain(fParent, fYadMapRPos->Clone().DynamicCast<ZYadMapRPos>()); }

ZRef<ZYadR> Chain::ReadInc(string& oName)
	{ return fYadMapRPos->ReadInc(oName); }

ZRef<ZYadR> Chain::ReadAt(const string& iName)
	{
	if (ZRef<ZYadR> theYad = fYadMapRPos->ReadAt(iName))
		return spWrap(this, theYad);
	
	if (ZRef<ZYadStrimR,false> theProtoYad = fYadMapRPos->ReadAt("_proto").DynamicCast<ZYadStrimR>())
		{}
	else
		{
		const string8 theTrailString = theProtoYad->GetStrimR().ReadAll8();
		if (theTrailString.empty())
			{}
		else
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
					cur->ReadAt(theTrail.At(index)).DynamicCast<ZYadMapRPos>())
					{ break; }
				else
					{
					cur = new Chain(cur, theYadMapRPos);
					if (++index == theTrail.Count())
						return spWrap(cur, theYadMapRPos->ReadAt(iName));
					}
				}
			}
		}
	return null;
	}

// =================================================================================================
#pragma mark -
#pragma mark * YadSeqRPos

YadSeqRPos::YadSeqRPos(const ZRef<Chain>& iChain, const ZRef<ZYadSeqRPos>& iYadSeqRPos)
:	fChain(iChain)
,	fYadSeqRPos(iYadSeqRPos)
	{}

ZRef<ZYadR> YadSeqRPos::ReadInc()
	{ return spWrap(fChain, fYadSeqRPos->ReadInc()); }

ZRef<ZYadSeqRClone> YadSeqRPos::Clone()
	{ return new YadSeqRPos(fChain, fYadSeqRPos->Clone().DynamicCast<ZYadSeqRPos>()); }

uint64 YadSeqRPos::GetPosition()
	{ return fYadSeqRPos->GetPosition(); }

void YadSeqRPos::SetPosition(uint64 iPosition)
	{ fYadSeqRPos->SetPosition(iPosition); }

uint64 YadSeqRPos::GetSize()
	{ return fYadSeqRPos->GetSize(); }

// =================================================================================================
#pragma mark -
#pragma mark * YadMapRPos

YadMapRPos::YadMapRPos(const ZRef<Chain>& iChain, const string& iPosition)
:	fChain(iChain)
,	fPosition(iPosition)
	{}

YadMapRPos::YadMapRPos(const ZRef<ZYadMapRPos>& iYad)
:	fChain(new Chain(null, iYad))
	{}

ZRef<ZYadR> YadMapRPos::ReadInc(std::string& oName)
	{
	if (fChain->IsShared())
		fChain = fChain->Clone();

	if (fPosition.empty())
		return fChain->ReadInc(oName);
	
	oName = fPosition;
	return fChain->ReadAt(sGetSet(fPosition, string()));
	}

ZRef<ZYadMapRClone> YadMapRPos::Clone()
	{ return new YadMapRPos(fChain->Clone(), fPosition); }

void YadMapRPos::SetPosition(const std::string& iName)
	{ fPosition = iName; }

} // namespace YadTree
} // namespace ZooLib
