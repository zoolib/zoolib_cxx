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

#ifndef __ZExpr_Relation__
#define __ZExpr_Relation__ 1
#include "zconfig.h"

#include "zoolib/ZExpr.h"
#include "zoolib/ZRelHead.h"

#include <string>

NAMESPACE_ZOOLIB_BEGIN

class ZVisitor_ExprRep_Relation;

// =================================================================================================
#pragma mark -
#pragma mark * ZExprRep_Relation

class ZExprRep_Relation : public ZExprRep
	{
protected:
	ZExprRep_Relation();

public:
// From ZExprRep
	virtual bool Accept(ZVisitor_ExprRep& iVisitor);

// Our protocol
	virtual bool Accept(ZVisitor_ExprRep_Relation& iVisitor);

	virtual ZRelHead GetRelHead() = 0;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZExprRep_Relation_Dyadic

class ZExprRep_Relation_Dyadic : public ZExprRep_Relation
	{
protected:
	ZExprRep_Relation_Dyadic(ZRef<ZExprRep_Relation> iLHS, ZRef<ZExprRep_Relation> iRHS);
	virtual ~ZExprRep_Relation_Dyadic();

public:
// Our protocol
	ZRef<ZExprRep_Relation> GetLHS();
	ZRef<ZExprRep_Relation> GetRHS();

protected:
	const ZRef<ZExprRep_Relation> fLHS;
	const ZRef<ZExprRep_Relation> fRHS;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZExprRep_Relation_Difference

class ZExprRep_Relation_Difference : public ZExprRep_Relation_Dyadic
	{
public:
	ZExprRep_Relation_Difference(
		ZRef<ZExprRep_Relation> iLHS, ZRef<ZExprRep_Relation> iRHS);

// From ZExprRep_Relation via ZExprRep_Relation_Dyadic
	virtual bool Accept(ZVisitor_ExprRep_Relation& iVisitor);
	virtual ZRelHead GetRelHead();
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZExprRep_Relation_Intersect

class ZExprRep_Relation_Intersect : public ZExprRep_Relation_Dyadic
	{
public:
	ZExprRep_Relation_Intersect(
		ZRef<ZExprRep_Relation> iLHS, ZRef<ZExprRep_Relation> iRHS);

// From ZExprRep_Relation via ZExprRep_Relation_Dyadic
	virtual bool Accept(ZVisitor_ExprRep_Relation& iVisitor);
	virtual ZRelHead GetRelHead();
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZExprRep_Relation_Join

class ZExprRep_Relation_Join : public ZExprRep_Relation_Dyadic
	{
public:
	ZExprRep_Relation_Join(ZRef<ZExprRep_Relation> iLHS, ZRef<ZExprRep_Relation> iRHS);

// From ZExprRep_Relation via ZExprRep_Relation_Dyadic
	virtual bool Accept(ZVisitor_ExprRep_Relation& iVisitor);
	virtual ZRelHead GetRelHead();
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZExprRep_Relation_Project

class ZExprRep_Relation_Project : public ZExprRep_Relation
	{
public:
	ZExprRep_Relation_Project(ZRef<ZExprRep_Relation> iExpr, const ZRelHead& iRelHead);
	virtual ~ZExprRep_Relation_Project();

// From ZExprRep_Relation
	virtual bool Accept(ZVisitor_ExprRep_Relation& iVisitor);
	virtual ZRelHead GetRelHead();

// Our protocol
	ZRef<ZExprRep_Relation> GetExpr();
	const ZRelHead& GetProjectRelHead();

private:
	const ZRef<ZExprRep_Relation> fExpr;
	const ZRelHead fRelHead;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZExprRep_Relation_Rename

class ZExprRep_Relation_Rename : public ZExprRep_Relation
	{
public:
	ZExprRep_Relation_Rename(
		ZRef<ZExprRep_Relation> iExpr, const std::string& iOld, const std::string& iNew);

	virtual ~ZExprRep_Relation_Rename();

// From ZExprRep_Relation
	virtual bool Accept(ZVisitor_ExprRep_Relation& iVisitor);
	virtual ZRelHead GetRelHead();

// Our protocol
	ZRef<ZExprRep_Relation> GetExpr();
	const std::string& GetOld();
	const std::string& GetNew();

private:
	const ZRef<ZExprRep_Relation> fExpr;
	const std::string fOld;
	const std::string fNew;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZExprRep_Relation_Union

class ZExprRep_Relation_Union : public ZExprRep_Relation_Dyadic
	{
public:
	ZExprRep_Relation_Union(ZRef<ZExprRep_Relation> iLHS, ZRef<ZExprRep_Relation> iRHS);

// From ZExprRep_Relation via ZExprRep_Relation_Dyadic
	virtual bool Accept(ZVisitor_ExprRep_Relation& iVisitor);
	virtual ZRelHead GetRelHead();
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZVisitor_ExprRep_Relation

class ZVisitor_ExprRep_Relation : public virtual ZVisitor_ExprRep
	{
public:
	virtual bool Visit_Difference(ZRef<ZExprRep_Relation_Difference> iRep);
	virtual bool Visit_Intersect(ZRef<ZExprRep_Relation_Intersect> iRep);
	virtual bool Visit_Join(ZRef<ZExprRep_Relation_Join> iRep);
	virtual bool Visit_Project(ZRef<ZExprRep_Relation_Project> iRep);
	virtual bool Visit_Rename(ZRef<ZExprRep_Relation_Rename> iRep);
	virtual bool Visit_Union(ZRef<ZExprRep_Relation_Union> iRep);
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZExpr_Relation

class ZExpr_Relation : public ZExpr
	{
	typedef ZExpr inherited;

	ZExpr_Relation operator=(const ZExpr&);
	ZExpr_Relation operator=(const ZRef<ZExprRep>&);

public:
	ZExpr_Relation();
	ZExpr_Relation(const ZExpr_Relation& iOther);
	~ZExpr_Relation();
	ZExpr_Relation& operator=(const ZExpr_Relation& iOther);

	ZExpr_Relation(const ZRef<ZExprRep_Relation>& iRep);

	operator ZRef<ZExprRep_Relation>() const;
	
	ZRelHead GetRelHead() const;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Relational operators

ZExpr_Relation sIntersect(const ZExpr_Relation& iLHS, const ZExpr_Relation& iRHS);

ZExpr_Relation sJoin(const ZExpr_Relation& iLHS, const ZExpr_Relation& iRHS);

ZExpr_Relation sProject(const ZRelHead& iRelHead, const ZExpr_Relation& iExpr);

ZExpr_Relation sRename(const std::string& iOldPropName, const std::string& iNewPropName,
	const ZExpr_Relation& iExpr);

ZExpr_Relation sUnion(const ZExpr_Relation& iLHS, const ZExpr_Relation& iRHS);

ZExpr_Relation operator&(const ZExpr_Relation& iLHS, const ZExpr_Relation& iRHS);

ZExpr_Relation operator|(const ZExpr_Relation& iLHS, const ZExpr_Relation& iRHS);

ZExpr_Relation operator*(const ZExpr_Relation& iLHS, const ZExpr_Relation& iRHS);

NAMESPACE_ZOOLIB_END

#endif // __ZExpr_Relation__
