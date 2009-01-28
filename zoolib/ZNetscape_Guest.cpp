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

#include "zoolib/ZNetscape_Guest.h"

#if ZCONFIG_SPI_Enabled(Netscape)

#include "zoolib/ZCompat_algorithm.h"
#include "zoolib/ZDebug.h"
#include "zoolib/ZMemory.h"
#include "zoolib/ZNetscape_Macros.h"

#include <string>

using std::min;
using std::string;

NAMESPACE_ZOOLIB_BEGIN

namespace ZNetscape {

// =================================================================================================
#pragma mark -
#pragma mark * NPVariantG

template <>
void spRelease_T(NPVariantG& iNPVariantG)
	{ GuestMeister::sGet()->Host_ReleaseVariantValue(&iNPVariantG); }

// =================================================================================================
#pragma mark -
#pragma mark * ObjectG

ZNETSCAPE_OBJECT_SETUP(NPVariantG);

// =================================================================================================
#pragma mark -
#pragma mark * NPObjectG

static NPP fake = nil;

NPObjectG::NPObjectG()
	{}

NPObjectG::~NPObjectG()
	{}

void NPObjectG::sIncRefCount(NPObjectG* iObject)
	{
	if (iObject)
		iObject->Retain();
	}

void NPObjectG::sDecRefCount(NPObjectG* iObject)
	{
	if (iObject)
		iObject->Release();
	}

void NPObjectG::sCheckAccess(NPObjectG* iObject)
	{
	ZAssertStopf(0, iObject, ("ZRef<NPObjectG> accessed with nil object"));
	ZAssertStopf(0,
		iObject->referenceCount >= 0, ("ZRef<NPObjectG> accessed with invalid refcount"));
	}

bool NPObjectG::sIsString(NPIdentifier iNPI)
	{ return GuestMeister::sGet()->Host_IdentifierIsString(iNPI); }

string NPObjectG::sAsString(NPIdentifier iNPI)
	{
	string result;
	if (NPUTF8* theString = GuestMeister::sGet()->Host_UTF8FromIdentifier(iNPI))
		{
		result = theString;
		free(theString);
		}
	return result;
	}

int32_t NPObjectG::sAsInt(NPIdentifier iNPI)
	{ return GuestMeister::sGet()->Host_IntFromIdentifier(iNPI); }

NPIdentifier NPObjectG::sAsNPI(const string& iName)
	{ return GuestMeister::sGet()->Host_GetStringIdentifier(iName.c_str()); }

NPIdentifier NPObjectG::sAsNPI(int32_t iInt)
	{ return GuestMeister::sGet()->Host_GetIntIdentifier(iInt); }

void NPObjectG::Retain()
	{ GuestMeister::sGet()->Host_RetainObject(this); }

void NPObjectG::Release()
	{ GuestMeister::sGet()->Host_ReleaseObject(this); }

bool NPObjectG::HasMethod(const string& iName)
	{ return GuestMeister::sGet()->Host_HasMethod(fake, this, sAsNPI(iName)); }

bool NPObjectG::Invoke(
	const string& iName, const NPVariantG* iArgs, size_t iCount, NPVariantG& oResult)
	{
	return GuestMeister::sGet()->Host_Invoke(fake, this, sAsNPI(iName), iArgs, iCount, &oResult);
	}

bool NPObjectG::InvokeDefault(const NPVariantG* iArgs, size_t iCount, NPVariantG& oResult)
	{ return GuestMeister::sGet()->Host_InvokeDefault(fake, this, iArgs, iCount, &oResult); }

bool NPObjectG::HasProperty(const string& iName)
	{ return GuestMeister::sGet()->Host_HasProperty(fake, this, sAsNPI(iName)); }

bool NPObjectG::HasProperty(size_t iIndex)
	{ return GuestMeister::sGet()->Host_HasProperty(fake, this, sAsNPI(iIndex)); }

bool NPObjectG::GetProperty(const string& iName, NPVariantG& oResult)
	{ return GuestMeister::sGet()->Host_GetProperty(fake, this, sAsNPI(iName), &oResult); }

bool NPObjectG::GetProperty(size_t iIndex, NPVariantG& oResult)
	{ return GuestMeister::sGet()->Host_GetProperty(fake, this, sAsNPI(iIndex), &oResult); }

bool NPObjectG::SetProperty(const string& iName, const NPVariantG& iValue)
	{ return GuestMeister::sGet()->Host_SetProperty(fake, this, sAsNPI(iName), &iValue); }

bool NPObjectG::SetProperty(size_t iIndex, const NPVariantG& iValue)
	{ return GuestMeister::sGet()->Host_SetProperty(fake, this, sAsNPI(iIndex), &iValue); }

bool NPObjectG::RemoveProperty(const string& iName)
	{ return GuestMeister::sGet()->Host_RemoveProperty(fake, this, sAsNPI(iName)); }

bool NPObjectG::RemoveProperty(size_t iIndex)
	{ return GuestMeister::sGet()->Host_RemoveProperty(fake, this, sAsNPI(iIndex)); }

NPVariantG NPObjectG::Invoke(const std::string& iName, const NPVariantG* iArgs, size_t iCount)
	{
	NPVariantG result;
	this->Invoke(iName, iArgs, iCount, result);
	return result;
	}

NPVariantG NPObjectG::Invoke(const std::string& iName)
	{
	NPVariantG result;
	this->Invoke(iName, nil, 0, result);
	return result;
	}

NPVariantG NPObjectG::InvokeDefault(const NPVariantG* iArgs, size_t iCount)
	{
	NPVariantG result;
	this->InvokeDefault(iArgs, iCount, result);
	return result;
	}

NPVariantG NPObjectG::InvokeDefault()
	{
	NPVariantG result;
	this->InvokeDefault(nil, 0, result);
	return result;
	}

NPVariantG NPObjectG::GetProperty(const std::string& iName)
	{
	NPVariantG result;
	this->GetProperty(iName, result);
	return result;
	}

NPVariantG NPObjectG::GetProperty(size_t iIndex)
	{
	NPVariantG result;
	this->GetProperty(iIndex, result);
	return result;
	}

bool NPObjectG::Enumerate(NPIdentifier*& oIdentifiers, uint32_t& oCount)
	{ return GuestMeister::sGet()->Host_Enumerate(fake, this, &oIdentifiers, &oCount); }

bool NPObjectG::Enumerate(std::vector<NPIdentifier>& oIdentifiers)
	{
	oIdentifiers.clear();
	NPIdentifier* theIDs = nil;
	uint32_t theCount;
	if (!this->Enumerate(theIDs, theCount))
		return false;

	oIdentifiers.insert(oIdentifiers.end(), theIDs, theIDs + theCount);
	free(theIDs);

	return true;
	}

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

NPError GuestMeister::Initialize(NPNetscapeFuncs_Z* iBrowserFuncs)
	{
	fNPNF.version = iBrowserFuncs->version;
	fNPNF.size = min(size_t(iBrowserFuncs->size), sizeof(NPNetscapeFuncs_Z));

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

	if (fNPNF.version >= NPVERS_HAS_POPUPS_ENABLED_STATE)
		{
		fNPNF.pushpopupsenabledstate = iBrowserFuncs->pushpopupsenabledstate;
		fNPNF.poppopupsenabledstate = iBrowserFuncs->poppopupsenabledstate;
		}

	if (fNPNF.version >= NPVERS_HAS_NPOBJECT_ENUM)
		{
		fNPNF.enumerate = iBrowserFuncs->enumerate;
		}

	if (fNPNF.version >= NPVERS_HAS_PLUGIN_THREAD_ASYNC_CALL)
		{
		fNPNF.pluginthreadasynccall = iBrowserFuncs->pluginthreadasynccall;
		fNPNF.construct = iBrowserFuncs->construct;
		}

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
    oPluginFuncs->writeready = (NPP_WriteReadyProcPtr)sWriteReady;
    oPluginFuncs->write = (NPP_WriteProcPtr)sWrite;
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

const NPNetscapeFuncs_Z& GuestMeister::GetNPNetscapeFuncs()
	{ return fNPNF; }

const NPNetscapeFuncs_Z& GuestMeister::GetNPNF()
	{ return fNPNF; }

NPError GuestMeister::Host_GetURL(NPP iNPP, const char* url, const char* target)
	{ return fNPNF.geturl(iNPP, url, target); }

NPError GuestMeister::Host_PostURL(NPP iNPP,
	const char* url, const char* target, uint32 len, const char* buf, NPBool file)
	{ return fNPNF.posturl(iNPP, url, target, len, buf, file); }

NPError GuestMeister::Host_RequestRead(NPStream* stream, NPByteRange* rangeList)
	{ return fNPNF.requestread(stream, rangeList); }

NPError GuestMeister::Host_NewStream(NPP iNPP,
	NPMIMEType type, const char* target, NPStream** stream)
	{ return fNPNF.newstream(iNPP, type, target, stream); }

int32 GuestMeister::Host_Write(NPP iNPP, NPStream* stream, int32 len, void* buffer)
	{ return fNPNF.write(iNPP, stream, len, buffer); }

NPError GuestMeister::Host_DestroyStream(NPP iNPP, NPStream* stream, NPReason reason)
	{ return fNPNF.destroystream(iNPP, stream, reason); }

void GuestMeister::Host_Status(NPP iNPP, const char* message)
	{ return fNPNF.status(iNPP, message); }

const char* GuestMeister::Host_UserAgent(NPP iNPP)
	{ return fNPNF.uagent(iNPP); }

void* GuestMeister::Host_MemAlloc(uint32 size)
	{ return fNPNF.memalloc(size); }

void GuestMeister::Host_MemFree(void* ptr)
	{ return fNPNF.memfree(ptr); }

uint32 GuestMeister::Host_MemFlush(uint32 size)
	{ return fNPNF.memflush(size); }

void GuestMeister::Host_ReloadPlugins(NPBool reloadPages)
	{ return fNPNF.reloadplugins(reloadPages); }

JRIEnv* GuestMeister::Host_GetJavaEnv()
	{ return fNPNF.getJavaEnv(); }

jref GuestMeister::Host_GetJavaPeer(NPP iNPP)
	{ return fNPNF.getJavaPeer(iNPP); }

NPError GuestMeister::Host_GetURLNotify(NPP iNPP,
	const char* url, const char* target, void* notifyData)
	{ return fNPNF.geturlnotify(iNPP, url, target, notifyData); }

NPError GuestMeister::Host_PostURLNotify(NPP iNPP, const char* url, const char* target,
	uint32 len, const char* buf, NPBool file, void* notifyData)
	{ return fNPNF.posturlnotify(iNPP, url, target, len, buf, file, notifyData); }

NPError GuestMeister::Host_GetValue(NPP iNPP, NPNVariable variable, void *value)
	{ return fNPNF.getvalue(iNPP, variable, value); }

NPError GuestMeister::Host_SetValue(NPP iNPP, NPPVariable variable, void *value)
	{ return fNPNF.setvalue(iNPP, variable, value); }

void GuestMeister::Host_InvalidateRect(NPP iNPP, NPRect *invalidRect)
	{ return fNPNF.invalidaterect(iNPP, invalidRect); }

void GuestMeister::Host_InvalidateRegion(NPP iNPP, NPRegion invalidRegion)
	{ return fNPNF.invalidateregion(iNPP, invalidRegion); }

void GuestMeister::Host_ForceRedraw(NPP iNPP)
	{ return fNPNF.forceredraw(iNPP); }

NPIdentifier GuestMeister::Host_GetStringIdentifier(const NPUTF8* name)
	{ return fNPNF.getstringidentifier(name); }

void GuestMeister::Host_GetStringIdentifiers(
	const NPUTF8** names, int32_t nameCount, NPIdentifier* identifiers)
	{ return fNPNF.getstringidentifiers(names, nameCount, identifiers); }

NPIdentifier GuestMeister::Host_GetIntIdentifier(int32_t intid)
	{ return fNPNF.getintidentifier(intid); }

bool GuestMeister::Host_IdentifierIsString(NPIdentifier identifier)
	{ return fNPNF.identifierisstring(identifier); }

NPUTF8* GuestMeister::Host_UTF8FromIdentifier(NPIdentifier identifier)
	{ return fNPNF.utf8fromidentifier(identifier); }

int32_t GuestMeister::Host_IntFromIdentifier(NPIdentifier identifier)
	{ return (int32_t)(fNPNF.intfromidentifier(identifier)); }
//	{ return reinterpret_cast<int32_t>(fNPNF.intfromidentifier(identifier)); }

NPObject* GuestMeister::Host_CreateObject(NPP iNPP, NPClass* aClass)
	{ return fNPNF.createobject(iNPP, aClass); }

NPObject* GuestMeister::Host_RetainObject(NPObject* obj)
	{ return fNPNF.retainobject(obj); }

void GuestMeister::Host_ReleaseObject(NPObject* obj)
	{ return fNPNF.releaseobject(obj); }

bool GuestMeister::Host_Invoke(NPP iNPP, NPObject* obj,
	NPIdentifier methodName, const NPVariant* args, unsigned argCount, NPVariant* result)
	{ return fNPNF.invoke(iNPP, obj, methodName, args, argCount, result); }

bool GuestMeister::Host_InvokeDefault(NPP iNPP,
	NPObject* obj, const NPVariant* args, unsigned argCount, NPVariant* result)
	{ return fNPNF.invokeDefault(iNPP, obj, args, argCount, result); }

bool GuestMeister::Host_Evaluate(NPP iNPP, NPObject* obj, NPString* script, NPVariant* result)
	{ return fNPNF.evaluate(iNPP, obj, script, result); }

bool GuestMeister::Host_GetProperty(NPP iNPP,
	NPObject* obj, NPIdentifier propertyName, NPVariant* result)
	{
	if (fNPNF.getproperty)
		return fNPNF.getproperty(iNPP, obj, propertyName, result);
	return false;
	}

bool GuestMeister::Host_SetProperty(NPP iNPP,
	NPObject* obj, NPIdentifier propertyName, const NPVariant* value)
	{
	if (fNPNF.setproperty)
		return fNPNF.setproperty(iNPP, obj, propertyName, value);
	return false;
	}

bool GuestMeister::Host_RemoveProperty(NPP iNPP, NPObject* obj, NPIdentifier propertyName)
	{
	if (fNPNF.removeproperty)
		return fNPNF.removeproperty(iNPP, obj, propertyName);
	return false;
	}

bool GuestMeister::Host_HasProperty(NPP iNPP, NPObject* npobj, NPIdentifier propertyName)
	{
	if (fNPNF.hasproperty)
		{
		return fNPNF.hasproperty(iNPP, npobj, propertyName);
		}
	else if (fNPNF.getproperty)
		{
		NPVariantG dummy;
		if (fNPNF.getproperty(iNPP, npobj, propertyName, &dummy))
			return !dummy.IsVoid();
		}

	return false;
	}

bool GuestMeister::Host_HasMethod(NPP iNPP, NPObject* npobj, NPIdentifier methodName)
	{
	if (fNPNF.hasmethod)
		{
		return fNPNF.hasmethod(iNPP, npobj, methodName);
		}
	else if (fNPNF.hasproperty)
		{
		return !fNPNF.hasproperty(iNPP, npobj, methodName);
		}
	else if (fNPNF.getproperty)
		{
		// We don't have hasmethod *or* hasproperty. WebKit, at least that in
		// Safari 3.1.2, craps out if you attempt a call of a value when it's
		// not a function. Later versions work.
		// So, we at least distinguish between primitives and objects, and
		// if the entity is an object then we claim that it's a method, which I
		// suppose is better than nothing.
		NPVariantG dummy;
		if (fNPNF.getproperty(iNPP, npobj, methodName, &dummy))
			{
			if (dummy.IsObject())
				return true;
			}
		}
	return false;
	}

void GuestMeister::Host_ReleaseVariantValue(NPVariant* variant)
	{ return fNPNF.releasevariantvalue(variant); }

// Disabled till I figure out what the real signature should be
#if 0
void GuestMeister::Host_SetException(NPObject* obj, const NPUTF8* message)
	{
	#if defined(NewNPN_SetExceptionProc)
		return fNPNF.setexception(obj, message);
	#else
		NPString theString;
		sNPStringChars(theString) = const_cast<NPUTF8*>(message);
		sNPStringLength(theString) = strlen(message);
		return fNPNF.setexception(obj, &theString);
	#endif
	}
#endif

void GuestMeister::Host_PushPopupsEnabledState(NPP iNPP, NPBool enabled)
	{
	if (fNPNF.pushpopupsenabledstate)
		fNPNF.pushpopupsenabledstate(iNPP, enabled);
	}

void GuestMeister::Host_PopPopupsEnabledState(NPP iNPP)
	{
	if (fNPNF.poppopupsenabledstate)
		fNPNF.poppopupsenabledstate(iNPP);
	}

bool GuestMeister::Host_Enumerate
	(NPP iNPP, NPObject* npobj, NPIdentifier** identifier, uint32_t* count)
	{
	if (fNPNF.enumerate)
		return fNPNF.enumerate(iNPP, npobj, identifier, count);
	return false;
	}

void GuestMeister::Host_PluginThreadAsyncCall
	(NPP iNPP, void (*func)(void *), void *userData)
	{
	if (fNPNF.pluginthreadasynccall)
		fNPNF.pluginthreadasynccall(iNPP, func, userData);
	}

bool GuestMeister::Host_Construct
	(NPP iNPP, NPObject* obj, const NPVariant *args, uint32_t argCount, NPVariant *result)
	{
	if (fNPNF.construct)
		return fNPNF.construct(iNPP, obj, args, argCount, result);
	return false;
	}

NPError GuestMeister::sNew(
	NPMIMEType pluginType, NPP instance, uint16 mode,
	int16 argc, char* argn[], char* argv[], NPSavedData* saved)
	{
	ZNETSCAPE_BEFORE
		return sGet()->New(pluginType, instance, mode, argc, argn, argv, saved);
	ZNETSCAPE_AFTER_NPERROR
	}

NPError GuestMeister::sDestroy(NPP instance, NPSavedData** save)
	{
	ZNETSCAPE_BEFORE
		return sGet()->Destroy(instance, save);
	ZNETSCAPE_AFTER_NPERROR
	}

NPError GuestMeister::sSetWindow(NPP instance, NPWindow* window)
	{
	ZNETSCAPE_BEFORE
		return sGet()->SetWindow(instance, window);
	ZNETSCAPE_AFTER_NPERROR
	}

NPError GuestMeister::sNewStream(
	NPP instance, NPMIMEType type, NPStream* stream, NPBool seekable, uint16* stype)
	{
	ZNETSCAPE_BEFORE
		return sGet()->NewStream(instance, type, stream, seekable, stype);
	ZNETSCAPE_AFTER_NPERROR
	}

NPError GuestMeister::sDestroyStream(NPP instance, NPStream* stream, NPReason reason)
	{
	ZNETSCAPE_BEFORE
		return sGet()->DestroyStream(instance, stream, reason);
	ZNETSCAPE_AFTER_NPERROR
	}

int32 GuestMeister::sWriteReady(NPP instance, NPStream* stream)
	{
	ZNETSCAPE_BEFORE
		return sGet()->WriteReady(instance, stream);
	ZNETSCAPE_AFTER_RETURN(0)
	}

int32 GuestMeister::sWrite(NPP instance, NPStream* stream, int32_t offset, int32_t len, void* buffer)
	{
	ZNETSCAPE_BEFORE
		return sGet()->Write(instance, stream, offset, len, buffer);
	ZNETSCAPE_AFTER_RETURN(0)
	}

void GuestMeister::sStreamAsFile(NPP instance, NPStream* stream, const char* fname)
	{
	ZNETSCAPE_BEFORE
		return sGet()->StreamAsFile(instance, stream, fname);
	ZNETSCAPE_AFTER_VOID
	}

void GuestMeister::sPrint(NPP instance, NPPrint* platformPrint)
	{
	ZNETSCAPE_BEFORE
		return sGet()->Print(instance, platformPrint);
	ZNETSCAPE_AFTER_VOID
	}

int16 GuestMeister::sHandleEvent(NPP instance, void* event)
	{
	ZNETSCAPE_BEFORE
		return sGet()->HandleEvent(instance, event);
	ZNETSCAPE_AFTER_RETURN(0)
	}

void GuestMeister::sURLNotify(NPP instance, const char* url, NPReason reason, void* notifyData)
	{
	ZNETSCAPE_BEFORE
		return sGet()->URLNotify(instance, url, reason, notifyData);
	ZNETSCAPE_AFTER_VOID
	}

jref GuestMeister::sGetJavaClass()
	{
	ZNETSCAPE_BEFORE
		return sGet()->GetJavaClass();
	ZNETSCAPE_AFTER_RETURN_NIL
	}

static bool sHostUsesOldWebKit(NPP instance)
	{
	// Logic and comments taken from NetscapeMoviePlugIn/main.c
    // This check is necessary if you want your exposed NPObject to not leak in WebKit-based
    // browsers (including Safari) released prior to Mac OS X 10.5 (Leopard).
    // Earlier versions of WebKit retained the NPObject returned from
    // NPP_GetValue(NPPVpluginScriptableNPObject). However, the NPRuntime API says NPObjects
    // should be retained by the plug-in before they are returned.  WebKit versions later than
    // 420 do not retain returned NPObjects automatically; plug-ins are required to retain them
    // before returning from NPP_GetValue(), as in other browsers.

	static bool sChecked = false;
	static bool sUsesOldWebKit = false;
	if (!sChecked)
		{
		if (const char* userAgent = GuestMeister::sGet()->Host_UserAgent(instance))
			{
			static const char* const prefix = " AppleWebKit/";
			if (char* versionString = strstr(userAgent, prefix))
				{
				versionString += strlen(prefix);
				int webKitVersion = atoi(versionString);
		        if (webKitVersion && webKitVersion < 420)
					sUsesOldWebKit = true;
				}
			}
		sChecked = true;
		}

	return sUsesOldWebKit;
	}

NPError GuestMeister::sGetValue(NPP instance, NPPVariable variable, void *value)
	{
	ZNETSCAPE_BEFORE
		NPError result = sGet()->GetValue(instance, variable, value);
		if (NPERR_NO_ERROR == result
			&& NPPVpluginScriptableNPObject == variable
			&& sHostUsesOldWebKit(instance))
			{
			// We do not call releaseObject, because the likelihood is that the ref
			// count is currently one, and an active release would destroy the object
			// before our buggy caller gets a chance to increment the count.
			NPObject* theNPO = static_cast<NPObject*>(value);
			--theNPO->referenceCount;
			}
		return result;
	ZNETSCAPE_AFTER_NPERROR
	}

NPError GuestMeister::sSetValue(NPP instance, NPNVariable variable, void *value)
	{
	ZNETSCAPE_BEFORE
		return sGet()->SetValue(instance, variable, value);
	ZNETSCAPE_AFTER_NPERROR
	}

// =================================================================================================
#pragma mark -
#pragma mark * Guest

Guest::Guest(NPP iNPP)
:	fNPP(iNPP)
	{}

Guest::~Guest()
	{}

NPP Guest::GetNPP()
	{ return fNPP; }

NPError Guest::Host_GetURL(const char* url, const char* target)
	{ return GuestMeister::sGet()->Host_GetURL(fNPP, url, target); }

NPError Guest::Host_PostURL(
	const char* url, const char* target, uint32 len, const char* buf, NPBool file)
	{ return GuestMeister::sGet()->Host_PostURL(fNPP, url, target, len, buf, file); }

NPError Guest::Host_RequestRead(NPStream* stream, NPByteRange* rangeList)
	{ return GuestMeister::sGet()->Host_RequestRead(stream, rangeList); }

NPError Guest::Host_NewStream(NPMIMEType type, const char* target, NPStream** stream)
	{ return GuestMeister::sGet()->Host_NewStream(fNPP, type, target, stream); }

int32 Guest::Host_Write(NPStream* stream, int32 len, void* buffer)
	{ return GuestMeister::sGet()->Host_Write(fNPP, stream, len, buffer); }

NPError Guest::Host_DestroyStream(NPStream* stream, NPReason reason)
	{ return GuestMeister::sGet()->Host_DestroyStream(fNPP, stream, reason); }

void Guest::Host_Status(const char* message)
	{ return GuestMeister::sGet()->Host_Status(fNPP, message); }

const char* Guest::Host_UserAgent()
	{ return GuestMeister::sGet()->Host_UserAgent(fNPP); }

void* Guest::Host_MemAlloc(uint32 size)
	{ return GuestMeister::sGet()->Host_MemAlloc(size); }

void Guest::Host_MemFree(void* ptr)
	{ return GuestMeister::sGet()->Host_MemFree(ptr); }

uint32 Guest::Host_MemFlush(uint32 size)
	{ return GuestMeister::sGet()->Host_MemFlush(size); }

void Guest::Host_ReloadPlugins(NPBool reloadPages)
	{ return GuestMeister::sGet()->Host_ReloadPlugins(reloadPages); }

JRIEnv* Guest::Host_GetJavaEnv()
	{ return GuestMeister::sGet()->Host_GetJavaEnv(); }

jref Guest::Host_GetJavaPeer()
	{ return GuestMeister::sGet()->Host_GetJavaPeer(fNPP); }

NPError Guest::Host_GetURLNotify(const char* url, const char* target, void* notifyData)
	{ return GuestMeister::sGet()->Host_GetURLNotify(fNPP, url, target, notifyData); }

NPError Guest::Host_PostURLNotify(const char* url, const char* target,
	uint32 len, const char* buf, NPBool file, void* notifyData)
	{
	return GuestMeister::sGet()->Host_PostURLNotify(fNPP, url, target, len, buf, file, notifyData);
	}

NPError Guest::Host_GetValue(NPNVariable variable, void *value)
	{ return GuestMeister::sGet()->Host_GetValue(fNPP, variable, value); }

NPError Guest::Host_SetValue(NPPVariable variable, void *value)
	{ return GuestMeister::sGet()->Host_SetValue(fNPP, variable, value); }

void Guest::Host_InvalidateRect(NPRect *invalidRect)
	{ return GuestMeister::sGet()->Host_InvalidateRect(fNPP, invalidRect); }

void Guest::Host_InvalidateRegion(NPRegion invalidRegion)
	{ return GuestMeister::sGet()->Host_InvalidateRegion(fNPP, invalidRegion); }

void Guest::Host_ForceRedraw()
	{ return GuestMeister::sGet()->Host_ForceRedraw(fNPP); }

NPIdentifier Guest::Host_GetStringIdentifier(const NPUTF8* name)
	{ return GuestMeister::sGet()->Host_GetStringIdentifier(name); }

void Guest::Host_GetStringIdentifiers(
	const NPUTF8** names, int32_t nameCount, NPIdentifier* identifiers)
	{ return GuestMeister::sGet()->Host_GetStringIdentifiers(names, nameCount, identifiers); }

NPIdentifier Guest::Host_GetIntIdentifier(int32_t intid)
	{ return GuestMeister::sGet()->Host_GetIntIdentifier(intid); }

bool Guest::Host_IdentifierIsString(NPIdentifier identifier)
	{ return GuestMeister::sGet()->Host_IdentifierIsString(identifier); }

NPUTF8* Guest::Host_UTF8FromIdentifier(NPIdentifier identifier)
	{ return GuestMeister::sGet()->Host_UTF8FromIdentifier(identifier); }

int32_t Guest::Host_IntFromIdentifier(NPIdentifier identifier)
	{ return GuestMeister::sGet()->Host_IntFromIdentifier(identifier); }

NPObject* Guest::Host_CreateObject(NPClass* aClass)
	{ return GuestMeister::sGet()->Host_CreateObject(fNPP, aClass); }

NPObject* Guest::Host_RetainObject(NPObject* obj)
	{ return GuestMeister::sGet()->Host_RetainObject(obj); }

void Guest::Host_ReleaseObject(NPObject* obj)
	{ return GuestMeister::sGet()->Host_ReleaseObject(obj); }

bool Guest::Host_Invoke(NPObject* obj,
	NPIdentifier methodName, const NPVariant* args, unsigned argCount, NPVariant* result)
	{ return GuestMeister::sGet()->Host_Invoke(fNPP, obj, methodName, args, argCount, result); }

bool Guest::Host_InvokeDefault(
	NPObject* obj, const NPVariant* args, unsigned argCount, NPVariant* result)
	{ return GuestMeister::sGet()->Host_InvokeDefault(fNPP, obj, args, argCount, result); }

bool Guest::Host_Evaluate(NPObject* obj, NPString* script, NPVariant* result)
	{ return GuestMeister::sGet()->Host_Evaluate(fNPP, obj, script, result); }

bool Guest::Host_GetProperty(NPObject* obj, NPIdentifier propertyName, NPVariant* result)
	{ return GuestMeister::sGet()->Host_GetProperty(fNPP, obj, propertyName, result); }

bool Guest::Host_SetProperty(NPObject* obj, NPIdentifier propertyName, const NPVariant* value)
	{ return GuestMeister::sGet()->Host_SetProperty(fNPP, obj, propertyName, value); }

bool Guest::Host_RemoveProperty(NPObject* obj, NPIdentifier propertyName)
	{ return GuestMeister::sGet()->Host_RemoveProperty(fNPP, obj, propertyName); }

bool Guest::Host_HasProperty(NPObject* npobj, NPIdentifier propertyName)
	{ return GuestMeister::sGet()->Host_HasProperty(fNPP, npobj, propertyName); }

bool Guest::Host_HasMethod(NPObject* npobj, NPIdentifier methodName)
	{ return GuestMeister::sGet()->Host_HasMethod(fNPP, npobj, methodName); }

void Guest::Host_ReleaseVariantValue(NPVariant* variant)
	{ return GuestMeister::sGet()->Host_ReleaseVariantValue(variant); }

// Disabled till I figure out what the real signature should be
//void Guest::Host_SetException(NPObject* obj, const NPUTF8* message)
//	{ return GuestMeister::sGet()->Host_SetException(obj, message); }

} // namespace ZNetscape

NAMESPACE_ZOOLIB_END

#endif // ZCONFIG_SPI_Enabled(Netscape)
