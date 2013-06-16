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

#include "zoolib/netscape/ZNetscape_Guest.h"
#include "zoolib/netscape/ZNetscape_ObjectPriv.h"
#include "zoolib/netscape/ZNetscape_VariantPriv.h"
#include "zoolib/ZCompat_algorithm.h"
#include "zoolib/ZDebug.h"
#include "zoolib/ZMemory.h"
#include "zoolib/netscape/ZNetscape_Macros.h"
#include "zoolib/ZUtil_STL_vector.h"

#include <string>

using std::min;
using std::string;

#define ZMACRO_Netscape_Before_Object(a) \
	SetRestoreNPP theSetRestoreNPP(static_cast<ObjectG*>(a)->GetNPP()); \
	try {

#define ZMACRO_Netscape_Before_GuestMeister(npp) \
	SetRestoreNPP theSetRestoreNPP(npp);\
	try {

namespace ZooLib {
namespace ZNetscape {

// =================================================================================================
// MARK: - SetRestoreNPP

static NPP spNPP;

SetRestoreNPP::SetRestoreNPP(NPP iNPP)
:	fPrior(spNPP)
	{ spNPP = iNPP; }

SetRestoreNPP::~SetRestoreNPP()
	{ spNPP = fPrior; }

NPP SetRestoreNPP::sCurrent()
	{ return spNPP; }

// =================================================================================================
// MARK: - NPVariantG

// Explicitly instantiate NPVariant_T<NPObjectG>, aka NPVariantG
template class NPVariant_T<NPObjectG>;

// Provide implementation of NPVariantBase::QGet<ZRef<NPObjectG> >
template <>
const ZQ<ZRef<NPObjectG> > NPVariantBase::QGet<ZRef<NPObjectG> >() const
	{
	if (type == NPVariantType_Object)
		return ZRef<NPObjectG>(static_cast<NPObjectG*>(value.objectValue));
	return null;
	}

template <>
void sVariantRelease_T(NPVariantG& iNPVariantG)
	{ GuestMeister::sGet()->Host_ReleaseVariantValue(&iNPVariantG); }

template <>
void* sMalloc_T(NPVariantG&, size_t iLength)
	{ return GuestMeister::sGet()->Host_MemAlloc(iLength); }

template <>
void sFree_T<NPVariantG>(void* iPtr)
	{ return GuestMeister::sGet()->Host_MemFree(iPtr); }

// =================================================================================================
// MARK: - NPObjectG

// Explicitly instantiate NPObject_T<NPVariantG>
template class NPObject_T<NPVariantG>;
 
 void sRetain(NPObjectG& iOb)
	{ iOb.Retain(); }

void sRelease(NPObjectG& iOb)
	{ iOb.Release(); }

NPObjectG::NPObjectG()
	{}

bool NPObjectG::sIsString(NPIdentifier iNPI)
	{ return GuestMeister::sGet()->Host_IdentifierIsString(iNPI); }

string NPObjectG::sAsString(NPIdentifier iNPI)
	{
	string result;
	GuestMeister* theGM = GuestMeister::sGet();
	if (NPUTF8* theString = theGM->Host_UTF8FromIdentifier(iNPI))
		{
		result = theString;
		theGM->Host_MemFree(theString);
		}
	return result;
	}

int32 NPObjectG::sAsInt(NPIdentifier iNPI)
	{ return GuestMeister::sGet()->Host_IntFromIdentifier(iNPI); }

NPIdentifier NPObjectG::sAsNPI(const string& iName)
	{ return GuestMeister::sGet()->Host_GetStringIdentifier(iName.c_str()); }

NPIdentifier NPObjectG::sAsNPI(int32 iInt)
	{ return GuestMeister::sGet()->Host_GetIntIdentifier(iInt); }

void NPObjectG::Retain()
	{ GuestMeister::sGet()->Host_RetainObject(this); }

void NPObjectG::Release()
	{ GuestMeister::sGet()->Host_ReleaseObject(this); }

bool NPObjectG::HasMethod(const string& iName)
	{ return GuestMeister::sGet()->Host_HasMethod(SetRestoreNPP::sCurrent(), this, sAsNPI(iName)); }

bool NPObjectG::Invoke(
	const string& iName, const NPVariantG* iArgs, size_t iCount, NPVariantG& oResult)
	{
	return GuestMeister::sGet()->Host_Invoke(
		SetRestoreNPP::sCurrent(), this, sAsNPI(iName), iArgs, iCount, &oResult);
	}

bool NPObjectG::InvokeDefault(const NPVariantG* iArgs, size_t iCount, NPVariantG& oResult)
	{ return GuestMeister::sGet()->Host_InvokeDefault(
		SetRestoreNPP::sCurrent(), this, iArgs, iCount, &oResult); }

bool NPObjectG::HasProperty(const string& iName)
	{
	return GuestMeister::sGet()->Host_HasProperty(SetRestoreNPP::sCurrent(), this, sAsNPI(iName));
	}

bool NPObjectG::HasProperty(size_t iIndex)
	{
	return GuestMeister::sGet()->Host_HasProperty(SetRestoreNPP::sCurrent(), this, sAsNPI(iIndex));
	}

bool NPObjectG::GetProperty(const string& iName, NPVariantG& oResult)
	{ return GuestMeister::sGet()->Host_GetProperty(
		SetRestoreNPP::sCurrent(), this, sAsNPI(iName), &oResult); }

bool NPObjectG::GetProperty(size_t iIndex, NPVariantG& oResult)
	{ return GuestMeister::sGet()->Host_GetProperty(
		SetRestoreNPP::sCurrent(), this, sAsNPI(iIndex), &oResult); }

bool NPObjectG::SetProperty(const string& iName, const NPVariantG& iValue)
	{ return GuestMeister::sGet()->Host_SetProperty(
		SetRestoreNPP::sCurrent(), this, sAsNPI(iName), &iValue); }

bool NPObjectG::SetProperty(size_t iIndex, const NPVariantG& iValue)
	{ return GuestMeister::sGet()->Host_SetProperty(
		SetRestoreNPP::sCurrent(), this, sAsNPI(iIndex), &iValue); }

bool NPObjectG::RemoveProperty(const string& iName)
	{ return GuestMeister::sGet()->Host_RemoveProperty(
		SetRestoreNPP::sCurrent(), this, sAsNPI(iName)); }

bool NPObjectG::RemoveProperty(size_t iIndex)
	{ return GuestMeister::sGet()->Host_RemoveProperty(
		SetRestoreNPP::sCurrent(), this, sAsNPI(iIndex)); }

bool NPObjectG::Enumerate(NPIdentifier*& oIdentifiers, uint32& oCount)
	{ return GuestMeister::sGet()->Host_Enumerate(
		SetRestoreNPP::sCurrent(), this, &oIdentifiers, &oCount); }

// =================================================================================================
// MARK: - ObjectG

NPClass_Z ObjectG::spNPClass(
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

NPP ObjectG::GetNPP()
	{ return fNPP; }

ObjectG::ObjectG()
	{
	this->_class = &spNPClass;
	this->referenceCount = 0;
	fNPP = SetRestoreNPP::sCurrent();
	}

ObjectG::~ObjectG()
	{}

void ObjectG::Imp_Invalidate()
	{}

bool ObjectG::Imp_HasMethod(const std::string& iName)
	{ return false; }

bool ObjectG::Imp_Invoke(
	const std::string& iName, const NPVariantG* iArgs, size_t iCount, NPVariantG& oResult)
	{ return false; }

bool ObjectG::Imp_InvokeDefault(const NPVariantG* iArgs, size_t iCount, NPVariantG& oResult)
	{ return false; }

bool ObjectG::Imp_HasProperty(const std::string& iName)
	{
	using std::string;
	using std::vector;
	vector<string> theNames;
	if (this->Imp_Enumerate(theNames))
		return ZUtil_STL::sContains(theNames, iName);

	return false;
	}

bool ObjectG::Imp_HasProperty(int32 iInt)
	{ return false; }

bool ObjectG::Imp_GetProperty(const std::string& iName, NPVariantG& oResult)
	{ return false; }

bool ObjectG::Imp_GetProperty(int32 iInt, NPVariantG& oResult)
	{ return false; }

bool ObjectG::Imp_SetProperty(const std::string& iName, const NPVariantG& iValue)
	{ return false; }

bool ObjectG::Imp_SetProperty(int32 iInt, const NPVariantG& iValue)
	{ return false; }

bool ObjectG::Imp_RemoveProperty(const std::string& iName)
	{ return false; }

bool ObjectG::Imp_RemoveProperty(int32 iInt)
	{ return false; }

bool ObjectG::Imp_Enumerate(NPIdentifier*& oIDs, uint32& oCount)
	{
	using std::string;
	using std::vector;
	vector<string> theNames;
	if (this->Imp_Enumerate(theNames))
		{
		oCount = theNames.size();
		void* p = GuestMeister::sGet()->Host_MemAlloc(sizeof(NPIdentifier) * theNames.size());
		oIDs = static_cast<NPIdentifier*>(p);
		for (size_t x = 0; x < oCount; ++x)
			oIDs[x] = sAsNPI(theNames[x]);

		return true;
		}
	return false;
	}

bool ObjectG::Imp_Enumerate(std::vector<std::string>& oNames)
	{ return false; }

NPObject* ObjectG::spAllocate(NPP npp, NPClass *aClass)
	{
	ZUnimplemented();
	return nullptr;
	}

void ObjectG::spDeallocate(NPObject* npobj)
	{
	ZMACRO_Netscape_Before_Object(npobj)
		delete static_cast<ObjectG*>(npobj);
	ZMACRO_Netscape_After_Void
	}

void ObjectG::spInvalidate(NPObject* npobj)
	{
	ZMACRO_Netscape_Before_Object(npobj)
		static_cast<ObjectG*>(npobj)->Imp_Invalidate();
	ZMACRO_Netscape_After_Void
	}

bool ObjectG::spHasMethod(NPObject* npobj, NPIdentifier name)
	{
	ZMACRO_Netscape_Before_Object(npobj)
		return static_cast<ObjectG*>(npobj)->Imp_HasMethod(sAsString(name));
	ZMACRO_Netscape_After_Return_False
	}

bool ObjectG::spInvoke(NPObject* npobj,
	NPIdentifier name, const NPVariant* args, unsigned argCount, NPVariant* result)
	{
	ZMACRO_Netscape_Before_Object(npobj)
		return static_cast<ObjectG*>(npobj)->Imp_Invoke(
			sAsString(name),
			static_cast<const NPVariantG*>(args),
			argCount,
			*static_cast<NPVariantG*>(result));
	ZMACRO_Netscape_After_Return_False
	}

bool ObjectG::spInvokeDefault(NPObject* npobj,
	const NPVariant* args, unsigned argCount, NPVariant* result)
	{
	ZMACRO_Netscape_Before_Object(npobj)
		return static_cast<ObjectG*>(npobj)->Imp_InvokeDefault(
			static_cast<const NPVariantG*>(args),
			argCount,
			*static_cast<NPVariantG*>(result));
	ZMACRO_Netscape_After_Return_False
	}

bool ObjectG::spHasProperty(NPObject* npobj, NPIdentifier name)
	{
	ZMACRO_Netscape_Before_Object(npobj)
		{
		if (sIsString(name))
			return static_cast<ObjectG*>(npobj)->Imp_HasProperty(sAsString(name));
		else
			return static_cast<ObjectG*>(npobj)->Imp_HasProperty(sAsInt(name));
		}
	ZMACRO_Netscape_After_Return_False
	}

bool ObjectG::spGetProperty(NPObject* npobj, NPIdentifier name, NPVariant* result)
	{
	ZMACRO_Netscape_Before_Object(npobj)
		if (sIsString(name))
			{
			return static_cast<ObjectG*>(npobj)->Imp_GetProperty(
				sAsString(name),
				*static_cast<NPVariantG*>(result));
			}
		else
			{
			return static_cast<ObjectG*>(npobj)->Imp_GetProperty(
				sAsInt(name),
				*static_cast<NPVariantG*>(result));
			}
	ZMACRO_Netscape_After_Return_False
	}

bool ObjectG::spSetProperty(NPObject* npobj, NPIdentifier name, const NPVariant* value)
	{
	ZMACRO_Netscape_Before_Object(npobj)
		if (sIsString(name))
			{
			return static_cast<ObjectG*>(npobj)->Imp_SetProperty(
				sAsString(name),
				*static_cast<const NPVariantG*>(value));
			}
		else
			{
			return static_cast<ObjectG*>(npobj)->Imp_SetProperty(
				sAsInt(name),
				*static_cast<const NPVariantG*>(value));
			}
	ZMACRO_Netscape_After_Return_False
	}

bool ObjectG::spRemoveProperty(NPObject* npobj, NPIdentifier name)
	{
	ZMACRO_Netscape_Before_Object(npobj)
		if (sIsString(name))
			return static_cast<ObjectG*>(npobj)->Imp_RemoveProperty(sAsString(name));
		else
			return static_cast<ObjectG*>(npobj)->Imp_RemoveProperty(sAsInt(name));
	ZMACRO_Netscape_After_Return_False
	}

bool ObjectG::spEnumerate(NPObject* npobj, NPIdentifier** oIdentifiers, uint32_t* oCount)
	{
	ZMACRO_Netscape_Before_Object(npobj)
		return static_cast<ObjectG*>(npobj)->Imp_Enumerate(*oIdentifiers, *(uint32*)oCount);
	ZMACRO_Netscape_After_Return_False
	}

// =================================================================================================
// MARK: - GuestMeister

static GuestMeister* spGuestMeister;

GuestMeister::GuestMeister()
	{
	ZAssert(!spGuestMeister);
	spGuestMeister = this;
	sMemZero_T(fNPNF);
	}

GuestMeister::~GuestMeister()
	{
	ZAssert(spGuestMeister == this);
	spGuestMeister = nullptr;
	}

GuestMeister* GuestMeister::sGet()
	{
	ZAssert(spGuestMeister);
	return spGuestMeister;
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

	oPluginFuncs->newp = spNew;
	oPluginFuncs->destroy = spDestroy;
	oPluginFuncs->setwindow = spSetWindow;
	oPluginFuncs->newstream = spNewStream;
	oPluginFuncs->destroystream = spDestroyStream;
	oPluginFuncs->asfile = spStreamAsFile;
	oPluginFuncs->writeready = (NPP_WriteReadyProcPtr)spWriteReady;
	oPluginFuncs->write = (NPP_WriteProcPtr)spWrite;
	oPluginFuncs->print = spPrint;
	oPluginFuncs->event = spHandleEvent;
	oPluginFuncs->urlnotify = spURLNotify;
	oPluginFuncs->getvalue = spGetValue;
	oPluginFuncs->setvalue = spSetValue;

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

NPError GuestMeister::Host_GetURL(NPP npp, const char* url, const char* target)
	{ return fNPNF.geturl(npp, url, target); }

NPError GuestMeister::Host_PostURL(NPP npp,
	const char* url, const char* target, uint32 len, const char* buf, NPBool file)
	{ return fNPNF.posturl(npp, url, target, len, buf, file); }

NPError GuestMeister::Host_RequestRead(NPStream* stream, NPByteRange* rangeList)
	{ return fNPNF.requestread(stream, rangeList); }

NPError GuestMeister::Host_NewStream(NPP npp,
	NPMIMEType type, const char* target, NPStream** stream)
	{ return fNPNF.newstream(npp, type, target, stream); }

int32 GuestMeister::Host_Write(NPP npp, NPStream* stream, int32 len, void* buffer)
	{ return fNPNF.write(npp, stream, len, buffer); }

NPError GuestMeister::Host_DestroyStream(NPP npp, NPStream* stream, NPReason reason)
	{ return fNPNF.destroystream(npp, stream, reason); }

void GuestMeister::Host_Status(NPP npp, const char* message)
	{ return fNPNF.status(npp, message); }

const char* GuestMeister::Host_UserAgent(NPP npp)
	{ return fNPNF.uagent(npp); }

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

jref GuestMeister::Host_GetJavaPeer(NPP npp)
	{ return fNPNF.getJavaPeer(npp); }

NPError GuestMeister::Host_GetURLNotify(NPP npp,
	const char* url, const char* target, void* notifyData)
	{ return fNPNF.geturlnotify(npp, url, target, notifyData); }

NPError GuestMeister::Host_PostURLNotify(NPP npp, const char* url, const char* target,
	uint32 len, const char* buf, NPBool file, void* notifyData)
	{ return fNPNF.posturlnotify(npp, url, target, len, buf, file, notifyData); }

NPError GuestMeister::Host_GetValue(NPP npp, NPNVariable variable, void *value)
	{ return fNPNF.getvalue(npp, variable, value); }

NPError GuestMeister::Host_SetValue(NPP npp, NPPVariable variable, void *value)
	{ return fNPNF.setvalue(npp, variable, value); }

void GuestMeister::Host_InvalidateRect(NPP npp, NPRect *invalidRect)
	{ return fNPNF.invalidaterect(npp, invalidRect); }

void GuestMeister::Host_InvalidateRegion(NPP npp, NPRegion invalidRegion)
	{ return fNPNF.invalidateregion(npp, invalidRegion); }

void GuestMeister::Host_ForceRedraw(NPP npp)
	{ return fNPNF.forceredraw(npp); }

NPIdentifier GuestMeister::Host_GetStringIdentifier(const NPUTF8* name)
	{ return fNPNF.getstringidentifier(name); }

void GuestMeister::Host_GetStringIdentifiers(
	const NPUTF8** names, int32 nameCount, NPIdentifier* identifiers)
	{ return fNPNF.getstringidentifiers(names, nameCount, identifiers); }

NPIdentifier GuestMeister::Host_GetIntIdentifier(int32 intid)
	{ return fNPNF.getintidentifier(intid); }

bool GuestMeister::Host_IdentifierIsString(NPIdentifier identifier)
	{ return fNPNF.identifierisstring(identifier); }

NPUTF8* GuestMeister::Host_UTF8FromIdentifier(NPIdentifier identifier)
	{ return fNPNF.utf8fromidentifier(identifier); }

int32 GuestMeister::Host_IntFromIdentifier(NPIdentifier identifier)
	{
	// WebKit 10.4 header is wrong -- return type is NPIdentifier, so coerce to correct type.
	return (int32)(fNPNF.intfromidentifier(identifier));
	}

NPObject* GuestMeister::Host_CreateObject(NPP npp, NPClass* aClass)
	{ return fNPNF.createobject(npp, aClass); }

NPObject* GuestMeister::Host_RetainObject(NPObject* obj)
	{ return fNPNF.retainobject(obj); }

void GuestMeister::Host_ReleaseObject(NPObject* obj)
	{ return fNPNF.releaseobject(obj); }

bool GuestMeister::Host_Invoke(NPP npp, NPObject* obj,
	NPIdentifier methodName, const NPVariant* args, unsigned argCount, NPVariant* result)
	{ return fNPNF.invoke(npp, obj, methodName, args, argCount, result); }

bool GuestMeister::Host_InvokeDefault(NPP npp,
	NPObject* obj, const NPVariant* args, unsigned argCount, NPVariant* result)
	{ return fNPNF.invokeDefault(npp, obj, args, argCount, result); }

bool GuestMeister::Host_Evaluate(NPP npp, NPObject* obj, NPString* script, NPVariant* result)
	{ return fNPNF.evaluate(npp, obj, script, result); }

bool GuestMeister::Host_GetProperty(NPP npp,
	NPObject* obj, NPIdentifier propertyName, NPVariant* result)
	{
	if (fNPNF.getproperty)
		return fNPNF.getproperty(npp, obj, propertyName, result);
	return false;
	}

bool GuestMeister::Host_SetProperty(NPP npp,
	NPObject* obj, NPIdentifier propertyName, const NPVariant* value)
	{
	if (fNPNF.setproperty)
		return fNPNF.setproperty(npp, obj, propertyName, value);
	return false;
	}

bool GuestMeister::Host_RemoveProperty(NPP npp, NPObject* obj, NPIdentifier propertyName)
	{
	if (fNPNF.removeproperty)
		return fNPNF.removeproperty(npp, obj, propertyName);
	return false;
	}

bool GuestMeister::Host_HasProperty(NPP npp, NPObject* npobj, NPIdentifier propertyName)
	{
	if (fNPNF.hasproperty)
		{
		return fNPNF.hasproperty(npp, npobj, propertyName);
		}
	else if (fNPNF.getproperty)
		{
		NPVariantG dummy;
		if (fNPNF.getproperty(npp, npobj, propertyName, &dummy))
			return !dummy.IsVoid();
		}

	return false;
	}

bool GuestMeister::Host_HasMethod(NPP npp, NPObject* npobj, NPIdentifier methodName)
	{
	if (fNPNF.hasmethod)
		{
		return fNPNF.hasmethod(npp, npobj, methodName);
		}
	else if (fNPNF.hasproperty)
		{
		return !fNPNF.hasproperty(npp, npobj, methodName);
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
		if (fNPNF.getproperty(npp, npobj, methodName, &dummy))
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

void GuestMeister::Host_PushPopupsEnabledState(NPP npp, NPBool enabled)
	{
	if (fNPNF.pushpopupsenabledstate)
		fNPNF.pushpopupsenabledstate(npp, enabled);
	}

void GuestMeister::Host_PopPopupsEnabledState(NPP npp)
	{
	if (fNPNF.poppopupsenabledstate)
		fNPNF.poppopupsenabledstate(npp);
	}

bool GuestMeister::Host_Enumerate(
	NPP npp, NPObject* npobj, NPIdentifier** identifier, uint32* count)
	{
	if (fNPNF.enumerate)
		return fNPNF.enumerate(npp, npobj, identifier, (uint32_t*)count);
	return false;
	}

void GuestMeister::Host_PluginThreadAsyncCall(NPP npp, void (*func)(void *), void *userData)
	{
	if (fNPNF.pluginthreadasynccall)
		fNPNF.pluginthreadasynccall(npp, func, userData);
	}

bool GuestMeister::Host_Construct(
	NPP npp, NPObject* obj, const NPVariant *args, uint32 argCount, NPVariant *result)
	{
	if (fNPNF.construct)
		return fNPNF.construct(npp, obj, args, argCount, result);
	return false;
	}

NPError GuestMeister::spNew(
	NPMIMEType pluginType, NPP npp, uint16 mode,
	int16 argc, char* argn[], char* argv[], NPSavedData* saved)
	{
	ZMACRO_Netscape_Before_GuestMeister(npp)
		return sGet()->New(pluginType, npp, mode, argc, argn, argv, saved);
	ZMACRO_Netscape_After_NPError
	}

NPError GuestMeister::spDestroy(NPP npp, NPSavedData** save)
	{
	ZMACRO_Netscape_Before_GuestMeister(npp)
		return sGet()->Destroy(npp, save);
	ZMACRO_Netscape_After_NPError
	}

NPError GuestMeister::spSetWindow(NPP npp, NPWindow* window)
	{
	ZMACRO_Netscape_Before_GuestMeister(npp)
		return sGet()->SetWindow(npp, window);
	ZMACRO_Netscape_After_NPError
	}

NPError GuestMeister::spNewStream(
	NPP npp, NPMIMEType type, NPStream* stream, NPBool seekable, uint16* stype)
	{
	ZMACRO_Netscape_Before_GuestMeister(npp)
		return sGet()->NewStream(npp, type, stream, seekable, stype);
	ZMACRO_Netscape_After_NPError
	}

NPError GuestMeister::spDestroyStream(NPP npp, NPStream* stream, NPReason reason)
	{
	ZMACRO_Netscape_Before_GuestMeister(npp)
		return sGet()->DestroyStream(npp, stream, reason);
	ZMACRO_Netscape_After_NPError
	}

int32 GuestMeister::spWriteReady(NPP npp, NPStream* stream)
	{
	ZMACRO_Netscape_Before_GuestMeister(npp)
		return sGet()->WriteReady(npp, stream);
	ZMACRO_Netscape_After_Return(0)
	}

int32 GuestMeister::spWrite(NPP npp, NPStream* stream, int32_t offset, int32_t len, void* buffer)
	{
	ZMACRO_Netscape_Before_GuestMeister(npp)
		return sGet()->Write(npp, stream, offset, len, buffer);
	ZMACRO_Netscape_After_Return(0)
	}

void GuestMeister::spStreamAsFile(NPP npp, NPStream* stream, const char* fname)
	{
	ZMACRO_Netscape_Before_GuestMeister(npp)
		return sGet()->StreamAsFile(npp, stream, fname);
	ZMACRO_Netscape_After_Void
	}

void GuestMeister::spPrint(NPP npp, NPPrint* platformPrint)
	{
	ZMACRO_Netscape_Before_GuestMeister(npp)
		return sGet()->Print(npp, platformPrint);
	ZMACRO_Netscape_After_Void
	}

int16 GuestMeister::spHandleEvent(NPP npp, void* event)
	{
	ZMACRO_Netscape_Before_GuestMeister(npp)
		return sGet()->HandleEvent(npp, event);
	ZMACRO_Netscape_After_Return(0)
	}

void GuestMeister::spURLNotify(NPP npp, const char* url, NPReason reason, void* notifyData)
	{
	ZMACRO_Netscape_Before_GuestMeister(npp)
		return sGet()->URLNotify(npp, url, reason, notifyData);
	ZMACRO_Netscape_After_Void
	}

jref GuestMeister::spGetJavaClass()
	{
	ZMACRO_Netscape_Before
		return sGet()->GetJavaClass();
	ZMACRO_Netscape_After_Return_Nil
	}

static bool spHostUsesOldWebKit(NPP npp)
	{
	// Logic and comments taken from NetscapeMoviePlugIn/main.c
	// This check is necessary if you want your exposed NPObject to not leak in WebKit-based
	// browsers (including Safari) released prior to Mac OS X 10.5 (Leopard).
	// Earlier versions of WebKit retained the NPObject returned from
	// NPP_GetValue(NPPVpluginScriptableNPObject). However, the NPRuntime API says NPObjects
	// should be retained by the plug-in before they are returned. WebKit versions later than
	// 420 do not retain returned NPObjects automatically; plug-ins are required to retain them
	// before returning from NPP_GetValue(), as in other browsers.

	static bool spChecked = false;
	static bool spUsesOldWebKit = false;
	if (!spChecked)
		{
		if (const char* userAgent = GuestMeister::sGet()->Host_UserAgent(npp))
			{
			static const char* const prefix = " AppleWebKit/";
			if (const char* versionString = strstr(userAgent, prefix))
				{
				versionString += strlen(prefix);
				int webKitVersion = atoi(versionString);
				if (webKitVersion && webKitVersion < 420)
					spUsesOldWebKit = true;
				}
			}
		spChecked = true;
		}

	return spUsesOldWebKit;
	}

NPError GuestMeister::spGetValue(NPP npp, NPPVariable variable, void *value)
	{
	ZMACRO_Netscape_Before_GuestMeister(npp)
		NPError result = sGet()->GetValue(npp, variable, value);
		if (NPERR_NO_ERROR == result
			&& NPPVpluginScriptableNPObject == variable
			&& spHostUsesOldWebKit(npp))
			{
			// We do not call releaseObject, because the likelihood is that the ref
			// count is currently one, and an active release would destroy the object
			// before our buggy caller gets a chance to increment the count.
			NPObject* theNPO = *static_cast<NPObject**>(value);
			--theNPO->referenceCount;
			}
		return result;
	ZMACRO_Netscape_After_NPError
	}

NPError GuestMeister::spSetValue(NPP npp, NPNVariable variable, void *value)
	{
	ZMACRO_Netscape_Before_GuestMeister(npp)
		return sGet()->SetValue(npp, variable, value);
	ZMACRO_Netscape_After_NPError
	}

// =================================================================================================
// MARK: - Guest

Guest::Guest(NPP npp)
:	fNPP(npp)
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
	const NPUTF8** names, int32 nameCount, NPIdentifier* identifiers)
	{ return GuestMeister::sGet()->Host_GetStringIdentifiers(names, nameCount, identifiers); }

NPIdentifier Guest::Host_GetIntIdentifier(int32 intid)
	{ return GuestMeister::sGet()->Host_GetIntIdentifier(intid); }

bool Guest::Host_IdentifierIsString(NPIdentifier identifier)
	{ return GuestMeister::sGet()->Host_IdentifierIsString(identifier); }

NPUTF8* Guest::Host_UTF8FromIdentifier(NPIdentifier identifier)
	{ return GuestMeister::sGet()->Host_UTF8FromIdentifier(identifier); }

int32 Guest::Host_IntFromIdentifier(NPIdentifier identifier)
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

void Guest::Host_PushPopupsEnabledState(NPBool enabled)
	{ return GuestMeister::sGet()->Host_PushPopupsEnabledState(fNPP, enabled); }

void Guest::Host_PopPopupsEnabledState()
	{ return GuestMeister::sGet()->Host_PopPopupsEnabledState(fNPP); }

bool Guest::Host_Enumerate(
	NPObject *npobj, NPIdentifier **identifier, uint32 *count)
	{ return GuestMeister::sGet()->Host_Enumerate(fNPP, npobj, identifier, count); }

void Guest::Host_PluginThreadAsyncCall(void (*func)(void *), void *userData)
	{ return GuestMeister::sGet()->Host_PluginThreadAsyncCall(fNPP, func, userData); }

bool Guest::Host_Construct(
	NPObject* obj, const NPVariant *args, uint32 argCount, NPVariant *result)
	{ return GuestMeister::sGet()->Host_Construct(fNPP, obj, args, argCount, result); }

ZRef<NPObjectG> Guest::Host_GetWindowObject()
	{
	ZRef<NPObjectG> result;
	this->Host_GetValue(NPNVWindowNPObject, &result);
	return result;
	}

ZRef<NPObjectG> Guest::Host_GetPluginObject()
	{
	ZRef<NPObjectG> result;
	this->Host_GetValue(NPNVPluginElementNPObject, &result);
	return result;
	}

} // namespace ZNetscape
} // namespace ZooLib
