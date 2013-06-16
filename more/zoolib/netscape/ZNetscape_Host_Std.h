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

#ifndef __ZNetscape_Host_Std_h__
#define __ZNetscape_Host_Std_h__ 1
#include "zconfig.h"

#include "zoolib/netscape/ZNetscape_Host.h"
#include "zoolib/ZSafeSet.h"
#include "zoolib/ZStreamer.h"
#include "zoolib/ZData_Any.h"

#include <set>

namespace ZooLib {
namespace ZNetscape {

class Host_Std;

// =================================================================================================
// MARK: - HostMeister_Std

class HostMeister_Std : public HostMeister
	{
public:
	HostMeister_Std();
	virtual ~HostMeister_Std();

	static Host_Std* sHostFromNPP_Std(NPP npp);
	static Host_Std* sHostFromStream_Std(NPStream* iNPStream);

// From HostMeister
	virtual NPError GetURL(NPP npp, const char* URL, const char* window);

	virtual NPError PostURL(NPP npp,
		const char* URL, const char* window, uint32 len, const char* buf, NPBool file);

	virtual NPError RequestRead(NPStream* stream, NPByteRange* rangeList);

	virtual NPError NewStream(NPP npp,
		NPMIMEType type, const char* window, NPStream** stream);

	virtual int32 Write(NPP npp, NPStream* stream, int32 len, void* buffer);

	virtual NPError DestroyStream(NPP npp, NPStream* stream, NPReason reason);

	virtual void Status(NPP npp, const char* message);

	virtual const char* UserAgent(NPP npp);

	virtual void* MemAlloc(uint32 size);

	virtual void MemFree(void* ptr);

	virtual uint32 MemFlush(uint32 size);

	virtual void ReloadPlugins(NPBool reloadPages);

	virtual void* GetJavaEnv();

	virtual void* GetJavaPeer(NPP npp);

	virtual NPError GetURLNotify(NPP npp,
		const char* URL, const char* window, void* notifyData);

	virtual NPError PostURLNotify(NPP npp,
		const char* URL, const char* window,
		uint32 len, const char* buf, NPBool file, void* notifyData);

	virtual NPError GetValue(NPP npp, NPNVariable variable, void* ret_value);

	virtual NPError SetValue(NPP npp, NPPVariable variable, void* value);

	virtual void InvalidateRect(NPP npp, NPRect* rect);

	virtual void InvalidateRegion(NPP npp, NPRegion region);

	virtual void ForceRedraw(NPP npp);

	virtual NPIdentifier GetStringIdentifier(const NPUTF8* name);

	virtual void GetStringIdentifiers(
		const NPUTF8* *names, int32 nameCount, NPIdentifier* identifiers);

	virtual NPIdentifier GetIntIdentifier(int32 intid);

	virtual bool IdentifierIsString(NPIdentifier identifier);

	virtual NPUTF8* UTF8FromIdentifier(NPIdentifier identifier);

	virtual int32 IntFromIdentifier(NPIdentifier identifier);

	virtual NPObject* CreateObject(NPP npp, NPClass* aClass);

	virtual NPObject* RetainObject(NPObject* obj);

	virtual void ReleaseObject(NPObject* obj);

	virtual bool Invoke(NPP npp,
		NPObject* obj, NPIdentifier methodName, const NPVariant* args, uint32 argCount,
		NPVariant* result);

	virtual bool InvokeDefault(NPP npp,
		NPObject* obj, const NPVariant* args, uint32 argCount, NPVariant* result);

	virtual bool Evaluate(NPP npp,
		NPObject* obj, NPString* script, NPVariant* result);

	virtual bool GetProperty(NPP npp,
		NPObject* obj, NPIdentifier propertyName, NPVariant* result);

	virtual bool SetProperty(NPP npp,
		NPObject* obj, NPIdentifier propertyName, const NPVariant* value);

	virtual bool RemoveProperty(NPP npp, NPObject* obj, NPIdentifier propertyName);

	virtual bool HasProperty(NPP, NPObject* npobj, NPIdentifier propertyName);

	virtual bool HasMethod(NPP npp, NPObject* npobj, NPIdentifier methodName);

	virtual void ReleaseVariantValue(NPVariant* variant);

	virtual void SetException(NPObject* obj, const NPUTF8* message);

	virtual void PushPopupsEnabledState(NPP npp, NPBool enabled);

	virtual void PopPopupsEnabledState(NPP npp);

	virtual bool Enumerate(
		NPP npp, NPObject *npobj, NPIdentifier **identifier, uint32 *count);

	virtual void PluginThreadAsyncCall(
		NPP npp, void (*func)(void *), void *userData);

	virtual bool Construct(
		NPP npp, NPObject* obj, const NPVariant *args, uint32 argCount, NPVariant *result);

	virtual uint32 ScheduleTimer(
		NPP npp, uint32 interval, NPBool repeat, void (*timerFunc)(NPP npp, uint32 timerID));

	virtual void UnscheduleTimer(NPP npp, uint32 timerID);

private:
	std::set<std::string> fStrings;
	};

// =================================================================================================
// MARK: - Host_Std

class Host_Std : public Host
	{
public:
	Host_Std(ZRef<GuestFactory> iGuestFactory);
	virtual ~Host_Std();

// Our protocol
	virtual NPError Host_GetURL(NPP npp, const char* URL, const char* window);

	virtual NPError Host_PostURL(NPP npp,
		const char* URL, const char* window, uint32 len, const char* buf, NPBool file);

	virtual NPError Host_RequestRead(NPStream* stream, NPByteRange* rangeList);

	virtual NPError Host_NewStream(NPP npp,
		NPMIMEType type, const char* window, NPStream** stream);

	virtual int32 Host_Write(NPP npp, NPStream* stream, int32 len, void* buffer);

	virtual NPError Host_DestroyStream(NPP npp, NPStream* stream, NPReason reason);

	virtual void Host_Status(NPP npp, const char* message);

// The defining call to UserAgent generally occurs before we've got a Host instance to
// handle it. If you need to modify the UA response, do so in your HostMeister.
//	virtual const char* Host_UserAgent(NPP npp);

	virtual void* Host_GetJavaPeer(NPP npp);

	virtual NPError Host_GetURLNotify(NPP npp,
		const char* URL, const char* window, void* notifyData);

	virtual NPError Host_PostURLNotify(NPP npp,
		const char* URL, const char* window,
		uint32 len, const char* buf, NPBool file, void* notifyData);

	virtual NPError Host_GetValue(NPP npp, NPNVariable variable, void* ret_value);

	virtual NPError Host_SetValue(NPP npp, NPPVariable variable, void* value);

	virtual void Host_InvalidateRect(NPP npp, NPRect* rect);

	virtual void Host_InvalidateRegion(NPP npp, NPRegion region);

	virtual void Host_ForceRedraw(NPP npp);

	virtual bool Host_Evaluate(NPP npp,
		NPObject* obj, NPString* script, NPVariant* result);

	virtual uint32 Host_ScheduleTimer(
		NPP npp, uint32 interval, NPBool repeat, void (*timerFunc)(NPP npp, uint32 timerID));

	virtual void Host_UnscheduleTimer(NPP npp, uint32 timerID);

// Our protocol
	typedef std::pair<std::string, std::string> Param_t;
	void CreateAndLoad(
		const std::string& iURL, const std::string& iMIME,
		const Param_t* iParams, size_t iCount);
	virtual void PostCreateAndLoad();
	void Destroy();

	void SendDataAsync(
		void* iNotifyData,
		const std::string& iURL, const std::string& iMIME, const ZData_Any& iHeaders,
		ZRef<ZStreamerR> iStreamerR);

	void SendDataSync(
		void* iNotifyData,
		const std::string& iURL, const std::string& iMIME,
		const ZStreamR& iStreamR);

	void DeliverData();

	virtual ZRef<NPObjectH> Host_GetWindowObject();
	virtual ZRef<NPObjectH> Host_GetPluginObject();

	ZRef<NPObjectH> Guest_GetNPObject();

protected:
	NPWindow fNPWindow;
	std::string fURL;

private:
	class HTTPFetcher;
	friend class HTTPFetcher;

	void pHTTPFetcher(ZRef<HTTPFetcher> iHTTPFetcher, void* iNotifyData,
		const std::string& iURL, const std::string& iMIME, const ZData_Any& iHeaders,
		ZRef<ZStreamerR> iStreamerR);

	ZSafeSet<ZRef<HTTPFetcher> > fHTTPFetchers;

	class Sender;
	ZSafeSet<Sender*> fSenders;
	};

} // namespace ZNetscape
} // namespace ZooLib

#endif // __ZNetscape_Host_Std_h__
