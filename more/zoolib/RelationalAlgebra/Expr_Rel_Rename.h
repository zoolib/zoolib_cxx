/* -------------------------------------------------------------------------------------------------
Copyright (c) 2010 Andrew Green
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

#ifndef __ZRA_Expr_Rel_Rename_h__
#define __ZRA_Expr_Rel_Rename_h__ 1
#include "zconfig.h"

#include "zoolib/ZExpr_Op_T.h"
#include "zoolib/RelationalAlgebra/Expr_Rel.h"

#include <string>

namespace ZooLib {
namespace RelationalAlgebra {

class Visitor_Expr_Rel_Rename;

// =================================================================================================
// MARK: - Expr_Rel_Rename

class Expr_Rel_Rename
:	public virtual Expr_Rel
,	public virtual ZExpr_Op1_T<Expr_Rel>
	{
	typedef ZExpr_Op1_T<Expr_Rel> inherited;
public:
	Expr_Rel_Rename(const ZRef<Expr_Rel>& iOp0, const ColName& iNew, const ColName& iOld);
	virtual ~Expr_Rel_Rename();

// From ZVisitee
	virtual void Accept(const ZVisitor& iVisitor);

// From ZExpr_Op1_T<Expr_Rel>
	virtual void Accept_Expr_Op1(ZVisitor_Expr_Op1_T<Expr_Rel>& iVisitor);

	virtual ZRef<Expr_Rel> Self();
	virtual ZRef<Expr_Rel> Clone(const ZRef<Expr_Rel>& iOp0);

// Our protocol
	virtual void Accept_Expr_Rel_Rename(Visitor_Expr_Rel_Rename& iVisitor);

	const ColName& GetNew() const;
	const ColName& GetOld() const;

private:
	const ColName fNew;
	const ColName fOld;
	};

// =================================================================================================
// MARK: - Visitor_Expr_Rel_Rename

class Visitor_Expr_Rel_Rename
:	public virtual ZVisitor_Expr_Op1_T<Expr_Rel>
	{
public:
	virtual void Visit_Expr_Rel_Rename(const ZRef<Expr_Rel_Rename>& iExpr);
	};

// =================================================================================================
// MARK: - Relational operators

ZRef<Expr_Rel> sRename(const ZRef<Expr_Rel>& iExpr,
	const ColName& iNewPropName, const ColName& iOldPropName);

} // namespace RelationalAlgebra

template <>
int sCompare_T(const RelationalAlgebra::Expr_Rel_Rename& iL, const RelationalAlgebra::Expr_Rel_Rename& iR);

} // namespace ZooLib

#endif // __ZRA_Expr_Rel_Rename_h__
