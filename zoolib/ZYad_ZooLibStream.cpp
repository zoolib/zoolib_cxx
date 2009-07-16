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

NAMESPACE_ZOOLIB_BEGIN

using std::min;
using std::string;
using std::vector;

ZRef<ZYadR_Std> sMakeYadR_ZooLibStream(const ZStreamR& iStreamR);

// =================================================================================================
#pragma mark -
#pragma mark * Static helpers

static void sToStream(const string& iString, const ZStreamW& iStreamW)
	{
	const size_t theLength = iString.length();
	iStreamW.WriteCount(theLength);
	if (theLength)
		iStreamW.Write(iString.data(), theLength);
	}

static string sStringFromStream(const ZStreamR& iStreamR)
	{
	if (const size_t theLength = iStreamR.ReadCount())
		return iStreamR.ReadString(theLength);
	return string();
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZYadPrimR_ZooLibStream

class ZYadPrimR_ZooLibStream
:	public ZYadR_Std,
	public ZYadR_ZooLib
	{
public:
	ZYadPrimR_ZooLibStream(ZType iType, const ZStreamR& iStreamR);

// From ZYadR_Std
	virtual void Finish();
	};

ZYadPrimR_ZooLibStream::ZYadPrimR_ZooLibStream(ZType iType, const ZStreamR& iStreamR)
:	ZYadR_ZooLib(iType, iStreamR)
	{}

void ZYadPrimR_ZooLibStream::Finish()
	{}

// =================================================================================================
#pragma mark -
#pragma mark * ZYadStreamR_ZooLibStreamNew

class ZYadStreamR_ZooLibStreamNew
:	public ZYadR_Std,
	public ZYadStreamR,
	private ZStreamR
	{
public:
	ZYadStreamR_ZooLibStreamNew(const ZStreamR& iStreamR);

// From ZYadR_Std
	virtual void Finish();

// From ZStreamerR via ZYadStreamR
	const ZStreamR& GetStreamR();

// From ZStreamR
	virtual void Imp_Read(void* iDest, size_t iCount, size_t* oCountRead);
	virtual size_t Imp_CountReadable();
	virtual void Imp_Skip(uint64 iCount, uint64* oCountSkipped);

private:
	const ZStreamR& fStreamR;
	size_t fChunkSize;
	bool fHitEnd;
	};

ZYadStreamR_ZooLibStreamNew::ZYadStreamR_ZooLibStreamNew(const ZStreamR& iStreamR)
:	fStreamR(iStreamR),
	fChunkSize(0),
	fHitEnd(false)
	{}

void ZYadStreamR_ZooLibStreamNew::Finish()
	{
	this->GetStreamR().SkipAll();
	}

const ZStreamR& ZYadStreamR_ZooLibStreamNew::GetStreamR()
	{ return *this; }

void ZYadStreamR_ZooLibStreamNew::Imp_Read(void* iDest, size_t iCount, size_t* oCountRead)
	{
	uint8* localDest = reinterpret_cast<uint8*>(iDest);
	while (iCount && !fHitEnd)
		{
		if (fChunkSize == 0)
			{
			fChunkSize = fStreamR.ReadCount();
			if (fChunkSize == 0)
				fHitEnd = true;
			}
		else
			{
			size_t countRead;
			fStreamR.Read(localDest, min(iCount, fChunkSize), &countRead);
			localDest += countRead;
			iCount -= countRead;
			fChunkSize -= countRead;
			}
		}
	if (oCountRead)
		*oCountRead = localDest - reinterpret_cast<uint8*>(iDest);
	}

size_t ZYadStreamR_ZooLibStreamNew::Imp_CountReadable()
	{ return min(fChunkSize, fStreamR.CountReadable()); }

void ZYadStreamR_ZooLibStreamNew::Imp_Skip(uint64 iCount, uint64* oCountSkipped)
	{
	uint64 countRemaining = iCount;
	while (countRemaining && !fHitEnd)
		{
		if (fChunkSize == 0)
			{
			fChunkSize = fStreamR.ReadCount();
			if (fChunkSize == 0)
				fHitEnd = true;
			}
		else
			{
			uint64 countSkipped;
			fStreamR.Skip(min(countRemaining, uint64(fChunkSize)), &countSkipped);
			countRemaining -= countSkipped;
			fChunkSize -= countSkipped;
			}
		}

	if (oCountSkipped)
		*oCountSkipped = iCount - countRemaining;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZYadStreamR_ZooLibStreamOld

class ZYadStreamR_ZooLibStreamOld
:	public ZYadR_Std,
	public ZYadStreamR
	{
public:
	ZYadStreamR_ZooLibStreamOld(const ZStreamR& iStreamR);

// From ZYadR_Std
	virtual void Finish();

// From ZStreamerR via ZYadStreamR
	const ZStreamR& GetStreamR();

private:
	ZStreamR_Limited fStreamR_Limited;
	};

ZYadStreamR_ZooLibStreamOld::ZYadStreamR_ZooLibStreamOld(const ZStreamR& iStreamR)
:	fStreamR_Limited(iStreamR.ReadCount(), iStreamR)
	{}

void ZYadStreamR_ZooLibStreamOld::Finish()
	{ fStreamR_Limited.SkipAll(); }

const ZStreamR& ZYadStreamR_ZooLibStreamOld::GetStreamR()
	{ return fStreamR_Limited; }

// =================================================================================================
#pragma mark -
#pragma mark * ZYadStrimR_ZooLibStream

class ZYadStrimR_ZooLibStream
:	public ZYadR_Std,
	public ZYadStrimR
	{
public:
	ZYadStrimR_ZooLibStream(const ZStreamR& iStreamR);

// From ZYadR_Std
	virtual void Finish();

// From ZStrimmerR via ZYadStrimR
	const ZStrimR& GetStrimR();

private:
	ZStreamR_Limited fStreamR_Limited;
	ZStrimR_StreamUTF8 fStrimR;
	};

ZYadStrimR_ZooLibStream::ZYadStrimR_ZooLibStream(const ZStreamR& iStreamR)
:	fStreamR_Limited(iStreamR.ReadCount(), iStreamR),
	fStrimR(fStreamR_Limited)
	{}

void ZYadStrimR_ZooLibStream::Finish()
	{ fStreamR_Limited.SkipAll(); }

const ZStrimR& ZYadStrimR_ZooLibStream::GetStrimR()
	{ return fStrimR; }

// =================================================================================================
#pragma mark -
#pragma mark * ZYadListR_ZooLibStreamNew

class ZYadListR_ZooLibStreamNew : public ZYadListR_Std
	{
public:
	ZYadListR_ZooLibStreamNew(const ZStreamR& iStreamR);

// From ZYadListR_Std
	virtual void Imp_ReadInc(bool iIsFirst, ZRef<ZYadR_Std>& oYadR);

private:
	const ZStreamR& fStreamR;
	};

ZYadListR_ZooLibStreamNew::ZYadListR_ZooLibStreamNew(const ZStreamR& iStreamR)
:	fStreamR(iStreamR)
	{}

void ZYadListR_ZooLibStreamNew::Imp_ReadInc(bool iIsFirst, ZRef<ZYadR_Std>& oYadR)
	{ oYadR = sMakeYadR_ZooLibStream(fStreamR); }

// =================================================================================================
#pragma mark -
#pragma mark * ZYadListR_ZooLibStreamOld

class ZYadListR_ZooLibStreamOld : public ZYadListR_Std
	{
public:
	ZYadListR_ZooLibStreamOld(const ZStreamR& iStreamR);

// From ZYadListR_Std
	virtual void Imp_ReadInc(bool iIsFirst, ZRef<ZYadR_Std>& oYadR);

private:
	const ZStreamR& fStreamR;
	size_t fCountRemaining;
	};

ZYadListR_ZooLibStreamOld::ZYadListR_ZooLibStreamOld(const ZStreamR& iStreamR)
:	fStreamR(iStreamR),
	fCountRemaining(fStreamR.ReadCount())
	{}

void ZYadListR_ZooLibStreamOld::Imp_ReadInc(bool iIsFirst, ZRef<ZYadR_Std>& oYadR)
	{
	if (fCountRemaining)
		{
		--fCountRemaining;
		oYadR = sMakeYadR_ZooLibStream(fStreamR);
		}
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZYadMapR_ZooLibStreamNew

class ZYadMapR_ZooLibStreamNew : public ZYadMapR_Std
	{
public:
	ZYadMapR_ZooLibStreamNew(const ZStreamR& iStreamR);

// From ZYadMapR_Std
	virtual void Imp_ReadInc(bool iIsFirst, std::string& oName, ZRef<ZYadR_Std>& oYadR);

private:
	const ZStreamR& fStreamR;
	};

ZYadMapR_ZooLibStreamNew::ZYadMapR_ZooLibStreamNew(const ZStreamR& iStreamR)
:	fStreamR(iStreamR)
	{}

void ZYadMapR_ZooLibStreamNew::Imp_ReadInc(
	bool iIsFirst, std::string& oName, ZRef<ZYadR_Std>& oYadR)
	{
	oName = sStringFromStream(fStreamR);
	oYadR = sMakeYadR_ZooLibStream(fStreamR);
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZYadMapR_ZooLibStreamOld

ZYadMapR_ZooLibStreamOld::ZYadMapR_ZooLibStreamOld(const ZStreamR& iStreamR)
:	fStreamR(iStreamR),
	fCountRemaining(fStreamR.ReadCount())
	{}

void ZYadMapR_ZooLibStreamOld::Imp_ReadInc(
	bool iIsFirst, std::string& oName, ZRef<ZYadR_Std>& oYadR)
	{
	if (fCountRemaining)
		{
		--fCountRemaining;
		oName = sStringFromStream(fStreamR);
		oYadR = sMakeYadR_ZooLibStream(fStreamR);
		}
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZYad_ZooLibStream

void ZYad_ZooLibStream::sToStream(const ZStreamW& iStreamW, ZRef<ZYadR> iYadR)
	{
	if (!iYadR)
		{
		return;
		}
	else if (ZRef<ZYadMapR> theYadMapR = ZRefDynamicCast<ZYadMapR>(iYadR))
		{
		iStreamW.WriteUInt8(0x80 | eZType_Tuple);
		string theName;
		while (ZRef<ZYadR> theChild = theYadMapR->ReadInc(theName))
			{
			sToStream(theName, iStreamW);
			sToStream(iStreamW, theChild);
			}
		iStreamW.WriteByte(0); // Empty name
		iStreamW.WriteByte(0xFF); // Terminator
		}
	else if (ZRef<ZYadListR> theYadListR = ZRefDynamicCast<ZYadListR>(iYadR))
		{
		if (ZRef<ZYadListRPos> theYadListRPos = ZRefDynamicCast<ZYadListRPos>(iYadR))
			{
			iStreamW.WriteUInt8(eZType_Vector);
			size_t theCount = theYadListRPos->GetSize() - theYadListRPos->GetPosition();
			iStreamW.WriteCount(theCount);
			while (--theCount)
				sToStream(iStreamW, theYadListRPos->ReadInc());
			}
		else
			{
			iStreamW.WriteUInt8(0x80 | eZType_Vector);
			while (ZRef<ZYadR> theChild = theYadListR->ReadInc())
				sToStream(iStreamW, theChild);
			iStreamW.WriteByte(0xFF); // Terminator (distinct from any value in ZType)
			}
		}
	else if (ZRef<ZYadStreamR> theYadStreamR = ZRefDynamicCast<ZYadStreamR>(iYadR))
		{
		if (ZRef<ZStreamerRPos> theStreamerRPos = ZRefDynamicCast<ZStreamerRPos>(theYadStreamR))
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
		sFromYadR_T<ZVal_ZooLib>(iYadR).ToStream(iStreamW);
		}
	}

ZRef<ZYadR> ZYad_ZooLibStream::sMakeYadR(const ZStreamR& iStreamR)
	{
	return sMakeYadR_ZooLibStream(iStreamR);
	}

ZRef<ZYadR_Std> sMakeYadR_ZooLibStream(const ZStreamR& iStreamR)
	{
	uint8 theType;
	size_t countRead;
	iStreamR.Read(&theType, 1, &countRead);
	if (countRead && theType != 0xFF)
		{
		if (theType == eZType_Vector)
			{
			return new ZYadListR_ZooLibStreamOld(iStreamR);
			}
		else if (theType == (0x80 | eZType_Vector))
			{
			return new ZYadListR_ZooLibStreamNew(iStreamR);
			}
		else if (theType == eZType_Tuple)
			{
			return new ZYadMapR_ZooLibStreamOld(iStreamR);
			}
		else if (theType == (0x80 | eZType_Tuple))
			{
			return new ZYadMapR_ZooLibStreamNew(iStreamR);
			}
		else if (theType == eZType_Raw)
			{
			return new ZYadStreamR_ZooLibStreamOld(iStreamR);			
			}
		else if (theType == (0x80 | eZType_Raw))
			{
			return new ZYadStreamR_ZooLibStreamNew(iStreamR);			
			}
		else if (theType == eZType_String)
			{
			return new ZYadStrimR_ZooLibStream(iStreamR);			
			}
		else
			{
			return new ZYadPrimR_ZooLibStream(ZType(theType), iStreamR);
			}
		}

	return ZRef<ZYadR_Std>();
	}

NAMESPACE_ZOOLIB_END
