/* -------------------------------------------------------------------------------------------------
Copyright (c) 2010 Andrew Green
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

#include "zoolib/ZData_Any.h"
#include "zoolib/ZStream_Data_T.h"
#include "zoolib/ZStream_Limited.h"
#include "zoolib/ZStream_Tee.h"
#include "zoolib/ZStreamR_Cat.h"

#include "zoolib/fileformat/ZFileFormat_QuickTime_Yad.h"

namespace ZooLib {
namespace FileFormat {
namespace QuickTime {

using std::string;

// =================================================================================================
#pragma mark -
#pragma mark * Helpers

namespace { // anonymous

static string spOSTypeAsString(uint32 iOSType)
	{
	string theString;
	theString += char(iOSType >> 24);
	theString += char(iOSType >> 16);
	theString += char(iOSType >> 8);
	theString += char(iOSType);
	return theString;
	}

static bool spIsName(uint8 iChar)
	{ return iChar >= 32; }

static bool spNotName(uint8 iChar)
	{ return !spIsName(iChar); }

static bool spIsValidName(uint32 iName)
	{
	return (spIsName(char(iName >> 24))
		&& spIsName(char(iName >> 16))
		&& spIsName(char(iName >> 8))
		&& spIsName(char(iName)));
	}

static ZQ<bool> spIsContainer(const string& iName)
	{
	// Disabled for now.
	if (true) {}
	else if (iName == "mean") return false;
	else if (iName == "hdlr") return false;
	else if (iName == "ftyp") return false;
	else if (iName == "mdat") return false;
	else if (iName == "----") return true;
	else if (iName == "moov") return true;
	else if (iName == "udta") return true;
	else if (iName == "ilst") return true;

	// Don't know.
	return null;
	}

} // anonymous namespace

// =================================================================================================
#pragma mark -
#pragma mark * YadStreamR

class YadStreamR
:	public ZYadStreamR
	{
public:
	YadStreamR(ZRef<ZStreamerR> iStreamerR, uint64 iSize)
	:	fStreamerR(iStreamerR)
	,	fStreamR_Limited(iSize, fStreamerR->GetStreamR())
		{}

// From ZStreamerR via ZYadStreamR
	const ZStreamR& GetStreamR()
		{ return fStreamR_Limited; }

private:
	ZRef<ZStreamerR> fStreamerR;
	ZStreamR_Limited fStreamR_Limited;
	};

// =================================================================================================
#pragma mark -
#pragma mark * YadMapR

class YadMapR : public ZYadMapR_Std
	{
public:
	YadMapR(ZRef<ZStreamerR> iStreamerR, uint64 iRemaining);
	YadMapR(ZRef<ZStreamerR> iStreamerR, uint64 iRemaining,
		uint64 iFirstSize, uint64 iFirstHeaderSize, const string& iFirstName);

// From ZYadMapR_Std
	virtual void Imp_ReadInc(bool iIsFirst, std::string& oName, ZRef<ZYadR>& oYadR);

private:
	ZRef<ZStreamerR> fStreamerR;
	uint64 fRemaining;
	const bool fHasFirst;
	uint64 fFirstSize;
	uint64 fFirstHeaderSize;
	string fFirstName;
	};

YadMapR::YadMapR(ZRef<ZStreamerR> iStreamerR, uint64 iRemaining)
:	fStreamerR(iStreamerR)
,	fRemaining(iRemaining)
,	fHasFirst(false)
	{}

YadMapR::YadMapR(ZRef<ZStreamerR> iStreamerR, uint64 iRemaining,
	uint64 iFirstSize, uint64 iFirstHeaderSize, const string& iFirstName)
:	fStreamerR(iStreamerR)
,	fRemaining(iRemaining)
,	fHasFirst(true)
,	fFirstSize(iFirstSize)
,	fFirstHeaderSize(iFirstHeaderSize)
,	fFirstName(iFirstName)
	{}

void YadMapR::Imp_ReadInc(bool iIsFirst, std::string& oName, ZRef<ZYadR>& oYadR)
	{
	if (!fRemaining)
		return;

	const ZStreamR& r = fStreamerR->GetStreamR();

	uint64 theSize;
	if (iIsFirst && fHasFirst)
		{
		fRemaining -= fFirstSize;
		theSize = fFirstSize - fFirstHeaderSize;
		oName = fFirstName;
		}
	else
		{
		theSize = r.ReadUInt32();
		const uint32 theName32 = r.ReadUInt32();
		size_t headerSize = 8;
		bool isValidSize = true;
		if (theSize == 0)
			{
			// Extends to end of file. Really only valid at top level.
			theSize = -1;
			}
		else if (theSize == 1)
			{
			// 64 bit size follows header
			theSize = r.ReadUInt64();
			headerSize += 8;
			if (theSize < 16)
				isValidSize = false;
			}
		else
			{
			if (theSize < 8)
				isValidSize= false;
			}

		if (theName32 == ZFOURCC('u','u','i','d'))
			{
			// 16 byte uuid
			oName = r.ReadString(16);
			headerSize += 16;
			}
		else
			{
			oName = spOSTypeAsString(theName32);
			}
		fRemaining -= theSize;
		theSize -= headerSize;
		}

	if (ZQ<bool> isContainerQ = spIsContainer(oName))
		{
		if (isContainerQ.Get())
			oYadR = new YadMapR(fStreamerR, theSize);
		else
			oYadR = new YadStreamR(fStreamerR, theSize);
		}
	else
		{
		// Don't know if this is a container or not. Try reading a little of the content,
		// keeping a copy in buffer.
		ZData_Any buffer;
		ZStreamRWPos_Data_T<ZData_Any> bufferStream(buffer, 0);
		ZStreamR_Tee theStreamR_Tee(r, bufferStream);
		const uint32 firstSize = theStreamR_Tee.ReadUInt32();
		const uint32 firstName32 = theStreamR_Tee.ReadUInt32();
		if (firstSize >= 8 && spIsValidName(firstName32))
			{
			size_t headerSize;
			string firstName;
			if (firstName32 == ZFOURCC('u','u','i','d'))
				{
				firstName = r.ReadString(16);
				headerSize = 24;
				}
			else
				{
				firstName = spOSTypeAsString(firstName32);
				headerSize = 8;
				}
			oYadR = new YadMapR(fStreamerR, theSize, firstSize, headerSize, firstName);
			}
		else
			{
			ZRef<ZStreamerR> prior =
				new ZStreamerR_T<ZStreamRPos_Data_T<ZData_Any> >(buffer);
			ZRef<ZStreamerR> theCat = new ZStreamerR_Cat(prior, fStreamerR);
			oYadR = new YadStreamR(theCat, theSize);
			}
		}
	}

// =================================================================================================
#pragma mark -
#pragma mark * sMakeYadR

ZRef<ZYadR> sMakeYadR(ZRef<ZStreamerR> iStreamerR)
	{
	if (ZRef<ZStreamerRPos> theSRPos = iStreamerR.DynamicCast<ZStreamerRPos>())
		return new YadMapR(iStreamerR, theSRPos->GetStreamRPos().CountReadable());

	return new YadMapR(iStreamerR, uint64(-1));
	}

} // namespace QuickTime
} // namespace FileFormat
} // namespace ZooLib
