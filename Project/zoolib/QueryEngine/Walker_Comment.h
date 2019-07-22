// Copyright (c) 2019 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_QueryEngine_Walker_Comment_h__
#define __ZooLib_QueryEngine_Walker_Comment_h__ 1
#include "zconfig.h"

#include "zoolib/Callable.h"

#include "zoolib/QueryEngine/Walker.h"

namespace ZooLib {
namespace QueryEngine {

// =================================================================================================
#pragma mark - Walker_Comment

class Walker_Comment : public Walker_Unary
	{
public:
	Walker_Comment(const ZP<Walker>& iWalker,
		const string8& iComment,
		const ZP<Callable_Void>& iCallable);

	virtual ~Walker_Comment();

// From QueryEngine::Walker
	virtual ZP<Walker> Prime(
		const std::map<string8,size_t>& iOffsets,
		std::map<string8,size_t>& oOffsets,
		size_t& ioBaseOffset);

	virtual bool QReadInc(Val_Any* ioResults);

// Our protocol
	string8 GetComment();

private:
	const string8 fComment;
	const ZP<Callable_Void> fCallable;
	};

} // namespace QueryEngine
} // namespace ZooLib

#endif // __ZooLib_QueryEngine_Walker_Comment_h__
