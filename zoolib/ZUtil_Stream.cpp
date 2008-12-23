/* -------------------------------------------------------------------------------------------------
Copyright (c) 2000 Andrew Green and Learning in Motion, Inc.
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

#include "zoolib/ZUtil_Stream.h"

#include "zoolib/ZCompat_algorithm.h"
#include "zoolib/ZDebug.h"
#include "zoolib/ZMemory.h"

#include <ctype.h> // for tolower etc
#include <vector>

NAMESPACE_ZOOLIB_USING

using std::max;
using std::string;
using std::swap;
using std::vector;

// =================================================================================================
#pragma mark -
#pragma mark * ZUtil_Stream::Search_BMH

ZUtil_Stream::Search_BMH::Search_BMH(const string& inPattern, bool inCaseSensitive)
:	fCaseSensitive(inCaseSensitive)
	{
	size_t patternSize = inPattern.size();
	// Initialize the skip vector entries to the pattern length
	for (size_t x = 0; x < 256; ++x)
		fDistance[x] = patternSize;

	// For each character in the search pattern, initialize the appropriate skip to the distance
	// from the last occurence of that character to the end of the pattern.
	if (fCaseSensitive)
		{
		fPattern = inPattern;
		for (size_t x = 0; x < patternSize; ++x)
			fDistance[inPattern.at(x)] = patternSize - x - 1;
		}
	else
		{
		fPattern.resize(patternSize);
		for (size_t x = 0; x < patternSize; ++x)
			{
			char currChar = tolower(inPattern.at(x));
			fPattern[x] = currChar;
			fDistance[currChar] = patternSize - x - 1;
			}
		}
	}

ZUtil_Stream::Search_BMH::~Search_BMH()
	{}

void ZUtil_Stream::Search_BMH::FindNext(const ZStreamR& inStream, size_t* outOffset, bool* outFound)
	{
	// outOffset will hold the number of bytes we read from inStream on *this* invocation. The
	// caller is thus responsible for accumulating the value of outOffset returned each time to
	// determine the overall offset of subsequent searches on the same stream. If outOffset is
	// zero then we didn't read anything from the stream which could be because we had a zero
	// length pattern (in which case outFound will be true), or else the stream was empty. We
	// explicitly set these return values, although the algorithm would generate them naturally,
	// because the accesses to the zeroth items of buf1 and buf2 will throw a range error if
	// patternSize is zero. If outOffset is non-zero, then outFound tells us whether we found
	// the pattern by reading outOffset characters, or if we ran off the end of the stream
	// without finding anything.

	const size_t patternSize = fPattern.size();
	if (patternSize)
		{
		if (outOffset)
			*outOffset = 0;
		if (outFound)
			*outFound = false;
		}
	else
		{
		if (outOffset)
			*outOffset = 0;
		if (outFound)
			*outFound = true;
		return;
		}

	// We use two buffers, buf1 and buf2. currBuf references the buffer we're currently examining.
	// When we need to shift the string we're examining we copy whichever portion of it is still
	// valid into the beginning of otherBuf, fill up the tail of otherBuf and swap the references.
	// We of course could use a single buffer and simply ZBlockMove its tail towards its head, but
	// ZBlockCopy does not have to check for overlapping buffers and hence is a tad faster.
	// The use of two buffers is thus *not* an essential feature of this algorithm's operation.
	vector<uint8> buf1(patternSize);
	vector<uint8> buf2(patternSize);
	uint8* currBuf = &buf1[0];
	uint8* otherBuf = &buf2[0];

	size_t countRead;
	inStream.Read(currBuf, patternSize, &countRead);
	if (outOffset)
		*outOffset += countRead;
	if (countRead < patternSize)
		return;

	while (true)
		{
		int x;
		uint8 curChar;
		if (fCaseSensitive)
			{
			for (x = patternSize - 1; x >= 0; --x)
				{
				curChar = currBuf[x];
				if (curChar != uint8(fPattern.at(x)))
					break;
				}
			}
		else
			{
			for (x = patternSize - 1; x >= 0; --x)
				{
				curChar = tolower(currBuf[x]);
				if (curChar != uint8(fPattern.at(x)))
					break;
				}
			}

		if (x < 0)
			{
			if (outFound)
				*outFound = true;
			break;
			}

		const size_t distance = max(fDistance[curChar], patternSize - x);

		ZBlockCopy(currBuf + distance, otherBuf, patternSize - distance);
		swap(currBuf, otherBuf);

		size_t countRead;
		inStream.Read(currBuf + patternSize - distance, distance, &countRead);
		if (outOffset)
			*outOffset += countRead;
		if (countRead != distance)
			break;
		}
	}
