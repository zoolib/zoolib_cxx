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

#include "zoolib/ChanW_UTF_string.h"

namespace ZooLib {

// =================================================================================================
// MARK: - ChanW_UTF_string<UTF32>

ChanW_UTF_string<UTF32>::ChanW_UTF_string(string32* ioString)
:	fStringPtr(ioString)
	{}

size_t ChanW_UTF_string<UTF32>::QWrite(const UTF32* iSource, size_t iCountCU)
	{
	fStringPtr->append(iSource, iCountCU);
	return iCountCU;
	}

// =================================================================================================
// MARK: - ChanW_UTF_string<UTF16>

ChanW_UTF_string<UTF16>::ChanW_UTF_string(string16* ioString)
:	fStringPtr(ioString)
	{}

size_t ChanW_UTF_string<UTF16>::QWrite(const UTF16* iSource, size_t iCountCU)
	{
	fStringPtr->append(iSource, iCountCU);
	return iCountCU;
	}

// =================================================================================================
// MARK: - ChanW_UTF_string<UTF8>

ChanW_UTF_string<UTF8>::ChanW_UTF_string(string8* ioString)
:	fStringPtr(ioString)
	{}

size_t ChanW_UTF_string<UTF8>::QWrite(const UTF8* iSource, size_t iCountCU)
	{
	fStringPtr->append(iSource, iCountCU);
	return iCountCU;
	}

} // namespace ZooLib
