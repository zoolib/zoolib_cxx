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

#include "zoolib/netscape/ZNetscape.h"

#include "zoolib/ZDebug.h" // For ZAssert
#include "zoolib/ZRef_Counted.h"
#include "zoolib/ZVal.h"
#include "zoolib/ZValAccessors.h"

#include <string>

NAMESPACE_ZOOLIB_BEGIN

namespace ZNetscape {

using std::string;

class NPObjectH;

// =================================================================================================
#pragma mark -
#pragma mark * NPVariantH

class NPVariantH
:	public NPVariantBase
,	public ZValR_T<NPVariantH>
	{
    ZOOLIB_DEFINE_OPERATOR_BOOL_TYPES(NPVariantH,
    	operator_bool_generator_type, operator_bool_type);

public:
	operator operator_bool_type() const;

	NPVariantH();
	NPVariantH(const NPVariantH& iOther);
	~NPVariantH();
	NPVariantH& operator=(const NPVariantH& iOther);

	NPVariantH(const NPVariant& iOther);
	NPVariantH& operator=(const NPVariant& iOther);

	NPVariantH(bool iValue);
	NPVariantH(int32 iValue);
	NPVariantH(double iValue);
	NPVariantH(const string& iValue);
	NPVariantH(const char* iValue);
	NPVariantH(NPObjectH* iValue);
	NPVariantH(const ZRef<NPObjectH>& iValue);

// ZVal protocol
	void Clear();

	template <class S>
	bool QGet_T(S& oVal) const;

	template <class S>
	void Set_T(const S& iVal);

// Our protocol
	operator ZRef<NPObjectH>() const;

	void SetVoid();
	void SetNull();

// Typename accessors
	ZMACRO_ZValAccessors_Decl_Entry(NPVariantH, Bool, bool)
	ZMACRO_ZValAccessors_Decl_Entry(NPVariantH, Int32, int32)
	ZMACRO_ZValAccessors_Decl_Entry(NPVariantH, Double, double)
	ZMACRO_ZValAccessors_Decl_Entry(NPVariantH, String, string)
	ZMACRO_ZValAccessors_Decl_Entry(NPVariantH, Object, ZRef<NPObjectH>)

private:
	void pSetString(const char* iChars, size_t iLength);
	void pSetString(const string& iString);
	void pCopyFrom(const NPVariant& iOther);
	void pRelease();
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
	static bool sIsString(NPIdentifier iNPI);
	static string sAsString(NPIdentifier iNPI);
	static int32_t sAsInt(NPIdentifier iNPI);

	static NPIdentifier sAsNPI(const string& iName);
	static NPIdentifier sAsNPI(int32_t iInt);

	void Retain();
	void Release();

	bool HasMethod(const string& iName);
	bool Invoke(
		const string& iName, const NPVariantH* iArgs, size_t iCount, NPVariantH& oResult);
	bool InvokeDefault(const NPVariantH* iArgs, size_t iCount, NPVariantH& oResult);

	bool HasProperty(const string& iName);
	bool HasProperty(size_t iIndex);

	bool GetProperty(const string& iName, NPVariantH& oResult);
	bool GetProperty(size_t iIndex, NPVariantH& oResult);

	bool SetProperty(const string& iName, const NPVariantH& iValue);
	bool SetProperty(size_t iIndex, const NPVariantH& iValue);

	bool RemoveProperty(const string& iName);
	bool RemoveProperty(size_t iIndex);

	NPVariantH Invoke(const string& iName,
		const NPVariantH* iArgs, size_t iCount);

	NPVariantH Invoke(const string& iName);

	NPVariantH Invoke(const string& iName,
		const NPVariantH& iP0);

	NPVariantH Invoke(const string& iName,
		const NPVariantH& iP0,
		const NPVariantH& iP1);

	NPVariantH Invoke(const string& iName,
		const NPVariantH& iP0,
		const NPVariantH& iP1,
		const NPVariantH& iP2);

	NPVariantH InvokeDefault(const NPVariantH* iArgs, size_t iCount);
	NPVariantH InvokeDefault();

	NPVariantH Get(const string& iName);
	NPVariantH Get(size_t iIndex);

	bool Set(const string& iName, const NPVariantH& iValue);
	bool Set(size_t iIndex, const NPVariantH& iValue);

	bool Enumerate(NPIdentifier*& oIdentifiers, uint32_t& oCount);
	bool Enumerate(std::vector<NPIdentifier>& oIdentifiers);
	};

void sRetain(NPObjectH& iOb);
void sRelease(NPObjectH& iOb);

// =================================================================================================
#pragma mark -
#pragma mark * ValMapH

// Sketch of API -- value semantics is an issue.

typedef NPVariantH ValH;

class ValMapH
:	public ZRef<NPObjectH>
	{
public:
	ValMapH();
	ValMapH(const ValMapH& iOther);
	~ValMapH();
	ValMapH& operator=(const ValMapH& iOther);

	ValMapH(const ZRef<NPObjectH>& iOther);
	ValMapH& operator=(const ZRef<NPObjectH>& iOther);

// ValMap protocol
	bool QGet(const string& iName, ValH& oVal) const;
	ValH DGet(const string& iName, const ValH& iDefault) const;
	ValH Get(const string& iName) const;
	void Set(const string& iName, const ValH& iVal);
	void Erase(const string& iName);
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
	virtual bool Imp_HasMethod(const string& iName);

	virtual bool Imp_Invoke(
		const string& iName, const NPVariantH* iArgs, size_t iCount, NPVariantH& oResult);

	virtual bool Imp_InvokeDefault(const NPVariantH* iArgs, size_t iCount, NPVariantH& oResult);

	virtual bool Imp_HasProperty(const string& iName);
	virtual bool Imp_HasProperty(int32_t iInt);
	virtual bool Imp_GetProperty(const string& iName, NPVariantH& oResult);
	virtual bool Imp_GetProperty(int32_t iInt, NPVariantH& oResult);
	virtual bool Imp_SetProperty(const string& iName, const NPVariantH& iValue);
	virtual bool Imp_SetProperty(int32_t iInt, const NPVariantH& iValue);
	virtual bool Imp_RemoveProperty(const string& iName);
	virtual bool Imp_RemoveProperty(int32_t iInt);
	virtual bool Imp_Enumerate(NPIdentifier*& oIDs, uint32_t& oCount);
	virtual bool Imp_Enumerate(std::vector<string>& oNames);

private:
	static NPObject* sAllocate(NPP npp, NPClass *aClass);	
	static void sDeallocate(NPObject* npobj);
	static void sInvalidate(NPObject* npobj);
	static bool sHasMethod(NPObject* npobj, NPIdentifier name);

	static bool sInvoke(NPObject* npobj,
		NPIdentifier name, const NPVariant* args, uint32_t argCount, NPVariant* result);

	static bool sInvokeDefault(NPObject* npobj,
		const NPVariant* args, uint32_t argCount, NPVariant* result);

	static bool sHasProperty(NPObject* npobj, NPIdentifier name);
	static bool sGetProperty(NPObject* npobj, NPIdentifier name, NPVariant* result);
	static bool sSetProperty(NPObject* npobj, NPIdentifier name, const NPVariant* value);
	static bool sRemoveProperty(NPObject* npobj, NPIdentifier name);
	static bool sEnumerate(NPObject* npobj, NPIdentifier** oIdentifiers, uint32_t* oCount);

	static NPClass_Z sNPClass;
	};

// =================================================================================================
#pragma mark -
#pragma mark * HostMeister

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
		const NPUTF8* *names, int32_t nameCount, NPIdentifier* identifiers) = 0;

	virtual NPIdentifier GetIntIdentifier(int32_t intid) = 0;

	virtual bool IdentifierIsString(NPIdentifier identifier) = 0;

	virtual NPUTF8* UTF8FromIdentifier(NPIdentifier identifier) = 0;

	virtual int32_t IntFromIdentifier(NPIdentifier identifier) = 0;

	virtual NPObject* CreateObject(NPP npp, NPClass* aClass) = 0;

	virtual NPObject* RetainObject(NPObject* obj) = 0;

	virtual void ReleaseObject(NPObject* obj) = 0;

	virtual bool Invoke(NPP npp,
		NPObject* obj, NPIdentifier methodName, const NPVariant* args, uint32_t argCount,
		NPVariant* result) = 0;

	virtual bool InvokeDefault(NPP npp,
		NPObject* obj, const NPVariant* args, uint32_t argCount, NPVariant* result) = 0;

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

	virtual bool Enumerate
		(NPP npp, NPObject *npobj, NPIdentifier **identifier, uint32_t *count) = 0;

	virtual void PluginThreadAsyncCall
		(NPP npp, void (*func)(void *), void *userData) = 0;

	virtual bool Construct
		(NPP npp, NPObject* obj, const NPVariant *args, uint32_t argCount, NPVariant *result) = 0;

private:
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

	static void* sMemAlloc(uint32 size);

	static void sMemFree(void* ptr);

	static uint32 sMemFlush(uint32 size);

	static void sReloadPlugins(NPBool reloadPages);

	static void* sGetJavaEnv();

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

	static NPIdentifier sGetStringIdentifier(const NPUTF8* name);

	static void sGetStringIdentifiers(
		const NPUTF8** names, int32_t nameCount, NPIdentifier* identifiers);

	static NPIdentifier sGetIntIdentifier(int32_t intid);

	static bool sIdentifierIsString(NPIdentifier identifier);

	static NPUTF8* sUTF8FromIdentifier(NPIdentifier identifier);

	static int32_t sIntFromIdentifier(NPIdentifier identifier);

	static NPObject* sCreateObject(NPP npp, NPClass* aClass);

	static NPObject* sRetainObject(NPObject* obj);

	static void sReleaseObject(NPObject* obj);

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

	static void sReleaseVariantValue(NPVariant* variant);

	static void sSetException(NPObject* obj, const NPUTF8* message);
	static void sSetExceptionNPString(NPObject* obj, NPString* message);

	static void sPushPopupsEnabledState(NPP npp, NPBool enabled);

	static void sPopPopupsEnabledState(NPP npp);

	static bool sEnumerate
		(NPP npp, NPObject *npobj, NPIdentifier **identifier, uint32_t *count);

	static void sPluginThreadAsyncCall
		(NPP npp, void (*func)(void *), void *userData);

	static bool sConstruct
		(NPP npp, NPObject* obj, const NPVariant *args, uint32_t argCount, NPVariant *result);
	};

// =================================================================================================
#pragma mark -
#pragma mark * GuestFactory

class GuestFactory : public ZRefCountedWithFinalize
	{
protected:
	GuestFactory();

public:
	virtual ~GuestFactory();

	void GetNPNF(NPNetscapeFuncs_Z& oNPNF);

	virtual const NPPluginFuncs& GetEntryPoints() = 0;
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

NAMESPACE_ZOOLIB_END

#endif // __ZNetscape_Host__
