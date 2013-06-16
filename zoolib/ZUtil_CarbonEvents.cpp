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

#if ZCONFIG_SPI_Enabled(Carbon64)

#include "zoolib/ZByteSwap.h"
#include "zoolib/ZLog.h"
#include "zoolib/ZStringf.h"
#include "zoolib/ZThread.h"
#include "zoolib/ZTypes.h"

namespace ZooLib {

using std::string;

#if defined(UNIVERSAL_INTERFACES_VERSION) && UNIVERSAL_INTERFACES_VERSION > 0x0341
	#define USING_ANCIENT_HEADERS 0
#else
	#define USING_ANCIENT_HEADERS 1
#endif

// =================================================================================================
// MARK: - ZUtil_CarbonEvents

bool ZUtil_CarbonEvents::sQGetParam(EventRef iEventRef, EventParamName iName, EventParamType iType,
	size_t iBufferSize, void* oBuffer)
	{
	return noErr == ::GetEventParameter(
		iEventRef, iName, iType, nullptr, iBufferSize, nullptr, oBuffer);
	}

size_t ZUtil_CarbonEvents::sGetParamLength(
	EventRef iEventRef, EventParamName iName, EventParamType iType)
	{
	ByteCount theLength;
	if (noErr == ::GetEventParameter(iEventRef, iName, iType, nullptr, 0, &theLength, nullptr))
		return theLength;
	return 0;
	}

void ZUtil_CarbonEvents::sSetParam(EventRef iEventRef, EventParamName iName, EventParamType iType,
	size_t iBufferSize, const void* iBuffer)
	{
	::SetEventParameter(iEventRef, iName, iType, iBufferSize, iBuffer);
	}

// =================================================================================================
// MARK: - Handler

namespace { // anonymous

using namespace ZUtil_CarbonEvents;

static const UInt32 kEventClassID_ZooLib = 'ZooL';
static const UInt32 kEventKindID_Call = 'Call';

class Handler
	{
public:
	Handler();
	~Handler();

	void InvokeOnMainThread(bool iAlwaysDelayed, CallbackProc_t iCallback, void* iRefcon);

private:
	static EventHandlerUPP spEventHandlerUPP;
	static pascal OSStatus spEventHandler(
		EventHandlerCallRef iCallRef, EventRef iEventRef, void* iRefcon);
	OSStatus EventHandler(EventHandlerCallRef iCallRef, EventRef iEventRef);

	EventQueueRef fEventQueueRef;
	EventHandlerRef fEventHandlerRef;
	ZThread::ID fMainID;
	};

struct Context_t
	{
	CallbackProc_t fCallbackProc;
	void* fRefcon;
	};

Handler::Handler()
	{
	fMainID = ZThread::sID();

	fEventQueueRef = ::GetMainEventQueue();

	static const EventTypeSpec sEvents[] =
		{
		{ kEventClassID_ZooLib, kEventKindID_Call }
		};

	::InstallEventHandler(::GetEventDispatcherTarget(), spEventHandlerUPP,
		countof(sEvents), sEvents,
		this, &fEventHandlerRef);
	}

Handler::~Handler()
	{
	if (fEventHandlerRef)
		::RemoveEventHandler(fEventHandlerRef);
	fEventHandlerRef = nullptr;

	fEventQueueRef = nullptr;
	}

void Handler::InvokeOnMainThread(bool iAlwaysDelayed, CallbackProc_t iCallbackProc, void* iRefcon)
	{
	if (not iAlwaysDelayed && fMainID == ZThread::sID())
		{
		iCallbackProc(iRefcon);
		}
	else
		{
		EventRef theEventRef;
		::MacCreateEvent(nullptr, kEventClassID_ZooLib, kEventKindID_Call,
			::GetCurrentEventTime(), kEventAttributeNone, &theEventRef);

		Context_t theContext;
		theContext.fCallbackProc = iCallbackProc;
		theContext.fRefcon = iRefcon;
		sSetParam_T(theEventRef, 'Cont', typeData, theContext);

		::PostEventToQueue(fEventQueueRef, theEventRef, kEventPriorityStandard);
		::ReleaseEvent(theEventRef);
		}
	}

EventHandlerUPP Handler::spEventHandlerUPP = NewEventHandlerUPP(spEventHandler);

pascal OSStatus Handler::spEventHandler(
	EventHandlerCallRef iCallRef, EventRef iEventRef, void* iRefcon)
	{
	try
		{
		return static_cast<Handler*>(iRefcon)->EventHandler(iCallRef, iEventRef);
		}
	catch (std::exception& ex)
		{
		if (ZLOGPF(s, eNotice))
			s << "Uncaught exception: " << ex.what();
		}
	catch (...)
		{
		if (ZLOGPF(s, eNotice))
			s << "uncaught exception, not derived fron std::exception";
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
						theContext.fCallbackProc(theContext.fRefcon);
					return noErr;
					}
				}
			break;
			}
		}
	return eventNotHandledErr;
	}

Handler spHandler;

} // anonymous namespace

void ZUtil_CarbonEvents::sInvokeOnMainThread(
	bool iAlwaysDelayed, CallbackProc_t iCallback, void* iRefcon)
	{ spHandler.InvokeOnMainThread(iAlwaysDelayed, iCallback, iRefcon); }

void ZUtil_CarbonEvents::sInvokeOnMainThread(CallbackProc_t iCallback, void* iRefcon)
	{ spHandler.InvokeOnMainThread(false, iCallback, iRefcon); }

#define CLASSONLY(a) case a: theClass = #a; break
#define CLASS(a) case a: theClass = #a
#define KIND(a) case a: theKind = #a; break

string ZUtil_CarbonEvents::sEventAsString(EventClass iEC, EventKind iEK)
	{
	string theClass;
	string theKind;
	switch (iEC)
		{
		CLASSONLY(kEventClassTextInput);
		CLASSONLY(kEventClassAppleEvent);
		CLASSONLY(kEventClassTablet);
		CLASSONLY(kEventClassVolume);

		#if !USING_ANCIENT_HEADERS
		CLASSONLY(kEventClassAppearance);
		CLASSONLY(kEventClassService);
		CLASSONLY(kEventClassToolbar);
		CLASSONLY(kEventClassToolbarItem);
		CLASSONLY(kEventClassToolbarItemView);
		CLASSONLY(kEventClassAccessibility);
		CLASSONLY(kEventClassSystem);
		CLASSONLY(kEventClassInk);
		CLASSONLY(kEventClassTSMDocumentAccess);
		#endif // !USING_ANCIENT_HEADERS

		CLASS(kEventClassMouse);
		switch (iEK)
			{
			KIND(kEventMouseDown);
			KIND(kEventMouseUp);
			KIND(kEventMouseMoved);
			KIND(kEventMouseDragged);
			KIND(kEventMouseWheelMoved);

			#if !USING_ANCIENT_HEADERS
			KIND(kEventMouseEntered);
			KIND(kEventMouseExited);
			#endif // !USING_ANCIENT_HEADERS
			}
		break;

		CLASS(kEventClassKeyboard);
		switch (iEK)
			{
			KIND(kEventRawKeyDown);
			KIND(kEventRawKeyRepeat);
			KIND(kEventRawKeyUp);
			KIND(kEventRawKeyModifiersChanged);
			KIND(kEventHotKeyPressed);
			KIND(kEventHotKeyReleased);
			}
		break;

		CLASS(kEventClassApplication);
		switch (iEK)
			{
			KIND(kEventAppActivated);
			KIND(kEventAppDeactivated);
			KIND(kEventAppQuit);
			KIND(kEventAppLaunchNotification);
			KIND(kEventAppLaunched);
			KIND(kEventAppTerminated);
			KIND(kEventAppFrontSwitched);

			#if !USING_ANCIENT_HEADERS
			KIND(kEventAppFocusMenuBar);
			KIND(kEventAppFocusNextDocumentWindow);
			KIND(kEventAppFocusNextFloatingWindow);
			KIND(kEventAppFocusToolbar);
			KIND(kEventAppFocusDrawer);
			KIND(kEventAppGetDockTileMenu);
			KIND(kEventAppIsEventInInstantMouser);
			KIND(kEventAppHidden);
			KIND(kEventAppShown);
			KIND(kEventAppSystemUIModeChanged);
			KIND(kEventAppAvailableWindowBoundsChanged);
			KIND(kEventAppActiveWindowChanged);
			#endif // !USING_ANCIENT_HEADERS
			}
		break;

		CLASS(kEventClassWindow);
		switch (iEK)
			{
			#if ZCONFIG_SPI_Enabled(Carbon)
				KIND(kEventWindowUpdate);
				KIND(kEventWindowDrawContent);
			#endif
			KIND(kEventWindowActivated);
			KIND(kEventWindowDeactivated);
			KIND(kEventWindowGetClickActivation);
			KIND(kEventWindowShowing);
			KIND(kEventWindowHiding);
			KIND(kEventWindowShown);
			KIND(kEventWindowHidden);
			KIND(kEventWindowCollapsed);
			KIND(kEventWindowExpanded);
			KIND(kEventWindowZoomed);
			KIND(kEventWindowBoundsChanging);
			KIND(kEventWindowBoundsChanged);
			KIND(kEventWindowResizeStarted);
			KIND(kEventWindowResizeCompleted);
			KIND(kEventWindowDragStarted);
			KIND(kEventWindowDragCompleted);
			KIND(kEventWindowClosed);
			#if ZCONFIG_SPI_Enabled(Carbon)
				KIND(kEventWindowClickDragRgn);
				KIND(kEventWindowClickResizeRgn);
				KIND(kEventWindowClickCollapseRgn);
				KIND(kEventWindowClickCloseRgn);
				KIND(kEventWindowClickZoomRgn);
				KIND(kEventWindowClickContentRgn);
				KIND(kEventWindowClickProxyIconRgn);
			#endif
			KIND(kEventWindowCursorChange);
			KIND(kEventWindowCollapse);
			KIND(kEventWindowCollapseAll);
			KIND(kEventWindowExpand);
			KIND(kEventWindowExpandAll);
			KIND(kEventWindowClose);
			KIND(kEventWindowCloseAll);
			KIND(kEventWindowZoom);
			KIND(kEventWindowZoomAll);
			KIND(kEventWindowContextualMenuSelect);
			KIND(kEventWindowPathSelect);
			KIND(kEventWindowGetIdealSize);
			KIND(kEventWindowGetMinimumSize);
			KIND(kEventWindowGetMaximumSize);
			KIND(kEventWindowConstrain);
			#if ZCONFIG_SPI_Enabled(Carbon)
				KIND(kEventWindowHandleContentClick);
			#endif
			KIND(kEventWindowProxyBeginDrag);
			KIND(kEventWindowProxyEndDrag);
			KIND(kEventWindowFocusAcquired);
			KIND(kEventWindowFocusRelinquish);
			KIND(kEventWindowDrawFrame);
			KIND(kEventWindowDrawPart);
			KIND(kEventWindowGetRegion);
			KIND(kEventWindowHitTest);
			KIND(kEventWindowInit);
			KIND(kEventWindowDispose);
			KIND(kEventWindowDragHilite);
			KIND(kEventWindowModified);
			KIND(kEventWindowSetupProxyDragImage);
			KIND(kEventWindowStateChanged);
			KIND(kEventWindowMeasureTitle);
			KIND(kEventWindowDrawGrowBox);
			KIND(kEventWindowGetGrowImageRegion);
			KIND(kEventWindowPaint);

			#if !USING_ANCIENT_HEADERS
			KIND(kEventWindowHandleActivate);
			KIND(kEventWindowHandleDeactivate);
			KIND(kEventWindowGetClickModality);
			KIND(kEventWindowCollapsing);
			KIND(kEventWindowExpanding);
			KIND(kEventWindowTransitionStarted);
			KIND(kEventWindowTransitionCompleted);
			#if ZCONFIG_SPI_Enabled(Carbon)
				KIND(kEventWindowClickToolbarButtonRgn);
				KIND(kEventWindowClickStructureRgn);
			#endif
			KIND(kEventWindowFocusDrawer);
			KIND(kEventWindowGetDockTileMenu);
			KIND(kEventWindowToolbarSwitchMode);
			KIND(kEventWindowFocusContent);
			KIND(kEventWindowFocusToolbar);
			KIND(kEventWindowAttributesChanged);
			KIND(kEventWindowSheetOpening);
			KIND(kEventWindowSheetOpened);
			KIND(kEventWindowSheetClosing);
			KIND(kEventWindowSheetClosed);
			KIND(kEventWindowDrawerOpening);
			KIND(kEventWindowDrawerOpened);
			KIND(kEventWindowDrawerClosing);
			KIND(kEventWindowDrawerClosed);
			KIND(kEventWindowTitleChanged);
			#endif // !USING_ANCIENT_HEADERS
			}
		break;

		CLASS(kEventClassMenu);
		switch (iEK)
			{
			KIND(kEventMenuBeginTracking);
			KIND(kEventMenuEndTracking);
			KIND(kEventMenuChangeTrackingMode);
			KIND(kEventMenuOpening);
			KIND(kEventMenuClosed);
			KIND(kEventMenuTargetItem);
			KIND(kEventMenuMatchKey);
			KIND(kEventMenuEnableItems);
			KIND(kEventMenuDispose);

			#if !USING_ANCIENT_HEADERS
			KIND(kEventMenuPopulate);
			KIND(kEventMenuMeasureItemWidth);
			KIND(kEventMenuMeasureItemHeight);
			KIND(kEventMenuDrawItem);
			KIND(kEventMenuDrawItemContent);
			KIND(kEventMenuCalculateSize);
			KIND(kEventMenuCreateFrameView);
			KIND(kEventMenuGetFrameBounds);
			KIND(kEventMenuBecomeScrollable);
			KIND(kEventMenuCeaseToBeScrollable);
			KIND(kEventMenuBarShown);
			KIND(kEventMenuBarHidden);
			#endif // !USING_ANCIENT_HEADERS
			}
		break;

		CLASS(kEventClassCommand);
		switch (iEK)
			{
			KIND(kEventCommandProcess);
			KIND(kEventCommandUpdateStatus);
			}
		break;

		CLASS(kEventClassControl);
		switch (iEK)
			{
			KIND(kEventControlInitialize);
			KIND(kEventControlDispose);
			KIND(kEventControlGetOptimalBounds);
			KIND(kEventControlHit);
			KIND(kEventControlSimulateHit);
			KIND(kEventControlHitTest);
			KIND(kEventControlDraw);
			#if ZCONFIG_SPI_Enabled(Carbon)
				KIND(kEventControlApplyBackground);
			#endif
			KIND(kEventControlApplyTextColor);
			KIND(kEventControlSetFocusPart);
			KIND(kEventControlGetFocusPart);
			KIND(kEventControlActivate);
			KIND(kEventControlDeactivate);
			KIND(kEventControlSetCursor);
			KIND(kEventControlContextualMenuClick);
			KIND(kEventControlClick);
//			KIND(kEventControlTrackingAreaEntered); // In HIView.h
//			KIND(kEventControlTrackingAreaExited); // In HIView.h
			KIND(kEventControlTrack);
			KIND(kEventControlGetScrollToHereStartPoint);
			KIND(kEventControlGetIndicatorDragConstraint);
			KIND(kEventControlIndicatorMoved);
			KIND(kEventControlGhostingFinished);
			KIND(kEventControlGetActionProcPart);
			KIND(kEventControlGetPartRegion);
			KIND(kEventControlGetPartBounds);
			KIND(kEventControlSetData);
			KIND(kEventControlGetData);
			KIND(kEventControlValueFieldChanged);
			KIND(kEventControlAddedSubControl);
			KIND(kEventControlRemovingSubControl);
			KIND(kEventControlBoundsChanged);
			KIND(kEventControlOwningWindowChanged);
			#if ZCONFIG_SPI_Enabled(Carbon)
				KIND(kEventControlArbitraryMessage);
			#endif

			#if !USING_ANCIENT_HEADERS
			KIND(kEventControlGetNextFocusCandidate);
			KIND(kEventControlGetAutoToggleValue);
			KIND(kEventControlInterceptSubviewClick);
			KIND(kEventControlGetClickActivation);
			KIND(kEventControlDragEnter);
			KIND(kEventControlDragWithin);
			KIND(kEventControlDragLeave);
			KIND(kEventControlDragReceive);
			KIND(kEventControlInvalidateForSizeChange);
			KIND(kEventControlGetSizeConstraints);
			KIND(kEventControlGetFrameMetrics);
			KIND(kEventControlVisibilityChanged);
			KIND(kEventControlTitleChanged);
			KIND(kEventControlHiliteChanged);
			KIND(kEventControlEnabledStateChanged);
			KIND(kEventControlLayoutInfoChanged);
			#endif // !USING_ANCIENT_HEADERS
			}
		break;
		}

	if (theClass.empty())
		{
		uint32 asBE = ZByteSwap_HostToBig32(iEC);
		theClass = string((char*)&asBE, 4);
		}

	if (theKind.empty())
		theKind = sStringf("%u", iEK);

	return theClass + "/" + theKind;
	}

#define EVENT(a) case a: return #a

string ZUtil_CarbonEvents::sEventTypeAsString(UInt16 iEventType)
	{
	switch (iEventType)
		{
		EVENT(nullEvent);
		EVENT(mouseDown);
		EVENT(mouseUp);
		EVENT(keyDown);
		EVENT(keyUp);
		EVENT(autoKey);
		EVENT(updateEvt);
		EVENT(diskEvt);
		EVENT(activateEvt);
		EVENT(osEvt);
		EVENT(kHighLevelEvent);
		}
	return sStringf("%u", iEventType);
	}

} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(Carbon64)
