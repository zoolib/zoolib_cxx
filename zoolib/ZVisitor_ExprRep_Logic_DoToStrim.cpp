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

#include "zoolib/ZVisitor_ExprRep_Logic_DoToStrim.h"

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * ZVisitor_ExprRep_Logic_DoToStrim

ZVisitor_ExprRep_Logic_DoToStrim::ZVisitor_ExprRep_Logic_DoToStrim(
	const Options& iOptions, const ZStrimW& iStrimW)
:	ZVisitor_ExprRep_DoToStrim(iOptions, iStrimW)
	{}

bool ZVisitor_ExprRep_Logic_DoToStrim::Visit_Logic_True(ZRef<ZExprRep_Logic_True> iRep)
	{
	fStrimW << "any";
	return true;
	}

bool ZVisitor_ExprRep_Logic_DoToStrim::Visit_Logic_False(ZRef<ZExprRep_Logic_False> iRep)
	{
	fStrimW << "none";
	return true;
	}

bool ZVisitor_ExprRep_Logic_DoToStrim::Visit_Logic_Not(ZRef<ZExprRep_Logic_Not> iRep)
	{
	fStrimW << "!(";
	this->DoToStrim(iRep->GetOperand());
	fStrimW << ")";
	return true;
	}

bool ZVisitor_ExprRep_Logic_DoToStrim::Visit_Logic_And(ZRef<ZExprRep_Logic_And> iRep)
	{
	fStrimW << "(";
	this->DoToStrim(iRep->GetLHS());
	fStrimW << " & ";
	this->DoToStrim(iRep->GetRHS());
	fStrimW << ")";
	return true;
	}

bool ZVisitor_ExprRep_Logic_DoToStrim::Visit_Logic_Or(ZRef<ZExprRep_Logic_Or> iRep)
	{
	fStrimW << "(";
	this->DoToStrim(iRep->GetLHS());
	fStrimW << " | ";
	this->DoToStrim(iRep->GetRHS());
	fStrimW << ")";
	return true;
	}

NAMESPACE_ZOOLIB_END
