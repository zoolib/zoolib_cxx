/* -------------------------------------------------------------------------------------------------
Copyright (c) 2001 Andrew Green and Learning in Motion, Inc.
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

#ifndef __ZStreamRPos_StreamR__
#define __ZStreamRPos_StreamR__ 1
#include "zconfig.h"

#include "zoolib/ZStream.h"

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamRPos_StreamR

/** Puts a ZStreamRPos interface over a ZStreamR. */

class ZStreamRPos_StreamR : public ZStreamRPos
	{
public:
	ZStreamRPos_StreamR(const ZStreamR& iSource, const ZStreamRWPos& iBuffer);
	~ZStreamRPos_StreamR();

// From ZStreamR via ZStreamRPos
	virtual void Imp_Read(void* iDest, size_t iCount, size_t* oCountRead);

// From ZStreamRPos
	virtual size_t Imp_CountReadable();
	virtual uint64 Imp_GetPosition();
	virtual void Imp_SetPosition(uint64 iPosition);
	virtual uint64 Imp_GetSize();

// Our protocol
	const ZStreamR& Committed();

protected:
	const ZStreamR& fSource;
	const ZStreamRWPos& fBuffer;
	bool fCommitted;
	};

NAMESPACE_ZOOLIB_END

#endif // __ZStreamRPos_StreamR__
