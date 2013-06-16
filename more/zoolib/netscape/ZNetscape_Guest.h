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

#ifndef __ZNetscape_Guest_h__
#define __ZNetscape_Guest_h__ 1
#include "zconfig.h"

#include "zoolib/netscape/ZNetscape.h"
#include "zoolib/netscape/ZNetscape_Object.h"
#include "zoolib/netscape/ZNetscape_Variant.h"
#include "zoolib/ZRef.h"
#include "zoolib/ZVal.h"
#include "zoolib/ZValAccessors.h"

#include <string>
#include <vector>

namespace ZooLib {
namespace ZNetscape {

class NPObjectG;

// =================================================================================================
// MARK: - SetRestoreNPP

class SetRestoreNPP
	{
public:
	SetRestoreNPP(NPP iNPP);
	~SetRestoreNPP();

	static NPP sCurrent();

private:
	NPP fPrior;
	};

typedef SetRestoreNPP NPPSetter;

// =================================================================================================
// MARK: - NPVariantG

typedef NPVariant_T<NPObjectG> NPVariantG;

template <> void sVariantRelease_T(NPVariantG& iNPVariantG);
template <> void* sMalloc_T(NPVariantG&, size_t iLength);
template <> void sFree_T<NPVariantG>(void* iPtr);

// =================================================================================================
// MARK: - NPObjectG

void sRetain(NPObjectG& iOb);
void sRelease(NPObjectG& iOb);
inline void sCheck(NPObjectG* iOb)
	{}

class NPObjectG
:	public NPObject_T<NPVariantG>
	{
protected:
	NPObjectG();

public:
	static bool sIsString(NPIdentifier iNPI);
	static std::string sAsString(NPIdentifier iNPI);
	static int32 sAsInt(NPIdentifier iNPI);

	static NPIdentifier sAsNPI(const std::string& iName);
	static NPIdentifier sAsNPI(int32 iInt);

	void Retain();
	void Release();

	bool HasMethod(const std::string& iName);

	using Base_t::Invoke;
	bool Invoke(
		const std::string& iName, const NPVariantG* iArgs, size_t iCount, NPVariantG& oResult);

	using Base_t::InvokeDefault;
	bool InvokeDefault(const NPVariantG* iArgs, size_t iCount, NPVariantG& oResult);

	bool HasProperty(const std::string& iName);
	bool HasProperty(size_t iIndex);

	bool GetProperty(const std::string& iName, NPVariantG& oResult);
	bool GetProperty(size_t iIndex, NPVariantG& oResult);

	bool SetProperty(const std::string& iName, const NPVariantG& iValue);
	bool SetProperty(size_t iIndex, const NPVariantG& iValue);

	bool RemoveProperty(const std::string& iName);
	bool RemoveProperty(size_t iIndex);

	bool Enumerate(NPIdentifier*& oIdentifiers, uint32& oCount);
	};

template <>
const ZQ<ZRef<NPObjectG> > NPVariantBase::QGet<ZRef<NPObjectG> >() const;

// =================================================================================================
// MARK: - ObjectG

class ObjectG : public NPObjectG
	{
public:
	NPP GetNPP();

protected:
	NPP fNPP;

	ObjectG();
	virtual ~ObjectG();

	virtual void Imp_Invalidate();
	virtual bool Imp_HasMethod(const std::string& iName);

	virtual bool Imp_Invoke(
		const std::string& iName, const NPVariantG* iArgs, size_t iCount, NPVariantG& oResult);

	virtual bool Imp_InvokeDefault(const NPVariantG* iArgs, size_t iCount, NPVariantG& oResult);
	virtual bool Imp_HasProperty(const std::string& iName);
	virtual bool Imp_HasProperty(int32 iInt);
	virtual bool Imp_GetProperty(const std::string& iName, NPVariantG& oResult);
	virtual bool Imp_GetProperty(int32 iInt, NPVariantG& oResult);
	virtual bool Imp_SetProperty(const std::string& iName, const NPVariantG& iValue);
	virtual bool Imp_SetProperty(int32 iInt, const NPVariantG& iValue);
	virtual bool Imp_RemoveProperty(const std::string& iName);
	virtual bool Imp_RemoveProperty(int32 iInt);
	virtual bool Imp_Enumerate(NPIdentifier*& oIDs, uint32& oCount);
	virtual bool Imp_Enumerate(std::vector<std::string>& oNames);

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
// MARK: - GuestMeister

// You must have a concrete instance of a subclass of GuestMeister in your project
class GuestMeister
	{
protected:
	GuestMeister();

public:
	virtual ~GuestMeister();

	static GuestMeister* sGet();

	virtual NPError Initialize(NPNetscapeFuncs_Z* iNPNF);
	virtual NPError GetEntryPoints(NPPluginFuncs* oPluginFuncs);
	virtual NPError Shutdown();

	const NPNetscapeFuncs_Z& GetNPNetscapeFuncs();
	const NPNetscapeFuncs_Z& GetNPNF();

// Calls to the Host.
	NPError Host_GetURL(NPP npp, const char* url, const char* target);

	NPError Host_PostURL(NPP npp,
		const char* url, const char* target, uint32 len, const char* buf, NPBool file);

	NPError Host_RequestRead(NPStream* stream, NPByteRange* rangeList);

	NPError Host_NewStream(NPP npp, NPMIMEType type, const char* target, NPStream** stream);

	int32 Host_Write(NPP npp, NPStream* stream, int32 len, void* buffer);

	NPError Host_DestroyStream(NPP npp, NPStream* stream, NPReason reason);

	void Host_Status(NPP npp, const char* message);

	const char* Host_UserAgent(NPP npp);

	void* Host_MemAlloc(uint32 size);

	void Host_MemFree(void* ptr);

	uint32 Host_MemFlush(uint32 size);

	void Host_ReloadPlugins(NPBool reloadPages);

	JRIEnv* Host_GetJavaEnv();

	jref Host_GetJavaPeer(NPP npp);

	NPError Host_GetURLNotify(NPP npp, const char* url, const char* target, void* notifyData);

	NPError Host_PostURLNotify(NPP npp, const char* url, const char* target,
		uint32 len, const char* buf, NPBool file, void* notifyData);

	NPError Host_GetValue(NPP npp, NPNVariable variable, void *value);

	NPError Host_SetValue(NPP npp, NPPVariable variable, void *value);

	void Host_InvalidateRect(NPP npp, NPRect *invalidRect);

	void Host_InvalidateRegion(NPP npp, NPRegion invalidRegion);

	void Host_ForceRedraw(NPP npp);

	NPIdentifier Host_GetStringIdentifier(const NPUTF8* name);

	void Host_GetStringIdentifiers(
		const NPUTF8** names, int32 nameCount, NPIdentifier* identifiers);

	NPIdentifier Host_GetIntIdentifier(int32 intid);

	bool Host_IdentifierIsString(NPIdentifier identifier);

	NPUTF8* Host_UTF8FromIdentifier(NPIdentifier identifier);

	int32 Host_IntFromIdentifier(NPIdentifier identifier);

	NPObject* Host_CreateObject(NPP npp, NPClass* aClass);

	NPObject* Host_RetainObject(NPObject* obj);

	void Host_ReleaseObject(NPObject* obj);

	bool Host_Invoke(NPP npp, NPObject* obj,
		NPIdentifier methodName, const NPVariant* args, unsigned argCount, NPVariant* result);

	bool Host_InvokeDefault(NPP npp,
		NPObject* obj, const NPVariant* args, unsigned argCount, NPVariant* result);

	bool Host_Evaluate(NPP npp, NPObject* obj, NPString* script, NPVariant* result);

	bool Host_GetProperty(NPP npp, NPObject* obj, NPIdentifier propertyName, NPVariant* result);

	bool Host_SetProperty(NPP npp,
		NPObject* obj, NPIdentifier propertyName, const NPVariant* value);

	bool Host_RemoveProperty(NPP npp, NPObject* obj, NPIdentifier propertyName);

	bool Host_HasProperty(NPP npp, NPObject* npobj, NPIdentifier propertyName);

	bool Host_HasMethod(NPP npp, NPObject* npobj, NPIdentifier methodName);

	void Host_ReleaseVariantValue(NPVariant* variant);

// Disabled till I figure out what the real signature should be
//	void Host_SetException(NPObject* obj, const NPUTF8* message);

	void Host_PushPopupsEnabledState(NPP npp, NPBool enabled);

	void Host_PopPopupsEnabledState(NPP npp);

	bool Host_Enumerate(
		NPP npp, NPObject *npobj, NPIdentifier **identifier, uint32 *count);

	void Host_PluginThreadAsyncCall(NPP npp, void (*func)(void *), void *userData);

	bool Host_Construct(
		NPP npp, NPObject* obj, const NPVariant *args, uint32 argCount, NPVariant *result);

// Calls from host to the guest meister.
	virtual NPError New(
		NPMIMEType pluginType, NPP npp, uint16 mode,
		int16 argc, char* argn[], char* argv[], NPSavedData* saved) = 0;

// Calls from host to a guest instance.
	virtual NPError Destroy(NPP npp, NPSavedData** ave) = 0;

	virtual NPError SetWindow(NPP npp, NPWindow* window) = 0;

	virtual NPError NewStream(NPP npp,
		NPMIMEType type, NPStream* stream, NPBool seekable, uint16* stype) = 0;

	virtual NPError DestroyStream(NPP npp, NPStream* stream, NPReason reason) = 0;

	virtual int32 WriteReady(NPP npp, NPStream* stream) = 0;

	virtual int32 Write(NPP npp,
		NPStream* stream, int32 offset, int32 len, void* buffer) = 0;

	virtual void StreamAsFile(NPP npp, NPStream* stream, const char* fname) = 0;

	virtual void Print(NPP npp, NPPrint* platformPrint) = 0;

	virtual int16 HandleEvent(NPP npp, void* event) = 0;

	virtual void URLNotify(NPP npp, const char* url, NPReason reason, void* notifyData) = 0;

	virtual jref GetJavaClass() = 0;

	virtual NPError GetValue(NPP npp, NPPVariable variable, void *value) = 0;

	virtual NPError SetValue(NPP npp, NPNVariable variable, void *value) = 0;

private:
	static NPError spNew(
		NPMIMEType pluginType, NPP npp, uint16 mode,
		int16 argc, char* argn[], char* argv[], NPSavedData* saved);

	static NPError spDestroy(NPP npp, NPSavedData** save);

	static NPError spSetWindow(NPP npp, NPWindow* window);

	static NPError spNewStream(NPP npp,
		NPMIMEType type, NPStream* stream, NPBool seekable, uint16* stype);

	static NPError spDestroyStream(NPP npp, NPStream* stream, NPReason reason);

	static int32 spWriteReady(NPP npp, NPStream* stream);

	static int32 spWrite(NPP npp, NPStream* stream, int32_t offset, int32_t len, void* buffer);

	static void spStreamAsFile(NPP npp, NPStream* stream, const char* fname);

	static void spPrint(NPP npp, NPPrint* platformPrint);

	static int16 spHandleEvent(NPP npp, void* event);

	static void spURLNotify(NPP npp, const char* url, NPReason reason, void* notifyData);

	static jref spGetJavaClass();

	static NPError spGetValue(NPP npp, NPPVariable variable, void *value);

	static NPError spSetValue(NPP npp, NPNVariable variable, void *value);

	NPNetscapeFuncs_Z fNPNF;

	#if __MACH__ && ZCONFIG(Processor, PPC)
		std::vector<char> fGlue_NPNF;
		std::vector<char> fGlue_PluginFuncs;
		std::vector<char> fGlue_Shutdown;
	#endif
	};

// =================================================================================================
// MARK: - Guest

class Guest
	{
protected:
	Guest(NPP npp);

public:
	virtual ~Guest();

	NPP GetNPP();

	NPError Host_GetURL(const char* url, const char* target);

	NPError Host_PostURL(
		const char* url, const char* target, uint32 len, const char* buf, NPBool file);

	NPError Host_RequestRead(NPStream* stream, NPByteRange* rangeList);

	NPError Host_NewStream(NPMIMEType type, const char* target, NPStream** stream);

	int32 Host_Write(NPStream* stream, int32 len, void* buffer);

	NPError Host_DestroyStream(NPStream* stream, NPReason reason);

	void Host_Status(const char* message);

	const char* Host_UserAgent();

	void* Host_MemAlloc(uint32 size);

	void Host_MemFree(void* ptr);

	uint32 Host_MemFlush(uint32 size);

	void Host_ReloadPlugins(NPBool reloadPages);

	JRIEnv* Host_GetJavaEnv();

	jref Host_GetJavaPeer();

	NPError Host_GetURLNotify(const char* url, const char* target, void* notifyData);

	NPError Host_PostURLNotify(const char* url, const char* target,
		uint32 len, const char* buf, NPBool file, void* notifyData);

	NPError Host_GetValue(NPNVariable variable, void *value);

	NPError Host_SetValue(NPPVariable variable, void *value);

	void Host_InvalidateRect(NPRect *invalidRect);

	void Host_InvalidateRegion(NPRegion invalidRegion);

	void Host_ForceRedraw();

	NPIdentifier Host_GetStringIdentifier(const NPUTF8* name);

	void Host_GetStringIdentifiers(
		const NPUTF8** names, int32 nameCount, NPIdentifier* identifiers);

	NPIdentifier Host_GetIntIdentifier(int32 intid);

	bool Host_IdentifierIsString(NPIdentifier identifier);

	NPUTF8* Host_UTF8FromIdentifier(NPIdentifier identifier);

	int32 Host_IntFromIdentifier(NPIdentifier identifier);

	NPObject* Host_CreateObject(NPClass* aClass);

	NPObject* Host_RetainObject(NPObject* obj);

	void Host_ReleaseObject(NPObject* obj);

	bool Host_Invoke(NPObject* obj,
		NPIdentifier methodName, const NPVariant* args, unsigned argCount, NPVariant* result);

	bool Host_InvokeDefault(
		NPObject* obj, const NPVariant* args, unsigned argCount, NPVariant* result);

	bool Host_Evaluate(NPObject* obj, NPString* script, NPVariant* result);

	bool Host_GetProperty(NPObject* obj, NPIdentifier propertyName, NPVariant* result);

	bool Host_SetProperty(NPObject* obj, NPIdentifier propertyName, const NPVariant* value);

	bool Host_RemoveProperty(NPObject* obj, NPIdentifier propertyName);

	bool Host_HasProperty(NPObject* npobj, NPIdentifier propertyName);

	bool Host_HasMethod(NPObject* npobj, NPIdentifier methodName);

	void Host_ReleaseVariantValue(NPVariant* variant);

// Disabled till I figure out what the real signature should be
//	void Host_SetException(NPObject* obj, const NPUTF8* message);

	void Host_PushPopupsEnabledState(NPBool enabled);

	void Host_PopPopupsEnabledState();

	bool Host_Enumerate(
		NPObject *npobj, NPIdentifier **identifier, uint32 *count);

	void Host_PluginThreadAsyncCall(void (*func)(void *), void *userData);

	bool Host_Construct(
		NPObject* obj, const NPVariant *args, uint32 argCount, NPVariant *result);

	ZRef<NPObjectG> Host_GetWindowObject();
	ZRef<NPObjectG> Host_GetPluginObject();

private:
	NPP fNPP;
	};

} // namespace ZNetscape
} // namespace ZooLib

#endif // __ZNetscape_Guest_h__
