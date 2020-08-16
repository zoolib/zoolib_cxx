// Copyright (c) 2011 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_QueryEngine_Walker_Dum_h__
#define __ZooLib_QueryEngine_Walker_Dum_h__ 1
#include "zconfig.h"

#include "zoolib/QueryEngine/Walker.h"

namespace ZooLib {
namespace QueryEngine {

// =================================================================================================
#pragma mark - Walker_Dum

class Walker_Dum : public Walker
	{
public:
	Walker_Dum();
	virtual ~Walker_Dum();

// From QueryEngine::Walker
	virtual void Rewind();

	virtual ZP<Walker> Prime(
		const std::map<string8,size_t>& iOffsets,
		std::map<string8,size_t>& oOffsets,
		size_t& ioBaseOffset);

	virtual bool QReadInc(Val_DB* ioResults);
	};

} // namespace QueryEngine
} // namespace ZooLib

#endif // __ZooLib_QueryEngine_Walker_Dum_h__
