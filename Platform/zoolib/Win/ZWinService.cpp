 // Copyright (c) 2005 Andrew Green and Learning in Motion, Inc.
// MIT License. http://www.zoolib.org

#include "zoolib/ZWinService.h"

#if ZCONFIG_SPI_Enabled(Win)

#include "zoolib/ZLog.h"
#include "zoolib/ZMemory.h"
#include "zoolib/ZUtil_Strim_Operators.h"

#include <vector>

namespace ZooLib {

using std::vector;
using std::wstring;

// =================================================================================================
#pragma mark - ZWinService

static vector<SERVICE_TABLE_ENTRYW> spEntries;

bool ZWinService::sIsService()
	{
	if (HWINSTA theHWINSTA = ::GetProcessWindowStation())
		{
		USEROBJECTFLAGS theUSEROBJECTFLAGS = {};
		if (::GetUserObjectInformation(
			theHWINSTA, UOI_FLAGS, &theUSEROBJECTFLAGS, sizeof(theUSEROBJECTFLAGS), nullptr))
			{
			if (not theUSEROBJECTFLAGS.dwFlags & WSF_VISIBLE)
				return true;
			}
		}
	return false;
	}

void ZWinService::sRunDispatcher()
	{
	ZLOGFUNCTION(eDebug);

	if (spEntries.empty())
		{
		// The vector is empty, so no service instances have been
		// created, and there's nothing to do. So bail out.
		return;
		}

	SERVICE_TABLE_ENTRYW nullEntry = { nullptr, nullptr };
	spEntries.push_back(nullEntry);

	::StartServiceCtrlDispatcherW(&spEntries[0]);
	}

ZWinService::ZWinService(
	const wstring& iServiceName, LPSERVICE_MAIN_FUNCTIONW iServiceMain, bool iAllowPause)
:	fServiceName(iServiceName),
	fAllowPause(iAllowPause)
	{
	ZLOGFUNCTION(eDebug);

	sMemZero_T(fServiceStatus);

	SERVICE_TABLE_ENTRYW theEntry;
	theEntry.lpServiceName = const_cast<wchar_t*>(fServiceName.c_str());
	theEntry.lpServiceProc = iServiceMain;
	spEntries.push_back(theEntry);
	}

void ZWinService::Pause()
	{ this->pPaused(); }

void ZWinService::Continue()
	{ this->pContinued(); }

DWORD ZWinService::ServiceCtrlHandlerEx(DWORD dwControl, DWORD dwEventType, LPVOID lpEventData)
	{
	if (ZLOGPF(s, eDebug))
		s << "dwControl=" << dwControl << " dwEventType=" << dwEventType;

	switch (dwControl)
		{
		case SERVICE_CONTROL_PAUSE:
			{
			if (fAllowPause)
				{
				ZAcqMtxR acq(fMtx);
				if (fServiceStatus.dwCurrentState == SERVICE_RUNNING)
					{
					fServiceStatus.dwCurrentState = SERVICE_PAUSE_PENDING;
					::SetServiceStatus(fServiceStatusHandle, &fServiceStatus);
					try
						{
						this->Pause();
						return NO_ERROR;
						}
					catch (...)
						{}
					}
				}
			break;
			}
		case SERVICE_CONTROL_CONTINUE:
			{
			if (fAllowPause)
				{
				ZAcqMtxR acq(fMtx);
				if (fServiceStatus.dwCurrentState == SERVICE_PAUSED)
					{
					fServiceStatus.dwCurrentState = SERVICE_CONTINUE_PENDING;
					::SetServiceStatus(fServiceStatusHandle, &fServiceStatus);
					try
						{
						this->Continue();
						return NO_ERROR;
						}
					catch (...)
						{}
					}
				}
			break;
			}
		case SERVICE_CONTROL_STOP:
			{
			ZAcqMtxR acq(fMtx);
			if (fServiceStatus.dwCurrentState == SERVICE_PAUSED
				|| fServiceStatus.dwCurrentState == SERVICE_RUNNING)
				{
				fServiceStatus.dwCurrentState = SERVICE_STOP_PENDING;
				::SetServiceStatus(fServiceStatusHandle, &fServiceStatus);
				try
					{
					this->Stop();
					return NO_ERROR;
					}
				catch (...)
					{}
				}
			}
		}
	return ERROR_CALL_NOT_IMPLEMENTED;
	}

void ZWinService::pStarted()
	{
	ZAcqMtxR acq(fMtx);
	fServiceStatus.dwCurrentState = SERVICE_RUNNING;
	::SetServiceStatus(fServiceStatusHandle, &fServiceStatus);
	}

void ZWinService::pPaused()
	{
	ZAcqMtxR acq(fMtx);
	fServiceStatus.dwCurrentState = SERVICE_PAUSED;
	::SetServiceStatus(fServiceStatusHandle, &fServiceStatus);
	}

void ZWinService::pContinued()
	{
	ZAcqMtxR acq(fMtx);
	fServiceStatus.dwCurrentState = SERVICE_RUNNING;
	::SetServiceStatus(fServiceStatusHandle, &fServiceStatus);
	}

void ZWinService::pServiceMain(DWORD argc, LPWSTR* argv)
	{
	ZGuardMtxR guard(fMtx);
	sMemZero_T(fServiceStatus);
	fServiceStatus.dwServiceType = SERVICE_WIN32;
	fServiceStatus.dwCurrentState = SERVICE_START_PENDING;
	fServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;
	if (fAllowPause)
		fServiceStatus.dwControlsAccepted |= SERVICE_ACCEPT_PAUSE_CONTINUE;

	fServiceStatusHandle = ::RegisterServiceCtrlHandlerExW(
		const_cast<wchar_t*>(fServiceName.c_str()), spServiceCtrlHandlerEx, this);

	if (!fServiceStatusHandle)
		{
		if (ZLOGPF(s, eErr))
			s << "RegisterServiceCtrlHandlerExW failed with error: " << ::GetLastError();
		return;
		}
	guard.Release();

	try
		{
		this->Run(argc, argv);
		}
	catch (...)
		{}

	guard.Acquire();

	fServiceStatus.dwCurrentState = SERVICE_STOPPED;
	::SetServiceStatus(fServiceStatusHandle, &fServiceStatus);
	}

DWORD WINAPI ZWinService::spServiceCtrlHandlerEx(
	DWORD dwControl, DWORD dwEventType, LPVOID lpEventData, LPVOID lpContext)
	{
	return static_cast<ZWinService*>(lpContext)->
		ServiceCtrlHandlerEx(dwControl, dwEventType, lpEventData);
	}

} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(Win)
