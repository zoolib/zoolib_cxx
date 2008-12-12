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

namespace ZooLib {

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
#pragma mark * ZSemTimeout_Win

ZSemTimeout_Win::ZSemTimeout_Win()
	{ fHANDLE = ::CreateSemaphore(nil, 0, 0x7FFFFFFF, nil); }

ZSemTimeout_Win::~ZSemTimeout_Win()
	{ ::CloseHandle(fHANDLE); }

void ZSemTimeout_Win::Wait()
	{ ::WaitForSingleObject(fHANDLE, INFINITE); }

bool ZSemTimeout_Win::Wait(double iTimeout)
	{ return WAIT_OBJECT_0 == ::WaitForSingleObject(fHANDLE, iTimeout * 1e3); }

void ZSemTimeout_Win::Signal()
	{ ::ReleaseSemaphore(fHANDLE, 1, nil); }

// =================================================================================================
#pragma mark -
#pragma mark * ZMtx_Win

ZMtx_Win::ZMtx_Win()
	{ ::InitializeCriticalSection(&fCRITICAL_SECTION); }

ZMtx_Win::~ZMtx_Win()
	{ ::DeleteCriticalSection(&fCRITICAL_SECTION); }

void ZMtx_Win::Acquire()
	{ ::EnterCriticalSection(&fCRITICAL_SECTION); }

void ZMtx_Win::Release()
	{ ::LeaveCriticalSection(&fCRITICAL_SECTION); }

// =================================================================================================
#pragma mark -
#pragma mark * ZCnd_Win

ZCnd_Win::ZCnd_Win()
	{}

ZCnd_Win::~ZCnd_Win()
	{}

void ZCnd_Win::Wait(ZMtx_Win& iMtx)
	{ this->Imp_Wait(iMtx); }

void ZCnd_Win::Wait(ZMtx_Win& iMtx, double iTimeout)
	{ this->Imp_Wait(iMtx, iTimeout); }

void ZCnd_Win::Signal()
	{ this->Imp_Signal(); }

void ZCnd_Win::Broadcast()
	{ this->Imp_Broadcast(); }

// =================================================================================================
#pragma mark -
#pragma mark * ZThreadImp_Win

ZThreadImp_Win::ID ZThreadImp_Win::sCreate(size_t iStackSize, Proc_t iProc, void* iParam)
	{
	ID theID;
	HANDLE theThreadHANDLE = (HANDLE) ::_beginthreadex(nil, 0, iProc, iParam, 0, &theID);
	if (!theThreadHANDLE)
		throw std::bad_alloc();

	return theID;
	}

ZThreadImp_Win::ID ZThreadImp_Win::sID()
	{ return ::GetCurrentThreadId(); }

} // namespace ZooLib

#endif // ZCONFIG_API_Enabled(ThreadImp_Win)
