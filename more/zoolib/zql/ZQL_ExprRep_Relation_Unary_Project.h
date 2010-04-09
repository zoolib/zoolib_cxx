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

#ifndef __ZQL_ExprRep_Relation_Unary_Project__
#define __ZQL_ExprRep_Relation_Unary_Project__ 1
#include "zconfig.h"

#include "zoolib/zql/ZQL_ExprRep_Relation_Unary.h"

NAMESPACE_ZOOLIB_BEGIN
namespace ZQL {

class Visitor_ExprRep_Relation_Unary_Project;

// =================================================================================================
#pragma mark -
#pragma mark * ExprRep_Relation_Unary_Project

class ExprRep_Relation_Unary_Project : public ExprRep_Relation_Unary
	{
public:
	ExprRep_Relation_Unary_Project(
		const ZRef<ExprRep_Relation>& iExprRep_Relation, const ZRelHead& iRelHead);

	virtual ~ExprRep_Relation_Unary_Project();

// From ExprRep_Relation via ExprRep_Relation_Unary
	virtual ZRelHead GetRelHead();

// From ExprRep_Relation_Unary
	virtual void Accept_ExprRep_Relation_Unary(Visitor_ExprRep_Relation_Unary& iVisitor);

	virtual ZRef<ExprRep_Relation_Unary> Clone(ZRef<ExprRep_Relation> iExprRep_Relation);

// Our protocol
	virtual void Accept_ExprRep_Relation_Unary_Project(
		Visitor_ExprRep_Relation_Unary_Project& iVisitor);

	ZRelHead GetProjectRelHead();

private:
	const ZRelHead fRelHead;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Visitor_ExprRep_Relation_Unary_Project

class Visitor_ExprRep_Relation_Unary_Project : public virtual Visitor_ExprRep_Relation_Unary
	{
public:
	virtual void Visit_ExprRep_Relation_Unary_Project(ZRef<ExprRep_Relation_Unary_Project> iRep);
	};

} // namespace ZQL
NAMESPACE_ZOOLIB_END

#endif // __ZQL_ExprRep_Relation_Unary_Project__
