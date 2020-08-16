// Copyright (c) 2013 Andrew Green. MIT License. http://www.zoolib.org

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
