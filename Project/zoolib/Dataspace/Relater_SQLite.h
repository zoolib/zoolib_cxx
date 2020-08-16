// Copyright (c) 2010 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Dataspace_Relater_SQLite_h__
#define __ZooLib_Dataspace_Relater_SQLite_h__ 1
#include "zconfig.h"

#include "zoolib/Dataspace/Relater.h"
#include "zoolib/SQLite/SQLite.h"

namespace ZooLib {
namespace Dataspace {

// =================================================================================================
#pragma mark - Relater_SQLite

class Relater_SQLite : public Relater
	{
public:
	enum { kDebug = 1 };

	Relater_SQLite(ZP<SQLite::DB> iDB);
	virtual ~Relater_SQLite();

	virtual bool Intersects(const RelHead& iRelHead);

	virtual void ModifyRegistrations(
		const AddedQuery* iAdded, size_t iAddedCount,
		const int64* iRemoved, size_t iRemovedCount);

	virtual void CollectResults(std::vector<QueryResult>& oChanged);

private:
	ZP<SQLite::DB> fDB;
	std::map<string8, RelHead> fMap_Tables;

	class DLink_ClientQuery_InPQuery;
	class ClientQuery;
	class PQuery;
	std::map<int64, ClientQuery> fMap_RefconToClientQuery;

	typedef std::map<ZP<RelationalAlgebra::Expr_Rel>, PQuery, Less_Compare_T<ZP<RelationalAlgebra::Expr_Rel> > >
		Map_Rel_PQuery;
	Map_Rel_PQuery fMap_Rel_PQuery;
	};

} // namespace Dataspace
} // namespace ZooLib

#endif // __ZooLib_Dataspace_Relater_SQLite_h__
