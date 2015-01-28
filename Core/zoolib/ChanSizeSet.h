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

#ifndef __ZooLib_ChanSizeSet_h__
#define __ZooLib_ChanSizeSet_h__ 1
#include "zconfig.h"

#include "zoolib/ZStdInt.h" // For uint64

#include <stdexcept> // For range_error

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark ChanSizeSet

class ChanSizeSet
	{
public:
	typedef ChanSizeSet Chan_Base;

	static void sThrowBadSize()
		{ throw std::range_error("ChanSizeSet"); }

	virtual void SizeSet(uint64 iSize) = 0;
	};

// =================================================================================================
#pragma mark -

inline void sSizeSet(uint64 iSize, const ChanSizeSet& iChanSizeSet)
	{ return sNonConst(iChanSizeSet).SizeSet(iSize); }

inline void sSetSize(uint64 iSize, const ChanSizeSet& iChanSizeSet)
	{ return sNonConst(iChanSizeSet).SizeSet(iSize); }

} // namespace ZooLib

#endif // __ZooLib_ChanSizeSet_h__
