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

#include "zoolib/ZStream_Data_T.h"
#include "zoolib/ZStream_Memory.h"
#include "zoolib/ZStrim_Stream.h"

#include "zoolib/fileformat/ZFileFormat_QuickTime_Val.h"

namespace ZooLib {
namespace FileFormat {
namespace QuickTime {

using std::string;

// =================================================================================================
// MARK: - Helpers

namespace { // anonymous

ZData_Any spRead(const ZStreamR& iStreamR)
	{ return sReadAll_T<ZData_Any>(iStreamR); }

} // anonymous namespace

// =================================================================================================
// MARK: - sAsVal

// See http://developer.apple.com/library/mac/#documentation/QuickTime/QTFF/Metadata/Metadata.html

ZQ<ZVal_Any> sQAsVal(const ZStreamRPos& iStreamRPos)
	{
	try
		{
		const uint32 theType = iStreamRPos.ReadUInt32();
		/*const uint32 theLocale =*/ iStreamRPos.ReadUInt32();
		switch (theType)
			{
			case 0:
				{
				return spRead(iStreamRPos);
				}
			case 1: // UTF8
			case 4: // UTF8, for sorting
				{
				return ZStrimR_StreamUTF8(iStreamRPos).ReadAll8();
				}
			case 2: // UTF16BE
			case 5: // UTF16BE, for sorting
				{
				return ZStrimR_StreamUTF8(iStreamRPos).ReadAll16();
				}
			case 3: // S/JIS
				break;
			case 13:
				{
				return Data_JPEG(spRead(iStreamRPos));
				}
			case 14:
				{
				return Data_PNG(spRead(iStreamRPos));
				}
			case 21:
			case 22:
				{
				// QuickTime fileformat says we'll get from one to four bytes.
				// I've seen 8 bytes, so we'll use int64 as the return type.
				size_t size = iStreamRPos.CountReadable();
				uint64 result = 0;
				while (size--)
					{
					result <<= 8;
					result |= iStreamRPos.ReadUInt8();
					}
				if (theType == 21)
					return int64(result);
				return result;
				}
			case 23:
				{
				return iStreamRPos.ReadFloat();
				}
			case 24:
				{
				return iStreamRPos.ReadDouble();
				}
			case 27:
				{
				return Data_BMP(spRead(iStreamRPos));
				}
			case 28:
				{
				return Data_Meta(spRead(iStreamRPos));
				}
			}
		}
	catch (...)
		{}
	return null;
	}

ZQ<ZVal_Any> sQAsVal(const void* iSource, size_t iSize)
	{ return sQAsVal(ZStreamRPos_Memory(iSource, iSize)); }

ZQ<ZVal_Any> sQAsVal(const ZData_Any& iData)
	{ return sQAsVal(iData.GetPtr(), iData.GetSize()); }

} // namespace QuickTime
} // namespace FileFormat
} // namespace ZooLib
