// Copyright (c) 2013 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/ZUtil_WinFirewall.h"

#if ZCONFIG_API_Enabled(Util_WinFirewall)

#include "zoolib/ZLog.h"

#include <shellapi.h>
#include <winhttp.h>

// For WNetGetUniversalNameW
#pragma comment(lib, "mpr.lib")

namespace ZooLib {
namespace ZUtil_WinFirewall {

using namespace ZWinCOM;

// =================================================================================================
#pragma mark - ZUtil_WinFirewall

ZRef<INetFwMgr> sMgr()
	{ return ZWinCOM::sCreate<NetFwMgr,INetFwMgr>(); }

ZRef<INetFwProfile>
sCurrentProfile_LocalPolicy(ZRef<INetFwMgr> iMgr)
	{
	if (false)
		{}
	else if (not iMgr)
		{
		if (ZLOGF(s, eDebug))
			s << "No mgr";
		}
	else if (OParam<ZRef<INetFwPolicy>,false> thePolicy = iMgr->get_LocalPolicy(thePolicy)
		|| not thePolicy.Get())
		{
		if (ZLOGF(s, eDebug))
			s << "No policy";
		}
	else if (OParam<ZRef<INetFwProfile>,false> theProfile =
		thePolicy->get_CurrentProfile(theProfile)
		|| not theProfile.Get())
		{
		if (ZLOGF(s, eDebug))
			s << "No profile";
		}
	else
		{
		return theProfile.Get();
		}
	return null;
	}

ZRef<INetFwAuthorizedApplications>
sAuthorizedApplications_CurrentProfile_LocalPolicy(ZRef<INetFwMgr> iMgr)
	{
	if (false)
		{}
	else if (ZRef<INetFwProfile,false> theProfile = sCurrentProfile_LocalPolicy(iMgr))
		{}
	else if (OParam<ZRef<INetFwAuthorizedApplications>,false> theApps =
		theProfile->get_AuthorizedApplications(theApps)
		|| not theApps.Get())
		{
		if (ZLOGF(s, eDebug))
			s << "No authorizedApplications";
		}
	else
		{
		return theApps.Get();
		}
	return null;
	}

ZQ<string16> sQUNCIsh(const UTF16* iPath)
	{
	for (DWORD theSize = 1024; theSize <= 1024*1024; /*no inc*/)
		{
		std::vector<char> buffer(theSize);
		const DWORD result = ::WNetGetUniversalNameW(
			iPath,
			UNIVERSAL_NAME_INFO_LEVEL,
			&buffer[0], &theSize);

		if (result == ERROR_MORE_DATA)
			continue;

		if (result == NO_ERROR)
			return ((UNIVERSAL_NAME_INFOW*)&buffer[0])->lpUniversalName;

		if (result == ERROR_NOT_CONNECTED)
			return iPath;

		break;
		}
	return null;
	}

} // namespace ZUtil_WinFirewall
} // namespace ZooLib

#endif // ZCONFIG_API_Enabled(Util_WinFirewall)
