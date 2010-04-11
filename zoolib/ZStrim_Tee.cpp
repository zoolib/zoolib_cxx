/* -------------------------------------------------------------------------------------------------
Copyright (c) 2004 Andrew Green and Learning in Motion, Inc.
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

#include "zoolib/ZStrim_Tee.h"

#include "zoolib/ZDebug.h"
#include "zoolib/ZMemory.h"

using std::max;
using std::min;

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * ZStrimR_Tee

ZStrimR_Tee::ZStrimR_Tee(const ZStrimR& iSource, const ZStrimW& iSink)
:	fSource(iSource),
	fSink(iSink)
	{}

void ZStrimR_Tee::Imp_ReadUTF32(UTF32* oDest, size_t iCount, size_t* oCount)
	{
	UTF32* localDest = oDest;
	size_t countRemaining = iCount;
	while (countRemaining)
		{
		// We have to read into a local buffer because we're going to pass
		// what we read to fSink, and oDest could reference memory that's
		// not safe to read (the garbage buffer, for example).
		UTF32 buffer[sStackBufferSize];

		size_t countRead;
		fSource.Read(buffer, min(countRemaining, countof(buffer)), &countRead);
		if (countRead == 0)
			break;

		ZMemCopy(localDest, buffer, countRead * sizeof(UTF32));

		size_t countWritten;
		fSink.Write(buffer, countRead, &countWritten);

		// This is another case where we're basically screwed if the
		// write stream goes dead on us, as far as preserving ZStreamR
		// semantics and not reading more data than we can dispose of.
		ZAssertLog(1, countRead == countWritten);

		localDest += countRead;
		countRemaining -= countRead;
		}

	if (oCount)
		*oCount = localDest - oDest;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZStrimmerR_Tee

ZStrimmerR_Tee::ZStrimmerR_Tee(ZRef<ZStrimmerR> iStrimmerSource, ZRef<ZStrimmerW> iStrimmerSink)
:	fStrimmerSource(iStrimmerSource),
	fStrimmerSink(iStrimmerSink),
	fStrim(iStrimmerSource->GetStrimR(), iStrimmerSink->GetStrimW())
	{}

ZStrimmerR_Tee::~ZStrimmerR_Tee()
	{}

const ZStrimR& ZStrimmerR_Tee::GetStrimR()
	{ return fStrim; }

// =================================================================================================
#pragma mark -
#pragma mark * ZStrimW_Tee

ZStrimW_Tee::ZStrimW_Tee(const ZStrimW& iSink1, const ZStrimW& iSink2)
:	fSink1(iSink1),
	fSink2(iSink2)
	{}

void ZStrimW_Tee::Imp_WriteUTF32(const UTF32* iSource, size_t iCountCU, size_t* oCountCU)
	{
	const UTF32* localSource = iSource;
	size_t countRemaining = iCountCU;
	while (countRemaining)
		{
		size_t countWritten1;
		fSink1.Write(localSource, countRemaining, &countWritten1);

		size_t countWritten2;
		fSink2.Write(localSource, countRemaining, &countWritten2);

		// Our strims will each write either countRemaining, or as much data as they are able to
		// fit, or they'll return zero when they've hit their bounds. We treat the maximum of what
		// was written to the two strims as being the amount we actually wrote, so that we'll
		// keep writing data so long as one of the strims is able to absorb it. This does rely on
		// strims not doing short writes unless they've really hit their limit. This might be an
		// unreasonable assumption, I'll have to see.
		size_t realCountWritten = max(countWritten1, countWritten2);
		localSource += realCountWritten;
		countRemaining -= realCountWritten;
		}
	if (oCountCU)
		*oCountCU = localSource - iSource;
	}

void ZStrimW_Tee::Imp_Flush()
	{
	fSink1.Flush();
	fSink2.Flush();
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZStrimmerW_Tee

ZStrimmerW_Tee::ZStrimmerW_Tee(ZRef<ZStrimmerW> iStrimmerSink1, ZRef<ZStrimmerW> iStrimmerSink2)
:	fStrimmerSink1(iStrimmerSink1),
	fStrimmerSink2(iStrimmerSink2),
	fStrim(iStrimmerSink1->GetStrimW(), iStrimmerSink2->GetStrimW())
	{}

ZStrimmerW_Tee::~ZStrimmerW_Tee()
	{}

const ZStrimW& ZStrimmerW_Tee::GetStrimW()
	{ return fStrim; }

NAMESPACE_ZOOLIB_END
