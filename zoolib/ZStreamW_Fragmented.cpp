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

#include "zoolib/ZStreamW_Fragmented.h"

using std::max;

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamW_Fragmented

/**
\class ZStreamW_Fragmented
\ingroup stream
ZStreamW_Fragmented passes data written to it, but no more than fFragmentSize at a time.
*/

ZStreamW_Fragmented::ZStreamW_Fragmented(size_t iFragmentSize, const ZStreamW& iStreamSink)
:	fStreamSink(iStreamSink),
	fFragmentSize(iFragmentSize)
	{}

void ZStreamW_Fragmented::Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten)
	{
	fStreamSink.Write(iSource, max(fFragmentSize, iCount), oCountWritten);
	}

void ZStreamW_Fragmented::Imp_Flush()
	{
	fStreamSink.Flush();
	}
