// Copyright (c) 2005 Andrew Green and Learning in Motion, Inc.
// MIT License. http://www.zoolib.org

#ifndef __ZWinService_h__
#define __ZWinService_h__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_SPI.h"

#if ZCONFIG_SPI_Enabled(Win)

#include "zoolib/ZCompat_Win.h"
#include "zoolib/ZThread.h"

#include <string>

namespace ZooLib {

// =================================================================================================
#pragma mark - ZWinService

class ZWinService
	{
public:
	static bool sIsService();
	static void sRunDispatcher();

	static const bool kAllowPause = true;
	static const bool kDontAllowPause = false;

	ZWinService(
		const std::wstring& iServiceName, LPSERVICE_MAIN_FUNCTIONW iServiceMain, bool iAllowPause);

	virtual void Run(DWORD argc, LPWSTR* argv) = 0;

	virtual void Stop() = 0;
	virtual void Pause();
	virtual void Continue();

	virtual DWORD ServiceCtrlHandlerEx(DWORD dwControl, DWORD dwEventType, LPVOID lpEventData);

	const std::wstring& Name();

protected:
	void pStarted();
	void pPaused();
	void pContinued();

	void pServiceMain(DWORD argc, LPWSTR* argv);

private:
	static DWORD WINAPI spServiceCtrlHandlerEx(
		DWORD dwControl, DWORD dwEventType, LPVOID lpEventData, LPVOID lpContext);

	std::wstring fServiceName;
	SERVICE_STATUS fServiceStatus;
	SERVICE_STATUS_HANDLE fServiceStatusHandle;
	bool fAllowPause;
	ZMtxR fMtx;
	};

// =================================================================================================
#pragma mark - ZWinService_T

template <class T>
class ZWinService_T : public ZWinService
	{
protected:
	ZWinService_T(const wchar_t* iServiceName, bool iAllowPause)
	:	ZWinService(iServiceName, spServiceMain, iAllowPause)
		{
		ZAssert(not spInstance());
		spInstance() = this;
		}

private:
	static void WINAPI spServiceMain(DWORD argc, LPWSTR *argv)
		{ spInstance()->pServiceMain(argc, argv); }

	static ZWinService_T*& spInstance()
		{
		// This method is a sneaky way to have static storage
		// for a template class.
		static ZWinService_T* sInstanceStorage;
		return sInstanceStorage;
		}
	};

} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(Win)

#endif // __ZWinService_h__
