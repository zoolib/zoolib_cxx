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

#ifndef __ZNetscape_Host_h__
#define __ZNetscape_Host_h__ 1
#include "zconfig.h"

#include "zoolib/netscape/ZNetscape.h"
#include "zoolib/netscape/ZNetscape_Object.h"
#include "zoolib/netscape/ZNetscape_Variant.h"

#include "zoolib/ZCounted.h"
#include "zoolib/ZDebug.h" // For ZAssert
#include "zoolib/ZValAccessors.h"

#include <string>

namespace ZooLib {
namespace ZNetscape {

using std::string;
using ::uint32;
class NPObjectH;

// =================================================================================================
// MARK: - NPVariantH

typedef NPVariant_T<NPObjectH> NPVariantH;

template <> void sVariantRelease_T(NPVariantH& iNPVariantH);
template <> void* sMalloc_T(NPVariantH&, size_t iLength);
template <> void sFree_T<NPVariantH>(void* iPtr);

// =================================================================================================
// MARK: - NPObjectH

void sRetain(NPObjectH& iOb);
void sRelease(NPObjectH& iOb);
inline void sCheck(NPObjectH* iOb)
	{}

class NPObjectH
:	public NPObject_T<NPVariantH>
	{
protected:
	NPObjectH();

public:
	static bool sIsString(NPIdentifier iNPI);
	static string sAsString(NPIdentifier iNPI);
	static int32 sAsInt(NPIdentifier iNPI);

	static NPIdentifier sAsNPI(const string& iName);
	static NPIdentifier sAsNPI(int32 iInt);

	void Retain();
	void Release();

	bool HasMethod(const string& iName);

	using Base_t::Invoke;
	bool Invoke(
		const string& iName, const NPVariantH* iArgs, size_t iCount, NPVariantH& oResult);

	using Base_t::InvokeDefault;
	bool InvokeDefault(const NPVariantH* iArgs, size_t iCount, NPVariantH& oResult);

	bool HasProperty(const string& iName);
	bool HasProperty(size_t iIndex);

	bool GetProperty(const string& iName, NPVariantH& oResult);
	bool GetProperty(size_t iIndex, NPVariantH& oResult);

	bool SetProperty(const string& iName, const NPVariantH& iValue);
	bool SetProperty(size_t iIndex, const NPVariantH& iValue);

	bool RemoveProperty(const string& iName);
	bool RemoveProperty(size_t iIndex);

	bool Enumerate(NPIdentifier*& oIdentifiers, uint32& oCount);
	};

template <>
const ZQ<ZRef<NPObjectH> > NPVariantBase::QGet<ZRef<NPObjectH> >() const;

// =================================================================================================
// MARK: - ObjectH

class ObjectH : public NPObjectH
	{
protected:
	ObjectH();
	virtual ~ObjectH();

	virtual void Imp_Invalidate();
	virtual bool Imp_HasMethod(const string& iName);

	virtual bool Imp_Invoke(
		const string& iName, const NPVariantH* iArgs, size_t iCount, NPVariantH& oResult);

	virtual bool Imp_InvokeDefault(const NPVariantH* iArgs, size_t iCount, NPVariantH& oResult);
	virtual bool Imp_HasProperty(const string& iName);
	virtual bool Imp_HasProperty(int32 iInt);
	virtual bool Imp_GetProperty(const string& iName, NPVariantH& oResult);
	virtual bool Imp_GetProperty(int32 iInt, NPVariantH& oResult);
	virtual bool Imp_SetProperty(const string& iName, const NPVariantH& iValue);
	virtual bool Imp_SetProperty(int32 iInt, const NPVariantH& iValue);
	virtual bool Imp_RemoveProperty(const string& iName);
	virtual bool Imp_RemoveProperty(int32 iInt);
	virtual bool Imp_Enumerate(NPIdentifier*& oIDs, uint32& oCount);
	virtual bool Imp_Enumerate(std::vector<string>& oNames);

private:
	static NPObject* spAllocate(NPP npp, NPClass *aClass);
	static void spDeallocate(NPObject* npobj);
	static void spInvalidate(NPObject* npobj);
	static bool spHasMethod(NPObject* npobj, NPIdentifier name);

	static bool spInvoke(NPObject* npobj,
		NPIdentifier name, const NPVariant* args, unsigned argCount, NPVariant* result);

	static bool spInvokeDefault(NPObject* npobj,
		const NPVariant* args, unsigned argCount, NPVariant* result);

	static bool spHasProperty(NPObject* npobj, NPIdentifier name);
	static bool spGetProperty(NPObject* npobj, NPIdentifier name, NPVariant* result);
	static bool spSetProperty(NPObject* npobj, NPIdentifier name, const NPVariant* value);
	static bool spRemoveProperty(NPObject* npobj, NPIdentifier name);
	static bool spEnumerate(NPObject* npobj, NPIdentifier** oIdentifiers, uint32_t* oCount);

	static NPClass_Z spNPClass;
	};

// =================================================================================================
// MARK: - HostMeister

class Host;

class HostMeister
	{
public:
	static HostMeister* sGet();

	static Host* sHostFromNPP(NPP npp);
	static Host* sHostFromStream(NPStream* iNPStream);
	static void sGetNPNF(NPNetscapeFuncs_Z& oNPNF);

	HostMeister();
	virtual ~HostMeister();

	string StringFromIdentifier(NPIdentifier identifier);

	virtual NPError GetURL(NPP npp, const char* URL, const char* window) = 0;

	virtual NPError PostURL(NPP npp,
		const char* URL, const char* window, uint32 len, const char* buf, NPBool file) = 0;

	virtual NPError RequestRead(NPStream* stream, NPByteRange* rangeList) = 0;

	virtual NPError NewStream(NPP npp,
		NPMIMEType type, const char* window, NPStream** stream) = 0;

	virtual int32 Write(NPP npp, NPStream* stream, int32 len, void* buffer) = 0;

	virtual NPError DestroyStream(NPP npp, NPStream* stream, NPReason reason) = 0;

	virtual void Status(NPP npp, const char* message) = 0;

	virtual const char* UserAgent(NPP npp) = 0;

	virtual void* MemAlloc(uint32 size) = 0;

	virtual void MemFree(void* ptr) = 0;

	virtual uint32 MemFlush(uint32 size) = 0;

	virtual void ReloadPlugins(NPBool reloadPages) = 0;

	virtual void* GetJavaEnv() = 0;

	virtual void* GetJavaPeer(NPP npp) = 0;

	virtual NPError GetURLNotify(NPP npp,
		const char* URL, const char* window, void* notifyData) = 0;

	virtual NPError PostURLNotify(NPP npp,
		const char* URL, const char* window,
		uint32 len, const char* buf, NPBool file, void* notifyData) = 0;

	virtual NPError GetValue(NPP npp, NPNVariable variable, void* ret_value) = 0;

	virtual NPError SetValue(NPP npp, NPPVariable variable, void* value) = 0;

	virtual void InvalidateRect(NPP npp, NPRect* rect) = 0;

	virtual void InvalidateRegion(NPP npp, NPRegion region) = 0;

	virtual void ForceRedraw(NPP npp) = 0;

	virtual NPIdentifier GetStringIdentifier(const NPUTF8* name) = 0;

	virtual void GetStringIdentifiers(
		const NPUTF8* *names, int32 nameCount, NPIdentifier* identifiers) = 0;

	virtual NPIdentifier GetIntIdentifier(int32 intid) = 0;

	virtual bool IdentifierIsString(NPIdentifier identifier) = 0;

	virtual NPUTF8* UTF8FromIdentifier(NPIdentifier identifier) = 0;

	virtual int32 IntFromIdentifier(NPIdentifier identifier) = 0;

	virtual NPObject* CreateObject(NPP npp, NPClass* aClass) = 0;

	virtual NPObject* RetainObject(NPObject* obj) = 0;

	virtual void ReleaseObject(NPObject* obj) = 0;

	virtual bool Invoke(NPP npp,
		NPObject* obj, NPIdentifier methodName, const NPVariant* args, uint32 argCount,
		NPVariant* result) = 0;

	virtual bool InvokeDefault(NPP npp,
		NPObject* obj, const NPVariant* args, uint32 argCount, NPVariant* result) = 0;

	virtual bool Evaluate(NPP npp,
		NPObject* obj, NPString* script, NPVariant* result) = 0;

	virtual bool GetProperty(NPP npp,
		NPObject* obj, NPIdentifier propertyName, NPVariant* result) = 0;

	virtual bool SetProperty(NPP npp,
		NPObject* obj, NPIdentifier propertyName, const NPVariant* value) = 0;

	virtual bool RemoveProperty(NPP npp, NPObject* obj, NPIdentifier propertyName) = 0;

	virtual bool HasProperty(NPP, NPObject* npobj, NPIdentifier propertyName) = 0;

	virtual bool HasMethod(NPP npp, NPObject* npobj, NPIdentifier methodName) = 0;

	virtual void ReleaseVariantValue(NPVariant* variant) = 0;

	virtual void SetException(NPObject* obj, const NPUTF8* message) = 0;

	virtual void PushPopupsEnabledState(NPP npp, NPBool enabled) = 0;

	virtual void PopPopupsEnabledState(NPP npp) = 0;

	virtual bool Enumerate(
		NPP npp, NPObject *npobj, NPIdentifier **identifier, uint32 *count) = 0;

	virtual void PluginThreadAsyncCall(
		NPP npp, void (*func)(void *), void *userData) = 0;

	virtual bool Construct(
		NPP npp, NPObject* obj, const NPVariant *args, uint32 argCount, NPVariant *result) = 0;

	virtual uint32 ScheduleTimer(
		NPP npp, uint32 interval, NPBool repeat, void (*timerFunc)(NPP npp, uint32 timerID)) = 0;

	virtual void UnscheduleTimer(NPP npp, uint32 timerID) = 0;

private:
	static NPError spGetURL(NPP npp, const char* URL, const char* window);

	static NPError spPostURL(NPP npp,
		const char* URL, const char* window, ::uint32 len, const char* buf, NPBool file);

	static NPError spRequestRead(NPStream* stream, NPByteRange* rangeList);

	static NPError spNewStream(NPP npp,
		NPMIMEType type, const char* window, NPStream** stream);

	static int32 spWrite(NPP npp, NPStream* stream, int32 len, void* buffer);

	static NPError spDestroyStream(NPP npp, NPStream* stream, NPReason reason);

	static void spStatus(NPP npp, const char* message);

	static const char* spUserAgent(NPP npp);

	static void* spMemAlloc(::uint32 size);

	static void spMemFree(void* ptr);

	static ::uint32 spMemFlush(::uint32 size);

	static void spReloadPlugins(NPBool reloadPages);

	static void* spGetJavaEnv();

	static void* spGetJavaPeer(NPP npp);

	static NPError spGetURLNotify(NPP npp,
		const char* URL, const char* window, void* notifyData);

	static NPError spPostURLNotify(NPP npp,
		const char* URL, const char* window,
		::uint32 len, const char* buf, NPBool file, void* notifyData);

	static NPError spGetValue(NPP npp, NPNVariable variable, void* ret_value);

	static NPError spSetValue(NPP npp, NPPVariable variable, void* value);

	static void spInvalidateRect(NPP npp, NPRect* rect);

	static void spInvalidateRegion(NPP npp, NPRegion region);

	static void spForceRedraw(NPP npp);

	static NPIdentifier spGetStringIdentifier(const NPUTF8* name);

	static void spGetStringIdentifiers(
		const NPUTF8** names, int32_t nameCount, NPIdentifier* identifiers);

	static NPIdentifier spGetIntIdentifier(int32_t intid);

	static bool spIdentifierIsString(NPIdentifier identifier);

	static NPUTF8* spUTF8FromIdentifier(NPIdentifier identifier);

	static int32_t spIntFromIdentifier(NPIdentifier identifier);

	static NPObject* spCreateObject(NPP npp, NPClass* aClass);

	static NPObject* spRetainObject(NPObject* obj);

	static void spReleaseObject(NPObject* obj);

	static bool spInvoke(NPP npp,
		NPObject* obj, NPIdentifier methodName, const NPVariant* args, unsigned argCount,
		NPVariant* result);

	static bool spInvokeDefault(NPP npp,
		NPObject* obj, const NPVariant* args, unsigned argCount, NPVariant* result);

	static bool spEvaluate(NPP npp,
		NPObject* obj, NPString* script, NPVariant* result);

	static bool spGetProperty(NPP npp,
		NPObject* obj, NPIdentifier propertyName, NPVariant* result);

	static bool spSetProperty(NPP npp,
		NPObject* obj, NPIdentifier propertyName, const NPVariant* value);

	static bool spRemoveProperty(NPP npp, NPObject* obj, NPIdentifier propertyName);

	static bool spHasProperty(NPP npp, NPObject* npobj, NPIdentifier propertyName);

	static bool spHasMethod(NPP npp, NPObject* npobj, NPIdentifier methodName);

	static void spReleaseVariantValue(NPVariant* variant);

	static void spSetException(NPObject* obj, const NPUTF8* message);
	static void spSetExceptionNPString(NPObject* obj, NPString* message);

	static void spPushPopupsEnabledState(NPP npp, NPBool enabled);

	static void spPopPopupsEnabledState(NPP npp);

	static bool spEnumerate(
		NPP npp, NPObject *npobj, NPIdentifier **identifier, uint32_t *count);

	static void spPluginThreadAsyncCall(
		NPP npp, void (*func)(void *), void *userData);

	static bool spConstruct(
		NPP npp, NPObject* obj, const NPVariant *args, uint32_t argCount, NPVariant *result);

	static uint32 spScheduleTimer(
		NPP npp, uint32 interval, NPBool repeat, void (*timerFunc)(NPP npp, uint32 timerID));

	static void spUnscheduleTimer(NPP npp, uint32 timerID);
	};

// =================================================================================================
// MARK: - GuestFactory

class GuestFactory : public ZCounted
	{
protected:
	GuestFactory();

public:
	virtual ~GuestFactory();

	void GetNPNF(NPNetscapeFuncs_Z& oNPNF);

	virtual const NPPluginFuncs& GetEntryPoints() = 0;

	virtual ZQ<int> QGetMajorVersion() = 0;
	};

// =================================================================================================
// MARK: - Host

class Host
	{
protected:
	Host(ZRef<GuestFactory> iGuestFactory);
	virtual ~Host();

public:
	ZRef<GuestFactory> GetGuestFactory();

	const NPP_t& GetNPP();

// Calls into the guest
	NPError Guest_New(NPMIMEType pluginType, uint16 mode,
		int16 argc, char* argn[], char* argv[], NPSavedData* saved);

	NPError Guest_Destroy(NPSavedData** save);

	NPError Guest_SetWindow(NPWindow* window);

	NPError Guest_NewStream(
		NPMIMEType type, NPStream* stream, NPBool seekable, uint16* stype);

	NPError Guest_DestroyStream(NPStream* stream, NPReason reason);

	int32 Guest_WriteReady(NPStream* stream);

	int32 Guest_Write(NPStream* stream, int32_t offset, int32_t len, void* buffer);

	void Guest_StreamAsFile(NPStream* stream, const char* fname);

	void Guest_Print(NPPrint* platformPrint);

	int16 Guest_HandleEvent(void* event);

	void Guest_URLNotify(const char* url, NPReason reason, void* notifyData);

	NPError Guest_GetValue(NPPVariable iNPPVariable, void* oValue);

	NPError Guest_SetValue(NPNVariable iNPNVariable, void* iValue);

private:
	ZRef<GuestFactory> fGuestFactory;
	const NPPluginFuncs& fNPPluginFuncs;
	NPP_t fNPP_t;
	};

} // namespace ZNetscape
} // namespace ZooLib

#endif // __ZNetscape_Host_h__
