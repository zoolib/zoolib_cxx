/* -------------------------------------------------------------------------------------------------
Copyright (c) 2007 Andrew Green
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

#ifndef __ZStrimU_StreamUTF8Buffered__
#define __ZStrimU_StreamUTF8Buffered__ 1
#include "zconfig.h"

#include "ZStream.h"
#include "ZStrimmer.h"

#include <string>

// =================================================================================================
#pragma mark -
#pragma mark * ZStrimU_StreamUTF8Buffered

class ZStrimU_StreamUTF8Buffered : public ZStrimU
	{
public:
	ZStrimU_StreamUTF8Buffered(size_t iBufferSize, const ZStreamR& iStreamR);
	~ZStrimU_StreamUTF8Buffered();

// From ZStrimR via ZStrimU
	virtual void Imp_ReadUTF32(UTF32* iDest, size_t iCount, size_t* oCount);

// From ZStrimU
	virtual void Imp_Unread();

protected:
	const ZStreamR& fStreamR;
	std::vector<UTF32> fBuffer;
	size_t fFeedIn;
	size_t fFeedOut;
	};

#if 0
// =================================================================================================
#pragma mark -
#pragma mark * ZStrimerR_Boundary

/// A read filter strimmer encapsulating a ZStrimU_StreamUTF8Buffered.

class ZStrimmerR_Boundary : public ZStrimmerR
	{
public:
	ZStrimmerR_Boundary(const string8& iBoundary, ZRef<ZStrimmerR> iStrimmerSource);

	ZStrimmerR_Boundary(
		const UTF8* iBoundary, size_t iBoundarySize, ZRef<ZStrimmerR> iStrimmerSource);

	ZStrimmerR_Boundary(const string32& iBoundary, ZRef<ZStrimmerR> iStrimmerSource);

	ZStrimmerR_Boundary(
		const UTF32* iBoundary, size_t iBoundarySize, ZRef<ZStrimmerR> iStrimmerSource);

	virtual ~ZStrimmerR_Boundary();

// From ZStrimmerR
	virtual const ZStrimR& GetStrimR();

protected:
	ZRef<ZStrimmerR> fStrimmerSource;
	ZStrimU_StreamUTF8Buffered fStrim;
	};

// typedef ZStrimmerRFT<ZStrimR_Boundary> ZStrimmerR_Boundary;

// =================================================================================================
#endif
#endif // __ZStrimU_StreamUTF8Buffered__
