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

#ifndef __ZNetscape_Guest__
#define __ZNetscape_Guest__ 1
#include "zconfig.h"

#include "zoolib/ZCompat_npapi.h"

namespace ZNetscape {

// =================================================================================================
#pragma mark -
#pragma mark * GuestMeister

// You must have a concrete instance of a subclass of GuestMeister in your project
class GuestMeister
	{
protected:
	GuestMeister();

public:
	virtual ~GuestMeister();

	static GuestMeister* sGet();

	virtual NPError Initialize(NPNetscapeFuncs* iNPNF);
	virtual NPError GetEntryPoints(NPPluginFuncs* oPluginFuncs);
	virtual NPError Shutdown();

	const NPNetscapeFuncs& GetNPNetscapeFuncs();

	virtual NPError New(
		NPMIMEType pluginType, NPP instance, uint16 mode,
		int16 argc, char* argn[], char* argv[], NPSavedData* saved) = 0;

	virtual NPError Destroy(NPP instance, NPSavedData** ave) = 0;

	virtual NPError SetWindow(NPP instance, NPWindow* window) = 0;

	virtual NPError NewStream(NPP instance,
		NPMIMEType type, NPStream* stream, NPBool seekable, uint16* stype) = 0;

	virtual NPError DestroyStream(NPP instance, NPStream* stream, NPReason reason) = 0;

	virtual int32 WriteReady(NPP instance, NPStream* stream) = 0;

	virtual int32 Write(NPP instance, NPStream* stream, int32 offset, int32 len, void* buffer) = 0;

	virtual void StreamAsFile(NPP instance, NPStream* stream, const char* fname) = 0;

	virtual void Print(NPP instance, NPPrint* platformPrint) = 0;

	virtual int16 HandleEvent(NPP instance, void* event) = 0;

	virtual void URLNotify(NPP instance, const char* url, NPReason reason, void* notifyData) = 0;

	virtual jref GetJavaClass() = 0;

	virtual NPError GetValue(NPP instance, NPPVariable variable, void *value) = 0;

	virtual NPError SetValue(NPP instance, NPNVariable variable, void *value) = 0;

private:
	static NPError sNew(
		NPMIMEType pluginType, NPP instance, uint16 mode,
		int16 argc, char* argn[], char* argv[], NPSavedData* saved);

	static NPError sDestroy(NPP instance, NPSavedData** save);

	static NPError sSetWindow(NPP instance, NPWindow* window);

	static NPError sNewStream(NPP instance,
		NPMIMEType type, NPStream* stream, NPBool seekable, uint16* stype);

	static NPError sDestroyStream(NPP instance, NPStream* stream, NPReason reason);

	static int32 sWriteReady(NPP instance, NPStream* stream);

	static int32 sWrite(NPP instance, NPStream* stream, int32 offset, int32 len, void* buffer);

	static void sStreamAsFile(NPP instance, NPStream* stream, const char* fname);

	static void sPrint(NPP instance, NPPrint* platformPrint);

	static int16 sHandleEvent(NPP instance, void* event);

	static void sURLNotify(NPP instance, const char* url, NPReason reason, void* notifyData);

	static jref sGetJavaClass();

	static NPError sGetValue(NPP instance, NPPVariable variable, void *value);

	static NPError sSetValue(NPP instance, NPNVariable variable, void *value);

	NPNetscapeFuncs fNPNF;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Guest

class Guest
	{
protected:
	Guest(NPP iNPP, const NPNetscapeFuncs& iNPNetscapeFuncs);

public:
	virtual ~Guest();

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
		const NPUTF8** names, int32_t nameCount, NPIdentifier* identifiers);

	NPIdentifier Host_GetIntIdentifier(int32_t intid);

	bool Host_IdentifierIsString(NPIdentifier identifier);

	NPUTF8* Host_UTF8FromIdentifier(NPIdentifier identifier);

	int32_t Host_IntFromIdentifier(NPIdentifier identifier);

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

	void Host_SetException(NPObject* obj, const NPUTF8* message);

private:
	NPP fNPP;
	NPNetscapeFuncs fNPNF;
	};

} // namespace ZNetscape

#endif // __ZNetscape_Guest__
