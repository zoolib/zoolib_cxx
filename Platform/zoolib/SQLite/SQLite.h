// Copyright (c) 2010 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_SQLite_SQLite_h__
#define __ZooLib_SQLite_SQLite_h__ 1
#include "zconfig.h"

#include "zoolib/Any.h"
#include "zoolib/Counted.h"
#include "zoolib/StdInt.h"
#include "zoolib/UnicodeString.h"

#include <sqlite3.h>

namespace ZooLib {
namespace SQLite {

// =================================================================================================
#pragma mark - SQLite

class DB : public Counted
	{
public:
	DB(const string8& iPath);
	DB(sqlite3* iDB, bool iAdopt);

	virtual ~DB();

	sqlite3* GetDB();

private:
	sqlite3* fDB;
	bool fAdopted;
	};

// =================================================================================================
#pragma mark - Iter

class Iter : public Counted
	{
	Iter(ZP<DB> iDB, const string8& iSQL, uint64 iPosition);

public:
	Iter(ZP<DB> iDB, const string8& iSQL);
	virtual ~Iter();

	ZP<Iter> Clone(bool iRewound);
	void Rewind();
	bool HasValue();
	void Advance();

	size_t Count();
	string8 NameOf(size_t iIndex);
	Any Get(size_t iIndex);

private:
	void pAdvance();

	ZP<DB> fDB;
	const string8 fSQL;
	sqlite3_stmt* fStmt;
	bool fHasValue;
	uint64 fPosition;
	};

} // namespace SQLite
} // namespace ZooLib

#endif // __ZooLib_SQLite_SQLite_h__
