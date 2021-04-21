// Copyright (c) 2014 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Chan_UTF_string_h__
#define __ZooLib_Chan_UTF_string_h__ 1
#include "zconfig.h"

#include "zoolib/ChanR_UTF.h"
#include "zoolib/ChanW_UTF.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - ChanRU_UTF_string8

class ChanRU_UTF_string8
:	public ChanRU<UTF32>
	{
public:
	ChanRU_UTF_string8(const string8& iString);
	~ChanRU_UTF_string8();

// From ChanR_UTF
	virtual size_t Read(UTF32* oDest, size_t iCount);

// From ChanU_UTF
	virtual size_t Unread(const UTF32* iSource, size_t iCount);

// Our protocol
	const string8& GetString8() const;

private:
	const string8 fString;
	size_t fPosition;
	};

// =================================================================================================
#pragma mark - ChanW_UTF_string

template <class UTF_p> class ChanW_UTF_string;

// =================================================================================================
#pragma mark - ChanW_UTF_string<UTF32>

template <>
class ChanW_UTF_string<UTF32>
:	public ChanW_UTF_Native32
	{
public:
	ChanW_UTF_string(string32* ioString)
	:	fStringPtr(ioString)
		{}

// From ChanW_UTF_Native32 (aka ChanW_UTF)
	virtual size_t Write(const UTF32* iSource, size_t iCountCU)
		{
		fStringPtr->append(iSource, iCountCU);
		return iCountCU;
		}

protected:
	string32* fStringPtr;
	};

typedef ChanW_UTF_string<UTF32> ChanW_UTF_string32;

// =================================================================================================
#pragma mark - ChanW_UTF_string<UTF16>

template <>
class ChanW_UTF_string<UTF16>
:	public ChanW_UTF_Native16
	{
public:
	ChanW_UTF_string(string16* ioString)
	:	fStringPtr(ioString)
		{}

// From ChanW_UTF_Native16
	virtual size_t WriteUTF16(const UTF16* iSource, size_t iCountCU)
		{
		fStringPtr->append(iSource, iCountCU);
		return iCountCU;
		}

protected:
	string16* fStringPtr;
	};

typedef ChanW_UTF_string<UTF16> ChanW_UTF_string16;

// =================================================================================================
#pragma mark - ChanW_UTF_string<string8>

template <>
class ChanW_UTF_string<UTF8>
:	public ChanW_UTF_Native8
	{
public:
	ChanW_UTF_string()
	:	fStringPtr(new string8)
	,	fAdopted(true)
		{}

	ChanW_UTF_string(string8* ioString)
	:	fStringPtr(ioString)
	,	fAdopted(false)
		{}

	~ChanW_UTF_string()
		{
		if (fAdopted)
			delete fStringPtr;
		}

// From ChanW_UTF_Native8
	virtual size_t WriteUTF8(const UTF8* iSource, size_t iCountCU)
		{
		fStringPtr->append(iSource, iCountCU);
		return iCountCU;
		}

// Our protocol
	const string8& GetString() const
		{ return *fStringPtr; }

	string8& MutString()
		{ return *fStringPtr; }

protected:
	string8* fStringPtr;
	const bool fAdopted;
	};

typedef ChanW_UTF_string<UTF8> ChanW_UTF_string8;

} // namespace ZooLib

#endif // __ZooLib_Chan_UTF_string_h__
