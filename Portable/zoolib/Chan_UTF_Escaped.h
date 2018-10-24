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

#ifndef __ZooLib_Chan_UTF_Escaped_h__
#define __ZooLib_Chan_UTF_Escaped_h__ 1
#include "zconfig.h"

#include "zoolib/ChanR_UTF.h"
#include "zoolib/ChanU_UTF.h"
#include "zoolib/ChanW_UTF.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark ChanR_UTF_Escaped

class ChanR_UTF_Escaped
:	public ChanR_UTF
	{
public:
	ChanR_UTF_Escaped(UTF32 iDelimiter, const ChanR_UTF& iChanR, const ChanU_UTF& iChanU);
	~ChanR_UTF_Escaped();

// From ChanR_UTF
	virtual size_t Read(UTF32* oDest, size_t iCountCU);

protected:
	const UTF32 fDelimiter;
	const ChanR_UTF& fChanR;
	const ChanU_UTF& fChanU;
	};

// =================================================================================================
#pragma mark -
#pragma mark ChanW_UTF_Escaped

/// A write filter strim that inserts C-style escape sequences.

class ChanW_UTF_Escaped
:	public ChanW_UTF_Native32
	{
public:
	struct Options
		{
		Options();

		string8 fEOL;
		bool fQuoteQuotes;
		bool fEscapeHighUnicode;
		};

	ChanW_UTF_Escaped(const Options& iOptions, const ChanW_UTF& iChanW);
	ChanW_UTF_Escaped(const ChanW_UTF& iChanW);
	~ChanW_UTF_Escaped();

// From ChanW_UTF_Native32
	virtual size_t Write(const UTF32* iSource, size_t iCountCU);

private:
	const ChanW_UTF& fChanW;
	string8 fEOL;
	bool fQuoteQuotes;
	bool fEscapeHighUnicode;
	bool fLastWasCR;
	};

// =================================================================================================

} // namespace ZooLib

#endif // __ZooLib_Chan_UTF_Escaped_h__
