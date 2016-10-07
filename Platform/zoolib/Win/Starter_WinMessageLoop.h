/* -------------------------------------------------------------------------------------------------
Copyright (c) 2011 Andrew Green
http://www.zoolib.org

Permission is hereby granted, free of charge, to any person obtaining a copy of this software
and associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge,Publish, distribute,
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

#ifndef __ZooLib_Starter_WinMessageLoop_h__
#define __ZooLib_Starter_WinMessageLoop_h__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_SPI.h"

#include "zoolib/Starter_EventLoopBase.h"

#if ZCONFIG_SPI_Enabled(Win)

#include "zoolib/ZCompat_Win.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark Starter_WinMessageLoop

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
