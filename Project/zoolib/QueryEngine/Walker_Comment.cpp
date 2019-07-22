// Copyright (c) 2019 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/QueryEngine/Walker_Comment.h"

namespace ZooLib {
namespace QueryEngine {

using std::map;
using std::set;

// =================================================================================================
#pragma mark - Walker_Comment

Walker_Comment::Walker_Comment(const ZP<Walker>& iWalker,
	const string8& iComment,
	const ZP<Callable_Void>& iCallable)
:	Walker_Unary(iWalker)
,	fComment(iComment)
,	fCallable(iCallable)
	{}

Walker_Comment::~Walker_Comment()
	{}

ZP<Walker> Walker_Comment::Prime(
	const map<string8,size_t>& iOffsets,
	map<string8,size_t>& oOffsets,
	size_t& ioBaseOffset)
	{
	if (fCallable)
		sCall(fCallable);
	fWalker->Prime(iOffsets, oOffsets, ioBaseOffset);
	return this;
	}

bool Walker_Comment::QReadInc(Val_Any* ioResults)
	{
	if (fCallable)
		sCall(fCallable);
	this->Called_QReadInc();
	return fWalker->QReadInc(ioResults);
	}

string8 Walker_Comment::GetComment()
	{ return fComment; }

} // namespace QueryEngine
} // namespace ZooLib
