/* -------------------------------------------------------------------------------------------------
Copyright (c) 2014 Andrew Green
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

#ifndef __ZooLib_Chan_UTF_string_h__
#define __ZooLib_Chan_UTF_string_h__ 1
#include "zconfig.h"

#include "zoolib/ChanR_UTF.h"
#include "zoolib/ChanU_UTF.h"
#include "zoolib/ChanW_UTF.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark ChanRU_UTF_string8

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

	virtual size_t UnreadableLimit();

// Our protocol
	const string8& GetString8() const;

private:
	const string8 fString;
	size_t fPosition;
	};

// =================================================================================================
#pragma mark -
#pragma mark ChanW_UTF_string

template <class UTF_p> class ChanW_UTF_string;

// =================================================================================================
#pragma mark -
#pragma mark ChanW_UTF_string<string32>

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
#pragma mark -
#pragma mark ChanW_UTF_string<string16>

template <>
class ChanW_UTF_string<UTF16>
:	public ChanW_UTF_Native16
	{
public:
	ChanW_UTF_string(string16* ioString)
	:	fStringPtr(ioString)
		{}

// From ChanW_UTF_Native16
	virtual size_t QWriteUTF16(const UTF16* iSource, size_t iCountCU)
		{
		fStringPtr->append(iSource, iCountCU);
		return iCountCU;
		}

protected:
	string16* fStringPtr;
	};

typedef ChanW_UTF_string<UTF16> ChanW_UTF_string16;

// =================================================================================================
#pragma mark -
#pragma mark ChanW_UTF_string<string8>

template <>
class ChanW_UTF_string<UTF8>
:	public ChanW_UTF_Native8
	{
public:
	ChanW_UTF_string(string8* ioString)
	:	fStringPtr(ioString)
		{}

// From ChanW_UTF_Native8
	virtual size_t QWriteUTF8(const UTF8* iSource, size_t iCountCU)
		{
		fStringPtr->append(iSource, iCountCU);
		return iCountCU;
		}

protected:
	string8* fStringPtr;
	};

typedef ChanW_UTF_string<UTF8> ChanW_UTF_string8;

} // namespace ZooLib

#endif // __ZooLib_Chan_UTF_string_h__
