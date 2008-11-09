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

#include "zoolib/ZYAD_ZooLibStream.h"
#include "zoolib/ZStream_Limited.h"
#include "zoolib/ZString.h"

using std::min;
using std::string;
using std::vector;

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
#pragma mark * ZMapReaderRep_ZooLibStream

class ZMapReaderRep_ZooLibStream : public ZMapReaderRep
	{
public:
	ZMapReaderRep_ZooLibStream(const ZStreamR& iStreamR);

	virtual bool HasValue();
	virtual std::string Name();
	virtual ZRef<ZYADReaderRep> Read();

private:
	void pMoveIfNecessary();

	const ZStreamR& fStreamR;
	bool fNeedsRead;
	string fName;
	uint8 fType;
	};

void ZMapReaderRep_ZooLibStream::pMoveIfNecessary()
	{
	if (!fNeedsRead)
		return;
	fNeedsRead = false;

	fName = sStringFromStream(fStreamR);
	fType = (ZType)fStreamR.ReadUInt8();
	}

ZMapReaderRep_ZooLibStream::ZMapReaderRep_ZooLibStream(const ZStreamR& iStreamR)
:	fStreamR(iStreamR),
	fNeedsRead(true)
	{}

bool ZMapReaderRep_ZooLibStream::HasValue()
	{
	this->pMoveIfNecessary();
	return fType != 0xFF;
	}

std::string ZMapReaderRep_ZooLibStream::Name()
	{
	this->pMoveIfNecessary();
	return fName;
	}

ZRef<ZYADReaderRep> ZMapReaderRep_ZooLibStream::Read()
	{
	this->pMoveIfNecessary();
	if (fType != 0xFF)
		{
		fNeedsRead = true;
		return new ZYADReaderRep_ZooLibStream((ZType)fType, fStreamR);
		}

	return ZRef<ZYADReaderRep>();
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZMapReaderRep_ZooLibStreamOld

class ZMapReaderRep_ZooLibStreamOld : public ZMapReaderRep
	{
public:
	ZMapReaderRep_ZooLibStreamOld(const ZStreamR& iStreamR);

	virtual bool HasValue();
	virtual std::string Name();
	virtual ZRef<ZYADReaderRep> Read();

private:
	void pMoveIfNecessary();

	const ZStreamR& fStreamR;
	bool fNeedsRead;
	string fName;
	size_t fCountRemaining;
	};

void ZMapReaderRep_ZooLibStreamOld::pMoveIfNecessary()
	{
	if (!fNeedsRead)
		return;
	fNeedsRead = false;

	if (!fCountRemaining)
		return;

	fName = sStringFromStream(fStreamR);
	}

ZMapReaderRep_ZooLibStreamOld::ZMapReaderRep_ZooLibStreamOld(const ZStreamR& iStreamR)
:	fStreamR(iStreamR),
	fNeedsRead(true)
	{
	fCountRemaining = iStreamR.ReadCount();
	}

bool ZMapReaderRep_ZooLibStreamOld::HasValue()
	{
	this->pMoveIfNecessary();
	return fCountRemaining;
	}

std::string ZMapReaderRep_ZooLibStreamOld::Name()
	{
	this->pMoveIfNecessary();
	return fName;
	}

ZRef<ZYADReaderRep> ZMapReaderRep_ZooLibStreamOld::Read()
	{
	this->pMoveIfNecessary();
	if (fCountRemaining)
		{
		--fCountRemaining;
		fNeedsRead = true;
		return new ZYADReaderRep_ZooLibStream(fStreamR);
		}

	return ZRef<ZYADReaderRep>();
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZListReaderRep_ZooLibStream

class ZListReaderRep_ZooLibStream : public ZListReaderRep
	{
public:
	ZListReaderRep_ZooLibStream(const ZStreamR& iStreamR);

	virtual bool HasValue();
	virtual size_t Index();
	virtual ZRef<ZYADReaderRep> Read();

private:
	void pMoveIfNecessary();

	const ZStreamR& fStreamR;
	size_t fIndex;
	bool fNeedsRead;
	uint8 fType;
	};

void ZListReaderRep_ZooLibStream::pMoveIfNecessary()
	{
	if (!fNeedsRead)
		return;
	fNeedsRead = false;

	fType = (ZType)fStreamR.ReadUInt8();
	}

ZListReaderRep_ZooLibStream::ZListReaderRep_ZooLibStream(const ZStreamR& iStreamR)
:	fStreamR(iStreamR),
	fIndex(0),
	fNeedsRead(true)
	{}
	
bool ZListReaderRep_ZooLibStream::HasValue()
	{
	this->pMoveIfNecessary();
	return fType != 0xFF;
	}

size_t ZListReaderRep_ZooLibStream::Index()
	{
	return fIndex;
	}

ZRef<ZYADReaderRep> ZListReaderRep_ZooLibStream::Read()
	{
	this->pMoveIfNecessary();
	if (fType != 0xFF)
		{
		fNeedsRead = true;
		++fIndex;
		return new ZYADReaderRep_ZooLibStream((ZType)fType, fStreamR);
		}
	return ZRef<ZYADReaderRep>();	
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZListReaderRep_ZooLibStreamOld

class ZListReaderRep_ZooLibStreamOld : public ZListReaderRep
	{
public:
	ZListReaderRep_ZooLibStreamOld(const ZStreamR& iStreamR);

	virtual bool HasValue();
	virtual size_t Index();
	virtual ZRef<ZYADReaderRep> Read();

private:
	const ZStreamR& fStreamR;
	size_t fIndex;
	size_t fCountRemaining;
	};

ZListReaderRep_ZooLibStreamOld::ZListReaderRep_ZooLibStreamOld(const ZStreamR& iStreamR)
:	fStreamR(iStreamR)
	{
	fCountRemaining = iStreamR.ReadCount();
	}
	
bool ZListReaderRep_ZooLibStreamOld::HasValue()
	{
	return fCountRemaining;
	}

size_t ZListReaderRep_ZooLibStreamOld::Index()
	{
	return fIndex;
	}

ZRef<ZYADReaderRep> ZListReaderRep_ZooLibStreamOld::Read()
	{
	if (fCountRemaining)
		{
		--fCountRemaining;
		++fIndex;
		return new ZYADReaderRep_ZooLibStream(fStreamR);
		}
	return ZRef<ZYADReaderRep>();	
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamerR_ZooLibStream

class ZStreamerR_ZooLibStream
:	public ZStreamerR,
	private ZStreamR
	{
public:
	ZStreamerR_ZooLibStream(const ZStreamR& iStreamR);
	virtual ~ZStreamerR_ZooLibStream();
	
// From ZStreamerR
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

ZStreamerR_ZooLibStream::ZStreamerR_ZooLibStream(const ZStreamR& iStreamR)
:	fStreamR(iStreamR),
	fChunkSize(0),
	fHitEnd(false)
	{}

ZStreamerR_ZooLibStream::~ZStreamerR_ZooLibStream()
	{}

const ZStreamR& ZStreamerR_ZooLibStream::GetStreamR()
	{ return *this; }

void ZStreamerR_ZooLibStream::Imp_Read(void* iDest, size_t iCount, size_t* oCountRead)
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

size_t ZStreamerR_ZooLibStream::Imp_CountReadable()
	{ return min(fChunkSize, fStreamR.CountReadable()); }

void ZStreamerR_ZooLibStream::Imp_Skip(uint64 iCount, uint64* oCountSkipped)
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
#pragma mark * ZStreamerR_ZooLibStreamOld

class ZStreamerR_ZooLibStreamOld
:	public ZStreamerR
	{
public:
	ZStreamerR_ZooLibStreamOld(const ZStreamR& iStreamR);

// From ZStreamerR
	const ZStreamR& GetStreamR();

private:
	ZStreamR_Limited fStreamR_Limited;
	};

ZStreamerR_ZooLibStreamOld::ZStreamerR_ZooLibStreamOld(const ZStreamR& iStreamR)
:	fStreamR_Limited(iStreamR.ReadCount(), iStreamR)
	{}

const ZStreamR& ZStreamerR_ZooLibStreamOld::GetStreamR()
	{ return fStreamR_Limited; }

// =================================================================================================
#pragma mark -
#pragma mark * ZYADReaderRep_ZooLibStream

void ZYADReaderRep_ZooLibStream::pReadIfNecessary()
	{
	if (!fNeedsRead)
		return;

	fNeedsRead = false;

	size_t countRead;
	
	uint8 theType;
	fStreamR.Read(&theType, 1, &countRead);
	fHasValue = countRead != 0;
	fType = (ZType)theType;
	}

ZYADReaderRep_ZooLibStream::ZYADReaderRep_ZooLibStream(ZType iType, const ZStreamR& iStreamR)
:	fStreamR(iStreamR),
	fType(iType),
	fNeedsRead(false),
	fHasValue(true)
	{}

ZYADReaderRep_ZooLibStream::ZYADReaderRep_ZooLibStream(const ZStreamR& iStreamR)
:	fStreamR(iStreamR),
	fNeedsRead(true),
	fHasValue(false)
	{}

bool ZYADReaderRep_ZooLibStream::HasValue()
	{
	this->pReadIfNecessary();
	
	return fHasValue;
	}

ZType ZYADReaderRep_ZooLibStream::Type()
	{
	this->pReadIfNecessary();

	return ZType(fType & ~0x80);
	}

ZRef<ZMapReaderRep> ZYADReaderRep_ZooLibStream::ReadMap()
	{
	this->pReadIfNecessary();

	if (fHasValue && fType == eZType_Tuple)
		{
		fHasValue = false;
		return new ZMapReaderRep_ZooLibStreamOld(fStreamR);
		}

	if (fHasValue && fType == (0x80 | eZType_Tuple))
		{
		fHasValue = false;
		return new ZMapReaderRep_ZooLibStream(fStreamR);
		}

	return ZRef<ZMapReaderRep>();
	}

ZRef<ZListReaderRep> ZYADReaderRep_ZooLibStream::ReadList()
	{
	this->pReadIfNecessary();

	if (fHasValue && fType == eZType_Vector)
		{
		fHasValue = false;
		return new ZListReaderRep_ZooLibStreamOld(fStreamR);
		}

	if (fHasValue && fType == (0x80 | eZType_Vector))
		{
		fHasValue = false;
		return new ZListReaderRep_ZooLibStream(fStreamR);
		}

	return ZRef<ZListReaderRep>();
	}

ZRef<ZStreamerR> ZYADReaderRep_ZooLibStream::ReadRaw()
	{
	this->pReadIfNecessary();

	if (fHasValue && fType == eZType_Raw)
		{
		fHasValue = false;
		return new ZStreamerR_ZooLibStreamOld(fStreamR);
		}

	if (fHasValue && fType == (0x80 | eZType_Raw))
		{
		fHasValue = false;
		return new ZStreamerR_ZooLibStream(fStreamR);
		}

	return ZRef<ZStreamerR>();
	}

ZRef<ZYAD> ZYADReaderRep_ZooLibStream::ReadYAD()
	{
	this->pReadIfNecessary();

	if (!fHasValue)
		return ZRef<ZYAD>();

	fHasValue = false;

	if (fType & 0x80)
		ZUnimplemented();

	return new ZYAD_ZTValue(fType, fStreamR);
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZYADUtil_ZooLibStream

void ZYADUtil_ZooLibStream::sToStream(const ZStreamW& iStreamW, ZListReader iListReader)
	{
	while (iListReader)
		sToStream(iStreamW, iListReader.Read());
	iStreamW.WriteByte(0xFF); // Terminator (distinct from any value in ZType)
	}

void ZYADUtil_ZooLibStream::sToStream(const ZStreamW& iStreamW, ZMapReader iMapReader)
	{
	while (iMapReader)
		{
		sToStream(iMapReader.Name(), iStreamW);
		sToStream(iStreamW, iMapReader.Read());
		}
	iStreamW.WriteByte(0); // Empty name
	iStreamW.WriteByte(0xFF); // Terminator
	}

void ZYADUtil_ZooLibStream::sToStream(const ZStreamW& iStreamW, const ZStreamR& iStreamR)
	{
	if (const ZStreamRPos* theStreamRPos = dynamic_cast<const ZStreamRPos*>(&iStreamR))
		{
		uint64 countRemaining = theStreamRPos->GetSize();
		while (countRemaining)
			{
			size_t countToWrite = size_t(min(countRemaining, 0xFFFFFFFFLLU));
			iStreamW.WriteCount(countToWrite);
			iStreamR.CopyTo(iStreamW, countToWrite);
			countRemaining -= countToWrite;
			}
		iStreamW.WriteCount(0); // Terminating chunk
		}
	else
		{
		const size_t chunkSize = 64 * 1024;
		vector<uint8> buffer(chunkSize);
		for (;;)
			{
			size_t countRead;
			iStreamR.ReadAll(&buffer[0], chunkSize, &countRead);
			iStreamW.WriteCount(countRead);
			if (!countRead)
				break;
			iStreamW.Write(&buffer[0], countRead);
			}
		}
	}

void ZYADUtil_ZooLibStream::sToStream(const ZStreamW& iStreamW, ZYADReader iYADReader)
	{
	if (!iYADReader)
		return;

	if (iYADReader.IsMap())
		{
		iStreamW.WriteUInt8(0x80 | eZType_Tuple);
		sToStream(iStreamW, iYADReader.ReadMap());
		}
	else if (iYADReader.IsList())
		{
		iStreamW.WriteUInt8(0x80 | eZType_Vector);
		sToStream(iStreamW, iYADReader.ReadList());
		}
	else if (iYADReader.IsRaw())
		{
		iStreamW.WriteUInt8(0x80 | eZType_Raw);
		sToStream(iStreamW, iYADReader.ReadRaw()->GetStreamR());
		}
	else
		{
		ZRef<ZYAD> theYAD = iYADReader.ReadYAD();
		const ZTValue& theTV = theYAD->GetTValue();
		theTV.ToStream(iStreamW);
		}
	}

