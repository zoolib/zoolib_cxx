/* -------------------------------------------------------------------------------------------------
Copyright (c) 2009 Andrew Green
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

#include "zoolib/ZNetscape_Host_Mac.h"

#if defined(XP_MAC) || defined(XP_MACOSX)

#include "zoolib/ZLog.h"
#include "zoolib/ZMemory.h"
#include "zoolib/ZStream_String.h"
#include "zoolib/ZUtil_CarbonEvents.h"

#if UNIVERSAL_INTERFACES_VERSION <= 0x0341
enum
	{
	kEventMouseEntered = 8,
	kEventMouseExited = 9
	};
#endif

using std::string;

NAMESPACE_ZOOLIB_BEGIN

using ZUtil_CarbonEvents::sGetParam_T;
using ZUtil_CarbonEvents::sSetParam_T;

namespace ZNetscape {

// =================================================================================================
#pragma mark -
#pragma mark * Host_Mac

Host_Mac::Host_Mac(ZRef<GuestFactory> iGuestFactory)
:	Host_Std(iGuestFactory)
	{
	#if defined(XP_MACOSX)
		fUseCoreGraphics = false;
		fNP_CGContext.context = nil;
		fNP_CGContext.window = nil;
	#endif

	ZBlockZero(&fNP_Port, sizeof(fNP_Port));

	fNPWindow.type = NPWindowTypeDrawable;
	}

Host_Mac::~Host_Mac()
	{
	}

NPError Host_Mac::Host_GetValue(NPP npp, NPNVariable variable, void* ret_value)
	{
	switch (variable)
		{
		case NPNVSupportsWindowless:
			{
			*static_cast<NPBool*>(ret_value) = TRUE;
            return NPERR_NO_ERROR;
			}
		#if defined(XP_MACOSX)
		case NPNVsupportsCoreGraphicsBool:
			{
				*static_cast<NPBool*>(ret_value) = TRUE;
				return NPERR_NO_ERROR;
			break;
			}
		#endif // defined(XP_MACOSX)
		}

	return Host_Std::Host_GetValue(npp, variable, ret_value);
	}

NPError Host_Mac::Host_SetValue(NPP npp, NPPVariable variable, void* value)
	{
	switch (variable)
		{
		#if defined(XP_MACOSX)
		case NPPVpluginDrawingModel:
			{
			if (ZLOG(s, eDebug, "Host_Mac"))
				s << "Host_SetValue, NPPVpluginDrawingModel";
			if (reinterpret_cast<int>(value) == NPDrawingModelCoreGraphics)
				fUseCoreGraphics = true;
			else
				fUseCoreGraphics = false;
				return NPERR_NO_ERROR;
			break;
			}
		#endif // defined(XP_MACOSX)
		}

	return Host_Std::Host_SetValue(npp, variable, value);
	}

static void sStuffEventRecord(EventRecord& oEventRecord)
	{
	oEventRecord.what = nullEvent;
	oEventRecord.message = 0;
	oEventRecord.when = ::TickCount();
	::GetGlobalMouse(&oEventRecord.where);
	oEventRecord.modifiers = ::GetCurrentKeyModifiers();
	}

void Host_Mac::DoActivate(bool iActivate)
	{
	EventRecord theER;
	sStuffEventRecord(theER);

	theER.what = activateEvt;
	if (iActivate)
		theER.modifiers = activeFlag;
	else
		theER.modifiers = 0;
	
	this->DoEvent(theER);
	}

void Host_Mac::DoFocus(bool iFocused)
	{
	EventRecord theER;
	sStuffEventRecord(theER);

	if (iFocused)
		theER.what = NPEventType_GetFocusEvent;
	else
		theER.what = NPEventType_LoseFocusEvent;

	this->DoEvent(theER);
	}

void Host_Mac::DoIdle()
	{
	EventRecord theER;
	sStuffEventRecord(theER);

	this->DoEvent(theER);
	}

void Host_Mac::DoDraw(WindowRef iWindowRef)
	{
	this->UpdateWindowRef(iWindowRef);

	EventRecord theER;
	sStuffEventRecord(theER);

	theER.what = updateEvt;
	theER.message = (UInt32)iWindowRef;

	this->DoEvent(theER);
	}

void Host_Mac::DoEvent(const EventRecord& iEvent)
	{
	// Hmm -- do we need to use the local?
	EventRecord localEvent = iEvent;
	this->Guest_HandleEvent(&localEvent);
	}

void Host_Mac::UpdateWindowRef(WindowRef iWindowRef)
	{
	ZPoint theLocation(0, 0);
	ZPoint theSize;
	
	Rect globalBounds;
	::GetWindowBounds(iWindowRef, kWindowGlobalPortRgn, &globalBounds);
	theSize.h = globalBounds.right - globalBounds.left;
	theSize.v = globalBounds.bottom - globalBounds.top;

	if (false) {}
	#if defined(XP_MACOSX)
	else if (fUseCoreGraphics)
		{
		fNPWindow.window = &fNP_CGContext;
		}
	#endif // defined(XP_MACOSX)
	else
		{
		fNPWindow.window = &fNP_Port;

		fNP_Port.port = ::GetWindowPort(iWindowRef);
		fNP_Port.portx = -theLocation.h;
		fNP_Port.porty = -theLocation.v;
		}

	fNPWindow.x = theLocation.h;
	fNPWindow.y = theLocation.v;
	fNPWindow.width = theSize.h;
	fNPWindow.height = theSize.v;

	fNPWindow.clipRect.left = theLocation.h;
	fNPWindow.clipRect.top = theLocation.v;
	fNPWindow.clipRect.right = theLocation.h + theSize.h;
	fNPWindow.clipRect.bottom = theLocation.v + theSize.v;

	this->Guest_SetWindow(&fNPWindow);
	}

// =================================================================================================
#pragma mark -
#pragma mark * Host_Mac_EventLoop

Host_Mac_EventLoop::Host_Mac_EventLoop(ZRef<ZNetscape::GuestFactory> iGF)
:	Host_Mac(iGF),
	fEventLoopTimerRef_Idle(nil)
	{
	::InstallEventLoopTimer(::GetMainEventLoop(), 0.02, 0.02,
		sEventLoopTimerUPP_Idle, this, &fEventLoopTimerRef_Idle);
	}

Host_Mac_EventLoop::~Host_Mac_EventLoop()
	{
	if (fEventLoopTimerRef_Idle)
		::RemoveEventLoopTimer(fEventLoopTimerRef_Idle);
	fEventLoopTimerRef_Idle = nil;
	}

bool Host_Mac_EventLoop::pDeliverEvent(EventRef iEventRef)
	{
	EventRecord theEventRecord;
	if (::ConvertEventRefToEventRecord(iEventRef, &theEventRecord))
		{
		this->DoEvent(theEventRecord);
		return true;
		}
	return false;
	}

EventLoopTimerUPP Host_Mac_EventLoop::sEventLoopTimerUPP_Idle
	= NewEventLoopTimerUPP(sEventLoopTimer_Idle);

pascal void Host_Mac_EventLoop::sEventLoopTimer_Idle(EventLoopTimerRef iTimer, void* iRefcon)
	{ static_cast<Host_Mac_EventLoop*>(iRefcon)->EventLoopTimer_Idle(iTimer); }

void Host_Mac_EventLoop::EventLoopTimer_Idle(EventLoopTimerRef iTimer)
	{
	Host_Mac::DeliverData();
	Host_Mac::DoIdle();
	}

// =================================================================================================
#pragma mark -
#pragma mark * Host_WindowRef

Host_WindowRef::Host_WindowRef(ZRef<ZNetscape::GuestFactory> iGF, WindowRef iWindowRef)
:	Host_Mac_EventLoop(iGF),
	fWindowRef(iWindowRef),
	fEventTargetRef_Window(nil),
	fEventHandlerRef_Window(nil)
	{
	fEventTargetRef_Window = ::GetWindowEventTarget(fWindowRef);

	#if defined(XP_MACOSX)
		fNP_CGContext.window = fWindowRef;
		#if 1
		CGContextRef theCGContextRef;
		CGrafPtr thePort = ::GetWindowPort(fWindowRef);
		::CreateCGContextForPort(thePort, &theCGContextRef);
		fNP_CGContext.context = theCGContextRef;
		#endif
	#endif

	static const EventTypeSpec sEvents_Window[] =
		{
		{ kEventClassCommand, kEventCommandProcess },

		{ kEventClassWindow, kEventWindowActivated },
		{ kEventClassWindow, kEventWindowDeactivated },
		{ kEventClassWindow, kEventWindowBoundsChanged },
		{ kEventClassWindow, kEventWindowHandleContentClick },
		{ kEventClassWindow, kEventWindowDrawContent },
//		{ kEventClassWindow, kEventWindowUpdate },

		{ kEventClassKeyboard, kEventRawKeyDown },
		{ kEventClassKeyboard, kEventRawKeyRepeat },
		{ kEventClassKeyboard, kEventRawKeyUp },
		{ kEventClassKeyboard, kEventRawKeyModifiersChanged },

		{ kEventClassMouse, kEventMouseDown },
		{ kEventClassMouse, kEventMouseUp },
		{ kEventClassMouse, kEventMouseMoved },
//		{ kEventClassMouse, kEventMouseDragged },
		{ kEventClassMouse, 11 }, // MightyMouse wheels
		{ kEventClassMouse, kEventMouseWheelMoved },
		{ kEventClassMouse, kEventMouseEntered },
		{ kEventClassMouse, kEventMouseExited },
		};

	::InstallEventHandler(fEventTargetRef_Window, sEventHandlerUPP_Window,
		countof(sEvents_Window), sEvents_Window,
		this, &fEventHandlerRef_Window);
	}

Host_WindowRef::~Host_WindowRef()
	{
	#if defined(XP_MACOSX)
//		::CFRelease(fNP_CGContext.context);
	#endif

	if (fEventHandlerRef_Window)
		::RemoveEventHandler(fEventHandlerRef_Window);
	fEventHandlerRef_Window = nil;

	fEventTargetRef_Window = nil;
	}

void Host_WindowRef::Host_InvalidateRect(NPP npp, NPRect* rect)
	{
	Rect theBounds;
	theBounds.left = rect->left;
	theBounds.top = rect->top;
	theBounds.right = rect->right;
	theBounds.bottom = rect->bottom;
	::InvalWindowRect(fWindowRef, &theBounds);
	Host_Mac::DoDraw(fWindowRef);
	}

void Host_WindowRef::PostCreateAndLoad()
	{
	this->UpdateWindowRef(fWindowRef);
	}

EventHandlerUPP Host_WindowRef::sEventHandlerUPP_Window
	= NewEventHandlerUPP(sEventHandler_Window);

pascal OSStatus Host_WindowRef::sEventHandler_Window(
	EventHandlerCallRef iCallRef, EventRef iEventRef, void* iRefcon)
	{ return static_cast<Host_WindowRef*>(iRefcon)->EventHandler_Window(iCallRef, iEventRef); }

OSStatus Host_WindowRef::EventHandler_Window(EventHandlerCallRef iCallRef, EventRef iEventRef)
	{
	OSStatus err = eventNotHandledErr;

	switch (::GetEventClass(iEventRef))
		{
        case kEventClassMouse:
			{
			WindowPartCode theWPC = sGetParam_T<WindowPartCode>(iEventRef, kEventParamWindowPartCode, typeWindowPartCode);
			if (true && ::GetEventKind(iEventRef) == kEventMouseDown && theWPC == inContent)
				{
				this->pDeliverEvent(iEventRef);
				return noErr;
				}
			else if (::GetEventKind(iEventRef) == kEventMouseUp)
				{
				this->pDeliverEvent(iEventRef);
				}
			else
				{
				this->pDeliverEvent(iEventRef);
				}
			break;
			}
		case kEventClassKeyboard:
			{
			if (this->pDeliverEvent(iEventRef))
				return noErr;
			break;
			}
		case kEventClassWindow:
			{
			switch (::GetEventKind(iEventRef))
				{
				case kEventWindowHandleContentClick:
					{
					// Claim we handled the event, to allow
					// raw mouseUp/mouseDowns to be delivered.
					return noErr;
					}
				case kEventWindowUpdate:
					{
					return eventNotHandledErr;
					break;
					}
				case kEventWindowDrawContent:
					{
					if (true)//!fUseCoreGraphics)
						{
						Host_Mac::DoDraw(fWindowRef);
						}
					else
						{
						CGrafPtr qdPort;
						GDHandle currentGDevice;
						::GetGWorld(&qdPort, &currentGDevice);

						CGContextRef cg;
						::QDBeginCGContext(qdPort, &cg);
						::CGContextSaveGState(cg);
						fNP_CGContext.context = cg;
						Host_Mac::DoDraw(fWindowRef);
						::CGContextRestoreGState(cg);
						::CGContextSynchronize(cg);

						::QDEndCGContext(qdPort, &cg);
						}

//					return noErr;
					break;
					}
				case kEventWindowActivated:
					{
					Host_Mac::DoActivate(true);
					break;
					}
				case kEventWindowDeactivated:
					{
					Host_Mac::DoActivate(false);
					break;
					}
				case kEventWindowFocusAcquired:
					{
					Host_Mac::DoFocus(true);
					break;
					}
				case kEventWindowFocusRelinquish:
					{
					Host_Mac::DoFocus(false);
					break;
					}
				case kEventWindowBoundsChanged:
					{
					this->UpdateWindowRef(fWindowRef);
					break;
					}
				}
			break;
			}
		}

	return err;
	}

} // namespace ZNetscape

NAMESPACE_ZOOLIB_END

#endif // defined(XP_MAC) || defined(XP_MACOSX)
