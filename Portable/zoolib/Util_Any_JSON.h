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

#ifndef __ZooLib_Util_JSON_Any_h__
#define __ZooLib_Util_JSON_Any_h__ 1
#include "zconfig.h"

#include "zoolib/Channer_Bin.h"
#include "zoolib/Channer_UTF.h"
#include "zoolib/Val_Any.h"

// =================================================================================================
// MARK: -

namespace ZooLib {
namespace Util_Any_JSON {

ZQ<Val_Any> sQRead(const ZRef<ChannerR_UTF>& iChannerR, const ZRef<ChannerU_UTF>& iChannerU);

ZQ<Val_Any> sQRead(const ZRef<ChannerR_UTF>& iChannerR);

ZQ<Val_Any> sQRead(const ZRef<ChannerR_Bin>& iChannerR);

void sWrite(const Val_Any& iVal, const ChanW_UTF& iChanW);

void sWrite(bool iPrettyPrint, const Val_Any& iVal, const ChanW_UTF& iChanW);

string8 sAsJSON(const Val_Any& iVal);

const Val_Any sFromJSON(const string8& iString);

} // namespace ZUtil_Any_JSON

namespace Operators_Any_JSON {

const ChanW_UTF& operator<<(const ChanW_UTF& iChanW, const Val_Any& iVal);
const ChanW_UTF& operator<<(const ChanW_UTF& iChanW, const Map_Any& iMap);
const ChanW_UTF& operator<<(const ChanW_UTF& iChanW, const Seq_Any& iSeq);
const ChanW_UTF& operator<<(const ChanW_UTF& iChanW, const Data_Any& iData);

} // namespace Operators_Any_JSON

} // namespace ZooLib

#endif // __ZooLib_Util_JSON_Any_h__
