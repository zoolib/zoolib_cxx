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

#include "zoolib/valbase/ZValBase.h"
#include "zoolib/zqe/ZQE_Iterator_Any.h"
#include "zoolib/zqe/ZQE_Result_Any.h"
#include "zoolib/zqe/ZQE_Visitor_Expr_Rel_DoMakeIterator.h"
#include "zoolib/zql/ZQL_Expr_Rel_Concrete.h"
#include "zoolib/zql/ZQL_Expr_Rel_Restrict.h"

NAMESPACE_ZOOLIB_BEGIN
namespace ZValBase {

// =================================================================================================
#pragma mark -
#pragma mark * Visitor_DoMakeIterator

namespace ZANONYMOUS {

class Visitor_DoMakeIterator
:	public virtual ZQE::Visitor_Expr_Rel_DoMakeIterator
,	public virtual ZQL::Visitor_Expr_Rel_Restrict
,	public virtual ZQL::Visitor_Expr_Rel_Concrete
	{
public:
// Via ZQE::Visitor_Expr_Rel_DoMakeIterator
	virtual void Visit_Expr_Rel_Select(ZRef<ZQL::Expr_Rel_Select> iExpr);

// From ZQL::Visitor_Expr_Rel_Restrict
	virtual void Visit_Expr_Rel_Restrict(ZRef<ZQL::Expr_Rel_Restrict> iExpr);

// From ZQL::Visitor_Expr_Rel_Concrete
	virtual void Visit_Expr_Rel_Concrete(ZRef<ZQL::Expr_Rel_Concrete> iExpr);	
	};

void Visitor_DoMakeIterator::Visit_Expr_Rel_Select(ZRef<ZQL::Expr_Rel_Select> iExpr)
	{
	if (ZRef<ZQE::Iterator> theIterator = this->Do(iExpr->GetOp0()))
		this->pSetResult(new ZQE::Iterator_Any_Select(iExpr->GetExpr_Logic(), theIterator));
	}

void Visitor_DoMakeIterator::Visit_Expr_Rel_Restrict(ZRef<ZQL::Expr_Rel_Restrict> iExpr)
	{
	if (ZRef<ZQE::Iterator> theIterator = this->Do(iExpr->GetOp0()))
		this->pSetResult(new ZQE::Iterator_Any_Restrict(iExpr->GetValCondition(), theIterator));
	}

void Visitor_DoMakeIterator::Visit_Expr_Rel_Concrete(ZRef<ZQL::Expr_Rel_Concrete> iExpr)
	{
	if (ZRef<Expr_Rel_Concrete> theExpr = iExpr.DynamicCast<Expr_Rel_Concrete>())
		this->pSetResult(theExpr->MakeIterator());
	}

} // anonymous namespace

// =================================================================================================
#pragma mark -
#pragma mark * Expr_Rel_Concrete_Dummy

class Expr_Rel_Concrete_Dummy : public ZQL::Expr_Rel_Concrete
	{
public:
	Expr_Rel_Concrete_Dummy(const ZQL::RelHead& iRelHead, const string8* iName);

// From ZQL::Expr_Rel_Concrete
	virtual ZQL::RelHead GetRelHead();

	virtual bool GetName(string8& oName);

private:
	const ZQL::RelHead fRelHead;
	bool fNamed;
	string8 fName;
	};

Expr_Rel_Concrete_Dummy::Expr_Rel_Concrete_Dummy(const ZQL::RelHead& iRelHead, const string8* iName)
:	fRelHead(iRelHead)
,	fNamed(false)
	{
	if (iName)
		{
		fNamed = true;
		fName = *iName;
		}
	}

ZQL::RelHead Expr_Rel_Concrete_Dummy::GetRelHead()
	{ return fRelHead; }

bool Expr_Rel_Concrete_Dummy::GetName(string8& oName)
	{
	if (fNamed)
		{
		oName = fName;
		return true;
		}
	return false;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZValBase pseudo constructors

// Put these outside of ZValBase? Perhaps over in ZQL?

ZRef<ZQL::Expr_Rel> sConcrete()
	{ return sConcrete(ZQL::RelHead::sUniversal()); }

ZRef<ZQL::Expr_Rel> sConcrete(const ZQL::RelHead& iRelHead)
	{ return new Expr_Rel_Concrete_Dummy(iRelHead, nullptr); }

ZRef<ZQL::Expr_Rel> sConcrete(const ZQL::RelHead& iRelHead, const string8& iName)
	{ return new Expr_Rel_Concrete_Dummy(iRelHead, &iName); }

ZRef<ZQE::Iterator> sIterator(ZRef<ZQL::Expr_Rel> iExpr)
	{ return Visitor_DoMakeIterator().Do(iExpr); }

} // namespace ZValBase
NAMESPACE_ZOOLIB_END
