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
// MARK: - Helpers

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
	{ return iChar >= 32 && iChar <= 126; }

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
// MARK: - YadStreamerR

class YadStreamerR
:	public ZYadStreamerR
	{
public:
	YadStreamerR(ZRef<ZStreamerR> iStreamerR, uint64 iSize)
	:	fStreamerR(iStreamerR)
	,	fStreamR_Limited(iSize, fStreamerR->GetStreamR())
		{}

// From ZYadR
	virtual void Finish()
		{ fStreamR_Limited.SkipAll(); }

// From ZStreamerR via ZYadStreamerR
	const ZStreamR& GetStreamR()
		{ return fStreamR_Limited; }

private:
	ZRef<ZStreamerR> fStreamerR;
	ZStreamR_Limited fStreamR_Limited;
	};

// =================================================================================================
// MARK: - YadStreamerRPos

class YadStreamerRPos
:	public ZYadStreamerR
,	public virtual ZStreamerRPos
	{
public:
	YadStreamerRPos(ZRef<ZStreamerRPos> iStreamerRPos, uint64 iSize)
	:	fStreamerRPos(iStreamerRPos)
	,	fStreamRPos_Limited(fStreamerRPos->GetStreamRPos().GetPosition(),
		iSize, fStreamerRPos->GetStreamRPos())
		{}

// From ZYadR
	virtual void Finish()
		{ fStreamRPos_Limited.SkipAll(); }

// From ZStreamerRPos
	const ZStreamRPos& GetStreamRPos()
		{ return fStreamRPos_Limited; }

private:
	ZRef<ZStreamerRPos> fStreamerRPos;
	ZStreamRPos_Limited fStreamRPos_Limited;
	};

// =================================================================================================
// MARK: - YadMapR

class YadMapR : public ZYadMapR_Std
	{
public:
	YadMapR(ZRef<ZStreamerR> iStreamerR, uint64 iRemaining);
	YadMapR(ZRef<ZStreamerR> iStreamerR, uint64 iRemaining,
		uint64 iFirstSize, uint64 iFirstHeaderSize, const string& iFirstName);

// From ZYadMapR_Std
	virtual void Imp_ReadInc(bool iIsFirst, ZName& oName, ZRef<ZYadR>& oYadR);

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

static bool spLooksLikeAContainer(const ZStreamR& iStreamR, size_t iOuterSize,
	uint32& oFirstSize, size_t& oHeaderSize, string& oFirstName)
	{
	if (iOuterSize >= 8)
		{
		oFirstSize = iStreamR.ReadUInt32();
		const uint32 firstName32 = iStreamR.ReadUInt32();
		if (oFirstSize >= 8 && spIsValidName(firstName32))
			{
			if (firstName32 == ZFOURCC('u','u','i','d'))
				{
				if (iOuterSize < 24)
					return false;
				oFirstName = iStreamR.ReadString(16);
				oHeaderSize = 24;
				}
			else
				{
				oFirstName = spOSTypeAsString(firstName32);
				oHeaderSize = 8;
				}
			return true;
			}
		}
	return false;
	}

void YadMapR::Imp_ReadInc(bool iIsFirst, ZName& oName, ZRef<ZYadR>& oYadR)
	{
	if (!fRemaining)
		return;

	const ZStreamR& r = fStreamerR->GetStreamR();

	uint64 theSize;
	if (iIsFirst && fHasFirst)
		{
		ZAssertLog(0, fRemaining >= fFirstSize);
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
				isValidSize = false;
			}

		ZAssertLog(0, isValidSize);

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
		ZAssertLog(0, fRemaining >= theSize);
		fRemaining -= theSize;
		theSize -= headerSize;
		}

	if (ZRef<ZStreamerRPos> theSRPos = fStreamerR.DynamicCast<ZStreamerRPos>())
		{
		if (ZQ<bool> isContainerQ = spIsContainer(oName))
			{
			if (*isContainerQ)
				oYadR = new YadMapR(fStreamerR, theSize);
			else
				oYadR = new YadStreamerRPos(theSRPos, theSize);
			}
		else
			{
			// Don't know if this is a container or not. Try reading a little of the content.
			uint64 priorPos = theSRPos->GetStreamRPos().GetPosition();
			uint32 firstSize;
			size_t headerSize;
			string firstName;
			if (spLooksLikeAContainer(theSRPos->GetStreamR(), theSize,
				firstSize, headerSize, firstName) && theSize >= firstSize)
				{
				oYadR = new YadMapR(theSRPos, theSize, firstSize, headerSize, firstName);
				}
			else
				{
				theSRPos->GetStreamRPos().SetPosition(priorPos);
				oYadR = new YadStreamerRPos(theSRPos, theSize);
				}
			}
		}
	else
		{
		if (ZQ<bool> isContainerQ = spIsContainer(oName))
			{
			if (*isContainerQ)
				oYadR = new YadMapR(fStreamerR, theSize);
			else
				oYadR = new YadStreamerR(fStreamerR, theSize);
			}
		else
			{
			// Don't know if this is a container or not.
			// Try reading a little of the content, keeping a copy in buffer.
			ZData_Any buffer;
			ZStreamRWPos_Data_T<ZData_Any> bufferStream(buffer, 0);
			ZStreamR_Tee theStreamR_Tee(r, bufferStream);
			uint32 firstSize;
			size_t headerSize;
			string firstName;
			if (spLooksLikeAContainer(theStreamR_Tee, theSize,
				firstSize, headerSize, firstName))
				{
				oYadR = new YadMapR(fStreamerR, theSize, firstSize, headerSize, firstName);
				}
			else
				{
				ZRef<ZStreamerR> prior =
					new ZStreamerR_T<ZStreamRPos_Data_T<ZData_Any> >(buffer);
				ZRef<ZStreamerR> theCat = new ZStreamerR_Cat(prior, fStreamerR);
				oYadR = new YadStreamerR(theCat, theSize);
				}
			}
		}
	}

// =================================================================================================
// MARK: - sYadR

ZRef<ZYadR> sYadR(ZRef<ZStreamerR> iStreamerR)
	{
	if (ZRef<ZStreamerRPos> theSRPos = iStreamerR.DynamicCast<ZStreamerRPos>())
		{
		const uint64 theSize = theSRPos->GetStreamRPos().GetSize();
		const uint64 thePosition = theSRPos->GetStreamRPos().GetPosition();
		return new YadMapR(iStreamerR, theSize - thePosition);
		}

	return new YadMapR(iStreamerR, uint64(-1));
	}

} // namespace QuickTime
} // namespace FileFormat
} // namespace ZooLib
