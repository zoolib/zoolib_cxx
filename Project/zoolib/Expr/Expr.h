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

#ifndef __ZooLib_Expr_Expr_h__
#define __ZooLib_Expr_Expr_h__ 1
#include "zconfig.h"

#include "zoolib/Visitor.h"

#include <string>

namespace ZooLib {

class Visitor_Expr;

// =================================================================================================
#pragma mark -
#pragma mark Expr

class Expr : public Visitee
	{
public:
// From Visitee
	virtual void Accept(const Visitor& iVisitor);

// Our protocol
	virtual void Accept_Expr(Visitor_Expr& iVisitor);

	virtual std::string DebugDescription();
	};

// =================================================================================================
#pragma mark -
#pragma mark Visitor_Expr

class Visitor_Expr
:	public virtual Visitor
	{
public:
// Our protocol
	virtual void Visit_Expr(const ZRef<Expr>& iExpr);
	};

} // namespace ZooLib

#endif // __ZooLib_Expr_Expr_h__
