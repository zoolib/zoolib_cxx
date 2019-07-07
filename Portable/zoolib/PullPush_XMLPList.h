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

#ifndef __ZooLib_PullPush_XMLPList_h__
#define __ZooLib_PullPush_XMLPList_h__ 1
#include "zconfig.h"

#include "zoolib/ChanRU_UTF_ML.h"
#include "zoolib/ChanW_UTF_ML.h"
#include "zoolib/PullPush.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - 

void sPull_XMLPList_Push_PPT(ChanRU_UTF_ML& iChanRU, const ChanW_PPT& iChanW);

void sPull_XMLPList_Push_PPT(const ChanR_PPT& iChanR, const ChanW_PPT& iChanW);

bool sPull_PPT_Push_XMLPList(const ChanR_PPT& iChanR, const ChanW_UTF_ML& iChanW);

void sWriteXMLPListPreamble(const ChanW_UTF_ML& s);
void sWriteXMLPListPostamble(const ChanW_UTF_ML& s);

} // namespace ZooLib

#endif // __ZooLib_PullPush_XMLPList_h__
