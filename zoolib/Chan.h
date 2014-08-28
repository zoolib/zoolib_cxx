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

#ifndef __ZooLib_Chan_h__
#define __ZooLib_Chan_h__ 1
#include "zconfig.h"

#include "zoolib/ZStdInt.h" // For uint8

namespace ZooLib {

// =================================================================================================
// MARK: -

template <class Elmt_p>
class ChanTraits
	{
public:
	typedef Elmt_p Elmt;
	typedef Elmt_p* Pointer;
	typedef const Elmt_p* ConstPointer;
	};

template <>
class ChanTraits<void>
	{
public:
	typedef uint8 Elmt;
	typedef void* Pointer;
	typedef const void* ConstPointer;
	};

} // namespace ZooLib

#endif // __ZooLib_Chan_h__
