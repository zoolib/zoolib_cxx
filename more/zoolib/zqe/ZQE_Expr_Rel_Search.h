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

#ifndef __ZQE_Expr_Rel_Search_h__
#define __ZQE_Expr_Rel_Search_h__ 1
#include "zconfig.h"

#include "zoolib/ZExpr_Bool.h"
#include "zoolib/zra/ZRA_Expr_Rel.h"

namespace ZooLib {
namespace ZQE {

class Visitor_Expr_Rel_Search;

// =================================================================================================
// MARK: - Expr_Rel_Search

class Expr_Rel_Search
:	public virtual ZRA::Expr_Rel
,	public virtual ZExpr_Op0_T<ZRA::Expr_Rel>
	{
	typedef ZExpr_Op0_T<Expr_Rel> inherited;
public:
	Expr_Rel_Search(const ZRA::Rename& iRename, const ZRef<ZExpr_Bool>& iExpr_Bool);

// From ZVisitee
	virtual void Accept(const ZVisitor& iVisitor);

// From ZExpr_Op0_T<Expr_Rel>
	virtual void Accept_Expr_Op0(ZVisitor_Expr_Op0_T<Expr_Rel>& iVisitor);

	virtual ZRef<ZRA::Expr_Rel> Self();
	virtual ZRef<ZRA::Expr_Rel> Clone();

// Our protocol
	virtual void Accept_Expr_Rel_Search(Visitor_Expr_Rel_Search& iVisitor);

	const ZRA::Rename& GetRename() const;
	const ZRef<ZExpr_Bool>& GetExpr_Bool() const;

private:
	const ZRA::Rename fRename;
	const ZRef<ZExpr_Bool> fExpr_Bool;
	};

// =================================================================================================
// MARK: - Visitor_Expr_Rel_Search

class Visitor_Expr_Rel_Search
:	public virtual ZVisitor_Expr_Op0_T<ZRA::Expr_Rel>
	{
public:
	virtual void Visit_Expr_Rel_Search(const ZRef<Expr_Rel_Search>& iExpr);
	};

} // namespace ZQE
} // namespace ZooLib

#endif // __ZQE_Expr_Rel_Search_h__
