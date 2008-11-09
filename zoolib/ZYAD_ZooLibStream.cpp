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
#include "zoolib/ZString.h"

using std::string;
using std::vector;

static const uint32 kMarker = 0xFFFFFFFFU;

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
	size_t fCountRemaining;
	bool fExpectingZeroTerminator;
	};

void ZMapReaderRep_ZooLibStream::pMoveIfNecessary()
	{
	if (!fNeedsRead)
		return;
	fNeedsRead = false;

	if (!fCountRemaining)
		{
		if (fExpectingZeroTerminator)
			{
			fCountRemaining = fStreamR.ReadCount();
			if (!fCountRemaining)
				fExpectingZeroTerminator = false;
			}
		}

	if (!fCountRemaining)
		return;

	fName = sStringFromStream(fStreamR);
	}

ZMapReaderRep_ZooLibStream::ZMapReaderRep_ZooLibStream(const ZStreamR& iStreamR)
:	fStreamR(iStreamR),
	fNeedsRead(true),
	fExpectingZeroTerminator(false)
	{
	fCountRemaining = iStreamR.ReadCount();
	if (fCountRemaining == kMarker)
		{
		fExpectingZeroTerminator = true;
		fCountRemaining = iStreamR.ReadCount();
		}
	}

bool ZMapReaderRep_ZooLibStream::HasValue()
	{
	this->pMoveIfNecessary();
	return fCountRemaining;
	}

std::string ZMapReaderRep_ZooLibStream::Name()
	{
	this->pMoveIfNecessary();
	return fName;
	}

ZRef<ZYADReaderRep> ZMapReaderRep_ZooLibStream::Read()
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
	size_t fCountRemaining;
	bool fHitEnd;
	};

void ZListReaderRep_ZooLibStream::pMoveIfNecessary()
	{
	if (!fCountRemaining && !fHitEnd)
		{
		fCountRemaining = fStreamR.ReadCount();
		if (!fCountRemaining)
			fHitEnd = true;
		}
	}

ZListReaderRep_ZooLibStream::ZListReaderRep_ZooLibStream(const ZStreamR& iStreamR)
:	fStreamR(iStreamR),
	fIndex(0),
	fHitEnd(true)
	{
	fCountRemaining = iStreamR.ReadCount();
	if (fCountRemaining == kMarker)
		{
		fHitEnd = false;
		fCountRemaining = iStreamR.ReadCount();
		}
	}
	
bool ZListReaderRep_ZooLibStream::HasValue()
	{
	this->pMoveIfNecessary();
	return fCountRemaining;
	}

size_t ZListReaderRep_ZooLibStream::Index()
	{
	return fIndex;
	}

ZRef<ZYADReaderRep> ZListReaderRep_ZooLibStream::Read()
	{
	this->pMoveIfNecessary();
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
#pragma mark * ZStreamerR_OSXPlist

class ZStreamerR_ZooLibStream
:	public ZStreamerR
	{
public:
	ZStreamerR_ZooLibStream(const ZStreamR& iStreamR);
	virtual ~ZStreamerR_ZooLibStream();
	
// From ZStreamerR
	const ZStreamR& GetStreamR();

private:
	const ZStreamR& fStreamR;
	};

#if 0
ZStreamerR_ZooLibStream::ZStreamerR_ZooLibStream(ZML::Reader& iR)
:	fR(iR),
	fStreamR_ASCIIStrim(fR.Text()),
	fStreamR_Base64Decode(fStreamR_ASCIIStrim)
	{
	sBegin(fR, "data");
	}

ZStreamerR_ZooLibStream::~ZStreamerR_ZooLibStream()
	{
	sEnd(fR, "data");
	}

const ZStreamR& ZStreamerR_ZooLibStream::GetStreamR()
	{ return fStreamR_Base64Decode; }
#endif

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

	return fType;
	}

ZRef<ZMapReaderRep> ZYADReaderRep_ZooLibStream::ReadMap()
	{
	this->pReadIfNecessary();

	if (fHasValue && fType == eZType_Tuple)
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
		ZUnimplemented();
//!!		return new ZStreamerR_ZooLibStream(fStreamR);
		}
	return ZRef<ZStreamerR>();
	}

ZRef<ZYAD> ZYADReaderRep_ZooLibStream::ReadYAD()
	{
	this->pReadIfNecessary();

	if (!fHasValue)
		return ZRef<ZYAD>();

	fHasValue = false;
	return new ZYAD_ZTValue(fType, fStreamR);
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZYADUtil_ZooLibStream

void ZYADUtil_ZooLibStream::sToStream(const ZStreamW& iStreamW, ZListReader iListReader)
	{
	if (iListReader.CanRandomAccess())
		{
		size_t theCount = iListReader.Count();
		iStreamW.WriteCount(theCount);
		while (theCount--)
			sToStream(iStreamW, iListReader.Read());
		}
	else
		{
		iStreamW.WriteCount(kMarker);
		while (iListReader)
			{
			iStreamW.WriteByte(1);
			sToStream(iStreamW, iListReader.Read());
			}
		iStreamW.WriteByte(0);
		}
	}

void ZYADUtil_ZooLibStream::sToStream(const ZStreamW& iStreamW, ZMapReader iMapReader)
	{
	if (iMapReader.CanRandomAccess())
		{
		size_t theCount = iMapReader.Count();
		iStreamW.WriteCount(theCount);
		while (theCount--)
			{
			sToStream(iMapReader.Name(), iStreamW);
			sToStream(iStreamW, iMapReader.Read());
			}
		}
	else
		{
		iStreamW.WriteCount(kMarker);
		while (iMapReader)
			{
			iStreamW.WriteByte(1);
			sToStream(iMapReader.Name(), iStreamW);
			sToStream(iStreamW, iMapReader.Read());
			}
		iStreamW.WriteByte(0);
		}
	}

void ZYADUtil_ZooLibStream::sToStream(const ZStreamW& iStreamW, const ZStreamR& iStreamR)
	{
	if (const ZStreamRPos* theStreamRPos = dynamic_cast<const ZStreamRPos*>(&iStreamR))
		{
		uint64 theLength = theStreamRPos->GetSize();
		if (theLength < kMarker)
			{
			iStreamW.WriteCount(theLength);
			theStreamRPos->CopyTo(iStreamW, theLength);
			}
		else
			{
			// Very large source stream. Unlikely in practice.
			#warning NDY
			ZUnimplemented();
			for (;;)
				{
				}
			}
		}
	else
		{
		const size_t chunkSize = 64 * 1024;
		vector<uint8> buffer(chunkSize);

		// Grab the first chunk, we may be able to encode in backwards-compatible style.
		size_t countRead;
		iStreamR.ReadAll(&buffer[0], chunkSize, &countRead);

		if (countRead < chunkSize)
			{
			iStreamW.WriteCount(countRead);
			iStreamW.Write(&buffer[0], countRead);
			}
		else
			{
			iStreamW.WriteUInt32(kMarker);
			iStreamW.WriteCount(countRead);
			iStreamW.Write(&buffer[0], countRead);
			for (;;)
				{
				iStreamR.ReadAll(&buffer[0], chunkSize, &countRead);
				iStreamW.WriteCount(countRead);
				iStreamW.Write(&buffer[0], countRead);
				}
			iStreamW.WriteCount(0);
			}
		}
	}

void ZYADUtil_ZooLibStream::sToStream(const ZStreamW& iStreamW, ZYADReader iYADReader)
	{
	if (!iYADReader)
		return;

	if (iYADReader.IsMap())
		{
		iStreamW.WriteUInt8(eZType_Tuple);
		sToStream(iStreamW, iYADReader.ReadMap());
		}
	else if (iYADReader.IsList())
		{
		iStreamW.WriteUInt8(eZType_Vector);
		sToStream(iStreamW, iYADReader.ReadList());
		}
	else if (iYADReader.IsRaw())
		{
		iStreamW.WriteUInt8(eZType_Raw);
		sToStream(iStreamW, iYADReader.ReadRaw()->GetStreamR());
		}
	else
		{
		ZRef<ZYAD> theYAD = iYADReader.ReadYAD();
		const ZTValue& theTV = theYAD->GetTValue();
		theTV.ToStream(iStreamW);
		}
	}

