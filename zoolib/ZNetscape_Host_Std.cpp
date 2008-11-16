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
	{
	}

HostMeister_Std::~HostMeister_Std()
	{
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
	{ return nil; }

void HostMeister_Std::ReleaseVariantValue(NPVariant* variant)
	{
	ZAssert(variant);

	if (variant->type == NPVariantType_Object)
		{
		sReleaseObject(variant->value.objectValue);
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

int32_t HostMeister_Std::IntFromIdentifier(NPIdentifier identifier)
	{
    Identifier* theID = static_cast<Identifier*>(identifier);
    if (theID->fIsString)
        return 0;
        
    return theID->fAsInt;
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

void Host_Std::pHTTPerFinished(HTTPer* iHTTPer, void* iNotifyData,
	const std::string& iURL, const std::string& iMIME, const ZMemoryBlock& iHeaders,
	ZRef<ZStreamerRCon> iStreamerRCon)
	{
	ZMutexLocker locker(fMutex);
	ZUtil_STL::sEraseMustContain(1, fHTTPers, iHTTPer);
	this->SendDataAsync(iNotifyData, iURL, iMIME, iHeaders, iStreamerRCon);
	}

NPError Host_Std::GetURLNotify(NPP npp,
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

NPError Host_Std::PostURLNotify(NPP npp,
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

NPError Host_Std::RequestRead(NPStream* stream, NPByteRange* rangeList)
	{
	if (ZLOG(s, eDebug, "Host_Std"))
		s.Writef("RequestRead");
	return NPERR_GENERIC_ERROR;
	}

NPError Host_Std::NewStream(NPP npp,
	NPMIMEType type, const char* window, NPStream** stream)
	{
	if (ZLOG(s, eDebug, "Host_Std"))
		s.Writef("NewStream");
	return NPERR_GENERIC_ERROR;
	}

int32 Host_Std::Write(NPP npp, NPStream* stream, int32 len, void* buffer)
	{
	if (ZLOG(s, eDebug, "Host_Std"))
		s.Writef("Write");
	return -1;
	}

NPError Host_Std::DestroyStream(NPP npp, NPStream* stream, NPReason reason)
	{
	if (ZLOG(s, eDebug, "Host_Std"))
		s.Writef("DestroyStream");
	return NPERR_GENERIC_ERROR;
	}

void Host_Std::Status(NPP npp, const char* message)
	{
	if (ZLOG(s, eDebug, "Host_Std"))
		s.Writef("Status");
	}

const char* Host_Std::UserAgent(NPP npp)
	{
	if (ZLOG(s, eDebug, "Host_Std"))
		s.Writef("UserAgent");

	return "Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; SV1)";
	}

NPError Host_Std::GetValue(NPP npp, NPNVariable variable, void* ret_value)
	{
	if (ZLOG(s, eDebug, "Host_Std"))
		s << "GetValue, " << sAsString(variable) ;
	return NPERR_GENERIC_ERROR;
	}

NPError Host_Std::SetValue(NPP npp, NPPVariable variable, void* value)
	{
	if (ZLOG(s, eDebug, "Host_Std"))
		s << "SetValue, " << sAsString(variable) ;
	return NPERR_GENERIC_ERROR;
	}

void Host_Std::InvalidateRect(NPP npp, NPRect* rect)
	{
	if (ZLOG(s, eDebug, "Host_Std"))
		s.Writef("InvalidateRect");
	}

void Host_Std::InvalidateRegion(NPP npp, NPRegion region)
	{
	if (ZLOG(s, eDebug, "Host_Std"))
		s.Writef("InvalidateRegion");
	}

void Host_Std::ForceRedraw(NPP npp)
	{
	if (ZLOG(s, eDebug, "Host_Std"))
		s.Writef("ForceRedraw");
	}

NPError Host_Std::GetURL(NPP npp, const char* URL, const char* window)
	{
	if (ZLOG(s, eDebug, "Host_Std"))
		s << "GetURL: " << URL;
	return NPERR_INVALID_URL;
	}

NPError Host_Std::PostURL(NPP npp,
	const char* URL, const char* window, uint32 len, const char* buf, NPBool file)
	{
	if (ZLOG(s, eDebug, "Host_Std"))
		s << "PostURL: " << URL;
	return NPERR_INVALID_URL;
	}

void* Host_Std::GetJavaPeer(NPP npp)
	{
	if (ZLOG(s, eDebug, "Host_Std"))
		s.Writef("GetJavaPeer");
	return nil;
	}

NPObject* Host_Std::CreateObject(NPP npp, NPClass* aClass)
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

bool Host_Std::Invoke(NPP npp,
	NPObject* obj, NPIdentifier methodName, const NPVariant* args, unsigned argCount,
	NPVariant* result)
	{
	if (ZLOG(s, eDebug, "Host_Std"))
		s << "Invoke: " << HostMeister::sUTF8FromIdentifier(methodName);

	if (obj && obj->_class->invoke)
		return obj->_class->invoke(obj, methodName, args, argCount, result);

	return false;
	}

bool Host_Std::InvokeDefault(NPP npp,
	NPObject* obj, const NPVariant* args, unsigned argCount, NPVariant* result)
	{
	if (ZLOG(s, eDebug, "Host_Std"))
		s.Writef("InvokeDefault");
	return false;
	}

bool Host_Std::Evaluate(NPP npp,
	NPObject* obj, NPString* script, NPVariant* result)
	{
	if (ZLOG(s, eDebug, "Host_Std"))
		s.Writef("Evaluate");
	return false;
	}

bool Host_Std::GetProperty(NPP npp,
	NPObject* obj, NPIdentifier propertyName, NPVariant* result)
	{
	if (ZLOG(s, eDebug, "Host_Std"))
		s << "GetProperty: " << HostMeister::sUTF8FromIdentifier(propertyName);

	if (obj && obj->_class->getProperty)
		return obj->_class->getProperty(obj, propertyName, result);

	return false;
	}

bool Host_Std::SetProperty(NPP npp,
	NPObject* obj, NPIdentifier propertyName, const NPVariant* value)
	{
	if (ZLOG(s, eDebug, "Host_Std"))
		s.Writef("SetProperty");
	return false;
	}

bool Host_Std::HasProperty(NPP, NPObject* npobj, NPIdentifier propertyName)
	{
	if (ZLOG(s, eDebug, "Host_Std"))
		s.Writef("HasProperty");
	return false;
	}

bool Host_Std::HasMethod(NPP npp, NPObject* npobj, NPIdentifier methodName)
	{
	if (ZLOG(s, eDebug, "Host_Std"))
		s.Writef("HasMethod");
	return false;
	}

bool Host_Std::RemoveProperty(NPP npp, NPObject* obj, NPIdentifier propertyName)
	{
	if (ZLOG(s, eDebug, "Host_Std"))
		s.Writef("RemoveProperty");
	return false;
	}

} // namespace ZNetscape
