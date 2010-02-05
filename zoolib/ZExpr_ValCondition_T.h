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

#ifndef __ZExpr_ValCondition_T__
#define __ZExpr_ValCondition_T__ 1

#include "zconfig.h"

#include "zoolib/ZExpr_Logical.h"
#include "zoolib/ZValCondition_T.h"

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * ZExprRep_ValCondition_T

template <class Val>
class ZExprRep_ValCondition_T : public ZExprRep_Logical
	{
public:
	ZExprRep_ValCondition_T(const ZValCondition_T<Val>& iValCondition);
	virtual ~ZExprRep_ValCondition_T();

// From LogOp
	virtual bool Accept(ZVisitor_ExprRep_Logical& iVisitor);

// Our protocol
	const ZValCondition_T<Val>& GetValCondition();

private:
	const ZValCondition_T<Val> fValCondition;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZVisitor_ExprRep_ValCondition_T

template <class Val>
class ZVisitor_ExprRep_ValCondition_T : public ZVisitor_ExprRep_Logical
	{
public:
	virtual bool Visit_ValCondition(ZRef<ZExprRep_ValCondition_T<Val> > iRep);
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZExpr_ValCondition_T

template <class Val>
class ZExpr_ValCondition_T : public ZExpr_Logical
	{
	typedef ZExpr_Logical inherited;

	ZExpr_ValCondition_T operator=(const ZExpr_Logical&);
	ZExpr_ValCondition_T operator=(const ZRef<ZExprRep_Logical>&);

public:
	ZExpr_ValCondition_T();
	ZExpr_ValCondition_T(const ZExpr_ValCondition_T& iOther);
	~ZExpr_ValCondition_T();
	ZExpr_ValCondition_T& operator=(const ZExpr_ValCondition_T& iOther);

	ZExpr_ValCondition_T(const ZRef<ZExprRep_ValCondition_T<Val> >& iRep);

	operator ZRef<ZExprRep_ValCondition_T<Val> >() const;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Operators

template <class Val>
ZExpr_ValCondition_T<Val> operator<(
	const ZValComparand_T<Val>& iLHS, const ZValComparand_T<Val>& iRHS);

template <class Val>
ZExpr_ValCondition_T<Val> operator<=(
	const ZValComparand_T<Val>& iLHS, const ZValComparand_T<Val>& iRHS);

template <class Val>
ZExpr_ValCondition_T<Val> operator==(
	const ZValComparand_T<Val>& iLHS, const ZValComparand_T<Val>& iRHS);

template <class Val>
ZExpr_ValCondition_T<Val> operator>=(
	const ZValComparand_T<Val>& iLHS, const ZValComparand_T<Val>& iRHS);

template <class Val>
ZExpr_ValCondition_T<Val> operator>(
	const ZValComparand_T<Val>& iLHS, const ZValComparand_T<Val>& iRHS);

template <class Val>
ZExpr_Logical operator&(
	const ZValCondition_T<Val>& iLHS, const ZValCondition_T<Val>& iRHS);

template <class Val>
ZExpr_Logical operator&(const ZValCondition_T<Val>& iLHS, const ZExpr_Logical& iRHS);

template <class Val>
ZExpr_Logical operator&(const ZExpr_Logical& iLHS, const ZValCondition_T<Val>& iRHS);

template <class Val>
ZExpr_Logical& operator&=(ZExpr_Logical& iLHS, const ZValCondition_T<Val>& iRHS);

template <class Val>
ZExpr_Logical operator|(
	const ZValCondition_T<Val>& iLHS, const ZValCondition_T<Val>& iRHS);

template <class Val>
ZExpr_Logical operator|(const ZValCondition_T<Val>& iLHS, const ZExpr_Logical& iRHS);

template <class Val>
ZExpr_Logical operator|(const ZExpr_Logical& iLHS, const ZValCondition_T<Val>& iRHS);

template <class Val>
ZExpr_Logical& operator|=(ZExpr_Logical& iLHS, const ZValCondition_T<Val>& iRHS);

// =================================================================================================
#pragma mark -
#pragma mark * GetRelHead

template <class Val>
ZRelHead sGetRelHead_T(const ZRef<ZExprRep_Logical>& iRep);

template <class Val>
bool sMatches_T(const ZRef<ZExprRep_Logical>& iRep, const Val& iVal);

NAMESPACE_ZOOLIB_END

#endif // __ZExpr_ValCondition_T__
