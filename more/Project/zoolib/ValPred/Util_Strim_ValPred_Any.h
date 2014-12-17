/* -------------------------------------------------------------------------------------------------
Copyright (c) 2010 Andrew Green
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

#ifndef __ZooLib_ValPred_Util_Strim_ValPred_Any_h__
#define __ZooLib_ValPred_Util_Strim_ValPred_Any_h__ 1
#include "zconfig.h"

#include "zoolib/ChanW_UTF.h"

#include "zoolib/ValPred/ValPred.h"

#include <set>

namespace ZooLib {
namespace Util_Strim_ValPred_Any {

// =================================================================================================
// MARK: - Util_Strim_ValPred_Any

void sToStrim(const ValPred& iValPred, const ChanW_UTF& iStrimW);

} // namespace Util_Strim_ValPred_Any

const ChanW_UTF& operator<<(const ChanW_UTF& w, const ValPred& iValPred);

} // namespace ZooLib

#endif // __ZooLib_ValPred_Util_Strim_ValPred_Any_h__
