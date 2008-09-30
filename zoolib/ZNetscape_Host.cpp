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

#include "zoolib/ZNetscape_Host.h"

#include "zoolib/ZCommer.h" // For sStartReaderRunner
#include "zoolib/ZDebug.h"
#include "zoolib/ZLog.h"
#include "zoolib/ZMemory.h" // For ZBlockZero
#include "zoolib/ZStreamCopier.h"
#include "zoolib/ZStreamRWCon_MemoryPipe.h"
#include "zoolib/ZString.h" // For ZString::sFormat

#include "zoolib/ZStream_Tee.h"
#include "zoolib/ZStreamRWPos_RAM.h"
#include "zoolib/ZUtil_Strim_Data.h"

#include <vector>

using std::list;
using std::string;
using std::vector;

#ifdef NPVERS_HAS_RESPONSE_HEADERS
	typedef NPStream NPStream_Z;
#else
	struct NPStream_Z : public NPStream
		{
		const char* headers;
		};
#endif

namespace ZNetscape {

// =================================================================================================
#pragma mark -
#pragma mark * HostMeister

HostMeister* sHostMeister;

HostMeister* HostMeister::sGetHostMeister()
	{
	ZAssert(sHostMeister);
	return sHostMeister;
	}

HostMeister::HostMeister()
	{
	ZAssert(!sHostMeister);
	sHostMeister = this;
	}

HostMeister::~HostMeister()
	{
	ZAssert(sHostMeister == this);
	sHostMeister = nil;
	}

Host* HostMeister::sHostFromNPP(NPP iNPP)
	{
	if (iNPP)
		return static_cast<Host*>(iNPP->ndata);
	return nil;
	}

Host* HostMeister::sHostFromStream(NPStream* iNPStream)
	{
	ZUnimplemented();
	return nil;
	}

void HostMeister::pGetNPNetscapeFuncs(NPNetscapeFuncs& oNPNetscapeFuncs)
	{
	ZBlockZero(&oNPNetscapeFuncs, sizeof(NPNetscapeFuncs));

	#ifdef NPVERS_HAS_RESPONSE_HEADERS
	oNPNetscapeFuncs.version = NPVERS_HAS_RESPONSE_HEADERS;
	#else
	oNPNetscapeFuncs.version = 17; // Urg
//	oNPNetscapeFuncs.version = 14; // Urg
	#endif

	oNPNetscapeFuncs.size = sizeof(NPNetscapeFuncs);
	
	oNPNetscapeFuncs.geturl = sGetURL;
	oNPNetscapeFuncs.posturl = sPostURL;
	oNPNetscapeFuncs.requestread = sRequestRead;
	oNPNetscapeFuncs.newstream = sNewStream;
	oNPNetscapeFuncs.write = sWrite;
	oNPNetscapeFuncs.destroystream = sDestroyStream;
	oNPNetscapeFuncs.status = sStatus;
	oNPNetscapeFuncs.uagent = sUserAgent;
	oNPNetscapeFuncs.memalloc = sMemAlloc;
	oNPNetscapeFuncs.memfree = sMemFree;
	oNPNetscapeFuncs.memflush = sMemFlush;
	oNPNetscapeFuncs.reloadplugins = sReloadPlugins;
	oNPNetscapeFuncs.getJavaEnv = sGetJavaEnv;
	oNPNetscapeFuncs.getJavaPeer = sGetJavaPeer;
	oNPNetscapeFuncs.geturlnotify = sGetURLNotify;
	oNPNetscapeFuncs.posturlnotify = sPostURLNotify;
	oNPNetscapeFuncs.getvalue = sGetValue;
	oNPNetscapeFuncs.setvalue = sSetValue;
	oNPNetscapeFuncs.invalidaterect = sInvalidateRect;
	oNPNetscapeFuncs.invalidateregion = sInvalidateRegion;
	oNPNetscapeFuncs.forceredraw = sForceRedraw;

	oNPNetscapeFuncs.getstringidentifier = sGetStringIdentifier;
	oNPNetscapeFuncs.getstringidentifiers = sGetStringIdentifiers;
	oNPNetscapeFuncs.getintidentifier = sGetIntIdentifier;
	oNPNetscapeFuncs.identifierisstring = sIdentifierIsString;
	oNPNetscapeFuncs.utf8fromidentifier = sUTF8FromIdentifier;
	// AG. Need to do this cast because Mac headers are wrong.
	oNPNetscapeFuncs.intfromidentifier = (NPN_IntFromIdentifierProcPtr)sIntFromIdentifier;
	oNPNetscapeFuncs.createobject = sCreateObject;
	oNPNetscapeFuncs.retainobject = sRetainObject;
	oNPNetscapeFuncs.releaseobject = sReleaseObject;
	oNPNetscapeFuncs.invoke = sInvoke;
	oNPNetscapeFuncs.invokeDefault = sInvokeDefault;
	oNPNetscapeFuncs.evaluate = sEvaluate;
	oNPNetscapeFuncs.getproperty = sGetProperty;
	oNPNetscapeFuncs.setproperty = sSetProperty;
	oNPNetscapeFuncs.removeproperty = sRemoveProperty;
	oNPNetscapeFuncs.hasproperty = sHasProperty;
	oNPNetscapeFuncs.hasmethod = sHasMethod;
	oNPNetscapeFuncs.releasevariantvalue = sReleaseVariantValue;
	oNPNetscapeFuncs.setexception = sSetException;
	}

// -----
// Forwarded to HostMeister

void* HostMeister::sMemAlloc(uint32 size)
	{ return sGetHostMeister()->MemAlloc(size); }

void HostMeister::sMemFree(void* ptr)
	{ sGetHostMeister()->MemFree(ptr); }

uint32 HostMeister::sMemFlush(uint32 size)
	{ return sGetHostMeister()->MemFlush(size); }

void HostMeister::sReloadPlugins(NPBool reloadPages)
	{ sGetHostMeister()->ReloadPlugins(reloadPages); }

void* HostMeister::sGetJavaEnv()
	{ return sGetHostMeister()->GetJavaEnv(); }

void HostMeister::sReleaseVariantValue(NPVariant* variant)
	{ sGetHostMeister()->ReleaseVariantValue(variant); }

NPIdentifier HostMeister::sGetStringIdentifier(const NPUTF8* name)
	{ return sGetHostMeister()->GetStringIdentifier(name); }

void HostMeister::sGetStringIdentifiers(
	const NPUTF8** names, int32_t nameCount, NPIdentifier* identifiers)
	{ sGetHostMeister()->GetStringIdentifiers(names, nameCount, identifiers); }

NPIdentifier HostMeister::sGetIntIdentifier(int32_t intid)
	{ return sGetHostMeister()->GetIntIdentifier(intid); }

int32_t HostMeister::sIntFromIdentifier(NPIdentifier identifier)
	{ return sGetHostMeister()->IntFromIdentifier(identifier); }

bool HostMeister::sIdentifierIsString(NPIdentifier identifier)
	{ return sGetHostMeister()->IdentifierIsString(identifier); }

NPUTF8* HostMeister::sUTF8FromIdentifier(NPIdentifier identifier)
	{ return sGetHostMeister()->UTF8FromIdentifier(identifier); }

NPObject* HostMeister::sRetainObject(NPObject* obj)
	{ return sGetHostMeister()->RetainObject(obj); }

void HostMeister::sReleaseObject(NPObject* obj)
	{ sGetHostMeister()->ReleaseObject(obj); }

void HostMeister::sSetException(NPObject* obj, const NPUTF8* message)
	{ sGetHostMeister()->SetException(obj, message); }

// -----
// Forwarded to Host

NPError HostMeister::sGetURLNotify(NPP npp,
	const char* URL, const char* window, void* notifyData)
	{ return sHostFromNPP(npp)->GetURLNotify(npp, URL, window, notifyData); }

NPError HostMeister::sPostURLNotify(NPP npp,
	const char* URL, const char* window,
	uint32 len, const char* buf, NPBool file, void* notifyData)
	{ return sHostFromNPP(npp)->PostURLNotify(npp, URL, window, len, buf, file, notifyData); }

NPError HostMeister::sRequestRead(NPStream* stream, NPByteRange* rangeList)
	{ return sHostFromStream(stream)->RequestRead(stream, rangeList); }

NPError HostMeister::sNewStream(NPP npp,
	NPMIMEType type, const char* window, NPStream** stream)
	{ return sHostFromNPP(npp)->NewStream(npp, type, window, stream); }

int32 HostMeister::sWrite(NPP npp, NPStream* stream, int32 len, void* buffer)
	{ return sHostFromNPP(npp)->Write(npp, stream, len, buffer); }

NPError HostMeister::sDestroyStream(NPP npp, NPStream* stream, NPReason reason)
	{ return sHostFromNPP(npp)->DestroyStream(npp, stream, reason); }

void HostMeister::sStatus(NPP npp, const char* message)
	{ return sHostFromNPP(npp)->Status(npp, message); }

const char* HostMeister::sUserAgent(NPP npp)
	{
	if (Host* theHost = sHostFromNPP(npp))
		return theHost->UserAgent(npp);
	return "Unknown";
	}

NPError HostMeister::sGetValue(NPP npp, NPNVariable variable, void* ret_value)
	{ return sHostFromNPP(npp)->GetValue(npp, variable, ret_value); }

NPError HostMeister::sSetValue(NPP npp, NPPVariable variable, void* value)
	{ return sHostFromNPP(npp)->SetValue(npp, variable, value); }

void HostMeister::sInvalidateRect(NPP npp, NPRect* rect)
	{ return sHostFromNPP(npp)->InvalidateRect(npp, rect); }

void HostMeister::sInvalidateRegion(NPP npp, NPRegion region)
	{ return sHostFromNPP(npp)->InvalidateRegion(npp, region); }

void HostMeister::sForceRedraw(NPP npp)
	{ return sHostFromNPP(npp)->ForceRedraw(npp); }

NPError HostMeister::sGetURL(NPP npp, const char* URL, const char* window)
	{ return sHostFromNPP(npp)->GetURL(npp, URL, window); }

NPError HostMeister::sPostURL(NPP npp,
	const char* URL, const char* window, uint32 len, const char* buf, NPBool file)
	{ return sHostFromNPP(npp)->PostURL(npp, URL, window, len, buf, file); }

void* HostMeister::sGetJavaPeer(NPP npp)
	{ return sHostFromNPP(npp)->GetJavaPeer(npp); }

NPObject* HostMeister::sCreateObject(NPP npp, NPClass* aClass)
	{ return sHostFromNPP(npp)->CreateObject(npp, aClass); }

bool HostMeister::sInvoke(NPP npp,
	NPObject* obj, NPIdentifier methodName, const NPVariant* args, unsigned argCount,
	NPVariant* result)
	{ return sHostFromNPP(npp)->Invoke(npp, obj, methodName, args, argCount, result); }

bool HostMeister::sInvokeDefault(NPP npp,
	NPObject* obj, const NPVariant* args, unsigned argCount, NPVariant* result)
	{ return sHostFromNPP(npp)->InvokeDefault(npp, obj, args, argCount, result); }

bool HostMeister::sEvaluate(NPP npp,
	NPObject* obj, NPString* script, NPVariant* result)
	{ return sHostFromNPP(npp)->Evaluate(npp, obj, script, result); }

bool HostMeister::sGetProperty(NPP npp,
	NPObject* obj, NPIdentifier propertyName, NPVariant* result)
	{ return sHostFromNPP(npp)->GetProperty(npp, obj, propertyName, result); }

bool HostMeister::sSetProperty(NPP npp,
	NPObject* obj, NPIdentifier propertyName, const NPVariant* value)
	{ return sHostFromNPP(npp)->SetProperty(npp, obj, propertyName, value); }

bool HostMeister::sHasProperty(NPP npp, NPObject* npobj, NPIdentifier propertyName)
	{ return sHostFromNPP(npp)->HasProperty(npp, npobj, propertyName); }

bool HostMeister::sHasMethod(NPP npp, NPObject* npobj, NPIdentifier methodName)
	{ return sHostFromNPP(npp)->HasMethod(npp, npobj, methodName); }

bool HostMeister::sRemoveProperty(NPP npp, NPObject* obj, NPIdentifier propertyName)
	{ return sHostFromNPP(npp)->RemoveProperty(npp, obj, propertyName); }

// =================================================================================================
#pragma mark -
#pragma mark * GuestFactory

GuestFactory::GuestFactory()
	{}

GuestFactory::~GuestFactory()
	{}

void GuestFactory::GetNPNetscapeFuncs(NPNetscapeFuncs& oNPNetscapeFuncs)
	{
	HostMeister::sGetHostMeister()->pGetNPNetscapeFuncs(oNPNetscapeFuncs);
	}

// =================================================================================================
#pragma mark -
#pragma mark * Host

Host::Host(ZRef<GuestFactory> iGuestFactory)
:	fGuestFactory(iGuestFactory),
	fNPObject(nil)
	{
	ZBlockZero(&fNPPluginFuncs, sizeof(fNPPluginFuncs));
	ZBlockZero(&fNPP_t, sizeof(fNPP_t));
	fNPPluginFuncs.size = sizeof(fNPPluginFuncs);
	fGuestFactory->GetEntryPoints(fNPPluginFuncs);
	fNPP_t.ndata = this;
	}

Host::~Host()
	{
	}

void Host::Create(const string& iURL, const string& iMIME)
	{
    char* npp_argv[] =
		{ const_cast<char*>(iURL.c_str()), const_cast<char*>(iMIME.c_str()),};

    char* npp_argn[] =
		{ "src", "type" };

	
	NPError theErr = fNPPluginFuncs.newp(
		const_cast<char*>(iMIME.c_str()), &fNPP_t, NP_FULL,
		countof(npp_argn), npp_argn, npp_argv, nil);

	if (ZLOG(s, eDebug + 1, "Host"))
		s.Writef("Create, theErr: %d", theErr);
	}

void Host::Destroy()
	{
	NPSavedData* theSavedData;
	NPError theErr = fNPPluginFuncs.destroy(&fNPP_t, &theSavedData);
	}

void Host::HostActivate(bool iActivate)
	{
	EventRecord fakeEvent;
	fakeEvent.what = activateEvt;
	fakeEvent.when = ::TickCount();
	fakeEvent.message = (UInt32)::GetWindowFromPort(fNP_Port.port);
	if (iActivate)
		fakeEvent.modifiers = activeFlag;
	else
		fakeEvent.modifiers = 0;
	::GetGlobalMouse(&fakeEvent.where);
	
	NPError theErr = fNPPluginFuncs.event(&fNPP_t, &fakeEvent);
	}

void Host::HostIdle()
	{
	EventRecord fakeEvent;
	fakeEvent.what = nullEvent;
	fakeEvent.when = ::TickCount();
	fakeEvent.message = 0;
	::GetGlobalMouse(&fakeEvent.where);
	
	NPError theErr = fNPPluginFuncs.event(&fNPP_t, &fakeEvent);
	}

void Host::HostDeliverData()
	{
	this->pDeliverData();
	}

void Host::HostDraw()
	{
	EventRecord fakeEvent;
	fakeEvent.what = updateEvt;
	fakeEvent.when = ::TickCount();
	fakeEvent.message = (UInt32)::GetWindowFromPort(fNP_Port.port);
	fakeEvent.modifiers = 0;
	::GetGlobalMouse(&fakeEvent.where);
	
	NPError theErr = fNPPluginFuncs.event(&fNPP_t, &fakeEvent);
	}

void Host::HostSetWindow(CGrafPtr iGrafPtr,
	ZooLib::ZPoint iLocation, ZooLib::ZPoint iSize, const ZooLib::ZRect& iClip)
	{
	fNP_Port.port = iGrafPtr;
	this->HostSetBounds(iLocation, iSize, iClip);
	}

void Host::HostSetBounds(
	ZooLib::ZPoint iLocation, ZooLib::ZPoint iSize, const ZooLib::ZRect& iClip)
	{
	fNP_Port.portx = -iLocation.h;
	fNP_Port.porty = -iLocation.v;
	fNPWindow.window = &fNP_Port;

	fNPWindow.type = NPWindowTypeDrawable;

	fNPWindow.x = iLocation.h;
	fNPWindow.y = iLocation.v;
	fNPWindow.width = iSize.h;
	fNPWindow.height = iSize.v;

	fNPWindow.clipRect.left = iClip.left;
	fNPWindow.clipRect.top = iClip.top;
	fNPWindow.clipRect.right = iClip.right;
	fNPWindow.clipRect.bottom = iClip.bottom;

	NPError theErr = fNPPluginFuncs.setwindow(&fNPP_t, &fNPWindow);
	}

class Host::Sender
	{
public:
	Sender(Host* iHost, const NPP_t& iNPP_t,
		void* iNotifyData,
		const std::string& iURL, const std::string& iMIME, const ZMemoryBlock& iHeaders,
		ZRef<ZStreamerRCon> iStreamerRCon);
	~Sender();

	bool DeliverData();

private:
	bool pDeliverData();

	bool fSentNew;
	Host* fHost;
	void* fNotifyData;
	const string fURL;
	const string fMIME;
	const ZMemoryBlock fHeaders;
	NPStream_Z fNPStream;
	ZRef<ZStreamerRCon> fStreamerRCon;
	};

Host::Sender::Sender(Host* iHost, const NPP_t& iNPP_t,
	void* iNotifyData,
	const std::string& iURL, const std::string& iMIME, const ZMemoryBlock& iHeaders,
	ZRef<ZStreamerRCon> iStreamerRCon)
:	fSentNew(false),
	fHost(iHost),
	fNotifyData(iNotifyData),
	fURL(iURL),
	fMIME(iMIME),
	fHeaders(iHeaders),
	fStreamerRCon(iStreamerRCon)
	{
	fNPStream.ndata = iNPP_t.ndata;
	fNPStream.pdata = iNPP_t.pdata;
	fNPStream.url = fURL.c_str();
	fNPStream.end = 0;
	fNPStream.lastmodified = 0;
	fNPStream.notifyData = iNotifyData;
	fNPStream.headers = static_cast<const char*>(fHeaders.GetData());
	}

Host::Sender::~Sender()
	{
	
	}

bool Host::Sender::DeliverData()
	{
	if (!fSentNew)
		{
		fSentNew = true;

		if (!fStreamerRCon)
			{
			fHost->HostURLNotify(fURL.c_str(), NPRES_NETWORK_ERR, fNotifyData);
			return false;
			}

		uint16 theStreamType = NP_NORMAL;
		if (fHost->HostNewStream(const_cast<char*>(fMIME.c_str()), &fNPStream, false, &theStreamType))
			{
			// Failed -- what result should we pass?
			fHost->HostURLNotify(fURL.c_str(), NPRES_NETWORK_ERR, fNotifyData);
			return false;
			}
		}


	if (this->pDeliverData())
		return true;

	if (fNotifyData)
		fHost->HostURLNotify(fURL.c_str(), NPRES_DONE, fNotifyData);

	fHost->HostDestroyStream(&fNPStream, NPRES_DONE);

	return false;
	}

bool Host::Sender::pDeliverData()
	{
	const ZStreamRCon& theStreamRCon = fStreamerRCon->GetStreamRCon();

	if (!theStreamRCon.WaitReadable(0))
		{
		if (ZLOG(s, eDebug, "Host::Sender"))
			s.Writef("waitReadable is false");
		return true;
		}

	const size_t countReadable = theStreamRCon.CountReadable();

	if (ZLOG(s, eDebug, "Host::Sender"))
		s.Writef("countReadable = %d", countReadable);
	
	if (countReadable == 0)
		return false;

	int32 countPossible = fHost->HostWriteReady(&fNPStream);

	if (ZLOG(s, eDebug, "Host::Sender"))
		s.Writef("countPossible = %d", countPossible);

	if (countPossible < 0)
		{
		return false;
		}
	else if (countPossible > 0)
		{
		countPossible = std::min(countPossible, 64 * 1024);

		vector<uint8> buffer;
		buffer.resize(countPossible);
		size_t countRead;
		theStreamRCon.Read(&buffer[0], countPossible, &countRead);
		if (countRead == 0)
			return false;

		if (ZLOG(s, eDebug, "Host::Sender"))
			s.Writef("countRead = %d", countRead);

		for (size_t start = 0; start < countRead; /*no inc*/)
			{
			int countWritten = fHost->HostWrite(&fNPStream, 0, countRead - start, &buffer[start]);

			if (ZLOG(s, eDebug, "Host::Sender"))
				s.Writef("countWritten = %d", countWritten);

			if (countWritten < 0)
				return false;

			start += countWritten;
			}
		}

	return true;
	}


void Host::pDeliverData()
	{
	ZMutexLocker locker(fMutex);
	for (list<Sender*>::iterator i = fSenders.begin(); i != fSenders.end(); /*no inc*/)
		{
		if ((*i)->DeliverData())
			{
			++i;
			}
		else
			{
			delete *i;
			i = fSenders.erase(i);
			}
		}
	}

void Host::SendDataAsync(
	void* iNotifyData,
	const std::string& iURL, const std::string& iMIME, const ZMemoryBlock& iHeaders,
	ZRef<ZStreamerRCon> iStreamerRCon)
	{
	ZMutexLocker locker(fMutex);
	Sender* theSender = new Sender(this, fNPP_t,
		iNotifyData, iURL, iMIME, iHeaders, iStreamerRCon);
	fSenders.push_back(theSender);
	}

void Host::SendDataSync(
	void* iNotifyData,
	const string& iURL, const string& iMIME,
	const ZStreamR& iStreamR)
	{
	NPStream_Z theNPStream;
	theNPStream.ndata = fNPP_t.ndata;
	theNPStream.pdata = fNPP_t.pdata;
	theNPStream.url = iURL.c_str();
	theNPStream.end = iStreamR.CountReadable();
	theNPStream.lastmodified = 0;
	theNPStream.notifyData = iNotifyData;
	theNPStream.headers = nil;

	uint16 theStreamType = NP_NORMAL;

	if (0 == fNPPluginFuncs.newstream(&fNPP_t,
		const_cast<char*>(iMIME.c_str()), &theNPStream, false, &theStreamType))
		{
		for (bool keepGoing = true; keepGoing; /*no inc*/)
			{
			int32 countPossible = fNPPluginFuncs.writeready(&fNPP_t, &theNPStream);
			if (countPossible < 0)
				{
				// Failure.
				break;
				}

			countPossible = std::min(countPossible, 1024 * 1024);

			vector<uint8> buffer;
			buffer.resize(countPossible);
			size_t countRead;
			iStreamR.Read(&buffer[0], countPossible, &countRead);
			if (countRead == 0)
				break;

			for (size_t start = 0; start < countRead; /*no inc*/)
				{
				int countWritten = fNPPluginFuncs.write(&fNPP_t, &theNPStream,
					0, countRead - start, &buffer[start]);

				if (countWritten < 0)
					{
					if (ZLOG(s, eDebug, "Host"))
						{
						s << "write failure";
						}
					keepGoing = false;
					break;
					}
				start += countWritten;
				}
			}

		if (iNotifyData)
            fNPPluginFuncs.urlnotify(&fNPP_t, iURL.c_str(), NPRES_DONE, iNotifyData);

		fNPPluginFuncs.destroystream(&fNPP_t, &theNPStream, NPRES_DONE);
		}
	}

bool Host::HostInvoke(
	NPObject* obj, const std::string& iMethod, const NPVariant* iArgs, size_t iCount,
	NPVariant& oResult)
	{
	if (obj && obj->_class->invoke)
		{
		NPIdentifier methodID = HostMeister::sGetStringIdentifier(iMethod.c_str());
		if (obj->_class->hasMethod(obj, methodID))
			{
			if (obj->_class->invoke(obj, methodID, iArgs, iCount, &oResult))
				return true;
			}
		}
	return false;
	}

NPObject* Host::GetScriptableNPObject()
	{
	if (!fNPObject)
		fNPPluginFuncs.getvalue(&fNPP_t, NPPVpluginScriptableNPObject, &fNPObject);
	return fNPObject;
	}

#define CASE(a) case a: return #a

string Host::sAsString(NPNVariable iVar)
	{
	switch (iVar)
		{
		CASE(NPNVxDisplay);
		CASE(NPNVxtAppContext);
		CASE(NPNVnetscapeWindow);
		CASE(NPNVjavascriptEnabledBool);
		CASE(NPNVasdEnabledBool);
		CASE(NPNVisOfflineBool);
		CASE(NPNVserviceManager);
		CASE(NPNVDOMElement);
		CASE(NPNVDOMWindow);
		CASE(NPNVToolkit);
		CASE(NPNVSupportsXEmbedBool);
		CASE(NPNVWindowNPObject);
		CASE(NPNVPluginElementNPObject);
		// Need to find a cleaner way to handle this portably.
		case 1000: return "NPNVpluginDrawingModel";
		case 2000: return "NPNVsupportsQuickDrawBool";
		case 2001: return "NPNVsupportsCoreGraphicsBool";
		}
	return ZString::sFormat("NPNVariable=%d", iVar);
	}

string Host::sAsString(NPPVariable iVar)
	{
	switch (iVar)
		{
		CASE(NPPVpluginNameString);
		CASE(NPPVpluginDescriptionString);
		CASE(NPPVpluginWindowBool);
		CASE(NPPVpluginTransparentBool);
		CASE(NPPVjavaClass);
		CASE(NPPVpluginWindowSize);
		CASE(NPPVpluginTimerInterval);
		CASE(NPPVpluginScriptableInstance);
		CASE(NPPVpluginScriptableIID);
		CASE(NPPVjavascriptPushCallerBool);
		CASE(NPPVpluginKeepLibraryInMemory);
		CASE(NPPVpluginNeedsXEmbed);
		CASE(NPPVpluginScriptableNPObject);
		}
	return ZString::sFormat("NPPVariable=%d", iVar);
	}

#undef CASE

void Host::HostURLNotify(const char* URL, NPReason reason, void* notifyData)
	{ fNPPluginFuncs.urlnotify(&fNPP_t, URL, reason, notifyData); }

NPError Host::HostNewStream(NPMIMEType type, NPStream* stream, NPBool seekable, uint16* stype)
	{ return fNPPluginFuncs.newstream(&fNPP_t, type, stream, seekable, stype); }

void Host::HostDestroyStream(NPStream* stream, NPReason reason)
	{ fNPPluginFuncs.destroystream(&fNPP_t, stream, reason); }

int32 Host::HostWriteReady(NPStream* stream)
	{ return fNPPluginFuncs.writeready(&fNPP_t, stream); }

int32 Host::HostWrite(NPStream* stream, int32_t offset, int32_t len, void* buffer)
	{ return fNPPluginFuncs.write(&fNPP_t, stream, offset, len, buffer); }

// =================================================================================================


NPBool NPN_VariantIsVoid (const NPVariant *variant)
{
    return variant->type == NPVariantType_Void;
}

NPBool NPN_VariantIsNull (const NPVariant *variant)
{
    return variant->type == NPVariantType_Null;
}

NPBool NPN_VariantIsUndefined (const NPVariant *variant)
{
    return variant->type == NPVariantType_Void;
}

NPBool NPN_VariantIsBool (const NPVariant *variant)
{
    return variant->type == NPVariantType_Bool;
}

NPBool NPN_VariantIsInt32 (const NPVariant *variant)
{
    return variant->type == NPVariantType_Int32;
}

NPBool NPN_VariantIsDouble (const NPVariant *variant)
{
    return variant->type == NPVariantType_Double;
}

NPBool NPN_VariantIsString (const NPVariant *variant)
{
    return variant->type == NPVariantType_String;
}

NPBool NPN_VariantIsObject (const NPVariant *variant)
{
    return variant->type == NPVariantType_Object;
}

NPBool NPN_VariantToBool (const NPVariant *variant, NPBool *result)
{
    if (variant->type != NPVariantType_Bool)
        return false;
        
    *result = variant->value.boolValue;
    
    return true;
}

NPString NPN_VariantToString (const NPVariant *variant)
{
    if (variant->type != NPVariantType_String) {
        NPString emptyString = { 0, 0 };
        return emptyString;
    }
            
    return variant->value.stringValue;
}

NPBool NPN_VariantToInt32 (const NPVariant *variant, int32_t *result)
{
    if (variant->type == NPVariantType_Int32)
        *result = variant->value.intValue;
    else if (variant->type == NPVariantType_Double)
        *result = (int32_t)variant->value.doubleValue;
    else
        return false;
    
    return true;
}

NPBool NPN_VariantToDouble (const NPVariant *variant, double *result)
{
    if (variant->type == NPVariantType_Int32)
        *result = (double)variant->value.intValue;
    else if (variant->type == NPVariantType_Double)
        *result = variant->value.doubleValue;
    else
        return false;
    
    return true;
}

NPBool NPN_VariantToObject (const NPVariant *variant, NPObject **result)
{
    if (variant->type != NPVariantType_Object)
        return false;
            
    *result = variant->value.objectValue;
    
    return true;
}

void NPN_InitializeVariantAsVoid (NPVariant *variant)
{
    variant->type = NPVariantType_Void;
}

void NPN_InitializeVariantAsNull (NPVariant *variant)
{
    variant->type = NPVariantType_Null;
}

void NPN_InitializeVariantAsUndefined (NPVariant *variant)
{
    variant->type = NPVariantType_Void;
}

void NPN_InitializeVariantWithBool (NPVariant *variant, NPBool value)
{
    variant->type = NPVariantType_Bool;
    variant->value.boolValue = value;
}

void NPN_InitializeVariantWithInt32 (NPVariant *variant, int32_t value)
{
    variant->type = NPVariantType_Int32;
    variant->value.intValue = value;
}

void NPN_InitializeVariantWithDouble (NPVariant *variant, double value)
{
    variant->type = NPVariantType_Double;
    variant->value.doubleValue = value;
}

void NPN_InitializeVariantWithString (NPVariant *variant, const NPString *value)
{
    variant->type = NPVariantType_String;
    variant->value.stringValue = *value;
}

void NPN_InitializeVariantWithStringCopy (NPVariant *variant, const NPString *value)
{
    variant->type = NPVariantType_String;
    variant->value.stringValue.UTF8Length = value->UTF8Length;
    variant->value.stringValue.UTF8Characters = (NPUTF8 *)malloc(sizeof(NPUTF8) * value->UTF8Length);
    memcpy ((void *)variant->value.stringValue.UTF8Characters, value->UTF8Characters, sizeof(NPUTF8) * value->UTF8Length);
}

void NPN_InitializeVariantWithObject (NPVariant *variant, NPObject *value)
{
    variant->type = NPVariantType_Object;
    variant->value.objectValue = HostMeister::sRetainObject(value);
}

void NPN_InitializeVariantWithVariant (NPVariant *destination, const NPVariant *source)
{
    switch (source->type){
        case NPVariantType_Void: {
            NPN_InitializeVariantAsVoid (destination);
            break;
        }
        case NPVariantType_Null: {
            NPN_InitializeVariantAsNull (destination);
            break;
        }
        case NPVariantType_Bool: {
            NPN_InitializeVariantWithBool (destination, source->value.boolValue);
            break;
        }
        case NPVariantType_Int32: {
            NPN_InitializeVariantWithInt32 (destination, source->value.intValue);
            break;
        }
        case NPVariantType_Double: {
            NPN_InitializeVariantWithDouble (destination, source->value.doubleValue);
            break;
        }
        case NPVariantType_String: {
            NPN_InitializeVariantWithStringCopy (destination, &source->value.stringValue);
            break;
        }
        case NPVariantType_Object: {
            NPN_InitializeVariantWithObject (destination, source->value.objectValue);
            break;
        }
        default: {
            NPN_InitializeVariantAsUndefined (destination);
            break;
        }
    }
}


} // namespace ZNetscape
