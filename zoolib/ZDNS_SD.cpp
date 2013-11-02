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

#include "zoolib/ZDNS_SD.h"

#include "zoolib/ZLog.h"

#include <vector>

namespace ZooLib {
namespace DNS_SD {

using std::runtime_error;
using std::string;
using std::vector;

// =================================================================================================
// MARK: - Registration

void Registration::pInit(ip_port iPort,
	const char* iName, const string& iRegType,
	const char* iDomain,
	ConstPString* iTXT, size_t iTXTCount)
	{
	ZAcqMtx acq(fMutex);

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
		kDNSServiceFlagsDefault, // flags
		0, // default interface index
		iName, // name, nullptr for default
		iRegType.c_str(),
		iDomain, // domain, nullptr for default
		nullptr, // host, we presume the current host
		htons(iPort),
		theTXTData.size(),
		theTXTData.empty() ? nullptr : &theTXTData[0],
		spDNSServiceRegisterReply, // our callback
		this); // pointer to self as refcon

	if (result)
		{
		if (ZLOG(s, eNotice, "Registration"))
			s.Writef("DNSServiceRegister, failed with result: %d", result);
		throw runtime_error("Couldn't register name");
		}
	}

Registration::Registration(ip_port iPort,
	const string& iName, const string& iRegType,
	const string& iDomain,
	ConstPString* iTXT, size_t iTXTCount)
	{
	this->pInit(iPort,
		iName.empty() ? nullptr : iName.c_str(),
		iRegType.c_str(),
		iDomain.empty() ? nullptr : iDomain.c_str(),
		iTXT, iTXTCount);
	}

Registration::Registration(ip_port iPort,
	const string& iName, const string& iRegType,
	ConstPString* iTXT, size_t iTXTCount)
	{
	this->pInit(iPort,
		iName.empty() ? nullptr : iName.c_str(),
		iRegType.c_str(),
		nullptr,
		iTXT, iTXTCount);
	}

Registration::Registration(ip_port iPort,
	const string& iName, const string& iRegType)
	{
	this->pInit(iPort,
		iName.empty() ? nullptr : iName.c_str(),
		iRegType.c_str(),
		nullptr,
		nullptr, 0);
	}

Registration::Registration(ip_port iPort,
	const string& iRegType,
	ConstPString* iTXT, size_t iTXTCount)
	{
	this->pInit(iPort,
		nullptr,
		iRegType.c_str(),
		nullptr,
		iTXT, iTXTCount);
	}

Registration::Registration(ip_port iPort,
	const string& iRegType)
	{
	this->pInit(iPort,
		nullptr,
		iRegType.c_str(),
		nullptr,
		nullptr, 0);
	}

Registration::~Registration()
	{
	ZAcqMtx acq(fMutex);
	if (fDNSServiceRef)
		::DNSServiceRefDeallocate(fDNSServiceRef);
	}

std::string Registration::GetName() const
	{
	ZAcqMtx acq(fMutex);
	return fName;
	}

std::string Registration::GetRegType() const
	{
	ZAcqMtx acq(fMutex);
	return fRegType;
	}

std::string Registration::GetDomain() const
	{
	ZAcqMtx acq(fMutex);
	return fDomain;
	}

ip_port Registration::GetPort() const
	{
	ZAcqMtx acq(fMutex);
	return fPort;
	}

void Registration::pDNSServiceRegisterReply(
	DNSServiceFlags flags,
	DNSServiceErrorType errorCode,
	const char* name,
	const char* regtype,
	const char* domain)
	{
	ZAcqMtx acq(fMutex);

	if (ZLOG(s, eNotice, "Registration"))
		{
		s.Writef("pDNSServiceRegisterReply, flags: %d, errorCode: %d", flags, errorCode);
		s << ", name: " << name;
		s << ", regtype: " << regtype;
		s << ", domain: " << domain;
		}
	}

void Registration::spDNSServiceRegisterReply(
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

	static_cast<Registration*>(context)->
		pDNSServiceRegisterReply(flags, errorCode, name, regtype, domain);
	}

} // namespace DNS_SD
} // namespace ZooLib
