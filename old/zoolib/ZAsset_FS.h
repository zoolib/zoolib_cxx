/* -------------------------------------------------------------------------------------------------
Copyright (c) 2002 Andrew Green and Learning in Motion, Inc.
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

#ifndef __ZAsset_FS_h__
#define __ZAsset_FS_h__ 1
#include "zconfig.h"

#include "zoolib/ZAsset.h"
#include "zoolib/ZFile.h"
#include "zoolib/ZThreadOld.h"

namespace ZooLib {

// =================================================================================================
// MARK: - ZAssetRep_FS

/*! \brief ZAssetRep_FS provides ZAsset-like access to a filesystem.

Given a ZFileSpec called theSpec, you can instantiate a root asset
referencing the filesystem entity it represents like this:

ZAsset myAsset(new ZAssetRep_FS(theSpec));

\note Unlike other asset implementations we do not have a ZAssetTree
object kicking around -- the filesystem itself is the equivalent
of the asset tree, and its lifetime extends beyond that of
any running code.
*/

class ZAssetRep_FS : public ZAssetRep
	{
public:
	ZAssetRep_FS(const ZFileSpec& iSpec);
	ZAssetRep_FS(const ZFileSpec& iSpec, const std::vector<std::string>& iComps);
	ZAssetRep_FS(const ZFileSpec& iSpec,
		const std::vector<std::string>::iterator& iBegin,
		const std::vector<std::string>::iterator& iEnd);

	virtual ~ZAssetRep_FS();

// From ZAssetRep
	virtual std::string GetName();
	virtual std::string GetPath();
	virtual ZRef<ZAssetRep> GetParentAssetRep();

	virtual ZRef<ZAssetRep> ResolvePath(const char* iPath);

	virtual ZRef<ZAssetIterRep> CreateIterRep();

	virtual void GetData(const void** oData, size_t* oSize);

	virtual ZRef<ZStreamerRPos> OpenRPos();

protected:
	ZFileSpec fSpec;
	std::vector<std::string> fComps;
	
	ZMutex fMutex;
	char* fData;
	size_t fDataSize;
	};

} // namespace ZooLib

#endif // __ZAsset_FS_h__
