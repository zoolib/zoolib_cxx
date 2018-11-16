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

#include "zoolib/Memory.h"

#include "zoolib/ZYad_FS.h"

namespace ZooLib {
namespace ZYad_FS {

namespace { // anonymous

// =================================================================================================
#pragma mark - ChanAtR_NameRefYad

class ChanAtR_NameRefYad_FS
:	public ChanAtR_NameRefYad
	{
public:
	ChanAtR_NameRefYad_FS(const FileSpec& iFS)
	:	fFileSpec(iFS)
		{}

// From ChanAtR_NameRefYad
	virtual size_t ReadAt(const Name& iLoc, RefYad* oDest, size_t iCount)
		{
		if (iCount)
			{
			*oDest = ZYad_FS::sYadR(fFileSpec.Child(iLoc));
			return 1;
			}
		return 0;
		}

	const FileSpec fFileSpec;
	};

using YadMapAtR_NameRefYad_FS = Channer_T<ChanAtR_NameRefYad_FS>;

} // anonymous namespace

// =================================================================================================
#pragma mark - ZYad_FS::sYadR

ZRef<YadR> sYadR(const FileSpec& iFS)
	{
	switch (iFS.Kind())
		{
		case File::kindDir:
			return new YadMapAtR_NameRefYad_FS(iFS);
		case File::kindFile:
			return iFS.OpenRPos();
		default:
			break;
		}
	return null;
	}

} // namespace ZYad_FS
} // namespace ZooLib
