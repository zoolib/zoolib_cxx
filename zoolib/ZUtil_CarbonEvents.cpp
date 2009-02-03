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

#include "zoolib/ZUtil_CarbonEvents.h"

#if ZCONFIG_SPI_Enabled(Carbon)

#include "zoolib/ZThreadImp.h"
#include "zoolib/ZLog.h"
#include "zoolib/ZTypes.h"

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * ZUtil_CarbonEvents

bool ZUtil_CarbonEvents::sGetParam(EventRef iEventRef, EventParamName iName, EventParamType iType,
	size_t iBufferSize, void* iBuffer)
	{
	return noErr == ::GetEventParameter(iEventRef, iName, iType, nil, iBufferSize, nil, iBuffer);
	}

size_t ZUtil_CarbonEvents::sGetParamLength(
	EventRef iEventRef, EventParamName iName, EventParamType iType)
	{
	UInt32 theLength;
	if (noErr == ::GetEventParameter(iEventRef, iName, iType, nil, 0, &theLength, nil))
		return theLength;
	return 0;
	}

void ZUtil_CarbonEvents::sSetParam(EventRef iEventRef, EventParamName iName, EventParamType iType,
	size_t iBufferSize, const void* iBuffer)
	{
	::SetEventParameter(iEventRef, iName, iType, iBufferSize, iBuffer);
	}

// =================================================================================================
#pragma mark -
#pragma mark * Handler

namespace {

using namespace ZUtil_CarbonEvents;

static const UInt32 kEventClassID_ZooLib = 'ZooL';
static const UInt32 kEventKindID_Call = 'Call';

class Handler
	{
public:
	Handler();
	~Handler();

	void InvokeOnMainThread(Callback_t iCallback, void* iRefcon);

	static EventHandlerUPP sEventHandlerUPP;
	static pascal OSStatus sEventHandler(EventHandlerCallRef iCallRef, EventRef iEventRef, void* iRefcon);
	OSStatus EventHandler(EventHandlerCallRef iCallRef, EventRef iEventRef);

	EventQueueRef fEventQueueRef;
	EventHandlerRef fEventHandlerRef;
	ZThreadImp::ID fMainID;
	};

struct Context_t
	{
	Callback_t fCallback;
	void* fRefcon;
	};

Handler::Handler()
	{
	fMainID = ZThreadImp::sID();

	fEventQueueRef = ::GetMainEventQueue();

	static const EventTypeSpec sEvents[] =
		{
		{ kEventClassID_ZooLib, kEventKindID_Call }
		};

	::InstallEventHandler(::GetEventDispatcherTarget(), sEventHandlerUPP,
		countof(sEvents), sEvents,
		this, &fEventHandlerRef);
	}

Handler::~Handler()
	{
	if (fEventHandlerRef)
		::RemoveEventHandler(fEventHandlerRef);
	fEventHandlerRef = nil;

	fEventQueueRef = nil;
	}

void Handler::InvokeOnMainThread(Callback_t iCallback, void* iRefcon)
	{
	if (fMainID == ZThreadImp::sID())
		{
		iCallback(iRefcon);
		}
	else
		{
		EventRef theEventRef;
		::MacCreateEvent(nil, kEventClassID_ZooLib, kEventKindID_Call,
			::GetCurrentEventTime(), kEventAttributeNone, &theEventRef);

		Context_t theContext;
		theContext.fCallback = iCallback;
		theContext.fRefcon = iRefcon;
		sSetParam_T(theEventRef, 'Cont', typeData, theContext);

		::PostEventToQueue(fEventQueueRef, theEventRef, kEventPriorityStandard);
		::ReleaseEvent(theEventRef);
		}
	}

EventHandlerUPP Handler::sEventHandlerUPP = NewEventHandlerUPP(sEventHandler);

pascal OSStatus Handler::sEventHandler(
	EventHandlerCallRef iCallRef, EventRef iEventRef, void* iRefcon)
	{
	try
		{
		return static_cast<Handler*>(iRefcon)->EventHandler(iCallRef, iEventRef);
		}
	catch (std::exception& ex)
		{
		if (ZLOG(s, eNotice, "Handler"))
			s << "sEventHandler, uncaught exception: " << ex.what();
		}
	catch (...)
		{
		if (ZLOG(s, eNotice, "ZThread::pRun"))
			s << "sEventHandler, uncaught exception, not derived fron std::exception";
		}
	return noErr;
	}

OSStatus Handler::EventHandler(EventHandlerCallRef iCallRef, EventRef iEventRef)
	{
	switch (::GetEventClass(iEventRef))
		{
		case kEventClassID_ZooLib:
			{
			switch (::GetEventKind(iEventRef))
				{
				case kEventKindID_Call:
					{
					Context_t theContext;
					if (sGetParam_T(iEventRef, 'Cont', typeData, theContext))
						theContext.fCallback(theContext.fRefcon);
					return noErr;
					}
				}
			break;
			}
		}
	return eventNotHandledErr;
	}

Handler sHandler;

} // anonymous namespace

void ZUtil_CarbonEvents::sInvokeOnMainThread(Callback_t iCallback, void* iRefcon)
	{
	sHandler.InvokeOnMainThread(iCallback, iRefcon);
	}

NAMESPACE_ZOOLIB_END

#endif // ZCONFIG_SPI_Enabled(Carbon)
