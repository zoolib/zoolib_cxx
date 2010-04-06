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

#ifndef __ZQL_Expr_All__
#define __ZQL_Expr_All__ 1
#include "zconfig.h"

#include "zoolib/zql/ZQL_Expr_Relation.h"

NAMESPACE_ZOOLIB_BEGIN
namespace ZQL {

// =================================================================================================
#pragma mark -
#pragma mark * ExprRep_All

class ExprRep_All : public ExprRep_Relation
	{
public:
	ExprRep_All(const std::string& iIDPropName);
	ExprRep_All(const ZRelHead& iRelHead);
	ExprRep_All(const std::string& iIDPropName, const ZRelHead& iRelHead);

	virtual ~ExprRep_All();

// From ExprRep_Relation
	virtual bool Accept(Visitor_ExprRep_Relation& iVisitor);

	virtual ZRelHead GetRelHead();

// Our protocol
	const std::string& GetIDPropName();
	const ZRelHead& GetAllRelHead();

private:
	const std::string fIDPropName;
	const ZRelHead fRelHead;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Visitor_ExprRep_All

class Visitor_ExprRep_All : public virtual Visitor_ExprRep_Relation
	{
public:
	virtual bool Visit_All(ZRef<ExprRep_All> iRep);
	};

// =================================================================================================
#pragma mark -
#pragma mark * Query operators

Expr_Relation sAll(const ZRelHead& iRelHead);

Expr_Relation sAllID(const std::string& iIDName);

Expr_Relation sAllID(const std::string& iIDName, const ZRelHead& iRelHead);

}// namespace ZQL
NAMESPACE_ZOOLIB_END

#endif // __ZQL_Expr_All__
