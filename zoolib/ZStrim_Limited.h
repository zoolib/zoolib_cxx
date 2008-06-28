/* -------------------------------------------------------------------------------------------------
Copyright (c) 2004 Andrew Green and Learning in Motion, Inc.
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

#ifndef __ZStrim_Limited__
#define __ZStrim_Limited__ 1
#include "zconfig.h"

#include "zoolib/ZStrim.h"

// =================================================================================================
#pragma mark -
#pragma mark * ZStrimR_Limited

/// A read filter strim that limits the number of code points read through it.

class ZStrimR_Limited : public ZStrimR
	{
public:
	ZStrimR_Limited(size_t iLimitCP, const ZStrimR& iSource);

// From ZStrimR
	virtual void Imp_ReadUTF32(UTF32* iDest, size_t iCount, size_t* oCount);

	virtual void Imp_CopyToDispatch(const ZStrimW& iStrimW, size_t iCountCP,
		size_t* oCountCPRead, size_t* oCountCPWritten);

	virtual void Imp_CopyTo(const ZStrimW& iStrimW, size_t iCountCP,
		size_t* oCountCPRead, size_t* oCountCPWritten);

	virtual void Imp_Skip(size_t iCountCP, size_t* oCountCPSkipped);

private:
	size_t fRemainingCP;
	const ZStrimR& fSource;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZStrimW_Limited

/// A write filter strim that limits the number of code points it will pass on.

class ZStrimW_Limited : public ZStrimW_NativeUTF32
	{
public:
	ZStrimW_Limited(size_t iLimitCP, const ZStrimW& iSink);

// From ZStrimW via ZStrimW_NativeUTF32
	virtual void Imp_WriteUTF32(const UTF32* iSource, size_t iCountCU, size_t* oCountCU);
	virtual void Imp_Flush();

private:
	size_t fRemainingCP;
	const ZStrimW& fSink;
	};

#endif // __ZStrim_Limited__
