/* -------------------------------------------------------------------------------------------------
Copyright (c) 2011 Andrew Green
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

#include "zoolib/ZCaller_CarbonEvents.h"

#if ZCONFIG_SPI_Enabled(Carbon64)

#include "zoolib/ZSingleton.h"
#include "zoolib/ZUtil_CarbonEvents.h"

namespace ZooLib {

// =================================================================================================
// MARK: - ZCaller_CarbonEvents

ZRef<ZCaller_CarbonEvents> ZCaller_CarbonEvents::sGet()
	{ return sSingleton<ZRef<ZCaller_CarbonEvents> >(); }

bool ZCaller_CarbonEvents::pTrigger()
	{
	this->Retain();
	ZUtil_CarbonEvents::sInvokeOnMainThread(true, spCallback, this);
	return true;
	}

void ZCaller_CarbonEvents::spCallback(void* iRefcon)
	{
	if (ZRef<ZCaller_CarbonEvents> theCaller = static_cast<ZCaller_CarbonEvents*>(iRefcon))
		{
		theCaller->Release();
		theCaller->pCall();
		}
	}

} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(Carbon64)
