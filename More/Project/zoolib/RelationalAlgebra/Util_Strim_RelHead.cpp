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

#include "zoolib/Util_Chan_UTF_Operators.h"
#include "zoolib/ValueOnce.h"
#include "zoolib/Yad_JSON.h"

#include "zoolib/ZMACRO_foreach.h"

#include "zoolib/RelationalAlgebra/Util_Strim_RelHead.h"

namespace ZooLib {
namespace RelationalAlgebra {
namespace Util_Strim_RelHead {

// =================================================================================================
#pragma mark -
#pragma mark Util_Strim_RelHead

using std::set;

void sWrite_PropName(const string8& iName, const ChanW_UTF& s)
	{
	s << "@";
	Yad_JSON::sWrite_PropName(iName, s);
	}

void sWrite_RelHead(const RelHead& iRelHead, const ChanW_UTF& s)
	{
	s << "[";

	FalseOnce needsSeparator;
	foreachi (ii, iRelHead)
		{
		if (needsSeparator())
			s << ", ";
		sWrite_PropName(*ii, s);
		}
	s << "]";
	}

} // namespace Util_Strim_RelHead
} // namespace RelationalAlgebra

using namespace RelationalAlgebra;

const ChanW_UTF& operator<<(const ChanW_UTF& w, const RelHead& iRH)
	{
	Util_Strim_RelHead::sWrite_RelHead(iRH, w);
	return w;
	}

const ChanW_UTF& operator<<(const ChanW_UTF& w, const Rename& iRename)
	{
	w << "[";
	FalseOnce needsSeparator;
	foreachi (ii, iRename)
		{
		if (needsSeparator())
			w << ", ";
		w << ii->second << "<--" << ii->first;
		}
	w << "]";
	return w;
	}

const ChanW_UTF& operator<<(const ChanW_UTF& w, const ConcreteHead& iCH)
	{
	w << "[";

	ValueOnce<std::string> separator("", ", ");
	for (ConcreteHead::const_iterator ii = iCH.begin(), end = iCH.end();
		ii != end; ++ii)
		{
		w << separator();
		if (ii->second)
			w << "@";
		else
			w << "?";
		Yad_JSON::sWrite_PropName(ii->first, w);
		}

	w << "]";

	return w;
	}

} // namespace ZooLib
