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

#include "zoolib/ZThreadImp_Win.h"

#if ZCONFIG_API_Enabled(ThreadImp_Win)

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
	{ ::InitializeCriticalSection(&fCRITICAL_SECTION); }

ZMtx_Win::~ZMtx_Win()
	{ ::DeleteCriticalSection(&fCRITICAL_SECTION); }

void ZMtx_Win::Acquire()
	{ ::EnterCriticalSection(&fCRITICAL_SECTION); }

void ZMtx_Win::Release()
	{ ::LeaveCriticalSection(&fCRITICAL_SECTION); }

// =================================================================================================
#pragma mark -
#pragma mark * ZSem_Win

ZSem_Win::ZSem_Win()
	{ fHANDLE = ::CreateSemaphore(nullptr, 0, 0x7FFFFFFF, nullptr); }

ZSem_Win::~ZSem_Win()
	{ ::CloseHandle(fHANDLE); }

void ZSem_Win::Wait()
	{ ::WaitForSingleObject(fHANDLE, INFINITE); }

bool ZSem_Win::Wait(double iTimeout)
	{ return WAIT_OBJECT_0 == ::WaitForSingleObject(fHANDLE, DWORD(iTimeout * 1e3)); }

void ZSem_Win::Signal()
	{ ::ReleaseSemaphore(fHANDLE, 1, nullptr); }

// =================================================================================================
#pragma mark -
#pragma mark * ZThreadImp_Win

ZThreadImp_Win::ID ZThreadImp_Win::sCreate(size_t iStackSize, Proc_t iProc, void* iParam)
	{
	ID theID;
	HANDLE theThreadHANDLE = (HANDLE) ::_beginthreadex(nullptr, 0, iProc, iParam, 0, &theID);
	if (!theThreadHANDLE)
		throw std::bad_alloc();

	::CloseHandle(theThreadHANDLE);

	return theID;
	}

ZThreadImp_Win::ID ZThreadImp_Win::sID()
	{ return ::GetCurrentThreadId(); }

void ZThreadImp_Win::sSleep(double iDuration)
	{ ::Sleep(DWORD(iDuration * 1e3)); }

NAMESPACE_ZOOLIB_END

#endif // ZCONFIG_API_Enabled(ThreadImp_Win)
