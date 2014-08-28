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

#ifndef __ZooLib_ChanR_UTFR_h__
#define __ZooLib_ChanR_UTFR_h__ 1
#include "zconfig.h"

#include "zoolib/ChanR.h"
#include "zoolib/ZUnicode.h"

namespace ZooLib {

typedef ChanR<UTF32> ChanR_UTF32;

//Maybe we derive ChanR_UTF8 from ChanR_UTF32 -- the 32 bit one has is the ChanR interface,
//8 and 16 have the CP and CU limiting things.
//
//class ChanR_UTF8
//:	public ChanR<UTF32>
//,	public ChanR<UTF8>
//	{
//	// Extended with CP and CU accessors?
//	};
//
//class ChanR_UTF16
//:	public ChanR<UTF16>
//,	public ChanR<UTF16>
//	{};
//

typedef ChanR<UTF16> ChanR_UTF16;
typedef ChanR<UTF8> ChanR_UTF8;

// =================================================================================================
// MARK: -

class ChanR_UTF
:	public ChanR_UTF32
,	public ChanR_UTF16
,	public ChanR_UTF8
	{
	// is this where we have the CP/CU variants of reads?
	// In fact those should all be external helper functions
	}

// =================================================================================================
// MARK: -

class ChanR_UTF_Native32
:	public ChanR_UTF
	{
public:
	virtual size_t Read(UTF32* oDest, size_t iCountCU) = 0;
	virtual size_t Read(UTF16* oDest, size_t iCountCU);
	virtual size_t Read(UTF8* oDest, size_t iCountCU);
	}

// =================================================================================================
// MARK: -

class ChanR_UTF_Native16
:	public ChanR_UTF
	{
public:
	virtual size_t Read(UTF32* oDest, size_t iCountCU);
	virtual size_t Read(UTF16* oDest, size_t iCountCU) = 0;
	virtual size_t Read(UTF8* oDest, size_t iCountCU);
	}

// =================================================================================================
// MARK: -

class ChanR_UTF_Native8
:	public ChanR_UTFR
	{
public:
	virtual size_t Read(UTF32* oDest, size_t iCountCU);
	virtual size_t Read(UTF16* oDest, size_t iCountCU);
	virtual size_t Read(UTF8* oDest, size_t iCountCU) = 0;
	}

// =================================================================================================
// MARK: -

// =================================================================================================
// MARK: -

string32 sReadUTF32(size_t iCountCP);
string16 sReadUTF16(size_t iCountCP);
string8 sReadUTF8(size_t iCountCP);


/** \name Read a single code point
*/	//@{

EndOfStrim exceptions?	

ZQ<UTF32> sQReadCP(const ChanR_UTF& iChanR);
UTF32 sReadCP(const ChanR_UTF& iChanR);
//@}


/** \name Standard library strings
*/	//@{
string32 ReadUTF32(size_t iCountCP) const;
string16 ReadUTF16(size_t iCountCP) const;
string8 ReadUTF8(size_t iCountCP) const;
//@}


/** \name String buffers
*/	//@{
void Read(UTF32* oDest, size_t iCount, size_t* oCount) const;
void Read(UTF16* oDest, size_t iCountCU, size_t* oCountCU) const;
void Read(UTF8* oDest, size_t iCountCU, size_t* oCountCU) const;
//@}


/** \name String buffers, limiting and reporting both CU and CP
*/	//@{
void Read(UTF16* oDest,
	size_t iCountCU, size_t* oCountCU, size_t iCountCP, size_t* oCountCP) const;

void Read(UTF8* oDest,
	size_t iCountCU, size_t* oCountCU, size_t iCountCP, size_t* oCountCP) const;
//@}

string8 ReadAll8() const;
string16 ReadAll16() const;
string32 ReadAll32() const;

/** \name Copy To
Read code points from this strim and write them to iStrimW.
*/	//@{
const ZStrimR& CopyAllTo(const ZStrimW& iStrimW) const;

const ZStrimR& CopyAllTo(const ZStrimW& iStrimW,
	uint64* oCountCPRead, uint64* oCountCPWritten) const;

const ZStrimR& CopyTo(const ZStrimW& iStrimW, uint64 iCountCP) const;

const ZStrimR& CopyTo(const ZStrimW& iStrimW, uint64 iCountCP,
	uint64* oCountCPRead, uint64* oCountCPWritten) const;
//@}


/** \name Skip
Skip over code points from this strim without returning them to the caller.
*/	//@{
void Skip(uint64 iCountCP) const;
void Skip(uint64 iCountCP, uint64* oCountCPSkipped) const;
void SkipAll() const;
void SkipAll(uint64* oCountCPSkipped) const;
//@}


/** \name Essential overrides
These methods must be overridden by subclasses.
*/	//@{
virtual void Imp_ReadUTF32(UTF32* oDest, size_t iCount, size_t* oCount) = 0;
//@}



} // namespace ZooLib

#endif // __ZooLib_ChanR_UTFR_h__
