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

#include "zoolib/ZThread.h"
#include "zoolib/ZYad_Any.h"
#include "zoolib/valbase/ZValBase.h"
#include "zoolib/valbase/ZValBase_YadSeqR.h"
#include "zoolib/zqe/ZQE_Result_Any.h"
#include "zoolib/zra/ZRA_Expr_Rel_Concrete.h"

#include <vector>

namespace ZooLib {
namespace ZValBase_YadSeqR {

// =================================================================================================
#pragma mark -
#pragma mark * Expr_Rel_Concrete declaration

class Expr_Rel_Concrete : public ZValBase::Expr_Rel_Concrete
	{
public:
	Expr_Rel_Concrete(const string8& iName, const ZRA::RelHead& iRelHead, ZRef<ZYadSeqR> iYadSeqR);

// From ZRA::Expr_Rel_Concrete via ZValBase::Expr_Rel_Concrete
	virtual ZRA::RelHead GetRelHead();
	virtual string8 GetName();

// From ZValBase::Expr_Rel_Concrete
	virtual ZRef<ZQE::Iterator> MakeIterator();

// Our protocol
	ZRef<ZQE::Result> ReadInc(size_t& ioIndex);

private:
	ZMtx fMtx;
	const string8 fName;
	const ZRA::RelHead fRelHead;
	ZRef<ZYadSeqR> fYadSeqR;
	std::vector<ZRef<ZQE::Result> > fVector;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Iterator

class Iterator : public ZQE::Iterator
	{
public:
	Iterator(ZRef<Expr_Rel_Concrete> iExpr, size_t iIndex);

	virtual ~Iterator();

	virtual ZRef<ZQE::Iterator> Clone();
	virtual ZRef<ZQE::Result> ReadInc();

protected:
	ZRef<Expr_Rel_Concrete> fExpr;
	size_t fIndex;
	};

Iterator::Iterator(ZRef<Expr_Rel_Concrete> iExpr, size_t iIndex)
:	fExpr(iExpr)
,	fIndex(0)
	{}

Iterator::~Iterator()
	{}

ZRef<ZQE::Iterator> Iterator::Clone()
	{ return new Iterator(fExpr, fIndex); }

ZRef<ZQE::Result> Iterator::ReadInc()
	{ return fExpr->ReadInc(fIndex); }

// =================================================================================================
#pragma mark -
#pragma mark * Expr_Rel_Concrete definition

Expr_Rel_Concrete::Expr_Rel_Concrete(
	const string8& iName, const ZRA::RelHead& iRelHead, ZRef<ZYadSeqR> iYadSeqR)
:	fName(iName)
,	fRelHead(iRelHead)
,	fYadSeqR(iYadSeqR)
	{}

ZRA::RelHead Expr_Rel_Concrete::GetRelHead()
	{ return fRelHead; }

string8 Expr_Rel_Concrete::GetName()
	{ return fName; }

ZRef<ZQE::Iterator> Expr_Rel_Concrete::MakeIterator()
	{ return new Iterator(this, 0); }

ZRef<ZQE::Result> Expr_Rel_Concrete::ReadInc(size_t& ioIndex)
	{
	ZAcqMtx acq(fMtx);

	if (ioIndex < fVector.size())
		return fVector.at(ioIndex++);

	if (ZRef<ZYadR> theYadR = fYadSeqR->ReadInc())
		{
		ZRef<ZQE::Result_Any> result = new ZQE::Result_Any(sFromYadR(ZVal_Any(), theYadR));
		if (this->GetRefCount() > 1)
			{
			fVector.push_back(result);
			++ioIndex;
			ZAssert(ioIndex == fVector.size());
			}
		return result;
		}

	return ZRef<ZQE::Result>();
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZValBase_YadSeqR pseudo constructors

ZRef<ZRA::Expr_Rel> sConcrete(
	const string8& iName, const ZRA::RelHead& iRelHead, ZRef<ZYadSeqR> iYadSeqR)
	{
	// Could do a dynamic cast on iYadSeqR to see if it's really a ZYadSeqRPos,
	// in which case returning a ZValBase_YadSeqRPos::Iterator would be a win.
	return new Expr_Rel_Concrete(iName, iYadSeqR);
	}

} // namespace ZValBase_YadSeqR
} // namespace ZooLib
