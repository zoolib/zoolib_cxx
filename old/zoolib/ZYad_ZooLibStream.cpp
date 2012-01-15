/* -------------------------------------------------------------------------------------------------
Copyright (c) 2008 Andrew Green
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
#include "zoolib/ZYad_ZooLib.h"
#include "zoolib/ZYad_ZooLibStream.h"

namespace ZooLib {

using std::min;
using std::string;
using std::vector;

static ZRef<ZYadR> spMakeYadR_ZooLibStream(ZRef<ZStreamerR> iStreamerR);

// =================================================================================================
// MARK: - Static helpers

static void spToStream(const string& iString, const ZStreamW& iStreamW)
	{
	const size_t theLength = iString.length();
	iStreamW.WriteCount(theLength);
	if (theLength)
		iStreamW.Write(iString.data(), theLength);
	}

static string spStringFromStream(const ZStreamR& iStreamR)
	{
	if (const size_t theLength = iStreamR.ReadCount())
		return iStreamR.ReadString(theLength);
	return string();
	}

// =================================================================================================
// MARK: - ZYadStreamR_ZooLibStreamNew

class ZYadStreamR_ZooLibStreamNew
:	public ZYadStreamR,
	private ZStreamR
	{
public:
	ZYadStreamR_ZooLibStreamNew(ZRef<ZStreamerR> iStreamerR);

// From ZYadR
	virtual void Finish();

// From ZStreamerR via ZYadStreamR
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

ZYadStreamR_ZooLibStreamNew::ZYadStreamR_ZooLibStreamNew(ZRef<ZStreamerR> iStreamerR)
:	fStreamerR(iStreamerR),
	fChunkSize(0),
	fHitEnd(false)
	{}

void ZYadStreamR_ZooLibStreamNew::Finish()
	{ this->GetStreamR().SkipAll(); }

const ZStreamR& ZYadStreamR_ZooLibStreamNew::GetStreamR()
	{ return *this; }

void ZYadStreamR_ZooLibStreamNew::Imp_Read(void* oDest, size_t iCount, size_t* oCountRead)
	{
	const ZStreamR& theStreamR = fStreamerR->GetStreamR();

	uint8* localDest = reinterpret_cast<uint8*>(oDest);
	while (iCount && !fHitEnd)
		{
		if (fChunkSize == 0)
			{
			fChunkSize = theStreamR.ReadCount();
			if (fChunkSize == 0)
				fHitEnd = true;
			}
		else
			{
			size_t countRead;
			theStreamR.Read(localDest, min(iCount, fChunkSize), &countRead);
			localDest += countRead;
			iCount -= countRead;
			fChunkSize -= countRead;
			}
		}
	if (oCountRead)
		*oCountRead = localDest - reinterpret_cast<uint8*>(oDest);
	}

size_t ZYadStreamR_ZooLibStreamNew::Imp_CountReadable()
	{
	const ZStreamR& theStreamR = fStreamerR->GetStreamR();

	return min(fChunkSize, theStreamR.CountReadable());
	}

void ZYadStreamR_ZooLibStreamNew::Imp_Skip(uint64 iCount, uint64* oCountSkipped)
	{
	const ZStreamR& theStreamR = fStreamerR->GetStreamR();

	uint64 countRemaining = iCount;
	while (countRemaining && !fHitEnd)
		{
		if (fChunkSize == 0)
			{
			fChunkSize = theStreamR.ReadCount();
			if (fChunkSize == 0)
				fHitEnd = true;
			}
		else
			{
			uint64 countSkipped;
			theStreamR.Skip(min(countRemaining, uint64(fChunkSize)), &countSkipped);
			countRemaining -= countSkipped;
			fChunkSize -= countSkipped;
			}
		}

	if (oCountSkipped)
		*oCountSkipped = iCount - countRemaining;
	}

// =================================================================================================
// MARK: - ZYadStreamR_ZooLibStreamOld

class ZYadStreamR_ZooLibStreamOld
:	public ZYadStreamR
	{
public:
	ZYadStreamR_ZooLibStreamOld(ZRef<ZStreamerR> iStreamerR);

// From ZYadR
	virtual void Finish();

// From ZStreamerR via ZYadStreamR
	const ZStreamR& GetStreamR();

private:
	ZRef<ZStreamerR> fStreamerR;
	ZStreamR_Limited fStreamR_Limited;
	};

ZYadStreamR_ZooLibStreamOld::ZYadStreamR_ZooLibStreamOld(ZRef<ZStreamerR> iStreamerR)
:	fStreamerR(iStreamerR)
,	fStreamR_Limited(iStreamerR->GetStreamR().ReadCount(), iStreamerR->GetStreamR())
	{}

void ZYadStreamR_ZooLibStreamOld::Finish()
	{ fStreamR_Limited.SkipAll(); }

const ZStreamR& ZYadStreamR_ZooLibStreamOld::GetStreamR()
	{ return fStreamR_Limited; }

// =================================================================================================
// MARK: - ZYadStrimR_ZooLibStream

class ZYadStrimR_ZooLibStream
:	public ZYadStrimR
	{
public:
	ZYadStrimR_ZooLibStream(ZRef<ZStreamerR> iStreamerR);

// From ZYadR
	virtual void Finish();

// From ZStrimmerR via ZYadStrimR
	const ZStrimR& GetStrimR();

private:
	ZRef<ZStreamerR> fStreamerR;
	ZStreamR_Limited fStreamR_Limited;
	ZStrimR_StreamUTF8 fStrimR;
	};

ZYadStrimR_ZooLibStream::ZYadStrimR_ZooLibStream(ZRef<ZStreamerR> iStreamerR)
:	fStreamerR(iStreamerR),
	fStreamR_Limited(iStreamerR->GetStreamR().ReadCount(), iStreamerR->GetStreamR()),
	fStrimR(fStreamR_Limited)
	{}

void ZYadStrimR_ZooLibStream::Finish()
	{ fStreamR_Limited.SkipAll(); }

const ZStrimR& ZYadStrimR_ZooLibStream::GetStrimR()
	{ return fStrimR; }

// =================================================================================================
// MARK: - ZYadSeqR_ZooLibStreamNew

class ZYadSeqR_ZooLibStreamNew : public ZYadSeqR_Std
	{
public:
	ZYadSeqR_ZooLibStreamNew(ZRef<ZStreamerR> iStreamerR);

// From ZYadSeqR_Std
	virtual void Imp_ReadInc(bool iIsFirst, ZRef<ZYadR>& oYadR);

private:
	ZRef<ZStreamerR> fStreamerR;
	};

ZYadSeqR_ZooLibStreamNew::ZYadSeqR_ZooLibStreamNew(ZRef<ZStreamerR> iStreamerR)
:	fStreamerR(iStreamerR)
	{}

void ZYadSeqR_ZooLibStreamNew::Imp_ReadInc(bool iIsFirst, ZRef<ZYadR>& oYadR)
	{ oYadR = spMakeYadR_ZooLibStream(fStreamerR); }

// =================================================================================================
// MARK: - ZYadSeqR_ZooLibStreamOld

class ZYadSeqR_ZooLibStreamOld : public ZYadSeqR_Std
	{
public:
	ZYadSeqR_ZooLibStreamOld(ZRef<ZStreamerR> iStreamerR);

// From ZYadSeqR_Std
	virtual void Imp_ReadInc(bool iIsFirst, ZRef<ZYadR>& oYadR);

private:
	ZRef<ZStreamerR> fStreamerR;
	size_t fCountRemaining;
	};

ZYadSeqR_ZooLibStreamOld::ZYadSeqR_ZooLibStreamOld(ZRef<ZStreamerR> iStreamerR)
:	fStreamerR(iStreamerR),
	fCountRemaining(fStreamerR->GetStreamR().ReadCount())
	{}

void ZYadSeqR_ZooLibStreamOld::Imp_ReadInc(bool iIsFirst, ZRef<ZYadR>& oYadR)
	{
	if (fCountRemaining)
		{
		--fCountRemaining;
		oYadR = spMakeYadR_ZooLibStream(fStreamerR);
		}
	}

// =================================================================================================
// MARK: - ZYadMapR_ZooLibStreamNew

class ZYadMapR_ZooLibStreamNew : public ZYadMapR_Std
	{
public:
	ZYadMapR_ZooLibStreamNew(ZRef<ZStreamerR> iStreamerR);

// From ZYadMapR_Std
	virtual void Imp_ReadInc(bool iIsFirst, std::string& oName, ZRef<ZYadR>& oYadR);

private:
	ZRef<ZStreamerR> fStreamerR;
	};

ZYadMapR_ZooLibStreamNew::ZYadMapR_ZooLibStreamNew(ZRef<ZStreamerR> iStreamerR)
:	fStreamerR(iStreamerR)
	{}

void ZYadMapR_ZooLibStreamNew::Imp_ReadInc
	(bool iIsFirst, std::string& oName, ZRef<ZYadR>& oYadR)
	{
	oName = spStringFromStream(fStreamerR->GetStreamR());
	oYadR = spMakeYadR_ZooLibStream(fStreamerR);
	}

// =================================================================================================
// MARK: - ZYadMapR_ZooLibStreamOld

ZYadMapR_ZooLibStreamOld::ZYadMapR_ZooLibStreamOld(ZRef<ZStreamerR> iStreamerR)
:	fStreamerR(iStreamerR),
	fCountRemaining(fStreamerR->GetStreamR().ReadCount())
	{}

void ZYadMapR_ZooLibStreamOld::Imp_ReadInc
	(bool iIsFirst, std::string& oName, ZRef<ZYadR>& oYadR)
	{
	if (fCountRemaining)
		{
		--fCountRemaining;
		oName = spStringFromStream(fStreamerR->GetStreamR());
		oYadR = spMakeYadR_ZooLibStream(fStreamerR);
		}
	}

// =================================================================================================
// MARK: - ZYad_ZooLibStream

void ZYad_ZooLibStream::sToStream(const ZStreamW& iStreamW, ZRef<ZYadR> iYadR)
	{
	if (not iYadR)
		{
		return;
		}
	else if (ZRef<ZYadMapR> theYadMapR = iYadR.DynamicCast<ZYadMapR>())
		{
		iStreamW.WriteUInt8(0x80 | eZType_Tuple);
		string theName;
		while (ZRef<ZYadR> theChild = theYadMapR->ReadInc(theName))
			{
			spToStream(theName, iStreamW);
			sToStream(iStreamW, theChild);
			}
		iStreamW.WriteByte(0); // Empty name
		iStreamW.WriteByte(0xFF); // Terminator
		}
	else if (ZRef<ZYadSeqR> theYadSeqR = iYadR.DynamicCast<ZYadSeqR>())
		{
		if (ZRef<ZYadSeqRPos> theYadSeqRPos = iYadR.DynamicCast<ZYadSeqRPos>())
			{
			iStreamW.WriteUInt8(eZType_Vector);
			size_t theCount = theYadSeqRPos->GetSize() - theYadSeqRPos->GetPosition();
			iStreamW.WriteCount(theCount);
			while (--theCount)
				sToStream(iStreamW, theYadSeqRPos->ReadInc());
			}
		else
			{
			iStreamW.WriteUInt8(0x80 | eZType_Vector);
			while (ZRef<ZYadR> theChild = theYadSeqR->ReadInc())
				sToStream(iStreamW, theChild);
			iStreamW.WriteByte(0xFF); // Terminator (distinct from any value in ZType)
			}
		}
	else if (ZRef<ZYadStreamR> theYadStreamR = iYadR.DynamicCast<ZYadStreamR>())
		{
		if (ZRef<ZStreamerRPos> theStreamerRPos = iYadR.DynamicCast<ZStreamerRPos>())
			{
			const ZStreamRPos& theStreamRPos = theStreamerRPos->GetStreamRPos();
			uint64 theCount = theStreamRPos.GetSize() - theStreamRPos.GetPosition();
			if (theCount <= ZUINT64_C(0xFFFFFFFF))
				{
				iStreamW.WriteUInt8(eZType_Raw);
				iStreamW.WriteCount(theCount);
				iStreamW.CopyAllFrom(theStreamRPos);
				return;
				}
			}
		const ZStreamR& theStreamR = theYadStreamR->GetStreamR();
		iStreamW.WriteUInt8(0x80 | eZType_Raw);
		const size_t chunkSize = 64 * 1024;
		vector<uint8> buffer(chunkSize);
		for (;;)
			{
			size_t countRead;
			theStreamR.ReadAll(&buffer[0], chunkSize, &countRead);
			iStreamW.WriteCount(countRead);
			if (!countRead)
				break;
			iStreamW.Write(&buffer[0], countRead);
			}
		}
	else
		{
		sFromYadR(ZVal_ZooLib(), iYadR).ToStream(iStreamW);
		}
	}

ZRef<ZYadR> ZYad_ZooLibStream::sYadR(ZRef<ZStreamerR> iStreamerR)
	{ return spMakeYadR_ZooLibStream(iStreamerR); }

ZRef<ZYadR> spMakeYadR_ZooLibStream(ZRef<ZStreamerR> iStreamerR)
	{
	uint8 theType;
	size_t countRead;
	iStreamerR->GetStreamR().Read(&theType, 1, &countRead);
	if (countRead && theType != 0xFF)
		{
		switch (theType)
			{
			case eZType_Vector:
				return new ZYadSeqR_ZooLibStreamOld(iStreamerR);
			case 0x80 | eZType_Vector:
				return new ZYadSeqR_ZooLibStreamNew(iStreamerR);
			case eZType_Tuple:
				return new ZYadMapR_ZooLibStreamOld(iStreamerR);
			case 0x80 | eZType_Tuple:
				return new ZYadMapR_ZooLibStreamNew(iStreamerR);
			case eZType_Raw:
				return new ZYadStreamR_ZooLibStreamOld(iStreamerR);
			case 0x80 | eZType_Raw:
				return new ZYadStreamR_ZooLibStreamNew(iStreamerR);
			case eZType_String:
				return new ZYadStrimR_ZooLibStream(iStreamerR);
			default:
				return new ZYadAtomR_ZooLib(ZType(theType), iStreamerR->GetStreamR());
			}
		}

	return null;
	}

} // namespace ZooLib
