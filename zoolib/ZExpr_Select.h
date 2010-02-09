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

#ifndef __ZExpr_Select__
#define __ZExpr_Select__ 1
#include "zconfig.h"

#include "zoolib/ZExpr_Logical.h"
#include "zoolib/ZExpr_Relation.h"

NAMESPACE_ZOOLIB_BEGIN

class ZVisitor_ExprRep_Select;

// =================================================================================================
#pragma mark -
#pragma mark * ZExprRep_Select

class ZExprRep_Select : public ZExprRep_Relation
	{
public:
	ZExprRep_Select(
		const ZRef<ZExprRep_Logical>& iExpr_Logical,
		const ZRef<ZExprRep_Relation>& iExpr_Relation);

	virtual ~ZExprRep_Select();

// From ZExprRep_Relation
	virtual bool Accept(ZVisitor_ExprRep_Relation& iVisitor);

	virtual ZRelHead GetRelHead();

// Our protocol
	virtual bool Accept(ZVisitor_ExprRep_Select& iVisitor);

	ZRef<ZExprRep_Logical> GetExpr_Logical();
	ZRef<ZExprRep_Relation> GetExpr_Relation();

private:
	const ZRef<ZExprRep_Logical> fExpr_Logical;
	const ZRef<ZExprRep_Relation> fExpr_Relation;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZVisitor_ExprRep_Select

class ZVisitor_ExprRep_Select : public virtual ZVisitor_ExprRep_Relation
	{
public:
	virtual bool Visit_Select(ZRef<ZExprRep_Select> iRep);
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZExpr_Select

class ZExpr_Select : public ZExpr_Relation
	{
	typedef ZExpr_Relation inherited;

	ZExpr_Select operator=(const ZExpr_Relation&);
	ZExpr_Select operator=(const ZRef<ZExprRep_Relation>&);

public:
	ZExpr_Select();
	ZExpr_Select(const ZExpr_Select& iOther);
	~ZExpr_Select();
	ZExpr_Select& operator=(const ZExpr_Select& iOther);

	ZExpr_Select(const ZRef<ZExprRep_Select>& iRep);

	operator ZRef<ZExprRep_Select>() const;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Query operators

ZExpr_Select sSelect(const ZExpr_Logical& iExpr_Logical, const ZExpr_Relation& iExpr_Relation);

ZExpr_Select operator&(const ZExpr_Logical& iExpr_Logical, const ZExpr_Relation& iExpr_Relation);

ZExpr_Select operator&(const ZExpr_Relation& iExpr_Relation, const ZExpr_Logical& iExpr_Logical);

NAMESPACE_ZOOLIB_END

#endif // __ZExpr_Select__
