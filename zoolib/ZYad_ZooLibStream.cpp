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

#include "zoolib/ZYad_ZooLibStream.h"
#include "zoolib/ZStream_Limited.h"

class ZYadR_ZooLibStream;

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

static ZRef<ZYadR_ZooLibStream> sMakeYadR_ZooLibStream(const ZStreamR& iStreamR);

// =================================================================================================
#pragma mark -
#pragma mark * ZYadR_ZooLibStream

class ZYadR_ZooLibStream : public virtual ZYadR
	{
public:
	// Our protocol
	virtual void Finish() = 0;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYadPrimR_ZooLibStream

class ZYadPrimR_ZooLibStream
:	public ZYadPrimR,
	public ZYadR_ZooLibStream
	{
public:
	ZYadPrimR_ZooLibStream(ZType iType, const ZStreamR& iStreamR);

// From ZYadR
	virtual ZRef<ZYad> ReadYad();

// From ZYadR_ZooLibStream
	virtual void Finish();

private:
	ZRef<ZYad> fYad;
	};

ZYadPrimR_ZooLibStream::ZYadPrimR_ZooLibStream(ZType iType, const ZStreamR& iStreamR)
:	fYad(new ZYad_TValue(iType, iStreamR))
	{}

ZRef<ZYad> ZYadPrimR_ZooLibStream::ReadYad()
	{
	ZRef<ZYad> result = fYad;
	fYad.Clear();
	return result;
	}

void ZYadPrimR_ZooLibStream::Finish()
	{ fYad.Clear(); }

// =================================================================================================
#pragma mark -
#pragma mark * ZYadRawR_ZooLibStream

class ZYadRawR_ZooLibStream
:	public ZYadRawR,
	public ZYadR_ZooLibStream,
	private ZStreamR
	{
public:
	ZYadRawR_ZooLibStream(const ZStreamR& iStreamR);

// From ZYadR_ZooLibStream
	virtual void Finish();

// From ZStreamerR via ZYadRawR
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

ZYadRawR_ZooLibStream::ZYadRawR_ZooLibStream(const ZStreamR& iStreamR)
:	fStreamR(iStreamR),
	fChunkSize(0),
	fHitEnd(false)
	{}

void ZYadRawR_ZooLibStream::Finish()
	{
	this->GetStreamR().SkipAll();
	}

const ZStreamR& ZYadRawR_ZooLibStream::GetStreamR()
	{ return *this; }

void ZYadRawR_ZooLibStream::Imp_Read(void* iDest, size_t iCount, size_t* oCountRead)
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

size_t ZYadRawR_ZooLibStream::Imp_CountReadable()
	{ return min(fChunkSize, fStreamR.CountReadable()); }

void ZYadRawR_ZooLibStream::Imp_Skip(uint64 iCount, uint64* oCountSkipped)
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
#pragma mark * ZYadRawR_ZooLibStreamOld

class ZYadRawR_ZooLibStreamOld
:	public ZYadRawR,
	public ZYadR_ZooLibStream
	{
public:
	ZYadRawR_ZooLibStreamOld(const ZStreamR& iStreamR);

// From ZYadR_ZooLibStream
	virtual void Finish();

// From ZStreamerR via ZYadRawR
	const ZStreamR& GetStreamR();

private:
	ZStreamR_Limited fStreamR_Limited;
	};

ZYadRawR_ZooLibStreamOld::ZYadRawR_ZooLibStreamOld(const ZStreamR& iStreamR)
:	fStreamR_Limited(iStreamR.ReadCount(), iStreamR)
	{}

void ZYadRawR_ZooLibStreamOld::Finish()
	{
	fStreamR_Limited.SkipAll();
	}

const ZStreamR& ZYadRawR_ZooLibStreamOld::GetStreamR()
	{ return fStreamR_Limited; }

// =================================================================================================
#pragma mark -
#pragma mark * ZYadListR_ZooLibStream

class ZYadListR_ZooLibStream
:	public ZYadListR,
	public ZYadR_ZooLibStream
	{
public:
	ZYadListR_ZooLibStream(const ZStreamR& iStreamR);

// From ZYadR via ZYadListR
	virtual bool HasChild();
	virtual ZRef<ZYadR> NextChild();

// From ZYadListR
	virtual size_t GetPosition();

// From ZYadR_ZooLibStream
	virtual void Finish();

private:
	void pMoveIfNecessary();

	const ZStreamR& fStreamR;
	size_t fPosition;
	ZRef<ZYadR_ZooLibStream> fValue_Current;
	ZRef<ZYadR_ZooLibStream> fValue_Prior;
	};

ZYadListR_ZooLibStream::ZYadListR_ZooLibStream(const ZStreamR& iStreamR)
:	fStreamR(iStreamR),
	fPosition(0)
	{}

bool ZYadListR_ZooLibStream::HasChild()
	{
	this->pMoveIfNecessary();
	return fValue_Current;
	}

ZRef<ZYadR> ZYadListR_ZooLibStream::NextChild()
	{
	this->pMoveIfNecessary();

	if (fValue_Current)
		{
		fValue_Prior = fValue_Current;
		fValue_Current.Clear();
		++fPosition;
		}

	return fValue_Prior;
	}

size_t ZYadListR_ZooLibStream::GetPosition()
	{ return fPosition; }

void ZYadListR_ZooLibStream::Finish()
	{
	this->SkipAll();
	}

void ZYadListR_ZooLibStream::pMoveIfNecessary()
	{
	if (fValue_Current)
		return;

	if (fValue_Prior)
		{
		fValue_Prior->Finish();
		fValue_Prior.Clear();
		}

	fValue_Current = sMakeYadR_ZooLibStream(fStreamR);
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZYadListR_ZooLibStreamOld

class ZYadListR_ZooLibStreamOld
:	public ZYadListR,
	public ZYadR_ZooLibStream
	{
public:
	ZYadListR_ZooLibStreamOld(const ZStreamR& iStreamR);

// From ZYadR via ZYadListR
	virtual bool HasChild();
	virtual ZRef<ZYadR> NextChild();

// From ZYadListR
	virtual size_t GetPosition();

// From ZYadR_ZooLibStream
	virtual void Finish();

private:
	void pMoveIfNecessary();

	const ZStreamR& fStreamR;
	size_t fCountRemaining;
	size_t fPosition;
	ZRef<ZYadR_ZooLibStream> fValue_Current;
	ZRef<ZYadR_ZooLibStream> fValue_Prior;
	};

ZYadListR_ZooLibStreamOld::ZYadListR_ZooLibStreamOld(const ZStreamR& iStreamR)
:	fStreamR(iStreamR),
	fCountRemaining(fStreamR.ReadCount()),
	fPosition(0)
	{}

bool ZYadListR_ZooLibStreamOld::HasChild()
	{
	this->pMoveIfNecessary();
	return fValue_Current;
	}

ZRef<ZYadR> ZYadListR_ZooLibStreamOld::NextChild()
	{
	this->pMoveIfNecessary();

	if (fValue_Current)
		{
		fValue_Prior = fValue_Current;
		fValue_Current.Clear();
		++fPosition;
		}

	return fValue_Prior;
	}

size_t ZYadListR_ZooLibStreamOld::GetPosition()
	{ return fPosition; }

void ZYadListR_ZooLibStreamOld::Finish()
	{
	this->SkipAll();
	}

void ZYadListR_ZooLibStreamOld::pMoveIfNecessary()
	{
	if (fValue_Current)
		return;

	if (fValue_Prior)
		{
		fValue_Prior->Finish();
		fValue_Prior.Clear();
		}

	if (fCountRemaining)
		{
		--fCountRemaining;
		fValue_Current = sMakeYadR_ZooLibStream(fStreamR);
		}
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZYadMapR_ZooLibStream

class ZYadMapR_ZooLibStream
:	public ZYadMapR,
	public ZYadR_ZooLibStream
	{
public:
	ZYadMapR_ZooLibStream(const ZStreamR& iStreamR);

// From ZYadR via ZYadMapR
	virtual bool HasChild();
	virtual ZRef<ZYadR> NextChild();

// From ZYadMapR
	virtual std::string Name();

// From ZYadR_ZooLibStream
	virtual void Finish();

private:
	void pMoveIfNecessary();

	const ZStreamR& fStreamR;
	std::string fName;
	size_t fCountRemaining;
	ZRef<ZYadR_ZooLibStream> fValue_Current;
	ZRef<ZYadR_ZooLibStream> fValue_Prior;
	};

ZYadMapR_ZooLibStream::ZYadMapR_ZooLibStream(const ZStreamR& iStreamR)
:	fStreamR(iStreamR),
	fCountRemaining(fStreamR.ReadCount())
	{}
	
bool ZYadMapR_ZooLibStream::HasChild()
	{
	this->pMoveIfNecessary();

	return fValue_Current;
	}

ZRef<ZYadR> ZYadMapR_ZooLibStream::NextChild()
	{
	this->pMoveIfNecessary();

	if (fValue_Current)
		{
		fValue_Prior = fValue_Current;
		fValue_Current.Clear();
		fName.clear();
		}

	return fValue_Prior;
	}

string ZYadMapR_ZooLibStream::Name()
	{
	this->pMoveIfNecessary();

	return fName;
	}

void ZYadMapR_ZooLibStream::Finish()
	{
	this->SkipAll();
	}

void ZYadMapR_ZooLibStream::pMoveIfNecessary()
	{
	if (fValue_Current)
		return;

	if (fValue_Prior)
		{
		fValue_Prior->Finish();
		fValue_Prior.Clear();
		}

	if (fCountRemaining)
		{
		--fCountRemaining;
		fName = sStringFromStream(fStreamR);
		fValue_Current = sMakeYadR_ZooLibStream(fStreamR);
		}
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZYadMapR_ZooLibStreamOld

class ZYadMapR_ZooLibStreamOld
:	public ZYadMapR,
	public ZYadR_ZooLibStream
	{
public:
	ZYadMapR_ZooLibStreamOld(const ZStreamR& iStreamR);

// From ZYadR via ZYadMapR
	virtual bool HasChild();
	virtual ZRef<ZYadR> NextChild();

// From ZYadMapR
	virtual std::string Name();

// From ZYadR_ZooLibStream
	virtual void Finish();

private:
	void pMoveIfNecessary();

	const ZStreamR& fStreamR;
	std::string fName;
	ZRef<ZYadR_ZooLibStream> fValue_Current;
	ZRef<ZYadR_ZooLibStream> fValue_Prior;
	};

ZYadMapR_ZooLibStreamOld::ZYadMapR_ZooLibStreamOld(const ZStreamR& iStreamR)
:	fStreamR(iStreamR)
	{}
	
bool ZYadMapR_ZooLibStreamOld::HasChild()
	{
	this->pMoveIfNecessary();

	return fValue_Current;
	}

ZRef<ZYadR> ZYadMapR_ZooLibStreamOld::NextChild()
	{
	this->pMoveIfNecessary();

	if (fValue_Current)
		{
		fValue_Prior = fValue_Current;
		fValue_Current.Clear();
		fName.clear();
		}

	return fValue_Prior;
	}

string ZYadMapR_ZooLibStreamOld::Name()
	{
	this->pMoveIfNecessary();

	return fName;
	}

void ZYadMapR_ZooLibStreamOld::Finish()
	{
	this->SkipAll();
	}

void ZYadMapR_ZooLibStreamOld::pMoveIfNecessary()
	{
	if (fValue_Current)
		return;

	if (fValue_Prior)
		{
		fValue_Prior->Finish();
		fValue_Prior.Clear();
		}

	fName = sStringFromStream(fStreamR);
	fValue_Current = sMakeYadR_ZooLibStream(fStreamR);
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZYadUtil_ZooLibStream

using namespace ZYadUtil_ZooLibStream;

static void sToStream(const ZStreamW& iStreamW, ZRef<ZYadMapR> iYadMapR)
	{
	while (iYadMapR->HasChild())
		{
		sToStream(iYadMapR->Name(), iStreamW);
		sToStream(iStreamW, iYadMapR->NextChild());
		}
	iStreamW.WriteByte(0); // Empty name
	iStreamW.WriteByte(0xFF); // Terminator
	}

static void sToStream(const ZStreamW& iStreamW, ZRef<ZYadListR> iYadListR)
	{
	while (iYadListR->HasChild())
		sToStream(iStreamW, iYadListR->NextChild());
	iStreamW.WriteByte(0xFF); // Terminator (distinct from any value in ZType)
	}

static void sToStream(const ZStreamW& iStreamW, const ZStreamR& iStreamR)
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

void ZYadUtil_ZooLibStream::sToStream(const ZStreamW& iStreamW, ZRef<ZYadR> iYadR)
	{
	if (!iYadR)
		return;

	if (ZRef<ZYadMapR> theYadMapR = ZRefDynamicCast<ZYadMapR>(iYadR))
		{
		iStreamW.WriteUInt8(0x80 | eZType_Tuple);
		sToStream(iStreamW, theYadMapR);
		}
	else if (ZRef<ZYadListR> theYadListR = ZRefDynamicCast<ZYadListR>(iYadR))
		{
		iStreamW.WriteUInt8(0x80 | eZType_Vector);
		sToStream(iStreamW, theYadListR);
		}
	else if (ZRef<ZYadRawR> theYadRawR = ZRefDynamicCast<ZYadRawR>(iYadR))
		{
		iStreamW.WriteUInt8(eZType_Raw);
		sToStream(iStreamW, theYadRawR->GetStreamR());
		}
	else
		{
		iYadR->ReadYad()->GetTValue().ToStream(iStreamW);
		}
	}

ZRef<ZYadR> ZYadUtil_ZooLibStream::sMakeYadR(const ZStreamR& iStreamR)
	{ return sMakeYadR_ZooLibStream(iStreamR); }

static ZRef<ZYadR_ZooLibStream> sMakeYadR_ZooLibStream(const ZStreamR& iStreamR)
	{
	uint8 theType;
	size_t countRead;
	iStreamR.Read(&theType, 1, &countRead);
	if (countRead)
		{
		if (theType == eZType_Vector)
			{
			return new ZYadListR_ZooLibStreamOld(iStreamR);
			}
		else if (theType == (0x80 | eZType_Vector))
			{
			return new ZYadListR_ZooLibStream(iStreamR);
			}
		else if (theType == eZType_Tuple)
			{
			return new ZYadMapR_ZooLibStreamOld(iStreamR);
			}
		else if (theType == (0x80 | eZType_Tuple))
			{
			return new ZYadMapR_ZooLibStream(iStreamR);
			}
		else if (theType == eZType_Raw)
			{
			return new ZYadRawR_ZooLibStreamOld(iStreamR);			
			}
		else if (theType == (0x80 | eZType_Raw))
			{
			return new ZYadRawR_ZooLibStream(iStreamR);			
			}
		else
			{
			return new ZYadPrimR_ZooLibStream(ZType(theType), iStreamR);
			}
		}

	return ZRef<ZYadR_ZooLibStream>();
	}
