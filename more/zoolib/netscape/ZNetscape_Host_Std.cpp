/* -------------------------------------------------------------------------------------------------
Copyright (c) 2002 Andrew Green
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

#include "zoolib/netscape/ZNetscape_Host_Std.h"

#include "zoolib/ZCallOnNewThread.h"
#include "zoolib/ZCompat_string.h" // For strdup
#include "zoolib/ZDebug.h"
#include "zoolib/ZHTTP_Requests.h"
#include "zoolib/ZLog.h"
#include "zoolib/ZNet_Internet.h"
#include "zoolib/ZStream_Data_T.h"
#include "zoolib/ZStringf.h"
#include "zoolib/ZUtil_STL_vector.h"
#include "zoolib/ZWorker.h"

#include <stdlib.h> // For malloc/free

#include <vector>

#if ZCONFIG(Compiler, MSVC)
	#define strdup _strdup
#endif

using std::set;
using std::string;
using std::vector;

namespace ZooLib {
namespace ZNetscape {

// =================================================================================================
// MARK: - HostMeister_Std

HostMeister_Std::HostMeister_Std()
	{}

HostMeister_Std::~HostMeister_Std()
	{}

Host_Std* HostMeister_Std::sHostFromNPP_Std(NPP npp)
	{ return static_cast<Host_Std*>(sHostFromNPP(npp)); }

Host_Std* HostMeister_Std::sHostFromStream_Std(NPStream* iNPStream)
	{ return static_cast<Host_Std*>(sHostFromStream(iNPStream)); }

NPError HostMeister_Std::GetURL(NPP npp, const char* URL, const char* window)
	{
	ZLOGFUNCTION(eDebug);

	if (Host_Std* theHost = sHostFromNPP_Std(npp))
		return theHost->Host_GetURL(npp, URL, window);

	return NPERR_GENERIC_ERROR;
	}

NPError HostMeister_Std::PostURL(NPP npp,
	const char* URL, const char* window, uint32 len, const char* buf, NPBool file)
	{
	ZLOGFUNCTION(eDebug);

	if (Host_Std* theHost = sHostFromNPP_Std(npp))
		return theHost->Host_PostURL(npp, URL, window, len, buf, file);

	return NPERR_GENERIC_ERROR;
	}

NPError HostMeister_Std::RequestRead(NPStream* stream, NPByteRange* rangeList)
	{
	ZLOGFUNCTION(eDebug);

	if (Host_Std* theHost = sHostFromStream_Std(stream))
		return theHost->Host_RequestRead(stream, rangeList);

	return NPERR_GENERIC_ERROR;
	}

NPError HostMeister_Std::NewStream(NPP npp,
	NPMIMEType type, const char* window, NPStream** stream)
	{
	ZLOGFUNCTION(eDebug);

	if (Host_Std* theHost = sHostFromNPP_Std(npp))
		return theHost->Host_NewStream(npp, type, window, stream);

	return NPERR_GENERIC_ERROR;
	}

int32 HostMeister_Std::Write(NPP npp, NPStream* stream, int32 len, void* buffer)
	{
	ZLOGFUNCTION(eDebug);

	if (Host_Std* theHost = sHostFromNPP_Std(npp))
		return theHost->Host_Write(npp, stream, len, buffer);

	return -1;
	}

NPError HostMeister_Std::DestroyStream(NPP npp, NPStream* stream, NPReason reason)
	{
	ZLOGFUNCTION(eDebug);

	if (Host_Std* theHost = sHostFromNPP_Std(npp))
		return theHost->Host_DestroyStream(npp, stream, reason);

	return NPERR_GENERIC_ERROR;
	}

void HostMeister_Std::Status(NPP npp, const char* message)
	{
	ZLOGFUNCTION(eDebug);

	if (Host_Std* theHost = sHostFromNPP_Std(npp))
		theHost->Host_Status(npp, message);
	}

const char* HostMeister_Std::UserAgent(NPP npp)
	{
	ZLOGFUNCTION(eDebug);

	#if defined(XP_WIN)
		// From WebKit, PluginViewWin.cpp.
		// Flash pre 10.0 will not request windowless mode unless a Mozilla-ish user agent
		// is returned. By inspection this seems to be the minimal UA that will work.
		return "Mozilla/5.0 (rv:1.8.1) Gecko/20061010";
	#elif ZCONFIG(Processor, PPC)
		return "Mozilla/5.0 (Macintosh; U; PPC Mac OS X)";
	#else
		// per Jethro Villegas at Adobe, this makes Flash 11 happy.
		return "Mozilla/5.0 (Macintosh; U; Intel Mac OS X)";
	#endif
	}

void* HostMeister_Std::MemAlloc(uint32 size)
	{ return ::malloc(size); }

void HostMeister_Std::MemFree(void* ptr)
	{ ::free(ptr); }

uint32 HostMeister_Std::MemFlush(uint32 size)
	{ return 0; }

void HostMeister_Std::ReloadPlugins(NPBool reloadPages)
	{}

void* HostMeister_Std::GetJavaEnv()
	{ return nullptr; }

void* HostMeister_Std::GetJavaPeer(NPP npp)
	{
	ZLOGFUNCTION(eDebug);

	if (Host_Std* theHost = sHostFromNPP_Std(npp))
		return theHost->Host_GetJavaPeer(npp);

	return nullptr;
	}

NPError HostMeister_Std::GetURLNotify(NPP npp,
	const char* URL, const char* window, void* notifyData)
	{
	if (ZLOGF(s, eDebug))
		{
		s	<< URL << ", notifyData: " << notifyData
			<< ", iRelativeURL: " << URL;
		if (window)
			s << ", target: " << window;
		}

	if (Host_Std* theHost = sHostFromNPP_Std(npp))
		return theHost->Host_GetURLNotify(npp, URL, window, notifyData);

	return NPERR_GENERIC_ERROR;
	}

NPError HostMeister_Std::PostURLNotify(NPP npp,
	const char* URL, const char* window,
	uint32 len, const char* buf, NPBool file, void* notifyData)
	{
	if (ZLOGF(s, eDebug))
		{
		s << URL << ", notifyData: " << notifyData;
		if (window)
			s << ", target: " << window;
		}

	if (Host_Std* theHost = sHostFromNPP_Std(npp))
		return theHost->Host_PostURLNotify(npp, URL, window, len, buf, file, notifyData);

	return NPERR_GENERIC_ERROR;
	}

NPError HostMeister_Std::GetValue(NPP npp, NPNVariable variable, void* ret_value)
	{
	if (ZLOGF(s, eDebug))
		s << ZNetscape::sAsString(variable);

	if (Host_Std* theHost = sHostFromNPP_Std(npp))
		return theHost->Host_GetValue(npp, variable, ret_value);

	return NPERR_GENERIC_ERROR;
	}

NPError HostMeister_Std::SetValue(NPP npp, NPPVariable variable, void* value)
	{
	if (ZLOGF(s, eDebug))
		s << ZNetscape::sAsString(variable);

	if (Host_Std* theHost = sHostFromNPP_Std(npp))
		return theHost->Host_SetValue(npp, variable, value);

	return NPERR_GENERIC_ERROR;
	}

void HostMeister_Std::InvalidateRect(NPP npp, NPRect* rect)
	{
	if (ZLOGF(s, eDebug + 1))
		{
		s	<< "(" << rect->left << ", " << rect->top
			<< ", " << rect->right << ", " << rect->bottom << ")";
		}

	if (Host_Std* theHost = sHostFromNPP_Std(npp))
		theHost->Host_InvalidateRect(npp, rect);
	}

void HostMeister_Std::InvalidateRegion(NPP npp, NPRegion region)
	{
	ZLOGFUNCTION(eDebug);

	if (Host_Std* theHost = sHostFromNPP_Std(npp))
		theHost->Host_InvalidateRegion(npp, region);
	}

void HostMeister_Std::ForceRedraw(NPP npp)
	{
	ZLOGFUNCTION(eDebug);

	if (Host_Std* theHost = sHostFromNPP_Std(npp))
		return theHost->Host_ForceRedraw(npp);
	}

NPIdentifier HostMeister_Std::GetStringIdentifier(const NPUTF8* name)
	{
	const string theName = name;
	set<string>::iterator iter = fStrings.lower_bound(theName);
	if (iter == fStrings.end() || *iter != theName)
		iter = fStrings.insert(iter, theName);
	return static_cast<NPIdentifier>(const_cast<string*>(&*iter));
	}

void HostMeister_Std::GetStringIdentifiers(
	const NPUTF8* *names, int32 nameCount, NPIdentifier* identifiers)
	{
	while (--nameCount)
		*identifiers++ = this->GetStringIdentifier(*names++);
	}

NPIdentifier HostMeister_Std::GetIntIdentifier(int32 intid)
	{ return reinterpret_cast<NPIdentifier>((intptr_t(intid) << 1) | 0x1); }

bool HostMeister_Std::IdentifierIsString(NPIdentifier identifier)
	{ return 0 == (reinterpret_cast<intptr_t>(identifier) & 0x1); }

NPUTF8* HostMeister_Std::UTF8FromIdentifier(NPIdentifier identifier)
	{
	if (0 == (reinterpret_cast<intptr_t>(identifier) & 0x1))
		return (NPUTF8*)strdup(static_cast<string*>(identifier)->c_str());
	return nullptr;
	}

int32 HostMeister_Std::IntFromIdentifier(NPIdentifier identifier)
	{
	const int32 asInt = reinterpret_cast<intptr_t>(identifier);
	if (asInt & 0x1)
		return asInt >> 1;
	return 0;
	}

NPObject* HostMeister_Std::CreateObject(NPP npp, NPClass* aClass)
	{
	ZLOGFUNCTION(eDebug);

	ZAssert(aClass);
	NPObject* result;
	if (aClass->allocate)
		result = aClass->allocate(npp, aClass);
	else
		result = (NPObject*)malloc(sizeof(NPObject));

	result->_class = aClass;
	result->referenceCount = 1;

	return result;
	}

NPObject* HostMeister_Std::RetainObject(NPObject* obj)
	{
	ZAssert(obj);
	++obj->referenceCount;
	return obj;
	}

void HostMeister_Std::ReleaseObject(NPObject* obj)
	{
	ZAssert(obj && obj->referenceCount > 0);
	if (--obj->referenceCount == 0)
		{
		NPClass_Z* theClass = sGetClass(obj);
		if (theClass && theClass->deallocate)
			theClass->deallocate(obj);
		else
			free(obj);
		}
	}

bool HostMeister_Std::Invoke(NPP npp,
	NPObject* obj, NPIdentifier methodName, const NPVariant* args, uint32 argCount,
	NPVariant* result)
	{
	if (ZLOGF(s, eDebug))
		s << this->StringFromIdentifier(methodName);

	if (NPClass_Z* theClass = sGetClass(obj))
		{
		if (theClass->invoke)
			return theClass->invoke(obj, methodName, args, argCount, result);
		}

	return false;
	}

bool HostMeister_Std::InvokeDefault(NPP npp,
	NPObject* obj, const NPVariant* args, uint32 argCount, NPVariant* result)
	{
	ZLOGFUNCTION(eDebug);

	if (NPClass_Z* theClass = sGetClass(obj))
		{
		if (theClass->invokeDefault)
			return theClass->invokeDefault(obj, args, argCount, result);
		}

	return false;
	}

bool HostMeister_Std::Evaluate(NPP npp,
	NPObject* obj, NPString* script, NPVariant* result)
	{
	if (ZLOGF(s, eDebug))
		s << sAsString(*script);

	if (Host_Std* theHost = sHostFromNPP_Std(npp))
		return theHost->Host_Evaluate(npp, obj, script, result);

	return false;
	}

bool HostMeister_Std::GetProperty(NPP npp,
	NPObject* obj, NPIdentifier propertyName, NPVariant* result)
	{
	if (ZLOGF(s, eDebug))
		s << this->StringFromIdentifier(propertyName);

	if (NPClass_Z* theClass = sGetClass(obj))
		{
		if (theClass->getProperty)
			return theClass->getProperty(obj, propertyName, result);
		}

	return false;
	}

bool HostMeister_Std::SetProperty(NPP npp,
	NPObject* obj, NPIdentifier propertyName, const NPVariant* value)
	{
	ZLOGFUNCTION(eDebug);

	if (NPClass_Z* theClass = sGetClass(obj))
		{
		if (theClass->getProperty)
			return theClass->setProperty(obj, propertyName, value);
		}

	return false;
	}

bool HostMeister_Std::RemoveProperty(NPP npp, NPObject* obj, NPIdentifier propertyName)
	{
	ZLOGFUNCTION(eDebug);

	if (NPClass_Z* theClass = sGetClass(obj))
		{
		if (theClass->removeProperty)
			return theClass->removeProperty(obj, propertyName);
		}

	return false;
	}

bool HostMeister_Std::HasProperty(NPP, NPObject* obj, NPIdentifier propertyName)
	{
	ZLOGFUNCTION(eDebug);

	if (NPClass_Z* theClass = sGetClass(obj))
		{
		if (theClass->hasProperty)
			return theClass->hasProperty(obj, propertyName);
		}

	return false;
	}

bool HostMeister_Std::HasMethod(NPP npp, NPObject* obj, NPIdentifier methodName)
	{
	ZLOGFUNCTION(eDebug);

	if (NPClass_Z* theClass = sGetClass(obj))
		{
		if (theClass->hasMethod)
			return theClass->hasMethod(obj, methodName);
		}

	return false;
	}

void HostMeister_Std::ReleaseVariantValue(NPVariant* variant)
	{
	ZAssert(variant);

	if (variant->type == NPVariantType_Object)
		{
		this->ReleaseObject(variant->value.objectValue);
		variant->value.objectValue = nullptr;
		}
	else if (variant->type == NPVariantType_String)
		{
		free((void*)sNPStringChars(variant->value.stringValue));
		sNPStringChars(variant->value.stringValue) = nullptr;
		sNPStringLength(variant->value.stringValue) = 0;
		}
	variant->type = NPVariantType_Void;
	}

void HostMeister_Std::SetException(NPObject* obj, const NPUTF8* message)
	{
	ZLOGFUNCTION(eDebug);
	ZUnimplemented();
	}

void HostMeister_Std::PushPopupsEnabledState(NPP npp, NPBool enabled)
	{
	ZLOGFUNCTION(eDebug);
	}

void HostMeister_Std::PopPopupsEnabledState(NPP npp)
	{
	ZLOGFUNCTION(eDebug);
	}

bool HostMeister_Std::Enumerate(
	NPP, NPObject* obj, NPIdentifier **identifier, uint32 *count)
	{
	ZLOGFUNCTION(eDebug);

	if (NPClass_Z* theClass = sGetClass(obj))
		{
		if (theClass->enumerate)
			return theClass->enumerate(obj, identifier, (uint32_t*)count);
		}

	return false;
	}

void HostMeister_Std::PluginThreadAsyncCall(
	NPP npp, void (*func)(void *), void *userData)
	{
	ZLOGFUNCTION(eDebug);
	}

bool HostMeister_Std::Construct(
	NPP npp, NPObject* obj, const NPVariant *args, uint32 argCount, NPVariant *result)
	{
	ZLOGFUNCTION(eDebug);
	return false;
	}

uint32 HostMeister_Std::ScheduleTimer(
	NPP npp, uint32 interval, NPBool repeat, void (*timerFunc)(NPP npp, uint32 timerID))
	{
	if (Host_Std* theHost = sHostFromNPP_Std(npp))
		return theHost->Host_ScheduleTimer(npp, interval, repeat, timerFunc);
	return 0;
	}

void HostMeister_Std::UnscheduleTimer(NPP npp, uint32 timerID)
	{
	if (Host_Std* theHost = sHostFromNPP_Std(npp))
		return theHost->Host_UnscheduleTimer(npp, timerID);
	}

// =================================================================================================
// MARK: - Host_Std::HTTPFetcher

class Host_Std::HTTPFetcher
:	public ZCallable_Void
	{
public:
	HTTPFetcher(Host_Std* iHost, const string& iURL, ZHTTP::Data* iData, void* iNotifyData);

// From ZCallable_Void
	virtual ZQ<void> QCall();

	void Cancel();

private:
	Host_Std* fHost;
	const string fURL;
	ZHTTP::Data fData;
	void* fNotifyData;
	bool fIsPOST;
	};

Host_Std::HTTPFetcher::HTTPFetcher(
	Host_Std* iHost, const string& iURL, ZHTTP::Data* iData, void* iNotifyData)
:	fHost(iHost),
	fURL(iURL),
	fNotifyData(iNotifyData)
	{
	if (iData)
		{
		fData = *iData;
		fIsPOST = true;
		}
	else
		{
		fIsPOST = false;
		}
	}

ZQ<void> Host_Std::HTTPFetcher::QCall()
	{
	try
		{
		string theURL = fURL;
		ZHTTP::Map theHeaders;
		ZHTTP::Data theRawHeaders;
		ZRef<ZStreamerR> theStreamerR;
		if (fIsPOST)
			{
			theStreamerR = ZHTTP::sPostRaw(null,
				theURL, ZStreamRPos_Data_T<ZHTTP::Data>(fData), nullptr,
				&theHeaders, &theRawHeaders);
			}
		else
			{
			theStreamerR = ZHTTP::sRequest(null,
				"GET", theURL, nullptr, &theHeaders, &theRawHeaders);
			}

		if (theStreamerR && fHost)
			{
			const ZHTTP::Map theCT = theHeaders.Get<ZHTTP::Map>("content-type");
			const string theMIME = theCT.Get<string>("type") + "/" + theCT.Get<string>("subtype");

			fHost->pHTTPFetcher(
				this, fNotifyData, theURL, theMIME, theRawHeaders, theStreamerR);
			return notnull;
			}
		}
	catch (...)
		{}

	// This causes async delivery of an error.
	if (fHost)
		{
		fHost->pHTTPFetcher(
			this, fNotifyData, fURL, "", ZHTTP::Data(), null);
		}
	return notnull;
	}

void Host_Std::HTTPFetcher::Cancel()
	{
	ZLOGFUNCTION(eDebug);

	fHost = nullptr;
	}

// =================================================================================================
// MARK: - Host_Std::Sender

class Host_Std::Sender
	{
public:
	Sender(Host* iHost, const NPP_t& iNPP_t,
		void* iNotifyData,
		const std::string& iURL, const std::string& iMIME, const ZHTTP::Data& iHeaders,
		ZRef<ZStreamerR> iStreamerR);
	~Sender();

	bool DeliverData();

private:
	bool pDeliverData();

	bool fSentNew;
	Host* fHost;
	void* fNotifyData;
	const string fURL;
	const string fMIME;
	const ZHTTP::Data fHeaders;
	NPStream_Z fNPStream;
	ZRef<ZStreamerR> fStreamerR;
	};

Host_Std::Sender::Sender(Host* iHost, const NPP_t& iNPP_t,
	void* iNotifyData,
	const std::string& iURL, const std::string& iMIME, const ZHTTP::Data& iHeaders,
	ZRef<ZStreamerR> iStreamerR)
:	fSentNew(false),
	fHost(iHost),
	fNotifyData(iNotifyData),
	fURL(iURL),
	fMIME(iMIME),
	fHeaders(iHeaders),
	fStreamerR(iStreamerR)
	{
	fNPStream.ndata = iNPP_t.ndata;
	fNPStream.pdata = iNPP_t.pdata;
	fNPStream.url = fURL.c_str();
	fNPStream.end = 0;
	fNPStream.lastmodified = 0;
	fNPStream.notifyData = iNotifyData;
	fNPStream.headers = nullptr;
//	fNPStream.headers = static_cast<const char*>(fHeaders.GetPtr());
	}

Host_Std::Sender::~Sender()
	{}

bool Host_Std::Sender::DeliverData()
	{
	if (!fSentNew)
		{
		fSentNew = true;

		if (!fStreamerR)
			{
			fHost->Guest_URLNotify(fURL.c_str(), NPRES_NETWORK_ERR, fNotifyData);
			return false;
			}

		uint16 theStreamType = NP_NORMAL;
		if (fHost->Guest_NewStream(
			const_cast<char*>(fMIME.c_str()), &fNPStream, false, &theStreamType))
			{
			// Failed -- what result should we pass?
			fHost->Guest_URLNotify(fURL.c_str(), NPRES_NETWORK_ERR, fNotifyData);
			return false;
			}
		}

	if (this->pDeliverData())
		return true;

	if (ZLOGF(s, eDebug + 1))
		s << "Calling Guest_URLNotify on " << fURL;

	fHost->Guest_URLNotify(fURL.c_str(), NPRES_DONE, fNotifyData);

	if (ZLOGF(s, eDebug + 1))
		s << "Calling Guest_DestroyStream";

	fHost->Guest_DestroyStream(&fNPStream, NPRES_DONE);

	return false;
	}

bool Host_Std::Sender::pDeliverData()
	{
	const ZStreamR& theStreamR = fStreamerR->GetStreamR();

	if (!theStreamR.WaitReadable(0))
		{
		if (ZLOGF(s, eDebug + 1))
			s << "waitReadable is false";
		return true;
		}

	const size_t countReadable = theStreamR.CountReadable();

	if (ZLOGF(s, eDebug + 1))
		s << "countReadable=" << countReadable;

	if (countReadable == 0)
		return false;

	int32 countPossible = fHost->Guest_WriteReady(&fNPStream);

	if (ZLOGF(s, eDebug + 1))
		s << "countPossible=" << countPossible;

	if (countPossible < 0)
		{
		return false;
		}
	else if (countPossible > 0)
		{
		countPossible = std::min(countPossible, int32(64 * 1024));

		vector<uint8> buffer;
		buffer.resize(countPossible);
		size_t countRead;
		theStreamR.Read(&buffer[0], countPossible, &countRead);
		if (countRead == 0)
			return false;

		if (ZLOGF(s, eDebug + 1))
			s << "countRead=" << countRead;

		for (size_t start = 0; start < countRead; /*no inc*/)
			{
			int countWritten = fHost->Guest_Write(&fNPStream, 0, countRead - start, &buffer[start]);

			if (ZLOGF(s, eDebug + 1))
				s << "countWritten=" << countWritten << " to " << fURL;

			if (countWritten < 0)
				return false;

			start += countWritten;
			}
		}

	return true;
	}

// =================================================================================================
// MARK: - Host_Std

Host_Std::Host_Std(ZRef<GuestFactory> iGuestFactory)
:	Host(iGuestFactory)
	{}

Host_Std::~Host_Std()
	{
	ZLOGFUNCTION(eDebug);

	for (ZSafeSetIterConst<ZRef<HTTPFetcher> > i = fHTTPFetchers;;)
		{
		if (ZRef<HTTPFetcher> current = i.ReadInc())
			current->Cancel();
		else
			break;
		}
	fHTTPFetchers.Clear();

	for (ZSafeSetIterConst<Sender* > i = fSenders;;)
		{
		if (Sender* current = i.ReadInc())
			delete current;
		else
			break;
		}
	fSenders.Clear();

	this->Destroy();
	}

NPError Host_Std::Host_GetURL(NPP npp, const char* URL, const char* window)
	{
	return NPERR_INVALID_URL;
	}

NPError Host_Std::Host_PostURL(NPP npp,
	const char* URL, const char* window, uint32 len, const char* buf, NPBool file)
	{
	return NPERR_INVALID_URL;
	}

NPError Host_Std::Host_RequestRead(NPStream* stream, NPByteRange* rangeList)
	{
	return NPERR_GENERIC_ERROR;
	}

NPError Host_Std::Host_NewStream(NPP npp,
	NPMIMEType type, const char* window, NPStream** stream)
	{
	return NPERR_GENERIC_ERROR;
	}

int32 Host_Std::Host_Write(NPP npp, NPStream* stream, int32 len, void* buffer)
	{
	return -1;
	}

NPError Host_Std::Host_DestroyStream(NPP npp, NPStream* stream, NPReason reason)
	{
	return NPERR_GENERIC_ERROR;
	}

void Host_Std::Host_Status(NPP npp, const char* message)
	{}

void* Host_Std::Host_GetJavaPeer(NPP npp)
	{
	return nullptr;
	}

static string spFixURL(const string& iBaseURL, const string& iRelativeURL)
	{
	string relScheme;
	string relHost;
	ip_port relPort;
	string relPath;
	if (not ZHTTP::sParseURL(iRelativeURL, &relScheme, &relHost, &relPort, &relPath))
		return string();

	if (!relScheme.empty())
		{
		return iRelativeURL;
		}
	else
		{
		string baseScheme;
		string baseHost;
		ip_port basePort = relPort;
		string basePath;
		if (not ZHTTP::sParseURL(iBaseURL, &baseScheme, &baseHost, &basePort, &basePath))
			return string();

		if (!basePort)
			basePort = 80;

		const string resultURL = baseScheme + "://" + baseHost + sStringf(":%d", basePort);
		if (relPath.substr(0, 1) == "/")
			{
			return resultURL + relPath;
			}
		else
			{
			const ZTrail baseDir = ZTrail(basePath).Branch();
			const ZTrail resultTrail = (baseDir + ZTrail(relPath)).Normalized();
			if (resultTrail.Count() && sIsEmpty(resultTrail.At(0)))
				{
				// We've got leading bounces.
				return string();
				}
			return resultURL + "/" + resultTrail.AsString();
			}
		}
	}

NPError Host_Std::Host_GetURLNotify(NPP npp,
	const char* URL, const char* window, void* notifyData)
	{
	ZLOGFUNCTION(eDebug);
	const string theURL = spFixURL(fURL, URL);
	if (theURL.empty())
		return NPERR_INVALID_URL;

	if (theURL.substr(0, 5) == "http:")
		{
		ZRef<HTTPFetcher> theFetcher = new HTTPFetcher(this, theURL, nullptr, notifyData);
		fHTTPFetchers.Insert(theFetcher);
		sCallOnNewThread(theFetcher);
		return NPERR_NO_ERROR;
		}

	if (ZLOGF(s, eDebug))
		s << "invalid URL";

	return NPERR_INVALID_URL;
	}

NPError Host_Std::Host_PostURLNotify(NPP npp,
	const char* URL, const char* window,
	uint32 len, const char* buf, NPBool file, void* notifyData)
	{
	const string theURL = spFixURL(fURL, URL);
	if (theURL.empty())
		return NPERR_INVALID_URL;

	if (theURL.substr(0, 5) == "http:")
		{
		ZHTTP::Data theData(buf, len);
		ZRef<HTTPFetcher> theFetcher = new HTTPFetcher(this, theURL, &theData, notifyData);
		fHTTPFetchers.Insert(theFetcher);
		sCallOnNewThread(theFetcher);
		return NPERR_NO_ERROR;
		}

	return NPERR_INVALID_URL;
	}

NPError Host_Std::Host_GetValue(NPP npp, NPNVariable variable, void* ret_value)
	{
	switch (variable)
		{
		case NPNVWindowNPObject:
			{
			if (ZRef<NPObjectH> theObject = this->Host_GetWindowObject())
				{
				*static_cast<ZRef<NPObjectH>*>(ret_value) = theObject;
				return NPERR_NO_ERROR;
				}
			break;
			}
		case NPNVPluginElementNPObject:
			{
			if (ZRef<NPObjectH> theObject = this->Host_GetPluginObject())
				{
				*static_cast<ZRef<NPObjectH>*>(ret_value) = theObject;
				return NPERR_NO_ERROR;
				}
			break;
			}
		default:
			break;
		}
	return NPERR_GENERIC_ERROR;
	}

NPError Host_Std::Host_SetValue(NPP npp, NPPVariable variable, void* value)
	{
	return NPERR_GENERIC_ERROR;
	}

void Host_Std::Host_InvalidateRect(NPP npp, NPRect* rect)
	{}

void Host_Std::Host_InvalidateRegion(NPP npp, NPRegion region)
	{}

void Host_Std::Host_ForceRedraw(NPP npp)
	{}

bool Host_Std::Host_Evaluate(NPP npp,
	NPObject* obj, NPString* script, NPVariant* result)
	{ return false; }

uint32 Host_Std::Host_ScheduleTimer(
	NPP npp, uint32 interval, NPBool repeat, void (*timerFunc)(NPP npp, uint32 timerID))
	{
	ZLOGFUNCTION(eDebug);
	return 0;
	}

void Host_Std::Host_UnscheduleTimer(NPP npp, uint32 timerID)
	{
	ZLOGFUNCTION(eDebug);
	}

void Host_Std::pHTTPFetcher(ZRef<HTTPFetcher> iHTTPFetcher, void* iNotifyData,
	const std::string& iURL, const std::string& iMIME, const ZHTTP::Data& iHeaders,
	ZRef<ZStreamerR> iStreamerR)
	{
	ZLOGFUNCTION(eDebug);
	fHTTPFetchers.Erase(iHTTPFetcher);
	this->SendDataAsync(iNotifyData, iURL, iMIME, iHeaders, iStreamerR);
	}

void Host_Std::CreateAndLoad(
	const std::string& iURL, const std::string& iMIME,
	const Param_t* iParams, size_t iCount)
	{
	fURL = iURL;

	vector<char*> names;
	vector<char*> values;
	for (size_t x = 0; x < iCount; ++x)
		{
		names.push_back(const_cast<char*>(iParams[x].first.c_str()));
		values.push_back(const_cast<char*>(iParams[x].second.c_str()));
		}

	this->Guest_New(
		const_cast<char*>(iMIME.c_str()), NP_EMBED,
		iCount, ZUtil_STL::sFirstOrNil(names), ZUtil_STL::sFirstOrNil(values),
		nullptr);

	this->Host_GetURLNotify(nullptr, fURL.c_str(), nullptr, nullptr);
	this->PostCreateAndLoad();
	}

void Host_Std::PostCreateAndLoad()
	{
	}

void Host_Std::Destroy()
	{
	NPSavedData* theSavedData;
	this->Guest_Destroy(&theSavedData);
	if (theSavedData)
		{
		if (theSavedData->buf)
			HostMeister::sGet()->MemFree(theSavedData->buf);
		HostMeister::sGet()->MemFree(theSavedData);
		}
	}

void Host_Std::SendDataAsync(
	void* iNotifyData,
	const std::string& iURL, const std::string& iMIME, const ZHTTP::Data& iHeaders,
	ZRef<ZStreamerR> iStreamerR)
	{
	ZLOGFUNCTION(eDebug);
	Sender* theSender = new Sender(this, this->GetNPP(),
		iNotifyData, iURL, iMIME, iHeaders, iStreamerR);
	fSenders.Insert(theSender);
	}

void Host_Std::SendDataSync(
	void* iNotifyData,
	const string& iURL, const string& iMIME,
	const ZStreamR& iStreamR)
	{
	NPStream_Z theNPStream;
	theNPStream.ndata = this->GetNPP().ndata;
	theNPStream.pdata = this->GetNPP().pdata;
	theNPStream.url = iURL.c_str();
	theNPStream.end = iStreamR.CountReadable();
	theNPStream.lastmodified = 0;
	theNPStream.notifyData = iNotifyData;
	theNPStream.headers = nullptr;

	uint16 theStreamType = NP_NORMAL;

	if (0 == this->Guest_NewStream(
		const_cast<char*>(iMIME.c_str()), &theNPStream, false, &theStreamType))
		{
		for (bool keepGoing = true; keepGoing; /*no inc*/)
			{
			int32 countPossible = this->Guest_WriteReady(&theNPStream);
			if (countPossible < 0)
				{
				// Failure.
				break;
				}

			countPossible = std::min(countPossible, int32(1024 * 1024));

			vector<uint8> buffer;
			buffer.resize(countPossible);
			size_t countRead;
			iStreamR.Read(&buffer[0], countPossible, &countRead);
			if (countRead == 0)
				break;

			for (size_t start = 0; start < countRead; /*no inc*/)
				{
				int countWritten = this->Guest_Write(
					&theNPStream, 0, countRead - start, &buffer[start]);

				if (countWritten < 0)
					{
					if (ZLOGPF(s, eDebug))
						s << "write failure";
					keepGoing = false;
					break;
					}
				start += countWritten;
				}
			}

		if (iNotifyData)
			this->Guest_URLNotify(iURL.c_str(), NPRES_DONE, iNotifyData);

		this->Guest_DestroyStream(&theNPStream, NPRES_DONE);
		}
	}

void Host_Std::DeliverData()
	{
	for (ZSafeSetIterConst<Sender*> i = fSenders;;)
		{
		if (Sender* current = i.ReadInc())
			{
			if (!current->DeliverData())
				{
				fSenders.Erase(current);
				delete current;
				}
			}
		else
			{
			break;
			}
		}
	}

ZRef<NPObjectH> Host_Std::Host_GetWindowObject()
	{ return null; }

ZRef<NPObjectH> Host_Std::Host_GetPluginObject()
	{ return null; }

ZRef<NPObjectH> Host_Std::Guest_GetNPObject()
	{
	ZRef<NPObjectH> theNPObject;
	this->Guest_GetValue(NPPVpluginScriptableNPObject, &theNPObject);
	return theNPObject;
	}

} // namespace ZNetscape
} // namespace ZooLib
