/* -------------------------------------------------------------------------------------------------
Copyright (c) 2010 Andrew Green
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

#include "zoolib/ZNet_Local_Win.h"

#if ZCONFIG_API_Enabled(Net_Local_Win)

ZMACRO_MSVCStaticLib_cpp(Net_Local_Win)

#include "zoolib/ZFunctionChain.h"
#include "zoolib/ZMemory.h"
#include "zoolib/ZRef_WinHANDLE.h"
#include "zoolib/ZUnicode.h"
#include "zoolib/ZUtil_WinFile.h"

#ifndef FILE_FLAG_FIRST_PIPE_INSTANCE
	#if (_WIN32_WINNT >= 0x0500)
		#define FILE_FLAG_FIRST_PIPE_INSTANCE 524288
	#endif
#endif


namespace ZooLib {

using std::runtime_error;
using std::string;

// =================================================================================================
#pragma mark -
#pragma mark * Factory functions

namespace { // anonymous

class Make_NameLookup
:	public ZFunctionChain_T<ZRef<ZNetNameLookup>, ZNetName_Local::LookupParam_t>
	{
	virtual bool Invoke(Result_t& oResult, Param_t iParam)
		{
		oResult = new ZNetNameLookup_Local_Win(iParam);
		return true;
		}
	} sMaker0;


class Make_Listener
:	public ZFunctionChain_T<ZRef<ZNetListener_Local>, ZNetListener_Local::MakeParam_t>
	{
	virtual bool Invoke(Result_t& oResult, Param_t iParam)
		{
		oResult = new ZNetListener_Local_Win(iParam.f0);
		return true;
		}
	} sMaker1;


class Make_Endpoint
:	public ZFunctionChain_T<ZRef<ZNetEndpoint_Local>, ZNetEndpoint_Local::MakeParam_t>
	{
	virtual bool Invoke(Result_t& oResult, Param_t iParam)
		{
		oResult = new ZNetEndpoint_Local_Win(iParam.f0);
		return true;
		}
	} sMaker2;

} // anonymous namespace

// =================================================================================================
#pragma mark -
#pragma mark * Helpers

static string16 spAsPipeName(const string8& iName)
	{ return ZUnicode::sAsUTF16("\\\\.\\pipe\\" + iName); }

static ZRef<HANDLE> spConnect(const string& iName)
	{
	const string16 thePipeName = spAsPipeName(iName);

	for (;;)
		{
		HANDLE theHANDLE = ::CreateFileW(
			thePipeName.c_str(), // pipe name
			GENERIC_READ | GENERIC_WRITE, // read and write access
			0, // no sharing
			nullptr, // default security attributes
			OPEN_EXISTING, // opens existing pipe
			FILE_FLAG_OVERLAPPED, // attributes
			nullptr); // no template file

		if (theHANDLE && theHANDLE != INVALID_HANDLE_VALUE)
			return Adopt& theHANDLE;

		if (::GetLastError() != ERROR_PIPE_BUSY)
			{
			if (::WaitNamedPipeW(thePipeName.c_str(), 20000))
				continue;
			}

		throw runtime_error("spConnect, couldn't connect");
		}
	}

static ZRef<HANDLE> spCreateNamedPipe(const string16& iPipeName, bool iFirst)
	{
	DWORD theOpenMode = PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED;

	if (iFirst)
		theOpenMode |= FILE_FLAG_FIRST_PIPE_INSTANCE;

	HANDLE theHANDLE = ::CreateNamedPipeW(
		iPipeName.c_str(), // lpName
		theOpenMode, // dwOpenMode
		PIPE_TYPE_BYTE | PIPE_WAIT, // dwPipeMode
		PIPE_UNLIMITED_INSTANCES, // nMaxInstances
		0, // nOutBufferSize
		0, // nInBufferSize
		0, // nDefaultTimeOut
		nullptr // lpSecurityAttributes
		);

	if (!theHANDLE || INVALID_HANDLE_VALUE == theHANDLE)
		throw runtime_error("ZNetListener_Local_Win, spCreateNamedPipe failed");

	return Adopt& theHANDLE;
	}

static ZRef<HANDLE> spCreateEvent()
	{
	HANDLE theHANDLE = ::CreateEvent(nullptr, true, false, nullptr);
	if (!theHANDLE || INVALID_HANDLE_VALUE == theHANDLE)
		throw runtime_error("spCreateEvent failed");
	return Adopt& theHANDLE;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZNetNameLookup_Local_Win

ZNetNameLookup_Local_Win::ZNetNameLookup_Local_Win(const std::string& iPath)
:	fPath(iPath),
	fFinished(false)
	{
	if (fPath.empty())
		fFinished = true;
	}

ZNetNameLookup_Local_Win::~ZNetNameLookup_Local_Win()
	{}

void ZNetNameLookup_Local_Win::Start()
	{}

bool ZNetNameLookup_Local_Win::Finished()
	{ return fFinished; }

void ZNetNameLookup_Local_Win::Advance()
	{
	ZAssertStop(2, !fFinished);
	fFinished = true;
	}

ZRef<ZNetAddress> ZNetNameLookup_Local_Win::CurrentAddress()
	{
	if (!fFinished)
		return new ZNetAddress_Local(fPath);

	return null;
	}

ZRef<ZNetName> ZNetNameLookup_Local_Win::CurrentName()
	{ return new ZNetName_Local(fPath); }

// =================================================================================================
#pragma mark -
#pragma mark * ZNetListener_Local_Win

ZNetListener_Local_Win::ZNetListener_Local_Win(const string& iName)
:	fPath(spAsPipeName(iName))
,	fHANDLE(spCreateNamedPipe(fPath, true))
,	fEvent(spCreateEvent())
	{
	ZMemZero_T(fOVERLAPPED);
	fOVERLAPPED.hEvent = fEvent;
	}

ZNetListener_Local_Win::~ZNetListener_Local_Win()
	{}

ZRef<ZNetEndpoint> ZNetListener_Local_Win::Listen()
	{
	bool success = ::ConnectNamedPipe(fHANDLE, &fOVERLAPPED);

//	DWORD err = ::GetLastError();
//	if (!success && (err == ERROR_IO_PENDING || err == ERROR_PIPE_LISTENING))
//		::WaitForSingleObjectEx(fEvent, INFINITE, TRUE);

	DWORD dummy;
	success = ::GetOverlappedResult(fHANDLE, &fOVERLAPPED, &dummy, TRUE);

	ZRef<HANDLE> theHANDLE;
	theHANDLE.swap(fHANDLE);
	fHANDLE = spCreateNamedPipe(fPath, false);
	return new ZNetEndpoint_Local_Win(theHANDLE);
	}

void ZNetListener_Local_Win::CancelListen()
	{
	// We should do a CancelIoEx here
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZNetEndpoint_Local_Win

ZNetEndpoint_Local_Win::ZNetEndpoint_Local_Win(const ZRef<HANDLE>& iHANDLE)
	{ this->pInit(iHANDLE); }

ZNetEndpoint_Local_Win::ZNetEndpoint_Local_Win(const string& iName)
	{ this->pInit(spConnect(iName)); }

ZNetEndpoint_Local_Win::~ZNetEndpoint_Local_Win()
	{}

const ZStreamRCon& ZNetEndpoint_Local_Win::GetStreamRCon()
	{ return *this; }

const ZStreamWCon& ZNetEndpoint_Local_Win::GetStreamWCon()
	{ return *this; }

ZRef<ZNetAddress> ZNetEndpoint_Local_Win::GetRemoteAddress()
	{
	ZUnimplemented();
	return null;
	}

ZRef<HANDLE> ZNetEndpoint_Local_Win::GetHANDLE()
	{ return fHANDLE; }

void ZNetEndpoint_Local_Win::Imp_Read(void* oDest, size_t iCount, size_t* oCountRead)
	{
	ZUtil_WinFile::sRead(fHANDLE, nullptr, fEvent_Read,
		oDest, iCount, oCountRead);
	}

size_t ZNetEndpoint_Local_Win::Imp_CountReadable()
	{
	DWORD countAvail;
	if (::PeekNamedPipe(fHANDLE,
		sGarbageBuffer, sizeof(sGarbageBuffer), nullptr, &countAvail, nullptr))
		{
		return countAvail;
		}
	return 0;
	}

bool ZNetEndpoint_Local_Win::Imp_WaitReadable(double iTimeout)
	{
	return WAIT_OBJECT_0 == ::WaitForSingleObject(fHANDLE, DWORD(iTimeout * 1e3));
	}

void ZNetEndpoint_Local_Win::Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten)
	{
	ZUtil_WinFile::sWrite(fHANDLE, nullptr, fEvent_Write,
		iSource, iCount, oCountWritten);
	}

void ZNetEndpoint_Local_Win::Imp_Flush()
	{
	// Do NOT call FlushFileBuffers. It won't return till the far end has read everything
	// we've written, which in common situations will likely cause deadlock.
	}

bool ZNetEndpoint_Local_Win::Imp_ReceiveDisconnect(double iTimeout)
	{
	for (;;)
		{
		uint64 countSkipped;
		this->Skip(1024, &countSkipped);
		if (countSkipped == 0)
			return true;
		}
	}

void ZNetEndpoint_Local_Win::Imp_SendDisconnect()
	{
	ZUtil_WinFile::sFlush(fHANDLE);
	::DisconnectNamedPipe(fHANDLE);
	}

void ZNetEndpoint_Local_Win::Imp_Abort()
	{
	::DisconnectNamedPipe(fHANDLE);
	}

void ZNetEndpoint_Local_Win::pInit(const ZRef<HANDLE>& iHANDLE)
	{
	fHANDLE = iHANDLE;
	fEvent_Read = spCreateEvent();
	fEvent_Write = spCreateEvent();
	}

// =================================================================================================

} // namespace ZooLib

#endif // ZCONFIG_API_Enabled(Net_Local_Win)
