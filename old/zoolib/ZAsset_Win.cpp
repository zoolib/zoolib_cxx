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

#include "zoolib/ZAsset_Win.h"

#if ZCONFIG_SPI_Enabled(Win)

#include "zoolib/ZStream_Win.h"

#include <stdexcept>
#include <string> // because runtime_error may require it

using std::runtime_error;
using std::string;

namespace ZooLib {

// =================================================================================================
// MARK: - ZAssetTree_Win_MemoryMapped

ZAssetTree_Win_MemoryMapped::ZAssetTree_Win_MemoryMapped(
	HANDLE iFileHandle, bool iAdopt, size_t iStart, size_t iLength)
	{
	ZAssertStop(1, iFileHandle != nullptr && iFileHandle != INVALID_HANDLE_VALUE);
	fHANDLE_File = iFileHandle;
	fAdopted = iAdopt;
	try
		{
		this->LoadUp(fHANDLE_File, iStart, iLength);
		}
	catch (...)
		{
		if (fAdopted && fHANDLE_File)
			::CloseHandle(fHANDLE_File);
		throw;
		}
	}

ZAssetTree_Win_MemoryMapped::~ZAssetTree_Win_MemoryMapped()
	{
	this->ShutDown();

	if (fMappedAddress)
		::UnmapViewOfFile(fMappedAddress);
	fMappedAddress = nullptr;

	if (fHANDLE_FileMapping)
		::CloseHandle(fHANDLE_FileMapping);
	fHANDLE_FileMapping = nullptr;

	if (fAdopted)
		{
		if (fHANDLE_File)
			::CloseHandle(fHANDLE_File);
		}
	}

void ZAssetTree_Win_MemoryMapped::LoadUp(HANDLE iFileHandle, size_t iStart, size_t iLength)
	{
	fHANDLE_FileMapping = nullptr;
	fMappedAddress = nullptr;

	SYSTEM_INFO theSYSTEM_INFO;
	::GetSystemInfo(&theSYSTEM_INFO);

	size_t realStart = theSYSTEM_INFO.dwAllocationGranularity
		* (iStart / theSYSTEM_INFO.dwAllocationGranularity);

	size_t mappedLength = iLength + iStart - realStart;

	try
		{
		fHANDLE_FileMapping = ::CreateFileMapping(iFileHandle, nullptr, PAGE_READONLY, 0, 0, nullptr);

		if (fHANDLE_FileMapping == nullptr)
			throw runtime_error("ZAssetTree_Win_MemoryMapped, CreateFileMapping failed");

		fMappedAddress = ::MapViewOfFile(
			fHANDLE_FileMapping, FILE_MAP_READ, 0, realStart, mappedLength);

		if (fMappedAddress == nullptr)
			throw runtime_error("ZAssetTree_Win_MemoryMapped, MapViewOfFile failed");
		}
	catch (...)
		{
		if (fMappedAddress)
			::UnmapViewOfFile(fMappedAddress);
		fMappedAddress = nullptr;
		if (fHANDLE_FileMapping)
			::CloseHandle(fHANDLE_FileMapping);
		fHANDLE_FileMapping = nullptr;
		throw;
		}

	ZAssetTree_Std_Memory::LoadUp(
		reinterpret_cast<char*>(fMappedAddress) + iStart - realStart, iLength);
	}

// =================================================================================================
// MARK: - ZAssetTree_Win_MultiResource

ZAssetTree_Win_MultiResource::ZAssetTree_Win_MultiResource(
	HMODULE iHMODULE, const string& iType, const string& iName)
	{
	fStream_Resource = new ZStreamRPos_Win_MultiResource(iHMODULE, iType, iName);
	this->LoadUp(fStream_Resource, 0, fStream_Resource->GetSize());
	}

ZAssetTree_Win_MultiResource::~ZAssetTree_Win_MultiResource()
	{
	this->ShutDown();
	delete fStream_Resource;
	}

} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(Win)
