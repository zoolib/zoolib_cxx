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

#ifndef __ZExpr_Query__
#define __ZExpr_Query__ 1
#include "zconfig.h"

#include "zoolib/ZExpr_Relation.h"
#include "zoolib/ZExpr_Restrict_T.h"
#include "zoolib/ZExpr_Select.h"
#include "zoolib/ZValCondition.h"

NAMESPACE_ZOOLIB_BEGIN

typedef ZExpr_Restrict_T<ZVal_Expr> ZExpr_Restrict;
typedef ZExprRep_Restrict_T<ZVal_Expr> ZExprRep_Restrict;
typedef ZVisitor_ExprRep_Restrict_T<ZVal_Expr> ZVisitor_ExprRep_Restrict;

class ZVisitor_ExprRep_Query;

// =================================================================================================
#pragma mark -
#pragma mark * ZExprRep_Query

class ZExprRep_Query : public ZExprRep_Relation
	{
protected:
	ZExprRep_Query();

public:
// From ZExprRep_Relation
	virtual bool Accept(ZVisitor_ExprRep_Relation& iVisitor);

// Our protocol
	virtual bool Accept(ZVisitor_ExprRep_Query& iVisitor) = 0;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZExprRep_Query_All

class ZExprRep_Query_All : public ZExprRep_Query
	{
public:
	ZExprRep_Query_All(const std::string& iIDPropName);
	ZExprRep_Query_All(const ZRelHead& iRelHead);
	ZExprRep_Query_All(const std::string& iIDPropName, const ZRelHead& iRelHead);

	virtual ~ZExprRep_Query_All();

// From ZExprRep_Relation via ZExprRep_Query
	virtual ZRelHead GetRelHead();

// From ZExprRep_Query
	virtual bool Accept(ZVisitor_ExprRep_Query& iVisitor);

// Our protocol
	const std::string& GetIDPropName();
	const ZRelHead& GetAllRelHead();

private:
	const std::string fIDPropName;
	const ZRelHead fRelHead;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZExprRep_Query_Explicit

class ZExprRep_Query_Explicit : public ZExprRep_Query
	{
public:
	ZExprRep_Query_Explicit(const ZVal_Expr* iVals, size_t iCount);
	ZExprRep_Query_Explicit(const std::vector<ZVal_Expr>& iVals);

	virtual ~ZExprRep_Query_Explicit();

// From ZExprRep_Relation via ZExprRep_Query
	virtual ZRelHead GetRelHead();

// From ZExprRep_Query
	virtual bool Accept(ZVisitor_ExprRep_Query& iVisitor);

// Our protocol
	const std::vector<ZVal_Expr>& GetVals();

private:
	const std::vector<ZVal_Expr> fVals;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZVisitor_ExprRep_Query

class ZVisitor_ExprRep_Query : public virtual ZVisitor_ExprRep_Relation
	{
public:
	virtual bool Visit_All(ZRef<ZExprRep_Query_All> iRep);
	virtual bool Visit_Explicit(ZRef<ZExprRep_Query_Explicit> iRep);
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZExpr_Query

class ZExpr_Query : public ZExpr_Relation
	{
	typedef ZExpr_Relation inherited;

	ZExpr_Query operator=(const ZExpr_Relation&);
	ZExpr_Query operator=(const ZRef<ZExprRep_Relation>&);

public:
	ZExpr_Query();
	ZExpr_Query(const ZExpr_Query& iOther);
	~ZExpr_Query();
	ZExpr_Query& operator=(const ZExpr_Query& iOther);

	ZExpr_Query(const ZRef<ZExprRep_Query>& iRep);

	operator ZRef<ZExprRep_Query>() const;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Query operators

ZExpr_Query sAll(const ZRelHead& iRelHead);

ZExpr_Query sAllID(const std::string& iIDName);

ZExpr_Query sAllID(const std::string& iIDName, const ZRelHead& iRelHead);

ZExpr_Query sExplicit(const ZVal_Expr* iVals, size_t iCount);

ZExpr_Query sExplicit(const std::vector<ZVal_Expr>& iVals);

// =================================================================================================
#pragma mark -
#pragma mark * ZQueryTransformer

class ZQueryTransformer
:	public ZVisitor_ExprRep_Query
,	public ZVisitor_ExprRep_Restrict
,	public ZVisitor_ExprRep_Select
	{
public:
// From ZVisitor_ExprRep_Relation via ZVisitor_ExprRep_Query
	virtual bool Visit_Difference(ZRef<ZExprRep_Relation_Difference> iRep);
	virtual bool Visit_Intersect(ZRef<ZExprRep_Relation_Intersect> iRep);
	virtual bool Visit_Join(ZRef<ZExprRep_Relation_Join> iRep);
	virtual bool Visit_Project(ZRef<ZExprRep_Relation_Project> iRep);
	virtual bool Visit_Rename(ZRef<ZExprRep_Relation_Rename> iRep);
	virtual bool Visit_Union(ZRef<ZExprRep_Relation_Union> iRep);

// From ZVisitor_ExprRep_Query
	virtual bool Visit_All(ZRef<ZExprRep_Query_All> iRep);
	virtual bool Visit_Explicit(ZRef<ZExprRep_Query_Explicit> iRep);

// From ZVisitor_ExprRep_Restrict
	virtual bool Visit_Restrict(ZRef<ZExprRep_Restrict> iRep);

// From ZVisitor_ExprRep_Select
	virtual bool Visit_Select(ZRef<ZExprRep_Select> iRep);

// Our protocol
	ZRef<ZExprRep_Relation> Transform(ZRef<ZExprRep_Relation> iRep);

	virtual ZRef<ZExprRep_Relation> Transform_All(ZRef<ZExprRep_Query_All> iRep);
	virtual ZRef<ZExprRep_Relation> Transform_Difference(ZRef<ZExprRep_Relation_Difference> iRep);
	virtual ZRef<ZExprRep_Relation> Transform_Explicit(ZRef<ZExprRep_Query_Explicit> iRep);
	virtual ZRef<ZExprRep_Relation> Transform_Intersect(ZRef<ZExprRep_Relation_Intersect> iRep);
	virtual ZRef<ZExprRep_Relation> Transform_Join(ZRef<ZExprRep_Relation_Join> iRep);
	virtual ZRef<ZExprRep_Relation> Transform_Project(ZRef<ZExprRep_Relation_Project> iRep);
	virtual ZRef<ZExprRep_Relation> Transform_Rename(ZRef<ZExprRep_Relation_Rename> iRep);
	virtual ZRef<ZExprRep_Relation> Transform_Restrict(ZRef<ZExprRep_Restrict> iRep);
	virtual ZRef<ZExprRep_Relation> Transform_Select(ZRef<ZExprRep_Select> iRep);
	virtual ZRef<ZExprRep_Relation> Transform_Union(ZRef<ZExprRep_Relation_Union> iRep);

private:
	ZRef<ZExprRep_Relation> fResult;
	};

NAMESPACE_ZOOLIB_END

#endif // __ZExpr_Query__
