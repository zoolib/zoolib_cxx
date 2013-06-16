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

#include "zoolib/ZMacFixup.h"

#include "zoolib/netscape/ZNetscape_Host_Mac.h"

#if ! ZCONFIG_Is64Bit

#if defined(XP_MAC) || defined(XP_MACOSX)

#include "zoolib/ZCartesian.h"
#include "zoolib/ZCartesian_CG.h"
#include "zoolib/ZCartesian_QD.h"
#include "zoolib/ZLog.h"
#include "zoolib/ZMemory.h"
#include "zoolib/ZRGBA.h"
#include "zoolib/ZStream_String.h"
#include "zoolib/ZUtil_CarbonEvents.h"
#include "zoolib/ZUtil_Strim_Cartesian.h"
#include "zoolib/ZUtil_Strim_Operators.h"

#include "zoolib/netscape/ZCartesian_Netscape.h"

#if UNIVERSAL_INTERFACES_VERSION <= 0x0341
enum
	{
//	kEventMouseDragged = 6,
	kEventMouseEntered = 8,
	kEventMouseExited = 9
	};
#endif


namespace ZooLib {

using std::string;

using ZUtil_CarbonEvents::sGetParam_T;
using ZUtil_CarbonEvents::sDGetParam_T;
using ZUtil_CarbonEvents::sSetParam_T;

namespace ZNetscape {


// =================================================================================================

static void spWriteEvent(const ZStrimW& w, const EventRecord& iER)
	{
	w	<< "what: " << ZUtil_CarbonEvents::sEventTypeAsString(iER.what)
		<< ", message: " << iER.message
		<< ", when: " << iER.when
		<< ", where: " << iER.where
		<< ", modifiers: " << iER.modifiers;
	}

#if defined(XP_MACOSX)

#endif // defined(XP_MACOSX)

// =================================================================================================
// MARK: - Host_Mac

#if defined(XP_MAC) || defined(XP_MACOSX)

template <class Rect_p>
Rect_p Host_Mac::pApplyInsets(const Rect_p& iRect)
	{ return sOffsettedRB(fRight, fBottom, sInsetted(fLeft, fTop, iRect)); }

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

	#if !defined(NP_NO_QUICKDRAW)
		ZMemZero_T(fNP_Port);
	#endif

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
			if (ZLOGPF(s, eDebug))
				s << "Host_SetValue, NPPVpluginDrawingModel";
			if (reinterpret_cast<intptr_t>(value) == NPDrawingModelCoreGraphics)
				fUseCoreGraphics = true;
			else
				fUseCoreGraphics = false;
			return NPERR_NO_ERROR;
			}
		#endif // defined(XP_MACOSX)
		}

	return Host_Std::Host_SetValue(npp, variable, value);
	}

static void spStuffEventRecord(EventRecord& oEventRecord)
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
	spStuffEventRecord(theER);

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

void Host_Mac::DoEvent(EventKind iWhat, uint32 iMessage)
	{
	EventRecord theER;
	spStuffEventRecord(theER);

	theER.what = iWhat;
	theER.message = iMessage;

	this->DoEvent(theER);
	}

void Host_Mac::DoEvent(const EventRecord& iEvent)
	{
	EventRecord theER = iEvent;
	if (theER.what != nullEvent)
		{
		if (ZLOGPF(s, eDebug+1))
			{
			s << "DoEvent, ";
			spWriteEvent(s, theER);
			}
		}
	this->Guest_HandleEvent(&theER);
	}

void Host_Mac::DoSetWindow(const Rect& iWinFrame)
	{
	this->DoSetWindow(L(iWinFrame), T(iWinFrame), W(iWinFrame), H(iWinFrame));
	}

void Host_Mac::DoSetWindow(int iX, int iY, int iWidth, int iHeight)
	{
	if (ZLOGPF(s, eDebug + 1))
		s.Writef("DoSetWindow, (%d, %d, %d, %d)", iX, iY, iX + iWidth, iY + iHeight);

	#if !defined(NP_NO_QUICKDRAW)
		fNP_Port.portx = -iX;
		fNP_Port.porty = -iY;
	#endif

	fNPWindow.x = iX;
	fNPWindow.y = iY;
	fNPWindow.width = iWidth;
	fNPWindow.height = iHeight;

	fNPWindow.clipRect = sRect<NPRect>(iWidth, iHeight);

	#define CHECKIT(a) if (fNPWindow_Prior.a != fNPWindow.a) callIt = true;
	bool callIt = false;
	CHECKIT(window);
	CHECKIT(x);
	CHECKIT(y);
	CHECKIT(width);
	CHECKIT(height);
	CHECKIT(clipRect.top);
	CHECKIT(clipRect.left);
	CHECKIT(clipRect.bottom);
	CHECKIT(clipRect.right);
	#undef CHECKIT
	fNPWindow_Prior = fNPWindow;

	#if defined(XP_MACOSX)
		if (fUseCoreGraphics)
			{
			if (fNP_CGContext_Prior.context != fNP_CGContext.context)
				callIt = true;
			if (fNP_CGContext_Prior.window != fNP_CGContext.window)
				callIt = true;
			fNP_CGContext_Prior = fNP_CGContext;
			}

	#endif

	if (callIt)
		{
		this->Guest_SetWindow(&fNPWindow);
		fNPWindow_Prior = fNPWindow;
		}
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

EventLoopTimerUPP Host_Mac::sEventLoopTimerUPP_Idle =
	NewEventLoopTimerUPP(sEventLoopTimer_Idle);

pascal void Host_Mac::sEventLoopTimer_Idle(EventLoopTimerRef iTimer, void* iRefcon)
	{ static_cast<Host_Mac*>(iRefcon)->EventLoopTimer_Idle(iTimer); }

void Host_Mac::EventLoopTimer_Idle(EventLoopTimerRef iTimer)
	{
	Host_Mac::DeliverData();
	Host_Mac::DoIdle();
	}

#endif // defined(XP_MAC) || defined(XP_MACOSX)

// =================================================================================================
// MARK: - Host_WindowRef

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

	static const EventTypeSpec spEvents_Window[] =
		{
		{ kEventClassWindow, kEventWindowActivated },
		{ kEventClassWindow, kEventWindowDeactivated },
		{ kEventClassWindow, kEventWindowFocusAcquired },
		{ kEventClassWindow, kEventWindowFocusRelinquish },
		{ kEventClassWindow, kEventWindowBoundsChanged },
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
		countof(spEvents_Window), spEvents_Window,
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
	#if !defined(NP_NO_QUICKDRAW)
	else if (true)
		{
		fNP_Port.port = ::GetWindowPort(fWindowRef);
		fNPWindow.window = &fNP_Port;
		}
	#endif // !defined(NP_NO_QUICKDRAW)
	else
		{
		ZUnimplemented();
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

//			theER.where.h -= structureRect.left - contentRect.left;
//			theER.where.v -= structureRect.top - contentRect.top;
			}
	#endif

	Host_Mac::DoEvent(theER);
	}

EventHandlerUPP Host_WindowRef::sEventHandlerUPP_Window =
	NewEventHandlerUPP(sEventHandler_Window);

pascal OSStatus Host_WindowRef::sEventHandler_Window(
	EventHandlerCallRef iCallRef, EventRef iEventRef, void* iRefcon)
	{ return static_cast<Host_WindowRef*>(iRefcon)->EventHandler_Window(iCallRef, iEventRef); }

OSStatus Host_WindowRef::EventHandler_Window(EventHandlerCallRef iCallRef, EventRef iEventRef)
	{
	switch (::GetEventClass(iEventRef))
		{
		case kEventClassMouse:
			{
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
				case kEventWindowDrawContent:
					{
					::CallNextEventHandler(iCallRef, iEventRef);

					Rect winFrameRect;
					::GetWindowBounds(fWindowRef, kWindowGlobalPortRgn, &winFrameRect);

					{
					using namespace ZCartesian::Operators;
					winFrameRect = sOffsetted(-LT(winFrameRect), winFrameRect);
					}

					if (ZLOGPF(s, eDebug + 1))
						{
						s << "kEventWindowDrawContent: " << winFrameRect;
						}

					::EraseRect(&winFrameRect);

					winFrameRect = this->pApplyInsets(winFrameRect);

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
						::CGContextTranslateCTM(cg, 0, H(winFrameRect));
						::CGContextScaleCTM(cg, 1.0, -1.0);
						::CGContextTranslateCTM(cg, L(winFrameRect), T(winFrameRect));

						fNP_CGContext.context = cg;

						Rect contentRect;
						::GetWindowBounds(fWindowRef, kWindowContentRgn, &contentRect);
						Rect structureRect;
						::GetWindowBounds(fWindowRef, kWindowStructureRgn, &structureRect);

						{
						using namespace ZCartesian::Operators;
						winFrameRect = sOffsetted(LT(contentRect) - LT(structureRect), winFrameRect);
						}

						this->DoSetWindow(winFrameRect);
						this->DoEvent(updateEvt, (UInt32)fWindowRef);

						::CGContextRestoreGState(cg);

						::CGContextSynchronize(cg);

						::QDEndCGContext(qdPort, &cg);
						}
					#endif // defined(XP_MACOSX)
					else
						{
						this->DoSetWindow(winFrameRect);
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
						Rect newFrame = sGetParam_T<Rect>(iEventRef,
							kEventParamCurrentBounds, typeQDRectangle);

						if (ZLOGPF(s, eDebug + 1))
							s << "kEventWindowBoundsChanged"
							<< ", newFrame: " << newFrame;

						newFrame = sRect(WH(newFrame));

						::InvalWindowRect(fWindowRef, &newFrame);

						if (!fUseCoreGraphics)
							{
							newFrame = this->pApplyInsets(newFrame);
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
// MARK: - Host_HIViewRef

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

	static const EventTypeSpec spEvents_View[] =
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
		countof(spEvents_View), spEvents_View,
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
	{ ::HIViewSetNeedsDisplayInRect(fHIViewRef, sConstPtr& sRect<CGRect>(*rect), true); }

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
	#if !defined(NP_NO_QUICKDRAW)
	else if (true)
		{
		fNP_Port.port = ::GetWindowPort(theWindowRef);
		fNPWindow.window = &fNP_Port;
		}
	#endif // !defined(NP_NO_QUICKDRAW)
	else
		{
		ZUnimplemented();
		}

	CGRect theFrame;
	::HIViewGetBounds(fHIViewRef, &theFrame);
	theFrame = this->pApplyInsets(theFrame);
	::HIViewSetNeedsDisplayInRect(fHIViewRef, &theFrame, true);
	}

EventHandlerUPP Host_HIViewRef::sEventHandlerUPP_View = NewEventHandlerUPP(sEventHandler_View);

pascal OSStatus Host_HIViewRef::sEventHandler_View(
	EventHandlerCallRef iCallRef, EventRef iEventRef, void* iRefcon)
	{ return static_cast<Host_HIViewRef*>(iRefcon)->EventHandler_View(iCallRef, iEventRef); }

OSStatus Host_HIViewRef::EventHandler_View(EventHandlerCallRef iCallRef, EventRef iEventRef)
	{
	if (ZLOGPF(s, eDebug + 1))
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
					Host_Mac::DoActivate(true);
					break;
					}
				case kEventControlDeactivate:
					{
					Host_Mac::DoActivate(false);
					break;
					}
				case kEventControlSetFocusPart:
					{
					bool isFocused =
						kControlFocusNoPart != sDGetParam_T<ControlFocusPart>(
						kControlFocusNoPart,
						iEventRef, kEventParamControlPart, typeControlPartCode);

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

					CGPoint startPoint = sPoint<CGPoint>(
						sGetParam_T<Point>(iEventRef, kEventParamMouseLocation, typeQDPoint));

					::HIPointConvert(&startPoint,
						kHICoordSpaceView, fHIViewRef, kHICoordSpace72DPIGlobal, theWindowRef);

					EventRecord theER;
					theER.what = mouseDown;
					theER.message = 0;
					theER.when = ::EventTimeToTicks(::GetCurrentEventTime());
					theER.where = sPoint<Point>(startPoint);
					theER.modifiers =
						sGetParam_T<UInt32>(iEventRef,kEventParamKeyModifiers, typeUInt32);

					if (ZLOGPF(s, eDebug + 1))
						{
						spWriteEvent(s, theER);
						}

					this->Guest_HandleEvent(&theER);

					for (;;)
						{
						MouseTrackingResult	theResult;
						UInt32 theModifiers;
						::TrackMouseLocationWithOptions(
							(CGrafPtr)-1, 0, 0.02, &theER.where, &theModifiers, &theResult);
						theER.when = ::EventTimeToTicks(::GetCurrentEventTime());
						ZLOGPF(s, eDebug + 1);
						if (theResult == kMouseTrackingMouseReleased)
							{
							theER.what = mouseUp;
							spWriteEvent(s, theER);
							this->Guest_HandleEvent(&theER);
							break;
							}
						else
							{
							theER.what = nullEvent;
							spWriteEvent(s, theER);
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

					CGRect theFrame;
					::HIViewGetBounds(fHIViewRef, &theFrame);
					theFrame = this->pApplyInsets(theFrame);

					if (ZLOGPF(s, eDebug + 1))
						s << "draw, theFrame: " << theFrame;

					if (fUseCoreGraphics)
						{
						::HIRectConvert(&theFrame,
							kHICoordSpaceView, fHIViewRef, kHICoordSpaceWindow, theWindowRef);

						if (CGContextRef theCG = sGetParam_T<CGContextRef>(iEventRef,
							kEventParamCGContextRef, typeCGContextRef))
							{
							fNP_CGContext.context = theCG;

							::CGContextSaveGState(theCG);
							::CGContextTranslateCTM(theCG, L(theFrame), T(theFrame));

							this->DoSetWindow(sRect<Rect>(theFrame));

							this->DoEvent(updateEvt, (UInt32)theWindowRef);

							::CGContextRestoreGState(theCG);

							fNP_CGContext.context = nullptr;
							}
						}
					else
						{
						this->DoSetWindow(sRect<Rect>(theFrame));
						this->DoEvent(updateEvt, (UInt32)theWindowRef);
						}
					return noErr;
					}
				case kEventControlBoundsChanged:
					{
					// WindowRef theWindowRef = ::HIViewGetWindow(fHIViewRef);

					Rect newFrame = sGetParam_T<Rect>(iEventRef,
						kEventParamCurrentBounds, typeQDRectangle);

					if (ZLOGPF(s, eDebug + 1))
						{
						s << "kEventControlBoundsChanged"
						<< ", newFrame1: " << newFrame;
						}

					if (!fUseCoreGraphics)
						newFrame = sRect(WH(newFrame));

					::HIViewSetNeedsDisplayInRect(
						fHIViewRef, sConstPtr& sRect<CGRect>(newFrame), true);

					newFrame = this->pApplyInsets(newFrame);

					if (ZLOGPF(s, eDebug + 1))
						s << "kEventControlBoundsChanged, newFrame2: " << newFrame;

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
} // namespace ZooLib

#endif // ! ZCONFIG_Is64Bit
#endif // defined(XP_MAC) || defined(XP_MACOSX)
