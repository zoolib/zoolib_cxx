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

#ifndef __ZStrim_CRLF__
#define __ZStrim_CRLF__ 1
#include "zconfig.h"

#include "zoolib/ZStrim.h"

// =================================================================================================
#pragma mark -
#pragma mark * ZStrimR_CRLFRemove

/// A read filter strim that removes CRLF sequences and replaces them with another code point.

class ZStrimR_CRLFRemove : public ZStrimR
	{
public:
	ZStrimR_CRLFRemove(const ZStrimR& iStrimSource);
	ZStrimR_CRLFRemove(UTF32 iReplaceCP, const ZStrimR& iStrimSource);

// From ZStrimR
	virtual void Imp_ReadUTF32(UTF32* iDest, size_t iCount, size_t* oCount);

private:
	const ZStrimR& fStrimSource;
	UTF32 fReplaceCP;
	bool fLastWasCR;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZStrimW_CRLFRemove

/// A write filter strim that removes CRLF sequences and replaces them with another code point.

class ZStrimW_CRLFRemove : public ZStrimW_NativeUTF32
	{
public:
	ZStrimW_CRLFRemove(const ZStrimW& iStrimSink);
	ZStrimW_CRLFRemove(UTF32 iReplaceCP, const ZStrimW& iStrimSink);

	virtual void Imp_WriteUTF32(const UTF32* iSource, size_t iCountCU, size_t* oCountCU);

private:
	const ZStrimW& fStrimSink;
	UTF32 fReplaceCP;
	bool fLastWasCR;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZStrimW_CRLFInsert

/// A write filter strim that replaces standalone CR or LF with CRLF.

class ZStrimW_CRLFInsert : public ZStrimW_NativeUTF32
	{
public:
	ZStrimW_CRLFInsert(const ZStrimW& iStrimSink);

	virtual void Imp_WriteUTF32(const UTF32* iSource, size_t iCountCU, size_t* oCountCU);

private:
	const ZStrimW& fStrimSink;
	bool fLastWasCR;
	};


#endif // __ZStrim_CRLF__
