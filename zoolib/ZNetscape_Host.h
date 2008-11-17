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

#include "zoolib/ZCompat_npapi.h"
#include "zoolib/ZGeom.h"
#include "zoolib/ZMemoryBlock.h"
#include "zoolib/ZRefCount.h"
#include "zoolib/ZStream.h"
#include "zoolib/ZStreamer.h"
#include "zoolib/ZThread.h"

#include <list>

namespace ZNetscape {

void sRetainH(NPObject* iObject);
void sReleaseH(NPObject* iObject);

// =================================================================================================
#pragma mark -
#pragma mark * NPVariantH

class NPVariantH : public NPVariant
	{
public:
	NPVariantH();
	NPVariantH(const NPVariant& iOther);
	~NPVariantH();
	NPVariantH& operator=(const NPVariant& iOther);

	NPVariantH(bool iValue);
	NPVariantH(int32 iValue);
	NPVariantH(double iValue);
	NPVariantH(const std::string& iValue);
	NPVariantH(NPObject* iValue);

	NPVariantH& operator=(bool iValue);
	NPVariantH& operator=(int32 iValue);
	NPVariantH& operator=(double iValue);
	NPVariantH& operator=(const std::string& iValue);
	NPVariantH& operator=(NPObject* iValue);

	bool IsVoid() const;
	bool IsNull() const;

	bool IsBool() const;
	bool IsInt32() const;
	bool IsDouble() const;
	bool IsString() const;
	bool IsObject() const;

	void SetVoid();
	void SetNull();

	bool GetBool() const;
	bool GetBool(bool& oValue) const;
	bool DGetBool(bool iDefault) const;
	void SetBool(bool iValue);

	int32 GetInt32() const;
	bool GetInt32(int32& oValue) const;
	int32 DGetInt32(int32 iDefault) const;
	void SetInt32(int32 iValue);

	double GetDouble() const;
	bool GetDouble(double& oValue) const;
	double DGetDouble(double iDefault) const;
	void SetDouble(bool iValue);

	std::string GetString() const;
	bool GetString(std::string& oValue) const;
	std::string DGetString(const std::string& iDefault) const;
	void SetString(const std::string& iValue);

	NPObject* GetObject() const;
	bool GetObject(NPObject*& oValue) const;
	NPObject* DGetObject(NPObject* iDefault) const;
	void SetObject(NPObject* iValue);

private:
	void pRelease();
	void pRetain(NPObject* iObject) const;
	void pCopyFrom(const NPVariant& iOther);
	};

// =================================================================================================
#pragma mark -
#pragma mark * NPObjectH

class NPObjectH : public NPObject
	{
protected:
	NPObjectH();
	~NPObjectH();

public:
	bool HasMethod(const std::string& iName);
	bool Invoke(
		const std::string& iName, const NPVariantH* iArgs, size_t iCount, NPVariantH& oResult);
	bool InvokeDefault(const NPVariantH* iArgs, size_t iCount, NPVariantH& oResult);

	bool HasProperty(const std::string& iName);
	bool GetProperty(const std::string& iName, NPVariantH& oResult);
	bool SetProperty(const std::string& iName, const NPVariantH& iValue);
	bool RemoveProperty(const std::string& iName);

	static std::string sAsString(NPIdentifier iNPI);
	static NPIdentifier sAsNPI(const std::string& iName);
	};

// =================================================================================================
#pragma mark -
#pragma mark * ObjectH

class ObjectH : public NPObjectH
	{
protected:
	ObjectH();
	virtual ~ObjectH();

	virtual void Imp_Invalidate();

	virtual bool Imp_HasMethod(const std::string& iName);
	virtual bool Imp_Invoke(
		const std::string& iName, const NPVariantH* iArgs, size_t iCount, NPVariantH& oResult);
	virtual bool Imp_InvokeDefault(const NPVariantH* iArgs, size_t iCount, NPVariantH& oResult);

	virtual bool Imp_HasProperty(const std::string& iName);
	virtual bool Imp_GetProperty(const std::string& iName, NPVariantH& oResult);
	virtual bool Imp_SetProperty(const std::string& iName, const NPVariantH& iValue);
	virtual bool Imp_RemoveProperty(const std::string& iName);

private:
	static NPObject* sAllocate(NPP npp, NPClass *aClass);
	static void sDeallocate(NPObject* npobj);
	static void sInvalidate(NPObject* npobj);

	static bool sHasMethod(NPObject* npobj, NPIdentifier name);

	static bool sInvoke(NPObject* npobj,
		NPIdentifier name, const NPVariant* args, uint32_t argCount, NPVariant* result);

	static bool sInvokeDefault(NPObject* npobj,
		const NPVariant* args, uint32_t argCount, NPVariant* result);

	static bool sHasProperty(NPObject*  npobj, NPIdentifier name);
	static bool sGetProperty(NPObject* npobj, NPIdentifier name, NPVariant* result);
	static bool sSetProperty(NPObject* npobj, NPIdentifier name, const NPVariant* value);
	static bool sRemoveProperty(NPObject* npobj, NPIdentifier name);

	static NPClass sNPClass;
	};

// =================================================================================================
#pragma mark -
#pragma mark * HostMeister

class Host;

class HostMeister
	{
public:
	static HostMeister* sGet();

	static Host* sHostFromNPP(NPP iNPP);
	static Host* sHostFromStream(NPStream* iNPStream);
	static void sGetNPNF(NPNetscapeFuncs& oNPNF);

	HostMeister();
	virtual ~HostMeister();

	std::string StringFromIdentifier(NPIdentifier identifier);

	virtual void* MemAlloc(uint32 size) = 0;

	virtual void MemFree(void* ptr) = 0;

	virtual uint32 MemFlush(uint32 size) = 0;

	virtual void ReloadPlugins(NPBool reloadPages) = 0;

	virtual void* GetJavaEnv() = 0;

	virtual NPIdentifier GetStringIdentifier(const NPUTF8* name) = 0;

	virtual void GetStringIdentifiers(
		const NPUTF8* *names, int32_t nameCount, NPIdentifier* identifiers) = 0;

	virtual NPIdentifier GetIntIdentifier(int32_t intid) = 0;

	virtual bool IdentifierIsString(NPIdentifier identifier) = 0;

	virtual NPUTF8* UTF8FromIdentifier(NPIdentifier identifier) = 0;

	virtual int32_t IntFromIdentifier(NPIdentifier identifier) = 0;

	virtual NPObject* RetainObject(NPObject* obj) = 0;

	virtual void ReleaseObject(NPObject* obj) = 0;

	virtual void ReleaseVariantValue(NPVariant* variant) = 0;

	virtual void SetException(NPObject* obj, const NPUTF8* message) = 0;

private:
// Forwarded to HostMeister
	static void* sMemAlloc(uint32 size);

	static void sMemFree(void* ptr);

	static uint32 sMemFlush(uint32 size);

	static void sReloadPlugins(NPBool reloadPages);

	static void* sGetJavaEnv();

	static NPIdentifier sGetStringIdentifier(const NPUTF8* name);

	static void sGetStringIdentifiers(
		const NPUTF8** names, int32_t nameCount, NPIdentifier* identifiers);

	static NPIdentifier sGetIntIdentifier(int32_t intid);

	static bool sIdentifierIsString(NPIdentifier identifier);

	static NPUTF8* sUTF8FromIdentifier(NPIdentifier identifier);

	static int32_t sIntFromIdentifier(NPIdentifier identifier);

	static NPObject* sRetainObject(NPObject* obj);

	static void sReleaseObject(NPObject* obj);

	static void sReleaseVariantValue(NPVariant* variant);

	static void sSetException(NPObject* obj, const NPUTF8* message);

// Forwarded to Host
	static NPError sGetURL(NPP npp, const char* URL, const char* window);

	static NPError sPostURL(NPP npp,
		const char* URL, const char* window, uint32 len, const char* buf, NPBool file);

	static NPError sRequestRead(NPStream* stream, NPByteRange* rangeList);

	static NPError sNewStream(NPP npp,
		NPMIMEType type, const char* window, NPStream** stream);

	static int32 sWrite(NPP npp, NPStream* stream, int32 len, void* buffer);

	static NPError sDestroyStream(NPP npp, NPStream* stream, NPReason reason);

	static void sStatus(NPP npp, const char* message);

	static const char* sUserAgent(NPP npp);

	static void* sGetJavaPeer(NPP npp);

	static NPError sGetURLNotify(NPP npp,
		const char* URL, const char* window, void* notifyData);

	static NPError sPostURLNotify(NPP npp,
		const char* URL, const char* window,
		uint32 len, const char* buf, NPBool file, void* notifyData);

	static NPError sGetValue(NPP npp, NPNVariable variable, void* ret_value);

	static NPError sSetValue(NPP npp, NPPVariable variable, void* value);

	static void sInvalidateRect(NPP npp, NPRect* rect);

	static void sInvalidateRegion(NPP npp, NPRegion region);

	static void sForceRedraw(NPP npp);

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

	static bool sRemoveProperty(NPP npp, NPObject* obj, NPIdentifier propertyName);

	static bool sHasProperty(NPP npp, NPObject* npobj, NPIdentifier propertyName);

	static bool sHasMethod(NPP npp, NPObject* npobj, NPIdentifier methodName);
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

	void GetNPNF(NPNetscapeFuncs& oNPNF);

	virtual void GetEntryPoints(NPPluginFuncs& oNPPluginFuncs) = 0;
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
	void Release(NPObject* iObj);

	bool Invoke(
		NPObject* obj, const std::string& iMethod, const NPVariant* iArgs, size_t iCount,
		NPVariant& oResult);

	void Create(const std::string& iURL, const std::string& iMIME);
	void Guest_Destroy();

	void SendDataAsync(
		void* iNotifyData,
		const std::string& iURL, const std::string& iMIME, const ZMemoryBlock& iHeaders,
		ZRef<ZStreamerRCon> iStreamerRCon);

	void SendDataSync(
		void* iNotifyData,
		const std::string& iURL, const std::string& iMIME,
		const ZStreamR& iStreamR);

	void DeliverData();

	void Guest_Activate(bool iActivate);
	void Guest_Event(const EventRecord& iEvent);
	void Guest_Idle();
	void Guest_Draw();

	void Guest_SetWindow(CGrafPtr iGrafPtr,
		ZooLib::ZPoint iLocation, ZooLib::ZPoint iSize, const ZooLib::ZRect& iClip);

	void Guest_SetBounds(
		ZooLib::ZPoint iLocation, ZooLib::ZPoint iSize, const ZooLib::ZRect& iClip);

	NPObject* Guest_GetScriptableNPObject();

// Calls into the guest
	virtual void Guest_URLNotify(const char* URL, NPReason reason, void* notifyData);
	virtual NPError Guest_NewStream(
		NPMIMEType type, NPStream* stream, NPBool seekable, uint16* stype);
	virtual NPError Guest_DestroyStream(NPStream* stream, NPReason reason);
	virtual int32 Guest_WriteReady(NPStream* stream);
	virtual int32 Guest_Write(NPStream* stream, int32_t offset, int32_t len, void* buffer);

// Our protocol
	virtual NPError Host_GetURL(NPP npp, const char* URL, const char* window) = 0;

	virtual NPError Host_PostURL(NPP npp,
		const char* URL, const char* window, uint32 len, const char* buf, NPBool file) = 0;

	virtual NPError Host_RequestRead(NPStream* stream, NPByteRange* rangeList) = 0;

	virtual NPError Host_NewStream(NPP npp,
		NPMIMEType type, const char* window, NPStream** stream) = 0;

	virtual int32 Host_Write(NPP npp, NPStream* stream, int32 len, void* buffer) = 0;

	virtual NPError Host_DestroyStream(NPP npp, NPStream* stream, NPReason reason) = 0;

	virtual void Host_Status(NPP npp, const char* message) = 0;

	virtual const char* Host_UserAgent(NPP npp) = 0;

	virtual void* Host_GetJavaPeer(NPP npp) = 0;

	virtual NPError Host_GetURLNotify(NPP npp,
		const char* URL, const char* window, void* notifyData) = 0;

	virtual NPError Host_PostURLNotify(NPP npp,
		const char* URL, const char* window,
		uint32 len, const char* buf, NPBool file, void* notifyData) = 0;

	virtual NPError Host_GetValue(NPP npp, NPNVariable variable, void* ret_value) = 0;

	virtual NPError Host_SetValue(NPP npp, NPPVariable variable, void* value) = 0;

	virtual void Host_InvalidateRect(NPP npp, NPRect* rect) = 0;

	virtual void Host_InvalidateRegion(NPP npp, NPRegion region) = 0;

	virtual void Host_ForceRedraw(NPP npp) = 0;

	virtual NPObject* Host_CreateObject(NPP npp, NPClass* aClass) = 0;

	virtual bool Host_Invoke(NPP npp,
		NPObject* obj, NPIdentifier methodName, const NPVariant* args, unsigned argCount,
		NPVariant* result) = 0;

	virtual bool Host_InvokeDefault(NPP npp,
		NPObject* obj, const NPVariant* args, unsigned argCount, NPVariant* result) = 0;

	virtual bool Host_Evaluate(NPP npp,
		NPObject* obj, NPString* script, NPVariant* result) = 0;

	virtual bool Host_GetProperty(NPP npp,
		NPObject* obj, NPIdentifier propertyName, NPVariant* result) = 0;

	virtual bool Host_SetProperty(NPP npp,
		NPObject* obj, NPIdentifier propertyName, const NPVariant* value) = 0;

	virtual bool Host_RemoveProperty(NPP npp, NPObject* obj, NPIdentifier propertyName) = 0;

	virtual bool Host_HasProperty(NPP, NPObject* npobj, NPIdentifier propertyName) = 0;

	virtual bool Host_HasMethod(NPP npp, NPObject* npobj, NPIdentifier methodName) = 0;

private:
	class Sender;
	ZooLib::ZMutex fMutex;
	std::list<Sender*> fSenders;

	ZRef<GuestFactory> fGuestFactory;
	NPPluginFuncs fNPPluginFuncs;

private:
	NPP_t fNPP_t;
	NPWindow fNPWindow;
	NP_Port fNP_Port;
	};

} // namespace ZNetscape

#endif // __ZNetscape_Host__
