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
#include "zoolib/ZYad_Std.h"

namespace ZooLib {
namespace ZYad_FS {

namespace { // anonymous

// =================================================================================================
// MARK: - YadStreamerRPos

class YadStreamerRPos
:	public virtual ZYadStreamerR
,	public virtual ZStreamerRPos
	{
public:
	YadStreamerRPos(const ZFileSpec& iFS);
	virtual ~YadStreamerRPos();

// From ZYadR
	virtual bool IsSimple(const ZYadOptions& iOptions);

// From ZStreamerRPos
	virtual const ZStreamRPos& GetStreamRPos();

private:
	ZFileSpec fFileSpec;
	ZRef<ZStreamerRPos> fStreamerRPos;
	};

YadStreamerRPos::YadStreamerRPos(const ZFileSpec& iFS)
:	fFileSpec(iFS)
	{}

YadStreamerRPos::~YadStreamerRPos()
	{}

bool YadStreamerRPos::IsSimple(const ZYadOptions& iOptions)
	{ return false; }

const ZStreamRPos& YadStreamerRPos::GetStreamRPos()
	{
	if (not fStreamerRPos)
		fStreamerRPos = fFileSpec.OpenRPos();

	return fStreamerRPos->GetStreamRPos();
	}

// =================================================================================================
// MARK: - YadMapAtRPos

class YadMapAtRPos
:	public ZYadMapAtRPos
	{
public:
	YadMapAtRPos(const ZFileSpec& iFS);
	YadMapAtRPos(const ZFileSpec& iFS, const ZFileIter& iFileIter);

// From ZYadMapR via ZYadMapRPos
	virtual ZRef<ZYadR> ReadInc(ZName& oName);

// From ZYadMapRClone via ZYadMapRPos
	virtual ZRef<ZYadMapRClone> Clone();

// From ZYadMapRPos via ZYadMapAtRPos
	virtual void SetPosition(const ZName& iName);

// From ZYadMapAtR via ZYadMapAtRPos
	virtual ZRef<ZYadR> ReadAt(const ZName& iName);

private:
	ZFileSpec fFileSpec;
	ZFileIter fFileIter;
	};

YadMapAtRPos::YadMapAtRPos(const ZFileSpec& iFS)
:	fFileSpec(iFS)
,	fFileIter(iFS)
	{}

	ZFileSpec fFileSpec;
	ZFileIter fFileIter;

YadMapAtRPos::YadMapAtRPos(const ZFileSpec& iFS, const ZFileIter& iFileIter)
:	fFileSpec(iFS)
,	fFileIter(iFileIter)
	{}

ZRef<ZYadR> YadMapAtRPos::ReadInc(ZName& oName)
	{
	while (fFileIter)
		{
		const ZFileSpec cur = fFileIter.Current();
		const ZName curName = fFileIter.CurrentName();
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

void YadMapAtRPos::SetPosition(const ZName& iName)
	{
	string8 asString = string8(iName);
	while (fFileIter && fFileIter.CurrentName() != asString)
		fFileIter.Advance();
	}

ZRef<ZYadR> YadMapAtRPos::ReadAt(const ZName& iName)
	{ return ZYad_FS::sYadR(fFileSpec.Child(iName)); }

} // anonymous namespace

// =================================================================================================
// MARK: - ZYad_FS::sYadR

ZRef<ZYadR> sYadR(const ZFileSpec& iFS)
	{
	switch (iFS.Kind())
		{
		case ZFile::kindDir:
			return new YadMapAtRPos(iFS);
		case ZFile::kindFile:
			return new YadStreamerRPos(iFS);
		default:
			break;
		}
	return null;
	}

} // namespace ZYad_FS
} // namespace ZooLib
