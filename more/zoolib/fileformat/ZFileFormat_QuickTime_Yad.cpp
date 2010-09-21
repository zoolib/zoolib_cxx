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

static void spReadHeader(const ZStreamR& r,
	size_t& oHeaderSize, uint64& oSize, string& oName, bool& oIsContainer, bool& oIsValid)
	{
	// For now assume that extended size follows 32 bit name. Specs are unclear, and
	// it may be that uuid name follows normal header, with extended size following.
	oIsContainer = true;
	oIsValid = true;

	oSize = r.ReadUInt32();
	const uint32 theName32 = r.ReadUInt32();
	oHeaderSize = 8;

	if (spNotName(char(theName32 >> 24))
		|| spNotName(char(theName32 >> 16))
		|| spNotName(char(theName32 >> 8))
		|| spNotName(char(theName32)))
		{
		oIsContainer = false;
		}
	else
		{
		if (oSize == 0)
			{
			// Extends to end of file. Really only valid at top level.
			oSize = -1;
			}
		else if (oSize == 1)
			{
			// 64 bit size follows header
			oSize = r.ReadUInt64();
			oHeaderSize += 8;
			if (oSize < 16)
				oIsValid = false;
			}
		else if (oSize < 8)
			{
			oIsValid = false;
			}

		if (theName32 == ZFOURCC('u','u','i','d'))
			{
			// 16 byte uuid
			oName = r.ReadString(16);
			oHeaderSize += 16;
			}
		else
			{
			oName = spOSTypeAsString(theName32);
			}
		}
	}

} // anonymous namespace

// =================================================================================================
#pragma mark -
#pragma mark * YadStreamR

class YadStreamR
:	public ZYadStreamR
	{
public:
	YadStreamR(ZRef<ZStreamerR> iPrior, ZRef<ZStreamerR> iReal, uint64 iSize)
	:	fPrior(iPrior)
	,	fReal(iReal)
	,	fStreamR_Cat(fPrior->GetStreamR(), fReal->GetStreamR())
	,	fStreamR_Limited(iSize, fStreamR_Cat)
		{}

// From ZStreamerR via ZYadStreamR
	const ZStreamR& GetStreamR()
		{ return fStreamR_Limited; }

private:
	ZRef<ZStreamerR> fPrior;
	ZRef<ZStreamerR> fReal;
	ZStreamR_Cat fStreamR_Cat;
	ZStreamR_Limited fStreamR_Limited;
	};

class YadMapR : public ZYadMapR_Std
	{
public:
	YadMapR(ZRef<ZStreamerR> iStreamerR, uint64 iRemaining);
	YadMapR(ZRef<ZStreamerR> iStreamerR, uint64 iRemaining, uint64 iFirstSize, const string& iFirstName);

// From ZYadMapR_Std
	virtual void Imp_ReadInc(bool iIsFirst, std::string& oName, ZRef<ZYadR>& oYadR);

private:
	ZRef<ZStreamerR> fStreamerR;
	uint64 fRemaining;
	bool fHasFirst;
	uint64 fFirstSize;
	string fFirstName;
	};

YadMapR::YadMapR(ZRef<ZStreamerR> iStreamerR, uint64 iRemaining)
:	fStreamerR(iStreamerR)
,	fRemaining(iRemaining)
,	fHasFirst(false)
	{}

YadMapR::YadMapR(ZRef<ZStreamerR> iStreamerR,
	uint64 iRemaining, uint64 iFirstSize, const string& iFirstName)
:	fStreamerR(iStreamerR)
,	fRemaining(iRemaining)
,	fHasFirst(true)
,	fFirstSize(iFirstSize)
,	fFirstName(iFirstName)
	{}

// =================================================================================================
#pragma mark -
#pragma mark * YadMapR

void YadMapR::Imp_ReadInc(bool iIsFirst, std::string& oName, ZRef<ZYadR>& oYadR)
	{
	if (!fRemaining)
		return;

	const ZStreamR& r = fStreamerR->GetStreamR();
	uint64 theSize;
	if (iIsFirst && fHasFirst)
		{
		theSize = fFirstSize;
		oName = fFirstName;
		fRemaining -= fFirstSize;
		}
	else
		{
		bool isContainer, isValid;
		size_t headerSize;
		spReadHeader(r, headerSize, theSize, oName, isContainer, isValid);
		fRemaining -= theSize;
		theSize -= headerSize;
		}

	// Read the entry's header to see if it looks like a container.
	ZData_Any buffer;
	size_t firstHeaderSize;
	uint64 firstSize;
	string firstName;
	bool firstIsContainer;
	bool firstIsValid;

	spReadHeader(
		ZStreamR_Tee(r, MakeStreamRWPos_Data_T(buffer)),
		firstHeaderSize, firstSize, firstName, firstIsContainer, firstIsValid);	

	if (firstIsContainer)
		{
		oYadR = new YadMapR(fStreamerR, theSize - firstHeaderSize, firstSize - firstHeaderSize, firstName);
		}
	else
		{
		ZRef<ZStreamerR> prior = new ZStreamerR_T<ZStreamRPos_Data_T<ZData_Any> >(buffer);
		oYadR = new YadStreamR(prior, fStreamerR, theSize);
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
