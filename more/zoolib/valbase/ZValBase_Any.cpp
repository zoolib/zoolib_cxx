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
#include "zoolib/valbase/ZValBase_Any.h"
#include "zoolib/zqe/ZQE_Result_Any.h"
#include "zoolib/zra/ZRA_Expr_Rel_Concrete.h"

namespace ZooLib {
namespace ZValBase_Any {

// =================================================================================================
#pragma mark -
#pragma mark * Iterator (anonymous)

namespace { // anonymous

class Iterator : public ZQE::Iterator
	{
public:
	Iterator(const ZSeq_Any& iSeq);
	Iterator(const ZSeq_Any& iSeq, size_t iIndex);

	virtual ~Iterator();
	
	virtual ZRef<ZQE::Iterator> Clone();
	virtual ZRef<ZQE::Result> ReadInc();

private:
	const ZSeq_Any fSeq;
	size_t fIndex;
	};

Iterator::Iterator(const ZSeq_Any& iSeq)
:	fSeq(iSeq)
,	fIndex(0)
	{}

Iterator::Iterator(const ZSeq_Any& iSeq, size_t iIndex)
:	fSeq(iSeq)
,	fIndex(iIndex)
	{}

Iterator::~Iterator()
	{}

ZRef<ZQE::Iterator> Iterator::Clone()
	{ return new Iterator(fSeq, fIndex); }

ZRef<ZQE::Result> Iterator::ReadInc()
	{
	if (fIndex < fSeq.Count())
		return new ZQE::Result_Any(fSeq.Get(fIndex++));
	return ZRef<ZQE::Result>();
	}

} // anonymous namespace

// =================================================================================================
#pragma mark -
#pragma mark * Expr_Rel_Concrete

class Expr_Rel_Concrete : public ZValBase::Expr_Rel_Concrete
	{
public:
	Expr_Rel_Concrete(const string8& iName, const ZRA::RelHead& iRelHead, const ZSeq_Any& iSeq);

// From ZRA::Expr_Rel_Concrete via ZValBase::Expr_Rel_Concrete
	virtual ZRA::RelHead GetRelHead();
	virtual string8 GetName();

// From ZValBase::Expr_Rel_Concrete
	virtual ZRef<ZQE::Iterator> MakeIterator();

private:
	const string8 fName;
	const ZRA::RelHead fRelHead;
	const ZSeq_Any fSeq;
	};

Expr_Rel_Concrete::Expr_Rel_Concrete(
	const string8& iName, const ZRA::RelHead& iRelHead, const ZSeq_Any& iSeq)
:	fName(iName)
,	fRelHead(iRelHead)
,	fSeq(iSeq)
	{}

ZRA::RelHead Expr_Rel_Concrete::GetRelHead()
	{ return fRelHead; }

string8 Expr_Rel_Concrete::GetName()
	{ return fName; }

ZRef<ZQE::Iterator> Expr_Rel_Concrete::MakeIterator()
	{ return new Iterator(fSeq); }

// =================================================================================================
#pragma mark -
#pragma mark * ZValBase_Any pseudo constructors

ZRef<ZRA::Expr_Rel> sConcrete(
	const string8& iName, const ZRA::RelHead& iRelHead, const ZSeq_Any& iSeq)
	{ return new Expr_Rel_Concrete(iName, iRelHead, iSeq); }

ZVal_Any sApplyRelHead(const ZRA::RelHead& iRelHead, const ZVal_Any& iVal)
	{
	if (const ZMap_Any* theMap = iVal.PGet_T<ZMap_Any>())
		{
		ZMap_Any newMap;
		for (ZMap_Any::Index_t i = theMap->Begin(); i != theMap->End(); ++i)
			{
			const string8& theName = theMap->NameOf(i);
			if (iRelHead.Contains(theName))
				newMap.Set(theName, theMap->Get(i));
			}
		return newMap;
		}
	return iVal;
	}

} // namespace ZValBase_Any
} // namespace ZooLib
