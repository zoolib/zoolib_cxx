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

#ifndef __ZooLib_ChanW_XX_Fragmented_h__
#define __ZooLib_ChanW_XX_Fragmented_h__ 1
#include "zconfig.h"

#include "zoolib/ChanFilter.h"
#include "zoolib/ChanW.h"
#include "zoolib/Compat_algorithm.h" // For min

namespace ZooLib {

// =================================================================================================
#pragma mark - ChanW_XX_Fragmented

/// A write filter stream that passes on data in chunks no larger than a limit.

template <class Chan_p>
class ChanW_XX_Fragmented
:	public ChanFilter<Chan_p>
	{
	typedef ChanFilter<Chan_p> inherited;
	typedef typename Chan_p::Element_t EE;
public:
	ChanW_XX_Fragmented(const Chan_p& iChan, size_t iFragmentCount)
	:	inherited(iChan)
	,	fFragmentCount(iFragmentCount)
		{}

	virtual size_t Write(const EE* iSource, size_t iCount)
		{ return sWrite(inherited::pGetChan(), iSource, min(fFragmentCount, iCount)); }

protected:
	const size_t fFragmentCount;
	};

} // namespace ZooLib

#endif // __ZooLib_ChanW_XX_Fragmented_h__
