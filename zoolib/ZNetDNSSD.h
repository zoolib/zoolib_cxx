/* -------------------------------------------------------------------------------------------------
Copyright (c) 2007 Andrew Green
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

#ifndef __ZNetDNSSD__
#define __ZNetDNSSD__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_API.h"
#include "zoolib/ZCONFIG_SPI.h"

#ifndef ZCONFIG_API_Avail__NetDNSSD
#	define ZCONFIG_API_Avail__NetDNSSD ZCONFIG_SPI_Enabled(DNSSD)
#endif

#ifndef ZCONFIG_API_Desired__NetDNSSD
#	define ZCONFIG_API_Desired__NetDNSSD 1
#endif


#if ZCONFIG_API_Enabled(NetDNSSD)

#include "zoolib/ZNet_Internet.h" // For ip_port
#include "zoolib/ZThread.h"

extern "C" {
#include <dns_sd.h>
}

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * ZNetNameRegistered_DNSSD

class ZNetNameRegistered_DNSSD
	{
protected:
	ZNetNameRegistered_DNSSD() {}
	ZNetNameRegistered_DNSSD(const ZNetNameRegistered_DNSSD& other); // Not implemented
	ZNetNameRegistered_DNSSD& operator=(const ZNetNameRegistered_DNSSD& other); // Not implemented

public:
	typedef const unsigned char* ConstPString;

	ZNetNameRegistered_DNSSD(ip_port iPort,
		const std::string& iName, const std::string& iRegType,
		const std::string& iDomain,
		ConstPString* iTXT, size_t iTXTCount);

	ZNetNameRegistered_DNSSD(ip_port iPort,
		const std::string& iName, const std::string& iRegType,
		ConstPString* iTXT, size_t iTXTCount);

	ZNetNameRegistered_DNSSD(ip_port iPort,
		const std::string& iName, const std::string& iRegType);

	ZNetNameRegistered_DNSSD(ip_port iPort,
		const std::string& iRegType,
		ConstPString* iTXT, size_t iTXTCount);

	ZNetNameRegistered_DNSSD(ip_port iPort,
		const std::string& iRegType);

	~ZNetNameRegistered_DNSSD();

	std::string GetName() const;
	std::string GetRegType() const;
	std::string GetDomain() const;
	ip_port GetPort() const;

private:
	void pInit(ip_port iPort,
		const char* iName, const std::string& iRegType,
		const char* iDomain,
		ConstPString* iTXT, size_t iTXTCount);

	void pDNSServiceRegisterReply(
		DNSServiceFlags flags,
		DNSServiceErrorType errorCode,
		const char* name,
		const char* regtype,
		const char* domain);

	static void spDNSServiceRegisterReply(
		DNSServiceRef sdRef,
		DNSServiceFlags flags,
		DNSServiceErrorType errorCode,
		const char* name,
		const char* regtype,
		const char* domain,
		void* context);

	DNSServiceRef fDNSServiceRef;
	ZMutex fMutex;
	std::string fName;
	std::string fRegType;
	std::string fDomain;
	ip_port fPort;
	};

NAMESPACE_ZOOLIB_END

#endif // ZCONFIG_API_Enabled(NetDNSSD)

#endif // __ZNetDNSSD__
