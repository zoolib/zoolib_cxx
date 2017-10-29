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
#pragma mark -
#pragma mark ChanAtR_NameRefYad

class ChanAtR_NameRefYad_FS
:	public ChanAtR_NameRefYad
	{
public:
	ChanAtR_NameRefYad(const FileSpec& iFS);

// From ZYadMapR via ZYadMapRPos
	virtual ZRef<ZYadR> ReadInc(Name& oName);

// From ZYadMapRClone via ZYadMapRPos
	virtual ZRef<ZYadMapRClone> Clone();

// From ZYadMapRPos via ZYadMapAtRPos
	virtual void SetPosition(const Name& iName);

// From ZYadMapAtR via ZYadMapAtRPos
	virtual ZRef<ZYadR> ReadAt(const Name& iName);

private:
	FileSpec fFileSpec;
	ZFileIter fFileIter;
	};

YadMapAtRPos::YadMapAtRPos(const FileSpec& iFS)
:	fFileSpec(iFS)
,	fFileIter(iFS)
	{}

	FileSpec fFileSpec;
	ZFileIter fFileIter;

YadMapAtRPos::YadMapAtRPos(const FileSpec& iFS, const ZFileIter& iFileIter)
:	fFileSpec(iFS)
,	fFileIter(iFileIter)
	{}

ZRef<ZYadR> YadMapAtRPos::ReadInc(Name& oName)
	{
	while (fFileIter)
		{
		const FileSpec cur = fFileIter.Current();
		const Name curName = fFileIter.CurrentName();
		fFileIter.Advance();
		if (ZRef<ZYadR> result = ZYad_FS::sYadR(cur))
			{
			oName = curName;
			return result;
			}
		}
	return null;
	}

ZRef<ZYadMapRClone> YadMapAtRPos::Clone()
	{ return new YadMapAtRPos(fFileSpec, fFileIter); }

//void YadMapAtRPos::SetPosition(const Name& iName)
//	{
//	string8 asString = string8(iName);
//	while (fFileIter && fFileIter.CurrentName() != asString)
//		fFileIter.Advance();
//	}

ZRef<ZYadR> YadMapAtRPos::ReadAt(const Name& iName)
	{ return ZYad_FS::sYadR(fFileSpec.Child(iName)); }

} // anonymous namespace

// =================================================================================================
#pragma mark -
#pragma mark ZYad_FS::sYadR

ZRef<YadR> sYadR(const FileSpec& iFS)
	{
	switch (iFS.Kind())
		{
		case ZFile::kindDir:
			return new YadMapAtRPos(iFS);
		case ZFile::kindFile:
			return iFS.OpenRPos();
		default:
			break;
		}
	return null;
	}

} // namespace ZYad_FS
} // namespace ZooLib
