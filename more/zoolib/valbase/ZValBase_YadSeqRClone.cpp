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
#include "zoolib/valbase/ZValBase_YadSeqRClone.h"
#include "zoolib/zqe/ZQE_Result_Any.h"
#include "zoolib/zra/ZRA_Expr_Rel_Concrete.h"

namespace ZooLib {
namespace ZValBase_YadSeqRClone {

// =================================================================================================
#pragma mark -
#pragma mark * Iterator

class Iterator : public ZQE::Iterator
	{
public:
	Iterator(ZRef<ZYadSeqRClone> iYadSeqRClone);
	virtual ~Iterator();

	virtual ZRef<ZQE::Iterator> Clone();
	virtual ZRef<ZQE::Result> ReadInc();

protected:
	ZRef<ZYadSeqRClone> fYadSeqRClone;
	};

Iterator::Iterator(ZRef<ZYadSeqRClone> iYadSeqRClone)
:	fYadSeqRClone(iYadSeqRClone)
	{}

Iterator::~Iterator()
	{}

ZRef<ZQE::Iterator> Iterator::Clone()
	{ return new Iterator(fYadSeqRClone->Clone()); }

ZRef<ZQE::Result> Iterator::ReadInc()
	{
	if (ZRef<ZYadR> theYadR = fYadSeqRClone->ReadInc())
		return new ZQE::Result_Any(sFromYadR(ZVal_Any(), theYadR));
	return ZRef<ZQE::Result>();
	}

// =================================================================================================
#pragma mark -
#pragma mark * Expr_Rel_Concrete

class Expr_Rel_Concrete : public ZValBase::Expr_Rel_Concrete
	{
public:
	Expr_Rel_Concrete(ZRef<ZYadSeqRClone> iYadSeqRClone);

// From ZRA::Expr_Rel_Concrete via ZValBase::Expr_Rel_Concrete
	virtual ZRA::RelHead GetRelHead();

// From ZValBase::Expr_Rel_Concrete
	virtual ZRef<ZQE::Iterator> MakeIterator();

private:
	ZRef<ZYadSeqRClone> fYadSeqRClone;
	};

Expr_Rel_Concrete::Expr_Rel_Concrete(ZRef<ZYadSeqRClone> iYadSeqRClone)
:	fYadSeqRClone(iYadSeqRClone)
	{}

ZRA::RelHead Expr_Rel_Concrete::GetRelHead()
	{ return ZRA::RelHead::sUniversal(); }

ZRef<ZQE::Iterator> Expr_Rel_Concrete::MakeIterator()
	{ return new Iterator(fYadSeqRClone); }

// =================================================================================================
#pragma mark -
#pragma mark * ZValBase_YadSeqRClone pseudo constructors

ZRef<ZRA::Expr_Rel> sConcrete(ZRef<ZYadSeqRClone> iYadSeqRClone)
	{ return new Expr_Rel_Concrete(iYadSeqRClone); }

} // namespace ZValBase_YadSeqRClone
} // namespace ZooLib
