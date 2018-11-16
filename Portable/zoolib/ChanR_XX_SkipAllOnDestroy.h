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

#ifndef __ZooLib_ChanR_XX_SkipAllOnDestroy_h__
#define __ZooLib_ChanR_XX_SkipAllOnDestroy_h__ 1
#include "zconfig.h"

#include "zoolib/ChanFilter.h"
#include "zoolib/ChanR.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - ChanR_XX_SkipAllOnDestroy

/// A read filter stream that when destroyed invokes SkipAll on its real stream.

template <class Chan_p>
class ChanR_XX_SkipAllOnDestroy
:	public ChanFilter<Chan_p>
	{
	typedef ChanFilter<Chan_p> inherited;
public:
	ChanR_XX_SkipAllOnDestroy(const Chan_p& iChan)
	:	inherited(iChan)
		{}

	~ChanR_XX_SkipAllOnDestroy()
		{ sSkipAll(inherited::pGetChan()); }
	};

} // namespace ZooLib

#endif // __ZooLib_ChanR_XX_SkipAllOnDestroy_h__
