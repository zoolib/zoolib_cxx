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

#include "ZCONFIG_SPI.h"

#ifndef ZCONFIG_SPI_Avail__DNSSD
#	define ZCONFIG_SPI_Avail__DNSSD 1
#endif

#ifndef ZCONFIG_SPI_Desired__DNSSD
#	define ZCONFIG_SPI_Desired__DNSSD 1
#endif

#if ZCONFIG_SPI_Enabled(DNSSD)

#include "ZNet_Internet.h" // For ip_port

extern "C" {
#include <dns_sd.h>
}

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
		const string& iName, const string& iRegType,
		const string& iDomain,
		ConstPString* iTXT, size_t iTXTCount);

	ZNetNameRegistered_DNSSD(ip_port iPort,
		const string& iName, const string& iRegType,
		ConstPString* iTXT, size_t iTXTCount);

	ZNetNameRegistered_DNSSD(ip_port iPort,
		const string& iName, const string& iRegType);

	ZNetNameRegistered_DNSSD(ip_port iPort,
		const string& iRegType,
		ConstPString* iTXT, size_t iTXTCount);

	ZNetNameRegistered_DNSSD(ip_port iPort,
		const string& iRegType);

	~ZNetNameRegistered_DNSSD();

	std::string GetName() const;
	std::string GetRegType() const;
	std::string GetDomain() const;
	ip_port GetPort() const;

private:
	void pInit(ip_port iPort,
		const char* iName, const string& iRegType,
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
	string fName;
	string fRegType;
	string fDomain;
	ip_port fPort;
	};

#endif // ZCONFIG_SPI_Enabled(DNSSD)

#endif // __ZNetDNSSD__
