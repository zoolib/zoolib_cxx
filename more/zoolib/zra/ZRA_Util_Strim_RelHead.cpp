/* -------------------------------------------------------------------------------------------------
Copyright (c) 2010 Andrew Green
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

#include "zoolib/ZUtil_Strim_Operators.h"
#include "zoolib/ZYad_ZooLibStrim.h"
#include "zoolib/zra/ZRA_Util_Strim_RelHead.h"

namespace ZooLib {
namespace ZRA {
namespace Util_Strim_RelHead {

// =================================================================================================
#pragma mark -
#pragma mark * Util_Strim_RelHead

using std::set;

void sWrite_PropName(const string8& iName, const ZStrimW& s)
	{
	s.Write("@");
	ZYad_ZooLibStrim::sWrite_PropName(iName, s);
	}

void sWrite_RelHead(const RelHead& iRelHead, const ZStrimW& s)
	{
	const RelHead::Base_t& names = iRelHead.GetElems();

	s.Write("[");

	bool isFirst = true;
	for (RelHead::Base_t::const_iterator i = names.begin(); i != names.end(); ++i)
		{
		if (not isFirst)
			s.Write(", ");
		isFirst = false;
		sWrite_PropName(*i, s);
		}
	s.Write("]");
	}

} // namespace Util_Strim_RelHead
} // namespace ZRA

const ZStrimW& operator<<(const ZStrimW& w, const ZRA::RelHead& iRH)
	{
	ZRA::Util_Strim_RelHead::sWrite_RelHead(iRH, w);
	return w;
	}

const ZStrimW& operator<<(const ZStrimW& w, const ZRA::Rename& iRename)
	{
	w << "[";
	bool isFirst = true;
	for (ZRA::Rename::const_iterator i = iRename.begin(); i != iRename.end(); ++i)
		{
		if (not isFirst)
			w << ", ";
		isFirst = false;
		w << i->second << "<--" << i->first;
		}
	w << "]";
	return w;
	}

} // namespace ZooLib
