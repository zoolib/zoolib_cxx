// Copyright (c) 2010 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_QueryEngine_Walker_Calc_h__
#define __ZooLib_QueryEngine_Walker_Calc_h__ 1
#include "zconfig.h"

#include "zoolib/RelationalAlgebra/Expr_Rel_Calc.h"
#include "zoolib/RelationalAlgebra/RelHead.h"
#include "zoolib/QueryEngine/Walker.h"

namespace ZooLib {
namespace QueryEngine {

// =================================================================================================
#pragma mark - Walker_Calc

class Walker_Calc : public Walker_Unary
	{
public:
	typedef RelationalAlgebra::Expr_Rel_Calc::Callable_t Callable_t;
	typedef RelationalAlgebra::Expr_Rel_Calc::PseudoMap PseudoMap;

	Walker_Calc(const ZP<Walker>& iWalker,
		const string8& iColName,
		const ZP<Callable_t>& iCallable);

	virtual ~Walker_Calc();

// From QueryEngine::Walker
	virtual ZP<Walker> Prime(
		const std::map<string8,size_t>& iOffsets,
		std::map<string8,size_t>& oOffsets,
		size_t& ioBaseOffset);

	virtual bool QReadInc(Val_DB* oResults);

private:
	const string8 fColName;
	const ZP<Callable_t> fCallable;
	size_t fOutputOffset;
	std::map<string8,size_t> fBindings;
	};

} // namespace QueryEngine
} // namespace ZooLib

#endif // __ZooLib_QueryEngine_Walker_Calc_h__
