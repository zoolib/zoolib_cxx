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

#ifndef __ZValBase_SQLite__
#define __ZValBase_SQLite__ 1
#include "zconfig.h"

#include "zoolib/ZCounted.h"
#include "zoolib/ZUnicodeString.h"
#include "zoolib/sqlite/ZSQLite.h"
#include "zoolib/valbase/ZValBase.h"
#include "zoolib/zra/ZRA_Expr_Rel.h"

namespace ZooLib {
namespace ZValBase_SQLite {

// =================================================================================================
#pragma mark -
#pragma mark * ZValBase_SQLite::Domain

class ConcreteDomain : public ZRA::ConcreteDomain
	{
public:
	ConcreteDomain(ZRef<ZSQLite::DB> iDB);

	ZRef<ZSQLite::DB> GetDB();

private:
	ZRef<ZSQLite::DB> fDB;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZValBase_SQLite pseudo constructors

ZRef<ZRA::Expr_Rel> sConcrete_Table(ZRef<ConcreteDomain> iConcreteDomain, const string8& iName);

} // namespace ZValBase_SQLite
} // namespace ZooLib

#endif // __ZValBase_SQLite__
