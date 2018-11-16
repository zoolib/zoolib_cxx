/* -------------------------------------------------------------------------------------------------
Copyright (c) 2013 Andrew Green
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

#ifndef __ZUtil_WinFirewall_h__
#define __ZUtil_WinFirewall_h__ 1
#include "zconfig.h"

// Pull in appropriate windows header fixups
#include "zoolib/ZNet_Internet_WinSock.h"

#ifndef ZCONFIG_API_Avail__Util_WinFirewall
	#define ZCONFIG_API_Avail__Util_WinFirewall ZCONFIG_SPI_Enabled(Win)
#endif

#ifndef ZCONFIG_API_Desired__Util_WinFirewall
	#define ZCONFIG_API_Desired__Util_WinFirewall 1
#endif

#if ZCONFIG_API_Enabled(Util_WinFirewall)

#include <netfw.h>

#include "zoolib/ZWinCOM.h"

namespace ZooLib {
namespace ZUtil_WinFirewall {

// =================================================================================================
#pragma mark - ZUtil_WinFirewall

ZRef<INetFwMgr> sMgr();

ZRef<INetFwProfile>
sCurrentProfile_LocalPolicy(ZRef<INetFwMgr> iMgr);

ZRef<INetFwAuthorizedApplications>
sAuthorizedApplications_CurrentProfile_LocalPolicy(ZRef<INetFwMgr> iMgr);

ZQ<string16> sQUNCIsh(const UTF16* iPath);

} // namespace ZUtil_WinFirewall
} // namespace ZooLib

#endif // ZCONFIG_API_Enabled(Util_WinFirewall)

#endif // __ZUtil_WinFirewall_h__
