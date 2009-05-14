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
#include "zoolib/ZRGBColor.h"
#include "zoolib/ZStream_String.h"
#include "zoolib/ZUtil_CarbonEvents.h"
#include "zoolib/ZUtil_Strim_Geom.h"

#if UNIVERSAL_INTERFACES_VERSION <= 0x0341
enum
	{
//	kEventMouseDragged = 6,
	kEventMouseEntered = 8,
	kEventMouseExited = 9
	};
#endif

using std::string;

using ZooLib::ZUtil_CarbonEvents::sGetParam_T;
using ZooLib::ZUtil_CarbonEvents::sDGetParam_T;
using ZooLib::ZUtil_CarbonEvents::sSetParam_T;

NAMESPACE_ZOOLIB_BEGIN

namespace ZNetscape {

// =================================================================================================

static void sWriteEvent(const ZStrimW&s, const EventRecord& iER)
	{
	s << "what: " << ZUtil_CarbonEvents::sEventTypeAsString(iER.what) << ", ";

	s.Writef("message: %u, when: %u, where: (%d, %d), modifiers: %u",
		iER.message, iER.when, iER.where.h, iER.where.v, iER.modifiers);
	}

#if defined(XP_MACOSX)

static void sSetFill(CGContextRef iCG, const ZRGBColor& iColor)
	{
	::CGContextSetRGBFillColor(iCG,
		iColor.floatRed(), iColor.floatGreen(),
		iColor.floatBlue(), iColor.floatAlpha());
	}

static void sSetStroke(CGContextRef iCG, const ZRGBColor& iColor)
	{
	::CGContextSetRGBStrokeColor(iCG,
		iColor.floatRed(), iColor.floatGreen(),
		iColor.floatBlue(), iColor.floatAlpha());
	}

static void sFillRectf(CGContextRef iCG, const ZGRectf& iRect, const ZRGBColor& iColor)
	{
	::CGContextSaveGState(iCG);
	sSetFill(iCG, iColor);
	::CGContextFillRect(iCG, iRect);
	::CGContextRestoreGState(iCG);	
	}

static HIRect& sHI(ZGRectf& iR)
	{ return *((HIRect*)&iR); }

static const HIRect& sHI(const ZGRectf& iR)
	{ return *((const HIRect*)&iR); }

static HIPoint& sHI(ZGPointf& iP)
	{ return *((HIPoint*)&iP); }

static const HIPoint& sHI(const ZGPointf& iP)
	{ return *((const HIPoint*)&iP); }

#endif // defined(XP_MACOSX)

// =================================================================================================
#pragma mark -
#pragma mark * Host_Mac

#if defined(XP_MAC) || defined(XP_MACOSX)

Host_Mac::Host_Mac(ZRef<GuestFactory> iGuestFactory, bool iAllowCG)
:	Host_Std(iGuestFactory),
	fAllowCG(iAllowCG),
	fEventLoopTimerRef_Idle(nullptr),
	fLeft(0), fTop(0), fRight(0), fBottom(0)
	{
	#if defined(XP_MACOSX)
		fUseCoreGraphics = false;
		fNP_CGContext.context = nullptr;
		fNP_CGContext.window = nullptr;
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
	fEventLoopTimerRef_Idle = nullptr;
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
			*static_cast<NPBool*>(ret_value) = fAllowCG;
			return NPERR_NO_ERROR;
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

void Host_Mac::pApplyInsets(ZGRectf& ioRect)
	{
	ioRect.origin.x += fLeft;
	ioRect.origin.y += fTop;
	ioRect.extent.h -= fLeft + fRight;
	ioRect.extent.v -= fTop + fBottom;
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
	EventRecord theER = iEvent;
	if (theER.what != nullEvent)
		{
		if (ZLOG(s, eDebug + 1, "Host_Mac"))
			{
			s << "DoEvent, ";
			sWriteEvent(s, theER);
			}
		}
	this->Guest_HandleEvent(&theER);
	}

void Host_Mac::DoSetWindow(const ZGRectf& iWinFrame)
	{
	this->DoSetWindow(
		iWinFrame.origin.x, iWinFrame.origin.y, iWinFrame.extent.h, iWinFrame.extent.v);
	}

void Host_Mac::DoSetWindow(int iX, int iY, int iWidth, int iHeight)
	{
	if (ZLOG(s, eDebug + 1, "Host_Mac"))
		s.Writef("DoSetWindow, (%d, %d, %d, %d)", iX, iY, iX + iWidth, iY + iHeight);

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

bool Host_Mac::pDeliverEvent(EventRef iEventRef)
	{
	EventRecord theER;
	if (::ConvertEventRefToEventRecord(iEventRef, &theER))
		{
		this->DoEvent(theER);
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

Host_WindowRef::Host_WindowRef(
	ZRef<ZNetscape::GuestFactory> iGF, bool iAllowCG, WindowRef iWindowRef)
:	Host_Mac(iGF, iAllowCG),
	fWindowRef(iWindowRef),
	fEventTargetRef_Window(nullptr),
	fEventHandlerRef_Window(nullptr)
	{
	#if defined(XP_MACOSX)
		WindowAttributes theWA;
		::GetWindowAttributes(fWindowRef, &theWA);

		// This is a compositing window, you should use a Host_HIView.
		ZAssertStop(0, 0 == (theWA & kWindowCompositingAttribute));
	#endif

	fEventTargetRef_Window = ::GetWindowEventTarget(fWindowRef);

	static const EventTypeSpec sEvents_Window[] =
		{
		{ kEventClassWindow, kEventWindowActivated },
		{ kEventClassWindow, kEventWindowDeactivated },
		{ kEventClassWindow, kEventWindowFocusAcquired },
		{ kEventClassWindow, kEventWindowFocusRelinquish },
		{ kEventClassWindow, kEventWindowBoundsChanged },
		{ kEventClassWindow, kEventWindowHandleContentClick },
		{ kEventClassWindow, kEventWindowDrawContent },

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
	if (fEventHandlerRef_Window)
		::RemoveEventHandler(fEventHandlerRef_Window);
	fEventHandlerRef_Window = nullptr;

	fEventTargetRef_Window = nullptr;
	}

void Host_WindowRef::Host_InvalidateRect(NPP npp, NPRect* rect)
	{
	Rect theBounds;
	theBounds.left = rect->left + fLeft;
	theBounds.top = rect->top + fTop;
	theBounds.right = rect->right + fLeft;
	theBounds.bottom = rect->bottom + fTop;
	::InvalWindowRect(fWindowRef, &theBounds);
	}

void Host_WindowRef::PostCreateAndLoad()
	{
	if (false) {}
	#if defined(XP_MACOSX)
	else if (fUseCoreGraphics)
		{
		fNP_CGContext.window = fWindowRef;
		fNPWindow.window = &fNP_CGContext;
		}
	#endif // defined(XP_MACOSX)
	else
		{
		fNP_Port.port = ::GetWindowPort(fWindowRef);
		fNPWindow.window = &fNP_Port;
		}

	Rect winFrameRect;
	::GetWindowBounds(fWindowRef, kWindowGlobalPortRgn, &winFrameRect);
	winFrameRect.right -= winFrameRect.left;
	winFrameRect.bottom -= winFrameRect.top;
	winFrameRect.left = 0;
	winFrameRect.top = 0;
	::InvalWindowRect(fWindowRef, &winFrameRect);
	}

void Host_WindowRef::DoEvent(const EventRecord& iEvent)
	{
	EventRecord theER = iEvent;

	#if defined(XP_MACOSX)
		if (fUseCoreGraphics)
			{
			Rect contentRect;
			::GetWindowBounds(fWindowRef, kWindowContentRgn, &contentRect);
			Rect structureRect;
			::GetWindowBounds(fWindowRef, kWindowStructureRgn, &structureRect);

			theER.where.h += structureRect.left - contentRect.left;
			theER.where.v += structureRect.top - contentRect.top;
			}
	#endif

	Host_Mac::DoEvent(theER);
	}


EventHandlerUPP Host_WindowRef::sEventHandlerUPP_Window
	= NewEventHandlerUPP(sEventHandler_Window);

pascal OSStatus Host_WindowRef::sEventHandler_Window(
	EventHandlerCallRef iCallRef, EventRef iEventRef, void* iRefcon)
	{ return static_cast<Host_WindowRef*>(iRefcon)->EventHandler_Window(iCallRef, iEventRef); }

OSStatus Host_WindowRef::EventHandler_Window(EventHandlerCallRef iCallRef, EventRef iEventRef)
	{
	switch (::GetEventClass(iEventRef))
		{
        case kEventClassMouse:
			{
			#if defined(XP_MACOSX)
				if (::GetEventKind(iEventRef) == kEventMouseDown)
					{
					if (inContent == sDGetParam_T<WindowPartCode>(iEventRef,
						kEventParamWindowPartCode, typeWindowPartCode, 0))
						{
						this->pDeliverEvent(iEventRef);
						// Absorb the mousedown, so that the standard handler
						// does not absorb the mouseUp.
						return noErr;
						}
					}
			#endif

			this->pDeliverEvent(iEventRef);
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
				case kEventWindowDrawContent:
					{
					::CallNextEventHandler(iCallRef, iEventRef);

					Rect winFrameRect;
					::GetWindowBounds(fWindowRef, kWindowGlobalPortRgn, &winFrameRect);
					ZGRectf winFrame(winFrameRect.right - winFrameRect.left,
						winFrameRect.bottom - winFrameRect.top);

					if (ZLOG(s, eDebug + 1, "Host_WindowRef"))
						{
						s << "kEventWindowDrawContent: " << winFrame;
						}

					winFrameRect = winFrame;
					::EraseRect(&winFrameRect);

					this->pApplyInsets(winFrame);

					if (false) {}
					#if defined(XP_MACOSX)
					else if (fUseCoreGraphics)
						{
						CGrafPtr qdPort;
						GDHandle currentGDevice;
						::GetGWorld(&qdPort, &currentGDevice);

						CGContextRef cg;
						::QDBeginCGContext(qdPort, &cg);
						::CGContextSaveGState(cg);
						::CGContextTranslateCTM(cg, 0, winFrameRect.bottom - winFrameRect.top);
						::CGContextScaleCTM(cg, 1.0, -1.0);
						::CGContextTranslateCTM(cg, winFrame.origin.x, winFrame.origin.y);

						fNP_CGContext.context = cg;
						this->DoSetWindow(winFrame);
						this->DoEvent(updateEvt, (UInt32)fWindowRef);
						::CGContextRestoreGState(cg);

						::CGContextSynchronize(cg);

						::QDEndCGContext(qdPort, &cg);
						}
					#endif // defined(XP_MACOSX)
					else
						{
						this->DoSetWindow(winFrame);
						this->DoEvent(updateEvt, (UInt32)fWindowRef);
						}

					return noErr;
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
					#if defined(XP_MACOSX)
						if (!fUseCoreGraphics)
							{
							ZGRectf newFrame = sGetParam_T<Rect>(iEventRef,
								kEventParamCurrentBounds, typeQDRectangle);

							if (ZLOG(s, eDebug + 1, "Host_WindowRef"))
								s << "kEventWindowBoundsChanged"
								<< ", newFrame: " << newFrame;

							newFrame.origin = 0;

							Rect qdRect = newFrame;
							::InvalWindowRect(fWindowRef, &qdRect);

							this->pApplyInsets(newFrame);

							this->DoSetWindow(newFrame);
							}
					#endif
					break;
					}
				}
			break;
			}
		}

	return eventNotHandledErr;
	}

#endif // defined(XP_MAC) || defined(XP_MACOSX)

// =================================================================================================
#pragma mark -
#pragma mark * Host_HIViewRef

#if defined(XP_MACOSX)

Host_HIViewRef::Host_HIViewRef(
	ZRef<ZNetscape::GuestFactory> iGF, bool iAllowCG, HIViewRef iHIViewRef)
:	Host_Mac(iGF, iAllowCG),
	fHIViewRef(iHIViewRef),
	fEventTargetRef_View(nullptr),
	fEventHandlerRef_View(nullptr)
	{
	WindowAttributes theWA;
	::GetWindowAttributes(::HIViewGetWindow(fHIViewRef), &theWA);

	// This is not a compositing window, you should use an Host_WindowRef.
	ZAssertStop(0, 0 != (theWA & kWindowCompositingAttribute));

	fEventTargetRef_View = ::GetControlEventTarget(fHIViewRef);

	static const EventTypeSpec sEvents_View[] =
		{
		{ kEventClassKeyboard, kEventRawKeyDown },
		{ kEventClassKeyboard, kEventRawKeyRepeat },
		{ kEventClassKeyboard, kEventRawKeyUp },
		{ kEventClassKeyboard, kEventRawKeyModifiersChanged },

		{ kEventClassControl, kEventControlHitTest },
		{ kEventClassControl, kEventControlDraw },
		{ kEventClassControl, kEventControlSetFocusPart },
		{ kEventClassControl, kEventControlGetFocusPart },
		{ kEventClassControl, kEventControlActivate },
		{ kEventClassControl, kEventControlDeactivate },
		{ kEventClassControl, kEventControlSetCursor },
		{ kEventClassControl, kEventControlClick },
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
	fEventHandlerRef_View = nullptr;

	fEventTargetRef_View = nullptr;
	}

void Host_HIViewRef::Host_InvalidateRect(NPP npp, NPRect* rect)
	{
	ZGRectf theRect;
	theRect.origin.x = rect->left + fLeft;
	theRect.origin.y = rect->top + fTop;
	theRect.extent.h = rect->right - rect->left;
	theRect.extent.v = rect->bottom - rect->top;

	::HIViewSetNeedsDisplayInRect(fHIViewRef, &sHI(theRect), true);
	}

void Host_HIViewRef::PostCreateAndLoad()
	{
	WindowRef theWindowRef = ::HIViewGetWindow(fHIViewRef);

	if (false) {}
	#if defined(XP_MACOSX)
	else if (fUseCoreGraphics)
		{
		fNP_CGContext.window = theWindowRef;
		fNPWindow.window = &fNP_CGContext;
		}
	#endif // defined(XP_MACOSX)
	else
		{
		fNP_Port.port = ::GetWindowPort(theWindowRef);
		fNPWindow.window = &fNP_Port;
		}

	ZGRectf theFrame;
	::HIViewGetBounds(fHIViewRef, &sHI(theFrame));
	this->pApplyInsets(theFrame);
	::HIViewSetNeedsDisplayInRect(fHIViewRef, &sHI(theFrame), true);
	}

EventHandlerUPP Host_HIViewRef::sEventHandlerUPP_View = NewEventHandlerUPP(sEventHandler_View);

pascal OSStatus Host_HIViewRef::sEventHandler_View(
	EventHandlerCallRef iCallRef, EventRef iEventRef, void* iRefcon)
	{ return static_cast<Host_HIViewRef*>(iRefcon)->EventHandler_View(iCallRef, iEventRef); }

OSStatus Host_HIViewRef::EventHandler_View(EventHandlerCallRef iCallRef, EventRef iEventRef)
	{
	if (ZLOG(s, eDebug + 1, "Host_HIViewRef"))
		{
		s << ZUtil_CarbonEvents::sEventAsString(
			::GetEventClass(iEventRef), ::GetEventKind(iEventRef));
		}

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
					bool isFocused =
						kControlFocusNoPart != sDGetParam_T<ControlFocusPart>(iEventRef,
						kEventParamControlPart, typeControlPartCode, kControlFocusNoPart);

					Host_Mac::DoFocus(isFocused);

					return noErr;
					}
				case kEventControlClick:
					{
					::SetKeyboardFocus(::HIViewGetWindow(fHIViewRef),
						fHIViewRef, kControlIndicatorPart);
					break;
					}
				case kEventControlTrack:
					{
					WindowRef theWindowRef = ::HIViewGetWindow(fHIViewRef);

					ZGPointf startPoint = sGetParam_T<Point>(iEventRef,
						kEventParamMouseLocation, typeQDPoint);

					::HIPointConvert(&sHI(startPoint),
						kHICoordSpaceView, fHIViewRef, kHICoordSpace72DPIGlobal, theWindowRef);

					EventRecord theER;
					theER.what = mouseDown;
					theER.message = 0;
					theER.when = ::EventTimeToTicks(::GetCurrentEventTime());
					theER.where = startPoint;
					theER.modifiers = sGetParam_T<UInt32>(iEventRef,
						kEventParamKeyModifiers, typeUInt32);

					if (ZLOG(s, eDebug + 1, "Host_ViewRef"))
						{
						sWriteEvent(s, theER);
						}
					this->Guest_HandleEvent(&theER);

					for (;true;)
						{
						MouseTrackingResult	theResult;
						UInt32 theModifiers;
						::TrackMouseLocationWithOptions(
							(CGrafPtr)-1, 0, 0.02, &theER.where, &theModifiers, &theResult);
						theER.when = ::EventTimeToTicks(::GetCurrentEventTime());
						ZLOG(s, eDebug + 1, "Host_ViewRef");
						if (theResult == kMouseTrackingMouseReleased)
							{
							theER.what = mouseUp;
							sWriteEvent(s, theER);
							this->Guest_HandleEvent(&theER);
							break;
							}
						else
							{
							theER.what = nullEvent;
							sWriteEvent(s, theER);
							this->Guest_HandleEvent(&theER);							
							}
						}
					return noErr;
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

					WindowRef theWindowRef = ::HIViewGetWindow(fHIViewRef);

					ZGRectf theFrame;
					::HIViewGetBounds(fHIViewRef, &sHI(theFrame));
					this->pApplyInsets(theFrame);

					ZGRectf winFrame = theFrame;

					if (ZLOG(s, eDebug + 1, "Host_HIViewRef"))
						s << "draw, winFrame: " << winFrame;

					if (fUseCoreGraphics)
						{
						::HIRectConvert(&sHI(winFrame),
							kHICoordSpaceView, fHIViewRef, kHICoordSpaceWindow, theWindowRef);

						if (CGContextRef theCG = sGetParam_T<CGContextRef>(iEventRef,
							kEventParamCGContextRef, typeCGContextRef))
							{
							fNP_CGContext.context = theCG;

							::CGContextSaveGState(theCG);
							::CGContextTranslateCTM(theCG, theFrame.origin.x, theFrame.origin.y);

							this->DoSetWindow(winFrame);

							this->DoEvent(updateEvt, (UInt32)theWindowRef);

							::CGContextRestoreGState(theCG);

							fNP_CGContext.context = nullptr;
							}
						}
					else
						{
						this->DoSetWindow(winFrame);
						this->DoEvent(updateEvt, (UInt32)theWindowRef);
						}
					return noErr;
					}
				case kEventControlBoundsChanged:
					{
					WindowRef theWindowRef = ::HIViewGetWindow(fHIViewRef);

					ZGRectf newFrame = sGetParam_T<Rect>(iEventRef,
						kEventParamCurrentBounds, typeQDRectangle);

					if (ZLOG(s, eDebug + 1, "Host_HIViewRef"))
						s << "kEventControlBoundsChanged"
						<< ", newFrame: " << newFrame;

					if (fUseCoreGraphics)
						{
						::HIRectConvert(&sHI(newFrame),
							kHICoordSpaceView, fHIViewRef, kHICoordSpaceWindow, theWindowRef);
						}
					else
						{
						newFrame.origin = 0;
						}

					::HIViewSetNeedsDisplayInRect(fHIViewRef, &sHI(ZGRectf(newFrame.extent)), true);

					this->pApplyInsets(newFrame);
						

					if (ZLOG(s, eDebug + 1, "Host_HIViewRef"))
						s << "kEventControlBoundsChanged, winFrame: " << newFrame;

					this->DoSetWindow(newFrame);
					break;
					}
				}
			break;
			}
		}

	return eventNotHandledErr;
	}

#endif // defined(XP_MACOSX)

} // namespace ZNetscape

NAMESPACE_ZOOLIB_END

#endif // defined(XP_MAC) || defined(XP_MACOSX)
