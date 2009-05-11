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

#include "zoolib/ZByteSwap.h"
#include "zoolib/ZLog.h"
#include "zoolib/ZString.h"
#include "zoolib/ZThreadImp.h"
#include "zoolib/ZTypes.h"

NAMESPACE_ZOOLIB_BEGIN

using std::string;

#if defined(UNIVERSAL_INTERFACES_VERSION) && UNIVERSAL_INTERFACES_VERSION <= 0x0341
	#define USING_ANCIENT_HEADERS 1
#else
	#define USING_ANCIENT_HEADERS 0
#endif

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
			KIND(kEventWindowUpdate);
			KIND(kEventWindowDrawContent);
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
			KIND(kEventWindowClickDragRgn);
			KIND(kEventWindowClickResizeRgn);
			KIND(kEventWindowClickCollapseRgn);
			KIND(kEventWindowClickCloseRgn);
			KIND(kEventWindowClickZoomRgn);
			KIND(kEventWindowClickContentRgn);
			KIND(kEventWindowClickProxyIconRgn);
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
			KIND(kEventWindowHandleContentClick);
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
			KIND(kEventWindowClickToolbarButtonRgn);
			KIND(kEventWindowClickStructureRgn);
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
			KIND(kEventControlApplyBackground);
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
			KIND(kEventControlArbitraryMessage);

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
		theKind = ZString::sFormat("%u", iEK);

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
	return ZString::sFormat("%u", iEventType);
	}
		
NAMESPACE_ZOOLIB_END

#endif // ZCONFIG_SPI_Enabled(Carbon)
