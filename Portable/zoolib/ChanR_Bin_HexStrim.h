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

#ifndef __ZooLib_ChanR_Bin_HexStrim_h__
#define __ZooLib_ChanR_Bin_HexStrim_h__ 1
#include "zconfig.h"

#include "zoolib/ChanR_Bin.h"
#include "zoolib/ChanRU_UTF.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark ChanR_Bin_HexStrim

/// A read filter stream that reads byte values from a strim, where they're encoded as hex digits.

class ChanR_Bin_HexStrim
:	public ChanR_Bin
	{
public:
	ChanR_Bin_HexStrim(const ChanRU_UTF& iChanRU);
	ChanR_Bin_HexStrim(bool iAllowUnderscore, const ChanRU_UTF& iChanRU);
	ChanR_Bin_HexStrim(const ChanR_UTF& iChanR, const ChanU_UTF& iChanU);
	ChanR_Bin_HexStrim(bool iAllowUnderscore, const ChanR_UTF& iChanR, const ChanU_UTF& iChanU);
	~ChanR_Bin_HexStrim();

	virtual size_t Read(byte* oDest, size_t iCount);

private:
	const ChanR_UTF& fChanR;
	const ChanU_UTF& fChanU;
	bool fAllowUnderscore;
	};

} // namespace ZooLib

#endif // __ZooLib_ChanR_Bin_HexStrim_h__
