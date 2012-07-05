/* -------------------------------------------------------------------------------------------------
Copyright (c) 2012 Andrew Green
http://www.zoolib.org

Permission is hereby granted, free of charge, to any person obtaining a copy of this software
and associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge,Publish, distribute,
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

#ifndef __ZYad_Zip_h__
#define __ZYad_Zip_h__ 1
#include "zconfig.h"

#include "zoolib/ZYad.h"

// Avoid pulling zip.h into everything using ZYad_Zip.
extern "C" { struct zip; }

namespace ZooLib {
namespace ZYad_Zip {

// =================================================================================================
// MARK: -

ZRef<ZYadMapAtRPos> sYadR(const char* iFilePath);
ZRef<ZYadMapAtRPos> sYadR(zip* i_zip, bool iAdopt);

} // namespace ZYad_Zip
} // namespace ZooLib

#endif // __ZYad_Zip_h__
