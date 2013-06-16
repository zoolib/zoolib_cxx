/* -------------------------------------------------------------------------------------------------
Copyright (c) 2008 Andrew Green
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

#ifndef __ZNetscape_h__
#define __ZNetscape_h__ 1
#include "zconfig.h"

#include "zoolib/ZCONFIG_SPI.h"

#include "zoolib/netscape/ZCompat_npapi.h"

#include <string>
#include <vector>

namespace ZooLib {
namespace ZNetscape {

// =================================================================================================
// MARK: - NPPoint_Z

struct NPPoint_Z
	{
	uint16 x;
	uint16 y;
	};

// =================================================================================================
// MARK: - NPStream_Z

class NPStream_Z : public NPStream
	{
public:
	#if NP_VERSION_MINOR < NPVERS_HAS_RESPONSE_HEADERS
		const char* headers;
	#endif
	};

// =================================================================================================
// MARK: - NPClass_Z

class NPClass_Z : public NPClass
	{
	NPClass_Z();
public:
	NPClass_Z(
		NPAllocateFunctionPtr allocate,
		NPDeallocateFunctionPtr deallocate,
		NPInvalidateFunctionPtr invalidate,
		NPHasMethodFunctionPtr hasMethod,
		NPInvokeFunctionPtr invoke,
		NPInvokeDefaultFunctionPtr invokeDefault,
		NPHasPropertyFunctionPtr hasProperty,
		NPGetPropertyFunctionPtr getProperty,
		NPSetPropertyFunctionPtr setProperty,
		NPRemovePropertyFunctionPtr removeProperty,
		NPEnumerationFunctionPtr enumerate);

	#if NP_CLASS_STRUCT_VERSION < 2
		NPEnumerationFunctionPtr enumerate;
	#endif
	};

NPClass_Z* sGetClass(NPObject* obj);

// =================================================================================================
// MARK: - NPNetscapeFuncs_Z

class NPNetscapeFuncs_Z : public NPNetscapeFuncs
	{
public:
	#if NP_VERSION_MINOR < NPVERS_HAS_POPUPS_ENABLED_STATE
		NPN_PushPopupsEnabledStateProcPtr pushpopupsenabledstate;
		NPN_PopPopupsEnabledStateProcPtr poppopupsenabledstate;
	#endif

	#if NP_VERSION_MINOR < NPVERS_HAS_NPOBJECT_ENUM
		NPN_EnumerateProcPtr enumerate;
	#endif

	#if NP_VERSION_MINOR < NPVERS_HAS_PLUGIN_THREAD_ASYNC_CALL
		NPN_PluginThreadAsyncCallProcPtr pluginthreadasynccall;
		NPN_ConstructProcPtr construct;
	#endif

	#if NP_VERSION_MINOR < NPVERS_MACOSX_HAS_EVENT_MODELS
		void* dummy_getvalueforurl;
		void* dummy_setvalueforurl;
		void* dummy_getauthenticationinfo;
		NPN_ScheduleTimerProcPtr scheduletimer;
		NPN_UnscheduleTimerProcPtr unscheduletimer;
		NPN_PopUpContextMenuProcPtr popupcontextmenu;
		void* dummy_convertpoint;
	#endif
	};

// =================================================================================================
// MARK: - sAsString

std::string sAsString(const NPString& iNPString);
std::string sAsString(NPNVariable iVar);
std::string sAsString(NPPVariable iVar);

} // namespace ZNetscape
} // namespace ZooLib

#endif // __ZNetscape_h__
