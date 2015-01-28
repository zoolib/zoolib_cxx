/* -------------------------------------------------------------------------------------------------
Copyright (c) 2006 Andrew Green and Learning in Motion, Inc.
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

#ifndef __ZooLib_Chan_Bin_ASCIIStrim_h__
#define __ZooLib_Chan_Bin_ASCIIStrim_h__ 1
#include "zconfig.h"

#include "zoolib/ChanR_Bin.h"
#include "zoolib/ChanW_Bin.h"
#include "zoolib/ChanR_UTF.h"
#include "zoolib/ChanW_UTF.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark ChanR_Bin_ASCIIStrim

/// A read filter stream that reads only the ASCII-range code points from a strim.

class ChanR_Bin_ASCIIStrim
:	public ChanR_Bin
	{
public:
	ChanR_Bin_ASCIIStrim(const ChanR_UTF& iChanR);
	virtual size_t QRead(byte* oDest, size_t iCount);

private:
	const ChanR_UTF& fChanR_UTF;
	};

// =================================================================================================
#pragma mark -
#pragma mark ChanW_Bin_ASCIIStrim

/// A write filter stream that writes only the ASCII-range bytes to a strim.

class ChanW_Bin_ASCIIStrim
:	public ChanW_Bin
	{
public:
	ChanW_Bin_ASCIIStrim(const ChanW_UTF& iChanW);
	virtual size_t QWrite(const byte* iSource, size_t iCount);

private:
	const ChanW_UTF& fChanW_UTF;
	};

} // namespace ZooLib

#endif // __ZooLib_Chan_Bin_ASCIIStrim_h__
