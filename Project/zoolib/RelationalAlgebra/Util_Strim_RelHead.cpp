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

#include "zoolib/RelationalAlgebra/Util_Strim_RelHead.h"

#include "zoolib/Util_Chan_JSON.h"
#include "zoolib/Util_Chan_UTF.h"
#include "zoolib/Util_Chan_UTF_Operators.h"
#include "zoolib/Util_STL_set.h"
#include "zoolib/ValueOnce.h"

#include "zoolib/ZMACRO_foreach.h"

#include "zoolib/pdesc.h"
#if defined(ZMACRO_pdesc)
	#include "zoolib/StdIO.h"
#endif

namespace ZooLib {
namespace RelationalAlgebra {
namespace Util_Strim_RelHead {

// =================================================================================================
#pragma mark - Util_Strim_RelHead

using namespace Util_STL;
using std::set;

void sWrite_PropName(const string8& iName, const ChanW_UTF& s)
	{
	s << "@";
	Util_Chan_JSON::sWrite_PropName(iName, s);
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

ZQ<ColName> sQRead_PropName(const ChanR_UTF& iChanR, const ChanU_UTF& iChanU)
	{
	using namespace Util_Chan;

	if (not sTryRead_CP('@', iChanR, iChanU))
		return null;

	if (NotQ<string8> theQ = Util_Chan_JSON::sQRead_PropName(iChanR, iChanU))
		throw ParseException("Expected PropName after '@'");
	else
		return *theQ;
	}

ZQ<RelHead> sQFromStrim_RelHead(const ChanR_UTF& iChanR, const ChanU_UTF& iChanU)
	{
	using namespace Util_Chan;

	if (not sTryRead_CP('[', iChanR, iChanU))
		return null;

	RelHead result;

	for (;;)
		{
		sSkip_WSAndCPlusPlusComments(iChanR, iChanU);

		if (NotQ<string8> theQ = sQRead_PropName(iChanR, iChanU))
			throw ParseException("Expected PropName");
		else
			result |= *theQ;

		sSkip_WSAndCPlusPlusComments(iChanR, iChanU);
		if (not sTryRead_CP(',', iChanR, iChanU))
			break;
		}

	if (not sTryRead_CP(']', iChanR, iChanU))
			throw ParseException("Expected ']'");
	return result;
	}

ZQ<std::pair<ColName,ColName> > sQFromStrim_Rename(
	const ChanR_UTF& iChanR, const ChanU_UTF& iChanU)
	{
	using namespace Util_Chan;

	if (NotQ<string8> theQ0 = sQRead_PropName(iChanR, iChanU))
		{ return null; }
	else
		{
		sSkip_WSAndCPlusPlusComments(iChanR, iChanU);

		if (not sTryRead_String("<--", iChanR, iChanU))
			throw ParseException("Expected <-- after first PropName");

		sSkip_WSAndCPlusPlusComments(iChanR, iChanU);

		if (NotQ<string8> theQ1 = sQRead_PropName(iChanR, iChanU))
			throw ParseException("Expected second PropName after <--");
		else
			return std::pair<ColName,ColName>(*theQ0, *theQ1);
		}
	}

ZQ<ConcreteHead> sQFromStrim_ConcreteHead(const ChanR_UTF& iChanR, const ChanU_UTF& iChanU)
	{
	using namespace Util_Chan;

	if (not sTryRead_CP('[', iChanR, iChanU))
		return null;

	ConcreteHead result;

	for (;;)
		{
		sSkip_WSAndCPlusPlusComments(iChanR, iChanU);
		if (sTryRead_CP('@', iChanR, iChanU))
			{
			if (NotQ<string8> theQ = Util_Chan_JSON::sQRead_PropName(iChanR, iChanU))
				throw ParseException("Expected PropName after '@'");
			else
				result[*theQ] = true;
			}
		else if (sTryRead_CP('?', iChanR, iChanU))
			{
			if (NotQ<string8> theQ = Util_Chan_JSON::sQRead_PropName(iChanR, iChanU))
				throw ParseException("Expected PropName after '?'");
			else
				result[*theQ] = false;
			}
		else
			{
			throw ParseException("Expected PropName");
			}
		sSkip_WSAndCPlusPlusComments(iChanR, iChanU);
		if (not sTryRead_CP(',', iChanR, iChanU))
			break;
		}

	if (not sTryRead_CP(']', iChanR, iChanU))
			throw ParseException("Expected ']'");
	return result;
	}

void sWrite_RenameWithOptional(
	const RelationalAlgebra::Rename& iRename, const RelationalAlgebra::RelHead& iOptional,
	const ChanW_UTF& w)
	{
	w << "[";
	ValueOnce<std::string> separator("", ", ");
	foreachi (ii, iRename)
		{
		w << separator();
		if (sContains(iOptional, ii->first))
			w << "?";
		else
			w << "@";
		Util_Chan_JSON::sWrite_PropName(ii->second, w);
		if (ii->first != ii->second)
			{
			w << "<--";
			Util_Chan_JSON::sWrite_PropName(ii->first, w);
			}
		}
	w << "]";
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
	Util_Strim_RelHead::sWrite_RenameWithOptional(iRename, sDefault(), w);
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
		Util_Chan_JSON::sWrite_PropName(ii->first, w);
		}

	w << "]";

	return w;
	}

} // namespace ZooLib

// =================================================================================================
#pragma mark - pdesc

#if defined(ZMACRO_pdesc)

using namespace ZooLib;

ZMACRO_pdesc(const RelationalAlgebra::RelHead& iRH)
	{
	StdIO::sChan_UTF_Err << iRH << "\n";
	}

ZMACRO_pdesc(const RelationalAlgebra::Rename& iRename)
	{
	StdIO::sChan_UTF_Err << iRename << "\n";
	}

ZMACRO_pdesc(const RelationalAlgebra::ConcreteHead& iConcreteHead)
	{
	StdIO::sChan_UTF_Err << iConcreteHead << "\n";
	}

#endif // defined(ZMACRO_pdesc)
