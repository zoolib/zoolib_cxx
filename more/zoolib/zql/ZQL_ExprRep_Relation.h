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

#ifndef __ZQL_ExprRep_Relation__
#define __ZQL_ExprRep_Relation__ 1
#include "zconfig.h"

#include "zoolib/ZExprRep.h"
#include "zoolib/ZRelHead.h"

#include <string>

NAMESPACE_ZOOLIB_BEGIN
namespace ZQL {

class Visitor_ExprRep_Relation;

// =================================================================================================
#pragma mark -
#pragma mark * ExprRep_Relation

class ExprRep_Relation : public ZExprRep
	{
protected:
	ExprRep_Relation();

public:
// From ZExprRep
	virtual bool Accept_ExprRep(ZVisitor_ExprRep& iVisitor);

// Our protocol
	virtual bool Accept_ExprRep_Relation(Visitor_ExprRep_Relation& iVisitor);

	virtual ZRelHead GetRelHead() = 0;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ExprRep_Relation_Dyadic

class ExprRep_Relation_Dyadic : public ExprRep_Relation
	{
protected:
	ExprRep_Relation_Dyadic(ZRef<ExprRep_Relation> iLHS, ZRef<ExprRep_Relation> iRHS);

public:
	virtual ~ExprRep_Relation_Dyadic();

// Our protocol
	ZRef<ExprRep_Relation> GetLHS();
	ZRef<ExprRep_Relation> GetRHS();

protected:
	const ZRef<ExprRep_Relation> fLHS;
	const ZRef<ExprRep_Relation> fRHS;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ExprRep_Relation_Difference

class ExprRep_Relation_Difference : public ExprRep_Relation_Dyadic
	{
public:
	ExprRep_Relation_Difference(
		ZRef<ExprRep_Relation> iLHS, ZRef<ExprRep_Relation> iRHS);

// From ExprRep_Relation via ExprRep_Relation_Dyadic
	virtual bool Accept_ExprRep_Relation(Visitor_ExprRep_Relation& iVisitor);
	virtual ZRelHead GetRelHead();
	};

// =================================================================================================
#pragma mark -
#pragma mark * ExprRep_Relation_Intersect

class ExprRep_Relation_Intersect : public ExprRep_Relation_Dyadic
	{
public:
	ExprRep_Relation_Intersect(
		ZRef<ExprRep_Relation> iLHS, ZRef<ExprRep_Relation> iRHS);

// From ExprRep_Relation via ExprRep_Relation_Dyadic
	virtual bool Accept_ExprRep_Relation(Visitor_ExprRep_Relation& iVisitor);
	virtual ZRelHead GetRelHead();
	};

// =================================================================================================
#pragma mark -
#pragma mark * ExprRep_Relation_Join

class ExprRep_Relation_Join : public ExprRep_Relation_Dyadic
	{
public:
	ExprRep_Relation_Join(ZRef<ExprRep_Relation> iLHS, ZRef<ExprRep_Relation> iRHS);

// From ExprRep_Relation via ExprRep_Relation_Dyadic
	virtual bool Accept_ExprRep_Relation(Visitor_ExprRep_Relation& iVisitor);
	virtual ZRelHead GetRelHead();
	};

// =================================================================================================
#pragma mark -
#pragma mark * ExprRep_Relation_Project

class ExprRep_Relation_Project : public ExprRep_Relation
	{
public:
	ExprRep_Relation_Project(ZRef<ExprRep_Relation> iExprRep, const ZRelHead& iRelHead);
	virtual ~ExprRep_Relation_Project();

// From ExprRep_Relation
	virtual bool Accept_ExprRep_Relation(Visitor_ExprRep_Relation& iVisitor);
	virtual ZRelHead GetRelHead();

// Our protocol
	ZRef<ExprRep_Relation> GetExprRep();
	const ZRelHead& GetProjectRelHead();

private:
	const ZRef<ExprRep_Relation> fExprRep;
	const ZRelHead fRelHead;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ExprRep_Relation_Rename

class ExprRep_Relation_Rename : public ExprRep_Relation
	{
public:
	ExprRep_Relation_Rename(
		ZRef<ExprRep_Relation> iExprRep, const std::string& iOld, const std::string& iNew);

	virtual ~ExprRep_Relation_Rename();

// From ExprRep_Relation
	virtual bool Accept_ExprRep_Relation(Visitor_ExprRep_Relation& iVisitor);
	virtual ZRelHead GetRelHead();

// Our protocol
	ZRef<ExprRep_Relation> GetExprRep();
	const std::string& GetOld();
	const std::string& GetNew();

private:
	const ZRef<ExprRep_Relation> fExprRep;
	const std::string fOld;
	const std::string fNew;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ExprRep_Relation_Union

class ExprRep_Relation_Union : public ExprRep_Relation_Dyadic
	{
public:
	ExprRep_Relation_Union(ZRef<ExprRep_Relation> iLHS, ZRef<ExprRep_Relation> iRHS);

// From ExprRep_Relation via ExprRep_Relation_Dyadic
	virtual bool Accept_ExprRep_Relation(Visitor_ExprRep_Relation& iVisitor);
	virtual ZRelHead GetRelHead();
	};

// =================================================================================================
#pragma mark -
#pragma mark * Visitor_ExprRep_Relation

class Visitor_ExprRep_Relation : public virtual ZVisitor_ExprRep
	{
public:
	virtual bool Visit_ExprRep_Relation_Difference(ZRef<ExprRep_Relation_Difference> iRep);
	virtual bool Visit_ExprRep_Relation_Intersect(ZRef<ExprRep_Relation_Intersect> iRep);
	virtual bool Visit_ExprRep_Relation_Join(ZRef<ExprRep_Relation_Join> iRep);
	virtual bool Visit_ExprRep_Relation_Project(ZRef<ExprRep_Relation_Project> iRep);
	virtual bool Visit_ExprRep_Relation_Rename(ZRef<ExprRep_Relation_Rename> iRep);
	virtual bool Visit_ExprRep_Relation_Union(ZRef<ExprRep_Relation_Union> iRep);
	};

// =================================================================================================
#pragma mark -
#pragma mark * Relational operators

ZRef<ExprRep_Relation> sIntersect(
	const ZRef<ExprRep_Relation>& iLHS, const ZRef<ExprRep_Relation>& iRHS);

ZRef<ExprRep_Relation> sJoin(
	const ZRef<ExprRep_Relation>& iLHS, const ZRef<ExprRep_Relation>& iRHS);

ZRef<ExprRep_Relation> sProject(const ZRelHead& iRelHead, const ZRef<ExprRep_Relation>& iExpr);

ZRef<ExprRep_Relation> sRename(const std::string& iOldPropName, const std::string& iNewPropName,
	const ZRef<ExprRep_Relation>& iExpr);

ZRef<ExprRep_Relation> sUnion(
	const ZRef<ExprRep_Relation>& iLHS, const ZRef<ExprRep_Relation>& iRHS);

ZRef<ExprRep_Relation> operator&(
	const ZRef<ExprRep_Relation>& iLHS, const ZRef<ExprRep_Relation>& iRHS);

ZRef<ExprRep_Relation> operator&(
	const ZRef<ExprRep_Relation>& iLHS, const ZRef<ExprRep_Relation>& iRHS);

ZRef<ExprRep_Relation> operator*(
	const ZRef<ExprRep_Relation>& iLHS, const ZRef<ExprRep_Relation>& iRHS);

ZRef<ExprRep_Relation> operator|(
	const ZRef<ExprRep_Relation>& iLHS, const ZRef<ExprRep_Relation>& iRHS);

} // namespace ZQL
NAMESPACE_ZOOLIB_END

#endif // __ZQL_ExprRep_Relation__
