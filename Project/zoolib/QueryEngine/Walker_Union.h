// Copyright (c) 2011 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_QueryEngine_Walker_Union_h__
#define __ZooLib_QueryEngine_Walker_Union_h__ 1
#include "zconfig.h"

#include "zoolib/QueryEngine/Walker.h"
#include "zoolib/RelationalAlgebra/RelHead.h"

namespace ZooLib {
namespace QueryEngine {

// =================================================================================================
#pragma mark - Walker_Union

class Walker_Union : public Walker
	{
public:
	Walker_Union(const ZP<Walker>& iWalker_Left, const ZP<Walker>& iWalker_Right);
	virtual ~Walker_Union();

// From QueryEngine::Walker
	virtual void Rewind();

	virtual ZP<Walker> Prime(
		const std::map<string8,size_t>& iOffsets,
		std::map<string8,size_t>& oOffsets,
		size_t& ioBaseOffset);

	virtual bool QReadInc(Val_DB* ioResults);

// Our protocol
	ZP<Walker> GetLeft()
		{ return fWalker_Left; }

	ZP<Walker> GetRight()
		{ return fWalker_Right; }

private:
	ZP<Walker> fWalker_Left;
	bool fExhaustedLeft;
	std::set<std::vector<Val_DB>> fPriors;
	std::vector<size_t> fMapping_Left;

	ZP<Walker> fWalker_Right;
	std::vector<size_t> fMapping_Right;
	};

} // namespace QueryEngine
} // namespace ZooLib

#endif // __ZooLib_QueryEngine_Walker_Union_h__
