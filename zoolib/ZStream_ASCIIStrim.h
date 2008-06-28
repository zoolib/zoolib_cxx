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

#ifndef __ZStream_ASCIIStrim__
#define __ZStream_ASCIIStrim__ 1
#include "zconfig.h"

#include "zoolib/ZStream.h"

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamR_ASCIIStrim

class ZStrimR;

/// A read filter stream that reads only the ASCII-range code points from a strim.

class ZStreamR_ASCIIStrim : public ZStreamR
	{
public:
	ZStreamR_ASCIIStrim(const ZStrimR& iStrimR);
	virtual void Imp_Read(void* iDest, size_t iCount, size_t* oCountRead);

private:
	const ZStrimR& fStrimR;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamW_ASCIIStrim

class ZStrimW;

/// A write filter stream that writes only the ASCII-range bytes to a strim.

class ZStreamW_ASCIIStrim : public ZStreamW
	{
public:
	ZStreamW_ASCIIStrim(const ZStrimW& iStrimW);
	virtual void Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten);

private:
	const ZStrimW& fStrimW;
	};

#endif // __ZStream_ASCIIStrim__
