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

#ifndef __ZNetscape_Host__
#define __ZNetscape_Host__ 1
#include "zconfig.h"

#include <string>

// Need to standardize a mechanism to pull in the np headers
// where we can be agnostic as to which set we're using.
#include "npapi.h"
#include <WebKit/npfunctions.h>

#include "zoolib/ZGeom.h"
#include "zoolib/ZRefCount.h"
#include "zoolib/ZMemoryBlock.h"
#include "zoolib/ZStream.h"
#include "zoolib/ZStreamer.h"
#include "zoolib/ZThread.h"

#include <list>

namespace ZNetscape {

//struct Variant : public NPVariant
//	{
//	}

class GuestFactory;
class Host;
class HostMeister;

// =================================================================================================
#pragma mark -
#pragma mark * HostMeister

class HostMeister
	{
	friend class GuestFactory;
public:
	static HostMeister* sGetHostMeister();

	HostMeister();
	virtual ~HostMeister();

	virtual void* MemAlloc(uint32 size) = 0;

	virtual void MemFree(void* ptr) = 0;

	virtual uint32 MemFlush(uint32 size) = 0;

	virtual void ReloadPlugins(NPBool reloadPages) = 0;

	virtual void* GetJavaEnv() = 0;

	virtual void ReleaseVariantValue(NPVariant* variant) = 0;

	virtual NPIdentifier GetStringIdentifier(const NPUTF8* name) = 0;

	virtual void GetStringIdentifiers(
		const NPUTF8* *names, int32_t nameCount, NPIdentifier* identifiers) = 0;

	virtual NPIdentifier GetIntIdentifier(int32_t intid) = 0;

	virtual int32_t IntFromIdentifier(NPIdentifier identifier) = 0;

	virtual bool IdentifierIsString(NPIdentifier identifier) = 0;

	virtual NPUTF8* UTF8FromIdentifier(NPIdentifier identifier) = 0;

	virtual NPObject* RetainObject(NPObject* obj) = 0;

	virtual void ReleaseObject(NPObject* obj) = 0;

	virtual void SetException(NPObject* obj, const NPUTF8* message) = 0;

	void pGetNPNetscapeFuncs(NPNetscapeFuncs& oNPNetscapeFuncs);

	static Host* sHostFromNPP(NPP iNPP);
	static Host* sHostFromStream(NPStream* iNPStream);

// HostMeister-implemented
	static void* sMemAlloc(uint32 size);

	static void sMemFree(void* ptr);

	static uint32 sMemFlush(uint32 size);

	static void sReloadPlugins(NPBool reloadPages);

	static void* sGetJavaEnv();

	static void sReleaseVariantValue(NPVariant* variant);

	static NPIdentifier sGetStringIdentifier(const NPUTF8* name);

	static void sGetStringIdentifiers(
		const NPUTF8** names, int32_t nameCount, NPIdentifier* identifiers);

	static NPIdentifier sGetIntIdentifier(int32_t intid);

	static int32_t sIntFromIdentifier(NPIdentifier identifier);

	static bool sIdentifierIsString(NPIdentifier identifier);

	static NPUTF8* sUTF8FromIdentifier(NPIdentifier identifier);

	static NPObject* sRetainObject(NPObject* obj);

	static void sReleaseObject(NPObject* obj);

	static void sSetException(NPObject* obj, const NPUTF8* message);

// Forwarded to Host
	static NPError sGetURLNotify(NPP npp,
		const char* URL, const char* window, void* notifyData);

	static NPError sPostURLNotify(NPP npp,
		const char* URL, const char* window,
		uint32 len, const char* buf, NPBool file, void* notifyData);

	static NPError sRequestRead(NPStream* stream, NPByteRange* rangeList);

	static NPError sNewStream(NPP npp,
		NPMIMEType type, const char* window, NPStream** stream);

	static int32 sWrite(NPP npp, NPStream* stream, int32 len, void* buffer);

	static NPError sDestroyStream(NPP npp, NPStream* stream, NPReason reason);

	static void sStatus(NPP npp, const char* message);

	static const char* sUserAgent(NPP npp);

	static NPError sGetValue(NPP npp, NPNVariable variable, void* ret_value);

	static NPError sSetValue(NPP npp, NPPVariable variable, void* value);

	static void sInvalidateRect(NPP npp, NPRect* rect);

	static void sInvalidateRegion(NPP npp, NPRegion region);

	static void sForceRedraw(NPP npp);

	static NPError sGetURL(NPP npp, const char* URL, const char* window);

	static NPError sPostURL(NPP npp,
		const char* URL, const char* window, uint32 len, const char* buf, NPBool file);

	static void* sGetJavaPeer(NPP npp);

	static NPObject* sCreateObject(NPP npp, NPClass* aClass);

	static bool sInvoke(NPP npp,
		NPObject* obj, NPIdentifier methodName, const NPVariant* args, unsigned argCount,
		NPVariant* result);

	static bool sInvokeDefault(NPP npp,
		NPObject* obj, const NPVariant* args, unsigned argCount, NPVariant* result);

	static bool sEvaluate(NPP npp,
		NPObject* obj, NPString* script, NPVariant* result);

	static bool sGetProperty(NPP npp,
		NPObject* obj, NPIdentifier propertyName, NPVariant* result);

	static bool sSetProperty(NPP npp,
		NPObject* obj, NPIdentifier propertyName, const NPVariant* value);

	static bool sHasProperty(NPP npp, NPObject* npobj, NPIdentifier propertyName);

	static bool sHasMethod(NPP npp, NPObject* npobj, NPIdentifier methodName);

	static bool sRemoveProperty(NPP npp, NPObject* obj, NPIdentifier propertyName);
	};

// =================================================================================================
#pragma mark -
#pragma mark * Host

class Host
	{
protected:
	Host(ZRef<GuestFactory> iGuestFactory);
	virtual ~Host();

public:
	void Create(const std::string& iURL, const std::string& iMIME);
	void Destroy();

	void SendDataAsync(
		void* iNotifyData,
		const std::string& iURL, const std::string& iMIME, const ZMemoryBlock& iHeaders,
		ZRef<ZStreamerRCon> iStreamerRCon);

	void SendDataSync(
		void* iNotifyData,
		const std::string& iURL, const std::string& iMIME,
		const ZStreamR& iStreamR);

	void HostActivate(bool iActivate);
	void HostIdle();
	void HostDeliverData();
	void HostDraw();
	void HostSetWindow(CGrafPtr iGrafPtr,
		ZooLib::ZPoint iLocation, ZooLib::ZPoint iSize, const ZooLib::ZRect& iClip);
	void HostSetBounds(ZooLib::ZPoint iLocation, ZooLib::ZPoint iSize, const ZooLib::ZRect& iClip);

	bool HostInvoke(
		NPObject* obj, const std::string& iMethod, const NPVariant* iArgs, size_t iCount,
		NPVariant& oResult);

	NPObject* GetScriptableNPObject();

	static std::string sAsString(NPNVariable iVar);
	static std::string sAsString(NPPVariable iVar);

// Host indirect API
	virtual void HostURLNotify(const char* URL, NPReason reason, void* notifyData);
	virtual NPError HostNewStream(NPMIMEType type, NPStream* stream, NPBool seekable, uint16* stype);
	virtual NPError HostDestroyStream(NPStream* stream, NPReason reason);
	virtual int32 HostWriteReady(NPStream* stream);
	virtual int32 HostWrite(NPStream* stream, int32_t offset, int32_t len, void* buffer);

// Host API
	virtual NPError GetURLNotify(NPP npp,
		const char* URL, const char* window, void* notifyData) = 0;

	virtual NPError PostURLNotify(NPP npp,
		const char* URL, const char* window,
		uint32 len, const char* buf, NPBool file, void* notifyData) = 0;

	virtual NPError RequestRead(NPStream* stream, NPByteRange* rangeList) = 0;

	virtual NPError NewStream(NPP npp,
		NPMIMEType type, const char* window, NPStream** stream) = 0;

	virtual int32 Write(NPP npp, NPStream* stream, int32 len, void* buffer) = 0;

	virtual NPError DestroyStream(NPP npp, NPStream* stream, NPReason reason) = 0;

	virtual void Status(NPP npp, const char* message) = 0;

	virtual const char* UserAgent(NPP npp) = 0;

	virtual NPError GetValue(NPP npp, NPNVariable variable, void* ret_value) = 0;

	virtual NPError SetValue(NPP npp, NPPVariable variable, void* value) = 0;

	virtual void InvalidateRect(NPP npp, NPRect* rect) = 0;

	virtual void InvalidateRegion(NPP npp, NPRegion region) = 0;

	virtual void ForceRedraw(NPP npp) = 0;

	virtual NPError GetURL(NPP npp, const char* URL, const char* window) = 0;

	virtual NPError PostURL(NPP npp,
		const char* URL, const char* window, uint32 len, const char* buf, NPBool file) = 0;

	virtual void* GetJavaPeer(NPP npp) = 0;

	virtual NPObject* CreateObject(NPP npp, NPClass* aClass) = 0;

	virtual bool Invoke(NPP npp,
		NPObject* obj, NPIdentifier methodName, const NPVariant* args, unsigned argCount,
		NPVariant* result) = 0;

	virtual bool InvokeDefault(NPP npp,
		NPObject* obj, const NPVariant* args, unsigned argCount, NPVariant* result) = 0;

	virtual bool Evaluate(NPP npp,
		NPObject* obj, NPString* script, NPVariant* result) = 0;

	virtual bool GetProperty(NPP npp,
		NPObject* obj, NPIdentifier propertyName, NPVariant* result) = 0;

	virtual bool SetProperty(NPP npp,
		NPObject* obj, NPIdentifier propertyName, const NPVariant* value) = 0;

	virtual bool HasProperty(NPP, NPObject* npobj, NPIdentifier propertyName) = 0;

	virtual bool HasMethod(NPP npp, NPObject* npobj, NPIdentifier methodName) = 0;

	virtual bool RemoveProperty(NPP npp, NPObject* obj, NPIdentifier propertyName) = 0;

// Plugin API

private:
	void pDeliverData();

	class Sender;
	ZooLib::ZMutex fMutex;
	std::list<Sender*> fSenders;

	ZRef<GuestFactory> fGuestFactory;
	NPPluginFuncs fNPPluginFuncs;
protected:
	NPP_t fNPP_t;
private:
	NPWindow fNPWindow;
	NP_Port fNP_Port;
	NPObject* fNPObject;
	};

// =================================================================================================
#pragma mark -
#pragma mark * GuestFactory

class GuestFactory : public ZRefCountedWithFinalization
	{
protected:
	GuestFactory();

public:
	virtual ~GuestFactory();

	void GetNPNetscapeFuncs(NPNetscapeFuncs& oNPNetscapeFuncs);

	virtual void GetEntryPoints(NPPluginFuncs& oNPPluginFuncs) = 0;
	};

} // namespace ZNetscape

#endif // __ZNetscape_Host__
