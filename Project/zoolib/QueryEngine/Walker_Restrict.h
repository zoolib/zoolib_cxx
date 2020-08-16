// Copyright (c) 2010 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_QueryEngine_Walker_Restrict_h__
#define __ZooLib_QueryEngine_Walker_Restrict_h__ 1
#include "zconfig.h"

#include "zoolib/Expr/Expr_Bool.h"
#include "zoolib/QueryEngine/Walker.h"

namespace ZooLib {
namespace QueryEngine {

// =================================================================================================
#pragma mark - Walker_Restrict

class Walker_Restrict : public Walker_Unary
	{
public:
	Walker_Restrict(ZP<Walker> iWalker, ZP<Expr_Bool> iExpr_Bool);
	virtual ~Walker_Restrict();

// From QueryEngine::Walker
	virtual ZP<Walker> Prime(
		const std::map<string8,size_t>& iOffsets,
		std::map<string8,size_t>& oOffsets,
		size_t& ioBaseOffset);

	virtual bool QReadInc(Val_DB* ioResults);

	class Exec;

private:
	const ZP<Expr_Bool> fExpr_Bool;
	std::map<string8,size_t> fCombinedOffsets;

	std::vector<Val_DB> fConsts;
	Exec* fExec;
	};

} // namespace QueryEngine
} // namespace ZooLib

#endif // __ZooLib_QueryEngine_Walker_Restrict_h__
