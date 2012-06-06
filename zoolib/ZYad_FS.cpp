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

#include "zoolib/ZYad_FS.h"

namespace ZooLib {

using std::min;
using std::string;

class ZYadR_FS;

// =================================================================================================
// MARK: - ZYadR_FS

class ZYadR_FS : public virtual ZYadR
	{
public:
	ZYadR_FS(const ZFileSpec& iFS);

// From ZYadR
	virtual ZRef<ZYadR> Meta();

protected:
	const ZFileSpec fFS;
	};

ZYadR_FS::ZYadR_FS(const ZFileSpec& iFS)
:	fFS(iFS)
	{}

ZRef<ZYadR> ZYadR_FS::Meta()
	{ return ZYadR::Meta(); }

// =================================================================================================
// MARK: - ZYadMapRPos_Dir

class ZYadMapRPos_Dir
:	public ZYadR_FS,
	public ZYadMapRPos
	{
public:
	ZYadMapRPos_Dir(const ZFileSpec& iFS);
	ZYadMapRPos_Dir(const ZFileSpec& iFS, const ZFileIter& iFileIter);

// From ZYadMapR via ZYadMapRPos
	virtual ZRef<ZYadR> ReadInc(ZName& oName);

// From ZYadMapRClone via ZYadMapRPos
	virtual ZRef<ZYadMapRClone> Clone();

// From ZYadMapRPos
	virtual void SetPosition(const ZName& iName);

private:
	ZFileIter fFileIter;
	};

ZYadMapRPos_Dir::ZYadMapRPos_Dir(const ZFileSpec& iFS)
:	ZYadR_FS(iFS),
	fFileIter(iFS)
	{}

ZYadMapRPos_Dir::ZYadMapRPos_Dir(const ZFileSpec& iFS, const ZFileIter& iFileIter)
:	ZYadR_FS(iFS),
	fFileIter(iFileIter)
	{}

ZRef<ZYadR> ZYadMapRPos_Dir::ReadInc(ZName& oName)
	{
	while (fFileIter)
		{
		const ZFileSpec cur = fFileIter.Current();
		const string curName = fFileIter.CurrentName();
		fFileIter.Advance();
		if (ZRef<ZYadR> result = ZYad_FS::sYadR(cur))
			{
			oName = curName;
			return result;
			}
		}
	return null;
	}

ZRef<ZYadMapRClone> ZYadMapRPos_Dir::Clone()
	{ return new ZYadMapRPos_Dir(fFS, fFileIter); }

void ZYadMapRPos_Dir::SetPosition(const ZName& iName)
	{
	string8 asString = string8(iName);
	while (fFileIter && fFileIter.CurrentName() != asString)
		fFileIter.Advance();
	}

// =================================================================================================
// MARK: - ZYadStreamerRPos_File

class ZYadStreamerRPos_File
:	public ZYadR_FS,
	public ZYadStreamerR,
	public virtual ZStreamerRPos
	{
public:
	ZYadStreamerRPos_File(const ZFileSpec& iFS);
	virtual ~ZYadStreamerRPos_File();

// From ZYadR
	virtual bool IsSimple(const ZYadOptions& iOptions);

// From ZStreamerRPos
	virtual const ZStreamRPos& GetStreamRPos();

private:
	ZRef<ZStreamerRPos> fStreamerRPos;
	};

ZYadStreamerRPos_File::ZYadStreamerRPos_File(const ZFileSpec& iFS)
:	ZYadR_FS(iFS)
	{}

ZYadStreamerRPos_File::~ZYadStreamerRPos_File()
	{}

bool ZYadStreamerRPos_File::IsSimple(const ZYadOptions& iOptions)
	{ return false; }

const ZStreamRPos& ZYadStreamerRPos_File::GetStreamRPos()
	{
	if (!fStreamerRPos)
		fStreamerRPos = fFS.OpenRPos();

	return fStreamerRPos->GetStreamRPos();
	}

// =================================================================================================
// MARK: - ZYad_FS::sYadR

ZRef<ZYadR> ZYad_FS::sYadR(const ZFileSpec& iFS)
	{
	if (iFS.IsDir()) return new ZYadMapRPos_Dir(iFS);
	else if (iFS.IsFile()) return new ZYadStreamerRPos_File(iFS);
	else return null;
	}

} // namespace ZooLib
