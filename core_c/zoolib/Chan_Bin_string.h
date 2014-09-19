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

#ifndef __ZooLib_Chan_Bin_string_h__
#define __ZooLib_Chan_Bin_string_h__ 1
#include "zconfig.h"

#include "zoolib/ChanCount.h"
#include "zoolib/ChanCountSet.h"
#include "zoolib/ChanPos.h"
#include "zoolib/ChanR_Bin.h"
#include "zoolib/ChanU.h"
#include "zoolib/ChanW_Bin.h"

namespace ZooLib {

// =================================================================================================
// MARK: - ChanBase_Bin_string

class ChanBase_Bin_string
:	public ChanR_Bin
,	public ChanCount
,	public ChanPos
	{
public:
	ChanBase_Bin_string(std::string* ioStringPtr);

// From ChanR
	virtual size_t QRead(byte* oDest, size_t iCount);

	virtual size_t Readable();

// From ChanGetCount
	virtual uint64 Count();

// From ChanPos
	virtual uint64 Pos();

	virtual void SetPos(uint64 iPos);

protected:
	std::string* fStringPtr;
	size_t fPosition;
	};

// =================================================================================================
// MARK: - ChanRPos_Bin_string

class ChanRPos_Bin_string
:	public ChanBase_Bin_string
,	public ChanU<byte>
	{
public:
	ChanRPos_Bin_string(const std::string& iString);

// From ChanU
	virtual size_t Unread(const byte* iSource, size_t iCount);
	virtual size_t UnreadableLimit();

	const std::string fString;
	};

// =================================================================================================
// MARK: - ChanRWPos_Bin_string

class ChanRWPos_Bin_string
:	public ChanBase_Bin_string
,	public ChanU<byte>
,	public ChanW_Bin
,	public ChanCountSet
	{
public:
	ChanRWPos_Bin_string(std::string* ioStringPtr);

// From ChanU
	virtual size_t Unread(const byte* iSource, size_t iCount);

	virtual size_t UnreadableLimit();

// From ChanW
	virtual size_t QWrite(const byte* iSource, size_t iCount);

// From ChanCountSet
	virtual void CountSet(uint64 iCount);
	};

} // namespace ZooLib

#endif // __ZooLib_Chan_Bin_string_h__
