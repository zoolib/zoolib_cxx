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

#ifndef __ZDNS_SD_h__
#define __ZDNS_SD_h__ 1
#include "zconfig.h"

#include "zoolib/Callable.h"

#include "zoolib/ZCompat_NonCopyable.h"
#include "zoolib/ZNet_Internet.h" // For ip_port
#include "zoolib/ZThread.h"

#include <vector>

extern "C" {
#include <dns_sd.h>
}

namespace ZooLib {
namespace DNS_SD {

// =================================================================================================
// MARK: - DNSService

class DNSService
:	public ZCounted, NonCopyable
	{
public:
	DNSService();

// From ZCounted
	virtual void Initialize();
	virtual void Finalize();

// Our protocol
	DNSServiceRef GetDNSServiceRef();

protected:
	void pSocketReadable();
	void pWatchSocket();

	ZMtxR fMtxR;
	DNSServiceRef fDNSServiceRef;
	ZRef<Callable_Void> fCallable_SocketReadable;
	};

// =================================================================================================
// MARK: - Registration

class Registration
:	public DNSService
	{
public:
	typedef const unsigned char* ConstPString;

	typedef Callable<void(ZRef<Registration>)> Callable_t;

	Registration(const ZRef<Callable_t>& iCallable,
		ip_port iPort,
		const std::string& iName, const std::string& iRegType, const std::string& iDomain,
		ConstPString* iTXT, size_t iTXTCount);

	Registration(const ZRef<Callable_t>& iCallable,
		ip_port iPort,
		const std::string& iName, const std::string& iRegType,
		ConstPString* iTXT, size_t iTXTCount);

	Registration(const ZRef<Callable_t>& iCallable,
		ip_port iPort,
		const std::string& iName, const std::string& iRegType);

	Registration(const ZRef<Callable_t>& iCallable,
		ip_port iPort,
		const std::string& iRegType,
		ConstPString* iTXT, size_t iTXTCount);

	Registration(const ZRef<Callable_t>& iCallable,
		ip_port iPort,
		const std::string& iRegType);

	~Registration();

// From ZCounted via DNSService
	virtual void Initialize();

// Our protocol
	std::string GetName_Desired() const;
	std::string GetRegType() const;
	std::string GetDomain() const;
	ip_port GetPort() const;

	ZQ<std::string> QGetName_Registered() const;

private:
	void pCallback(
		DNSServiceFlags flags,
		DNSServiceErrorType errorCode,
		const char* name,
		const char* regtype,
		const char* domain);

	static void DNSSD_API spCallback(
		DNSServiceRef sdRef,
		DNSServiceFlags flags,
		DNSServiceErrorType errorCode,
		const char* name,
		const char* regtype,
		const char* domain,
		void* context);

	ZRef<Callable_t> fCallable;

	ip_port fPort;
	std::string fName;
	std::string fRegType;
	std::string fDomain;
	std::vector<unsigned char> fTXT;
	ZQ<std::string> fName_RegisteredQ;
	};

// =================================================================================================
// MARK: - Browse

class Browse
:	public DNSService
	{
public:
	typedef Callable<void(
		bool iMoreComing,
		bool iAdd,
		const std::string& iServiceName,
		const std::string& iRegType,
		const std::string& iReplyDomain)> Callable_t;

	Browse(const ZRef<Callable_t>& iCallable, const std::string& iRegType);

// From ZCounted via DNSService
	virtual void Initialize();

private:
	static void DNSSD_API spCallback(
		DNSServiceRef sdRef,
		DNSServiceFlags flags,
		uint32_t interfaceIndex,
		DNSServiceErrorType errorCode,
		const char* serviceName,
		const char* regtype,
		const char* replyDomain,
		void* context);

	ZRef<Callable_t> fCallable;

	std::string fRegType;
	std::string fDomain;
	};

// =================================================================================================
// MARK: - Resolve

class Resolve
:	public DNSService
	{
public:
	typedef Callable<void(
		bool iMoreComing,
		const std::string& iServiceName,
		const std::string& iHostTarget,
		uint16_t port,
		uint16_t txtLen,
		const unsigned char* txtRecord)> Callable_t;

	Resolve(const ZRef<Callable_t>& iCallable,
		const std::string& iName,
		const std::string& iRegType,
		const std::string& iDomain);

// From ZCounted via DNSService
	virtual void Initialize();

private:
	static void DNSSD_API spCallback(
		DNSServiceRef sdRef,
		DNSServiceFlags flags,
		uint32_t interfaceIndex,
		DNSServiceErrorType errorCode,
		const char* fullname,
		const char* hosttarget,
		uint16_t port,
		uint16_t txtLen,
		const unsigned char* txtRecord,
		void* context);

	ZRef<Callable_t> fCallable;

	std::string fName;
	std::string fRegType;
	std::string fDomain;
	};

} // namespace DNS_SD
} // namespace ZooLib

#endif // __ZDNS_SD_h__
