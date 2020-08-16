// Copyright (c) 2010 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/QueryEngine/Walker_Const.h"

namespace ZooLib {
namespace QueryEngine {

using std::map;
using std::set;

// =================================================================================================
#pragma mark - Walker_Const

Walker_Const::Walker_Const(const string8& iColName, const Val_DB& iVal)
:	fExhausted(false)
,	fColName(iColName)
,	fVal(iVal)
	{}

Walker_Const::~Walker_Const()
	{}

void Walker_Const::Rewind()
	{
	this->Called_Rewind();
	fExhausted = false;
	}

ZP<Walker> Walker_Const::Prime(
	const map<string8,size_t>& iOffsets,
	map<string8,size_t>& oOffsets,
	size_t& ioBaseOffset)
	{
	fOutputOffset = ioBaseOffset++;
	oOffsets[fColName] = fOutputOffset;
	return this;
	}

bool Walker_Const::QReadInc(Val_DB* ioResults)
	{
	this->Called_QReadInc();

	if (sGetSet(fExhausted, true))
		return false;

	ioResults[fOutputOffset] = fVal;
	return true;
	}

} // namespace QueryEngine
} // namespace ZooLib
