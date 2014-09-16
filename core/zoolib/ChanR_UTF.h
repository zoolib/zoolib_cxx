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

#ifndef __ZooLib_ChanR_UTF_h__
#define __ZooLib_ChanR_UTF_h__ 1
#include "zconfig.h"

#include "zoolib/ChanR.h"
#include "zoolib/UnicodeString.h"

namespace ZooLib {

using std::pair;

// =================================================================================================
// MARK: - ChanR_UTF32

typedef ChanR<UTF32> ChanR_UTF32;

// =================================================================================================
// MARK: - ChanR_UTF16

class ChanR_UTF16
:	public ChanR<UTF16>
	{
public:
// From ChanR<UTF16>
	virtual size_t Read(UTF16* oDest, size_t iCount)
		{
		size_t countRead;
		this->ReadUTF16(oDest, iCount, &countRead, iCount, nullptr);
		return countRead;
		}

// Our protocol
	virtual void ReadUTF16(UTF16* oDest,
		 size_t iCountCU, size_t* oCountCU, size_t iCountCP, size_t* oCountCP) = 0;
	};

// =================================================================================================
// MARK: - ChanR_UTF8

class ChanR_UTF8
:	public ChanR<UTF8>
	{
public:
// From ChanR<UTF8>
	virtual size_t Read(UTF8* oDest, size_t iCount)
		{
		size_t countRead;
		this->ReadUTF8(oDest, iCount, &countRead, iCount, nullptr);
		return countRead;
		}

// Our protocol
	virtual void ReadUTF8(UTF8* oDest,
		 size_t iCountCU, size_t* oCountCU, size_t iCountCP, size_t* oCountCP) = 0;
	};

// =================================================================================================
// MARK: - ChanR_UTF

class ChanR_UTF
:	public ChanR_UTF32
,	public ChanR_UTF16
,	public ChanR_UTF8
	{
public:
	static void sThrow_Exhausted();

	using ChanR_UTF32::Read;
	using ChanR_UTF32::Skip;
	using ChanR_UTF16::Read;
	using ChanR_UTF16::Skip;
	using ChanR_UTF8::Read;
	using ChanR_UTF8::Skip;
	};

// =================================================================================================
// MARK: -

inline
void sRead(UTF16* oDest,
	 size_t iCountCU, size_t* oCountCU, size_t iCountCP, size_t* oCountCP,
	 const ChanR_UTF& iChanR)
	{ return sNonConst(iChanR).ReadUTF16(oDest, iCountCU, oCountCU, iCountCP, oCountCP); }

inline
void sRead(UTF8* oDest,
	 size_t iCountCU, size_t* oCountCU, size_t iCountCP, size_t* oCountCP,
	 const ChanR_UTF& iChanR)
	{ return sNonConst(iChanR).ReadUTF8(oDest, iCountCU, oCountCU, iCountCP, oCountCP); }

// =================================================================================================
// MARK: -

inline
ZQ<UTF32> sQReadCP(const ChanR_UTF& iChanR)
	{
	UTF32 result;
	if (1 == sRead(&result, 1, iChanR))
		return result;
	return null;
	}

inline
bool sQReadCP(UTF32& oCP, const ChanR_UTF& iChanR)
	{ return sRead(&oCP, 1, iChanR); }

string32 sReadUTF32(size_t iCountCP, const ChanR_UTF& iChanR);
string16 sReadUTF16(size_t iCountCP, const ChanR_UTF& iChanR);
string8 sReadUTF8(size_t iCountCP, const ChanR_UTF& iChanR);

} // namespace ZooLib

#endif // __ZooLib_ChanR_UTF_h__
