// Copyright (c) 2011 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Starter_WinMessageLoop_h__
#define __ZooLib_Starter_WinMessageLoop_h__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_SPI.h"

#include "zoolib/Starter_EventLoopBase.h"

#if ZCONFIG_SPI_Enabled(Win)

#include "zoolib/ZCompat_Win.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - Starter_WinMessageLoop

class Starter_WinMessageLoop
:	public Starter_EventLoopBase
	{
public:
	Starter_WinMessageLoop();
	virtual ~Starter_WinMessageLoop();

// From ZCounted via Starter_EventLoopBase
	virtual void Initialize();
	virtual void Finalize();

// Our protocol
	void Disable();

protected:
// From Starter_EventLoopBase
	virtual bool pTrigger();

private:
	ZQ<LRESULT> pWindowProc(HWND iHWND, UINT iMessage, WPARAM iWPARAM, LPARAM iLPARAM);

	HWND fHWND;
	};

} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(Win)

#endif // __ZooLib_Starter_WinMessageLoop_h__
