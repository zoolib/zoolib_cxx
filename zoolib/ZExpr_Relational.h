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

#ifndef __ZExpr_Relational__
#define __ZExpr_Relational__ 1
#include "zconfig.h"

#include "zoolib/ZExpr.h"
#include "zoolib/ZRelHead.h"

#include <string>

NAMESPACE_ZOOLIB_BEGIN

class ZVisitor_ExprRep_Relational;

// =================================================================================================
#pragma mark -
#pragma mark * ZExprRep_Relational

class ZExprRep_Relational : public ZExprRep
	{
protected:
	ZExprRep_Relational();

public:
// From ZExprRep
	virtual bool Accept(ZVisitor_ExprRep& iVisitor);

// Our protocol
	virtual bool Accept(ZVisitor_ExprRep_Relational& iVisitor);

	virtual ZRelHead GetRelHead() = 0;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZExprRep_Relational_Dyadic

class ZExprRep_Relational_Dyadic : public ZExprRep_Relational
	{
protected:
	ZExprRep_Relational_Dyadic(ZRef<ZExprRep_Relational> iLHS, ZRef<ZExprRep_Relational> iRHS);
	virtual ~ZExprRep_Relational_Dyadic();

public:
// Our protocol
	ZRef<ZExprRep_Relational> GetLHS();
	ZRef<ZExprRep_Relational> GetRHS();

protected:
	ZRef<ZExprRep_Relational> fLHS;
	ZRef<ZExprRep_Relational> fRHS;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZExprRep_Relational_Difference

class ZExprRep_Relational_Difference : public ZExprRep_Relational_Dyadic
	{
public:
	ZExprRep_Relational_Difference(
		ZRef<ZExprRep_Relational> iLHS, ZRef<ZExprRep_Relational> iRHS);

// From ZExprRep_Relational via ZExprRep_Relational_Dyadic
	virtual bool Accept(ZVisitor_ExprRep_Relational& iVisitor);
	virtual ZRelHead GetRelHead();
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZExprRep_Relational_Intersect

class ZExprRep_Relational_Intersect : public ZExprRep_Relational_Dyadic
	{
public:
	ZExprRep_Relational_Intersect(
		ZRef<ZExprRep_Relational> iLHS, ZRef<ZExprRep_Relational> iRHS);

// From ZExprRep_Relational via ZExprRep_Relational_Dyadic
	virtual bool Accept(ZVisitor_ExprRep_Relational& iVisitor);
	virtual ZRelHead GetRelHead();
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZExprRep_Relational_Join

class ZExprRep_Relational_Join : public ZExprRep_Relational_Dyadic
	{
public:
	ZExprRep_Relational_Join(ZRef<ZExprRep_Relational> iLHS, ZRef<ZExprRep_Relational> iRHS);

// From ZExprRep_Relational via ZExprRep_Relational_Dyadic
	virtual bool Accept(ZVisitor_ExprRep_Relational& iVisitor);
	virtual ZRelHead GetRelHead();
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZExprRep_Relational_Project

class ZExprRep_Relational_Project : public ZExprRep_Relational
	{
public:
	ZExprRep_Relational_Project(ZRef<ZExprRep_Relational> iExpr, const ZRelHead& iRelHead);
	virtual ~ZExprRep_Relational_Project();

// From ZExprRep_Relational
	virtual bool Accept(ZVisitor_ExprRep_Relational& iVisitor);
	virtual ZRelHead GetRelHead();

// Our protocol
	ZRef<ZExprRep_Relational> GetExpr();
	const ZRelHead& GetProjectRelHead();

private:
	ZRef<ZExprRep_Relational> fExpr;
	ZRelHead fRelHead;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZExprRep_Relational_Rename

class ZExprRep_Relational_Rename : public ZExprRep_Relational
	{
public:
	ZExprRep_Relational_Rename(
		ZRef<ZExprRep_Relational> iExpr, const std::string& iOld, const std::string& iNew);

	virtual ~ZExprRep_Relational_Rename();

// From ZExprRep_Relational
	virtual bool Accept(ZVisitor_ExprRep_Relational& iVisitor);
	virtual ZRelHead GetRelHead();

// Our protocol
	ZRef<ZExprRep_Relational> GetExpr();
	const std::string& GetOld();
	const std::string& GetNew();

private:
	ZRef<ZExprRep_Relational> fExpr;
	std::string fOld;
	std::string fNew;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZExprRep_Relational_Union

class ZExprRep_Relational_Union : public ZExprRep_Relational_Dyadic
	{
public:
	ZExprRep_Relational_Union(ZRef<ZExprRep_Relational> iLHS, ZRef<ZExprRep_Relational> iRHS);

// From ZExprRep_Relational via ZExprRep_Relational_Dyadic
	virtual bool Accept(ZVisitor_ExprRep_Relational& iVisitor);
	virtual ZRelHead GetRelHead();
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZVisitor_ExprRep_Relational

class ZVisitor_ExprRep_Relational : public ZVisitor_ExprRep
	{
public:
	virtual bool Visit_Difference(ZRef<ZExprRep_Relational_Difference> iRep);
	virtual bool Visit_Intersect(ZRef<ZExprRep_Relational_Intersect> iRep);
	virtual bool Visit_Join(ZRef<ZExprRep_Relational_Join> iRep);
	virtual bool Visit_Project(ZRef<ZExprRep_Relational_Project> iRep);
	virtual bool Visit_Rename(ZRef<ZExprRep_Relational_Rename> iRep);
	virtual bool Visit_Union(ZRef<ZExprRep_Relational_Union> iRep);
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZExpr_Relational

class ZExpr_Relational : public ZExpr
	{
	typedef ZExpr inherited;
public:
	ZExpr_Relational();
	ZExpr_Relational(const ZExpr_Relational& iOther);
	~ZExpr_Relational();
	ZExpr_Relational& operator=(const ZExpr_Relational& iOther);

	ZExpr_Relational(const ZRef<ZExprRep_Relational>& iRep);

	operator ZRef<ZExprRep_Relational>() const;
	
	ZRelHead GetRelHead() const;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Relational operators

ZExpr_Relational sIntersect(const ZExpr_Relational& iLHS, const ZExpr_Relational& iRHS);

ZExpr_Relational sJoin(const ZExpr_Relational& iLHS, const ZExpr_Relational& iRHS);

ZExpr_Relational sProject(const ZRelHead& iRelHead, const ZExpr_Relational& iExpr);

ZExpr_Relational sRename(const std::string& iOldPropName, const std::string& iNewPropName,
	const ZExpr_Relational& iExpr);

ZExpr_Relational sUnion(const ZExpr_Relational& iLHS, const ZExpr_Relational& iRHS);

ZExpr_Relational operator&(const ZExpr_Relational& iLHS, const ZExpr_Relational& iRHS);

ZExpr_Relational operator|(const ZExpr_Relational& iLHS, const ZExpr_Relational& iRHS);

ZExpr_Relational operator*(const ZExpr_Relational& iLHS, const ZExpr_Relational& iRHS);

NAMESPACE_ZOOLIB_END

#endif // __ZExpr_Relational__
