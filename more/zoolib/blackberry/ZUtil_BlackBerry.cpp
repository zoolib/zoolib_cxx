/* -------------------------------------------------------------------------------------------------
Copyright (c) 2009 Andrew Green
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

#include "zoolib/blackberry/ZUtil_BlackBerry.h"

#include "zoolib/blackberry/ZBlackBerry_BBDevMgr.h"
#include "zoolib/blackberry/ZBlackBerry_Client.h"
#include "zoolib/blackberry/ZBlackBerry_OSXUSB.h"
#include "zoolib/blackberry/ZBlackBerry_Union.h"
#include "zoolib/ZLog.h"
#include "zoolib/ZNet_Internet.h"
#include "zoolib/ZNet_Internet_Socket.h"
#include "zoolib/ZNet_Local.h"
#include "zoolib/ZStreamerFactory_Precon.h"

namespace ZooLib {
namespace ZUtil_BlackBerry {

// =================================================================================================
// MARK: - ZUtil_BlackBerry

ZRef<ZBlackBerry::Manager> sMakeManager(ZRef<ZStreamerRWConFactory> iFactory)
	{
	try
		{
		if (iFactory)
			{
			if (ZRef<ZStreamerRWCon> theSRWCon = iFactory->MakeStreamerRWCon())
				{
				ZRef<ZStreamerRWConFactory> theFactory =
					new ZStreamerRWConFactory_Precon(theSRWCon, iFactory);
				return new ZBlackBerry::Manager_Client(theFactory, true);
				}
			}
		}
	catch (...)
		{}
	return null;
	}

ZRef<ZBlackBerry::Manager> sMakeManager()
	{
	using namespace ZBlackBerry;
	// First try using an env-specified tcp socket
	if (const char* daemonAddressTCP = ::getenv("org_zoolib_BlackBerryDaemon_tcp"))
		{
		if (ZLOG(s, eDebug, "sMakeManager"))
			s << "org_zoolib_BlackBerryDaemon_tcp = " << daemonAddressTCP;
		if (ZRef<ZBlackBerry::Manager> theManager =
			sMakeManager(new ZNetName_Internet(daemonAddressTCP, 17983)))
			{
			return theManager;
			}
		}

	// Then an env-specified local socket
	if (const char* daemonAddressLocal = ::getenv("org_zoolib_BlackBerryDaemon_local"))
		{
		if (ZLOG(s, eDebug, "sMakeManager"))
			s << "org_zoolib_BlackBerryDaemon_local = " << daemonAddressLocal;
		if (ZRef<ZBlackBerry::Manager> theManager =
			sMakeManager(new ZNetAddress_Local(daemonAddressLocal)))
			{
			return theManager;
			}
		}

	if (ZRef<ZBlackBerry::Manager> theManager =
		sMakeManager(new ZNetAddress_Local("/tmp/org.zoolib.BlackBerryDaemon")))
		{
		return theManager;
		}

	#if ZCONFIG_API_Enabled(BlackBerry_OSXUSB)
		try
			{
			return new ZBlackBerry::Manager_OSXUSB(::CFRunLoopGetCurrent(), true);
			}
		catch (...)
			{}
	#endif

	#if ZCONFIG_API_Enabled(BlackBerry_BBDevMgr)
		try
			{
			return new ZBlackBerry::Manager_BBDevMgr;
			}
		catch (...)
			{}
	#endif

	return null;
	}

} // namespace ZUtil_BlackBerry
} // namespace ZooLib
