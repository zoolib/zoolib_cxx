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

#include "zoolib/ZStream_Limited.h"
#include "zoolib/ZStrim_Stream.h"
#include "zoolib/ZUtil_Any.h"
#include "zoolib/ZYad_Any.h"
#include "zoolib/ZYad_Bin.h"
#include "zoolib/ZYad_Std.h"

#include <vector>

#include "zoolib/ZLog.h"
#include "zoolib/ZMACRO_foreach.h"

namespace ZooLib {
namespace ZYad_Bin {

using std::min;
using std::string;
using std::vector;

namespace { // anonymous

class Counter
:	public ZCounted
	{
public:
	virtual void Finalize()
		{
#if 0
		if (ZLOGF(w,eNotice))
			{
			foreachi (ii, fMap)
				w << "\n" << ii->second << "\t'" << ii->first << "'";
			}
		ZCounted::Finalize();
#endif
		}

	ZName GetName(const string8& iString)
		{
		auto iter = fMap.find(iString);
		if (iter != fMap.end())
			return iter->second;

		const ZName theName = iString;
		fMap.insert(std::make_pair(iString, theName));
		return theName;
		}

	std::map<string8,ZName> fMap;
	};

ZRef<ZYadR> spMakeYadR(const ZRef<ZStreamerR>& iStreamerR, const ZRef<Counter>& iCounter);

// =================================================================================================
// MARK: - Helpers

void spToStream(const string& iString, const ZStreamW& w)
	{
	const size_t theLength = iString.length();
	w.WriteCount(theLength);
	if (theLength)
		w.Write(iString.data(), theLength);
	}

string spStringFromStream(const ZStreamR& r)
	{
	if (const size_t theLength = r.ReadCount())
		return r.ReadString(theLength);
	return string();
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
			fChunkSize = r.ReadCount();
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
			fChunkSize = r.ReadCount();
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
// MARK: - YadStrimmerR

#if 0
class YadStrimmerR
:	public ZYadStrimmerR
	{
public:
	YadStrimmerR(ZRef<ZStreamerR> iStreamerR)
	:	fStreamerR(iStreamerR)
	,	fStreamR_Limited(iStreamerR->GetStreamR().ReadCount(), iStreamerR->GetStreamR())
	,	fStrimR(fStreamR_Limited)
		{}

// From ZYadR
	virtual void Finish()
		{ fStreamR_Limited.SkipAll(); }

// From ZStrimmerR via ZYadStrimmerR
	const ZStrimR& GetStrimR()
		{ return fStrimR; }

private:
	ZRef<ZStreamerR> fStreamerR;
	ZStreamR_Limited fStreamR_Limited;
	ZStrimR_StreamUTF8 fStrimR;
	};

#endif

// =================================================================================================
// MARK: - YadSeqR

class YadSeqR : public ZYadSeqR_Std
	{
public:
	YadSeqR(const ZRef<ZStreamerR>& iStreamerR, const ZRef<Counter>& iCounter)
	:	fStreamerR(iStreamerR)
	,	fCounter(iCounter)
		{}

// From ZYadSeqR_Std
	virtual void Imp_ReadInc(bool iIsFirst, ZRef<ZYadR>& oYadR)
		{ oYadR = spMakeYadR(fStreamerR, fCounter); }

private:
	ZRef<ZStreamerR> fStreamerR;
	ZRef<Counter> fCounter;
	};

// =================================================================================================
// MARK: - YadMapR

class YadMapR : public ZYadMapR_Std
	{
public:
	YadMapR(const ZRef<ZStreamerR>& iStreamerR, const ZRef<Counter>& iCounter)
	:	fStreamerR(iStreamerR)
	,	fCounter(iCounter)
		{}

// From ZYadMapR_Std
	virtual void Imp_ReadInc(bool iIsFirst, ZName& oName, ZRef<ZYadR>& oYadR)
		{
		oName = fCounter->GetName(spStringFromStream(fStreamerR->GetStreamR()));
		oYadR = spMakeYadR(fStreamerR, fCounter);
		}

private:
	ZRef<ZStreamerR> fStreamerR;
	ZRef<Counter> fCounter;
	};

// =================================================================================================
// MARK: - Yad

ZRef<ZYadR> spMakeYadR(const ZRef<ZStreamerR>& iStreamerR, const ZRef<Counter>& iCounter)
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
			case 8: return ZooLib::sYadR(spStringFromStream(r));
			case 11: return new YadSeqR(iStreamerR, iCounter);
			case 13: return new YadMapR(iStreamerR, iCounter);
			}
		ZUnimplemented();
		}

	return null;
	}

// =================================================================================================
#pragma mark -
#pragma mark * Visitor_ToStream

class Visitor_ToStream
:	public ZVisitor_Yad_PreferRPos
	{
public:
	Visitor_ToStream(const ZStreamW& iStreamW)
	:	fW(iStreamW)
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
				fW.WriteUInt8(3);
			else
				fW.WriteUInt8(2);
			}
		else if (ZQ<int64> theQ = sQCoerceInt(theVal))
			{
			fW.WriteUInt8(4);
			fW.WriteInt64(*theQ);
			}
		else if (ZQ<double> theQ = sQCoerceRat(theVal))
			{
			fW.WriteUInt8(5);
			fW.WriteDouble(*theQ);
			}
		else
			{
			fW.WriteUInt8(1);
			}
		}

	virtual void Visit_YadStreamerR(const ZRef<ZYadStreamerR>& iYadStreamerR)
		{
		const ZStreamR& r = iYadStreamerR->GetStreamR();
		fW.WriteUInt8(7);
		const size_t chunkSize = 64 * 1024;
		vector<uint8> buffer(chunkSize);
		for (;;)
			{
			size_t countRead;
			r.ReadAll(&buffer[0], chunkSize, &countRead);
			fW.WriteCount(countRead);
			if (!countRead)
				break;
			fW.Write(&buffer[0], countRead);
			}
		}

	virtual void Visit_YadStrimmerR(const ZRef<ZYadStrimmerR>& iYadStrimmerR)
		{
		const string8 theString8 = iYadStrimmerR->GetStrimR().ReadAll8();
		fW.WriteUInt8(8);
		fW.WriteCount(theString8.size());
		fW.WriteString(theString8);
		}

	virtual void Visit_YadSeqR(const ZRef<ZYadSeqR>& iYadSeqR)
		{
		fW.WriteUInt8(11);
		while (ZRef<ZYadR> theChild = iYadSeqR->ReadInc())
			theChild->Accept(*this);
		fW.WriteByte(0xFF); // Terminator
		}

	virtual void Visit_YadMapR(const ZRef<ZYadMapR>& iYadMapR)
		{
		fW.WriteUInt8(13);
		ZName theName;
		while (ZRef<ZYadR> theChild = iYadMapR->ReadInc(theName))
			{
			spToStream(theName, fW);
			theChild->Accept(*this);
			}
		fW.WriteByte(0); // Empty name
		fW.WriteByte(0xFF); // Terminator
		}

private:
	const ZStreamW& fW;
	};

} // anonymous namespace

ZRef<ZYadR> sYadR(ZRef<ZStreamerR> iStreamerR)
	{ return spMakeYadR(iStreamerR, new Counter); }

void sToStream(ZRef<ZYadR> iYadR, const ZStreamW& w)
	{ iYadR->Accept(Visitor_ToStream(w)); }

} // namespace ZYad_Bin
} // namespace ZooLib
