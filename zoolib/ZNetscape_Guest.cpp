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

#include "zoolib/ZDebug.h"
#include "zoolib/ZMemory.h"
#include "zoolib/ZNetscape_Guest.h"

#pragma export on
// Mach-o entry points
extern "C" NPError NP_Initialize(NPNetscapeFuncs*);
extern "C" NPError NP_GetEntryPoints(NPPluginFuncs*);
extern "C" NPError NP_Shutdown();

// For compatibility with CFM browsers.
extern "C" int main(
	NPNetscapeFuncs *browserFuncs, NPPluginFuncs *pluginFuncs, NPP_ShutdownProcPtr *shutdown);
#pragma export off


NPError NP_Initialize(NPNetscapeFuncs* iBrowserFuncs)
	{ return ZNetscape::GuestMeister::sGet()->Initialize(iBrowserFuncs); }

NPError NP_GetEntryPoints(NPPluginFuncs* oPluginFuncs)
	{ return ZNetscape::GuestMeister::sGet()->GetEntryPoints(oPluginFuncs); }

NPError NP_Shutdown()
	{ return ZNetscape::GuestMeister::sGet()->Shutdown(); }

namespace ZNetscape {

// =================================================================================================
#pragma mark -
#pragma mark * GuestMeister

static GuestMeister* sGuestMeister;

GuestMeister::GuestMeister()
	{
	ZAssert(!sGuestMeister);
	sGuestMeister = this;
	ZBlockZero(&fNPNF, sizeof(fNPNF));
	}

GuestMeister::~GuestMeister()
	{
	ZAssert(sGuestMeister == this);
	sGuestMeister = nil;
	}

GuestMeister* GuestMeister::sGet()
	{
	ZAssert(sGuestMeister);
	return sGuestMeister;
	}

NPError GuestMeister::Initialize(NPNetscapeFuncs* iBrowserFuncs)
	{
	fNPNF.geturl = iBrowserFuncs->geturl;
	fNPNF.posturl = iBrowserFuncs->posturl;
	fNPNF.requestread = iBrowserFuncs->requestread;
	fNPNF.newstream = iBrowserFuncs->newstream;
	fNPNF.write = iBrowserFuncs->write;
	fNPNF.destroystream = iBrowserFuncs->destroystream;
	fNPNF.status = iBrowserFuncs->status;
	fNPNF.uagent = iBrowserFuncs->uagent;
	fNPNF.memalloc = iBrowserFuncs->memalloc;
	fNPNF.memfree = iBrowserFuncs->memfree;
	fNPNF.memflush = iBrowserFuncs->memflush;
	fNPNF.reloadplugins = iBrowserFuncs->reloadplugins;
	fNPNF.getJavaEnv = iBrowserFuncs->getJavaEnv;
	fNPNF.getJavaPeer = iBrowserFuncs->getJavaPeer;
	fNPNF.geturlnotify = iBrowserFuncs->geturlnotify;
	fNPNF.posturlnotify = iBrowserFuncs->posturlnotify;
	fNPNF.getvalue = iBrowserFuncs->getvalue;
	fNPNF.setvalue = iBrowserFuncs->setvalue;
	fNPNF.invalidaterect = iBrowserFuncs->invalidaterect;
	fNPNF.invalidateregion = iBrowserFuncs->invalidateregion;
	fNPNF.forceredraw = iBrowserFuncs->forceredraw;

	fNPNF.getstringidentifier = iBrowserFuncs->getstringidentifier;
	fNPNF.getstringidentifiers = iBrowserFuncs->getstringidentifiers;
	fNPNF.getintidentifier = iBrowserFuncs->getintidentifier;
	fNPNF.identifierisstring = iBrowserFuncs->identifierisstring;
	fNPNF.utf8fromidentifier = iBrowserFuncs->utf8fromidentifier;
	fNPNF.intfromidentifier = iBrowserFuncs->intfromidentifier;
	fNPNF.createobject = iBrowserFuncs->createobject;
	fNPNF.retainobject = iBrowserFuncs->retainobject;
	fNPNF.releaseobject = iBrowserFuncs->releaseobject;
	fNPNF.invoke = iBrowserFuncs->invoke;
	fNPNF.invokeDefault = iBrowserFuncs->invokeDefault;
	fNPNF.evaluate = iBrowserFuncs->evaluate;
	fNPNF.getproperty = iBrowserFuncs->getproperty;
	fNPNF.setproperty = iBrowserFuncs->setproperty;
	fNPNF.removeproperty = iBrowserFuncs->removeproperty;
	fNPNF.hasproperty = iBrowserFuncs->hasproperty;
	fNPNF.hasmethod = iBrowserFuncs->hasmethod;
	fNPNF.releasevariantvalue = iBrowserFuncs->releasevariantvalue;
	fNPNF.setexception = iBrowserFuncs->setexception;
	
    return NPERR_NO_ERROR;
	}

NPError GuestMeister::GetEntryPoints(NPPluginFuncs* oPluginFuncs)
	{
	oPluginFuncs->size = sizeof(NPPluginFuncs);
	oPluginFuncs->version = 11;

    oPluginFuncs->newp = sNew;
    oPluginFuncs->destroy = sDestroy;
    oPluginFuncs->setwindow = sSetWindow;
    oPluginFuncs->newstream = sNewStream;
    oPluginFuncs->destroystream = sDestroyStream;
    oPluginFuncs->asfile = sStreamAsFile;
    oPluginFuncs->writeready = sWriteReady;
    oPluginFuncs->write = sWrite;
    oPluginFuncs->print = sPrint;
    oPluginFuncs->event = sHandleEvent;
    oPluginFuncs->urlnotify = sURLNotify;
    oPluginFuncs->getvalue = sGetValue;
    oPluginFuncs->setvalue = sSetValue;
    
    return NPERR_NO_ERROR;
	}

NPError GuestMeister::Shutdown()
	{
    return NPERR_NO_ERROR;
	}

const NPNetscapeFuncs& GuestMeister::GetNPNetscapeFuncs()
	{ return fNPNF; }

NPError GuestMeister::sNew(
	NPMIMEType pluginType, NPP instance, uint16 mode,
	int16 argc, char* argn[], char* argv[], NPSavedData* saved)
	{ return sGet()->New(pluginType, instance, mode, argc, argn, argv, saved); }

NPError GuestMeister::sDestroy(NPP instance, NPSavedData** save)
	{ return sGet()->Destroy(instance, save); }

NPError GuestMeister::sSetWindow(NPP instance, NPWindow* window)
	{ return sGet()->SetWindow(instance, window); }

NPError GuestMeister::sNewStream(
	NPP instance, NPMIMEType type, NPStream* stream, NPBool seekable, uint16* stype)
	{ return sGet()->NewStream(instance, type, stream, seekable, stype); }

NPError GuestMeister::sDestroyStream(NPP instance, NPStream* stream, NPReason reason)
	{ return sGet()->DestroyStream(instance, stream, reason); }

int32 GuestMeister::sWriteReady(NPP instance, NPStream* stream)
	{ return sGet()->WriteReady(instance, stream); }

int32 GuestMeister::sWrite(NPP instance, NPStream* stream, int32 offset, int32 len, void* buffer)
	{ return sGet()->Write(instance, stream, offset, len, buffer); }

void GuestMeister::sStreamAsFile(NPP instance, NPStream* stream, const char* fname)
	{ return sGet()->StreamAsFile(instance, stream, fname); }

void GuestMeister::sPrint(NPP instance, NPPrint* platformPrint)
	{ return sGet()->Print(instance, platformPrint); }

int16 GuestMeister::sHandleEvent(NPP instance, void* event)
	{ return sGet()->HandleEvent(instance, event); }

void GuestMeister::sURLNotify(NPP instance, const char* url, NPReason reason, void* notifyData)
	{ return sGet()->URLNotify(instance, url, reason, notifyData); }

jref GuestMeister::sGetJavaClass()
	{ return sGet()->GetJavaClass(); }

NPError GuestMeister::sGetValue(NPP instance, NPPVariable variable, void *value)
	{ return sGet()->GetValue(instance, variable, value); }

NPError GuestMeister::sSetValue(NPP instance, NPNVariable variable, void *value)
	{ return sGet()->SetValue(instance, variable, value); }

// =================================================================================================
#pragma mark -
#pragma mark * Guest

Guest::Guest(NPP iNPP, const NPNetscapeFuncs& iNPNetscapeFuncs)
:	fNPP(iNPP),
	fNPNF(iNPNetscapeFuncs)
	{
//	ZBlockZero(&fNPP, sizeof(fNPP));
//	ZBlockZero(&fNPNF, sizeof(fNPNF));
	}

Guest::~Guest()
	{}

NPError Guest::Host_GetURL(const char* url, const char* target)
	{ return fNPNF.geturl(fNPP, url, target); }

NPError Guest::Host_PostURL(
	const char* url, const char* target, uint32 len, const char* buf, NPBool file)
	{ return fNPNF.posturl(fNPP, url, target, len, buf, file); }

NPError Guest::Host_RequestRead(NPStream* stream, NPByteRange* rangeList)
	{ return fNPNF.requestread(stream, rangeList); }

NPError Guest::Host_NewStream(NPMIMEType type, const char* target, NPStream** stream)
	{ return fNPNF.newstream(fNPP, type, target, stream); }

int32 Guest::Host_Write(NPStream* stream, int32 len, void* buffer)
	{ return fNPNF.write(fNPP, stream, len, buffer); }

NPError Guest::Host_DestroyStream(NPStream* stream, NPReason reason)
	{ return fNPNF.destroystream(fNPP, stream, reason); }

void Guest::Host_Status(const char* message)
	{ return fNPNF.status(fNPP, message); }

const char* Guest::Host_UserAgent()
	{ return fNPNF.uagent(fNPP); }

void* Guest::Host_MemAlloc(uint32 size)
	{ return fNPNF.memalloc(size); }

void Guest::Host_MemFree(void* ptr)
	{ return fNPNF.memfree(ptr); }

uint32 Guest::Host_MemFlush(uint32 size)
	{ return fNPNF.memflush(size); }

void Guest::Host_ReloadPlugins(NPBool reloadPages)
	{ return fNPNF.reloadplugins(reloadPages); }

JRIEnv* Guest::Host_GetJavaEnv()
	{ return fNPNF.getJavaEnv(); }

jref Guest::Host_GetJavaPeer()
	{ return fNPNF.getJavaPeer(fNPP); }

NPError Guest::Host_GetURLNotify(const char* url, const char* target, void* notifyData)
	{ return fNPNF.geturlnotify(fNPP, url, target, notifyData); }

NPError Guest::Host_PostURLNotify(const char* url, const char* target,
	uint32 len, const char* buf, NPBool file, void* notifyData)
	{ return fNPNF.posturlnotify(fNPP, url, target, len, buf, file, notifyData); }

NPError Guest::Host_GetValue(NPNVariable variable, void *value)
	{ return fNPNF.getvalue(fNPP, variable, value); }

NPError Guest::Host_SetValue(NPPVariable variable, void *value)
	{ return fNPNF.setvalue(fNPP, variable, value); }

void Guest::Host_InvalidateRect(NPRect *invalidRect)
	{ return fNPNF.invalidaterect(fNPP, invalidRect); }

void Guest::Host_InvalidateRegion(NPRegion invalidRegion)
	{ return fNPNF.invalidateregion(fNPP, invalidRegion); }

void Guest::Host_ForceRedraw()
	{ return fNPNF.forceredraw(fNPP); }

NPIdentifier Guest::Host_GetStringIdentifier(const NPUTF8* name)
	{ return fNPNF.getstringidentifier(name); }

void Guest::Host_GetStringIdentifiers(
	const NPUTF8** names, int32_t nameCount, NPIdentifier* identifiers)
	{ return fNPNF.getstringidentifiers(names, nameCount, identifiers); }

NPIdentifier Guest::Host_GetIntIdentifier(int32_t intid)
	{ return fNPNF.getintidentifier(intid); }

bool Guest::Host_IdentifierIsString(NPIdentifier identifier)
	{ return fNPNF.identifierisstring(identifier); }

NPUTF8* Guest::Host_UTF8FromIdentifier(NPIdentifier identifier)
	{ return fNPNF.utf8fromidentifier(identifier); }

int32_t Guest::Host_IntFromIdentifier(NPIdentifier identifier)
	{ return reinterpret_cast<int32_t>(fNPNF.intfromidentifier(identifier)); }

NPObject* Guest::Host_CreateObject(NPClass* aClass)
	{ return fNPNF.createobject(fNPP, aClass); }

NPObject* Guest::Host_RetainObject(NPObject* obj)
	{ return fNPNF.retainobject(obj); }

void Guest::Host_ReleaseObject(NPObject* obj)
	{ return fNPNF.releaseobject(obj); }

bool Guest::Host_Invoke(NPObject* obj,
	NPIdentifier methodName, const NPVariant* args, unsigned argCount, NPVariant* result)
	{ return fNPNF.invoke(fNPP, obj, methodName, args, argCount, result); }

bool Guest::Host_InvokeDefault(
	NPObject* obj, const NPVariant* args, unsigned argCount, NPVariant* result)
	{ return fNPNF.invokeDefault(fNPP, obj, args, argCount, result); }

bool Guest::Host_Evaluate(NPObject* obj, NPString* script, NPVariant* result)
	{ return fNPNF.evaluate(fNPP, obj, script, result); }

bool Guest::Host_GetProperty(NPObject* obj, NPIdentifier propertyName, NPVariant* result)
	{ return fNPNF.getproperty(fNPP, obj, propertyName, result); }

bool Guest::Host_SetProperty(NPObject* obj, NPIdentifier propertyName, const NPVariant* value)
	{ return fNPNF.setproperty(fNPP, obj, propertyName, value); }

bool Guest::Host_RemoveProperty(NPObject* obj, NPIdentifier propertyName)
	{ return fNPNF.removeproperty(fNPP, obj, propertyName); }

bool Guest::Host_HasProperty(NPObject* npobj, NPIdentifier propertyName)
	{ return fNPNF.hasproperty(fNPP, npobj, propertyName); }

bool Guest::Host_HasMethod(NPObject* npobj, NPIdentifier methodName)
	{ return fNPNF.hasmethod(fNPP, npobj, methodName); }

void Guest::Host_ReleaseVariantValue(NPVariant* variant)
	{ return fNPNF.releasevariantvalue(variant); }

void Guest::Host_SetException(NPObject* obj, const NPUTF8* message)
	{ return fNPNF.setexception(obj, message); }

} // namespace ZNetscape
