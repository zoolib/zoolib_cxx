/* ------------------------------------------------------------
Copyright (c) 2007 Andrew Green and Learning in Motion, Inc.
http://www.zoolib.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
COPYRIGHT HOLDER(S) BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
------------------------------------------------------------ */

#ifndef __ZUtil_Strim_TQL_Spec__
#define __ZUtil_Strim_TQL_Spec__
#include "zconfig.h"

#include "ZTQL_Spec.h"

class ZStrimW;

// =================================================================================================
#pragma mark -
#pragma mark * ZUtil_Strim_TQL_Spec

namespace ZUtil_Strim_TQL_Spec {

void sToStrim(const ZStrimW& iStrimW, const ZTQL::Condition& iCondition);
void sToStrim(const ZStrimW& iStrimW, const ZTQL::Spec& iSpec);
void sToStrim(const ZStrimW& iStrimW, ZRef<ZTQL::LogOp> iLogOp);

} // namespace ZUtil_Strim_TQL_Spec

#endif // __ZUtil_Strim_TQL_Spec__
