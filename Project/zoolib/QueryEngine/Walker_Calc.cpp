// Copyright (c) 2010 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/QueryEngine/Walker_Calc.h"

namespace ZooLib {
namespace QueryEngine {

using std::map;
using std::set;
using std::vector;

// =================================================================================================
#pragma mark - Walker_Calc

Walker_Calc::Walker_Calc(const ZP<Walker>& iWalker,
	const string8& iColName,
	const ZP<Callable_t>& iCallable)
:	Walker_Unary(iWalker)
,	fColName(iColName)
,	fCallable(iCallable)
	{}

Walker_Calc::~Walker_Calc()
	{}

ZP<Walker> Walker_Calc::Prime(
	const map<string8,size_t>& iOffsets,
	map<string8,size_t>& oOffsets,
	size_t& ioBaseOffset)
	{
	fWalker = fWalker->Prime(iOffsets, fBindings, ioBaseOffset);
	oOffsets.insert(fBindings.begin(), fBindings.end());
	fOutputOffset = ioBaseOffset++;
	oOffsets[fColName] = fOutputOffset;

	if (not fWalker)
		return null;

	return this;
	}

bool Walker_Calc::QReadInc(Val_DB* ioResults)
	{
	this->Called_QReadInc();

	if (not fWalker->QReadInc(ioResults))
		return false;

	ioResults[fOutputOffset] = fCallable->Call(PseudoMap(&fBindings, ioResults));

	return true;
	}

} // namespace QueryEngine
} // namespace ZooLib
