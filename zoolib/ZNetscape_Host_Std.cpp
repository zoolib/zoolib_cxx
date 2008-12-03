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

#include "zoolib/ZNetscape_Host_Std.h"

#include "zoolib/ZDebug.h"
#include "zoolib/ZHTTP_Requests.h"
#include "zoolib/ZLog.h"
#include "zoolib/ZMIME.h"
#include "zoolib/ZNet_Internet.h"
#include "zoolib/ZNetscape.h"
#include "zoolib/ZStreamRWPos_RAM.h"
#include "zoolib/ZStream_CRLF.h"
#include "zoolib/ZStream_Tee.h"
#include "zoolib/ZString.h"
#include "zoolib/ZThreadSimple.h"
#include "zoolib/ZTuple.h"
#include "zoolib/ZUtil_STL.h"
#include "zoolib/ZUtil_Strim_Tuple.h"

#include "zoolib/ZCompat_string.h" // For strdup

using std::map;
using std::pair;
using std::string;
using std::vector;

namespace ZNetscape {

// =================================================================================================
#pragma mark -
#pragma mark * HostMeister_Std

HostMeister_Std::HostMeister_Std()
	{}

HostMeister_Std::~HostMeister_Std()
	{}

void* HostMeister_Std::MemAlloc(uint32 size)
	{ return ::malloc(size); }

void HostMeister_Std::MemFree(void* ptr)
	{ ::free(ptr); }

uint32 HostMeister_Std::MemFlush(uint32 size)
	{ return 0; }

void HostMeister_Std::ReloadPlugins(NPBool reloadPages)
	{}

void* HostMeister_Std::GetJavaEnv()
	{ return nil; }

NPIdentifier HostMeister_Std::GetStringIdentifier(const NPUTF8* name)
	{
	const string theName = name;
	map<string, Identifier*>::iterator i = fMap_Strings.find(theName);
	if (i != fMap_Strings.end())
		return static_cast<NPIdentifier>((*i).second);

	Identifier* theIdentifier = new Identifier;

	theIdentifier->fIsString = true;
	theIdentifier->fAsString = strdup(name);
	fMap_Strings.insert(pair<string, Identifier*>(theName, theIdentifier));

	return static_cast<NPIdentifier>(theIdentifier);
	}

void HostMeister_Std::GetStringIdentifiers(
	const NPUTF8* *names, int32_t nameCount, NPIdentifier* identifiers)
	{
	while (--nameCount)
		{
		*identifiers++ = this->GetStringIdentifier(*names++);
		}
	}

NPIdentifier HostMeister_Std::GetIntIdentifier(int32_t intid)
	{
	map<int, Identifier*>::iterator i = fMap_Ints.find(intid);
	if (i != fMap_Ints.end())
		return static_cast<NPIdentifier>((*i).second);

	Identifier* theIdentifier = new Identifier;

	theIdentifier->fIsString = false;
	theIdentifier->fAsInt = intid;
	fMap_Ints.insert(pair<int, Identifier*>(intid, theIdentifier));

	return static_cast<NPIdentifier>(theIdentifier);
	}

bool HostMeister_Std::IdentifierIsString(NPIdentifier identifier)
	{
	return static_cast<Identifier*>(identifier)->fIsString;
	}

NPUTF8* HostMeister_Std::UTF8FromIdentifier(NPIdentifier identifier)
	{
    Identifier* theID = static_cast<Identifier*>(identifier);
    if (!theID->fIsString || !theID->fAsString)
        return nil;
        
    return (NPUTF8*)strdup(theID->fAsString);
	}

int32_t HostMeister_Std::IntFromIdentifier(NPIdentifier identifier)
	{
    Identifier* theID = static_cast<Identifier*>(identifier);
    if (theID->fIsString)
        return 0;
        
    return theID->fAsInt;
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
		if (obj->_class->deallocate)
			obj->_class->deallocate(obj);
		else
			free(obj);
		}
	}

void HostMeister_Std::ReleaseVariantValue(NPVariant* variant)
	{
	ZAssert(variant);

	if (variant->type == NPVariantType_Object)
		{
		this->ReleaseObject(variant->value.objectValue);
		variant->value.objectValue = nil;
		}
	else if (variant->type == NPVariantType_String)
		{
		free((void*)sNPStringChars(variant->value.stringValue));
		sNPStringChars(variant->value.stringValue) = nil;
		sNPStringLength(variant->value.stringValue) = 0;
		}
	variant->type = NPVariantType_Void;
	}

void HostMeister_Std::SetException(NPObject* obj, const NPUTF8* message)
	{
	ZUnimplemented();
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamerRCon_Fake

class ZStreamerRCon_Fake
:	public ZStreamerRCon,
	private ZStreamRCon
	{
public:
	ZStreamerRCon_Fake(ZRef<ZStreamerR> iStreamerR);

// From ZStreamerRCon
	virtual const ZStreamRCon& GetStreamRCon();

// From ZStreamR via ZStreamRCon
	virtual void Imp_Read(void* iDest, size_t iCount, size_t* oCountRead);
	virtual size_t Imp_CountReadable();

// From ZStreamRCon
	virtual bool Imp_WaitReadable(int iMilliseconds);
	virtual bool Imp_ReceiveDisconnect(int iMilliseconds);

	virtual void Imp_Abort();

private:
	ZRef<ZStreamerR> fStreamerR;
	const ZStreamR& fStreamR;
	};

ZStreamerRCon_Fake::ZStreamerRCon_Fake(ZRef<ZStreamerR> iStreamerR)
:	fStreamerR(iStreamerR),
	fStreamR(fStreamerR->GetStreamR())
	{}

const ZStreamRCon& ZStreamerRCon_Fake::GetStreamRCon()
	{ return *this; }

void ZStreamerRCon_Fake::Imp_Read(void* iDest, size_t iCount, size_t* oCountRead)
	{
	if (fStreamerR)
		{
		fStreamR.Read(iDest, iCount, oCountRead);
		}
	else if (oCountRead)
		{
		*oCountRead = 0;
		}
	}

size_t ZStreamerRCon_Fake::Imp_CountReadable()
	{
	if (fStreamerR)
		return fStreamR.CountReadable();
	return 0;
	}

bool ZStreamerRCon_Fake::Imp_WaitReadable(int iMilliseconds)
	{
	if (fStreamerR)
		return fStreamR.CountReadable();
	return true;
	}

bool ZStreamerRCon_Fake::Imp_ReceiveDisconnect(int iMilliseconds)
	{
	if (fStreamerR)
		{
		fStreamR.SkipAll();
		return true;
		}
	return false;
	}

void ZStreamerRCon_Fake::Imp_Abort()
	{
	fStreamerR.Clear();
	}

// =================================================================================================
#pragma mark -
#pragma mark * Host_Std::HTTPer

class Host_Std::HTTPer
	{
public:
	HTTPer(Host_Std* iHost, const string& iURL, ZMemoryBlock* iMB, void* iNotifyData);

	void Start();
	void Cancel();

private:
	void pRun();
	static void spRun(HTTPer* iHTTPer);

	const string fURL;
	void* fNotifyData;
	Host_Std* fHost;
	ZMemoryBlock fMB;
	bool fIsPOST;
	};

Host_Std::HTTPer::HTTPer(Host_Std* iHost, const string& iURL, ZMemoryBlock* iMB, void* iNotifyData)
:	fHost(iHost),
	fURL(iURL),
	fNotifyData(iNotifyData)
	{
	if (iMB)
		{
		fMB = *iMB;
		fIsPOST = true;
		}
	else
		{
		fIsPOST = false;
		}
	}

void Host_Std::HTTPer::Start()
	{ (new ZThreadSimple<HTTPer*>(spRun, this))->Start(); }

void Host_Std::HTTPer::Cancel()
	{
	fHost = nil;
	}

void Host_Std::HTTPer::pRun()
	{
	try
		{
		string theURL = fURL;
		ZTuple theHeaders;
		ZMemoryBlock theRawHeaders;	
		ZRef<ZStreamerR> theStreamerR;
		if (fIsPOST)
			{
			theStreamerR = ZHTTP::sPost(
				theURL, ZStreamRWPos_MemoryBlock(fMB), nil, &theHeaders, &theRawHeaders);
			}
		else
			{
			theStreamerR = ZHTTP::sRequest("GET", theURL, nil, &theHeaders, &theRawHeaders);
			}

		if (theStreamerR && fHost)
			{
			const ZTuple theCT = theHeaders.GetTuple("content-type");
			const string theMIME = theCT.GetString("type") + "/" + theCT.GetString("subtype");

			ZRef<ZStreamerRCon> theStreamerRCon = ZRefDynamicCast<ZStreamerRCon>(theStreamerR);
			if (!theStreamerRCon)
				theStreamerRCon = new ZStreamerRCon_Fake(theStreamerR);

			fHost->pHTTPerFinished(
				this, fNotifyData, theURL, theMIME, theRawHeaders, theStreamerRCon);
			return;
			}
		}
	catch (...)
		{}

	// This causes async delivery of an error.
	if (fHost)
		{
		fHost->pHTTPerFinished(
			this, fNotifyData, fURL, "", ZMemoryBlock(), ZRef<ZStreamerRCon>());
		}
	}

void Host_Std::HTTPer::spRun(HTTPer* iHTTPer)
	{
	iHTTPer->pRun();
	delete iHTTPer;
	}

// =================================================================================================
#pragma mark -
#pragma mark * Host_Std

Host_Std::Host_Std(ZRef<GuestFactory> iGuestFactory)
:	Host(iGuestFactory)
	{}

Host_Std::~Host_Std()
	{
	ZMutexLocker locker(fMutex);
	for (vector<HTTPer*>::iterator i = fHTTPers.begin(); i != fHTTPers.end(); ++i)
		{
		(*i)->Cancel();
		}
	}

NPError Host_Std::Host_GetURL(NPP npp, const char* URL, const char* window)
	{
	if (ZLOG(s, eDebug, "Host_Std"))
		s << "GetURL: " << URL;
	return NPERR_INVALID_URL;
	}

NPError Host_Std::Host_PostURL(NPP npp,
	const char* URL, const char* window, uint32 len, const char* buf, NPBool file)
	{
	if (ZLOG(s, eDebug, "Host_Std"))
		s << "PostURL: " << URL;
	return NPERR_INVALID_URL;
	}

NPError Host_Std::Host_RequestRead(NPStream* stream, NPByteRange* rangeList)
	{
	if (ZLOG(s, eDebug, "Host_Std"))
		s.Writef("RequestRead");
	return NPERR_GENERIC_ERROR;
	}

NPError Host_Std::Host_NewStream(NPP npp,
	NPMIMEType type, const char* window, NPStream** stream)
	{
	if (ZLOG(s, eDebug, "Host_Std"))
		s.Writef("NewStream");
	return NPERR_GENERIC_ERROR;
	}

int32 Host_Std::Host_Write(NPP npp, NPStream* stream, int32 len, void* buffer)
	{
	if (ZLOG(s, eDebug, "Host_Std"))
		s.Writef("Write");
	return -1;
	}

NPError Host_Std::Host_DestroyStream(NPP npp, NPStream* stream, NPReason reason)
	{
	if (ZLOG(s, eDebug, "Host_Std"))
		s.Writef("DestroyStream");
	return NPERR_GENERIC_ERROR;
	}

void Host_Std::Host_Status(NPP npp, const char* message)
	{
	if (ZLOG(s, eDebug, "Host_Std"))
		s.Writef("Status");
	}

const char* Host_Std::Host_UserAgent(NPP npp)
	{
	if (ZLOG(s, eDebug, "Host_Std"))
		s.Writef("UserAgent");

	return "Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; SV1)";
	}

void* Host_Std::Host_GetJavaPeer(NPP npp)
	{
	if (ZLOG(s, eDebug, "Host_Std"))
		s.Writef("GetJavaPeer");
	return nil;
	}

NPError Host_Std::Host_GetURLNotify(NPP npp,
	const char* URL, const char* window, void* notifyData)
	{
	if (ZLOG(s, eDebug, "Host_Std"))
		{
		s << "GetURLNotify: " << URL;
		if (window)
			s << " target: " << window;
		}

	if (URL == strstr(URL, "http:"))
		{
		HTTPer* theG = new HTTPer(this, URL, nil, notifyData);

		ZMutexLocker locker(fMutex);
		fHTTPers.push_back(theG);
		theG->Start();
		return NPERR_NO_ERROR;
		}

	return NPERR_INVALID_URL;
	}

NPError Host_Std::Host_PostURLNotify(NPP npp,
	const char* URL, const char* window,
	uint32 len, const char* buf, NPBool file, void* notifyData)
	{
	if (ZLOG(s, eDebug, "Host_Std"))
		s << "PostURLNotify: " << URL;

	if (URL == strstr(URL, "http:"))
		{
		ZMemoryBlock theData(buf, len);
		HTTPer* theG = new HTTPer(this, URL, &theData, notifyData);

		ZMutexLocker locker(fMutex);
		fHTTPers.push_back(theG);
		theG->Start();
		return NPERR_NO_ERROR;
		}

	return NPERR_INVALID_URL;
	}

NPError Host_Std::Host_GetValue(NPP npp, NPNVariable variable, void* ret_value)
	{
	if (ZLOG(s, eDebug, "Host_Std"))
		s << "GetValue, " << ZNetscape::sAsString(variable) ;
	return NPERR_GENERIC_ERROR;
	}

NPError Host_Std::Host_SetValue(NPP npp, NPPVariable variable, void* value)
	{
	if (ZLOG(s, eDebug, "Host_Std"))
		s << "SetValue, " << ZNetscape::sAsString(variable) ;
	return NPERR_GENERIC_ERROR;
	}

void Host_Std::Host_InvalidateRect(NPP npp, NPRect* rect)
	{
	if (ZLOG(s, eDebug, "Host_Std"))
		s.Writef("InvalidateRect");
	}

void Host_Std::Host_InvalidateRegion(NPP npp, NPRegion region)
	{
	if (ZLOG(s, eDebug, "Host_Std"))
		s.Writef("InvalidateRegion");
	}

void Host_Std::Host_ForceRedraw(NPP npp)
	{
	if (ZLOG(s, eDebug, "Host_Std"))
		s.Writef("ForceRedraw");
	}

NPObject* Host_Std::Host_CreateObject(NPP npp, NPClass* aClass)
	{
	if (ZLOG(s, eDebug, "Host_Std"))
		s.Writef("CreateObject");

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

bool Host_Std::Host_Invoke(NPP npp,
	NPObject* obj, NPIdentifier methodName, const NPVariant* args, uint32_t argCount,
	NPVariant* result)
	{
	if (ZLOG(s, eDebug, "Host_Std"))
		s << "Invoke: " << HostMeister::sGet()->StringFromIdentifier(methodName);

	if (obj && obj->_class && obj->_class->invoke)
		return obj->_class->invoke(obj, methodName, args, argCount, result);

	return false;
	}

bool Host_Std::Host_InvokeDefault(NPP npp,
	NPObject* obj, const NPVariant* args, uint32_t argCount, NPVariant* result)
	{
	if (ZLOG(s, eDebug, "Host_Std"))
		s.Writef("InvokeDefault");

	if (obj && obj->_class && obj->_class->invokeDefault)
		return obj->_class->invokeDefault(obj, args, argCount, result);

	return false;
	}

bool Host_Std::Host_Evaluate(NPP npp,
	NPObject* obj, NPString* script, NPVariant* result)
	{
	if (ZLOG(s, eDebug, "Host_Std"))
		s.Writef("Evaluate");
	return false;
	}

bool Host_Std::Host_GetProperty(NPP npp,
	NPObject* obj, NPIdentifier propertyName, NPVariant* result)
	{
	if (ZLOG(s, eDebug, "Host_Std"))
		s << "GetProperty: " << HostMeister::sGet()->StringFromIdentifier(propertyName);

	if (obj && obj->_class && obj->_class->getProperty)
		return obj->_class->getProperty(obj, propertyName, result);

	return false;
	}

bool Host_Std::Host_SetProperty(NPP npp,
	NPObject* obj, NPIdentifier propertyName, const NPVariant* value)
	{
	if (ZLOG(s, eDebug, "Host_Std"))
		s.Writef("SetProperty");

	if (obj && obj->_class && obj->_class->getProperty)
		return obj->_class->setProperty(obj, propertyName, value);

	return false;
	}

bool Host_Std::Host_RemoveProperty(NPP npp, NPObject* obj, NPIdentifier propertyName)
	{
	if (ZLOG(s, eDebug, "Host_Std"))
		s.Writef("RemoveProperty");

	if (obj && obj->_class && obj->_class->removeProperty)
		return obj->_class->removeProperty(obj, propertyName);

	return false;
	}

bool Host_Std::Host_HasProperty(NPP, NPObject* obj, NPIdentifier propertyName)
	{
	if (ZLOG(s, eDebug, "Host_Std"))
		s.Writef("HasProperty");

	if (obj && obj->_class && obj->_class->hasProperty)
		return obj->_class->hasProperty(obj, propertyName);

	return false;
	}

bool Host_Std::Host_HasMethod(NPP npp, NPObject* obj, NPIdentifier methodName)
	{
	if (ZLOG(s, eDebug, "Host_Std"))
		s.Writef("HasMethod");

	if (obj && obj->_class && obj->_class->hasMethod)
		return obj->_class->hasMethod(obj, methodName);

	return false;
	}

void Host_Std::pHTTPerFinished(HTTPer* iHTTPer, void* iNotifyData,
	const std::string& iURL, const std::string& iMIME, const ZMemoryBlock& iHeaders,
	ZRef<ZStreamerRCon> iStreamerRCon)
	{
	ZMutexLocker locker(fMutex);
	ZUtil_STL::sEraseMustContain(1, fHTTPers, iHTTPer);
	this->SendDataAsync(iNotifyData, iURL, iMIME, iHeaders, iStreamerRCon);
	}

} // namespace ZNetscape
