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

#include "zoolib/ZNetscape_Guest_Std.h"

#include "zoolib/ZDebug.h"
#include "zoolib/ZMemory.h"

namespace ZNetscape {

// =================================================================================================
#pragma mark -
#pragma mark * GuestMeister

static Guest_Std* sGuest(NPP instance)
	{ return static_cast<Guest_Std*>(instance->pdata); }

GuestMeister_Std::GuestMeister_Std()
	{}

GuestMeister_Std::~GuestMeister_Std()
	{}

NPError GuestMeister_Std::New(
	NPMIMEType pluginType, NPP instance, uint16 mode,
	int16 argc, char* argn[], char* argv[], NPSavedData* saved)
	{
	try
		{
		Guest_Std* theG = this->MakeGuest(pluginType, instance, mode, argc, argn, argv, saved);
		instance->pdata = theG;
		}
	catch (...)
		{
		return NPERR_GENERIC_ERROR;
		}
	return NPERR_NO_ERROR;
	}

NPError GuestMeister_Std::Destroy(NPP instance, NPSavedData** save)
	{ return sGuest(instance)->Guest_Destroy(save); }

NPError GuestMeister_Std::SetWindow(NPP instance, NPWindow* window)
	{ return sGuest(instance)->Guest_SetWindow(window); }

NPError GuestMeister_Std::NewStream(NPP instance,
	NPMIMEType type, NPStream* stream, NPBool seekable, uint16* stype)
	{ return sGuest(instance)->Guest_NewStream(type, stream, seekable, stype); }

NPError GuestMeister_Std::DestroyStream(NPP instance, NPStream* stream, NPReason reason)
	{ return sGuest(instance)->Guest_DestroyStream(stream, reason); }

int32 GuestMeister_Std::WriteReady(NPP instance, NPStream* stream)
	{ return sGuest(instance)->Guest_WriteReady(stream); }

int32 GuestMeister_Std::Write(NPP instance,
	NPStream* stream, int32 offset, int32 len, void* buffer)
	{ return sGuest(instance)->Guest_Write(stream, offset, len, buffer); }

void GuestMeister_Std::StreamAsFile(NPP instance, NPStream* stream, const char* fname)
	{ return sGuest(instance)->Guest_StreamAsFile(stream, fname); }

void GuestMeister_Std::Print(NPP instance, NPPrint* platformPrint)
	{ return sGuest(instance)->Guest_Print(platformPrint); }

int16 GuestMeister_Std::HandleEvent(NPP instance, void* event)
	{ return sGuest(instance)->Guest_HandleEvent(event); }

void GuestMeister_Std::URLNotify(NPP instance, const char* url, NPReason reason, void* notifyData)
	{ return sGuest(instance)->Guest_URLNotify(url, reason, notifyData); }

jref GuestMeister_Std::GetJavaClass()
	{ return nil; }

NPError GuestMeister_Std::GetValue(NPP instance, NPPVariable variable, void *value)
	{ return sGuest(instance)->Guest_GetValue(variable, value); }

NPError GuestMeister_Std::SetValue(NPP instance, NPNVariable variable, void *value)
	{ return sGuest(instance)->Guest_SetValue(variable, value); }

// =================================================================================================
#pragma mark -
#pragma mark * Guest_Std

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
	{
	}

NPError Guest_Std::Guest_GetValue(NPPVariable iNPPVariable, void* oValue)
	{ return NPERR_GENERIC_ERROR; }

NPError Guest_Std::Guest_SetValue(NPNVariable iNPNVariable, void* iValue)
	{ return NPERR_GENERIC_ERROR; }

} // namespace ZNetscape
