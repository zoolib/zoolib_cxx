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

#include "zoolib/Util_Chan_Bin_Operators.h"
#include "zoolib/Util_Chan.h"

// =================================================================================================
#pragma mark -
#pragma mark Util_Chan_Bin_Operators

namespace ZooLib {
namespace Util_Chan_Bin_Operators {

const ChanW_Bin& operator<<(const ChanW_Bin& w, const char* iString)
	{
	sEWriteMem(w, iString, strlen(iString));
	return w;
	}

const ChanW_Bin& operator<<(const ChanW_Bin& w, char* iString)
	{
	sEWriteMem(w, iString, strlen(iString));
	return w;
	}

const ChanW_Bin& operator<<(const ChanW_Bin& w, const std::string& iString)
	{
	sEWriteMem(w, iString.c_str(), iString.length());
	return w;
	}

const ChanW_Bin& operator<<(const ChanW_Bin& w, const ChanR_Bin& r)
	{
	sCopyAll<byte>(r, w);
	return w;
	}

} // namespace Util_Chan_Bin_Operators
} // namespace ZooLib
