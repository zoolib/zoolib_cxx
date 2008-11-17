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
#include "zoolib/ZHTTP.h"
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
		free((void*)variant->value.stringValue.UTF8Characters);
		variant->value.stringValue.UTF8Characters = 0;
		variant->value.stringValue.UTF8Length = 0;
		}
	variant->type = NPVariantType_Void;
	}

void HostMeister_Std::SetException(NPObject* obj, const NPUTF8* message)
	{
	ZUnimplemented();
	}

// =================================================================================================
#pragma mark -
#pragma mark * HTTP helper functions

static bool sParseURL(const string& iURL, string& oHost, ip_port& oPort, string& oPath)
	{
	oPath.clear();

	const char httpPrefix[] = "http://";
	const size_t httpPrefixLength = strlen(httpPrefix);
	string hostAndPort = iURL;
	if (iURL.length() > httpPrefixLength)
		{
		if (ZString::sEquali(iURL.substr(0, httpPrefixLength), httpPrefix))
			{
			const size_t slashOffset = iURL.find('/', httpPrefixLength);
			hostAndPort = iURL.substr(httpPrefixLength, slashOffset - httpPrefixLength);
			if (slashOffset != string::npos)
				oPath = iURL.substr(std::min(iURL.size(), slashOffset + 1));
			oPath = "/" + oPath;
			}
		}

	const size_t colonOffset = hostAndPort.find(':');
	if (colonOffset != string::npos)
		{
		oPort = ZString::sAsInt(hostAndPort.substr(colonOffset + 1));
		oHost = hostAndPort.substr(0, colonOffset);
		}
	else
		{
		oPort = 80;
		oHost = hostAndPort;
		}

	return true;
	}

static bool sHTTP(const ZStreamW& w, const ZStreamR& r,
	const string& iHost, const string& iPath, const ZMemoryBlock* iPOSTData,
	int& oResponseCode, ZMemoryBlock& oRawHeaders, ZTuple& oHeaders, string& oMIME)
	{
	if (ZLOG(s, eDebug, "ZNetscape"))
		s << "sHTTP";

	if (iPOSTData)
		w.WriteString("POST ");
	else
		w.WriteString("GET ");
	w.WriteString(iPath);
	w.WriteString(" HTTP/1.1\r\n");
	w.WriteString("Host: ");
	w.WriteString(iHost);
	w.WriteString("\r\n");
	w.WriteString("Connection: close\r\n");
	if (iPOSTData)
		w.Writef("Content-Length: %ld\r\n", iPOSTData->GetSize());
	w.WriteString("\r\n");
	if (iPOSTData)
		w.Write(iPOSTData->GetData(), iPOSTData->GetSize());
	w.Flush();

	ZStreamRWPos_RAM theHeaderStream;
	ZStreamR_Tee theStream_Tee(r, theHeaderStream);
	ZMIME::StreamR_Header theSIH_Server(theStream_Tee);

	string serverResultMessage;
	if (!ZHTTP::sReadResponse(ZStreamU_Unreader(theSIH_Server),
		&oResponseCode, &serverResultMessage))
		{
		return false;
		}

	if (!ZHTTP::sReadHeader(theSIH_Server, &oHeaders))
		{
		return false;
		}

	if (ZLOG(s, eDebug, "sHTTP"))
		{
		s << "Server response, code: "
			<< ZString::sFormat("%d", oResponseCode)
			<< ", message: " << serverResultMessage
			<< ", Headers: "
			<< ZUtil_Strim_Tuple::Format(oHeaders, 1, ZUtil_Strim_Tuple::Options());
		}

	ZTuple theCT = oHeaders.GetTuple("content-type");
	oMIME = theCT.GetString("type") + "/" + theCT.GetString("subtype");

	// Zero-terminate the header data block.
	theHeaderStream.WriteByte(0);
	theHeaderStream.SetPosition(0);

	ZStreamR_CRLFRemove(theHeaderStream).CopyAllTo(ZStreamRWPos_MemoryBlock(oRawHeaders));

	return true;
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

	string fURL;
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
	string theURL = fURL;

	for (bool keepGoing = true; keepGoing; /*no inc*/)
		{
		string theHostName;
		ip_port thePort;
		string thePath;
		if (sParseURL(theURL, theHostName, thePort, thePath))
			{
			ZRef<ZNetName_Internet> theNN = new ZNetName_Internet(theHostName, thePort);
			ZRef<ZNetEndpoint> theEndpoint = theNN->Connect(10);
			if (!theEndpoint)
				{
				if (ZLOG(s, eDebug, "ZNetscape::Host_Std::HTTPer"))
					s << "pRun, couldn't connect to server";
				break;
				}

			int theResponseCode;
			ZTuple theHeaders;
			ZMemoryBlock theRawHeaders;
			string theMIME;
			if (fIsPOST)
				{
				if (!sHTTP(theEndpoint->GetStreamW(), theEndpoint->GetStreamR(),
					theHostName, thePath, &fMB,
					theResponseCode, theRawHeaders, theHeaders, theMIME))
					{
					break;
					}
				}
			else
				{
				if (!sHTTP(theEndpoint->GetStreamW(), theEndpoint->GetStreamR(),
					theHostName, thePath, nil,
					theResponseCode, theRawHeaders, theHeaders, theMIME))
					{
					break;
					}
				}

			switch (theResponseCode)
				{
				case 200:
					{
					// Will need to inspect it to see if it's chunked or otherwise
					// encoded, and undo that encoding in the streamer we pass off.
					if (fHost)
						{
						fHost->pHTTPerFinished(
							this, fNotifyData, theURL, theMIME, theRawHeaders, theEndpoint);
						}
					return;
					// 
					}
				case 301:
				case 302:
				case 303:
					{
					if (fIsPOST)
						{
						keepGoing = false;
						}
					else
						{
						string newURI = ZHTTP::sGetString0(theHeaders.GetValue("location"));
						theURL = newURI;
						}
					break;
					}
				default:
					{
					keepGoing = false;
					break;
					}
				}
			}
		}

	// This causes async delivery of an error.
	if (fHost)
		{
		fHost->pHTTPerFinished(
			this, fNotifyData, theURL, "", ZMemoryBlock(), ZRef<ZStreamerRCon>());
		}

	delete this;
	}

void Host_Std::HTTPer::spRun(HTTPer* iHTTPer)
	{ iHTTPer->pRun(); }

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
		ZMutexLocker locker(fMutex);
		HTTPer* theG = new HTTPer(this, URL, nil, notifyData);
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
	NPObject* obj, NPIdentifier methodName, const NPVariant* args, unsigned argCount,
	NPVariant* result)
	{
	if (ZLOG(s, eDebug, "Host_Std"))
		s << "Invoke: " << HostMeister::sGet()->StringFromIdentifier(methodName);

	if (obj && obj->_class && obj->_class->invoke)
		return obj->_class->invoke(obj, methodName, args, argCount, result);

	return false;
	}

bool Host_Std::Host_InvokeDefault(NPP npp,
	NPObject* obj, const NPVariant* args, unsigned argCount, NPVariant* result)
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
