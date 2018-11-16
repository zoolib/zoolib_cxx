/* -------------------------------------------------------------------------------------------------
Copyright (c) 2011 Andrew Green
http://www.zoolib.org

Permission is hereby granted, free of charge, to any person obtaining a copy of this software
and associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software
is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE COPYRIGHT HOLDER(S) BE LIABLE FOR ANY CLAIM, DAMAGES
OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
------------------------------------------------------------------------------------------------- */

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
		const ZRef<Expr_Bool>& iExpr_Bool);

// From Visitee
	virtual void Accept(const Visitor& iVisitor);

// From Expr_Op0_T<Expr_Rel>
	virtual void Accept_Expr_Op0(Visitor_Expr_Op0_T<Expr_Rel>& iVisitor);

	virtual ZRef<RelationalAlgebra::Expr_Rel> Self();
	virtual ZRef<RelationalAlgebra::Expr_Rel> Clone();

// Our protocol
	virtual void Accept_Expr_Rel_Search(Visitor_Expr_Rel_Search& iVisitor);

	const RelationalAlgebra::RelHead& GetRelHead_Bound() const;
	const RelationalAlgebra::Rename& GetRename() const;
	const RelationalAlgebra::RelHead& GetRelHead_Optional() const;
	const ZRef<Expr_Bool>& GetExpr_Bool() const;

private:
	const RelationalAlgebra::RelHead fRelHead_Bound;
	const RelationalAlgebra::Rename fRename;
	const RelationalAlgebra::RelHead fRelHead_Optional;
	const ZRef<Expr_Bool> fExpr_Bool;
	};

// =================================================================================================
#pragma mark - Visitor_Expr_Rel_Search

class Visitor_Expr_Rel_Search
:	public virtual Visitor_Expr_Op0_T<RelationalAlgebra::Expr_Rel>
	{
public:
	virtual void Visit_Expr_Rel_Search(const ZRef<Expr_Rel_Search>& iExpr);
	};

} // namespace QueryEngine
} // namespace ZooLib

#endif // __ZooLib_QueryEngine_Expr_Rel_Search_h__
