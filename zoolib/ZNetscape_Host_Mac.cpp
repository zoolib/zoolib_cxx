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

#include "zoolib/ZLog.h"
#include "zoolib/ZMemory.h"
#include "zoolib/ZStream_String.h"
#include "zoolib/ZUtil_CarbonEvents.h"
#include "zoolib/ZUtil_Strim_Geom.h"

#if defined(XP_MAC) || defined(XP_MACOSX)

#if UNIVERSAL_INTERFACES_VERSION <= 0x0341
enum
	{
	kEventMouseDragged = 6,
	kEventMouseEntered = 8,
	kEventMouseExited = 9
	};
#endif

using std::string;

using ZooLib::ZUtil_CarbonEvents::sGetParam_T;
using ZooLib::ZUtil_CarbonEvents::sSetParam_T;

#endif // defined(XP_MAC) || defined(XP_MACOSX)

NAMESPACE_ZOOLIB_BEGIN

namespace ZNetscape {

// =================================================================================================
#pragma mark -
#pragma mark * Host_Mac

#if defined(XP_MAC) || defined(XP_MACOSX)

Host_Mac::Host_Mac(ZRef<GuestFactory> iGuestFactory)
:	Host_Std(iGuestFactory),
	fEventLoopTimerRef_Idle(nil)
	{
	#if defined(XP_MACOSX)
		fUseCoreGraphics = false;
		fNP_CGContext.context = nil;
		fNP_CGContext.window = nil;
	#endif

	ZBlockZero(&fNP_Port, sizeof(fNP_Port));

	fNPWindow.type = NPWindowTypeDrawable;

	::InstallEventLoopTimer(::GetMainEventLoop(), 0.02, 0.02,
		sEventLoopTimerUPP_Idle, this, &fEventLoopTimerRef_Idle);
	}

Host_Mac::~Host_Mac()
	{
	if (fEventLoopTimerRef_Idle)
		::RemoveEventLoopTimer(fEventLoopTimerRef_Idle);
	fEventLoopTimerRef_Idle = nil;
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
	this->DoEvent(iFocused ? NPEventType_GetFocusEvent : NPEventType_LoseFocusEvent, 0);
	}

void Host_Mac::DoIdle()
	{
	this->DoEvent(nullEvent, 0);
	}

void Host_Mac::DoDraw(WindowRef iWindowRef)
	{
#if 0
	this->UpdateWindowRef(iWindowRef);

	EventRecord theER;
	sStuffEventRecord(theER);

	theER.what = updateEvt;
	theER.message = (UInt32)iWindowRef;

	this->DoEvent(theER);
#endif
	}

void Host_Mac::DoEvent(EventKind iWhat, uint32 iMessage)
	{
	EventRecord theER;
	sStuffEventRecord(theER);

	theER.what = iWhat;
	theER.message = iMessage;

	this->DoEvent(theER);
	}

void Host_Mac::DoEvent(const EventRecord& iEvent)
	{
	// Hmm -- do we need to use the local?
	EventRecord localEvent = iEvent;
	this->Guest_HandleEvent(&localEvent);
	}

void Host_Mac::UpdateWindowRef(WindowRef iWindowRef, CGContextRef iContextRef)
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
		fNP_CGContext.window = iWindowRef;
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

bool Host_Mac::pDeliverEvent(EventRef iEventRef)
	{
	EventRecord theEventRecord;
	if (::ConvertEventRefToEventRecord(iEventRef, &theEventRecord))
		{
		this->DoEvent(theEventRecord);
		return true;
		}
	return false;
	}

EventLoopTimerUPP Host_Mac::sEventLoopTimerUPP_Idle
	= NewEventLoopTimerUPP(sEventLoopTimer_Idle);

pascal void Host_Mac::sEventLoopTimer_Idle(EventLoopTimerRef iTimer, void* iRefcon)
	{ static_cast<Host_Mac*>(iRefcon)->EventLoopTimer_Idle(iTimer); }

void Host_Mac::EventLoopTimer_Idle(EventLoopTimerRef iTimer)
	{
	Host_Mac::DeliverData();
	Host_Mac::DoIdle();
	}

#endif // defined(XP_MAC) || defined(XP_MACOSX)

// =================================================================================================
#pragma mark -
#pragma mark * Host_WindowRef

#if defined(XP_MAC) || defined(XP_MACOSX)

Host_WindowRef::Host_WindowRef(ZRef<ZNetscape::GuestFactory> iGF, WindowRef iWindowRef)
:	Host_Mac(iGF),
	fWindowRef(iWindowRef),
	fEventTargetRef_Window(nil),
	fEventHandlerRef_Window(nil)
	{
	fEventTargetRef_Window = ::GetWindowEventTarget(fWindowRef);

	#if defined(XP_MACOSX)
		fNP_CGContext.window = fWindowRef;
		#if 0
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
		{ kEventClassMouse, kEventMouseDragged },
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
//##	this->UpdateWindowRef(fWindowRef);
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
					if (fUseCoreGraphics)
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
					else
						{
						Host_Mac::DoDraw(fWindowRef);
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
//##					this->UpdateWindowRef(fWindowRef);
					break;
					}
				}
			break;
			}
		}

	return err;
	}

#endif // defined(XP_MAC) || defined(XP_MACOSX)

// =================================================================================================
#pragma mark -
#pragma mark * Host_HIViewRef

#if defined(XP_MACOSX)

Host_HIViewRef::Host_HIViewRef(ZRef<ZNetscape::GuestFactory> iGF, HIViewRef iHIViewRef)
:	Host_Mac(iGF),
	fHIViewRef(iHIViewRef),
	fEventTargetRef_View(nil),
	fEventHandlerRef_View(nil)
	{
	fEventTargetRef_View = ::GetControlEventTarget(fHIViewRef);

	static const EventTypeSpec sEvents_View[] =
		{
		{ kEventClassKeyboard, kEventRawKeyDown },
		{ kEventClassKeyboard, kEventRawKeyRepeat },
		{ kEventClassKeyboard, kEventRawKeyUp },
		{ kEventClassKeyboard, kEventRawKeyModifiersChanged },

		{ kEventClassControl, kEventControlHitTest },
		{ kEventClassControl, kEventControlSimulateHit },
		{ kEventClassControl, kEventControlHitTest },
		{ kEventClassControl, kEventControlDraw },
		{ kEventClassControl, kEventControlApplyBackground },
		{ kEventClassControl, kEventControlApplyTextColor },
		{ kEventClassControl, kEventControlSetFocusPart },
		{ kEventClassControl, kEventControlGetFocusPart },
		{ kEventClassControl, kEventControlActivate },
		{ kEventClassControl, kEventControlDeactivate },
		{ kEventClassControl, kEventControlSetCursor },
		{ kEventClassControl, kEventControlContextualMenuClick },
		{ kEventClassControl, kEventControlClick },
		{ kEventClassControl, kEventControlGetNextFocusCandidate },
		{ kEventClassControl, kEventControlGetAutoToggleValue },
		{ kEventClassControl, kEventControlInterceptSubviewClick },
		{ kEventClassControl, kEventControlGetClickActivation },
		{ kEventClassControl, kEventControlDragEnter },
		{ kEventClassControl, kEventControlDragWithin },
		{ kEventClassControl, kEventControlDragLeave },
		{ kEventClassControl, kEventControlDragReceive },
		{ kEventClassControl, kEventControlInvalidateForSizeChange },
		{ kEventClassControl, kEventControlTrackingAreaEntered },
		{ kEventClassControl, kEventControlTrackingAreaExited },


		{ kEventClassControl, kEventControlTrack },

		{ kEventClassControl, kEventControlBoundsChanged },
		

		{ kEventClassMouse, kEventMouseDown },
		{ kEventClassMouse, kEventMouseUp },
		{ kEventClassMouse, kEventMouseMoved },
		{ kEventClassMouse, kEventMouseDragged },
		{ kEventClassMouse, 11 }, // MightyMouse wheels
		{ kEventClassMouse, kEventMouseWheelMoved },
		{ kEventClassMouse, kEventMouseEntered },
		{ kEventClassMouse, kEventMouseExited },
		};

	::InstallEventHandler(fEventTargetRef_View, sEventHandlerUPP_View,
		countof(sEvents_View), sEvents_View,
		this, &fEventHandlerRef_View);
	}

Host_HIViewRef::~Host_HIViewRef()
	{
	if (fEventHandlerRef_View)
		::RemoveEventHandler(fEventHandlerRef_View);
	fEventHandlerRef_View = nil;

	fEventTargetRef_View = nil;
	}

void Host_HIViewRef::Host_InvalidateRect(NPP npp, NPRect* rect)
	{
	HIRect theHIRect;
	theHIRect.origin.x = rect->left;
	theHIRect.origin.y = rect->top;
	theHIRect.size.width = rect->right - rect->left;
	theHIRect.size.height = rect->bottom - rect->top;
	::HIViewSetNeedsDisplayInRect(fHIViewRef, &theHIRect, true);
	}

void Host_HIViewRef::PostCreateAndLoad()
	{
	}

EventHandlerUPP Host_HIViewRef::sEventHandlerUPP_View = NewEventHandlerUPP(sEventHandler_View);

pascal OSStatus Host_HIViewRef::sEventHandler_View(
	EventHandlerCallRef iCallRef, EventRef iEventRef, void* iRefcon)
	{ return static_cast<Host_HIViewRef*>(iRefcon)->EventHandler_View(iCallRef, iEventRef); }

void Host_HIViewRef::DoSetWindow(int iX, int iY, int iWidth, int iHeight)
	{
	// This is spurious if we're doing CoreGraphics
	fNP_Port.portx = -iX;
	fNP_Port.porty = -iY;

	fNPWindow.x = iX;
	fNPWindow.y = iY;
	fNPWindow.width = iWidth;
	fNPWindow.height = iHeight;

	fNPWindow.clipRect.left = iX;
	fNPWindow.clipRect.top = iY;
	fNPWindow.clipRect.right = iX + iWidth;
	fNPWindow.clipRect.bottom = iY + iHeight;

	this->Guest_SetWindow(&fNPWindow);
	}

OSStatus Host_HIViewRef::EventHandler_View(EventHandlerCallRef iCallRef, EventRef iEventRef)
	{
	if (ZLOG(s, eDebug, "Host_HIViewRef"))
		{
		s << ZUtil_CarbonEvents::sEventAsString(
			::GetEventClass(iEventRef), ::GetEventKind(iEventRef));
		}

	OSStatus err = eventNotHandledErr;

	switch (::GetEventClass(iEventRef))
		{
        case kEventClassMouse:
		case kEventClassKeyboard:
			{
			if (this->pDeliverEvent(iEventRef))
				return noErr;
			break;
			}

		case kEventClassControl:
			{
			switch (::GetEventKind(iEventRef))
				{
				case kEventControlActivate:
					{
					this->DoActivate(true);
					break;
					}
				case kEventControlDeactivate:
					{
					this->DoActivate(false);
					break;
					}
				case kEventControlSetFocusPart:
					{
					return noErr;
					}
				case kEventControlClick:
					{
					::SetKeyboardFocus(::HIViewGetWindow(fHIViewRef), fHIViewRef, kControlIndicatorPart);
//					if (this->pDeliverEvent(iEventRef))
//						return noErr;
					break;
					}
				case kEventControlTrack:
					{
//					if (this->pDeliverEvent(iEventRef))
//						return noErr;
					break;
					}
				case kEventControlHitTest:
					{
					ControlPartCode theCode = kControlIndicatorPart;
					sSetParam_T(iEventRef, kEventParamControlPart, typeControlPartCode, theCode);
					return noErr;
					}
				case kEventControlDraw:
					{
					::CallNextEventHandler(iCallRef, iEventRef);
					HIRect theBounds;
					::HIViewGetBounds(fHIViewRef, &theBounds);

					if (ZLOG(s, eDebug, "Host_HIViewRef"))
						s << ZGRectf(theBounds);

					WindowRef theWindowRef = ::HIViewGetWindow(fHIViewRef);

					if (fUseCoreGraphics)
						{
						if (CGContextRef theCG = sGetParam_T<CGContextRef>(iEventRef,
							kEventParamCGContextRef, typeCGContextRef))
							{
							fNP_CGContext.context = theCG;
							fNP_CGContext.window = theWindowRef;

							fNPWindow.window = &fNP_CGContext;

							::CGContextSaveGState(theCG);

							this->DoSetWindow(theBounds.origin.x, theBounds.origin.y,
								theBounds.size.width, theBounds.size.height);

							this->DoEvent(updateEvt, (UInt32)theWindowRef);

							::CGContextRestoreGState(theCG);
							fNP_CGContext.context = nil;
							}
						}
					else if (0)
						{
						if (CGrafPtr theCGrafPtr = sGetParam_T<CGrafPtr>(iEventRef,
							kEventParamGrafPort, typeGrafPtr))
							{
//							fNP_Port.port = ::GetWindowPort(theWindowRef);
							fNP_Port.port = theCGrafPtr;
							fNPWindow.window = &fNP_Port;
							this->DoSetWindow(0, 0, theBounds.size.width, theBounds.size.height);
							this->DoEvent(updateEvt, (UInt32)theWindowRef);
//							fNP_Port.port = nil;
							}
						}
					err = noErr;
					break;
					}
				case kEventControlBoundsChanged:
					{
					ZGRectf newBounds = sGetParam_T<Rect>(iEventRef,
						kEventParamCurrentBounds, typeQDRectangle);
//					newBounds.origin = 0;
//					newBounds -= newBounds.origin;

					if (ZLOG(s, eDebug, "Host_HIViewRef"))
						s << newBounds;

					WindowRef theWindowRef = ::HIViewGetWindow(fHIViewRef);
					if (fUseCoreGraphics)
						{
						fNP_CGContext.window = theWindowRef;
						}
					else if (0)
						{
						fNP_Port.port = ::GetWindowPort(theWindowRef);
						}

					this->DoSetWindow(newBounds.origin.x,
						newBounds.origin.y,
						newBounds.extent.h,
						newBounds.extent.v);
					break;
					}
				}
			break;
			}
		}

	return err;
	}

#endif // defined(XP_MACOSX)

} // namespace ZNetscape

NAMESPACE_ZOOLIB_END
