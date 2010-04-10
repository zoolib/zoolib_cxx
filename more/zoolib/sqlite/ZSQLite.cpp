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

#include "zoolib/ZData_Any.h"
#include "zoolib/sqlite/ZSQLite.h"

#include <stdexcept>

NAMESPACE_ZOOLIB_BEGIN
namespace ZSQLite {

// =================================================================================================
#pragma mark -
#pragma mark * ZSQLite

DB::DB(const string8& iPath)
:	fDB(nullptr)
	{
	if (SQLITE_OK != ::sqlite3_open(iPath.c_str(), &fDB))
		throw std::runtime_error(std::string(__FUNCTION__) + ", Couldn't open sqlite database");
	}

DB::~DB()
	{
	if (fDB)
		::sqlite3_close(fDB);
	}

sqlite3* DB::GetDB()
	{ return fDB; }

// =================================================================================================
#pragma mark -
#pragma mark * Iter

Iter::Iter(ZRef<DB> iDB, const string8& iSQL, size_t iPosition)
:	fDB(iDB)
,	fSQL(iSQL)
,	fStmt(nullptr)
,	fHasValue(false)
,	fPosition(0)
	{
	const char* tail;
	if (SQLITE_OK != ::sqlite3_prepare_v2(fDB->GetDB(), fSQL.c_str(), -1, &fStmt, &tail))
		{
		fStmt = nullptr;
		}
	else
		{
		while (iPosition--)
			this->pAdvance();
		}
	}

Iter::Iter(ZRef<DB> iDB, const string8& iSQL)
:	fDB(iDB)
,	fSQL(iSQL)
,	fStmt(nullptr)
,	fHasValue(false)
,	fPosition(0)
	{
	const char* tail;
	if (SQLITE_OK != ::sqlite3_prepare_v2(fDB->GetDB(), fSQL.c_str(), -1, &fStmt, &tail))
		fStmt = nullptr;
	}

void Iter::pAdvance()
	{
	ZAssert(fStmt);

	if ((fPosition == 0 || fHasValue) && SQLITE_ROW == ::sqlite3_step(fStmt))
		{
		++fPosition;
		fHasValue = true;
		}
	else
		{
		fHasValue = false;
		}
	}

Iter::~Iter()
	{
	if (fStmt)
		::sqlite3_finalize(fStmt);
	}

ZRef<Iter> Iter::Clone(bool iRewound)
	{
	if (!fStmt)
		return this;

	if (iRewound)
		return new Iter(fDB, fSQL);

	return new Iter(fDB, fSQL, fPosition);
	}

void Iter::Rewind()
	{
	if (fStmt)
		{
		::sqlite3_reset(fStmt);
		fHasValue = false;
		fPosition = 0;
		}
	}

bool Iter::HasValue()
	{
	if (fStmt)
		{
		if (fPosition == 0)
			this->pAdvance();
		return fHasValue;
		}
	return false;
	}

void Iter::Advance()
	{
	if (fStmt)
		this->pAdvance();
	}

size_t Iter::Count()
	{
	if (fStmt)
		return ::sqlite3_column_count(fStmt);
	return 0;
	}

string8 Iter::NameOf(size_t iIndex)
	{
	if (fStmt)
		{
		if (const char* theName = ::sqlite3_column_name(fStmt, iIndex))
			return theName;
		}
	return string8();
	}

ZAny Iter::Get(size_t iIndex)
	{
	if (fStmt)
		{
		switch (::sqlite3_column_type(fStmt, iIndex))
			{
			case SQLITE_INTEGER:
				{
				return ZAny(int64(::sqlite3_column_int64(fStmt, iIndex)));
				}
			case SQLITE_FLOAT:
				{
				return ZAny(::sqlite3_column_double(fStmt, iIndex));
				}
			case SQLITE_TEXT:
				{
				const unsigned char* theText = ::sqlite3_column_text(fStmt, iIndex);
				return ZAny(string8((const char*)theText, ::sqlite3_column_bytes(fStmt, iIndex)));
				}
			case SQLITE_BLOB:
				{
				const void* theData = ::sqlite3_column_blob(fStmt, iIndex);
				return ZAny(ZData_Any(theData, ::sqlite3_column_bytes(fStmt, iIndex)));
				}
			case SQLITE_NULL:
			default:
				break;
			}
		}
	return ZAny();
	}


} // namespace ZSQLite
NAMESPACE_ZOOLIB_END
