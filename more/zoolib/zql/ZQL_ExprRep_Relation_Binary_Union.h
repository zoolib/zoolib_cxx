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

#ifndef __ZQL_ExprRep_Relation_Binary_Union__
#define __ZQL_ExprRep_Relation_Binary_Union__ 1
#include "zconfig.h"

#include "zoolib/zql/ZQL_ExprRep_Relation_Binary.h"

NAMESPACE_ZOOLIB_BEGIN
namespace ZQL {

class Visitor_ExprRep_Relation_Binary_Union;

// =================================================================================================
#pragma mark -
#pragma mark * ExprRep_Relation_Binary_Union

class ExprRep_Relation_Binary_Union : public ExprRep_Relation_Binary
	{
public:
	ExprRep_Relation_Binary_Union(ZRef<ExprRep_Relation> iLHS, ZRef<ExprRep_Relation> iRHS);
	virtual ~ExprRep_Relation_Binary_Union();

// From ExprRep_Relation via ExprRep_Relation_Binary
	virtual ZRelHead GetRelHead();

// From ExprRep_Relation_Binary
	virtual bool Accept_ExprRep_Relation_Binary(Visitor_ExprRep_Relation_Binary& iVisitor);

	virtual ZRef<ExprRep_Relation_Binary> Clone(
		ZRef<ExprRep_Relation> iLHS, ZRef<ExprRep_Relation> iRHS);

// Our protocol
	virtual bool Accept_ExprRep_Relation_Binary_Union(
		Visitor_ExprRep_Relation_Binary_Union& iVisitor);
	};

// =================================================================================================
#pragma mark -
#pragma mark * Visitor_ExprRep_Relation_Binary_Union

class Visitor_ExprRep_Relation_Binary_Union : public virtual Visitor_ExprRep_Relation_Binary
	{
public:
	virtual bool Visit_ExprRep_Relation_Binary_Union(ZRef<ExprRep_Relation_Binary_Union> iRep);
	};

} // namespace ZQL
NAMESPACE_ZOOLIB_END

#endif // __ZQL_ExprRep_Relation_Binary__
