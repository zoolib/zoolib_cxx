/* -------------------------------------------------------------------------------------------------
Copyright (c) 2006 Andrew Green and Learning in Motion, Inc.
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

#ifndef __ZFileFormat_AppleDouble_h__
#define __ZFileFormat_AppleDouble_h__ 1
#include "zconfig.h"

#include "zoolib/ZStreamer.h"

#include <vector>

namespace ZooLib {
namespace FileFormat {
namespace AppleDouble {

// =================================================================================================
// MARK: - FileFormat::AppleDouble

static const uint32 entryID_DataFork = 1;
static const uint32 entryID_ResourceFork = 2;
static const uint32 entryID_RealName = 3;
static const uint32 entryID_Comment = 4;
static const uint32 entryID_IconBW = 5;
static const uint32 entryID_IconColor = 6;
static const uint32 entryID_FileInfo = 7;
static const uint32 entryID_FinderInfo = 9;
static const uint32 entryID_User = 0x80000000U;

// =================================================================================================
// MARK: - FileFormat::AppleDouble::Writer

class Writer
	{
public:
	Writer();

	void SetIsAppleSingle(bool iIsAppleSingle);

	void Append(uint32 iEntryID, ZRef<ZStreamerRPos> iStreamerRPos);
	void Append(uint32 iEntryID, const ZStreamRPos& iStreamRPos);
	void Append(uint32 iEntryID, const ZStreamR& iStreamR, size_t iSize);
	void Append(uint32 iEntryID, const void* iSource, size_t iSize);

	ZRef<ZStreamerRWPos> Create(uint32 iEntryID);

	void ToStream(const ZStreamW& iStreamW) const;

private:
	struct Entry
		{
		uint32 fID;
		ZRef<ZStreamerRPos> fData;
		uint32 fPadding;
		};

	bool fIsAppleSingle;
	std::vector<Entry> fEntries;
	};

} // namespace AppleDouble
} // namespace FileFormat
} // namespace ZooLib

#endif // __ZFileFormat_AppleDouble_h__
