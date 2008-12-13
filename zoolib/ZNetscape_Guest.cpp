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
#include "zoolib/ZUtil_MacOSX.h"

#include <string>

using std::min;
using std::string;

extern "C" {

#if __MACH__
#	pragma export on
#endif

NPError NP_Initialize(NPNetscapeFuncs*);
NPError NP_Initialize(NPNetscapeFuncs* iBrowserFuncs)
	{ return ZNetscape::GuestMeister::sGet()->Initialize(iBrowserFuncs); }

NPError NP_GetEntryPoints(NPPluginFuncs*);
NPError NP_GetEntryPoints(NPPluginFuncs* oPluginFuncs)
	{ return ZNetscape::GuestMeister::sGet()->GetEntryPoints(oPluginFuncs); }

NPError NP_Shutdown();
NPError NP_Shutdown()
	{ return ZNetscape::GuestMeister::sGet()->Shutdown(); }

#if __MACH__
#	pragma export off
#endif

// For compatibility with CFM and Mozilla-type browsers
#pragma export on

int main(NPNetscapeFuncs* iNPNF, NPPluginFuncs* oPluginFuncs, NPP_ShutdownProcPtr* oShutdownFunc);
int main(NPNetscapeFuncs* iNPNF, NPPluginFuncs* oPluginFuncs, NPP_ShutdownProcPtr* oShutdownFunc)
	{ return ZNetscape::GuestMeister::sGet()->Main(iNPNF, oPluginFuncs, oShutdownFunc); }

#pragma export off

} // extern "C"



// =================================================================================================
#pragma mark -
#pragma mark * ZNetscape

namespace ZNetscape {

void sRetainG(NPObject* iObject)
	{ GuestMeister::sGet()->Host_RetainObject(iObject); }

void sReleaseG(NPObject* iObject)
	{ GuestMeister::sGet()->Host_ReleaseObject(iObject); }

void sRetain(NPObjectG* iObject)
	{ sRetainG(iObject); }

void sRelease(NPObjectG* iObject)
	{ sReleaseG(iObject); }

// =================================================================================================
#pragma mark -
#pragma mark * NPVariantG

void NPVariantG::pRelease()
	{
	GuestMeister::sGet()->Host_ReleaseVariantValue(this);
	type = NPVariantType_Void;
	}

void NPVariantG::pRetain(NPObject* iObject) const
	{ GuestMeister::sGet()->Host_RetainObject(iObject); }

void NPVariantG::pCopyFrom(const NPVariant& iOther)
	{
	switch (iOther.type)
		{
		case NPVariantType_Void:
		case NPVariantType_Null:
			break;
		case NPVariantType_Bool:
			{
			value.boolValue = iOther.value.boolValue;
			break;
			}
		case NPVariantType_Int32:
			{
			value.intValue = iOther.value.intValue;
			break;
			}
		case NPVariantType_Double:
			{
			value.doubleValue = iOther.value.doubleValue;
			break;
			}
		case NPVariantType_String:
			{
			this->pSetString(
				sNPStringCharsConst(iOther.value.stringValue),
				sNPStringLengthConst(iOther.value.stringValue));
			break;
			}
		case NPVariantType_Object:
			{
			value.objectValue = iOther.value.objectValue;
			this->pRetain(value.objectValue);
			break;
			}
		}
	type = iOther.type;
	}

NPVariantG::NPVariantG()
	{}

NPVariantG::NPVariantG(const NPVariant& iOther)
	{
	ZAssert(this != &iOther);
	this->pCopyFrom(iOther);
	}

NPVariantG::~NPVariantG()
	{
	this->pRelease();
	}

NPVariantG& NPVariantG::operator=(const NPVariant& iOther)
	{
	if (this != &iOther)
		{
		this->pRelease();
		this->pCopyFrom(iOther);
		}
	return *this;
	}

NPVariantG::NPVariantG(bool iValue)
:	NPVariantBase(iValue)
	{}

NPVariantG::NPVariantG(int32 iValue)
:	NPVariantBase(iValue)
	{}

NPVariantG::NPVariantG(double iValue)
:	NPVariantBase(iValue)
	{}

NPVariantG::NPVariantG(const string& iValue)
:	NPVariantBase(iValue)
	{}

NPVariantG::NPVariantG(NPObjectG* iValue)
	{
	type = NPVariantType_Void;
	this->SetObject(iValue);
	}

NPVariantG& NPVariantG::operator=(bool iValue)
	{
	this->SetBool(iValue);
	return *this;
	}

NPVariantG& NPVariantG::operator=(int32 iValue)
	{
	this->SetInt32(iValue);
	return *this;
	}

NPVariantG& NPVariantG::operator=(double iValue)
	{
	this->SetDouble(iValue);
	return *this;
	}

NPVariantG& NPVariantG::operator=(const string& iValue)
	{
	this->SetString(iValue);
	return *this;
	}

NPVariantG& NPVariantG::operator=(NPObjectG* iValue)
	{
	this->SetObject(iValue);
	return *this;
	}

void NPVariantG::SetVoid()
	{
	this->pRelease();
	type = NPVariantType_Void;
	}

void NPVariantG::SetNull()
	{
	this->pRelease();
	type = NPVariantType_Null;
	}

void NPVariantG::SetBool(bool iValue)
	{
	this->pRelease();
	type = NPVariantType_Bool;
	value.boolValue = iValue;
	}

void NPVariantG::SetInt32(int32 iValue)
	{
	this->pRelease();
	type = NPVariantType_Int32;
	value.intValue = iValue;
	}

void NPVariantG::SetDouble(double iValue)
	{
	this->pRelease();
	type = NPVariantType_Double;
	value.doubleValue = iValue;
	}

void NPVariantG::SetString(const string& iValue)
	{
	this->pRelease();
	this->pSetString(iValue);
	type = NPVariantType_String;
	}

NPObjectG* NPVariantG::GetObject() const
	{ return this->DGetObject(nil); }

bool NPVariantG::GetObject(NPObjectG*& oValue) const
	{
	if (type != NPVariantType_String)
		return false;
	oValue = static_cast<NPObjectG*>(value.objectValue);
	this->pRetain(oValue);
	return true;	
	}

NPObjectG* NPVariantG::DGetObject(NPObjectG* iDefault) const
	{
	NPObjectG* result = iDefault;
	if (type == NPVariantType_Object)
		result = static_cast<NPObjectG*>(value.objectValue);

	this->pRetain(result);
	return result;
	}

void NPVariantG::SetObject(NPObjectG* iValue)
	{
	this->pRelease();
	value.objectValue = iValue;
	this->pRetain(iValue);
	type = NPVariantType_Object;	
	}

// =================================================================================================
#pragma mark -
#pragma mark * NPObjectG

NPObjectG::NPObjectG()
	{}

NPObjectG::~NPObjectG()
	{}

static NPP fake = (NPP)1;

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

bool NPObjectG::GetProperty(const string& iName, NPVariantG& oResult)
	{ return GuestMeister::sGet()->Host_GetProperty(fake, this, sAsNPI(iName), &oResult); }

bool NPObjectG::SetProperty(const string& iName, const NPVariantG& iValue)
	{ return GuestMeister::sGet()->Host_SetProperty(fake, this, sAsNPI(iName), &iValue); }

bool NPObjectG::RemoveProperty(const string& iName)
	{ return GuestMeister::sGet()->Host_RemoveProperty(fake, this, sAsNPI(iName)); }

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

NPIdentifier NPObjectG::sAsNPI(const string& iName)
	{ return GuestMeister::sGet()->Host_GetStringIdentifier(iName.c_str()); }

// =================================================================================================
#pragma mark -
#pragma mark * ObjectG

ObjectG::ObjectG()
	{
	_class = &sNPClass;
	referenceCount = 1;
	}

ObjectG::~ObjectG()
	{}

void ObjectG::Imp_Invalidate()
	{}

bool ObjectG::Imp_HasMethod(const string& iName)
	{ return false; }

bool ObjectG::Imp_Invoke(
	const string& iName, const NPVariantG* iArgs, size_t iCount, NPVariantG& oResult)
	{ return false; }

bool ObjectG::Imp_InvokeDefault(const NPVariantG* iArgs, size_t iCount, NPVariantG& oResult)
	{ return false; }

bool ObjectG::Imp_HasProperty(const string& iName)
	{ return false; }

bool ObjectG::Imp_GetProperty(const string& iName, NPVariantG& oResult)
	{ return false; }

bool ObjectG::Imp_SetProperty(const string& iName, const NPVariantG& iValue)
	{ return false; }

bool ObjectG::Imp_RemoveProperty(const string& iName)
	{ return false; }

NPObject* ObjectG::sAllocate(NPP npp, NPClass *aClass)
	{
	ZUnimplemented();
	return nil;
	}

void ObjectG::sDeallocate(NPObject* npobj)
	{ delete static_cast<ObjectG*>(npobj); }

void ObjectG::sInvalidate(NPObject* npobj)
	{ static_cast<ObjectG*>(npobj)->Imp_Invalidate(); }

bool ObjectG::sHasMethod(NPObject* npobj, NPIdentifier name)
	{ return static_cast<ObjectG*>(npobj)->Imp_HasMethod(sAsString(name)); }

bool ObjectG::sInvoke(NPObject* npobj,
	NPIdentifier name, const NPVariant* args, uint32_t argCount, NPVariant* result)
	{
	return static_cast<ObjectG*>(npobj)->Imp_Invoke(
		sAsString(name),
		static_cast<const NPVariantG*>(args),
		argCount,
		*static_cast<NPVariantG*>(result));
	}

bool ObjectG::sInvokeDefault(NPObject* npobj,
	const NPVariant* args, uint32_t argCount, NPVariant* result)
	{
	return static_cast<ObjectG*>(npobj)->Imp_InvokeDefault(
		static_cast<const NPVariantG*>(args),
		argCount,
		*static_cast<NPVariantG*>(result));
	}

bool ObjectG::sHasProperty(NPObject*  npobj, NPIdentifier name)
	{ return static_cast<ObjectG*>(npobj)->Imp_HasProperty(sAsString(name)); }

bool ObjectG::sGetProperty(NPObject* npobj, NPIdentifier name, NPVariant* result)
	{
	return static_cast<ObjectG*>(npobj)->Imp_GetProperty(
		sAsString(name),
		*static_cast<NPVariantG*>(result));
	}

bool ObjectG::sSetProperty(NPObject* npobj, NPIdentifier name, const NPVariant* value)
	{
	return static_cast<ObjectG*>(npobj)->Imp_SetProperty(
		sAsString(name),
		*static_cast<const NPVariantG*>(value));
	}

bool ObjectG::sRemoveProperty(NPObject* npobj, NPIdentifier name)
	{ return static_cast<ObjectG*>(npobj)->Imp_RemoveProperty(sAsString(name)); }

NPClass ObjectG::sNPClass =
	{
	1,
	sAllocate,
	sDeallocate,
	sInvalidate,
	sHasMethod,
	sInvoke,
	sInvokeDefault,
	sHasProperty,
	sGetProperty,
	sSetProperty,
	sRemoveProperty
	};

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
	fNPNF.version = iBrowserFuncs->version;
	fNPNF.size = min(size_t(iBrowserFuncs->size), sizeof(NPNetscapeFuncs));

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

int GuestMeister::Main(
	NPNetscapeFuncs* iNPNF, NPPluginFuncs* oPluginFuncs, NPP_ShutdownProcPtr* oShutdownFunc)
	{
	// This function is called by CFM browsers, and also by Mozilla-based code.
	// On Intel the function pointers are just regular function pointers, on PPC they
	// will be tvectors.

	NPError result;
	#if __MACH__ && ZCONFIG(Processor, PPC)
		// We're MachO on PPC, but main has been called. We have to assume that
		// the caller is expecting CFM function pointers.

		// Take a local copy of the passed-in table, no bigger than what was
		// passed, and no bigger than what we're expecting.
		NPNetscapeFuncs localNPNF;
		ZBlockZero(&localNPNF, sizeof(localNPNF));
		ZBlockCopy(iNPNF, &localNPNF, min(size_t(iNPNF->size), sizeof(localNPNF)));

		// Rewrite them as CFM-callable thunks.
		ZUtil_MacOSX::sCreateThunks_MachOCalledByCFM(
			&localNPNF.geturl,
			(localNPNF.size - offsetof(NPNetscapeFuncs, geturl)) / sizeof(void*),
			fGlue_NPNF);

		// And pass the munged local structure to NP_Initialize.
		result = NP_Initialize(&localNPNF);

		// Get our plugin's function pointers
		NP_GetEntryPoints(oPluginFuncs);

		// And munge them into MachO-callable thunks.
		ZUtil_MacOSX::sCreateThunks_CFMCalledByMachO(
			&oPluginFuncs->newp,
			(oPluginFuncs->size - offsetof(NPPluginFuncs, newp)) / sizeof(void*),
			fGlue_PluginFuncs);

		*oShutdownFunc = (NPP_ShutdownProcPtr)NP_Shutdown; 	
		ZUtil_MacOSX::sCreateThunks_CFMCalledByMachO(&oShutdownFunc, 1, fGlue_Shutdown);
		
	#else
		// We're something else, so raw function pointers are OK.
		result = NP_Initialize(iNPNF);
		NP_GetEntryPoints(oPluginFuncs);
		*oShutdownFunc = (NPP_ShutdownProcPtr)NP_Shutdown; 	

	#endif

	return result;
	}

const NPNetscapeFuncs& GuestMeister::GetNPNetscapeFuncs()
	{ return fNPNF; }

const NPNetscapeFuncs& GuestMeister::GetNPNF()
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
	{ return fNPNF.getproperty(iNPP, obj, propertyName, result); }

bool GuestMeister::Host_SetProperty(NPP iNPP,
	NPObject* obj, NPIdentifier propertyName, const NPVariant* value)
	{ return fNPNF.setproperty(iNPP, obj, propertyName, value); }

bool GuestMeister::Host_RemoveProperty(NPP iNPP, NPObject* obj, NPIdentifier propertyName)
	{ return fNPNF.removeproperty(iNPP, obj, propertyName); }

bool GuestMeister::Host_HasProperty(NPP iNPP, NPObject* npobj, NPIdentifier propertyName)
	{ return fNPNF.hasproperty(iNPP, npobj, propertyName); }

bool GuestMeister::Host_HasMethod(NPP iNPP, NPObject* npobj, NPIdentifier methodName)
	{ return fNPNF.hasmethod(iNPP, npobj, methodName); }

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

int32 GuestMeister::sWrite(NPP instance, NPStream* stream, int32_t offset, int32_t len, void* buffer)
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

#warning look at this
#if 0

static bool shouldRetainReturnedNPObjects(NPP instance)
{
    // This check is necessary if you want your exposed NPObject to not leak in WebKit-based browsers (including
    // Safari) released prior to Mac OS X 10.5 (Leopard).
    //
    // Earlier versions of WebKit retained the NPObject returned from NPP_GetValue(NPPVpluginScriptableNPObject).
    // However, the NPRuntime API says NPObjects should be retained by the plug-in before they are returned.  WebKit
    // versions later than 420 do not retain returned NPObjects automatically; plug-ins are required to retain them
    // before returning from NPP_GetValue(), as in other browsers.
    static const unsigned webKitVersionNumberWithRetainFix = 420;
    static const char* const webKitVersionPrefix = " AppleWebKit/";
    const char *userAgent = browser->uagent(instance);
    if (userAgent) {
        // Find " AppleWebKit/" in the user agent string
        char *webKitVersionString = strstr(userAgent, webKitVersionPrefix);
        if (!webKitVersionString)
            return true; // Not WebKit
            
        // Skip past " AppleWebKit/"
        webKitVersionString += strlen(webKitVersionPrefix);
        
        // Convert the version string into an integer.  There are some trailing junk characters after the version
        // number, but atoi() is smart enough to handle those.
        int webKitVersion = atoi(webKitVersionString);
        
        // Should not retain returned NPObjects when running in versions of WebKit earlier than 420
        if (webKitVersion && webKitVersion < webKitVersionNumberWithRetainFix)
            return false;
    }
    
    return true;
}

NPError NPP_GetValue(NPP instance, NPPVariable variable, void *value)
{
    if (variable == NPPVpluginScriptableNPObject) {
        void **v = (void **)value;
        PluginObject *obj = instance->pdata;
        
        // Returned objects are expected to be retained in most browsers, but not all.
        // See comments in shouldRetainReturnedNPObjects().
        if (obj && shouldRetainReturnedNPObjects(instance))
            browser->retainobject((NPObject*)obj);
        
        *v = obj;
        return NPERR_NO_ERROR;
    }
    return NPERR_GENERIC_ERROR;
}
#endif

#endif // ZCONFIG_SPI_Enabled(Netscape)
