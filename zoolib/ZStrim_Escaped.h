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

#ifndef __ZStrim_Escaped__
#define __ZStrim_Escaped__ 1
#include "zconfig.h"

#include "zoolib/ZStrim.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * ZStrimR_Escaped

class ZStrimR_Escaped : public ZStrimR
	{
public:
	ZStrimR_Escaped(const ZStrimU& iStrimSource, UTF32 iDelimiter);
	~ZStrimR_Escaped();

// From ZStrimR
	virtual void Imp_ReadUTF32(UTF32* oDest, size_t iCount, size_t* oCount);

protected:
	const ZStrimU& fStrimSource;
	UTF32 fDelimiter;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZStrimW_Escaped

/// A write filter strim that inserts C-style escape sequences.

class ZStrimW_Escaped : public ZStrimW_NativeUTF32
	{
public:
	struct Options
		{
		Options();

		string8 fEOL;
		bool fQuoteQuotes;
		bool fEscapeHighUnicode;
		};

	ZStrimW_Escaped(const Options& iOptions, const ZStrimW& iStrimSink);
	ZStrimW_Escaped(const ZStrimW& iStrimSink);
	~ZStrimW_Escaped();

// From ZStrimW via ZStrimW_NativeUTF32
	virtual void Imp_WriteUTF32(const UTF32* iSource, size_t iCountCU, size_t* oCountCU);

private:
	const ZStrimW& fStrimSink;
	string8 fEOL;
	bool fQuoteQuotes;
	bool fEscapeHighUnicode;
	bool fLastWasCR;
	};

// =================================================================================================

} // namespace ZooLib

#endif // __ZStrim_Escaped__
