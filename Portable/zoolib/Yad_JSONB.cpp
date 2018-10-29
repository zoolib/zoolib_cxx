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
#include "zoolib/Coerce_Any.h"
#include "zoolib/Log.h"
#include "zoolib/NameUniquifier.h" // For sName
#include "zoolib/Stringf.h"
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
		return sName(sReadString(r, theCount));
	return Name();
	}

ZRef<YadR> spMakeYadR(ZRef<ReadFilter> iReadFilter, const ZRef<ChannerR_Bin>& iChannerR_Bin);

// =================================================================================================
#pragma mark -
#pragma mark String to/from stream

void spToChan(const ChanW_Bin& w, const string& iString)
	{
	const size_t theLength = iString.length();
	sEWriteCount(w, theLength);
	if (theLength)
		{
		if (theLength != sWriteMemFully(w, iString.data(), theLength))
			sThrow_ExhaustedW();
		}
	}

string spStringFromChan(const ChanR_Bin& r)
	{
	if (size_t theCount = sReadCount(r))
		return sReadString(r, theCount);
	return string();
	}

// =================================================================================================
#pragma mark -
#pragma mark YadStreamerR_JSONB

class YadStreamerR_JSONB
:	public YadStreamerR
	{
public:
	YadStreamerR_JSONB(ZRef<ChannerR_Bin> iChannerR_Bin);

// From YadR
	virtual void Finish();

// From ChanR
	virtual size_t Read(byte* oDest, size_t iCount);
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


size_t YadStreamerR_JSONB::Read(byte* oDest, size_t iCount)
	{
	const ChanR_Bin& r = *fChannerR_Bin;

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
			const size_t countRead = sReadFully(r, localDest, min(iCount, fChunkSize));
			localDest += countRead;
			iCount -= countRead;
			fChunkSize -= countRead;
			}
		}
	return localDest - oDest;
	}

size_t YadStreamerR_JSONB::Readable()
	{ return min(fChunkSize, sReadable(*fChannerR_Bin)); }

uint64 YadStreamerR_JSONB::QSkip(uint64 iCount)
	{
	const ChanR_Bin& r = *fChannerR_Bin;

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
			const uint64 countSkipped = sSkip(r, min(countRemaining, uint64(fChunkSize)));
			countRemaining -= countSkipped;
			fChunkSize -= countSkipped;
			}
		}

	return iCount - countRemaining;
	}

// =================================================================================================
#pragma mark -
#pragma mark YadSeqR_JSONB

class ChanR_RefYad_JSONB
:	public ChanR_RefYad_Std
	{
public:
	ChanR_RefYad_JSONB(const ZRef<ReadFilter>& iReadFilter, const ZRef<ChannerR_Bin>& iChannerR_Bin)
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

typedef Channer_T<ChanR_RefYad_JSONB> YadSeqR_JSONB;

// =================================================================================================
#pragma mark -
#pragma mark YadMapR_JSONB

class ChanR_NameRefYad_JSONB
:	public ChanR_NameRefYad_Std
	{
public:
	ChanR_NameRefYad_JSONB(const ZRef<ReadFilter>& iReadFilter, const ZRef<ChannerR_Bin>& iChannerR_Bin)
	:	fReadFilter(iReadFilter)
	,	fChannerR_Bin(iChannerR_Bin)
		{}

// From YadMapR_Std
	virtual void Imp_ReadInc(bool iIsFirst, Name& oName, ZRef<YadR>& oYadR)
		{
		oName = spNameFromChan(*fChannerR_Bin);
		oYadR = spMakeYadR(fReadFilter, fChannerR_Bin);
		}

private:
	const ZRef<ReadFilter> fReadFilter;
	const ZRef<ChannerR_Bin> fChannerR_Bin;
	};

typedef Channer_T<ChanR_NameRefYad_JSONB> YadMapR_JSONB;

// =================================================================================================
#pragma mark -
#pragma mark Yad

ZRef<YadR> spMakeYadR(ZRef<ReadFilter> iReadFilter, const ZRef<ChannerR_Bin>& iChannerR_Bin)
	{
	const ChanR_Bin& r = *iChannerR_Bin;

	if (ZQ<uint8> theTypeQ = sQReadBE<uint8>(r))
		{
		switch (*theTypeQ)
			{
			case 0xE0: return sYadAtomR_Any(Any());
			case 0xE2: return sYadAtomR_Any(Any(false));
			case 0xE3: return sYadAtomR_Any(Any(true));
			case 0xE4: return sYadAtomR_Any(Any(sEReadBE<int64>(r)));
			case 0xE5: return sYadAtomR_Any(Any(sEReadBE<double>(r)));
			case 0xE7: return new YadStreamerR_JSONB(iChannerR_Bin);
			case 0xE8: return spYadRFromString(spStringFromChan(r));
			case 0xEA: return new YadSeqR_JSONB(iReadFilter, iChannerR_Bin);
			case 0xED: return new YadMapR_JSONB(iReadFilter, iChannerR_Bin);
			case 254:
				{
				if (iReadFilter)
					{
					if (ZQ<Any> theQ = iReadFilter->QRead(iChannerR_Bin))
						return sYadR(*theQ);
					ZLOGTRACE(eDebug);
					}
				break;
				}
			case 0xFF:
				{
				// End of list marker.
				return null;
				}
			}
		throw std::runtime_error(sStringf("JSONB invalid type %d", int(*theTypeQ)));
		}

	return null;
	}

// =================================================================================================
#pragma mark -
#pragma mark Visitor_Writer

class Visitor_Writer
	{
public:
	Visitor_Writer(ZRef<WriteFilter> iWriteFilter, const ChanW_Bin& iChanW)
	:	fWriteFilter(iWriteFilter)
	,	fW(iChanW)
		{}

	void Visit(const ZRef<YadR>& iYadR)
		{
		if (false)
			{}

		else if (ZRef<YadAtomR> theYadAtomR = iYadR.DynamicCast<YadAtomR>())
			{ this->Visit_YadAtomR(theYadAtomR); }

		else if (ZRef<ChannerR_UTF> theChanner = iYadR.DynamicCast<ChannerR_UTF>())
			{ this->Visit_YadStrimmerR(theChanner); }

		else if (ZRef<ChannerR_Bin> theChanner = iYadR.DynamicCast<ChannerR_Bin>())
			{ this->Visit_YadStreamerR(theChanner); }

		else if (ZRef<YadSeqR> theYadSeqR = iYadR.DynamicCast<YadSeqR>())
			{ this->Visit_YadSeqR(theYadSeqR); }

		else if (ZRef<YadMapR> theYadMapR = iYadR.DynamicCast<YadMapR>())
			{ this->Visit_YadMapR(theYadMapR); }

		else
			{ ZUnimplemented(); }
		}

	void Visit_YadAtomR(const ZRef<YadAtomR>& iYadAtomR)
		{
		const Any theVal = iYadAtomR->AsAny();

		if (false)
			{}
		else if (theVal.IsNull())
			{
			sEWriteBE<uint8>(fW, 0xE0);
			}
		else if (const bool* p = theVal.PGet<bool>())
			{
			if (*p)
				sEWriteBE<uint8>(fW, 0xE3);
			else
				sEWriteBE<uint8>(fW, 0xE2);
			}
		else if (ZQ<int64> theQ = sQCoerceInt(theVal))
			{
			sEWriteBE<uint8>(fW, 0xE4);
			sEWriteBE<int64>(fW, *theQ);
			}
		else if (ZQ<double> theQ = sQCoerceRat(theVal))
			{
			sEWriteBE<uint8>(fW, 0xE5);
			sEWriteBE<double>(fW, *theQ);
			}
		else
			{
			sEWriteBE<uint8>(fW, 254);
			if (not fWriteFilter || not fWriteFilter->QWrite(theVal, fW))
				{
				if (ZLOGF(w, eErr))
					w << "Couldn't write " << theVal.Type().name();
				ZUnimplemented();
				}
			}
		}

	void Visit_YadStreamerR(const ZRef<YadStreamerR>& iYadStreamerR)
		{
		const ChanR_Bin& r = *iYadStreamerR;
		sEWriteBE<uint8>(fW, 0xE7);
		const size_t chunkSize = 64 * 1024;
		vector<uint8> buffer(chunkSize);
		for (;;)
			{
			const size_t countRead = sReadFully(r, &buffer[0], chunkSize);
			sEWriteCount(fW, countRead);
			if (!countRead)
				break;
			sEWrite(fW, &buffer[0], countRead);
			}
		}

	void Visit_YadStrimmerR(const ZRef<YadStrimmerR>& iYadStrimmerR)
		{
		const string8 theString8 = sReadAllUTF8(*iYadStrimmerR);
		sEWriteBE<uint8>(fW, 0xE8);
		sEWriteCount(fW, theString8.size());
		sEWrite(fW, theString8);
		}

	void Visit_YadSeqR(const ZRef<YadSeqR>& iYadSeqR)
		{
		sEWriteBE<uint8>(fW, 0xEA);
		while (ZRef<YadR> theChild = sReadInc(iYadSeqR))
			this->Visit(theChild);
		sEWriteBE<uint8>(fW, 0xFF); // Terminator
		}

	void Visit_YadMapR(const ZRef<YadMapR>& iYadMapR)
		{
		sEWriteBE<uint8>(fW, 0xED);
		Name theName;
		while (ZRef<YadR> theChild = sReadInc(iYadMapR, theName))
			{
			spToChan(fW, theName);
			this->Visit(theChild);
			}
		sEWriteBE<uint8>(fW, 0); // Empty name
		sEWriteBE<uint8>(fW, 0xFF); // Terminator
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
	{ Visitor_Writer(iWriteFilter, w).Visit(iYadR); }

void sToChan(ZRef<YadR> iYadR, const ChanW_Bin& w)
	{ Visitor_Writer(null, w).Visit(iYadR); }

} // namespace Yad_JSONB
} // namespace ZooLib
