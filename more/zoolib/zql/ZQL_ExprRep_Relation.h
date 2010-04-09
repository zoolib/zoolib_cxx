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
	virtual void Accept_ExprRep(ZVisitor_ExprRep& iVisitor);

// Our protocol
	virtual void Accept_ExprRep_Relation(Visitor_ExprRep_Relation& iVisitor);

	virtual ZRelHead GetRelHead() = 0;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Visitor_ExprRep_Relation

class Visitor_ExprRep_Relation : public virtual ZVisitor_ExprRep
	{
public:
	virtual void Visit_ExprRep_Relation(ZRef<ExprRep_Relation> iRep);
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
