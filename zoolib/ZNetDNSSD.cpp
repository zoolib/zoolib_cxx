/* ------------------------------------------------------------
Copyright (c) 2007 Andrew Green
http://www.zoolib.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
COPYRIGHT HOLDER(S) BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
------------------------------------------------------------ */

#include "ZNetDNSSD.h"

#if ZCONFIG_SPI_Enabled(DNSSD)

#include "ZLog.h"

// =================================================================================================
#pragma mark -
#pragma mark * ZNetNameRegistered_DNSSD

void ZNetNameRegistered_DNSSD::pInit(ip_port iPort,
	const char* iName, const string& iRegType,
	const char* iDomain,
	ConstPString* iTXT, size_t iTXTCount)
	{
	ZMutexLocker locker(fMutex);

	vector<uint8> theTXTData;
	if (iTXT && iTXTCount)
		{
		while (iTXTCount--)
			{
			if (const size_t theSize = iTXT[0][0])
				theTXTData.insert(theTXTData.end(), iTXT[0], iTXT[0] + theSize + 1);
			++iTXT;
			}
		}
	
	DNSServiceErrorType result = ::DNSServiceRegister(
		&fDNSServiceRef, // output service ref
		0, // flags
		0, // default interface index
		iName, // name, nil for default
		iRegType.c_str(),
		iDomain, // domain, nil for default
		nil, // host, we presume the current host
		iPort,
		theTXTData.size(),
		theTXTData.empty() ? nil : &theTXTData[0],
		spDNSServiceRegisterReply, // our callback
		this); // pointer to self as refcon

	if (result)
		{
		if (const ZLog::S& s = ZLog::S(ZLog::ePriority_Notice, "ZNetNameRegistered_DNSSD"))
			{
			s.Writef("DNSServiceRegister, failed with result: %d", result);
			throw runtime_error("Couldn't register name");
			}		
		}
	}

ZNetNameRegistered_DNSSD::ZNetNameRegistered_DNSSD(ip_port iPort,
	const string& iName, const string& iRegType,
	const string& iDomain,
	ConstPString* iTXT, size_t iTXTCount)
	{
	this->pInit(iPort,
		iName.empty() ? nil : iName.c_str(),
		iRegType.c_str(),
		iDomain.empty() ? nil : iDomain.c_str(),
		iTXT, iTXTCount);
	}

ZNetNameRegistered_DNSSD::ZNetNameRegistered_DNSSD(ip_port iPort,
	const string& iName, const string& iRegType,
	ConstPString* iTXT, size_t iTXTCount)
	{
	this->pInit(iPort,
		iName.empty() ? nil : iName.c_str(),
		iRegType.c_str(),
		nil,
		iTXT, iTXTCount);
	}

ZNetNameRegistered_DNSSD::ZNetNameRegistered_DNSSD(ip_port iPort,
	const string& iName, const string& iRegType)
	{
	this->pInit(iPort,
		iName.empty() ? nil : iName.c_str(),
		iRegType.c_str(),
		nil,
		nil, 0);
	}

ZNetNameRegistered_DNSSD::ZNetNameRegistered_DNSSD(ip_port iPort,
	const string& iRegType,
	ConstPString* iTXT, size_t iTXTCount)
	{
	this->pInit(iPort,
		nil,
		iRegType.c_str(),
		nil,
		iTXT, iTXTCount);
	}

ZNetNameRegistered_DNSSD::ZNetNameRegistered_DNSSD(ip_port iPort,
	const string& iRegType)
	{
	this->pInit(iPort,
		nil,
		iRegType.c_str(),
		nil,
		nil, 0);
	}

ZNetNameRegistered_DNSSD::~ZNetNameRegistered_DNSSD()
	{
	ZMutexLocker locker(fMutex);
	if (fDNSServiceRef)
		::DNSServiceRefDeallocate(fDNSServiceRef);
	}

std::string ZNetNameRegistered_DNSSD::GetName() const
	{
	ZMutexLocker locker(fMutex);
	return fName;
	}

std::string ZNetNameRegistered_DNSSD::GetRegType() const
	{
	ZMutexLocker locker(fMutex);
	return fRegType;
	}

std::string ZNetNameRegistered_DNSSD::GetDomain() const
	{
	ZMutexLocker locker(fMutex);
	return fDomain;
	}

ip_port ZNetNameRegistered_DNSSD::GetPort() const
	{
	ZMutexLocker locker(fMutex);
	return fPort;
	}

void ZNetNameRegistered_DNSSD::pDNSServiceRegisterReply(
	DNSServiceFlags flags,
	DNSServiceErrorType errorCode,
	const char* name,
	const char* regtype,
	const char* domain)
	{
	ZMutexLocker locker(fMutex);

	if (const ZLog::S& s = ZLog::S(ZLog::ePriority_Notice, "ZNetNameRegistered_DNSSD"))
		{
		s.Writef("pDNSServiceRegisterReply, flags: %d, errorCode: %d", flags, errorCode);
		s << ", name: " << name;
		s << ", regtype: " << regtype;
		s << ", domain: " << domain;
		}
	}

void ZNetNameRegistered_DNSSD::spDNSServiceRegisterReply(
	DNSServiceRef sdRef,
	DNSServiceFlags flags,
	DNSServiceErrorType errorCode,
	const char* name,
	const char* regtype,
	const char* domain,
	void* context)
	{
	if (!context)
		return;

	ZNetNameRegistered_DNSSD* theNNR = static_cast<ZNetNameRegistered_DNSSD*>(context);
	ZMutexLocker locker(theNNR->fMutex);

	ZAssert(sdRef == theNNR->fDNSServiceRef);
	
	theNNR->pDNSServiceRegisterReply(flags, errorCode, name, regtype, domain);
	}

#endif // ZCONFIG_SPI_Enabled(DNSSD)
