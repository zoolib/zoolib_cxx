// Copyright (c) 2010 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/QueryEngine/Walker_Dee.h"

namespace ZooLib {
namespace QueryEngine {

using std::map;
using std::set;

// =================================================================================================
#pragma mark - Walker_Dee

Walker_Dee::Walker_Dee()
:	fExhausted(false)
	{}

Walker_Dee::~Walker_Dee()
	{}

void Walker_Dee::Rewind()
	{
	this->Called_Rewind();
	fExhausted = false;
	}

ZP<Walker> Walker_Dee::Prime(
	const map<string8,size_t>& iOffsets,
	map<string8,size_t>& oOffsets,
	size_t& ioBaseOffset)
	{ return this; }

bool Walker_Dee::QReadInc(Val_DB* ioResults)
	{
	this->Called_QReadInc();
	return not sGetSet(fExhausted, true);
	}

} // namespace QueryEngine
} // namespace ZooLib
