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

#include "zoolib/ByteSwap.h"

#include "zoolib/ZByteSwap.h"

// For now we're using the functions provided in ZByteSwap. Ultimately we'll migrate their
// implementations to here, and probably those function will wrap the ones here.

namespace ZooLib {

// =================================================================================================
// MARK: -

template <>
void sByteSwap<size_t(1)>(void* ioBuf)
	{}

template <>
void sByteSwap<size_t(2)>(void* ioBuf)
	{ ZByteSwap_16(ioBuf); }

template <>
void sByteSwap<size_t(4)>(void* ioBuf)
	{ ZByteSwap_32(ioBuf); }

template <>
void sByteSwap<size_t(8)>(void* ioBuf)
	{ ZByteSwap_64(ioBuf); }

} // namespace ZooLib
