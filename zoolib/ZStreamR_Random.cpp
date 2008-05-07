/* ------------------------------------------------------------
Copyright (c) 2004 Andrew Green and Learning in Motion, Inc.
http://www.zoolib.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
COPYRIGHT HOLDER(S) BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
------------------------------------------------------------ */

#include "ZStreamR_Random.h"

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamR_Random

/**
\class ZStreamR_Random
\ingroup stream
The sequence of bytes is determined by the seed passed to the stream's constructor. An instantiated
stream's sequence can be reset by calling ZStreamR_Random::SetSeed. It's most useful as the source
to a read filter stream which needs to be tested against non-homogeneous but repeatable data, and
where you don't want to have to manually generate that sequence.
*/

ZStreamR_Random::ZStreamR_Random(int32 iSeed)
:	fState(iSeed ? iSeed : 1)
	{}

ZStreamR_Random::~ZStreamR_Random()
	{}

void ZStreamR_Random::Imp_Read(void* iDest, size_t iCount, size_t* oCountRead)
	{
	size_t countRemaining = iCount;
	uint8* localDest = reinterpret_cast<uint8*>(iDest);
	while (countRemaining--)
		{
		fState = (fState * 16807) % 0x7FFFFFFF;
		*localDest++ = uint8(fState >> 16);
		}
	if (oCountRead)
		*oCountRead = iCount;
	}

/**
Set the stream's pseudo-random seed, thus setting the sequence of bytes that will be generated
by subsequent reads from the stream.
*/
void ZStreamR_Random::SetSeed(int32 iSeed)
	{
	fState = iSeed ? iSeed : 1;
	}
