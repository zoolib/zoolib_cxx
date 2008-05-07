/* -------------------------------------------------------------------------------------------------
Copyright (c) 2005 Andrew Green and Learning in Motion, Inc.
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

#ifndef __ZUtil_Strim_JSON__
#define __ZUtil_Strim_JSON__ 1
#include "zconfig.h"

#include <vector>

class ZTuple;
class ZTValue;
class ZStrimU;
class ZStrimW;

namespace ZUtil_Strim_JSON {

void sToStrim(const ZStrimW& iStrimW, const ZTValue& iTV);
void sToStrim(const ZStrimW& iStrimW, const ZTuple& iTuple);
void sToStrim(const ZStrimW& iStrimW, const std::vector<ZTValue>& iVector);

bool sFromStrim(const ZStrimU& iStrimU, ZTValue& oTValue);

void sNormalize(ZTValue& ioTV);
bool sNormalize(bool iPreserveTuples, bool iPreserveVectors, ZTValue& ioTV);

ZTValue sNormalized(const ZTValue& iTV);
ZTValue sNormalized(bool iPreserveTuples, bool iPreserveVectors, const ZTValue& iTV);

void sNormalized(const ZTValue& iTV, ZTValue& oTV);
bool sNormalized(bool iPreserveTuples, bool iPreserveVectors, const ZTValue& iTV, ZTValue& oTV);

} // namespace ZUtil_Strim_JSON

#endif // __ZUtil_Strim_JSON__
