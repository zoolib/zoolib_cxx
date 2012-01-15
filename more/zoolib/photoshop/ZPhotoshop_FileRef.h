/* -------------------------------------------------------------------------------------------------
Copyright (c) 2009 Andrew Green
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

#ifndef __ZPhotoshop_FileRef_h__
#define __ZPhotoshop_FileRef_h__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_SPI.h"

#include "zoolib/ZMacFixup.h"
#include "zoolib/ZQ.h"
#include "zoolib/ZTrail.h"
#include "zoolib/ZTypes.h"
#include "zoolib/ZUnicodeString.h"

#include "zoolib/photoshop/ZPhotoshop.h"

#include "SPFiles.h" // For SPPlatformFileSpecification

namespace ZooLib {

// =================================================================================================
// MARK: - ZPhotoshop

namespace ZPhotoshop {

// =================================================================================================
// MARK: - Public utilities

ZTrail sWinAsTrail(const string8& iWin);
ZQ<ZTrail> sAsTrail(const SPPlatformFileSpecification& iSpec);

// =================================================================================================
// MARK: - FileRef

class FileRef
	{
public:
	FileRef();
	FileRef(const FileRef& iOther);
	~FileRef();
	FileRef& operator=(const FileRef& iOther);

	FileRef(Handle iHandle);
	FileRef(Adopt_T<Handle> iHandle);

	FileRef& operator=(Handle iHandle);
	FileRef& operator=(Adopt_T<Handle> iHandle);

	explicit FileRef(const ZTrail& iTrail);

	ZQ<ZTrail> AsTrail() const;

	Handle Get() const;
	Handle Orphan();
	Handle& OParam();

private:
	Handle fHandle;
	};

} // namespace ZPhotoshop
} // namespace ZooLib

#endif // __ZPhotoshop_FileRef_h__
