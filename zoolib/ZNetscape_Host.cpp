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

#include "zoolib/ZDebug.h"
#include "zoolib/ZMemory.h" // For ZBlockZero
#include "zoolib/ZNetscape_Macros.h"
#include "zoolib/ZString.h" // For ZString::sFormat

#include <stdexcept>
#include <stdlib.h> // For free/malloc

using std::string;

#define ZNETSCAPE_BEFORE_OBJECT(a) \
	try {

NAMESPACE_ZOOLIB_BEGIN

namespace ZNetscape {

// =================================================================================================
#pragma mark -
#pragma mark * NPVariantH

template <>
void spRelease_T(NPVariantH& iNPVariantH)
	{ HostMeister::sGet()->ReleaseVariantValue(&iNPVariantH); }

template <>
void* spMalloc_T(NPVariantH&, size_t iLength)
	{ return HostMeister::sGet()->MemAlloc(iLength); }

// =================================================================================================
#pragma mark -
#pragma mark * NPObjectH

static NPP fake = nil;

NPObjectH::NPObjectH()
	{}

NPObjectH::~NPObjectH()
	{}
bool NPObjectH::sIsString(NPIdentifier iNPI)
	{ return HostMeister::sGet()->IdentifierIsString(iNPI); }

string NPObjectH::sAsString(NPIdentifier iNPI)
	{
	string result;
	HostMeister* theHM = HostMeister::sGet();
	if (NPUTF8* theString = theHM->UTF8FromIdentifier(iNPI))
		{
		result = theString;
		theHM->MemFree(theString);
		}
	return result;
	}

int32_t NPObjectH::sAsInt(NPIdentifier iNPI)
	{ return HostMeister::sGet()->IntFromIdentifier(iNPI); }

NPIdentifier NPObjectH::sAsNPI(const string& iName)
	{ return HostMeister::sGet()->GetStringIdentifier(iName.c_str()); }

NPIdentifier NPObjectH::sAsNPI(int32_t iInt)
	{ return HostMeister::sGet()->GetIntIdentifier(iInt); }

void NPObjectH::Retain()
	{ HostMeister::sGet()->RetainObject(this); }

void NPObjectH::Release()
	{ HostMeister::sGet()->ReleaseObject(this); }

bool NPObjectH::HasMethod(const string& iName)
	{ return HostMeister::sGet()->HasMethod(fake, this, sAsNPI(iName)); }

bool NPObjectH::Invoke(
	const string& iName, const NPVariantH* iArgs, size_t iCount, NPVariantH& oResult)
	{ return HostMeister::sGet()->Invoke(fake, this, sAsNPI(iName), iArgs, iCount, &oResult); }

bool NPObjectH::InvokeDefault(const NPVariantH* iArgs, size_t iCount, NPVariantH& oResult)
	{ return HostMeister::sGet()->InvokeDefault(fake, this, iArgs, iCount, &oResult); }

bool NPObjectH::HasProperty(const string& iName)
	{ return HostMeister::sGet()->HasProperty(fake, this, sAsNPI(iName)); }

bool NPObjectH::HasProperty(size_t iIndex)
	{ return HostMeister::sGet()->HasProperty(fake, this, sAsNPI(iIndex)); }

bool NPObjectH::GetProperty(const string& iName, NPVariantH& oResult)
	{ return HostMeister::sGet()->GetProperty(fake, this, sAsNPI(iName), &oResult); }

bool NPObjectH::GetProperty(size_t iIndex, NPVariantH& oResult)
	{ return HostMeister::sGet()->GetProperty(fake, this, sAsNPI(iIndex), &oResult); }

bool NPObjectH::SetProperty(const string& iName, const NPVariantH& iValue)
	{ return HostMeister::sGet()->SetProperty(fake, this, sAsNPI(iName), &iValue); }

bool NPObjectH::SetProperty(size_t iIndex, const NPVariantH& iValue)
	{ return HostMeister::sGet()->SetProperty(fake, this, sAsNPI(iIndex), &iValue); }

bool NPObjectH::RemoveProperty(const string& iName)
	{ return HostMeister::sGet()->RemoveProperty(fake, this, sAsNPI(iName)); }

bool NPObjectH::RemoveProperty(size_t iIndex)
	{ return HostMeister::sGet()->RemoveProperty(fake, this, sAsNPI(iIndex)); }

NPVariantH NPObjectH::Invoke(const std::string& iName, const NPVariantH* iArgs, size_t iCount)
	{
	NPVariantH result;
	this->Invoke(iName, iArgs, iCount, result);
	return result;
	}

NPVariantH NPObjectH::Invoke(const std::string& iName)
	{
	NPVariantH result;
	this->Invoke(iName, nil, 0, result);
	return result;
	}

NPVariantH NPObjectH::Invoke(const std::string& iName,
	const NPVariantH& iP0)
	{ return this->Invoke(iName, &iP0, 1); }

NPVariantH NPObjectH::Invoke(const std::string& iName,
	const NPVariantH& iP0,
	const NPVariantH& iP1)
	{
	NPVariantH arr[] = { iP0, iP1};
	return this->Invoke(iName, arr, countof(arr));
	}

NPVariantH NPObjectH::Invoke(const std::string& iName,
	const NPVariantH& iP0,
	const NPVariantH& iP1,
	const NPVariantH& iP2)
	{
	NPVariantH arr[] = { iP0, iP1, iP2 };
	return this->Invoke(iName, arr, countof(arr));
	}

NPVariantH NPObjectH::InvokeDefault(const NPVariantH* iArgs, size_t iCount)
	{
	NPVariantH result;
	this->InvokeDefault(iArgs, iCount, result);
	return result;
	}

NPVariantH NPObjectH::InvokeDefault()
	{
	NPVariantH result;
	this->InvokeDefault(nil, 0, result);
	return result;
	}

NPVariantH NPObjectH::Get(const std::string& iName)
	{
	NPVariantH result;
	this->GetProperty(iName, result);
	return result;
	}

NPVariantH NPObjectH::Get(size_t iIndex)
	{
	NPVariantH result;
	this->GetProperty(iIndex, result);
	return result;
	}

bool NPObjectH::Set(const std::string& iName, const NPVariantH& iValue)
	{ return this->SetProperty(iName, iValue); }

bool NPObjectH::Set(size_t iIndex, const NPVariantH& iValue)
	{ return this->SetProperty(iIndex, iValue); }

bool NPObjectH::Enumerate(NPIdentifier*& oIdentifiers, uint32_t& oCount)
	{ return HostMeister::sGet()->Enumerate(fake, this, &oIdentifiers, &oCount); }

bool NPObjectH::Enumerate(std::vector<NPIdentifier>& oIdentifiers)
	{
	oIdentifiers.clear();
	NPIdentifier* theIDs = nil;
	uint32_t theCount;
	if (!this->Enumerate(theIDs, theCount))
		return false;

	oIdentifiers.insert(oIdentifiers.end(), theIDs, theIDs + theCount);
	HostMeister::sGet()->MemFree(theIDs);

	return true;
	}

void sRetain(NPObjectH& iOb)
	{ iOb.Retain(); }

void sRelease(NPObjectH& iOb)
	{ iOb.Release(); }

// =================================================================================================
#pragma mark -
#pragma mark * ObjectH

NPClass_Z ObjectH::sNPClass(
	sAllocate,
	sDeallocate,
	sInvalidate,
	sHasMethod,
	sInvoke,
	sInvokeDefault,
	sHasProperty,
	sGetProperty,
	sSetProperty,
	sRemoveProperty,
	sEnumerate);

ObjectH::ObjectH()
	{
	this->_class = &sNPClass;
	this->referenceCount = 0;
	}

ObjectH::~ObjectH()
	{}

void ObjectH::Imp_Invalidate()
	{}

bool ObjectH::Imp_HasMethod(const std::string& iName)
	{ return false; }

bool ObjectH::Imp_Invoke(
	const std::string& iName, const NPVariantH* iArgs, size_t iCount, NPVariantH& oResult)
	{ return false; }

bool ObjectH::Imp_InvokeDefault(const NPVariantH* iArgs, size_t iCount, NPVariantH& oResult)
	{ return false; }

bool ObjectH::Imp_HasProperty(const std::string& iName)
	{
	using std::string;
	using std::vector;
	vector<string> theNames;
	if (this->Imp_Enumerate(theNames))
		return ZUtil_STL::sContains(theNames, iName);
	
	return false;
	}

bool ObjectH::Imp_HasProperty(int32_t iInt)
	{ return false; }

bool ObjectH::Imp_GetProperty(const std::string& iName, NPVariantH& oResult)
	{ return false; }

bool ObjectH::Imp_GetProperty(int32_t iInt, NPVariantH& oResult)
	{ return false; }

bool ObjectH::Imp_SetProperty(const std::string& iName, const NPVariantH& iValue)
	{ return false; }

bool ObjectH::Imp_SetProperty(int32_t iInt, const NPVariantH& iValue)
	{ return false; }

bool ObjectH::Imp_RemoveProperty(const std::string& iName)
	{ return false; }

bool ObjectH::Imp_RemoveProperty(int32_t iInt)
	{ return false; }

bool ObjectH::Imp_Enumerate(NPIdentifier*& oIDs, uint32_t& oCount)
	{
	using std::string;
	using std::vector;
	vector<string> theNames;
	if (this->Imp_Enumerate(theNames))
		{
		oCount = theNames.size();
		void* p = HostMeister::sGet()->MemAlloc(sizeof(NPIdentifier) * theNames.size());
		oIDs = static_cast<NPIdentifier*>(p);
		for (size_t x = 0; x < oCount; ++x)
			oIDs[x] = sAsNPI(theNames[x]);

		return true;
		}
	return false;
	}

bool ObjectH::Imp_Enumerate(std::vector<std::string>& oNames)
	{ return false; }

NPObject* ObjectH::sAllocate(NPP npp, NPClass *aClass)
	{
	ZUnimplemented();
	return nil;
	}

void ObjectH::sDeallocate(NPObject* npobj)
	{
	ZNETSCAPE_BEFORE_OBJECT(npobj)
		delete static_cast<ObjectH*>(npobj);
	ZNETSCAPE_AFTER_VOID
	}

void ObjectH::sInvalidate(NPObject* npobj)
	{
	ZNETSCAPE_BEFORE_OBJECT(npobj)
		static_cast<ObjectH*>(npobj)->Imp_Invalidate();
	ZNETSCAPE_AFTER_VOID
	}

bool ObjectH::sHasMethod(NPObject* npobj, NPIdentifier name)
	{
	ZNETSCAPE_BEFORE_OBJECT(npobj)
		return static_cast<ObjectH*>(npobj)->Imp_HasMethod(sAsString(name));
	ZNETSCAPE_AFTER_RETURN_FALSE
	}

bool ObjectH::sInvoke(NPObject* npobj,
	NPIdentifier name, const NPVariant* args, uint32_t argCount, NPVariant* result)
	{
	ZNETSCAPE_BEFORE_OBJECT(npobj)
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
	ZNETSCAPE_BEFORE_OBJECT(npobj)
		return static_cast<ObjectH*>(npobj)->Imp_InvokeDefault(
			static_cast<const NPVariantH*>(args),
			argCount,
			*static_cast<NPVariantH*>(result));
	ZNETSCAPE_AFTER_RETURN_FALSE
	}

bool ObjectH::sHasProperty(NPObject* npobj, NPIdentifier name)
	{
	ZNETSCAPE_BEFORE_OBJECT(npobj)
		{
		if (sIsString(name))
			return static_cast<ObjectH*>(npobj)->Imp_HasProperty(sAsString(name));
		else
			return static_cast<ObjectH*>(npobj)->Imp_HasProperty(sAsInt(name));
		}
	ZNETSCAPE_AFTER_RETURN_FALSE
	}

bool ObjectH::sGetProperty(NPObject* npobj, NPIdentifier name, NPVariant* result)
	{
	ZNETSCAPE_BEFORE_OBJECT(npobj)
		if (sIsString(name))
			{
			return static_cast<ObjectH*>(npobj)->Imp_GetProperty(
				sAsString(name),
				*static_cast<NPVariantH*>(result));
			}
		else
			{
			return static_cast<ObjectH*>(npobj)->Imp_GetProperty(
				sAsInt(name),
				*static_cast<NPVariantH*>(result));
			}
	ZNETSCAPE_AFTER_RETURN_FALSE
	}

bool ObjectH::sSetProperty(NPObject* npobj, NPIdentifier name, const NPVariant* value)
	{
	ZNETSCAPE_BEFORE_OBJECT(npobj)
		if (sIsString(name))
			{
			return static_cast<ObjectH*>(npobj)->Imp_SetProperty(
				sAsString(name),
				*static_cast<const NPVariantH*>(value));
			}
		else
			{
			return static_cast<ObjectH*>(npobj)->Imp_SetProperty(
				sAsInt(name),
				*static_cast<const NPVariantH*>(value));
			}
	ZNETSCAPE_AFTER_RETURN_FALSE
	}

bool ObjectH::sRemoveProperty(NPObject* npobj, NPIdentifier name)
	{
	ZNETSCAPE_BEFORE_OBJECT(npobj)
		if (sIsString(name))
			return static_cast<ObjectH*>(npobj)->Imp_RemoveProperty(sAsString(name));
		else
			return static_cast<ObjectH*>(npobj)->Imp_RemoveProperty(sAsInt(name));
	ZNETSCAPE_AFTER_RETURN_FALSE
	}

bool ObjectH::sEnumerate(NPObject* npobj, NPIdentifier** oIdentifiers, uint32_t* oCount)
	{
	ZNETSCAPE_BEFORE_OBJECT(npobj)
		return static_cast<ObjectH*>(npobj)->Imp_Enumerate(*oIdentifiers, *oCount);
	ZNETSCAPE_AFTER_RETURN_FALSE
	}

// =================================================================================================
#pragma mark -
#pragma mark * HostMeister

static HostMeister* sHostMeister;

HostMeister* HostMeister::sGet()
	{
	ZAssert(sHostMeister);
	return sHostMeister;
	}

Host* HostMeister::sHostFromNPP(NPP npp)
	{
	if (npp)
		return static_cast<Host*>(npp->ndata);
	return nil;
	}

Host* HostMeister::sHostFromStream(NPStream* iNPStream)
	{
	ZUnimplemented();
	return nil;
	}

void HostMeister::sGetNPNF(NPNetscapeFuncs_Z& oNPNF)
	{
	ZBlockZero(&oNPNF, sizeof(oNPNF));

	oNPNF.version = NPVERS_HAS_PLUGIN_THREAD_ASYNC_CALL;

	oNPNF.size = sizeof(oNPNF);
	
	oNPNF.geturl = sGetURL;
	oNPNF.posturl = sPostURL;
	oNPNF.requestread = sRequestRead;
	oNPNF.newstream = sNewStream;
	oNPNF.write = sWrite;
	oNPNF.destroystream = sDestroyStream;
	oNPNF.status = sStatus;
	oNPNF.uagent = sUserAgent;
	oNPNF.memalloc = sMemAlloc;
	oNPNF.memfree = sMemFree;
	oNPNF.memflush = sMemFlush;
	oNPNF.reloadplugins = sReloadPlugins;

	// Mozilla return value is a JRIEnv
	#if defined(NewNPN_GetJavaEnvProc)
		oNPNF.getJavaEnv = (NPN_GetJavaEnvUPP)sGetJavaEnv;
	#else
		oNPNF.getJavaEnv = (NPN_GetJavaEnvProcPtr)sGetJavaEnv;
	#endif

	// Mozilla return value is a jref
	#if defined(NewNPN_GetJavaPeerProc)
		oNPNF.getJavaPeer = (NPN_GetJavaPeerUPP)sGetJavaPeer;
	#else
		oNPNF.getJavaPeer = (NPN_GetJavaPeerProcPtr)sGetJavaPeer;
	#endif

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

	// WebKit/10.4 return value is (incorrectly) an NPIdentifier.
	#if defined(NewNPN_IntFromIdentifierProc)
		oNPNF.intfromidentifier = sIntFromIdentifier;
	#else
		oNPNF.intfromidentifier = (NPN_IntFromIdentifierProcPtr)sIntFromIdentifier;
	#endif

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

	// I'm disabling setexception for now -- it's defined as taking
	// NPString* or UTF8* in different versions of headers, and I
	// haven't determined which is correct, or indeed if it's a host-specific thing.
	oNPNF.setexception = nil;

	// Mozilla return value is a bool
	#if defined(NewNPN_PushPopupsEnabledStateProc)
		oNPNF.pushpopupsenabledstate = (NPN_PushPopupsEnabledStateUPP)sPushPopupsEnabledState;
	#else
		oNPNF.pushpopupsenabledstate = sPushPopupsEnabledState;
	#endif

	// Mozilla return value is a bool
	#if defined(NewNPN_PopPopupsEnabledStateProc)
		oNPNF.poppopupsenabledstate = (NPN_PopPopupsEnabledStateUPP)sPopPopupsEnabledState;
	#else
		oNPNF.poppopupsenabledstate = sPopPopupsEnabledState;
	#endif

	oNPNF.enumerate = sEnumerate;
	oNPNF.pluginthreadasynccall = sPluginThreadAsyncCall;
	oNPNF.construct = sConstruct;
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

NPError HostMeister::sGetURL(NPP npp, const char* URL, const char* window)
	{
	ZNETSCAPE_BEFORE
		return sGet()->GetURL(npp, URL, window);
	ZNETSCAPE_AFTER_NPERROR
	}

NPError HostMeister::sPostURL(NPP npp,
	const char* URL, const char* window, uint32 len, const char* buf, NPBool file)
	{
	ZNETSCAPE_BEFORE
		return sGet()->PostURL(npp, URL, window, len, buf, file);
	ZNETSCAPE_AFTER_NPERROR
	}

NPError HostMeister::sRequestRead(NPStream* stream, NPByteRange* rangeList)
	{
	ZNETSCAPE_BEFORE
		return sGet()->RequestRead(stream, rangeList);
	ZNETSCAPE_AFTER_NPERROR
	}

NPError HostMeister::sNewStream(NPP npp,
	NPMIMEType type, const char* window, NPStream** stream)
	{
	ZNETSCAPE_BEFORE
		return sGet()->NewStream(npp, type, window, stream);
	ZNETSCAPE_AFTER_NPERROR
	}

int32 HostMeister::sWrite(NPP npp, NPStream* stream, int32 len, void* buffer)
	{
	ZNETSCAPE_BEFORE
		return sGet()->Write(npp, stream, len, buffer);
	ZNETSCAPE_AFTER_RETURN(-1)
	}

NPError HostMeister::sDestroyStream(NPP npp, NPStream* stream, NPReason reason)
	{
	ZNETSCAPE_BEFORE
		return sGet()->DestroyStream(npp, stream, reason);
	ZNETSCAPE_AFTER_NPERROR
	}

void HostMeister::sStatus(NPP npp, const char* message)
	{
	ZNETSCAPE_BEFORE
		return sGet()->Status(npp, message);
	ZNETSCAPE_AFTER_VOID
	}

const char* HostMeister::sUserAgent(NPP npp)
	{
	ZNETSCAPE_BEFORE			
		return sGet()->UserAgent(npp);
	ZNETSCAPE_AFTER_RETURN_NIL
	}

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

void* HostMeister::sGetJavaPeer(NPP npp)
	{
	ZNETSCAPE_BEFORE
		return sGet()->GetJavaPeer(npp);
	ZNETSCAPE_AFTER_RETURN_NIL
	}

NPError HostMeister::sGetURLNotify(NPP npp,
	const char* URL, const char* window, void* notifyData)
	{
	ZNETSCAPE_BEFORE
		return sGet()->GetURLNotify(npp, URL, window, notifyData);
	ZNETSCAPE_AFTER_NPERROR
	}

NPError HostMeister::sPostURLNotify(NPP npp,
	const char* URL, const char* window,
	uint32 len, const char* buf, NPBool file, void* notifyData)
	{
	ZNETSCAPE_BEFORE
		return sGet()->PostURLNotify(npp, URL, window, len, buf, file, notifyData);
	ZNETSCAPE_AFTER_NPERROR
	}

NPError HostMeister::sGetValue(NPP npp, NPNVariable variable, void* ret_value)
	{
	ZNETSCAPE_BEFORE
		return sGet()->GetValue(npp, variable, ret_value);
	ZNETSCAPE_AFTER_NPERROR
	}

NPError HostMeister::sSetValue(NPP npp, NPPVariable variable, void* value)
	{
	ZNETSCAPE_BEFORE
		return sGet()->SetValue(npp, variable, value);
	ZNETSCAPE_AFTER_NPERROR
	}

void HostMeister::sInvalidateRect(NPP npp, NPRect* rect)
	{
	ZNETSCAPE_BEFORE
		return sGet()->InvalidateRect(npp, rect);
	ZNETSCAPE_AFTER_VOID
	}

void HostMeister::sInvalidateRegion(NPP npp, NPRegion region)
	{
	ZNETSCAPE_BEFORE
		return sGet()->InvalidateRegion(npp, region);
	ZNETSCAPE_AFTER_VOID
	}

void HostMeister::sForceRedraw(NPP npp)
	{
	ZNETSCAPE_BEFORE
		return sGet()->ForceRedraw(npp);
	ZNETSCAPE_AFTER_VOID
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

NPObject* HostMeister::sCreateObject(NPP npp, NPClass* aClass)
	{
	ZNETSCAPE_BEFORE
		return sGet()->CreateObject(npp, aClass);
	ZNETSCAPE_AFTER_RETURN_NIL
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

bool HostMeister::sInvoke(NPP npp,
	NPObject* obj, NPIdentifier methodName, const NPVariant* args, unsigned argCount,
	NPVariant* result)
	{
	ZNETSCAPE_BEFORE
		return sGet()->Invoke(npp, obj, methodName, args, argCount, result);
	ZNETSCAPE_AFTER_RETURN_FALSE
	}

bool HostMeister::sInvokeDefault(NPP npp,
	NPObject* obj, const NPVariant* args, unsigned argCount, NPVariant* result)
	{
	ZNETSCAPE_BEFORE
		return sGet()->InvokeDefault(npp, obj, args, argCount, result);
	ZNETSCAPE_AFTER_RETURN_FALSE
	}

bool HostMeister::sEvaluate(NPP npp,
	NPObject* obj, NPString* script, NPVariant* result)
	{
	ZNETSCAPE_BEFORE
		return sGet()->Evaluate(npp, obj, script, result);
	ZNETSCAPE_AFTER_RETURN_FALSE
	}

bool HostMeister::sGetProperty(NPP npp,
	NPObject* obj, NPIdentifier propertyName, NPVariant* result)
	{
	ZNETSCAPE_BEFORE
		return sGet()->GetProperty(npp, obj, propertyName, result);
	ZNETSCAPE_AFTER_RETURN_FALSE
	}

bool HostMeister::sSetProperty(NPP npp,
	NPObject* obj, NPIdentifier propertyName, const NPVariant* value)
	{
	ZNETSCAPE_BEFORE
		return sGet()->SetProperty(npp, obj, propertyName, value);
	ZNETSCAPE_AFTER_RETURN_FALSE
	}

bool HostMeister::sRemoveProperty(NPP npp, NPObject* obj, NPIdentifier propertyName)
	{
	ZNETSCAPE_BEFORE
		return sGet()->RemoveProperty(npp, obj, propertyName);
	ZNETSCAPE_AFTER_RETURN_FALSE
	}

bool HostMeister::sHasProperty(NPP npp, NPObject* npobj, NPIdentifier propertyName)
	{
	ZNETSCAPE_BEFORE
		return sGet()->HasProperty(npp, npobj, propertyName);
	ZNETSCAPE_AFTER_RETURN_FALSE
	}

bool HostMeister::sHasMethod(NPP npp, NPObject* npobj, NPIdentifier methodName)
	{
	ZNETSCAPE_BEFORE
		return sGet()->HasMethod(npp, npobj, methodName);
	ZNETSCAPE_AFTER_RETURN_FALSE
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

void HostMeister::sPushPopupsEnabledState(NPP npp, NPBool enabled)
	{
	ZNETSCAPE_BEFORE
		sGet()->PushPopupsEnabledState(npp, enabled);
	ZNETSCAPE_AFTER_VOID
	}

void HostMeister::sPopPopupsEnabledState(NPP npp)
	{
	ZNETSCAPE_BEFORE
		sGet()->PopPopupsEnabledState(npp);
	ZNETSCAPE_AFTER_VOID
	}

bool HostMeister::sEnumerate
	(NPP npp, NPObject *npobj, NPIdentifier **identifier, uint32_t *count)
	{
	ZNETSCAPE_BEFORE
		return sGet()->Enumerate(npp, npobj, identifier, count);
	ZNETSCAPE_AFTER_RETURN_FALSE
	}

void HostMeister::sPluginThreadAsyncCall
	(NPP npp, void (*func)(void *), void *userData)
	{
	ZNETSCAPE_BEFORE
		sGet()->sPluginThreadAsyncCall(npp, func, userData);
	ZNETSCAPE_AFTER_VOID
	}

bool HostMeister::sConstruct
	(NPP npp, NPObject* obj, const NPVariant *args, uint32_t argCount, NPVariant *result)
	{
	ZNETSCAPE_BEFORE
		sGet()->Construct(npp, obj, args, argCount, result);
	ZNETSCAPE_AFTER_RETURN_FALSE
	}

// =================================================================================================
#pragma mark -
#pragma mark * GuestFactory

GuestFactory::GuestFactory()
	{}

GuestFactory::~GuestFactory()
	{}

void GuestFactory::GetNPNF(NPNetscapeFuncs_Z& oNPNF)
	{ HostMeister::sGet()->sGetNPNF(oNPNF); }

// =================================================================================================
#pragma mark -
#pragma mark * Host

Host::Host(ZRef<GuestFactory> iGuestFactory)
:	fGuestFactory(iGuestFactory),
	fNPPluginFuncs(fGuestFactory->GetEntryPoints())
	{
	ZBlockZero(&fNPP_t, sizeof(fNPP_t));
	fNPP_t.ndata = this;
	}

Host::~Host()
	{}

ZRef<GuestFactory> Host::GetGuestFactory()
	{ return fGuestFactory; }

const NPP_t& Host::GetNPP()
	{ return fNPP_t; }

NPError Host::Guest_New(NPMIMEType pluginType, uint16 mode,
	int16 argc, char* argn[], char* argv[], NPSavedData* saved)
	{ return fNPPluginFuncs.newp(pluginType, &fNPP_t, mode, argc, argn, argv, saved); }

NPError Host::Guest_Destroy(NPSavedData** save)
	{
	NPError result = NPERR_GENERIC_ERROR;
	*save = nil;
	if (fNPP_t.pdata)
		{
		result = fNPPluginFuncs.destroy(&fNPP_t, save);
		fNPP_t.pdata = nil;
		}
	return result;
	}

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
