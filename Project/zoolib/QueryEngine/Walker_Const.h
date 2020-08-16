// Copyright (c) 2010 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_QueryEngine_Walker_Const_h__
#define __ZooLib_QueryEngine_Walker_Const_h__ 1
#include "zconfig.h"

#include "zoolib/QueryEngine/Walker.h"

namespace ZooLib {
namespace QueryEngine {

// =================================================================================================
#pragma mark - Walker_Const

class Walker_Const : public Walker
	{
public:
	Walker_Const(const string8& iColName, const Val_DB& iVal);
	virtual ~Walker_Const();

// From QueryEngine::Walker
	virtual void Rewind();

	virtual ZP<Walker> Prime(
		const std::map<string8,size_t>& iOffsets,
		std::map<string8,size_t>& oOffsets,
		size_t& ioBaseOffset);

	virtual bool QReadInc(Val_DB* ioResults);

private:
	bool fExhausted;
	const string8 fColName;
	size_t fOutputOffset;
	const Val_DB fVal;
	};

} // namespace QueryEngine
} // namespace ZooLib

#endif // __ZooLib_QueryEngine_Walker_Const_h__
