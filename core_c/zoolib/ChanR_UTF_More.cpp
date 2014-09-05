/* -------------------------------------------------------------------------------------------------
Copyright (c) 2014 Andrew Green
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

#include "zoolib/ChanR_UTF_More.h"

namespace ZooLib {

// =================================================================================================
// MARK: - ChanR_UTF_Native32

void ChanR_UTF_Native32::ReadUTF16(UTF16* oDest,
	 size_t iCountCU, size_t* oCountCU, size_t iCountCP, size_t* oCountCP)
	{
	ZUnimplemented();
	}

void ChanR_UTF_Native32::ReadUTF8(UTF8* oDest,
	size_t iCountCU, size_t* oCountCU, size_t iCountCP, size_t* oCountCP)
	{
	ZUnimplemented();
	}

// =================================================================================================
// MARK: - ChanR_UTF_Native16

size_t ChanR_UTF_Native16::Read(UTF32* oDest, size_t iCountCU)
	{
	ZUnimplemented();
	}

void ChanR_UTF_Native16::ReadUTF8(UTF8* oDest,
	size_t iCountCU, size_t* oCountCU, size_t iCountCP, size_t* oCountCP)
	{
	ZUnimplemented();
	}

// =================================================================================================
// MARK: - ChanR_UTF_Native8

size_t ChanR_UTF_Native8::Read(UTF32* oDest, size_t iCountCU)
	{
	ZUnimplemented();
	}

void ChanR_UTF_Native8::ReadUTF16(UTF16* oDest,
	 size_t iCountCU, size_t* oCountCU, size_t iCountCP, size_t* oCountCP)
	{
	ZUnimplemented();
	}

} // namespace ZooLib
