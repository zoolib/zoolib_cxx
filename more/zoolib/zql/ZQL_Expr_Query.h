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

#ifndef __ZQL_Expr_Query__
#define __ZQL_Expr_Query__ 1
#include "zconfig.h"

#include "zoolib/ZValCondition.h"

#include "zoolib/zql/ZQL_Expr_Relation.h"
#include "zoolib/zql/ZQL_Expr_Restrict_T.h"
#include "zoolib/zql/ZQL_Expr_Select.h"

NAMESPACE_ZOOLIB_BEGIN
namespace ZQL {

typedef Expr_Restrict_T<ZVal_Expr> Expr_Restrict;
typedef ExprRep_Restrict_T<ZVal_Expr> ExprRep_Restrict;
typedef Visitor_ExprRep_Restrict_T<ZVal_Expr> Visitor_ExprRep_Restrict;

class Visitor_ExprRep_Query;

// =================================================================================================
#pragma mark -
#pragma mark * ExprRep_Query

class ExprRep_Query : public ExprRep_Relation
	{
protected:
	ExprRep_Query();

public:
// From ExprRep_Relation
	virtual bool Accept(Visitor_ExprRep_Relation& iVisitor);

// Our protocol
	virtual bool Accept(Visitor_ExprRep_Query& iVisitor) = 0;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ExprRep_Query_All

class ExprRep_Query_All : public ExprRep_Query
	{
public:
	ExprRep_Query_All(const std::string& iIDPropName);
	ExprRep_Query_All(const ZRelHead& iRelHead);
	ExprRep_Query_All(const std::string& iIDPropName, const ZRelHead& iRelHead);

	virtual ~ExprRep_Query_All();

// From ExprRep_Relation via ExprRep_Query
	virtual ZRelHead GetRelHead();

// From ExprRep_Query
	virtual bool Accept(Visitor_ExprRep_Query& iVisitor);

// Our protocol
	const std::string& GetIDPropName();
	const ZRelHead& GetAllRelHead();

private:
	const std::string fIDPropName;
	const ZRelHead fRelHead;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ExprRep_Query_Explicit

class ExprRep_Query_Explicit : public ExprRep_Query
	{
public:
	ExprRep_Query_Explicit(const ZVal_Expr* iVals, size_t iCount);
	ExprRep_Query_Explicit(const std::vector<ZVal_Expr>& iVals);

	virtual ~ExprRep_Query_Explicit();

// From ExprRep_Relation via ExprRep_Query
	virtual ZRelHead GetRelHead();

// From ExprRep_Query
	virtual bool Accept(Visitor_ExprRep_Query& iVisitor);

// Our protocol
	const std::vector<ZVal_Expr>& GetVals();

private:
	const std::vector<ZVal_Expr> fVals;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Visitor_ExprRep_Query

class Visitor_ExprRep_Query : public virtual Visitor_ExprRep_Relation
	{
public:
	virtual bool Visit_All(ZRef<ExprRep_Query_All> iRep);
	virtual bool Visit_Explicit(ZRef<ExprRep_Query_Explicit> iRep);
	};

// =================================================================================================
#pragma mark -
#pragma mark * Expr_Query

class Expr_Query : public Expr_Relation
	{
	typedef Expr_Relation inherited;

	Expr_Query operator=(const Expr_Relation&);
	Expr_Query operator=(const ZRef<ExprRep_Relation>&);

public:
	Expr_Query();
	Expr_Query(const Expr_Query& iOther);
	~Expr_Query();
	Expr_Query& operator=(const Expr_Query& iOther);

	Expr_Query(const ZRef<ExprRep_Query>& iRep);

	operator ZRef<ExprRep_Query>() const;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Query operators

Expr_Query sAll(const ZRelHead& iRelHead);

Expr_Query sAllID(const std::string& iIDName);

Expr_Query sAllID(const std::string& iIDName, const ZRelHead& iRelHead);

Expr_Query sExplicit(const ZVal_Expr* iVals, size_t iCount);

Expr_Query sExplicit(const std::vector<ZVal_Expr>& iVals);

// =================================================================================================
#pragma mark -
#pragma mark * QueryTransformer

class QueryTransformer
:	public Visitor_ExprRep_Query
,	public Visitor_ExprRep_Restrict
,	public Visitor_ExprRep_Select
	{
public:
// From Visitor_ExprRep_Relation via Visitor_ExprRep_Query
	virtual bool Visit_Difference(ZRef<ExprRep_Relation_Difference> iRep);
	virtual bool Visit_Intersect(ZRef<ExprRep_Relation_Intersect> iRep);
	virtual bool Visit_Join(ZRef<ExprRep_Relation_Join> iRep);
	virtual bool Visit_Project(ZRef<ExprRep_Relation_Project> iRep);
	virtual bool Visit_Rename(ZRef<ExprRep_Relation_Rename> iRep);
	virtual bool Visit_Union(ZRef<ExprRep_Relation_Union> iRep);

// From Visitor_ExprRep_Query
	virtual bool Visit_All(ZRef<ExprRep_Query_All> iRep);
	virtual bool Visit_Explicit(ZRef<ExprRep_Query_Explicit> iRep);

// From Visitor_ExprRep_Restrict
	virtual bool Visit_Restrict(ZRef<ExprRep_Restrict> iRep);

// From Visitor_ExprRep_Select
	virtual bool Visit_Select(ZRef<ExprRep_Select> iRep);

// Our protocol
	ZRef<ExprRep_Relation> Transform(ZRef<ExprRep_Relation> iRep);

	virtual ZRef<ExprRep_Relation> Transform_All(ZRef<ExprRep_Query_All> iRep);
	virtual ZRef<ExprRep_Relation> Transform_Difference(ZRef<ExprRep_Relation_Difference> iRep);
	virtual ZRef<ExprRep_Relation> Transform_Explicit(ZRef<ExprRep_Query_Explicit> iRep);
	virtual ZRef<ExprRep_Relation> Transform_Intersect(ZRef<ExprRep_Relation_Intersect> iRep);
	virtual ZRef<ExprRep_Relation> Transform_Join(ZRef<ExprRep_Relation_Join> iRep);
	virtual ZRef<ExprRep_Relation> Transform_Project(ZRef<ExprRep_Relation_Project> iRep);
	virtual ZRef<ExprRep_Relation> Transform_Rename(ZRef<ExprRep_Relation_Rename> iRep);
	virtual ZRef<ExprRep_Relation> Transform_Restrict(ZRef<ExprRep_Restrict> iRep);
	virtual ZRef<ExprRep_Relation> Transform_Select(ZRef<ExprRep_Select> iRep);
	virtual ZRef<ExprRep_Relation> Transform_Union(ZRef<ExprRep_Relation_Union> iRep);

private:
	ZRef<ExprRep_Relation> fResult;
	};

}// namespace ZQL
NAMESPACE_ZOOLIB_END

#endif // __ZQL_Expr_Query__
