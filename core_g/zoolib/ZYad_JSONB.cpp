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

#include "zoolib/ZStrim_Stream.h"
#include "zoolib/ZUtil_Any.h"
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
// MARK: - Uniqued ZNames

ZName spNameFromStream(const ZStreamR& r)
	{
	if (size_t theCount = sReadCount(r))
		return sName(r.ReadString(theCount));
	return ZName();
	}

ZRef<ZYadR> spMakeYadR(const ZRef<ZStreamerR>& iStreamerR);

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

ZAny spStringFromStream(const ZStreamR& r)
	{
	if (const size_t theLength = sReadCount(r))
		{
		ZAny theAny = sAny<string>(theLength, 0);
		string* theP = sPMut<string>(theAny);
		r.Read(&(*theP)[0], theLength);
		return theAny;
		}
	return sAny<string>();
	}

// =================================================================================================
// MARK: - YadStreamerR

class YadStreamerR
:	public ZYadStreamerR
,	private ZStreamR
	{
public:
	YadStreamerR(ZRef<ZStreamerR> iStreamerR);

// From ZYadR
	virtual void Finish();

// From ZStreamerR via ZYadStreamerR
	const ZStreamR& GetStreamR();

// From ZStreamR
	virtual void Imp_Read(void* oDest, size_t iCount, size_t* oCountRead);
	virtual size_t Imp_CountReadable();
	virtual void Imp_Skip(uint64 iCount, uint64* oCountSkipped);

private:
	ZRef<ZStreamerR> fStreamerR;
	size_t fChunkSize;
	bool fHitEnd;
	};

YadStreamerR::YadStreamerR(ZRef<ZStreamerR> iStreamerR)
:	fStreamerR(iStreamerR)
,	fChunkSize(0)
,	fHitEnd(false)
	{}

void YadStreamerR::Finish()
	{ this->GetStreamR().SkipAll(); }

const ZStreamR& YadStreamerR::GetStreamR()
	{ return *this; }

void YadStreamerR::Imp_Read(void* oDest, size_t iCount, size_t* oCountRead)
	{
	const ZStreamR& r = fStreamerR->GetStreamR();

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
			size_t countRead;
			r.Read(localDest, min(iCount, fChunkSize), &countRead);
			localDest += countRead;
			iCount -= countRead;
			fChunkSize -= countRead;
			}
		}
	if (oCountRead)
		*oCountRead = localDest - reinterpret_cast<uint8*>(oDest);
	}

size_t YadStreamerR::Imp_CountReadable()
	{
	const ZStreamR& r = fStreamerR->GetStreamR();

	return min(fChunkSize, r.CountReadable());
	}

void YadStreamerR::Imp_Skip(uint64 iCount, uint64* oCountSkipped)
	{
	const ZStreamR& r = fStreamerR->GetStreamR();

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
			uint64 countSkipped;
			r.Skip(min(countRemaining, uint64(fChunkSize)), &countSkipped);
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
	YadSeqR(const ZRef<ZStreamerR>& iStreamerR)
	:	fStreamerR(iStreamerR)
		{}

// From ZYadSeqR_Std
	virtual void Imp_ReadInc(bool iIsFirst, ZRef<ZYadR>& oYadR)
		{ oYadR = spMakeYadR(fStreamerR); }

private:
	ZRef<ZStreamerR> fStreamerR;
	};

// =================================================================================================
// MARK: - YadMapR

class YadMapR : public ZYadMapR_Std
	{
public:
	YadMapR(const ZRef<ZStreamerR>& iStreamerR)
	:	fStreamerR(iStreamerR)
		{}

// From ZYadMapR_Std
	virtual void Imp_ReadInc(bool iIsFirst, ZName& oName, ZRef<ZYadR>& oYadR)
		{
		oName = spNameFromStream(fStreamerR->GetStreamR());
		oYadR = spMakeYadR(fStreamerR);
		}

private:
	ZRef<ZStreamerR> fStreamerR;
	};

// =================================================================================================
// MARK: - Yad

ZRef<ZYadR> spMakeYadR(const ZRef<ZStreamerR>& iStreamerR)
	{
	const ZStreamR& r = iStreamerR->GetStreamR();

	uint8 theType;
	size_t countRead;
	r.Read(&theType, 1, &countRead);
	if (countRead && theType != 0xFF)
		{
		switch (theType)
			{
			case 1: return sYadR(ZAny());
			case 2: return sYadR(ZAny(false));
			case 3: return sYadR(ZAny(true));
			case 4: return sYadR(ZAny(r.ReadInt64()));
			case 5: return sYadR(ZAny(r.ReadDouble()));
			case 7: return new YadStreamerR(iStreamerR);
			case 8: return new ZYadStrimmerU_String(spStringFromStream(r));
			case 11: return new YadSeqR(iStreamerR);
			case 13: return new YadMapR(iStreamerR);
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
		ZName theName;
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

ZRef<ZYadR> sYadR(ZRef<ZStreamerR> iStreamerR)
	{ return spMakeYadR(iStreamerR); }

void sToChan(ZRef<ZYadR> iYadR, const ChanW_Bin& w)
	{ iYadR->Accept(Visitor_ToChan(w)); }

} // namespace ZYad_JSONB
} // namespace ZooLib
