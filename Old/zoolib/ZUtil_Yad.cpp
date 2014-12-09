/* -------------------------------------------------------------------------------------------------
Copyright (c) 2008 Andrew Green
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

#include "zoolib/Util_string.h"

#include "zoolib/ZUtil_Yad.h"

namespace ZooLib {

using std::string;

// =================================================================================================
// MARK: - Static helpers

static ZRef<ZYadR> spGetChild(ZRef<ZYadR> iYadR, const string& iName)
	{
	if (ZRef<ZYadMapR> theYadMapR = iYadR.DynamicCast<ZYadMapR>())
		{
		if (ZRef<ZYadMapAtR> theYadMapAtR = iYadR.DynamicCast<ZYadMapAtR>())
			{
			return theYadMapAtR->ReadAt(iName);
			}
		else if (ZRef<ZYadMapRPos> theYadMapRPos = iYadR.DynamicCast<ZYadMapRPos>())
			{
			theYadMapRPos->SetPosition(iName);
			Name dummy;
			return theYadMapRPos->ReadInc(dummy);
			}
		else for (;;)
			{
			Name theName;
			if (ZRef<ZYadR,false> cur = theYadMapR->ReadInc(theName))
				break;
			else if (theName == iName)
				return cur;
			}
		}
	else if (ZRef<ZYadSeqR> theYadSeqR = iYadR.DynamicCast<ZYadSeqR>())
		{
		if (ZQ<int64> theQ = Util_string::sQInt64(iName))
			{
			int64 theIntIndex = *theQ;
			if (theIntIndex >= 0)
				{
				if (ZRef<ZYadSeqAtR> theYadSeqAtR = iYadR.DynamicCast<ZYadSeqAtR>())
					{
					return theYadSeqAtR->ReadAt(theIntIndex);
					}
				else if (ZRef<ZYadSeqRPos> theYadSeqRPos = iYadR.DynamicCast<ZYadSeqRPos>())
					{
					theYadSeqRPos->SetPosition(theIntIndex);
					return theYadSeqRPos->ReadInc();
					}
				else for (;;)
					{
					if (ZRef<ZYadR,false> cur = theYadSeqR->ReadInc())
						break;
					else if (0 == theIntIndex--)
						return cur;
					}
				}
			}
		}
	return null;
	}

// =================================================================================================
// MARK: - ZUtil_Yad

ZRef<ZYadR> ZUtil_Yad::sWalk(ZRef<ZYadR> iYadR, const ZTrail& iTrail)
	{
	const ZTrail theTrail = iTrail.Normalized();

	for (size_t x = 0, count = theTrail.Count(); iYadR && x < count; ++x)
		iYadR = spGetChild(iYadR, theTrail.At(x));

	return iYadR;
	}

} // namespace ZooLib
