/* -------------------------------------------------------------------------------------------------
Copyright (c) 2007 Andrew Green and Learning in Motion, Inc.
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

#ifndef __ZUtil_Strim_TQL__
#define __ZUtil_Strim_TQL__
#include "zconfig.h"

#include "zoolib/ZRelHead.h"
#include "zoolib/ZStrim.h"

NAMESPACE_ZOOLIB_BEGIN

namespace ZUtil_Strim_TQL {

// =================================================================================================
#pragma mark -
#pragma mark * ZUtil_Strim_TQL

void sWrite_PropName(const std::string& iName, const ZStrimW& s);
void sWrite_RelHead(const ZRelHead& iRelHead, const ZStrimW& s);

} // namespace ZUtil_Strim_TQL

NAMESPACE_ZOOLIB_END

#endif // __ZUtil_Strim_TQL__
