/* -------------------------------------------------------------------------------------------------
Copyright (c) 2012 Andrew Green
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

#ifndef __ZUtil_JSON_Any_h__
#define __ZUtil_JSON_Any_h__
#include "zconfig.h"

#include "zoolib/ZStreamer.h"
#include "zoolib/ZStrimmer.h"
#include "zoolib/ZVal_Any.h"

// =================================================================================================
// MARK: -

namespace ZooLib {
namespace ZUtil_Any_JSON {

ZQ<ZVal_Any> sQRead(const ZRef<ZStrimmerU>& iSU);

ZQ<ZVal_Any> sQRead(const ZRef<ZStrimmerR>& iSR);

ZQ<ZVal_Any> sQRead(const ZRef<ZStreamerR>& iSR);

void sWrite(const ZVal_Any& iVal, const ZStrimW& iStrimW);

} // namespace ZUtil_Any_JSON
} // namespace ZooLib

#endif // __ZUtil_JSON_Any_h__
