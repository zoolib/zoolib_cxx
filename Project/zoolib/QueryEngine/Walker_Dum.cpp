// Copyright (c) 2011 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/QueryEngine/Walker_Dum.h"

namespace ZooLib {
namespace QueryEngine {

using std::map;
using std::set;

// =================================================================================================
#pragma mark - Walker_Dum

Walker_Dum::Walker_Dum()
	{}

Walker_Dum::~Walker_Dum()
	{}

void Walker_Dum::Rewind()
	{
	this->Called_Rewind();
	}

ZP<Walker> Walker_Dum::Prime(
	const map<string8,size_t>& iOffsets,
	map<string8,size_t>& oOffsets,
	size_t& ioBaseOffset)
	{ return this; }

bool Walker_Dum::QReadInc(Val_DB* ioResults)
	{
	this->Called_QReadInc();
	return false;
	}

} // namespace QueryEngine
} // namespace ZooLib
