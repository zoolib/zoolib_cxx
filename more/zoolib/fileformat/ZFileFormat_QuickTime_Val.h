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

#ifndef __ZFileFormat_QuickTime_Val_h__
#define __ZFileFormat_QuickTime_Val_h__ 1
#include "zconfig.h"

#include "zoolib/TagVal.h"

#include "zoolib/ZVal_Any.h"

namespace ZooLib {
namespace FileFormat {
namespace QuickTime {

// =================================================================================================
// MARK: - sAsVal

typedef TagVal<ZData_Any,struct BMP> Data_BMP;
typedef TagVal<ZData_Any,struct JPEG> Data_JPEG;
typedef TagVal<ZData_Any,struct PNG> Data_PNG;
typedef TagVal<ZData_Any,struct Meta> Data_Meta;

ZQ<ZVal_Any> sQAsVal(const void* iSource, size_t iSize);
ZQ<ZVal_Any> sQAsVal(const ZData_Any& iData);
ZQ<ZVal_Any> sQAsVal(const ZStreamR& iStreamR);

} // namespace QuickTime
} // namespace FileFormat
} // namespace ZooLib

#endif // __ZFileFormat_QuickTime_Val_h__
