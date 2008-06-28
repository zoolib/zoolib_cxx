/* -------------------------------------------------------------------------------------------------
Copyright (c) 2008 Andrew Green
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

#ifndef __ZUtil_Tuple__
#define __ZUtil_Tuple__ 1
#include "zconfig.h"

#include "zoolib/ZTuple.h"

// =================================================================================================
#pragma mark -
#pragma mark * ZUtil_Tuple

namespace ZUtil_Tuple {

bool sInt64(const ZTValue& iTV, int64& oVal);
int64 sInt64D(const ZTValue& iTV, int64 iDefault);
int64 sInt64(const ZTValue& iTV);

bool sUInt64(const ZTValue& iTV, uint64& oVal);
uint64 sUInt64D(const ZTValue& iTV, uint64 iDefault);
uint64 sUInt64(const ZTValue& iTV);

bool sDouble(const ZTValue& iTV, double& oVal);
double sDoubleD(const ZTValue& iTV, double iDefault);
double sDouble(const ZTValue& iTV);

bool sUDouble(const ZTValue& iTV, double& oVal);
double sUDoubleD(const ZTValue& iTV, double iDefault);
double sUDouble(const ZTValue& iTV);

bool sBool(const ZTValue& iTV, bool& oVal);
bool sBoolD(const ZTValue& iTV, bool iDefault);
bool sBool(const ZTValue& iTV);

bool sGetValuei(const ZTuple& iTuple, const std::string& iNamei, ZTValue& oTV);
ZTValue sDGetValuei(const ZTuple& iTuple, const std::string& iNamei, const ZTValue& iDefault);
ZTValue sGetValuei(const ZTuple& iTuple, const std::string& iNamei);

ZTuple::const_iterator sIteratorofi(const ZTuple& iTuple, const std::string& iNamei);

} // namespace ZUtil_Tuple

#endif // __ZUtil_Tuple__
