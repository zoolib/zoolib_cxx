/* -------------------------------------------------------------------------------------------------
Copyright (c) 2009 Andrew Green
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

#include "zoolib/ZStreamR_HexStrim.h"
#include "zoolib/ZStrim.h"
#include "zoolib/ZUtil_Strim.h"

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamR_HexStrim

ZStreamR_HexStrim::ZStreamR_HexStrim(const ZStrimU& iStrimU)
:	fStrimU(iStrimU),
	fAllowUnderscore(false)
	{}

ZStreamR_HexStrim::ZStreamR_HexStrim(bool iAllowUnderscore, const ZStrimU& iStrimU)
:	fStrimU(iStrimU),
	fAllowUnderscore(iAllowUnderscore)
	{}

ZStreamR_HexStrim::~ZStreamR_HexStrim()
	{}

void ZStreamR_HexStrim::Imp_Read(void* iDest, size_t iCount, size_t* oCountRead)
	{
	using namespace ZUtil_Strim;

	uint8* localDest = reinterpret_cast<uint8*>(iDest);

	while (iCount)
		{
		sSkip_WSAndCPlusPlusComments(fStrimU);
		int firstDigit;
		if (!sTryRead_HexDigit(fStrimU, firstDigit))
			{
			if (!fAllowUnderscore || !sTryRead_CP(fStrimU, '_'))
				break;
			firstDigit = 0;
			}

		sSkip_WSAndCPlusPlusComments(fStrimU);
		int secondDigit;
		if (!sTryRead_HexDigit(fStrimU, secondDigit))
			{
			if (!fAllowUnderscore || !sTryRead_CP(fStrimU, '_'))
				{
				throw ParseException("Could not read second nibble of byte");
				break;
				}
			secondDigit = 0;
			}

		*localDest++ = firstDigit * 16 + secondDigit;
		--iCount;
		}

	if (oCountRead)
		*oCountRead = localDest - reinterpret_cast<uint8*>(iDest);
	}

NAMESPACE_ZOOLIB_END
