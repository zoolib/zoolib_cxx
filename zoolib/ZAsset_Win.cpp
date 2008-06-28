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

// =================================================================================================
#pragma mark -
#pragma mark * ZAssetTree_Win_MemoryMapped

ZAssetTree_Win_MemoryMapped::ZAssetTree_Win_MemoryMapped(HANDLE inFileHandle, bool inAdopt, size_t inStart, size_t inLength)
	{
	ZAssertStop(1, inFileHandle != nil && inFileHandle != INVALID_HANDLE_VALUE);
	fHANDLE_File = inFileHandle;
	fAdopted = inAdopt;
	try
		{
		this->LoadUp(fHANDLE_File, inStart, inLength);
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
	fMappedAddress = nil;

	if (fHANDLE_FileMapping)
		::CloseHandle(fHANDLE_FileMapping);
	fHANDLE_FileMapping = nil;

	if (fAdopted)
		{
		if (fHANDLE_File)
			::CloseHandle(fHANDLE_File);
		}
	}

void ZAssetTree_Win_MemoryMapped::LoadUp(HANDLE inFileHandle, size_t inStart, size_t inLength)
	{
	fHANDLE_FileMapping = nil;
	fMappedAddress = nil;

	SYSTEM_INFO theSYSTEM_INFO;
	::GetSystemInfo(&theSYSTEM_INFO);

	size_t realStart = (inStart / theSYSTEM_INFO.dwAllocationGranularity) * theSYSTEM_INFO.dwAllocationGranularity;
	size_t mappedLength = inLength + inStart - realStart;

	try
		{
		fHANDLE_FileMapping = ::CreateFileMapping(inFileHandle, nil, PAGE_READONLY, 0, 0, nil);
		if (fHANDLE_FileMapping == nil)
			throw runtime_error("ZAssetTree_Win_MemoryMapped, CreateFileMapping failed");
		fMappedAddress = ::MapViewOfFile(fHANDLE_FileMapping, FILE_MAP_READ, 0, realStart, mappedLength);
		if (fMappedAddress == nil)
			throw runtime_error("ZAssetTree_Win_MemoryMapped, MapViewOfFile failed");
		}
	catch (...)
		{
		if (fMappedAddress)
			::UnmapViewOfFile(fMappedAddress);
		fMappedAddress = nil;
		if (fHANDLE_FileMapping)
			::CloseHandle(fHANDLE_FileMapping);
		fHANDLE_FileMapping = nil;
		throw;
		}

	ZAssetTree_Std_Memory::LoadUp(reinterpret_cast<char*>(fMappedAddress) + inStart - realStart, inLength);
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZAssetTree_Win_MultiResource

ZAssetTree_Win_MultiResource::ZAssetTree_Win_MultiResource(HMODULE inHMODULE, const string& inType, const string& inName)
	{
	fStream_Resource = new ZStreamRPos_Win_MultiResource(inHMODULE, inType, inName);
	this->LoadUp(fStream_Resource, 0, fStream_Resource->GetSize());
	}

ZAssetTree_Win_MultiResource::~ZAssetTree_Win_MultiResource()
	{
	this->ShutDown();
	delete fStream_Resource;
	}

#endif // ZCONFIG_SPI_Enabled(Win)
