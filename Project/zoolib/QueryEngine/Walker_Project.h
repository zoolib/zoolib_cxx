// Copyright (c) 2010 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_QueryEngine_Walker_Project_h__
#define __ZooLib_QueryEngine_Walker_Project_h__ 1
#include "zconfig.h"

#include "zoolib/QueryEngine/Walker.h"
#include "zoolib/RelationalAlgebra/RelHead.h"

namespace ZooLib {
namespace QueryEngine {

// =================================================================================================
#pragma mark - Walker_Project

class Walker_Project : public Walker_Unary
	{
public:
	Walker_Project(const ZP<Walker>& iWalker, const RelationalAlgebra::RelHead& iRelHead);
	virtual ~Walker_Project();

// From QueryEngine::Walker
	virtual void Rewind();

	virtual ZP<Walker> Prime(
		const std::map<string8,size_t>& iOffsets,
		std::map<string8,size_t>& oOffsets,
		size_t& ioBaseOffset);

	virtual bool QReadInc(Val_DB* ioResults);

private:
	const RelationalAlgebra::RelHead fRelHead;
	std::vector<size_t> fChildMapping;
	std::set<std::vector<Val_DB>> fPriors;
	};

} // namespace QueryEngine
} // namespace ZooLib

#endif // __ZooLib_QueryEngine_Walker_Project_h__
