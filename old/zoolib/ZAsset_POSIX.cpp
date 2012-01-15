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

#include "zoolib/ZAsset_POSIX.h"

#if ZCONFIG_SPI_Enabled(POSIX)

#include <stdexcept>
#include <string> // because runtime_error may require it

#include <errno.h>
#include <unistd.h>
#include <sys/mman.h>

using std::runtime_error;

namespace ZooLib {

// =================================================================================================
// MARK: - ZAssetTree_POSIX_MemoryMapped

ZAssetTree_POSIX_MemoryMapped::ZAssetTree_POSIX_MemoryMapped(
	int inFileDescriptor, bool inAdopt, size_t inStart, size_t inLength)
	{
	ZAssertStop(1, inFileDescriptor != 0);

	fFileDescriptor = inFileDescriptor;
	fAdopted = inAdopt;
	try
		{
		this->LoadUp(inFileDescriptor, inStart, inLength);
		}
	catch (...)
		{
		if (fAdopted)
			::close(fFileDescriptor);
		throw;
		}
	}

ZAssetTree_POSIX_MemoryMapped::~ZAssetTree_POSIX_MemoryMapped()
	{
	this->ShutDown();
	::munmap(fMappedAddress, fMappedLength);

	if (fAdopted)
		::close(fFileDescriptor);
	}

void ZAssetTree_POSIX_MemoryMapped::LoadUp(int inFileDescriptor, size_t inStart, size_t inLength)
	{
	size_t pageSize = ::getpagesize();

	// Round inStart down to align with pageSize
	off_t realStart = (inStart / pageSize) * pageSize;

	// inLength must be extended to compensate for any change in the start offset
	fMappedLength = inLength + inStart - realStart;
	fMappedAddress = ::mmap(nullptr, fMappedLength,
		PROT_READ, MAP_PRIVATE, inFileDescriptor, realStart);

	if (fMappedAddress == MAP_FAILED)
		throw runtime_error("ZAssetTree_POSIX_MemoryMapped, could not map file");

	ZAssetTree_Std_Memory::LoadUp(
		reinterpret_cast<char*>(fMappedAddress) + inStart - realStart,
		inLength);
	}

} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(POSIX)
