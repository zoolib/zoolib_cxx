/* -------------------------------------------------------------------------------------------------
Copyright (c) 2001 Andrew Green and Learning in Motion, Inc.
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

#ifndef __ZAsset_Win_h__
#define __ZAsset_Win_h__ 1
#include "zconfig.h"

#include "zoolib/ZCompat_Win.h"

#if ZCONFIG_SPI_Enabled(Win)

#include "zoolib/ZAsset_Std.h"

namespace ZooLib {

// =================================================================================================
// MARK: - ZAssetTree_Win_MemoryMapped

class ZAssetTree_Win_MemoryMapped : public ZAssetTree_Std_Memory
	{
public:
	ZAssetTree_Win_MemoryMapped(HANDLE iFileHandle, bool iAdopt, size_t iStart, size_t iLength);
	virtual ~ZAssetTree_Win_MemoryMapped();

protected:
	void LoadUp(HANDLE iFileHandle, size_t iStart, size_t iLength);

	HANDLE fHANDLE_File;
	bool fAdopted;

	HANDLE fHANDLE_FileMapping;
	LPVOID fMappedAddress;
	};

// =================================================================================================
// MARK: - ZAssetTree_Win_MultiResource

class ZStreamRPos_Win_MultiResource;

class ZAssetTree_Win_MultiResource : public ZAssetTree_Std_Stream
	{
public:
	ZAssetTree_Win_MultiResource(
		HMODULE iHMODULE, const std::string& iType, const std::string& iName);
	virtual ~ZAssetTree_Win_MultiResource();

protected:
	ZStreamRPos_Win_MultiResource* fStream_Resource;
	};

} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(Win)

#endif // __ZAsset_Win_h__
