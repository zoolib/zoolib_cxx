/* -------------------------------------------------------------------------------------------------
Copyright (c) 2005 Andrew Green and Learning in Motion, Inc.
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

#include "ZWinService.h"

#if ZCONFIG_SPI_Enabled(Win)

#include "ZLog.h"

#include <vector>

// =================================================================================================
#pragma mark -
#pragma mark * ZWinService

static vector<SERVICE_TABLE_ENTRYW> sEntries;

void ZWinService::sRunDispatcher()
	{
	if (const ZLog::S& s = ZLog::S(ZLog::eDebug, "ZWinService"))
		s << "ZWinService::sRunDispatcher 1";

	if (sEntries.empty())
		{
		// The vector is empty, so no service instances have been
		// created, and there's nothing to do. So bail out.
		return;
		}

	SERVICE_TABLE_ENTRYW nullEntry = { nil, nil };
	sEntries.push_back(nullEntry);

	if (const ZLog::S& s = ZLog::S(ZLog::eDebug, "ZWinService"))
		s << "ZWinService::sRunDispatcher 2";
	::StartServiceCtrlDispatcherW(&sEntries[0]);	
	}

ZWinService::ZWinService(const wstring& iServiceName, LPSERVICE_MAIN_FUNCTIONW iServiceMain, bool iAllowPause)
:	fServiceName(iServiceName),
	fAllowPause(iAllowPause)
	{
	if (const ZLog::S& s = ZLog::S(ZLog::eDebug, "ZWinService"))
		s << "ZWinService::ZWinService";

	SERVICE_TABLE_ENTRYW theEntry;
	theEntry.lpServiceName = const_cast<wchar_t*>(fServiceName.c_str());
	theEntry.lpServiceProc = iServiceMain;
	sEntries.push_back(theEntry);
	}

void ZWinService::Pause()
	{
	this->Paused();
	}

void ZWinService::Continue()
	{
	this->Continued();
	}

DWORD ZWinService::ServiceCtrlHandlerEx(DWORD dwControl, DWORD dwEventType, LPVOID lpEventData)
	{
	if (const ZLog::S& s = ZLog::S(ZLog::eDebug, "ZWinService"))
		s.Writef("ZWinService::ServiceCtrlHandlerEx, dwControl=%d, dwEventType=%d", dwControl, dwEventType);

	switch (dwControl)
		{
		case SERVICE_CONTROL_PAUSE:
			{
			if (fAllowPause)
				{
				ZMutexLocker locker(fMutex_State);
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
				ZMutexLocker locker(fMutex_State);
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
			ZMutexLocker locker(fMutex_State);
			if (fServiceStatus.dwCurrentState == SERVICE_PAUSED || fServiceStatus.dwCurrentState == SERVICE_RUNNING)
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

void ZWinService::Started()
	{
	fServiceStatus.dwCurrentState = SERVICE_RUNNING;
	::SetServiceStatus(fServiceStatusHandle, &fServiceStatus);
	}

void ZWinService::Paused()
	{
	fServiceStatus.dwCurrentState = SERVICE_PAUSED;
	::SetServiceStatus(fServiceStatusHandle, &fServiceStatus);
	}

void ZWinService::Continued()
	{
	fServiceStatus.dwCurrentState = SERVICE_RUNNING;
	::SetServiceStatus(fServiceStatusHandle, &fServiceStatus);
	}

void ZWinService::pServiceMain(DWORD argc, LPWSTR* argv)
	{
	ZMutexLocker locker(fMutex_State);
	fServiceStatus.dwServiceType = SERVICE_WIN32;
	fServiceStatus.dwCurrentState = SERVICE_START_PENDING;
	fServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_PAUSE_CONTINUE;
	fServiceStatus.dwWin32ExitCode = 0;
	fServiceStatus.dwServiceSpecificExitCode = 0;
	fServiceStatus.dwCheckPoint = 0;
	fServiceStatus.dwWaitHint = 0;

	fServiceStatusHandle = ::RegisterServiceCtrlHandlerExW(const_cast<wchar_t*>(fServiceName.c_str()), sServiceCtrlHandlerEx, this);
	if (!fServiceStatusHandle)
		{
		if (const ZLog::S& s = ZLog::S(ZLog::eErr, "ZWinService"))
			s << "ZWinService::pServiceMain, RegisterServiceCtrlHandlerExW failed";
		return;
		}

	locker.Release();

	try
		{
		this->Run(argc, argv);
		}
	catch (...)
		{}

	locker.Acquire();

	fServiceStatus.dwCurrentState = SERVICE_STOPPED;
	::SetServiceStatus(fServiceStatusHandle, &fServiceStatus);
	}

DWORD WINAPI ZWinService::sServiceCtrlHandlerEx(DWORD dwControl, DWORD dwEventType, LPVOID lpEventData, LPVOID lpContext)
	{
	return static_cast<ZWinService*>(lpContext)->ServiceCtrlHandlerEx(dwControl, dwEventType, lpEventData);
	}

#endif // ZCONFIG_SPI_Enabled(Win)
