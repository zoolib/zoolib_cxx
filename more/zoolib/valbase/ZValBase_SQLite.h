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
#if 0
#include <sqlite.h>

NAMESPACE_ZOOLIB_BEGIN

namespace ZValBase_SQLite {

// =================================================================================================
#pragma mark -
#pragma mark * ZValBase_SQLite

class Domain : public ZRefCountedWithFinalization
	{
public:
	
//	Expr_Concrete GetConcrete
	
private:
	sqlite* fDB;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZValBase_SQLite

class ZValBase_SQLite
	{
public:
	ZValBase_SQLite(sqlite* iDB);

	// Get list of ZExpr_Physicals representing tables?
	// get list of table names?, then be able to get a ZExpr_Physical for the table?
	
	// then again, we probably want to transform the expr-tree into an sql statement,
	// to be passed to sqlite.

private:
	sqlite* fDB;
	};


} // namespace ZValBase_SQLite
NAMESPACE_ZOOLIB_END
#endif
#endif // __ZValBase_SQLite__
