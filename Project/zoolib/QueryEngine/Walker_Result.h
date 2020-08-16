// Copyright (c) 2010 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_QueryEngine_Walker_Result_h__
#define __ZooLib_QueryEngine_Walker_Result_h__ 1
#include "zconfig.h"

#include "zoolib/QueryEngine/Result.h"
#include "zoolib/QueryEngine/Walker.h"

namespace ZooLib {
namespace QueryEngine {

// =================================================================================================
#pragma mark - Walker_Result

class Walker_Result : public Walker
	{
public:
	Walker_Result(ZP<Result> iResult);
	virtual ~Walker_Result();

// From QueryEngine::Walker
	virtual void Rewind();

	virtual ZP<Walker> Prime(
		const std::map<string8,size_t>& iOffsets,
		std::map<string8,size_t>& oOffsets,
		size_t& ioBaseOffset);

	virtual bool QReadInc(Val_DB* oResults);

	ZP<Result> fResult;
	size_t fIndex;
	size_t fBaseOffset;
	};

} // namespace QueryEngine
} // namespace ZooLib

#endif // __ZooLib_QueryEngine_Walker_Result_h__
