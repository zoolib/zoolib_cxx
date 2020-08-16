// Copyright (c) 2010 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/ZUtil_WinFile.h"

#if ZCONFIG_SPI_Enabled(Win)

namespace ZooLib {
namespace ZUtil_WinFile {

// =================================================================================================
#pragma mark - ZUtil_WinFile

DWORD sFlush(HANDLE iHANDLE)
	{
	if (not ::FlushFileBuffers(iHANDLE))
		return ::GetLastError();
	return 0;
	}

DWORD sRead(HANDLE iHANDLE, void* oDest, size_t iCount, size_t* oCountRead)
	{
	if (oCountRead)
		*oCountRead = 0;

	if (iCount)
		{
		DWORD countRead = 0;
		if (not ::ReadFile(iHANDLE, oDest, iCount, &countRead, nullptr))
			return ::GetLastError();

		if (oCountRead)
			*oCountRead = countRead;
		}

	return 0;
	}

DWORD sWrite(HANDLE iHANDLE, const void* iSource, size_t iCount, size_t* oCountWritten)
	{
	if (oCountWritten)
		*oCountWritten = 0;

	if (iCount)
		{
		DWORD countWritten = 0;
		if (not ::WriteFile(iHANDLE, iSource, iCount, &countWritten, nullptr))
			return ::GetLastError();

		if (oCountWritten)
			*oCountWritten = countWritten;
		}

	return 0;
	}

DWORD sRead(HANDLE iHANDLE, const uint64* iOffset,
	void* oDest, size_t iCount, size_t* oCountRead)
	{ return sRead(iHANDLE, iOffset, nullptr, oDest, iCount, oCountRead); }

DWORD sWrite(HANDLE iHANDLE, const uint64* iOffset,
	const void* iSource, size_t iCount, size_t* oCountWritten)
	{ return sWrite(iHANDLE, iOffset, nullptr, iSource, iCount, oCountWritten); }

DWORD sRead(HANDLE iHANDLE, const uint64* iOffset, HANDLE iEvent,
	void* oDest, size_t iCount, size_t* oCountRead)
	{
	if (oCountRead)
		*oCountRead = 0;

	OVERLAPPED theOVERLAPPED = {};

	if (iOffset)
		{
		theOVERLAPPED.Offset = *iOffset;
		theOVERLAPPED.OffsetHigh = *iOffset >> 32;
		}

	if (iEvent)
		theOVERLAPPED.hEvent = iEvent;

	DWORD countRead = 0;
	if (not ::ReadFile(iHANDLE, oDest, iCount, &countRead, &theOVERLAPPED))
		{
		int err = ::GetLastError();

		if (err != ERROR_IO_PENDING)
			return err;

		if (iEvent)
			::WaitForSingleObject(iEvent, INFINITE);

		if (not ::GetOverlappedResult(iHANDLE, &theOVERLAPPED, &countRead, FALSE))
			return ::GetLastError();
		}

	if (oCountRead)
		*oCountRead = countRead;

	return 0;
	}

DWORD sWrite(HANDLE iHANDLE, const uint64* iOffset, HANDLE iEvent,
	const void* iSource, size_t iCount, size_t* oCountWritten)
	{
	if (oCountWritten)
		*oCountWritten = 0;

	OVERLAPPED theOVERLAPPED = {};

	if (iOffset)
		{
		theOVERLAPPED.Offset = *iOffset;
		theOVERLAPPED.OffsetHigh = *iOffset >> 32;
		}

	if (iEvent)
		theOVERLAPPED.hEvent = iEvent;

	DWORD countWritten = 0;
	if (not ::WriteFile(iHANDLE, iSource, iCount, &countWritten, &theOVERLAPPED))
		{
		int err = ::GetLastError();

		if (err != ERROR_IO_PENDING)
			return err;

		if (iEvent)
			::WaitForSingleObject(iEvent, INFINITE);

		if (not ::GetOverlappedResult(iHANDLE, &theOVERLAPPED, &countWritten, FALSE))
			return ::GetLastError();
		}

	if (oCountWritten)
		*oCountWritten = countWritten;

	return 0;
	}

} // namespace ZUtil_WinFile
} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(Win)
