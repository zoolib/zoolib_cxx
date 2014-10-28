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

#include "zoolib/ZUnicode_Normalize.h"

namespace ZooLib {
namespace ZUnicode {

// =================================================================================================
// MARK: - ZUnicode, normalization -- very incomplete for now

bool sNormalized(string16& oResult, const Param_Normalize& iParam)
	{ return FunctionChain<string16, Param_Normalize>::sInvoke(oResult, iParam); }

bool sNormalized(const string16& iString, ENormForm iNormForm, string16& oResult)
	{ return sNormalized(oResult, Param_Normalize(iString, iNormForm)); }

string16 sNormalized(const string16& iString, ENormForm iNormForm)
	{
	string16 result;
	if (sNormalized(result, Param_Normalize(iString, iNormForm)))
		return result;
	return iString;
	}

} // namespace ZUnicode
} // namespace ZooLib
