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

#ifndef __ZQE_Iterator_Any__
#define __ZQE_Iterator_Any__ 1
#include "zconfig.h"

#include "zoolib/ZExpr_Logic.h"
#include "zoolib/ZValPred.h"
#include "zoolib/zqe/ZQE_Iterator.h"
#include "zoolib/zqe/ZQE_Visitor_Expr_Rel_DoMakeIterator.h"
#include "zoolib/zra/ZRA_Expr_Rel_Restrict.h"

namespace ZooLib {
namespace ZQE {

// =================================================================================================
#pragma mark -
#pragma mark * Iterator_Any_Project

class Iterator_Any_Project
:	public Iterator
	{
public:
	Iterator_Any_Project(ZRef<Iterator> iIterator, const ZRA::RelHead& iRelHead);

// From Iterator
	virtual ZRef<Iterator> Clone();
	virtual ZRef<Result> ReadInc();

private:
	ZRef<Iterator> fIterator;
	ZRA::RelHead fRelHead;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Iterator_Any_Rename

class Iterator_Any_Rename
:	public Iterator
	{
public:
	Iterator_Any_Rename(ZRef<Iterator> iIterator, const ZRA::RelName& iNew, const ZRA::RelName& iOld);

// From Iterator
	virtual ZRef<Iterator> Clone();
	virtual ZRef<Result> ReadInc();

private:
	ZRef<Iterator> fIterator;
	const ZRA::RelName fNew;
	const ZRA::RelName fOld;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Iterator_Any_Restrict

class Iterator_Any_Restrict
:	public Iterator
	{
public:
	Iterator_Any_Restrict(ZRef<Iterator> iIterator, const ZValPred& iValPred);

// From Iterator
	virtual ZRef<Iterator> Clone();
	virtual ZRef<Result> ReadInc();

private:
	ZRef<Iterator> fIterator;
	const ZValPred fValPred;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Iterator_Any_Select

class Iterator_Any_Select
:	public Iterator
	{
public:
	Iterator_Any_Select(ZRef<Iterator> iIterator, ZRef<ZExpr_Logic> iExpr_Logic);

// From Iterator
	virtual ZRef<Iterator> Clone();
	virtual ZRef<Result> ReadInc();

private:
	ZRef<Iterator> fIterator;
	ZRef<ZExpr_Logic> fExpr_Logic;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Visitor_DoMakeIterator_Any

class Visitor_DoMakeIterator_Any
:	public virtual ZQE::Visitor_Expr_Rel_DoMakeIterator
,	public virtual ZRA::Visitor_Expr_Rel_Restrict
	{
public:
	virtual void Visit_Expr_Rel_Project(ZRef<ZRA::Expr_Rel_Project> iExpr);
	virtual void Visit_Expr_Rel_Rename(ZRef<ZRA::Expr_Rel_Rename> iExpr);
	virtual void Visit_Expr_Rel_Restrict(ZRef<ZRA::Expr_Rel_Restrict> iExpr);
	virtual void Visit_Expr_Rel_Select(ZRef<ZRA::Expr_Rel_Select> iExpr);
	};

} // namespace ZQE
} // namespace ZooLib

#endif // __ZQE_Iterator_Any__
