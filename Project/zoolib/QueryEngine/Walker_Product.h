// Copyright (c) 2010 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_QueryEngine_Walker_Product_h__
#define __ZooLib_QueryEngine_Walker_Product_h__ 1
#include "zconfig.h"

#include "zoolib/QueryEngine/Walker.h"

namespace ZooLib {
namespace QueryEngine {

// =================================================================================================
#pragma mark - Walker_Product

class Walker_Product : public Walker
	{
public:
	Walker_Product(const ZP<Walker>& iWalker_Left, const ZP<Walker>& iWalker_Right);
	virtual ~Walker_Product();

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
	std::vector<Val_DB> fResults_Left;

	ZP<Walker> fWalker_Right;
	size_t fBaseOffset;
	bool fNeedLoadLeft;
	};

} // namespace QueryEngine
} // namespace ZooLib

#endif // __ZooLib_QueryEngine_Walker_Product_h__
