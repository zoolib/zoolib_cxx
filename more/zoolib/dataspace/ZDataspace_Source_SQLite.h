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

#ifndef __ZDataspace_Source_SQLite__
#define __ZDataspace_Source_SQLite__ 1
#include "zconfig.h"

#include "zoolib/ZWorker.h"

#include "zoolib/dataspace/ZDataspace_Source.h"
#include "zoolib/sqlite/ZSQLite.h"

namespace ZooLib {
namespace ZDataspace {

// =================================================================================================
#pragma mark -
#pragma mark * Source_SQLite

class Source_SQLite : public Source
	{
public:
	enum { kDebug = 1 };

	Source_SQLite(ZRef<ZSQLite::DB> iDB);
	virtual ~Source_SQLite();

	virtual bool Intersects(const RelHead& iRelHead);

	virtual void ModifyRegistrations(
		const AddedQuery* iAdded, size_t iAddedCount,
		const int64* iRemoved, size_t iRemovedCount);

	virtual void CollectResults(std::vector<QueryResult>& oChanged);

private:
	bool pCheck(ZRef<ZWorker> iWorker);

	ZRef<ZSQLite::DB> fDB;
	int64 fChangeCount;
	ZRef<Clock> fClock;
	std::map<string8, RelHead> fMap_Tables;

	class DLink_ClientQuery_InPQuery;
	class ClientQuery;
	class PQuery;
	std::map<int64, ClientQuery> fMap_RefconToClientQuery;

	typedef std::map<ZRef<ZRA::Expr_Rel>, PQuery, Less_Compare_T<ZRef<ZRA::Expr_Rel> > >
		Map_Rel_PQuery;
	Map_Rel_PQuery fMap_Rel_PQuery;
	class PQuery;
	};

} // namespace ZDataspace
} // namespace ZooLib

#endif // __ZDataspace_Source_SQLite__
