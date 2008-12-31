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

#include "zoolib/ZNetscape_Host.h"

#if ZCONFIG_SPI_Enabled(Netscape)

#include "zoolib/ZCommer.h" // For sStartReaderRunner
#include "zoolib/ZDebug.h"
#include "zoolib/ZMemory.h" // For ZBlockZero
#include "zoolib/ZNetscape_Macros.h"
#include "zoolib/ZString.h" // For ZString::sFormat

#include <stdexcept>

using std::string;

NAMESPACE_ZOOLIB_BEGIN

namespace ZNetscape {

// =================================================================================================
#pragma mark -
#pragma mark * ZNetscape

void sRetainH(NPObject* iObject)
	{ HostMeister::sGet()->RetainObject(iObject); }

void sReleaseH(NPObject* iObject)
	{ HostMeister::sGet()->ReleaseObject(iObject); }

void sRetain(NPObjectH* iObject)
	{ sRetainH(iObject); }

void sRelease(NPObjectH* iObject)
	{ sReleaseH(iObject); }

// =================================================================================================
#pragma mark -
#pragma mark * NPVariantH

void NPVariantH::pRelease()
	{
	HostMeister::sGet()->ReleaseVariantValue(this);
	type = NPVariantType_Void;
	}

void NPVariantH::pRetain(NPObject* iObject) const
	{ HostMeister::sGet()->RetainObject(iObject); }

void NPVariantH::pCopyFrom(const NPVariant& iOther)
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

NPVariantH::NPVariantH()
	{}

NPVariantH::NPVariantH(const NPVariant& iOther)
	{
	ZAssert(this != &iOther);
	this->pCopyFrom(iOther);
	}

NPVariantH::~NPVariantH()
	{
	this->pRelease();
	}

NPVariantH& NPVariantH::operator=(const NPVariant& iOther)
	{
	if (this != &iOther)
		{
		this->pRelease();
		this->pCopyFrom(iOther);
		}
	return *this;
	}

NPVariantH::NPVariantH(bool iValue)
:	NPVariantBase(iValue)
	{}

NPVariantH::NPVariantH(int32 iValue)
:	NPVariantBase(iValue)
	{}

NPVariantH::NPVariantH(double iValue)
:	NPVariantBase(iValue)
	{}

NPVariantH::NPVariantH(const string& iValue)
:	NPVariantBase(iValue)
	{}

NPVariantH::NPVariantH(NPObjectH* iValue)
	{
	type = NPVariantType_Void;
	this->SetObject(iValue);
	}

NPVariantH& NPVariantH::operator=(bool iValue)
	{
	this->SetBool(iValue);
	return *this;
	}

NPVariantH& NPVariantH::operator=(int32 iValue)
	{
	this->SetInt32(iValue);
	return *this;
	}

NPVariantH& NPVariantH::operator=(double iValue)
	{
	this->SetDouble(iValue);
	return *this;
	}

NPVariantH& NPVariantH::operator=(const string& iValue)
	{
	this->SetString(iValue);
	return *this;
	}

NPVariantH& NPVariantH::operator=(NPObjectH* iValue)
	{
	this->SetObject(iValue);
	return *this;
	}

void NPVariantH::SetVoid()
	{
	this->pRelease();
	type = NPVariantType_Void;
	}

void NPVariantH::SetNull()
	{
	this->pRelease();
	type = NPVariantType_Null;
	}

void NPVariantH::SetBool(bool iValue)
	{
	this->pRelease();
	type = NPVariantType_Bool;
	value.boolValue = iValue;
	}

void NPVariantH::SetInt32(int32 iValue)
	{
	this->pRelease();
	type = NPVariantType_Int32;
	value.intValue = iValue;
	}

void NPVariantH::SetDouble(double iValue)
	{
	this->pRelease();
	type = NPVariantType_Double;
	value.doubleValue = iValue;
	}

void NPVariantH::SetString(const string& iValue)
	{
	this->pRelease();
	this->pSetString(iValue);
	type = NPVariantType_String;
	}

NPObjectH* NPVariantH::GetObject() const
	{ return this->DGetObject(nil); }

bool NPVariantH::GetObject(NPObjectH*& oValue) const
	{
	if (type != NPVariantType_String)
		return false;
	oValue = static_cast<NPObjectH*>(value.objectValue);
	this->pRetain(oValue);
	return true;	
	}

NPObjectH* NPVariantH::DGetObject(NPObjectH* iDefault) const
	{
	NPObjectH* result = iDefault;
	if (type == NPVariantType_Object)
		result = static_cast<NPObjectH*>(value.objectValue);

	this->pRetain(result);
	return result;
	}

void NPVariantH::SetObject(NPObjectH* iValue)
	{
	this->pRelease();
	value.objectValue = iValue;
	this->pRetain(iValue);
	type = NPVariantType_Object;	
	}

// =================================================================================================
#pragma mark -
#pragma mark * NPObjectH

NPObjectH::NPObjectH()
	{}

NPObjectH::~NPObjectH()
	{}

bool NPObjectH::HasMethod(const string& iName)
	{
	if (_class && _class->hasMethod)
		return _class->hasMethod(this, sAsNPI(iName));
	return false;
	}

bool NPObjectH::Invoke(
	const string& iName, const NPVariantH* iArgs, size_t iCount, NPVariantH& oResult)
	{
	if (_class && _class->invoke)
		return _class->invoke(this, sAsNPI(iName), iArgs, iCount, &oResult);
	return false;
	}

bool NPObjectH::InvokeDefault(const NPVariantH* iArgs, size_t iCount, NPVariantH& oResult)
	{
	if (_class && _class->invokeDefault)
		return _class->invokeDefault(this, iArgs, iCount, &oResult);
	return false;
	}

bool NPObjectH::HasProperty(const string& iName)
	{
	if (_class && _class->hasProperty)
		return _class->hasProperty(this, sAsNPI(iName));
	return false;
	}

bool NPObjectH::GetProperty(const string& iName, NPVariantH& oResult)
	{
	if (_class && _class->getProperty)
		return _class->getProperty(this, sAsNPI(iName), &oResult);
	return false;
	}

bool NPObjectH::SetProperty(const string& iName, const NPVariantH& iValue)
	{
	if (_class && _class->setProperty)
		return _class->setProperty(this, sAsNPI(iName), &iValue);
	return false;
	}

bool NPObjectH::RemoveProperty(const string& iName)
	{
	if (_class && _class->removeProperty)
		return _class->removeProperty(this, sAsNPI(iName));
	return false;
	}

string NPObjectH::sAsString(NPIdentifier iNPI)
	{
	string result;
	if (NPUTF8* theString = HostMeister::sGet()->UTF8FromIdentifier(iNPI))
		{
		result = theString;
		free(theString);
		}
	return result;
	}

NPIdentifier NPObjectH::sAsNPI(const string& iName)
	{ return HostMeister::sGet()->GetStringIdentifier(iName.c_str()); }

// =================================================================================================
#pragma mark -
#pragma mark * ObjectH

ObjectH::ObjectH()
	{
	_class = &sNPClass;
	referenceCount = 1;
	}

ObjectH::~ObjectH()
	{}

void ObjectH::Imp_Invalidate()
	{}

bool ObjectH::Imp_HasMethod(const string& iName)
	{ return false; }

bool ObjectH::Imp_Invoke(
	const string& iName, const NPVariantH* iArgs, size_t iCount, NPVariantH& oResult)
	{ return false; }

bool ObjectH::Imp_InvokeDefault(const NPVariantH* iArgs, size_t iCount, NPVariantH& oResult)
	{ return false; }

bool ObjectH::Imp_HasProperty(const string& iName)
	{ return false; }

bool ObjectH::Imp_GetProperty(const string& iName, NPVariantH& oResult)
	{ return false; }

bool ObjectH::Imp_SetProperty(const string& iName, const NPVariantH& iValue)
	{ return false; }

bool ObjectH::Imp_RemoveProperty(const string& iName)
	{ return false; }

NPObject* ObjectH::sAllocate(NPP npp, NPClass *aClass)
	{
	ZUnimplemented();
	return nil;
	}

void ObjectH::sDeallocate(NPObject* npobj)
	{
	ZNETSCAPE_BEFORE
		delete static_cast<ObjectH*>(npobj);
	ZNETSCAPE_AFTER_VOID
	}

void ObjectH::sInvalidate(NPObject* npobj)
	{
	ZNETSCAPE_BEFORE
		static_cast<ObjectH*>(npobj)->Imp_Invalidate();
	ZNETSCAPE_AFTER_VOID
	}

bool ObjectH::sHasMethod(NPObject* npobj, NPIdentifier name)
	{
	ZNETSCAPE_BEFORE
		return static_cast<ObjectH*>(npobj)->Imp_HasMethod(sAsString(name));
	ZNETSCAPE_AFTER_RETURN_FALSE
	}

bool ObjectH::sInvoke(NPObject* npobj,
	NPIdentifier name, const NPVariant* args, uint32_t argCount, NPVariant* result)
	{
	ZNETSCAPE_BEFORE
		return static_cast<ObjectH*>(npobj)->Imp_Invoke(
			sAsString(name),
			static_cast<const NPVariantH*>(args),
			argCount,
			*static_cast<NPVariantH*>(result));
	ZNETSCAPE_AFTER_RETURN_FALSE
	}

bool ObjectH::sInvokeDefault(NPObject* npobj,
	const NPVariant* args, uint32_t argCount, NPVariant* result)
	{
	ZNETSCAPE_BEFORE
		return static_cast<ObjectH*>(npobj)->Imp_InvokeDefault(
			static_cast<const NPVariantH*>(args),
			argCount,
			*static_cast<NPVariantH*>(result));
	ZNETSCAPE_AFTER_RETURN_FALSE
	}

bool ObjectH::sHasProperty(NPObject*  npobj, NPIdentifier name)
	{
	ZNETSCAPE_BEFORE
		return static_cast<ObjectH*>(npobj)->Imp_HasProperty(sAsString(name));
	ZNETSCAPE_AFTER_RETURN_FALSE
	}

bool ObjectH::sGetProperty(NPObject* npobj, NPIdentifier name, NPVariant* result)
	{
	ZNETSCAPE_BEFORE
		return static_cast<ObjectH*>(npobj)->Imp_GetProperty(
			sAsString(name),
			*static_cast<NPVariantH*>(result));
	ZNETSCAPE_AFTER_RETURN_FALSE
	}

bool ObjectH::sSetProperty(NPObject* npobj, NPIdentifier name, const NPVariant* value)
	{
	ZNETSCAPE_BEFORE
		return static_cast<ObjectH*>(npobj)->Imp_SetProperty(
			sAsString(name),
			*static_cast<const NPVariantH*>(value));
	ZNETSCAPE_AFTER_RETURN_FALSE
	}

bool ObjectH::sRemoveProperty(NPObject* npobj, NPIdentifier name)
	{
	ZNETSCAPE_BEFORE
		return static_cast<NPObjectH*>(npobj)->RemoveProperty(sAsString(name));
	ZNETSCAPE_AFTER_RETURN_FALSE
	}

NPClass ObjectH::sNPClass =
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
#pragma mark * HostMeister

static HostMeister* sHostMeister;

HostMeister* HostMeister::sGet()
	{
	ZAssert(sHostMeister);
	return sHostMeister;
	}

Host* HostMeister::sHostFromNPP(NPP iNPP)
	{
	if (iNPP)
		return static_cast<Host*>(iNPP->ndata);
	return nil;
	}

Host* HostMeister::sHostFromStream(NPStream* iNPStream)
	{
	ZUnimplemented();
	return nil;
	}

void HostMeister::sGetNPNF(NPNetscapeFuncs& oNPNF)
	{
	ZBlockZero(&oNPNF, sizeof(NPNetscapeFuncs));

	#ifdef NPVERS_HAS_RESPONSE_HEADERS
	oNPNF.version = NPVERS_HAS_RESPONSE_HEADERS;
	#else
//	oNPNF.version = 17; // Urg
	oNPNF.version = 14; // Urg
	#endif

	oNPNF.size = sizeof(NPNetscapeFuncs);
	
	oNPNF.geturl = sGetURL;
	oNPNF.posturl = sPostURL;
	oNPNF.requestread = sRequestRead;
	oNPNF.newstream = sNewStream;
	oNPNF.destroystream = sDestroyStream;
	oNPNF.status = sStatus;
	oNPNF.uagent = sUserAgent;
	oNPNF.memalloc = sMemAlloc;
	oNPNF.memfree = sMemFree;
	oNPNF.memflush = sMemFlush;
	oNPNF.reloadplugins = sReloadPlugins;
	oNPNF.geturlnotify = sGetURLNotify;
	oNPNF.posturlnotify = sPostURLNotify;
	oNPNF.getvalue = sGetValue;
	oNPNF.setvalue = sSetValue;
	oNPNF.invalidaterect = sInvalidateRect;
	oNPNF.invalidateregion = sInvalidateRegion;
	oNPNF.forceredraw = sForceRedraw;

	oNPNF.getstringidentifier = sGetStringIdentifier;
	oNPNF.getstringidentifiers = sGetStringIdentifiers;
	oNPNF.getintidentifier = sGetIntIdentifier;
	oNPNF.identifierisstring = sIdentifierIsString;
	oNPNF.utf8fromidentifier = sUTF8FromIdentifier;
	oNPNF.createobject = sCreateObject;
	oNPNF.retainobject = sRetainObject;
	oNPNF.releaseobject = sReleaseObject;
	oNPNF.invoke = sInvoke;
	oNPNF.invokeDefault = sInvokeDefault;
	oNPNF.evaluate = sEvaluate;
	oNPNF.getproperty = sGetProperty;
	oNPNF.setproperty = sSetProperty;
	oNPNF.removeproperty = sRemoveProperty;
	oNPNF.hasproperty = sHasProperty;
	oNPNF.hasmethod = sHasMethod;
	oNPNF.releasevariantvalue = sReleaseVariantValue;

	// These are problematic in one way or another.
	
	// I'm disabling setexception for now -- it's defined as taking
	// an NPString* or UTF8* in different versions of headers, and I
	// haven't determined which is correct, or if it's a host-specific thing.
	oNPNF.setexception = nil;

	#if defined(NewNPN_WriteProc)
		oNPNF.write = sWrite;
	#else
		oNPNF.write = (NPN_WriteProcPtr)sWrite;
	#endif

	#if defined(NewNPN_IntFromIdentifierProc)
		oNPNF.intfromidentifier = sIntFromIdentifier;
	#else
		oNPNF.intfromidentifier = (NPN_IntFromIdentifierProcPtr)sIntFromIdentifier;
	#endif

	#if defined(NewNPN_GetJavaEnvProc)
		oNPNF.getJavaEnv = (NPN_GetJavaEnvUPP)sGetJavaEnv;
	#else
		oNPNF.getJavaEnv = sGetJavaEnv;
	#endif

	#if defined(NewNPN_GetJavaPeerProc)
		oNPNF.getJavaPeer = (NPN_GetJavaPeerUPP)sGetJavaPeer;
	#else
		oNPNF.getJavaPeer = sGetJavaPeer;
	#endif
	}

HostMeister::HostMeister()
	{
	ZAssert(!sHostMeister);
	sHostMeister = this;
	}

HostMeister::~HostMeister()
	{
	ZAssert(sHostMeister == this);
	sHostMeister = nil;
	}

string HostMeister::StringFromIdentifier(NPIdentifier identifier)
	{
	string result;
	if (NPUTF8* theName = this->UTF8FromIdentifier(identifier))
		{
		result = theName;
		free(theName);
		}
	return result;
	}

// -----
// Forwarded to HostMeister

void* HostMeister::sMemAlloc(uint32 size)
	{
	ZNETSCAPE_BEFORE
		return sGet()->MemAlloc(size);
	ZNETSCAPE_AFTER_RETURN_NIL
	}

void HostMeister::sMemFree(void* ptr)
	{
	ZNETSCAPE_BEFORE
		sGet()->MemFree(ptr);
	ZNETSCAPE_AFTER_VOID
	}

uint32 HostMeister::sMemFlush(uint32 size)
	{
	ZNETSCAPE_BEFORE
		return sGet()->MemFlush(size);
	ZNETSCAPE_AFTER_RETURN(0)
	}

void HostMeister::sReloadPlugins(NPBool reloadPages)
	{
	ZNETSCAPE_BEFORE
		sGet()->ReloadPlugins(reloadPages);
	ZNETSCAPE_AFTER_VOID
	}

void* HostMeister::sGetJavaEnv()
	{
	ZNETSCAPE_BEFORE
		return sGet()->GetJavaEnv();
	ZNETSCAPE_AFTER_RETURN_NIL
	}

NPIdentifier HostMeister::sGetStringIdentifier(const NPUTF8* name)
	{
	ZNETSCAPE_BEFORE
		return sGet()->GetStringIdentifier(name);
	ZNETSCAPE_AFTER_RETURN_NIL
	}

void HostMeister::sGetStringIdentifiers(
	const NPUTF8** names, int32_t nameCount, NPIdentifier* identifiers)
	{
	ZNETSCAPE_BEFORE
		sGet()->GetStringIdentifiers(names, nameCount, identifiers);
	ZNETSCAPE_AFTER_VOID
	}

NPIdentifier HostMeister::sGetIntIdentifier(int32_t intid)
	{
	ZNETSCAPE_BEFORE
		return sGet()->GetIntIdentifier(intid);
	ZNETSCAPE_AFTER_RETURN_NIL
	}

bool HostMeister::sIdentifierIsString(NPIdentifier identifier)
	{
	ZNETSCAPE_BEFORE
		return sGet()->IdentifierIsString(identifier);
	ZNETSCAPE_AFTER_RETURN_FALSE
	}

NPUTF8* HostMeister::sUTF8FromIdentifier(NPIdentifier identifier)
	{
	ZNETSCAPE_BEFORE
		return sGet()->UTF8FromIdentifier(identifier);
	ZNETSCAPE_AFTER_RETURN_NIL
	}

int32_t HostMeister::sIntFromIdentifier(NPIdentifier identifier)
	{
	ZNETSCAPE_BEFORE
		return sGet()->IntFromIdentifier(identifier);
	ZNETSCAPE_AFTER_RETURN(0xFFFFFFFF)
	}

NPObject* HostMeister::sRetainObject(NPObject* obj)
	{
	ZNETSCAPE_BEFORE
		return sGet()->RetainObject(obj);
	ZNETSCAPE_AFTER_RETURN_NIL
	}

void HostMeister::sReleaseObject(NPObject* obj)
	{
	ZNETSCAPE_BEFORE
		sGet()->ReleaseObject(obj);
	ZNETSCAPE_AFTER_VOID
	}

void HostMeister::sReleaseVariantValue(NPVariant* variant)
	{
	ZNETSCAPE_BEFORE
		sGet()->ReleaseVariantValue(variant);
	ZNETSCAPE_AFTER_VOID
	}

void HostMeister::sSetException(NPObject* obj, const NPUTF8* message)
	{
	ZNETSCAPE_BEFORE
		sGet()->SetException(obj, message);
	ZNETSCAPE_AFTER_VOID
	}

void HostMeister::sSetExceptionNPString(NPObject* obj, NPString* message)
	{
	ZNETSCAPE_BEFORE
		sGet()->SetException(obj, sNPStringChars(*message));
	ZNETSCAPE_AFTER_VOID
	}

// -----
// Forwarded to Host

NPError HostMeister::sGetURL(NPP npp, const char* URL, const char* window)
	{
	ZNETSCAPE_BEFORE
		return sHostFromNPP(npp)->Host_GetURL(npp, URL, window);
	ZNETSCAPE_AFTER_NPERROR
	}

NPError HostMeister::sPostURL(NPP npp,
	const char* URL, const char* window, uint32 len, const char* buf, NPBool file)
	{
	ZNETSCAPE_BEFORE
		return sHostFromNPP(npp)->Host_PostURL(npp, URL, window, len, buf, file);
	ZNETSCAPE_AFTER_NPERROR
	}

NPError HostMeister::sRequestRead(NPStream* stream, NPByteRange* rangeList)
	{
	ZNETSCAPE_BEFORE
		return sHostFromStream(stream)->Host_RequestRead(stream, rangeList);
	ZNETSCAPE_AFTER_NPERROR
	}

NPError HostMeister::sNewStream(NPP npp,
	NPMIMEType type, const char* window, NPStream** stream)
	{
	ZNETSCAPE_BEFORE
		return sHostFromNPP(npp)->Host_NewStream(npp, type, window, stream);
	ZNETSCAPE_AFTER_NPERROR
	}

int32 HostMeister::sWrite(NPP npp, NPStream* stream, int32 len, void* buffer)
	{
	ZNETSCAPE_BEFORE
		return sHostFromNPP(npp)->Host_Write(npp, stream, len, buffer);
	ZNETSCAPE_AFTER_RETURN(-1)
	}

NPError HostMeister::sDestroyStream(NPP npp, NPStream* stream, NPReason reason)
	{
	ZNETSCAPE_BEFORE
		return sHostFromNPP(npp)->Host_DestroyStream(npp, stream, reason);
	ZNETSCAPE_AFTER_NPERROR
	}

void HostMeister::sStatus(NPP npp, const char* message)
	{
	ZNETSCAPE_BEFORE
		return sHostFromNPP(npp)->Host_Status(npp, message);
	ZNETSCAPE_AFTER_VOID
	}

const char* HostMeister::sUserAgent(NPP npp)
	{
	ZNETSCAPE_BEFORE			
		if (Host* theHost = sHostFromNPP(npp))
			return theHost->Host_UserAgent(npp);
		return "Unknown";
	ZNETSCAPE_AFTER_RETURN_NIL
	}

void* HostMeister::sGetJavaPeer(NPP npp)
	{
	ZNETSCAPE_BEFORE
		return sHostFromNPP(npp)->Host_GetJavaPeer(npp);
	ZNETSCAPE_AFTER_RETURN_NIL
	}

NPError HostMeister::sGetURLNotify(NPP npp,
	const char* URL, const char* window, void* notifyData)
	{
	ZNETSCAPE_BEFORE
		return sHostFromNPP(npp)->Host_GetURLNotify(npp, URL, window, notifyData);
	ZNETSCAPE_AFTER_NPERROR
	}

NPError HostMeister::sPostURLNotify(NPP npp,
	const char* URL, const char* window,
	uint32 len, const char* buf, NPBool file, void* notifyData)
	{
	ZNETSCAPE_BEFORE
		return sHostFromNPP(npp)->Host_PostURLNotify(npp, URL, window, len, buf, file, notifyData);
	ZNETSCAPE_AFTER_NPERROR
	}

NPError HostMeister::sGetValue(NPP npp, NPNVariable variable, void* ret_value)
	{
	ZNETSCAPE_BEFORE
		return sHostFromNPP(npp)->Host_GetValue(npp, variable, ret_value);
	ZNETSCAPE_AFTER_NPERROR
	}

NPError HostMeister::sSetValue(NPP npp, NPPVariable variable, void* value)
	{
	ZNETSCAPE_BEFORE
		return sHostFromNPP(npp)->Host_SetValue(npp, variable, value);
	ZNETSCAPE_AFTER_NPERROR
	}

void HostMeister::sInvalidateRect(NPP npp, NPRect* rect)
	{
	ZNETSCAPE_BEFORE
		return sHostFromNPP(npp)->Host_InvalidateRect(npp, rect);
	ZNETSCAPE_AFTER_VOID
	}

void HostMeister::sInvalidateRegion(NPP npp, NPRegion region)
	{
	ZNETSCAPE_BEFORE
		return sHostFromNPP(npp)->Host_InvalidateRegion(npp, region);
	ZNETSCAPE_AFTER_VOID
	}

void HostMeister::sForceRedraw(NPP npp)
	{
	ZNETSCAPE_BEFORE
		return sHostFromNPP(npp)->Host_ForceRedraw(npp);
	ZNETSCAPE_AFTER_VOID
	}

NPObject* HostMeister::sCreateObject(NPP npp, NPClass* aClass)
	{
	ZNETSCAPE_BEFORE
		return sHostFromNPP(npp)->Host_CreateObject(npp, aClass);
	ZNETSCAPE_AFTER_RETURN_NIL
	}

bool HostMeister::sInvoke(NPP npp,
	NPObject* obj, NPIdentifier methodName, const NPVariant* args, uint32_t argCount,
	NPVariant* result)
	{
	ZNETSCAPE_BEFORE
		return sHostFromNPP(npp)->Host_Invoke(npp, obj, methodName, args, argCount, result);
	ZNETSCAPE_AFTER_RETURN_FALSE
	}

bool HostMeister::sInvokeDefault(NPP npp,
	NPObject* obj, const NPVariant* args, uint32_t argCount, NPVariant* result)
	{
	ZNETSCAPE_BEFORE
		return sHostFromNPP(npp)->Host_InvokeDefault(npp, obj, args, argCount, result);
	ZNETSCAPE_AFTER_RETURN_FALSE
	}

bool HostMeister::sEvaluate(NPP npp,
	NPObject* obj, NPString* script, NPVariant* result)
	{
	ZNETSCAPE_BEFORE
		return sHostFromNPP(npp)->Host_Evaluate(npp, obj, script, result);
	ZNETSCAPE_AFTER_RETURN_FALSE
	}

bool HostMeister::sGetProperty(NPP npp,
	NPObject* obj, NPIdentifier propertyName, NPVariant* result)
	{
	ZNETSCAPE_BEFORE
		return sHostFromNPP(npp)->Host_GetProperty(npp, obj, propertyName, result);
	ZNETSCAPE_AFTER_RETURN_FALSE
	}

bool HostMeister::sSetProperty(NPP npp,
	NPObject* obj, NPIdentifier propertyName, const NPVariant* value)
	{
	ZNETSCAPE_BEFORE
		return sHostFromNPP(npp)->Host_SetProperty(npp, obj, propertyName, value);
	ZNETSCAPE_AFTER_RETURN_FALSE
	}

bool HostMeister::sRemoveProperty(NPP npp, NPObject* obj, NPIdentifier propertyName)
	{
	ZNETSCAPE_BEFORE
		return sHostFromNPP(npp)->Host_RemoveProperty(npp, obj, propertyName);
	ZNETSCAPE_AFTER_RETURN_FALSE
	}

bool HostMeister::sHasProperty(NPP npp, NPObject* npobj, NPIdentifier propertyName)
	{
	ZNETSCAPE_BEFORE
		return sHostFromNPP(npp)->Host_HasProperty(npp, npobj, propertyName);
	ZNETSCAPE_AFTER_RETURN_FALSE
	}

bool HostMeister::sHasMethod(NPP npp, NPObject* npobj, NPIdentifier methodName)
	{
	ZNETSCAPE_BEFORE
		return sHostFromNPP(npp)->Host_HasMethod(npp, npobj, methodName);
	ZNETSCAPE_AFTER_RETURN_FALSE
	}

// =================================================================================================
#pragma mark -
#pragma mark * GuestFactory

GuestFactory::GuestFactory()
	{}

GuestFactory::~GuestFactory()
	{}

void GuestFactory::GetNPNF(NPNetscapeFuncs& oNPNF)
	{
	HostMeister::sGet()->sGetNPNF(oNPNF);
	}

// =================================================================================================
#pragma mark -
#pragma mark * Host

Host::Host(ZRef<GuestFactory> iGuestFactory)
:	fGuestFactory(iGuestFactory)
	{
	ZBlockZero(&fNPPluginFuncs, sizeof(fNPPluginFuncs));
	ZBlockZero(&fNPP_t, sizeof(fNPP_t));
	fNPPluginFuncs.size = sizeof(fNPPluginFuncs);
	fGuestFactory->GetEntryPoints(fNPPluginFuncs);
	fNPP_t.ndata = this;
	}

Host::~Host()
	{}

ZRef<GuestFactory> Host::GetGuestFactory()
	{ return fGuestFactory; }

void Host::Release(NPObject* iObj)
	{ HostMeister::sGet()->ReleaseObject(iObj); }

bool Host::Invoke(
	NPObject* obj, const std::string& iMethod, const NPVariant* iArgs, size_t iCount,
	NPVariant& oResult)
	{
	NPIdentifier methodID = HostMeister::sGet()->GetStringIdentifier(iMethod.c_str());

	if (obj && obj->_class && obj->_class->hasMethod && obj->_class->invoke)
		{
		if (obj->_class->hasMethod(obj, methodID))
			{
			if (obj->_class->invoke(obj, methodID, iArgs, iCount, &oResult))
				return true;
			}
		}
	return false;
	}

const NPP_t& Host::GetNPP()
	{ return fNPP_t; }

NPError Host::Guest_New(NPMIMEType pluginType, uint16 mode,
	int16 argc, char* argn[], char* argv[], NPSavedData* saved)
	{ return fNPPluginFuncs.newp(pluginType, &fNPP_t, mode, argc, argn, argv, saved); }

NPError Host::Guest_Destroy(NPSavedData** save)
	{ return fNPPluginFuncs.destroy(&fNPP_t, save); }

NPError Host::Guest_SetWindow(NPWindow* window)
	{ return fNPPluginFuncs.setwindow(&fNPP_t, window); }

NPError Host::Guest_NewStream(NPMIMEType type, NPStream* stream, NPBool seekable, uint16* stype)
	{ return fNPPluginFuncs.newstream(&fNPP_t, type, stream, seekable, stype); }

NPError Host::Guest_DestroyStream(NPStream* stream, NPReason reason)
	{ return fNPPluginFuncs.destroystream(&fNPP_t, stream, reason); }

void Host::Guest_StreamAsFile(NPStream* stream, const char* fname)
	{ fNPPluginFuncs.asfile(&fNPP_t, stream, fname); }

int32 Host::Guest_WriteReady(NPStream* stream)
	{ return fNPPluginFuncs.writeready(&fNPP_t, stream); }

int32 Host::Guest_Write(NPStream* stream, int32_t offset, int32_t len, void* buffer)
	{ return fNPPluginFuncs.write(&fNPP_t, stream, offset, len, buffer); }

void Host::Guest_Print(NPPrint* platformPrint)
	{ return fNPPluginFuncs.print(&fNPP_t, platformPrint); }

int16 Host::Guest_HandleEvent(void* event)
	{ return fNPPluginFuncs.event(&fNPP_t, event); }

void Host::Guest_URLNotify(const char* URL, NPReason reason, void* notifyData)
	{ fNPPluginFuncs.urlnotify(&fNPP_t, URL, reason, notifyData); }

// JRIGlobalRef

NPError Host::Guest_GetValue(NPPVariable iNPPVariable, void* oValue)
	{ return fNPPluginFuncs.getvalue(&fNPP_t, iNPPVariable, oValue); }

NPError Host::Guest_SetValue(NPNVariable iNPNVariable, void* iValue)
	{ return fNPPluginFuncs.setvalue(&fNPP_t, iNPNVariable, iValue); }

} // namespace ZNetscape

NAMESPACE_ZOOLIB_END

#endif // ZCONFIG_SPI_Enabled(Netscape)
