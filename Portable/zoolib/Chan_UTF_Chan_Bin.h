/* -------------------------------------------------------------------------------------------------
Copyright (c) 2003 Andrew Green and Learning in Motion, Inc.
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

#ifndef __ZooLib_Chan_UTF_Chan_Bin_h__
#define __ZooLib_Chan_UTF_Chan_Bin_h__ 1

#include "zoolib/ChanR_Bin.h"
#include "zoolib/ChanR_UTF.h"
#include "zoolib/ChanW_Bin.h"
#include "zoolib/ChanW_UTF.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark ChanR_UTF_Chan_Bin_UTF8

/// A read strim that sources text by reading UTF-8 code units from a ZStreamR.

class ChanR_UTF_Chan_Bin_UTF8
:	public ChanR_UTF
	{
public:
	ChanR_UTF_Chan_Bin_UTF8(const ChanR_Bin& iChanR_Bin);

// From ChanR_UTF
	virtual size_t Read(UTF32* oDest, size_t iCount);

private:
	const ChanR_Bin& fChanR_Bin;
	};

// =================================================================================================
#pragma mark -
#pragma mark ChanW_UTF_Chan_Bin_UTF8

/// A write strim that writes text to a ZStreamW in UTF-8.

class ChanW_UTF_Chan_Bin_UTF8
:	public ChanW_UTF_Native8
	{
public:
	ChanW_UTF_Chan_Bin_UTF8(const ChanW_Bin& iChanW_Bin);

// From ChanW_UTF
	virtual void Flush();

// From ChanW_UTF_Native8
	virtual size_t WriteUTF8(const UTF8* iSource, size_t iCountCU);

private:
	const ChanW_Bin& fChanW_Bin;
	};

} // namespace ZooLib

#endif // __ZooLib_Chan_UTF_Chan_Bin_h__
