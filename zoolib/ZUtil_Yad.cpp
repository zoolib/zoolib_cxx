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

#include "zoolib/ZUtil_Yad.h"
#include "zoolib/ZString.h"

#include <stdio.h> // For sscanf

NAMESPACE_ZOOLIB_BEGIN

using std::min;
using std::string;
using std::vector;

// =================================================================================================
#pragma mark -
#pragma mark * Static helpers

static bool sAsInt(const string& iString, int& oInt)
	{
	if (iString.size())
		{
		if (sscanf(iString.c_str(), "%d", &oInt) > 0)
			return true;
		}
	return false;
	}

static ZRef<ZYadR> sGetChild(ZRef<ZYadR> iYadR, const string& iName)
	{
	if (ZRef<ZYadMapR> theYadMapR = ZRefDynamicCast<ZYadMapR>(iYadR))
		{
		if (ZRef<ZYadMapRPos> theYadMapRPos = ZRefDynamicCast<ZYadMapRPos>(iYadR))
			{
			theYadMapRPos->SetPosition(iName);
			string dummy;
			return theYadMapRPos->ReadInc(dummy);
			}
		else
			{
			for (;;)
				{
				string theName;
				if (ZRef<ZYadR> cur = theYadMapR->ReadInc(theName))
					{
					if (theName == iName)
						return cur;
					}
				else
					{
					break;
					}
				}
			}
		}
	else if (ZRef<ZYadListR> theYadListR = ZRefDynamicCast<ZYadListR>(iYadR))
		{
		int64 theIntIndex;
		if (ZString::sInt64Q(iName, theIntIndex) && theIntIndex >= 0)
			{
			if (ZRef<ZYadListRPos> theYadListPosR = ZRefDynamicCast<ZYadListRPos>(iYadR))
				{
				theYadListPosR->SetPosition(theIntIndex);
				return theYadListPosR->ReadInc();
				}
			else
				{
				for (;;)
					{
					if (ZRef<ZYadR> cur = theYadListR->ReadInc())
						{
						if (0 == theIntIndex--)
							return cur;
						}
					else
						{
						break;
						}
					}
				}
			}
		}
	return ZRef<ZYadR>();
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZUtil_Yad

ZRef<ZYadR> ZUtil_Yad::sWalk(ZRef<ZYadR> iYadR, const ZTrail& iTrail)
	{
	const ZTrail theTrail = iTrail.Normalized();
	const vector<string>& theComps = theTrail.GetComps();

	for (vector<string>::const_iterator i = theComps.begin();
		iYadR && i != theComps.end(); ++i)
		{
		iYadR = sGetChild(iYadR, *i);
		}

	return iYadR;
	}

NAMESPACE_ZOOLIB_END
