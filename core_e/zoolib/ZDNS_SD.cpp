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

#include "zoolib/Callable_PMF.h"
#include "zoolib/Singleton.h"
#include "zoolib/SocketWatcher.h"

#include "zoolib/ZDNS_SD.h"
#include "zoolib/ZNet_Socket.h"

#include "zoolib/ZLog.h"

#include <vector>

namespace ZooLib {
namespace DNS_SD {

using std::runtime_error;
using std::string;
using std::vector;

// =================================================================================================
// MARK: - DNSService

DNSService::DNSService()
:	fDNSServiceRef(nullptr)
	{}

void DNSService::Initialize()
	{
	ZCounted::Initialize();
	fCallable_SocketReadable = sCallable(sWeakRef(this), &DNSService::pSocketReadable);
	}

void DNSService::Finalize()
	{
	ZGuardMtxR guard(fMtxR);
	if (fDNSServiceRef)
		{
		sSingleton<SocketWatcher>().QErase(
			DNSServiceRefSockFD(fDNSServiceRef), fCallable_SocketReadable);
		::DNSServiceRefDeallocate(fDNSServiceRef);
		fDNSServiceRef = nullptr;
		}
	guard.Release();
	ZCounted::Finalize();
	}

DNSServiceRef DNSService::GetDNSServiceRef()
	{ return fDNSServiceRef; }

void DNSService::pSocketReadable()
	{
	ZGuardMtxR guard(fMtxR);
	if (fDNSServiceRef)
		{
		if (ZNet_Socket::sWaitReadable(DNSServiceRefSockFD(fDNSServiceRef), 0))
			{
			guard.Release();
			::DNSServiceProcessResult(fDNSServiceRef);
			}
		this->pWatchSocket();
		}
	}

void DNSService::pWatchSocket()
	{
	if (fDNSServiceRef)
		{
		sSingleton<SocketWatcher>().QInsert(
			DNSServiceRefSockFD(fDNSServiceRef), fCallable_SocketReadable);
		}
	}

// =================================================================================================
// MARK: - Registration

static vector<uint8> spFillVector(Registration::ConstPString* iTXT, size_t iTXTCount)
	{
	vector<unsigned char> result;
	while (iTXTCount--)
		{
		if (const size_t theSize = iTXT[0][0])
			result.insert(result.end(), iTXT[0], iTXT[0] + theSize + 1);
		++iTXT;
		}
	return result;
	}

Registration::Registration(const ZRef<Callable_t>& iCallable,
	ip_port iPort,
	const string& iName, const string& iRegType, const string& iDomain,
	ConstPString* iTXT, size_t iTXTCount)
:	fCallable(iCallable)
,	fPort(iPort)
,	fName(iName)
,	fRegType(iRegType)
,	fDomain(iDomain)
,	fTXT(spFillVector(iTXT, iTXTCount))
	{}

Registration::Registration(const ZRef<Callable_t>& iCallable,
	ip_port iPort,
	const string& iName, const string& iRegType,
	ConstPString* iTXT, size_t iTXTCount)
:	fCallable(iCallable)
,	fPort(iPort)
,	fName(iName)
,	fRegType(iRegType)
,	fTXT(spFillVector(iTXT, iTXTCount))
	{}

Registration::Registration(const ZRef<Callable_t>& iCallable,
	ip_port iPort,
	const string& iName, const string& iRegType)
:	fCallable(iCallable)
,	fPort(iPort)
,	fName(iName)
,	fRegType(iRegType)
	{}

Registration::Registration(const ZRef<Callable_t>& iCallable,
	ip_port iPort,
	const string& iRegType,
	ConstPString* iTXT, size_t iTXTCount)
:	fCallable(iCallable)
,	fPort(iPort)
,	fRegType(iRegType)
,	fTXT(spFillVector(iTXT, iTXTCount))
	{}

Registration::Registration(const ZRef<Callable_t>& iCallable,
	ip_port iPort,
	const string& iRegType)
:	fCallable(iCallable)
,	fPort(iPort)
,	fRegType(iRegType)
	{}

Registration::~Registration()
	{}

void Registration::Initialize()
	{
	DNSService::Initialize();
	ZAcqMtxR acq(fMtxR);

	DNSServiceErrorType result = ::DNSServiceRegister(
		&fDNSServiceRef, // output service ref
		kDNSServiceFlagsDefault, // flags
		0, // default interface index
		fName.empty() ? nullptr : fName.c_str(),
		fRegType.c_str(),
		fDomain.empty() ? nullptr : fDomain.c_str(),
		nullptr, // host, we presume the current host
		htons(fPort),
		fTXT.size(),
		fTXT.empty() ? nullptr : &fTXT[0],
		spCallback, // our callback
		this); // pointer to self as refcon

	if (result)
		{
		if (ZLOG(s, eNotice, "Registration"))
			s.Writef("DNSServiceRegister, failed with result: %d", result);
		throw runtime_error("Couldn't register name");
		}
	this->pWatchSocket();
	}

std::string Registration::GetName_Desired() const
	{
	ZAcqMtxR acq(fMtxR);
	return fName;
	}

ZQ<std::string> Registration::QGetName_Registered() const
	{
	ZAcqMtxR acq(fMtxR);
	return fName_RegisteredQ;
	}

std::string Registration::GetRegType() const
	{
	ZAcqMtxR acq(fMtxR);
	return fRegType;
	}

std::string Registration::GetDomain() const
	{
	ZAcqMtxR acq(fMtxR);
	return fDomain;
	}

ip_port Registration::GetPort() const
	{
	ZAcqMtxR acq(fMtxR);
	return fPort;
	}

void Registration::pCallback(
	DNSServiceFlags flags,
	DNSServiceErrorType errorCode,
	const char* name,
	const char* regtype,
	const char* domain)
	{
	ZAcqMtxR acq(fMtxR);

	fName_RegisteredQ = string(name);

	sCall(fCallable, this);
	}

void Registration::spCallback(
	DNSServiceRef sdRef,
	DNSServiceFlags flags,
	DNSServiceErrorType errorCode,
	const char* name,
	const char* regtype,
	const char* domain,
	void* context)
	{
	if (ZRef<Registration> theOb = static_cast<Registration*>(context))
		theOb->pCallback(flags, errorCode, name, regtype, domain);
	}

// =================================================================================================
// MARK: - Browse

Browse::Browse(const ZRef<Callable_t>& iCallable, const std::string& iRegType)
:	fCallable(iCallable)
,	fRegType(iRegType)
	{}

void Browse::Initialize()
	{
	DNSService::Initialize();

	ZAcqMtxR acq(fMtxR);

	DNSServiceErrorType result = ::DNSServiceBrowse(
		&fDNSServiceRef, // output service ref
		kDNSServiceFlagsDefault, // flags
		0, // default interface index
		fRegType.c_str(),
		fDomain.empty() ? nullptr : fDomain.c_str(),
		spCallback, // our callback
		this); // pointer to self as refcon

	if (result)
		{
		if (ZLOG(s, eNotice, "Browse"))
			s.Writef("DNSServiceBrowse, failed with result: %d", result);
		throw runtime_error("Couldn't browse");
		}

	this->pWatchSocket();
	}

void Browse::spCallback(
	DNSServiceRef sdRef,
	DNSServiceFlags flags,
	uint32_t interfaceIndex,
	DNSServiceErrorType errorCode,
	const char* serviceName,
	const char* regtype,
	const char* replyDomain,
	void* context)
	{
	if (ZRef<Browse> theOb = static_cast<Browse*>(context))
		{
		if (ZRef<Callable_t> theCallable = theOb->fCallable)
			{
			theCallable->Call(flags & kDNSServiceFlagsMoreComing, flags & kDNSServiceFlagsAdd,
				serviceName, regtype, replyDomain);
			}
		}
	}

// =================================================================================================
// MARK: - Resolve

Resolve::Resolve(const ZRef<Callable_t>& iCallable,
	const std::string& iName,
	const std::string& iRegType,
	const std::string& iDomain)
:	fCallable(iCallable)
,	fName(iName)
,	fRegType(iRegType)
,	fDomain(iDomain)
	{}

void Resolve::Initialize()
	{
	DNSService::Initialize();

	ZAcqMtxR acq(fMtxR);

	DNSServiceErrorType result = ::DNSServiceResolve(
		&fDNSServiceRef, // output service ref
		0, // flags
		0, // default interface index
		fName.c_str(),
		fRegType.c_str(),
		fDomain.c_str(),
		spCallback, // our callback
		this); // pointer to self as refcon

	if (result)
		{
		if (ZLOG(s, eNotice, "Resolve"))
			s.Writef("DNSServiceResolve, failed with result: %d", result);
		throw runtime_error("Couldn't resolve");
		}

	this->pWatchSocket();
	}

void Resolve::spCallback(
	DNSServiceRef sdRef,
	DNSServiceFlags flags,
	uint32_t interfaceIndex,
	DNSServiceErrorType errorCode,
	const char* fullname,
	const char* hosttarget,
	uint16_t port,
	uint16_t txtLen,
	const unsigned char* txtRecord,
	void* context)
	{
	if (ZRef<Resolve> theOb = static_cast<Resolve*>(context))
		{
		if (ZRef<Callable_t> theCallable = theOb->fCallable)
			{
			theCallable->Call(flags & kDNSServiceFlagsMoreComing,
				fullname, hosttarget, ntohs(port), txtLen, txtRecord);
			}
		}
	}

} // namespace DNS_SD
} // namespace ZooLib
