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

#include "zoolib/netscape/ZNetscape_Guest_Std.h"

#include "zoolib/ZDebug.h"
#include "zoolib/ZMemory.h"

namespace ZooLib {
namespace ZNetscape {

// =================================================================================================
// MARK: - GuestMeister

static Guest_Std* spGuest(NPP iNPP)
	{ return static_cast<Guest_Std*>(iNPP->pdata); }

GuestMeister_Std::GuestMeister_Std()
	{}

GuestMeister_Std::~GuestMeister_Std()
	{}

NPError GuestMeister_Std::New(
	NPMIMEType pluginType, NPP iNPP, uint16 mode,
	int16 argc, char* argn[], char* argv[], NPSavedData* saved)
	{
	try
		{
		Guest_Std* theG = this->MakeGuest(pluginType, iNPP, mode, argc, argn, argv, saved);
		iNPP->pdata = theG;
		}
	catch (...)
		{
		return NPERR_GENERIC_ERROR;
		}
	return NPERR_NO_ERROR;
	}

NPError GuestMeister_Std::Destroy(NPP iNPP, NPSavedData** save)
	{ return spGuest(iNPP)->Guest_Destroy(save); }

NPError GuestMeister_Std::SetWindow(NPP iNPP, NPWindow* window)
	{ return spGuest(iNPP)->Guest_SetWindow(window); }

NPError GuestMeister_Std::NewStream(NPP iNPP,
	NPMIMEType type, NPStream* stream, NPBool seekable, uint16* stype)
	{ return spGuest(iNPP)->Guest_NewStream(type, stream, seekable, stype); }

NPError GuestMeister_Std::DestroyStream(NPP iNPP, NPStream* stream, NPReason reason)
	{ return spGuest(iNPP)->Guest_DestroyStream(stream, reason); }

int32 GuestMeister_Std::WriteReady(NPP iNPP, NPStream* stream)
	{ return spGuest(iNPP)->Guest_WriteReady(stream); }

int32 GuestMeister_Std::Write(NPP iNPP,
	NPStream* stream, int32_t offset, int32_t len, void* buffer)
	{ return spGuest(iNPP)->Guest_Write(stream, offset, len, buffer); }

void GuestMeister_Std::StreamAsFile(NPP iNPP, NPStream* stream, const char* fname)
	{ return spGuest(iNPP)->Guest_StreamAsFile(stream, fname); }

void GuestMeister_Std::Print(NPP iNPP, NPPrint* platformPrint)
	{ return spGuest(iNPP)->Guest_Print(platformPrint); }

int16 GuestMeister_Std::HandleEvent(NPP iNPP, void* event)
	{ return spGuest(iNPP)->Guest_HandleEvent(event); }

void GuestMeister_Std::URLNotify(NPP iNPP, const char* url, NPReason reason, void* notifyData)
	{ return spGuest(iNPP)->Guest_URLNotify(url, reason, notifyData); }

jref GuestMeister_Std::GetJavaClass()
	{ return nullptr; }

NPError GuestMeister_Std::GetValue(NPP iNPP, NPPVariable variable, void *value)
	{ return spGuest(iNPP)->Guest_GetValue(variable, value); }

NPError GuestMeister_Std::SetValue(NPP iNPP, NPNVariable variable, void *value)
	{ return spGuest(iNPP)->Guest_SetValue(variable, value); }

// =================================================================================================
// MARK: - Guest_Std

Guest_Std::Guest_Std(NPP iNPP)
:	Guest(iNPP)
	{}

Guest_Std::~Guest_Std()
	{}

NPError Guest_Std::Guest_Destroy(NPSavedData** save)
	{
	delete this;
	return NPERR_NO_ERROR;
	}

NPError Guest_Std::Guest_SetWindow(NPWindow* window)
	{ return NPERR_GENERIC_ERROR; }

NPError Guest_Std::Guest_NewStream(
	NPMIMEType type, NPStream* stream, NPBool seekable, uint16* stype)
	{ return NPERR_GENERIC_ERROR; }

NPError Guest_Std::Guest_DestroyStream(NPStream* stream, NPReason reason)
	{ return NPERR_GENERIC_ERROR; }

int32 Guest_Std::Guest_WriteReady(NPStream* stream)
	{ return 0; }

int32 Guest_Std::Guest_Write(NPStream* stream, int32 offset, int32 len, void* buffer)
	{ return 0; }

void Guest_Std::Guest_StreamAsFile(NPStream* stream, const char* fname)
	{}

void Guest_Std::Guest_Print(NPPrint* platformPrint)
	{}

int16 Guest_Std::Guest_HandleEvent(void* event)
	{
	return 0; // ??
	}

void Guest_Std::Guest_URLNotify(const char* url, NPReason reason, void* notifyData)
	{}

NPError Guest_Std::Guest_GetValue(NPPVariable iNPPVariable, void* oValue)
	{
	switch (iNPPVariable)
		{
		case NPPVpluginScriptableNPObject:
			{
			if (ZRef<NPObjectG> theObject = this->Guest_GetNPObject())
				{
				*static_cast<ZRef<NPObjectG>*>(oValue) = theObject;
				return NPERR_NO_ERROR;
				}
			break;
			}
		default:
			break;
		}

	return NPERR_GENERIC_ERROR;
	}

NPError Guest_Std::Guest_SetValue(NPNVariable iNPNVariable, void* iValue)
	{ return NPERR_GENERIC_ERROR; }

ZRef<NPObjectG> Guest_Std::Guest_GetNPObject()
	{ return null; }

} // namespace ZNetscape
} // namespace ZooLib
