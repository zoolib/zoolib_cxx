// Copyright (c) 2011 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_QueryEngine_Expr_Rel_Search_h__
#define __ZooLib_QueryEngine_Expr_Rel_Search_h__ 1
#include "zconfig.h"

#include "zoolib/Expr/Expr_Bool.h"
#include "zoolib/RelationalAlgebra/Expr_Rel.h"

namespace ZooLib {
namespace QueryEngine {

class Visitor_Expr_Rel_Search;

// =================================================================================================
#pragma mark - Expr_Rel_Search

class Expr_Rel_Search
:	public virtual RelationalAlgebra::Expr_Rel
,	public virtual Expr_Op0_T<RelationalAlgebra::Expr_Rel>
	{
	typedef Expr_Op0_T<Expr_Rel> inherited;
public:
	Expr_Rel_Search(const RelationalAlgebra::RelHead& iRelHead_Bound,
		const RelationalAlgebra::Rename& iRename,
		const RelationalAlgebra::RelHead& iRelHead_Optional,
		const ZP<Expr_Bool>& iExpr_Bool);

// From Visitee
	virtual void Accept(const Visitor& iVisitor);

// From Expr_Op0_T<Expr_Rel>
	virtual void Accept_Expr_Op0(Visitor_Expr_Op0_T<Expr_Rel>& iVisitor);

	virtual ZP<RelationalAlgebra::Expr_Rel> Self();
	virtual ZP<RelationalAlgebra::Expr_Rel> Clone();

// Our protocol
	virtual void Accept_Expr_Rel_Search(Visitor_Expr_Rel_Search& iVisitor);

	const RelationalAlgebra::RelHead& GetRelHead_Bound() const;
	const RelationalAlgebra::Rename& GetRename() const;
	const RelationalAlgebra::RelHead& GetRelHead_Optional() const;
	const ZP<Expr_Bool>& GetExpr_Bool() const;

private:
	const RelationalAlgebra::RelHead fRelHead_Bound;
	const RelationalAlgebra::Rename fRename;
	const RelationalAlgebra::RelHead fRelHead_Optional;
	const ZP<Expr_Bool> fExpr_Bool;
	};

// =================================================================================================
#pragma mark - Visitor_Expr_Rel_Search

class Visitor_Expr_Rel_Search
:	public virtual Visitor_Expr_Op0_T<RelationalAlgebra::Expr_Rel>
	{
public:
	virtual void Visit_Expr_Rel_Search(const ZP<Expr_Rel_Search>& iExpr);
	};

} // namespace QueryEngine
} // namespace ZooLib

#endif // __ZooLib_QueryEngine_Expr_Rel_Search_h__
