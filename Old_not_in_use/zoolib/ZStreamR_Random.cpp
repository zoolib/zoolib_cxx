// Copyright (c) 2004 Andrew Green and Learning in Motion, Inc.
// MIT License. http://www.zoolib.org

#include "zoolib/ZStreamR_Random.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - ZStreamR_Random

/**
\class ZStreamR_Random
\ingroup stream
The sequence of bytes is determined by the seed passed to the stream's constructor. An instantiated
stream's sequence can be reset by calling ZStreamR_Random::SetSeed. It's most useful as the source
to a read filter stream which needs to be tested against non-homogeneous but repeatable data, and
where you don't want to have to manually generate that sequence.

Revised 2013-08-27 to conform to MINSTD.
<http://www.firstpr.com.au/dsp/rand31/>
<https://en.wikipedia.org/wiki/Lehmer_random_number_generator>
*/

ZStreamR_Random::ZStreamR_Random(int32 iSeed)
:	fState(iSeed ? iSeed : 1)
	{}

ZStreamR_Random::~ZStreamR_Random()
	{}

void ZStreamR_Random::Imp_Read(void* oDest, size_t iCount, size_t* oCountRead)
	{
	size_t countRemaining = iCount;
	uint8* localDest = reinterpret_cast<uint8*>(oDest);
	while (countRemaining--)
		{
		uint32 hi = 16807 * (fState >> 16);
		uint32 lo = 16807 * (fState & 0xFFFF);
		lo += (hi & 0x7FFF) << 16;
		lo += hi >> 15;
		if (lo > 2147483647)
			lo -= 2147483647;
		fState = lo;
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
	{ fState = iSeed ? iSeed : 1; }

int32 ZStreamR_Random::GetSeed() const
	{ return fState; }

} // namespace ZooLib
