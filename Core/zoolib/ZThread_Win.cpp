// Copyright (c) 2008 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/ZThread_Win.h"

#if ZCONFIG_API_Enabled(Thread_Win)

#include "zoolib/ZCompat_Win.h"

#include <new> // For std::bad_alloc
#include <process.h> // For _beginthreadex

namespace ZooLib {

// =================================================================================================
#pragma mark - ZThread_Win

void ZThread_Win::sStartRaw(size_t iStackSize, ProcRaw_t iProc, void* iParam)
	{
	ID theID;
	HANDLE theThreadHANDLE = (HANDLE) ::_beginthreadex(nullptr, 0, iProc, iParam, 0, &theID);
	if (not theThreadHANDLE || theThreadHANDLE == (HANDLE)-1)
		throw std::bad_alloc();
	::CloseHandle(theThreadHANDLE);
	}

ZThread_Win::ID ZThread_Win::sID()
	{ return ::GetCurrentThreadId(); }

void ZThread_Win::sSleep(double iDuration)
	{ ::Sleep(DWORD(iDuration * 1e3)); }

void ZThread_Win::sSetName(const char* iName)
	{
	// This uses a secret handshake with VC debugger and WinDbg,
	// the system itself has no notion of thread names.

	// http://msdn.microsoft.com/en-us/library/xcb2z8hs(VS.90).aspx
	// http://blogs.msdn.com/b/stevejs/archive/2005/12/19/505815.aspx

	#if ZCONFIG_Debug
		#pragma pack(push,8)
		struct THREADNAME_INFO
			{
			DWORD dwType; // Must be 0x1000.
			LPCSTR szName; // Pointer to name (in user addr space).
			DWORD dwThreadID; // Thread ID (-1=caller thread).
			DWORD dwFlags; // Reserved for future use, must be zero.
			};
		#pragma pack(pop)

		THREADNAME_INFO info;
		info.dwType = 0x1000;
		info.szName = iName;
		info.dwThreadID = -1;
		info.dwFlags = 0;

		__try
			{
			RaiseException(0x406D1388, 0, sizeof(info)/sizeof(ULONG_PTR), (ULONG_PTR*)&info);
			}
		__except(EXCEPTION_EXECUTE_HANDLER)
			{}
	#endif
	}

ZAssertCompile(sizeof(ZThread_Win::Dummy_CRITICAL_SECTION) == sizeof(CRITICAL_SECTION));

// =================================================================================================
#pragma mark - ZTSS_Win

ZTSS_Win::Key ZTSS_Win::sCreate()
	{ return ::TlsAlloc(); }

void ZTSS_Win::sFree(Key iKey)
	{ ::TlsFree(iKey); }

void ZTSS_Win::sSet(Key iKey, Value iValue)
	{ ::TlsSetValue(iKey, iValue); }

ZTSS_Win::Value ZTSS_Win::sGet(Key iKey)
	{ return ::TlsGetValue(iKey); }

// =================================================================================================
#pragma mark - ZMtx_Win

ZMtx_Win::ZMtx_Win()
	{ ::InitializeCriticalSection(reinterpret_cast<CRITICAL_SECTION*>(&fCRITICAL_SECTION)); }

ZMtx_Win::~ZMtx_Win()
	{ ::DeleteCriticalSection(reinterpret_cast<CRITICAL_SECTION*>(&fCRITICAL_SECTION)); }

void ZMtx_Win::Acquire()
	{ ::EnterCriticalSection(reinterpret_cast<CRITICAL_SECTION*>(&fCRITICAL_SECTION)); }

void ZMtx_Win::Release()
	{ ::LeaveCriticalSection(reinterpret_cast<CRITICAL_SECTION*>(&fCRITICAL_SECTION)); }

// =================================================================================================
#pragma mark - ZSem_Win

ZSem_Win::ZSem_Win()
	{ fHANDLE = ::CreateSemaphore(nullptr, 0, 0x7FFFFFFF, nullptr); }

ZSem_Win::~ZSem_Win()
	{ ::CloseHandle(fHANDLE); }

void ZSem_Win::Procure()
	{ ::WaitForSingleObject(fHANDLE, INFINITE); }

bool ZSem_Win::TryProcureFor(double iTimeout)
	{ return WAIT_OBJECT_0 == ::WaitForSingleObject(fHANDLE, DWORD(iTimeout * 1e3)); }

bool ZSem_Win::TryProcureUntil(double iDeadline)
	{ return this->TryProcureFor(iDeadline - Time::sSystem()); }

void ZSem_Win::Vacate()
	{ ::ReleaseSemaphore(fHANDLE, 1, nullptr); }

} // namespace ZooLib

#endif // ZCONFIG_API_Enabled(Thread_Win)
