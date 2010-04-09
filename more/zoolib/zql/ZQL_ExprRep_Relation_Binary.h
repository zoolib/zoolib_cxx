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

#ifndef __ZQL_ExprRep_Relation_Binary__
#define __ZQL_ExprRep_Relation_Binary__ 1
#include "zconfig.h"

#include "zoolib/zql/ZQL_ExprRep_Relation.h"

NAMESPACE_ZOOLIB_BEGIN
namespace ZQL {

class Visitor_ExprRep_Relation_Binary;

// =================================================================================================
#pragma mark -
#pragma mark * ExprRep_Relation_Binary

class ExprRep_Relation_Binary : public ExprRep_Relation
	{
protected:
	ExprRep_Relation_Binary(ZRef<ExprRep_Relation> iLHS, ZRef<ExprRep_Relation> iRHS);

public:
	virtual ~ExprRep_Relation_Binary();

// From ExprRep_Relation
	virtual void Accept_ExprRep_Relation(Visitor_ExprRep_Relation& iVisitor);

// Our protocol
	virtual void Accept_ExprRep_Relation_Binary(Visitor_ExprRep_Relation_Binary& iVisitor);
	
	virtual ZRef<ExprRep_Relation_Binary> Clone (
		ZRef<ExprRep_Relation> iLHS, ZRef<ExprRep_Relation> iRHS) = 0;

	ZRef<ExprRep_Relation> GetLHS();
	ZRef<ExprRep_Relation> GetRHS();

protected:
	const ZRef<ExprRep_Relation> fLHS;
	const ZRef<ExprRep_Relation> fRHS;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Visitor_ExprRep_Relation_Binary

class Visitor_ExprRep_Relation_Binary : public virtual Visitor_ExprRep_Relation
	{
public:
	virtual void Visit_ExprRep_Relation_Binary(ZRef<ExprRep_Relation_Binary> iRep);
	};

} // namespace ZQL
NAMESPACE_ZOOLIB_END

#endif // __ZQL_ExprRep_Relation_Binary__
