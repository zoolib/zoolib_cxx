/* -------------------------------------------------------------------------------------------------
Copyright (c) 2006 Andrew Green and Learning in Motion, Inc.
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

#ifndef __ZUtil_Strim_TBSpec__
#define __ZUtil_Strim_TBSpec__
#include "zconfig.h"

namespace ZooLib {

class ZStrimU;
class ZStrimW;
class ZTBSpec;

// =================================================================================================
#pragma mark -
#pragma mark * ZUtil_Strim_TBSpec

namespace ZUtil_Strim_TBSpec {

void sToStrim(const ZTBSpec& iTBSpec, const ZStrimW& iStrimW);

bool sFromStrim(const ZStrimU& iStrimU, ZTBSpec& oTBSpec);

} // namespace ZUtil_Strim_TBSpec

// =================================================================================================
#pragma mark -
#pragma mark * operator<< overloads

const ZStrimW& operator<<(const ZStrimW& s, const ZTBSpec& iTBSpec);

} // namespace ZooLib

#endif // __ZUtil_Strim_TBSpec__
