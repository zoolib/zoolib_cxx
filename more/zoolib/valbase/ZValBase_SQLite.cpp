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
#include "zoolib/valbase/ZValBase_SQLite.h"
#include "zoolib/zqe/ZQE_Result_Any.h"

NAMESPACE_ZOOLIB_BEGIN
namespace ZValBase_SQLite {

using namespace ZSQLite;

// =================================================================================================
#pragma mark -
#pragma mark * Anonymous Iterator

namespace ZANONYMOUS {

class Iterator : public ZQE::Iterator
	{
public:
	Iterator(ZRef<Iter> iIter);

	virtual ~Iterator();
	
	virtual ZRef<ZQE::Iterator> Clone();
	virtual ZRef<ZQE::Result> ReadInc();

protected:
	ZRef<Iter> fIter;
	};

Iterator::Iterator(ZRef<Iter> iIter)
:	fIter(iIter)
	{}

Iterator::~Iterator()
	{}

ZRef<ZQE::Iterator> Iterator::Clone()
	{ return new Iterator(fIter->Clone(false)); }

ZRef<ZQE::Result> Iterator::ReadInc()
	{
	if (fIter->HasValue())
		{
		ZMap_Any theMap;
		for (size_t x = 0; x < fIter->Count(); ++x)
			theMap.Set(fIter->NameOf(x), fIter->Get(x));
		fIter->Advance();
		return new ZQE::Result_Any(theMap);
		}
	return nullref;
	}

} // anonymous namespace

// =================================================================================================
#pragma mark -
#pragma mark * Anonymous Expr_Rel_Concrete

namespace ZANONYMOUS {

class Expr_Rel_Concrete : public ZValBase::Expr_Rel_Concrete
	{
public:
	Expr_Rel_Concrete(ZRef<ConcreteDomain> iConcreteDomain, ZRef<Iter> iIter);

// From ExprRel via ZValBase::Expr_Rel_Concrete
	virtual ZRelHead GetRelHead();

// From ZValBase::Expr_Rel_Concrete
	virtual ZRef<ZQE::Iterator> MakeIterator();

private:
	ZRef<ConcreteDomain> fConcreteDomain;
	ZRef<Iter> fIter;
	};

Expr_Rel_Concrete::Expr_Rel_Concrete(ZRef<ConcreteDomain> iConcreteDomain, ZRef<Iter> iIter)
:	fConcreteDomain(iConcreteDomain)
,	fIter(iIter)
	{}

ZRelHead Expr_Rel_Concrete::GetRelHead()
	{
	ZRelHead result;
	for (size_t x = 0; x < fIter->Count(); ++x)
		result.Insert(fIter->NameOf(x));
	return result;
	}

ZRef<ZQE::Iterator> Expr_Rel_Concrete::MakeIterator()
	{ return new Iterator(fIter->Clone(true)); }

} // anonymous namespace

// =================================================================================================
#pragma mark -
#pragma mark * ZValBase_SQLite::ConcreteDomain

ConcreteDomain::ConcreteDomain(ZRef<ZSQLite::DB> iDB)
:	fDB(iDB)
	{}

ZRef<ZSQLite::DB> ConcreteDomain::GetDB()
	{ return fDB; }

// =================================================================================================
#pragma mark -
#pragma mark * ZValBase_SQLite pseudo constructors

ZRef<ZQL::Expr_Rel> sConcrete_Table(ZRef<ConcreteDomain> iConcreteDomain, const string8& iName)
	{
	ZRef<Iter> iter = new Iter(iConcreteDomain->GetDB(), "pragma table_info(" + iName + ");");
	return new Expr_Rel_Concrete(iConcreteDomain, iter);
	}

ZRef<ZQL::Expr_Rel> sConcrete_SQL(ZRef<ConcreteDomain> iConcreteDomain, const string8& iSQL)
	{
	ZRef<Iter> iter = new Iter(iConcreteDomain->GetDB(), iSQL);
	return new Expr_Rel_Concrete(iConcreteDomain, iter);
	}


} // namespace ZValBase_SQLite
NAMESPACE_ZOOLIB_END
