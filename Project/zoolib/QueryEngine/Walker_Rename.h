// Copyright (c) 2010 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_QueryEngine_Walker_Rename_h__
#define __ZooLib_QueryEngine_Walker_Rename_h__ 1
#include "zconfig.h"

#include "zoolib/QueryEngine/Walker.h"

namespace ZooLib {
namespace QueryEngine {

// =================================================================================================
#pragma mark - Walker_Rename

class Walker_Rename : public Walker_Unary
	{
public:
	Walker_Rename(const ZP<Walker>& iWalker, const string8& iNew, const string8& iOld);
	virtual ~Walker_Rename();

// From QueryEngine::Walker
	virtual ZP<Walker> Prime(
		const std::map<string8,size_t>& iOffsets,
		std::map<string8,size_t>& oOffsets,
		size_t& ioBaseOffset);

	virtual bool QReadInc(Val_DB* ioResults);

private:
	const string8 fNew;
	const string8 fOld;
	};

} // namespace QueryEngine
} // namespace ZooLib

#endif // __ZooLib_QueryEngine_Walker_Rename_h__
