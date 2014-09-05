/* -------------------------------------------------------------------------------------------------
Copyright (c) 2006 Andrew Green and Learning in Motion, Inc.
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

#include "zoolib/ZUtil_MacOSX.h"

#if ZCONFIG_SPI_Enabled(Carbon) && ZCONFIG(Processor, PPC)

#include ZMACINCLUDE3(CoreServices,CarbonCore,OSUtils.h) // For MakeDataExecutable

using std::vector;

namespace ZooLib {

// =================================================================================================
// MARK: - ZUtil_MacOSX

void ZUtil_MacOSX::sCreateThunks_CFMCalledByMachO(
	void* ioFuncs, size_t iCount, vector<char>& ioStorage)
	{
	ioStorage.resize(iCount * sizeof(Thunk_CFMCalledByMachO));
	Thunk_CFMCalledByMachO* localDest = reinterpret_cast<Thunk_CFMCalledByMachO*>(&ioStorage[0]);
	void** localFuncs = static_cast<void**>(ioFuncs);
	while (iCount--)
		{
		localDest->glue[0] = *localFuncs;
		localDest->glue[1] = 0;
		*localFuncs++ = localDest++;
		}
	}

void ZUtil_MacOSX::sCreateThunks_MachOCalledByCFM(
	void* ioFuncs, size_t iCount, vector<char>& ioStorage)
	{
	ioStorage.resize(iCount * sizeof(Thunk_MachOCalledByCFM));
	Thunk_MachOCalledByCFM* localDest = reinterpret_cast<Thunk_MachOCalledByCFM*>(&ioStorage[0]);
	void** localFuncs = static_cast<void**>(ioFuncs);
	while (iCount--)
		{
		const uint32 theFunc = reinterpret_cast<uint32>(*localFuncs);
		localDest->glue[0] = 0x3D800000 | (theFunc >> 16);
		localDest->glue[1] = 0x618C0000 | (theFunc & 0xFFFF);
		localDest->glue[2] = 0x800C0000;
		localDest->glue[3] = 0x804C0004;
		localDest->glue[4] = 0x7C0903A6;
		localDest->glue[5] = 0x4E800420;
		*localFuncs = localDest++;
		}

	::MakeDataExecutable(&ioStorage[0], ioStorage.size());
	}

} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(Carbon) && ZCONFIG(Processor, PPC)
