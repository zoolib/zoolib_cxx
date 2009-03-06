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

#include "zoolib/ZUtil_Strim_JSON.h"

#include "zoolib/ZYad_JSON.h"
#include "zoolib/ZYad_ZooLib.h"

NAMESPACE_ZOOLIB_BEGIN

using std::vector;

// =================================================================================================
#pragma mark -
#pragma mark * ZUtil_Strim_JSON

namespace ZUtil_Strim_JSON {

void sToStrim(const ZStrimW& s, const ZTValue& iTV)
	{
	ZYad_JSON::sToStrim(s, ZYad_ZooLib::sMakeYadR(iTV));
	}

void sToStrim(const ZStrimW& s, const ZTuple& iTuple)
	{
	ZYad_JSON::sToStrim(s, ZYad_ZooLib::sMakeYadR(iTuple));
	}

void sToStrim(const ZStrimW& s, const vector<ZTValue>& iVector)
	{
	ZYad_JSON::sToStrim(s, ZYad_ZooLib::sMakeYadR(iVector));
	}

bool sFromStrim(const ZStrimU& s, ZTValue& oTV)
	{
	if (ZRef<ZYadR> theYadR = ZYad_JSON::sMakeYadR(s))
		{
		oTV = ZYad_ZooLib::sFromYadR(theYadR);
		return true;
		}
	return false;
	}

} // namespace ZUtil_Strim_JSON

NAMESPACE_ZOOLIB_END
