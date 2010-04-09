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

#include "zoolib/ZYad_Any.h"
#include "zoolib/valbase/ZValBase.h"
#include "zoolib/valbase/ZValBase_YadSeqRPos.h"
#include "zoolib/zqe/ZQE_Result_Any.h"
#include "zoolib/zql/ZQL_Expr_Relation_Concrete.h"

NAMESPACE_ZOOLIB_BEGIN
namespace ZValBase_YadSeqRPos {

// =================================================================================================
#pragma mark -
#pragma mark * Iterator

class Iterator : public ZQE::Iterator
	{
public:
	Iterator(ZRef<ZYadSeqRPos> iYadSeqRPos);
	virtual ~Iterator();
	
	virtual ZRef<ZQE::Iterator> Clone();
	virtual ZRef<ZQE::Result> ReadInc();

protected:
	ZRef<ZYadSeqRPos> fYadSeqRPos;
	};

Iterator::Iterator(ZRef<ZYadSeqRPos> iYadSeqRPos)
:	fYadSeqRPos(iYadSeqRPos)
	{}

Iterator::~Iterator()
	{}

ZRef<ZQE::Iterator> Iterator::Clone()
	{ return new Iterator(fYadSeqRPos->Clone()); }

ZRef<ZQE::Result> Iterator::ReadInc()
	{
	if (ZRef<ZYadR> theYadR = fYadSeqRPos->ReadInc())
		return new ZQE::Result_Any(sFromYadR(ZVal_Any(), theYadR));
	return ZRef<ZQE::Result>();
	}

// =================================================================================================
#pragma mark -
#pragma mark * Expr_Relation_Concrete

class Expr_Relation_Concrete : public ZValBase::Expr_Relation_Concrete
	{
public:
	Expr_Relation_Concrete(ZRef<ZYadSeqRPos> iYadSeqRPos);

// From ZValBase::Expr_Relation_Concrete
	virtual ZRef<ZQE::Iterator> MakeIterator();

private:
	ZRef<ZYadSeqRPos> fYadSeqRPos;
	};

Expr_Relation_Concrete::Expr_Relation_Concrete(ZRef<ZYadSeqRPos> iYadSeqRPos)
:	fYadSeqRPos(iYadSeqRPos)
	{}

ZRef<ZQE::Iterator> Expr_Relation_Concrete::MakeIterator()
	{ return new Iterator(fYadSeqRPos); }

// =================================================================================================
#pragma mark -
#pragma mark * ZValBase_YadSeqRPos pseudo constructors

ZRef<ZQL::Expr_Relation> sConcrete(ZRef<ZYadSeqRPos> iYadSeqRPos)
	{ return new Expr_Relation_Concrete(iYadSeqRPos); }

} // namespace ZValBase_YadSeqRPos
NAMESPACE_ZOOLIB_END
