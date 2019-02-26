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

#include "zoolib/ChanR_Bin.h"
#include "zoolib/ChanW_Bin.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - ChanW_Bin_string

class ChanW_Bin_string
:	public ChanW_Bin
	{
public:
	ChanW_Bin_string(std::string* ioString);

	virtual size_t Write(const byte* iSource, size_t iCountCU);

protected:
	std::string* fStringPtr;
	};

// =================================================================================================
#pragma mark - ChanRPos_Bin_string

class ChanRPos_Bin_string
:	public ChanRPos<byte>
	{
public:
	ChanRPos_Bin_string(const std::string& iString);

// From ChanR
	virtual size_t Read(byte* oDest, size_t iCount);
	virtual size_t Readable();

// From ChanPos
	virtual uint64 Pos();

	virtual void PosSet(uint64 iPos);

// From ChanSize
	virtual uint64 Size();

// From ChanU
	virtual void Unread(const byte* iSource, size_t iCount);
//	virtual size_t UnreadableLimit();

private:
	const std::string fString;
	size_t fPosition;
	};

// =================================================================================================
#pragma mark - ChanRWPos_Bin_string

class ChanRWPos_Bin_string
:	public ChanRWPos<byte>
	{
public:
	ChanRWPos_Bin_string(std::string* ioStringPtr);

// From ChanR
	virtual size_t Read(byte* oDest, size_t iCount);
	virtual size_t Readable();

// From ChanPos
	virtual uint64 Pos();

	virtual void PosSet(uint64 iPos);

// From ChanSize
	virtual uint64 Size();

// From ChanSizeSet
	virtual void SizeSet(uint64 iSize);

// From ChanU
	virtual void Unread(const byte* iSource, size_t iCount);

//	virtual size_t UnreadableLimit();

// From ChanW
	virtual size_t Write(const byte* iSource, size_t iCount);

private:
	std::string* fStringPtr;
	size_t fPosition;
	};

} // namespace ZooLib

#endif // __ZooLib_Chan_Bin_string_h__
