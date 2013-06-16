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

#include "zoolib/netscape/ZNetscape_Host.h"
#include "zoolib/netscape/ZNetscape_ObjectPriv.h"
#include "zoolib/netscape/ZNetscape_VariantPriv.h"

#include "zoolib/ZDebug.h"
#include "zoolib/ZMemory.h" // For ZBlockZero
#include "zoolib/netscape/ZNetscape_Macros.h"
#include "zoolib/ZUtil_STL_vector.h"

#include <stdexcept>
#include <stdlib.h> // For free/malloc

#define ZMACRO_Netscape_Before_Object(a) \
	try {

namespace ZooLib {
namespace ZNetscape {

// =================================================================================================
// MARK: - NPVariantH

// Explicitly instantiate NPVariant_T<NPObjectH>, aka NPVariantH
template class NPVariant_T<NPObjectH>;

// Provide implementation of NPVariantBase::QGet<ZRef<NPObjectH> >
template <>
const ZQ<ZRef<NPObjectH> > NPVariantBase::QGet<ZRef<NPObjectH> >() const
	{
	if (type == NPVariantType_Object)
		return ZRef<NPObjectH>(static_cast<NPObjectH*>(value.objectValue));
	return null;
	}

template <>
void sVariantRelease_T(NPVariantH& iNPVariantH)
	{ HostMeister::sGet()->ReleaseVariantValue(&iNPVariantH); }

template <>
void* sMalloc_T(NPVariantH&, size_t iLength)
	{ return HostMeister::sGet()->MemAlloc(iLength); }

template <>
void sFree_T<NPVariantH>(void* iPtr)
	{ return HostMeister::sGet()->MemFree(iPtr); }

// =================================================================================================
// MARK: - NPObjectH

// Explicitly instantiate NPObject_T<NPVariantH>
template class NPObject_T<NPVariantH>;

void sRetain(NPObjectH& iOb)
	{ iOb.Retain(); }

void sRelease(NPObjectH& iOb)
	{ iOb.Release(); }

static NPP fake = nullptr;

NPObjectH::NPObjectH()
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

int32 NPObjectH::sAsInt(NPIdentifier iNPI)
	{ return HostMeister::sGet()->IntFromIdentifier(iNPI); }

NPIdentifier NPObjectH::sAsNPI(const string& iName)
	{ return HostMeister::sGet()->GetStringIdentifier(iName.c_str()); }

NPIdentifier NPObjectH::sAsNPI(int32 iInt)
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

bool NPObjectH::Enumerate(NPIdentifier*& oIdentifiers, uint32& oCount)
	{ return HostMeister::sGet()->Enumerate(fake, this, &oIdentifiers, &oCount); }

// =================================================================================================
// MARK: - ObjectH

NPClass_Z ObjectH::spNPClass(
	spAllocate,
	spDeallocate,
	spInvalidate,
	spHasMethod,
	spInvoke,
	spInvokeDefault,
	spHasProperty,
	spGetProperty,
	spSetProperty,
	spRemoveProperty,
	spEnumerate);

ObjectH::ObjectH()
	{
	this->_class = &spNPClass;
	this->referenceCount = 0;
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
	{
	using std::vector;
	vector<string> theNames;
	if (this->Imp_Enumerate(theNames))
		return ZUtil_STL::sContains(theNames, iName);

	return false;
	}

bool ObjectH::Imp_HasProperty(int32 iInt)
	{ return false; }

bool ObjectH::Imp_GetProperty(const string& iName, NPVariantH& oResult)
	{ return false; }

bool ObjectH::Imp_GetProperty(int32 iInt, NPVariantH& oResult)
	{ return false; }

bool ObjectH::Imp_SetProperty(const string& iName, const NPVariantH& iValue)
	{ return false; }

bool ObjectH::Imp_SetProperty(int32 iInt, const NPVariantH& iValue)
	{ return false; }

bool ObjectH::Imp_RemoveProperty(const string& iName)
	{ return false; }

bool ObjectH::Imp_RemoveProperty(int32 iInt)
	{ return false; }

bool ObjectH::Imp_Enumerate(NPIdentifier*& oIDs, uint32& oCount)
	{
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

bool ObjectH::Imp_Enumerate(std::vector<string>& oNames)
	{ return false; }

NPObject* ObjectH::spAllocate(NPP npp, NPClass *aClass)
	{
	ZUnimplemented();
	return nullptr;
	}

void ObjectH::spDeallocate(NPObject* npobj)
	{
	ZMACRO_Netscape_Before_Object(npobj)
		delete static_cast<ObjectH*>(npobj);
	ZMACRO_Netscape_After_Void
	}

void ObjectH::spInvalidate(NPObject* npobj)
	{
	ZMACRO_Netscape_Before_Object(npobj)
		static_cast<ObjectH*>(npobj)->Imp_Invalidate();
	ZMACRO_Netscape_After_Void
	}

bool ObjectH::spHasMethod(NPObject* npobj, NPIdentifier name)
	{
	ZMACRO_Netscape_Before_Object(npobj)
		return static_cast<ObjectH*>(npobj)->Imp_HasMethod(sAsString(name));
	ZMACRO_Netscape_After_Return_False
	}

bool ObjectH::spInvoke(NPObject* npobj,
	NPIdentifier name, const NPVariant* args, unsigned argCount, NPVariant* result)
	{
	ZMACRO_Netscape_Before_Object(npobj)
		return static_cast<ObjectH*>(npobj)->Imp_Invoke(
			sAsString(name),
			static_cast<const NPVariantH*>(args),
			argCount,
			*static_cast<NPVariantH*>(result));
	ZMACRO_Netscape_After_Return_False
	}

bool ObjectH::spInvokeDefault(NPObject* npobj,
	const NPVariant* args, unsigned argCount, NPVariant* result)
	{
	ZMACRO_Netscape_Before_Object(npobj)
		return static_cast<ObjectH*>(npobj)->Imp_InvokeDefault(
			static_cast<const NPVariantH*>(args),
			argCount,
			*static_cast<NPVariantH*>(result));
	ZMACRO_Netscape_After_Return_False
	}

bool ObjectH::spHasProperty(NPObject* npobj, NPIdentifier name)
	{
	ZMACRO_Netscape_Before_Object(npobj)
		{
		if (sIsString(name))
			return static_cast<ObjectH*>(npobj)->Imp_HasProperty(sAsString(name));
		else
			return static_cast<ObjectH*>(npobj)->Imp_HasProperty(sAsInt(name));
		}
	ZMACRO_Netscape_After_Return_False
	}

bool ObjectH::spGetProperty(NPObject* npobj, NPIdentifier name, NPVariant* result)
	{
	ZMACRO_Netscape_Before_Object(npobj)
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
	ZMACRO_Netscape_After_Return_False
	}

bool ObjectH::spSetProperty(NPObject* npobj, NPIdentifier name, const NPVariant* value)
	{
	ZMACRO_Netscape_Before_Object(npobj)
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
	ZMACRO_Netscape_After_Return_False
	}

bool ObjectH::spRemoveProperty(NPObject* npobj, NPIdentifier name)
	{
	ZMACRO_Netscape_Before_Object(npobj)
		if (sIsString(name))
			return static_cast<ObjectH*>(npobj)->Imp_RemoveProperty(sAsString(name));
		else
			return static_cast<ObjectH*>(npobj)->Imp_RemoveProperty(sAsInt(name));
	ZMACRO_Netscape_After_Return_False
	}

bool ObjectH::spEnumerate(NPObject* npobj, NPIdentifier** oIdentifiers, uint32_t* oCount)
	{
	ZMACRO_Netscape_Before_Object(npobj)
		return static_cast<ObjectH*>(npobj)->Imp_Enumerate(*oIdentifiers, *(uint32*)oCount);
	ZMACRO_Netscape_After_Return_False
	}

// =================================================================================================
// MARK: - HostMeister

static HostMeister* spHostMeister;

HostMeister* HostMeister::sGet()
	{
	ZAssert(spHostMeister);
	return spHostMeister;
	}

Host* HostMeister::sHostFromNPP(NPP npp)
	{
	if (npp)
		return static_cast<Host*>(npp->ndata);
	return nullptr;
	}

Host* HostMeister::sHostFromStream(NPStream* iNPStream)
	{
	ZUnimplemented();
	return nullptr;
	}

void HostMeister::sGetNPNF(NPNetscapeFuncs_Z& oNPNF)
	{
	sMemZero_T(oNPNF);

	oNPNF.version = NP_VERSION_MINOR;//NPVERS_HAS_PLUGIN_THREAD_ASYNC_CALL;

	oNPNF.size = sizeof(oNPNF);

	oNPNF.geturl = spGetURL;
	oNPNF.posturl = spPostURL;
	oNPNF.requestread = spRequestRead;
	oNPNF.newstream = spNewStream;
	oNPNF.write = (NPN_WriteProcPtr)spWrite;
	oNPNF.destroystream = spDestroyStream;
	oNPNF.status = spStatus;
	oNPNF.uagent = spUserAgent;
	oNPNF.memalloc = spMemAlloc;
	oNPNF.memfree = spMemFree;
	oNPNF.memflush = spMemFlush;
	oNPNF.reloadplugins = spReloadPlugins;

	// Mozilla return value is a JRIEnv
	#if defined(NewNPN_GetJavaEnvProc)
		oNPNF.getJavaEnv = (NPN_GetJavaEnvUPP)spGetJavaEnv;
	#else
		oNPNF.getJavaEnv = (NPN_GetJavaEnvProcPtr)spGetJavaEnv;
	#endif

	// Mozilla return value is a jref
	#if defined(NewNPN_GetJavaPeerProc)
		oNPNF.getJavaPeer = (NPN_GetJavaPeerUPP)spGetJavaPeer;
	#else
		oNPNF.getJavaPeer = (NPN_GetJavaPeerProcPtr)spGetJavaPeer;
	#endif

	oNPNF.geturlnotify = spGetURLNotify;
	oNPNF.posturlnotify = spPostURLNotify;
	oNPNF.getvalue = spGetValue;
	oNPNF.setvalue = spSetValue;
	oNPNF.invalidaterect = spInvalidateRect;
	oNPNF.invalidateregion = spInvalidateRegion;
	oNPNF.forceredraw = spForceRedraw;

	oNPNF.getstringidentifier = spGetStringIdentifier;
	oNPNF.getstringidentifiers = spGetStringIdentifiers;
	oNPNF.getintidentifier = spGetIntIdentifier;
	oNPNF.identifierisstring = spIdentifierIsString;
	oNPNF.utf8fromidentifier = spUTF8FromIdentifier;

	// WebKit/10.4 return value is (incorrectly) an NPIdentifier.
	#if defined(NewNPN_IntFromIdentifierProc)
		oNPNF.intfromidentifier = spIntFromIdentifier;
	#else
		oNPNF.intfromidentifier = (NPN_IntFromIdentifierProcPtr)spIntFromIdentifier;
	#endif

	oNPNF.createobject = spCreateObject;
	oNPNF.retainobject = spRetainObject;
	oNPNF.releaseobject = spReleaseObject;
	oNPNF.invoke = spInvoke;
	oNPNF.invokeDefault = spInvokeDefault;
	oNPNF.evaluate = spEvaluate;
	oNPNF.getproperty = spGetProperty;
	oNPNF.setproperty = spSetProperty;
	oNPNF.removeproperty = spRemoveProperty;
	oNPNF.hasproperty = spHasProperty;
	oNPNF.hasmethod = spHasMethod;
	oNPNF.releasevariantvalue = spReleaseVariantValue;

	// I'm disabling setexception for now -- it's defined as taking
	// NPString* or UTF8* in different versions of headers, and I
	// haven't determined which is correct, or indeed if it's a host-specific thing.
	oNPNF.setexception = nullptr;

	// Mozilla return value is a bool
	#if defined(NewNPN_PushPopupsEnabledStateProc)
		oNPNF.pushpopupsenabledstate = (NPN_PushPopupsEnabledStateUPP)spPushPopupsEnabledState;
	#else
		oNPNF.pushpopupsenabledstate = spPushPopupsEnabledState;
	#endif

	// Mozilla return value is a bool
	#if defined(NewNPN_PopPopupsEnabledStateProc)
		oNPNF.poppopupsenabledstate = (NPN_PopPopupsEnabledStateUPP)spPopPopupsEnabledState;
	#else
		oNPNF.poppopupsenabledstate = spPopPopupsEnabledState;
	#endif

	oNPNF.enumerate = spEnumerate;
	oNPNF.pluginthreadasynccall = spPluginThreadAsyncCall;
	oNPNF.construct = spConstruct;
	oNPNF.scheduletimer = spScheduleTimer;
	oNPNF.unscheduletimer = spUnscheduleTimer;
	}

HostMeister::HostMeister()
	{
	ZAssert(!spHostMeister);
	spHostMeister = this;
	}

HostMeister::~HostMeister()
	{
	ZAssert(spHostMeister == this);
	spHostMeister = nullptr;
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

NPError HostMeister::spGetURL(NPP npp, const char* URL, const char* window)
	{
	ZMACRO_Netscape_Before
		return sGet()->GetURL(npp, URL, window);
	ZMACRO_Netscape_After_NPError
	}

NPError HostMeister::spPostURL(NPP npp,
	const char* URL, const char* window, ::uint32 len, const char* buf, NPBool file)
	{
	ZMACRO_Netscape_Before
		return sGet()->PostURL(npp, URL, window, len, buf, file);
	ZMACRO_Netscape_After_NPError
	}

NPError HostMeister::spRequestRead(NPStream* stream, NPByteRange* rangeList)
	{
	ZMACRO_Netscape_Before
		return sGet()->RequestRead(stream, rangeList);
	ZMACRO_Netscape_After_NPError
	}

NPError HostMeister::spNewStream(NPP npp,
	NPMIMEType type, const char* window, NPStream** stream)
	{
	ZMACRO_Netscape_Before
		return sGet()->NewStream(npp, type, window, stream);
	ZMACRO_Netscape_After_NPError
	}

int32 HostMeister::spWrite(NPP npp, NPStream* stream, int32 len, void* buffer)
	{
	ZMACRO_Netscape_Before
		return sGet()->Write(npp, stream, len, buffer);
	ZMACRO_Netscape_After_Return(-1)
	}

NPError HostMeister::spDestroyStream(NPP npp, NPStream* stream, NPReason reason)
	{
	ZMACRO_Netscape_Before
		return sGet()->DestroyStream(npp, stream, reason);
	ZMACRO_Netscape_After_NPError
	}

void HostMeister::spStatus(NPP npp, const char* message)
	{
	ZMACRO_Netscape_Before
		return sGet()->Status(npp, message);
	ZMACRO_Netscape_After_Void
	}

const char* HostMeister::spUserAgent(NPP npp)
	{
	ZMACRO_Netscape_Before
		return sGet()->UserAgent(npp);
	ZMACRO_Netscape_After_Return_Nil
	}

void* HostMeister::spMemAlloc(::uint32 size)
	{
	ZMACRO_Netscape_Before
		return sGet()->MemAlloc(size);
	ZMACRO_Netscape_After_Return_Nil
	}

void HostMeister::spMemFree(void* ptr)
	{
	ZMACRO_Netscape_Before
		sGet()->MemFree(ptr);
	ZMACRO_Netscape_After_Void
	}

::uint32 HostMeister::spMemFlush(::uint32 size)
	{
	ZMACRO_Netscape_Before
		return sGet()->MemFlush(size);
	ZMACRO_Netscape_After_Return(0)
	}

void HostMeister::spReloadPlugins(NPBool reloadPages)
	{
	ZMACRO_Netscape_Before
		sGet()->ReloadPlugins(reloadPages);
	ZMACRO_Netscape_After_Void
	}

void* HostMeister::spGetJavaEnv()
	{
	ZMACRO_Netscape_Before
		return sGet()->GetJavaEnv();
	ZMACRO_Netscape_After_Return_Nil
	}

void* HostMeister::spGetJavaPeer(NPP npp)
	{
	ZMACRO_Netscape_Before
		return sGet()->GetJavaPeer(npp);
	ZMACRO_Netscape_After_Return_Nil
	}

NPError HostMeister::spGetURLNotify(NPP npp,
	const char* URL, const char* window, void* notifyData)
	{
	ZMACRO_Netscape_Before
		return sGet()->GetURLNotify(npp, URL, window, notifyData);
	ZMACRO_Netscape_After_NPError
	}

NPError HostMeister::spPostURLNotify(NPP npp,
	const char* URL, const char* window,
	::uint32 len, const char* buf, NPBool file, void* notifyData)
	{
	ZMACRO_Netscape_Before
		return sGet()->PostURLNotify(npp, URL, window, len, buf, file, notifyData);
	ZMACRO_Netscape_After_NPError
	}

NPError HostMeister::spGetValue(NPP npp, NPNVariable variable, void* ret_value)
	{
	ZMACRO_Netscape_Before
		return sGet()->GetValue(npp, variable, ret_value);
	ZMACRO_Netscape_After_NPError
	}

NPError HostMeister::spSetValue(NPP npp, NPPVariable variable, void* value)
	{
	ZMACRO_Netscape_Before
		return sGet()->SetValue(npp, variable, value);
	ZMACRO_Netscape_After_NPError
	}

void HostMeister::spInvalidateRect(NPP npp, NPRect* rect)
	{
	ZMACRO_Netscape_Before
		return sGet()->InvalidateRect(npp, rect);
	ZMACRO_Netscape_After_Void
	}

void HostMeister::spInvalidateRegion(NPP npp, NPRegion region)
	{
	ZMACRO_Netscape_Before
		return sGet()->InvalidateRegion(npp, region);
	ZMACRO_Netscape_After_Void
	}

void HostMeister::spForceRedraw(NPP npp)
	{
	ZMACRO_Netscape_Before
		return sGet()->ForceRedraw(npp);
	ZMACRO_Netscape_After_Void
	}

NPIdentifier HostMeister::spGetStringIdentifier(const NPUTF8* name)
	{
	ZMACRO_Netscape_Before
		return sGet()->GetStringIdentifier(name);
	ZMACRO_Netscape_After_Return_Nil
	}

void HostMeister::spGetStringIdentifiers(
	const NPUTF8** names, int32_t nameCount, NPIdentifier* identifiers)
	{
	ZMACRO_Netscape_Before
		sGet()->GetStringIdentifiers(names, nameCount, identifiers);
	ZMACRO_Netscape_After_Void
	}

NPIdentifier HostMeister::spGetIntIdentifier(int32_t intid)
	{
	ZMACRO_Netscape_Before
		return sGet()->GetIntIdentifier(intid);
	ZMACRO_Netscape_After_Return_Nil
	}

bool HostMeister::spIdentifierIsString(NPIdentifier identifier)
	{
	ZMACRO_Netscape_Before
		return sGet()->IdentifierIsString(identifier);
	ZMACRO_Netscape_After_Return_False
	}

NPUTF8* HostMeister::spUTF8FromIdentifier(NPIdentifier identifier)
	{
	ZMACRO_Netscape_Before
		return sGet()->UTF8FromIdentifier(identifier);
	ZMACRO_Netscape_After_Return_Nil
	}

int32_t HostMeister::spIntFromIdentifier(NPIdentifier identifier)
	{
	ZMACRO_Netscape_Before
		return sGet()->IntFromIdentifier(identifier);
	ZMACRO_Netscape_After_Return(0xFFFFFFFF)
	}

NPObject* HostMeister::spCreateObject(NPP npp, NPClass* aClass)
	{
	ZMACRO_Netscape_Before
		return sGet()->CreateObject(npp, aClass);
	ZMACRO_Netscape_After_Return_Nil
	}

NPObject* HostMeister::spRetainObject(NPObject* obj)
	{
	ZMACRO_Netscape_Before
		return sGet()->RetainObject(obj);
	ZMACRO_Netscape_After_Return_Nil
	}

void HostMeister::spReleaseObject(NPObject* obj)
	{
	ZMACRO_Netscape_Before
		sGet()->ReleaseObject(obj);
	ZMACRO_Netscape_After_Void
	}

bool HostMeister::spInvoke(NPP npp,
	NPObject* obj, NPIdentifier methodName, const NPVariant* args, unsigned argCount,
	NPVariant* result)
	{
	ZMACRO_Netscape_Before
		return sGet()->Invoke(npp, obj, methodName, args, argCount, result);
	ZMACRO_Netscape_After_Return_False
	}

bool HostMeister::spInvokeDefault(NPP npp,
	NPObject* obj, const NPVariant* args, unsigned argCount, NPVariant* result)
	{
	ZMACRO_Netscape_Before
		return sGet()->InvokeDefault(npp, obj, args, argCount, result);
	ZMACRO_Netscape_After_Return_False
	}

bool HostMeister::spEvaluate(NPP npp,
	NPObject* obj, NPString* script, NPVariant* result)
	{
	ZMACRO_Netscape_Before
		return sGet()->Evaluate(npp, obj, script, result);
	ZMACRO_Netscape_After_Return_False
	}

bool HostMeister::spGetProperty(NPP npp,
	NPObject* obj, NPIdentifier propertyName, NPVariant* result)
	{
	ZMACRO_Netscape_Before
		return sGet()->GetProperty(npp, obj, propertyName, result);
	ZMACRO_Netscape_After_Return_False
	}

bool HostMeister::spSetProperty(NPP npp,
	NPObject* obj, NPIdentifier propertyName, const NPVariant* value)
	{
	ZMACRO_Netscape_Before
		return sGet()->SetProperty(npp, obj, propertyName, value);
	ZMACRO_Netscape_After_Return_False
	}

bool HostMeister::spRemoveProperty(NPP npp, NPObject* obj, NPIdentifier propertyName)
	{
	ZMACRO_Netscape_Before
		return sGet()->RemoveProperty(npp, obj, propertyName);
	ZMACRO_Netscape_After_Return_False
	}

bool HostMeister::spHasProperty(NPP npp, NPObject* npobj, NPIdentifier propertyName)
	{
	ZMACRO_Netscape_Before
		return sGet()->HasProperty(npp, npobj, propertyName);
	ZMACRO_Netscape_After_Return_False
	}

bool HostMeister::spHasMethod(NPP npp, NPObject* npobj, NPIdentifier methodName)
	{
	ZMACRO_Netscape_Before
		return sGet()->HasMethod(npp, npobj, methodName);
	ZMACRO_Netscape_After_Return_False
	}

void HostMeister::spReleaseVariantValue(NPVariant* variant)
	{
	ZMACRO_Netscape_Before
		sGet()->ReleaseVariantValue(variant);
	ZMACRO_Netscape_After_Void
	}

void HostMeister::spSetException(NPObject* obj, const NPUTF8* message)
	{
	ZMACRO_Netscape_Before
		sGet()->SetException(obj, message);
	ZMACRO_Netscape_After_Void
	}

void HostMeister::spSetExceptionNPString(NPObject* obj, NPString* message)
	{
	ZMACRO_Netscape_Before
		sGet()->SetException(obj, sNPStringChars(*message));
	ZMACRO_Netscape_After_Void
	}

void HostMeister::spPushPopupsEnabledState(NPP npp, NPBool enabled)
	{
	ZMACRO_Netscape_Before
		sGet()->PushPopupsEnabledState(npp, enabled);
	ZMACRO_Netscape_After_Void
	}

void HostMeister::spPopPopupsEnabledState(NPP npp)
	{
	ZMACRO_Netscape_Before
		sGet()->PopPopupsEnabledState(npp);
	ZMACRO_Netscape_After_Void
	}

bool HostMeister::spEnumerate(
	NPP npp, NPObject *npobj, NPIdentifier **identifier, uint32_t *count)
	{
	ZMACRO_Netscape_Before
		return sGet()->Enumerate(npp, npobj, identifier, (uint32*)count);
	ZMACRO_Netscape_After_Return_False
	}

void HostMeister::spPluginThreadAsyncCall(
	NPP npp, void (*func)(void *), void *userData)
	{
	ZMACRO_Netscape_Before
		sGet()->PluginThreadAsyncCall(npp, func, userData);
	ZMACRO_Netscape_After_Void
	}

bool HostMeister::spConstruct(
	NPP npp, NPObject* obj, const NPVariant *args, uint32_t argCount, NPVariant *result)
	{
	ZMACRO_Netscape_Before
		sGet()->Construct(npp, obj, args, argCount, result);
	ZMACRO_Netscape_After_Return_False
	}

uint32 HostMeister::spScheduleTimer(
	NPP npp, uint32 interval, NPBool repeat, void (*timerFunc)(NPP npp, uint32 timerID))
	{
	ZMACRO_Netscape_Before
		return sGet()->ScheduleTimer(npp, interval, repeat, timerFunc);
	ZMACRO_Netscape_After_Return(0)
	}

void HostMeister::spUnscheduleTimer(NPP npp, uint32 timerID)
	{
	ZMACRO_Netscape_Before
		return sGet()->UnscheduleTimer(npp, timerID);
	ZMACRO_Netscape_After_Void
	}

// =================================================================================================
// MARK: - GuestFactory

GuestFactory::GuestFactory()
	{}

GuestFactory::~GuestFactory()
	{}

void GuestFactory::GetNPNF(NPNetscapeFuncs_Z& oNPNF)
	{ HostMeister::sGet()->sGetNPNF(oNPNF); }

// =================================================================================================
// MARK: - Host

Host::Host(ZRef<GuestFactory> iGuestFactory)
:	fGuestFactory(iGuestFactory),
	fNPPluginFuncs(fGuestFactory->GetEntryPoints())
	{
	sMemZero_T(fNPP_t);
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
	*save = nullptr;
	if (fNPP_t.pdata)
		{
		result = fNPPluginFuncs.destroy(&fNPP_t, save);
		fNPP_t.pdata = nullptr;
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
	{
	if (fNPPluginFuncs.getvalue)
		return fNPPluginFuncs.getvalue(&fNPP_t, iNPPVariable, oValue);
	return NPERR_GENERIC_ERROR;
	}

NPError Host::Guest_SetValue(NPNVariable iNPNVariable, void* iValue)
	{
	if (fNPPluginFuncs.setvalue)
		return fNPPluginFuncs.setvalue(&fNPP_t, iNPNVariable, iValue);
	return NPERR_GENERIC_ERROR;
	}

} // namespace ZNetscape
} // namespace ZooLib
