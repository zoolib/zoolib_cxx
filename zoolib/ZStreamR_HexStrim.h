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

#ifndef __ZStreamR_HexStrim_h__
#define __ZStreamR_HexStrim_h__ 1
#include "zconfig.h"

#include "zoolib/ZStream.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamR_HexStrim

class ZStrimU;

/// A read filter stream that reads byte values from a strim, where they're encoded as hex digits.

class ZStreamR_HexStrim : public ZStreamR
	{
public:
	ZStreamR_HexStrim(const ZStrimU& iStrimU);
	ZStreamR_HexStrim(bool iAllowUnderscore, const ZStrimU& iStrimU);

	virtual ~ZStreamR_HexStrim();
	virtual void Imp_Read(void* oDest, size_t iCount, size_t* oCountRead);

private:
	const ZStrimU& fStrimU;
	bool fAllowUnderscore;
	};

} // namespace ZooLib

#endif // __ZStream_HexStrim_h__
