/* -------------------------------------------------------------------------------------------------
Copyright (c) 2008 Andrew Green
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

#include "zoolib/ZThread_Win.h"

#if ZCONFIG_API_Enabled(Thread_Win)

#include "zoolib/ZCompat_Win.h"

#include <new> // For std::bad_alloc
#include <process.h> // For _beginthreadex

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * ZTSS_Win

ZTSS_Win::Key ZTSS_Win::sCreate()
	{ return ::TlsAlloc(); }

void ZTSS_Win::sFree(Key iKey)
	{ ::TlsFree(iKey); }

void ZTSS_Win::sSet(Key iKey, Value iValue)
	{ ::TlsSetValue(iKey, iValue); }

ZTSS_Win::Value ZTSS_Win::sGet(Key iKey)
	{ return ::TlsGetValue(iKey); }

// =================================================================================================
#pragma mark -
#pragma mark * ZMtx_Win




ZMtx_Win::ZMtx_Win(const char* iName)
	{
	ZAssertCompile(sizeof(Dummy_CRITICAL_SECTION) == sizeof(CRITICAL_SECTION));

	::InitializeCriticalSection(reinterpret_cast<CRITICAL_SECTION*>(&fCRITICAL_SECTION));
	}

ZMtx_Win::~ZMtx_Win()
	{ ::DeleteCriticalSection(reinterpret_cast<CRITICAL_SECTION*>(&fCRITICAL_SECTION)); }

void ZMtx_Win::Acquire()
	{ ::EnterCriticalSection(reinterpret_cast<CRITICAL_SECTION*>(&fCRITICAL_SECTION)); }

void ZMtx_Win::Release()
	{ ::LeaveCriticalSection(reinterpret_cast<CRITICAL_SECTION*>(&fCRITICAL_SECTION)); }

// =================================================================================================
#pragma mark -
#pragma mark * ZSem_Win

ZSem_Win::ZSem_Win()
	{ fHANDLE = ::CreateSemaphore(nullptr, 0, 0x7FFFFFFF, nullptr); }

ZSem_Win::~ZSem_Win()
	{ ::CloseHandle(fHANDLE); }

void ZSem_Win::Wait()
	{ ::WaitForSingleObject(fHANDLE, INFINITE); }

bool ZSem_Win::WaitFor(double iTimeout)
	{ return WAIT_OBJECT_0 == ::WaitForSingleObject(fHANDLE, DWORD(iTimeout * 1e3)); }

bool ZSem_Win::WaitUntil(ZTime iDeadline)
	{ return this->WaitFor(iDeadline - ZTime::sSystem()); }

void ZSem_Win::Signal()
	{ ::ReleaseSemaphore(fHANDLE, 1, nullptr); }

// =================================================================================================
#pragma mark -
#pragma mark * ZThread_Win

namespace ZThread_Win {

void sCreateRaw(size_t iStackSize, ProcRaw_t iProc, void* iParam)
	{
	ID theID;
	HANDLE theThreadHANDLE = (HANDLE) ::_beginthreadex(nullptr, 0, iProc, iParam, 0, &theID);
	if (!theThreadHANDLE || theThreadHANDLE == (HANDLE)-1)
		throw std::bad_alloc();
	::CloseHandle(theThreadHANDLE);
	}

ID sID()
	{ return ::GetCurrentThreadId(); }

void sSleep(double iDuration)
	{ ::Sleep(DWORD(iDuration * 1e3)); }

} // namespace ZThread_Win

NAMESPACE_ZOOLIB_END

#endif // ZCONFIG_API_Enabled(Thread_Win)
