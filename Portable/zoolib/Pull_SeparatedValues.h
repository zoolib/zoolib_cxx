/* -------------------------------------------------------------------------------------------------
Copyright (c) 2018 Andrew Green
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

#ifndef __ZooLib_Pull_SeparatedValues_h__
#define __ZooLib_Pull_SeparatedValues_h__ 1
#include "zconfig.h"

#include "zoolib/ChanR_UTF.h"
#include "zoolib/PullPush.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - 

struct Pull_SeparatedValues_Options
	{
	Pull_SeparatedValues_Options(UTF32 iSeparator_Value, UTF32 iSeparator_Line);

	UTF32 fSeparator_Value;
	UTF32 fSeparator_Line;
	};

bool sPull_SeparatedValues_Push_PPT(const ChanR_UTF& iChanR,
	const Pull_SeparatedValues_Options& iOptions,
	const ChanW_PPT& iChanW);

bool sPull_SeparatedValues_Push_PPT(const ChanR_UTF& iChanR,
	UTF32 iSeparator_Value, UTF32 iSeparator_Line,
	const ChanW_PPT& iChanW);

} // namespace ZooLib

#endif // __ZooLib_Pull_SeparatedValues_h__
