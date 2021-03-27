// Copyright (c) 2010 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/RelationalAlgebra/Util_Strim_RelHead.h"

#include "zoolib/ParseException.h"
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

void sWrite_PropName(const ChanW_UTF& s, const string8& iName)
	{
	s << "@";
	Util_Chan_JSON::sWrite_PropName(s, iName);
	}

void sWrite_RelHead(const ChanW_UTF& s, const RelHead& iRelHead)
	{
	s << "[";

	FalseOnce needsSeparator;
	foreacha (entry, iRelHead)
		{
		if (needsSeparator())
			s << ", ";
		sWrite_PropName(s, entry);
		}
	s << "]";
	}

ZQ<ColName> sQRead_PropName(const ChanRU_UTF& iChanRU)
	{
	using namespace Util_Chan;

	if (not sTryRead_CP('@', iChanRU))
		return null;

	if (NotQ<string8> theQ = Util_Chan_JSON::sQRead_PropName(iChanRU))
		throw ParseException("Expected PropName after '@'");
	else
		return *theQ;
	}

ZQ<RelHead> sQFromStrim_RelHead(const ChanRU_UTF& iChanRU)
	{
	using namespace Util_Chan;

	if (not sTryRead_CP('[', iChanRU))
		return null;

	RelHead result;

	for (;;)
		{
		sSkip_WSAndCPlusPlusComments(iChanRU);

		if (NotQ<string8> theQ = sQRead_PropName(iChanRU))
			throw ParseException("Expected PropName");
		else
			result |= *theQ;

		sSkip_WSAndCPlusPlusComments(iChanRU);
		if (not sTryRead_CP(',', iChanRU))
			break;
		}

	if (not sTryRead_CP(']', iChanRU))
		throw ParseException("Expected ']'");
	return result;
	}

ZQ<std::pair<ColName,ColName>> sQFromStrim_Rename(const ChanRU_UTF& iChanRU)
	{
	using namespace Util_Chan;

	if (NotQ<string8> theQ0 = sQRead_PropName(iChanRU))
		{ return null; }
	else
		{
		sSkip_WSAndCPlusPlusComments(iChanRU);

		if (not sTryRead_String(iChanRU, "<--"))
			throw ParseException("Expected <-- after first PropName");

		sSkip_WSAndCPlusPlusComments(iChanRU);

		if (NotQ<string8> theQ1 = sQRead_PropName(iChanRU))
			throw ParseException("Expected second PropName after <--");
		else
			return std::pair<ColName,ColName>(*theQ0, *theQ1);
		}
	}

ZQ<ConcreteHead> sQFromStrim_ConcreteHead(const ChanRU_UTF& iChanRU)
	{
	using namespace Util_Chan;

	if (not sTryRead_CP('[', iChanRU))
		return null;

	ConcreteHead result;

	for (;;)
		{
		sSkip_WSAndCPlusPlusComments(iChanRU);
		if (sTryRead_CP('@', iChanRU))
			{
			if (NotQ<string8> theQ = Util_Chan_JSON::sQRead_PropName(iChanRU))
				throw ParseException("Expected PropName after '@'");
			else
				result[*theQ] = true;
			}
		else if (sTryRead_CP('?', iChanRU))
			{
			if (NotQ<string8> theQ = Util_Chan_JSON::sQRead_PropName(iChanRU))
				throw ParseException("Expected PropName after '?'");
			else
				result[*theQ] = false;
			}
		else
			{
			throw ParseException("Expected PropName");
			}
		sSkip_WSAndCPlusPlusComments(iChanRU);
		if (not sTryRead_CP(',', iChanRU))
			break;
		}

	if (not sTryRead_CP(']', iChanRU))
			throw ParseException("Expected ']'");
	return result;
	}

void sWrite_RenameWithOptional(
	const ChanW_UTF& ww,
	const RelationalAlgebra::Rename& iRename, const RelationalAlgebra::RelHead& iOptional)
	{
	ww << "[";
	ValueOnce<std::string> separator("", ", ");
	foreacha (entry, iRename)
		{
		ww << separator();
		if (sContains(iOptional, entry.first))
			ww << "?";
		else
			ww << "@";
		Util_Chan_JSON::sWrite_PropName(ww, entry.second);
		if (entry.first != entry.second)
			{
			ww << "<--";
			Util_Chan_JSON::sWrite_PropName(ww, entry.first);
			}
		}
	ww << "]";
	}

} // namespace Util_Strim_RelHead
} // namespace RelationalAlgebra

using namespace RelationalAlgebra;

const ChanW_UTF& operator<<(const ChanW_UTF& ww, const RelHead& iRH)
	{
	Util_Strim_RelHead::sWrite_RelHead(ww, iRH);
	return ww;
	}

const ChanW_UTF& operator<<(const ChanW_UTF& ww, const Rename& iRename)
	{
	Util_Strim_RelHead::sWrite_RenameWithOptional(ww, iRename, sDefault());
	return ww;
	}

const ChanW_UTF& operator<<(const ChanW_UTF& ww, const ConcreteHead& iCH)
	{
	ww << "[";

	ValueOnce<std::string> separator("", ", ");
	foreacha (entry, iCH)
		{
		ww << separator();
		if (entry.second)
			ww << "@";
		else
			ww << "?";
		Util_Chan_JSON::sWrite_PropName(ww, entry.first);
		}

	ww << "]";

	return ww;
	}

} // namespace ZooLib

// =================================================================================================
#pragma mark - pdesc

#if defined(ZMACRO_pdesc)

using namespace ZooLib;

ZMACRO_pdesc(const RelationalAlgebra::RelHead& iRH)
	{
	StdIO::sChanW_UTF_Err << iRH << "\n";
	}

ZMACRO_pdesc(const RelationalAlgebra::Rename& iRename)
	{
	StdIO::sChanW_UTF_Err << iRename << "\n";
	}

ZMACRO_pdesc(const RelationalAlgebra::ConcreteHead& iConcreteHead)
	{
	StdIO::sChanW_UTF_Err << iConcreteHead << "\n";
	}

#endif // defined(ZMACRO_pdesc)
