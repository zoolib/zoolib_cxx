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

#include "zoolib/ZExpr_Relational.h"
#include "zoolib/ZValCondition.h"

NAMESPACE_ZOOLIB_BEGIN

class ZVisitor_ExprRep_Query;

// =================================================================================================
#pragma mark -
#pragma mark * ZExprRep_Query

class ZExprRep_Query : public ZExprRep_Relational
	{
protected:
	ZExprRep_Query();

public:
// From ZExprRep_Relational
	virtual bool Accept(ZVisitor_ExprRep_Relational& iVisitor);

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

// From ZExprRep_Relational via ZExprRep_Query
	virtual ZRelHead GetRelHead();

// From ZExprRep_Query
	virtual bool Accept(ZVisitor_ExprRep_Query& iVisitor);

// Our protocol
	const std::string& GetIDPropName();
	const ZRelHead& GetAllRelHead();

private:
	std::string fIDPropName;
	ZRelHead fRelHead;
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

// From ZExprRep_Relational via ZExprRep_Query
	virtual ZRelHead GetRelHead();

// From ZExprRep_Query
	virtual bool Accept(ZVisitor_ExprRep_Query& iVisitor);

// Our protocol
	const std::vector<ZVal_Expr>& GetVals();

private:
	std::vector<ZVal_Expr> fVals;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZExprRep_Query_Restrict

class ZExprRep_Query_Restrict : public ZExprRep_Query
	{
public:
	ZExprRep_Query_Restrict(
		const ZValCondition& iValCondition, const ZRef<ZExprRep_Relational>& iExpr);

	virtual ~ZExprRep_Query_Restrict();

// From ZExprRep_Relational via ZExprRep_Query
	virtual ZRelHead GetRelHead();

// From ZExprRep_Query
	virtual bool Accept(ZVisitor_ExprRep_Query& iVisitor);

// Our protocol
	ZValCondition GetValCondition();
	ZRef<ZExprRep_Relational> GetExpr();

private:
	const ZValCondition fValCondition;
	ZRef<ZExprRep_Relational> fExpr;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZExprRep_Query_Select

class ZExprRep_Query_Select : public ZExprRep_Query
	{
public:
	ZExprRep_Query_Select(
		const ZRef<ZExprRep_Logical>& iExpr_Logical,
		const ZRef<ZExprRep_Relational>& iExpr_Relational);

	virtual ~ZExprRep_Query_Select();

// From ZExprRep_Relational via ZExprRep_Query
	virtual ZRelHead GetRelHead();

// From ZExprRep_Query
	virtual bool Accept(ZVisitor_ExprRep_Query& iVisitor);

// Our protocol
	ZRef<ZExprRep_Logical> GetExpr_Logical();
	ZRef<ZExprRep_Relational> GetExpr_Relational();

private:
	ZRef<ZExprRep_Logical> fExpr_Logical;
	ZRef<ZExprRep_Relational> fExpr_Relational;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZVisitor_ExprRep_Query

class ZVisitor_ExprRep_Query : public ZVisitor_ExprRep_Relational
	{
public:
	virtual bool Visit_All(ZRef<ZExprRep_Query_All> iRep);
	virtual bool Visit_Explicit(ZRef<ZExprRep_Query_Explicit> iRep);
	virtual bool Visit_Restrict(ZRef<ZExprRep_Query_Restrict> iRep);
	virtual bool Visit_Select(ZRef<ZExprRep_Query_Select> iRep);
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZExpr_Query

class ZExpr_Query : public ZExpr_Relational
	{
	typedef ZExpr_Relational inherited;
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

ZExpr_Query sSelect(const ZExpr_Relational& iExpr_Relational, const ZExpr_Logical& iExpr_Logical);

ZExpr_Query sSelect(const ZExpr_Logical& iExpr_Logical, const ZExpr_Relational& iExpr_Relational);

ZExpr_Query operator&(const ZExpr_Relational& iExpr_Relational, const ZExpr_Logical& iExpr_Logical);

ZExpr_Query operator&(const ZExpr_Logical& iExpr_Logical, const ZExpr_Relational& iExpr_Relational);

ZExpr_Query operator&(const ZExpr_Relational& iExpr_Relational, const ZValCondition& iValCondition);

ZExpr_Query operator&(const ZValCondition& iValCondition, const ZExpr_Relational& iExpr_Relational);

// =================================================================================================
#pragma mark -
#pragma mark * ZQueryTransformer

class ZQueryTransformer : public ZVisitor_ExprRep_Query
	{
public:
// From ZVisitor_ExprRep_Relational via ZVisitor_ExprRep_Query
	virtual bool Visit_Difference(ZRef<ZExprRep_Relational_Difference> iRep);
	virtual bool Visit_Intersect(ZRef<ZExprRep_Relational_Intersect> iRep);
	virtual bool Visit_Join(ZRef<ZExprRep_Relational_Join> iRep);
	virtual bool Visit_Project(ZRef<ZExprRep_Relational_Project> iRep);
	virtual bool Visit_Rename(ZRef<ZExprRep_Relational_Rename> iRep);
	virtual bool Visit_Union(ZRef<ZExprRep_Relational_Union> iRep);

// From ZVisitor_ExprRep_Query
	virtual bool Visit_All(ZRef<ZExprRep_Query_All> iRep);
	virtual bool Visit_Explicit(ZRef<ZExprRep_Query_Explicit> iRep);
	virtual bool Visit_Restrict(ZRef<ZExprRep_Query_Restrict> iRep);
	virtual bool Visit_Select(ZRef<ZExprRep_Query_Select> iRep);

// Our protocol
	ZRef<ZExprRep_Relational> Transform(ZRef<ZExprRep_Relational> iRep);

	virtual ZRef<ZExprRep_Relational> Transform_All(ZRef<ZExprRep_Query_All> iRep);
	virtual ZRef<ZExprRep_Relational> Transform_Difference(ZRef<ZExprRep_Relational_Difference> iRep);
	virtual ZRef<ZExprRep_Relational> Transform_Explicit(ZRef<ZExprRep_Query_Explicit> iRep);
	virtual ZRef<ZExprRep_Relational> Transform_Intersect(ZRef<ZExprRep_Relational_Intersect> iRep);
	virtual ZRef<ZExprRep_Relational> Transform_Join(ZRef<ZExprRep_Relational_Join> iRep);
	virtual ZRef<ZExprRep_Relational> Transform_Project(ZRef<ZExprRep_Relational_Project> iRep);
	virtual ZRef<ZExprRep_Relational> Transform_Rename(ZRef<ZExprRep_Relational_Rename> iRep);
	virtual ZRef<ZExprRep_Relational> Transform_Restrict(ZRef<ZExprRep_Query_Restrict> iRep);
	virtual ZRef<ZExprRep_Relational> Transform_Select(ZRef<ZExprRep_Query_Select> iRep);
	virtual ZRef<ZExprRep_Relational> Transform_Union(ZRef<ZExprRep_Relational_Union> iRep);

private:
	ZRef<ZExprRep_Relational> fResult;
	};

NAMESPACE_ZOOLIB_END

#endif // __ZExpr_Query__
