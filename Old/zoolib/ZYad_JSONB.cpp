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
#include "zoolib/NameUniquifier.h" // For sName
#include "zoolib/Util_Any.h"

#include "zoolib/ZStrim_Stream.h"
#include "zoolib/ZYad_Any.h"
#include "zoolib/ZYad_JSONB.h"
#include "zoolib/ZYad_Std.h"

#include <map>
#include <set>
#include <vector>

#include "zoolib/ZLog.h"
#include "zoolib/ZMACRO_foreach.h"

namespace ZooLib {
namespace ZYad_JSONB {

using std::min;
using std::set;
using std::string;
using std::vector;

namespace { // anonymous

// =================================================================================================
// MARK: - Uniqued Names

Name spNameFromChan(const ChanR_Bin& r)
	{
	if (size_t theCount = sReadCount(r))
		return sName(sReadString(theCount, r));
	return Name();
	}

ZRef<ZYadR> spMakeYadR(const ZRef<ChannerR_Bin>& iChannerR_Bin);

// =================================================================================================
// MARK: - String to/from stream

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

ZAny spStringFromChan(const ChanR_Bin& r)
	{
	if (size_t theCount = sReadCount(r))
		return ZAny(sReadString(theCount, r));
	return sAny<string>();
	}

// =================================================================================================
// MARK: - YadStreamerR

class YadStreamerR
:	public ZYadStreamerR
,	private ZStreamR
	{
public:
	YadStreamerR(ZRef<ChannerR_Bin> iChannerR_Bin);

// From ZYadR
	virtual void Finish();

// From ZStreamerR via ZYadStreamerR
	const ZStreamR& GetStreamR();

// From ZStreamR
	virtual void Imp_Read(void* oDest, size_t iCount, size_t* oCountRead);
	virtual size_t Imp_CountReadable();
	virtual void Imp_Skip(uint64 iCount, uint64* oCountSkipped);

private:
	ZRef<ChannerR_Bin> fChannerR_Bin;
	size_t fChunkSize;
	bool fHitEnd;
	};

YadStreamerR::YadStreamerR(ZRef<ChannerR_Bin> iChannerR_Bin)
:	fChannerR_Bin(iChannerR_Bin)
,	fChunkSize(0)
,	fHitEnd(false)
	{}

void YadStreamerR::Finish()
	{ this->GetStreamR().SkipAll(); }

const ZStreamR& YadStreamerR::GetStreamR()
	{ return *this; }

void YadStreamerR::Imp_Read(void* oDest, size_t iCount, size_t* oCountRead)
	{
	const ChanR_Bin& r = sGetChan(fChannerR_Bin);

	uint8* localDest = reinterpret_cast<uint8*>(oDest);
	while (iCount && !fHitEnd)
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
	if (oCountRead)
		*oCountRead = localDest - reinterpret_cast<uint8*>(oDest);
	}

size_t YadStreamerR::Imp_CountReadable()
	{ return min(fChunkSize, sReadable(sGetChan(fChannerR_Bin))); }

void YadStreamerR::Imp_Skip(uint64 iCount, uint64* oCountSkipped)
	{
	const ChanR_Bin& r = sGetChan(fChannerR_Bin);

	uint64 countRemaining = iCount;
	while (countRemaining && !fHitEnd)
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

	if (oCountSkipped)
		*oCountSkipped = iCount - countRemaining;
	}

// =================================================================================================
// MARK: - YadSeqR

class YadSeqR : public ZYadSeqR_Std
	{
public:
	YadSeqR(const ZRef<ChannerR_Bin>& iChannerR_Bin)
	:	fChannerR_Bin(iChannerR_Bin)
		{}

// From ZYadSeqR_Std
	virtual void Imp_ReadInc(bool iIsFirst, ZRef<ZYadR>& oYadR)
		{ oYadR = spMakeYadR(fChannerR_Bin); }

private:
	ZRef<ChannerR_Bin> fChannerR_Bin;
	};

// =================================================================================================
// MARK: - YadMapR

class YadMapR : public ZYadMapR_Std
	{
public:
	YadMapR(const ZRef<ChannerR_Bin>& iChannerR_Bin)
	:	fChannerR_Bin(iChannerR_Bin)
		{}

// From ZYadMapR_Std
	virtual void Imp_ReadInc(bool iIsFirst, Name& oName, ZRef<ZYadR>& oYadR)
		{
		oName = spNameFromChan(sGetChan(fChannerR_Bin));
		oYadR = spMakeYadR(fChannerR_Bin);
		}

private:
	ZRef<ChannerR_Bin> fChannerR_Bin;
	};

// =================================================================================================
// MARK: - Yad

ZRef<ZYadR> spMakeYadR(const ZRef<ChannerR_Bin>& iChannerR_Bin)
	{
	const ChanR_Bin& r = sGetChan(iChannerR_Bin);

	if (ZQ<uint8> theTypeQ = sQReadBE<uint8>(r))
		{
		switch (*theTypeQ)
			{
			case 1: return sYadR(ZAny());
			case 2: return sYadR(ZAny(false));
			case 3: return sYadR(ZAny(true));
			case 4: return sYadR(ZAny(sReadBE<int64>(r)));
			case 5: return sYadR(ZAny(sReadBE<double>(r)));
			case 7: return new YadStreamerR(iChannerR_Bin);
			case 8: return new ZYadStrimmerU_String(spStringFromChan(r));
			case 11: return new YadSeqR(iChannerR_Bin);
			case 13: return new YadMapR(iChannerR_Bin);
			case 255: return null;
			}
		ZUnimplemented();
		}

	return null;
	}

// =================================================================================================
// MARK: - Visitor_ToChan

class Visitor_ToChan
:	public ZVisitor_Yad_PreferRPos
	{
public:
	Visitor_ToChan(const ChanW_Bin& iChanW)
	:	fW(iChanW)
		{}

// From ZVisitor_Yad
	virtual void Visit_YadR(const ZRef<ZYadR>& iYadR)
		{
		ZUnimplemented();
		}

	virtual void Visit_YadAtomR(const ZRef<ZYadAtomR>& iYadAtomR)
		{
		const ZAny theVal = iYadAtomR->AsAny();

		if (false)
			{}
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
			sWriteBE<uint8>(1, fW);
			}
		}

	virtual void Visit_YadStreamerR(const ZRef<ZYadStreamerR>& iYadStreamerR)
		{
		const ZStreamR& r = iYadStreamerR->GetStreamR();
		sWriteBE<uint8>(7, fW);
		const size_t chunkSize = 64 * 1024;
		vector<uint8> buffer(chunkSize);
		for (;;)
			{
			size_t countRead;
			r.ReadFully(&buffer[0], chunkSize, &countRead);
			sWriteCountMust(countRead, fW);
			if (!countRead)
				break;
			sWriteMust(&buffer[0], countRead, fW);
			}
		}

	virtual void Visit_YadStrimmerR(const ZRef<ZYadStrimmerR>& iYadStrimmerR)
		{
		const string8 theString8 = iYadStrimmerR->GetStrimR().ReadAll8();
		sWriteBE<uint8>(8, fW);
		sWriteCountMust(theString8.size(), fW);
		sWriteMust(theString8, fW);
		}

	virtual void Visit_YadSeqR(const ZRef<ZYadSeqR>& iYadSeqR)
		{
		sWriteBE<uint8>(11, fW);
		while (ZRef<ZYadR> theChild = iYadSeqR->ReadInc())
			theChild->Accept(*this);
		sWriteBE<uint8>(0xFF, fW);  // Terminator
		}

	virtual void Visit_YadMapR(const ZRef<ZYadMapR>& iYadMapR)
		{
		sWriteBE<uint8>(13, fW);
		Name theName;
		while (ZRef<ZYadR> theChild = iYadMapR->ReadInc(theName))
			{
			spToChan(theName, fW);
			theChild->Accept(*this);
			}
		sWriteBE<uint8>(0, fW); // Empty name
		sWriteBE<uint8>(0xFF, fW);  // Terminator
		}

private:
	const ChanW_Bin& fW;
	};

} // anonymous namespace

ZRef<ZYadR> sYadR(ZRef<ChannerR_Bin> iChannerR_Bin)
	{ return spMakeYadR(iChannerR_Bin); }

void sToChan(ZRef<ZYadR> iYadR, const ChanW_Bin& w)
	{ iYadR->Accept(Visitor_ToChan(w)); }

} // namespace ZYad_JSONB
} // namespace ZooLib
