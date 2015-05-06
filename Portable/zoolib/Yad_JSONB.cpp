/* -------------------------------------------------------------------------------------------------
Copyright (c) 2012 Andrew Green
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

#include "zoolib/ChanR_Bin_More.h"
#include "zoolib/ChanW_Bin_More.h"
#include "zoolib/Log.h"
#include "zoolib/NameUniquifier.h" // For sName
#include "zoolib/Util_Any.h"
#include "zoolib/Yad_Any.h"
#include "zoolib/Yad_JSONB.h"
#include "zoolib/Yad_Std.h"

#include <map>
#include <set>
#include <vector>

#include "zoolib/ZMACRO_foreach.h"

namespace ZooLib {

static ZRef<YadR> spYadRFromString(const std::string& iVal)
	{
	return sYadR(iVal);
	}

namespace Yad_JSONB {

using std::min;
using std::set;
using std::string;
using std::vector;

namespace { // anonymous

// =================================================================================================
#pragma mark -
#pragma mark Uniqued Names

Name spNameFromChan(const ChanR_Bin& r)
	{
	if (size_t theCount = sReadCount(r))
		return sName(sReadString(theCount, r));
	return Name();
	}

ZRef<YadR> spMakeYadR(ZRef<ReadFilter> iReadFilter, const ZRef<ChannerR_Bin>& iChannerR_Bin);

// =================================================================================================
#pragma mark -
#pragma mark String to/from stream

void spToChan(const string& iString, const ChanW_Bin& w)
	{
	const size_t theLength = iString.length();
	sWriteCountMust(theLength, w);
	if (theLength)
		{
		if (theLength != sQWriteFully(iString.data(), theLength, w))
			sThrow_ExhaustedW();
		}
	}

string spStringFromChan(const ChanR_Bin& r)
	{
	if (size_t theCount = sReadCount(r))
		return sReadString(theCount, r);
	return string();
	}

// =================================================================================================
#pragma mark -
#pragma mark YadStreamerR_JSONB

class YadStreamerR_JSONB
:	public YadStreamerR
,	private ChanR_Bin
	{
public:
	YadStreamerR_JSONB(ZRef<ChannerR_Bin> iChannerR_Bin);

// From YadR
	virtual void Finish();

// From ChannerR via YadStreamerR
	virtual void GetChan(const ChanR_Bin*& oChanPtr);

// From ChanR
	virtual size_t QRead(byte* oDest, size_t iCount);
	virtual size_t Readable();
	virtual uint64 QSkip(uint64 iCount);

private:
	ZRef<ChannerR_Bin> fChannerR_Bin;
	size_t fChunkSize;
	bool fHitEnd;
	};

YadStreamerR_JSONB::YadStreamerR_JSONB(ZRef<ChannerR_Bin> iChannerR_Bin)
:	fChannerR_Bin(iChannerR_Bin)
,	fChunkSize(0)
,	fHitEnd(false)
	{}

void YadStreamerR_JSONB::Finish()
	{ sSkipAll(*this); }

void YadStreamerR_JSONB::GetChan(const ChanR_Bin*& oChanPtr)
	{ oChanPtr = this; }

size_t YadStreamerR_JSONB::QRead(byte* oDest, size_t iCount)
	{
	const ChanR_Bin& r = sGetChan(fChannerR_Bin);

	byte* localDest = oDest;
	while (iCount && not fHitEnd)
		{
		if (fChunkSize == 0)
			{
			fChunkSize = sReadCount(r);
			if (fChunkSize == 0)
				fHitEnd = true;
			}
		else
			{
			const size_t countRead = sQReadFully(localDest, min(iCount, fChunkSize), r);
			localDest += countRead;
			iCount -= countRead;
			fChunkSize -= countRead;
			}
		}
	return localDest - oDest;
	}

size_t YadStreamerR_JSONB::Readable()
	{ return min(fChunkSize, sReadable(sGetChan(fChannerR_Bin))); }

uint64 YadStreamerR_JSONB::QSkip(uint64 iCount)
	{
	const ChanR_Bin& r = sGetChan(fChannerR_Bin);

	uint64 countRemaining = iCount;
	while (countRemaining && not fHitEnd)
		{
		if (fChunkSize == 0)
			{
			fChunkSize = sReadCount(r);
			if (fChunkSize == 0)
				fHitEnd = true;
			}
		else
			{
			const uint64 countSkipped = sSkip(min(countRemaining, uint64(fChunkSize)), r);
			countRemaining -= countSkipped;
			fChunkSize -= countSkipped;
			}
		}

	return iCount - countRemaining;
	}

// =================================================================================================
#pragma mark -
#pragma mark YadSeqR_JSONB

class YadSeqR_JSONB : public YadSeqR_Std
	{
public:
	YadSeqR_JSONB(const ZRef<ReadFilter>& iReadFilter, const ZRef<ChannerR_Bin>& iChannerR_Bin)
	:	fReadFilter(iReadFilter)
	,	fChannerR_Bin(iChannerR_Bin)
		{}

// From YadSeqR_Std
	virtual void Imp_ReadInc(bool iIsFirst, ZRef<YadR>& oYadR)
		{ oYadR = spMakeYadR(fReadFilter, fChannerR_Bin); }

private:
	const ZRef<ReadFilter> fReadFilter;
	const ZRef<ChannerR_Bin> fChannerR_Bin;
	};

// =================================================================================================
#pragma mark -
#pragma mark YadMapR_JSONB

class YadMapR_JSONB : public YadMapR_Std
	{
public:
	YadMapR_JSONB(const ZRef<ReadFilter>& iReadFilter, const ZRef<ChannerR_Bin>& iChannerR_Bin)
	:	fReadFilter(iReadFilter)
	,	fChannerR_Bin(iChannerR_Bin)
		{}

// From YadMapR_Std
	virtual void Imp_ReadInc(bool iIsFirst, Name& oName, ZRef<YadR>& oYadR)
		{
		oName = spNameFromChan(sGetChan(fChannerR_Bin));
		oYadR = spMakeYadR(fReadFilter, fChannerR_Bin);
		}

private:
	const ZRef<ReadFilter> fReadFilter;
	const ZRef<ChannerR_Bin> fChannerR_Bin;
	};

// =================================================================================================
#pragma mark -
#pragma mark Yad

ZRef<YadR> spMakeYadR(ZRef<ReadFilter> iReadFilter, const ZRef<ChannerR_Bin>& iChannerR_Bin)
	{
	const ChanR_Bin& r = sGetChan(iChannerR_Bin);

	if (ZQ<uint8> theTypeQ = sQReadBE<uint8>(r))
		{
		switch (*theTypeQ)
			{
			case 1: return sYadR(Any());
			case 2: return sYadR(Any(false));
			case 3: return sYadR(Any(true));
			case 4: return sYadR(Any(sReadBE<int64>(r)));
			case 5: return sYadR(Any(sReadBE<double>(r)));
			case 7: return new YadStreamerR_JSONB(iChannerR_Bin);
			case 8: return spYadRFromString(spStringFromChan(r));
			case 11: return new YadSeqR_JSONB(iReadFilter, iChannerR_Bin);
			case 13: return new YadMapR_JSONB(iReadFilter, iChannerR_Bin);
			case 254:
				{
				if (iReadFilter)
					{
					if (ZQ<Any> theQ = iReadFilter->QRead(iChannerR_Bin))
						return sYadR(*theQ);
					}
				break;
				}
			case 255:
				{
				// End of list marker.
				return null;
				}
			}
		ZUnimplemented();
		}

	return null;
	}

// =================================================================================================
#pragma mark -
#pragma mark Visitor_ToChan

class Visitor_ToChan
:	public Visitor_Yad
	{
public:
	Visitor_ToChan(ZRef<WriteFilter> iWriteFilter, const ChanW_Bin& iChanW)
	:	fWriteFilter(iWriteFilter)
	,	fW(iChanW)
		{}

// From ZVisitor_Yad
	virtual void Visit_YadR(const ZRef<YadR>& iYadR)
		{
		ZUnimplemented();
		}

	virtual void Visit_YadAtomR(const ZRef<YadAtomR>& iYadAtomR)
		{
		const Any theVal = iYadAtomR->AsAny();

		if (false)
			{}
		else if (theVal.IsNull())
			{
			sWriteBE<uint8>(1, fW);
			}
		else if (const bool* p = theVal.PGet<bool>())
			{
			if (*p)
				sWriteBE<uint8>(3, fW);
			else
				sWriteBE<uint8>(2, fW);
			}
		else if (ZQ<int64> theQ = sQCoerceInt(theVal))
			{
			sWriteBE<uint8>(4, fW);
			sWriteBE<int64>(*theQ, fW);
			}
		else if (ZQ<double> theQ = sQCoerceRat(theVal))
			{
			sWriteBE<uint8>(5, fW);
			sWriteBE<double>(*theQ, fW);
			}
		else
			{
			sWriteBE<uint8>(254, fW);
			if (not fWriteFilter || not fWriteFilter->QWrite(theVal, fW))
				{
				if (ZLOGF(w, eErr))
						w << "Couldn't write " << theVal.Type().name();
				ZUnimplemented();
				}
			}
		}

	virtual void Visit_YadStreamerR(const ZRef<YadStreamerR>& iYadStreamerR)
		{
		const ChanR_Bin& r = sGetChan<ChanR_Bin>(iYadStreamerR);
		sWriteBE<uint8>(7, fW);
		const size_t chunkSize = 64 * 1024;
		vector<uint8> buffer(chunkSize);
		for (;;)
			{
			const size_t countRead = sQReadFully(&buffer[0], chunkSize, r);
			sWriteCountMust(countRead, fW);
			if (!countRead)
				break;
			sWriteMust(&buffer[0], countRead, fW);
			}
		}

	virtual void Visit_YadStrimmerR(const ZRef<YadStrimmerR>& iYadStrimmerR)
		{
		const string8 theString8 = sReadAllUTF8(sGetChan<ChanR_UTF>(iYadStrimmerR));
		sWriteBE<uint8>(8, fW);
		sWriteCountMust(theString8.size(), fW);
		sWriteMust(theString8, fW);
		}

	virtual void Visit_YadSeqR(const ZRef<YadSeqR>& iYadSeqR)
		{
		sWriteBE<uint8>(11, fW);
		while (ZRef<YadR> theChild = iYadSeqR->ReadInc())
			theChild->Accept(*this);
		sWriteBE<uint8>(0xFF, fW); // Terminator
		}

	virtual void Visit_YadMapR(const ZRef<YadMapR>& iYadMapR)
		{
		sWriteBE<uint8>(13, fW);
		Name theName;
		while (ZRef<YadR> theChild = iYadMapR->ReadInc(theName))
			{
			spToChan(theName, fW);
			theChild->Accept(*this);
			}
		sWriteBE<uint8>(0, fW); // Empty name
		sWriteBE<uint8>(0xFF, fW); // Terminator
		}

private:
	ZRef<WriteFilter> fWriteFilter;
	const ChanW_Bin& fW;
	};

} // anonymous namespace

ZRef<YadR> sYadR(ZRef<ReadFilter> iReadFilter, ZRef<ChannerR_Bin> iChannerR_Bin)
	{ return spMakeYadR(iReadFilter, iChannerR_Bin); }

ZRef<YadR> sYadR(ZRef<ChannerR_Bin> iChannerR_Bin)
	{ return spMakeYadR(null, iChannerR_Bin); }

void sToChan(ZRef<WriteFilter> iWriteFilter, ZRef<YadR> iYadR, const ChanW_Bin& w)
	{ iYadR->Accept(Visitor_ToChan(iWriteFilter, w)); }

void sToChan(ZRef<YadR> iYadR, const ChanW_Bin& w)
	{ iYadR->Accept(Visitor_ToChan(null, w)); }

} // namespace Yad_JSONB
} // namespace ZooLib
