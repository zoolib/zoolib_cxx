#include "ZNSPlugin_FakeWindow.h"
#include "ZDC_GDI.h"
#include "ZDC_QD.h"
#include "ZUtil_Win.h"

#define kDebug_FakeWindow_NSPlugin 0

// =================================================================================================
#pragma mark -
#pragma mark * ZNSPluginMeister_FakeWindow

#if ZCONFIG(OS, Win32)

extern HINSTANCE sHINSTANCE;

static const UTF16 sWindowClassNameW[] = L"ZNSPlugin_FakeWindow::sWindowProcW";
static const char sWindowClassNameA[] = "ZNSPlugin_FakeWindow::sWindowProcA";

#endif // ZCONFIG(OS, Win32)

ZNSPluginMeister_FakeWindow::ZNSPluginMeister_FakeWindow()
	{
#if ZCONFIG(OS, Win32)
	ATOM theResult;
	if (ZUtil_Win::sUseWAPI())
		{
		WNDCLASSW windowClass;
		windowClass.style = 0;
		windowClass.lpfnWndProc = ZNSPlugin_FakeWindow::sWindowProcW;
		windowClass.cbClsExtra = 0;
		windowClass.cbWndExtra = 0;
		windowClass.hInstance = sHINSTANCE;
		windowClass.hIcon = nil;
		windowClass.hCursor = nil;
		windowClass.hbrBackground = nil;
		windowClass.lpszMenuName = nil;
		windowClass.lpszClassName = sWindowClassNameW;
		theResult = ::RegisterClassW(&windowClass);
		}
	else
		{
		WNDCLASSA windowClass;
		windowClass.style = 0;
		windowClass.lpfnWndProc = ZNSPlugin_FakeWindow::sWindowProcA;
		windowClass.cbClsExtra = 0;
		windowClass.cbWndExtra = 0;
		windowClass.hInstance = sHINSTANCE;
		windowClass.hIcon = nil;
		windowClass.hCursor = nil;
		windowClass.hbrBackground = nil;
		windowClass.lpszMenuName = nil;
		windowClass.lpszClassName = sWindowClassNameA;
		theResult = ::RegisterClassA(&windowClass);
		}
	ZAssertStop(kDebug_FakeWindow_NSPlugin, theResult);
#endif // ZCONFIG(OS, Win32)

#if ZCONFIG(OS, MacOS7) || ZCONFIG(OS, Carbon)
	ZDCScratch::sSet(new ZDCCanvas_QD_OffScreen(ZPoint(2,1), ZDCPixmapNS::eFormatEfficient_Color_32));
#elif ZCONFIG(OS, Win32)
	ZDCScratch::sSet(new ZDCCanvas_GDI_IC);
#endif
	}

ZNSPluginMeister_FakeWindow::~ZNSPluginMeister_FakeWindow()
	{
	ZDCScratch::sSet(ZRef<ZDCCanvas>());

#if ZCONFIG(OS, Win32)
	if (ZUtil_Win::sUseWAPI())
		::UnregisterClassW(sWindowClassNameW, sHINSTANCE);
	else
		::UnregisterClassA(sWindowClassNameA, sHINSTANCE);
#endif // ZCONFIG(OS, Win32)
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZNSPlugin_FakeWindow

ZNSPlugin_FakeWindow::ZNSPlugin_FakeWindow(ZNSPlugin::NPP iNPP)
:	ZNSPluginInstance(iNPP),
	ZFakeWindow(nil),
	fEntryCount(0)
	{
#if ZCONFIG(OS, MacOS7) || ZCONFIG(OS, Carbon)
	fGrafPort = nil;
#endif

#if ZCONFIG(OS, Win32)
	fHWND = nil;
#endif
	}

ZNSPlugin_FakeWindow::~ZNSPlugin_FakeWindow()
	{
	this->DeleteAllWindowPanes();
#if ZCONFIG(OS, Win32)
	if (fHWND)
		::DestroyWindow(fHWND);
	ZAssertStop(kDebug_FakeWindow_NSPlugin, !fHWND);
#endif
	}

ZNSPlugin::EError ZNSPlugin_FakeWindow::SetWindow(ZNSPlugin::NPWindow* iNPWindow)
	{
	if (iNPWindow)
		{
		#if ZCONFIG(OS, MacOS7) || ZCONFIG(OS, Carbon)
		fFrame.left = iNPWindow->fLocationH;
		fFrame.top = iNPWindow->fLocationV;
		fFrame.right = iNPWindow->fLocationH + iNPWindow->fWidth;
		fFrame.bottom = iNPWindow->fLocationV + iNPWindow->fHeight;

		fAperture.left = iNPWindow->fClipRect.left - iNPWindow->fLocationH;
		fAperture.top = iNPWindow->fClipRect.top - iNPWindow->fLocationV;
		fAperture.right = iNPWindow->fClipRect.right - iNPWindow->fLocationH;
		fAperture.bottom = iNPWindow->fClipRect.bottom - iNPWindow->fLocationV;

		ZNSPlugin::NPPort* thePort = static_cast<ZNSPlugin::NPPort*>(iNPWindow->fPlatformWindow);
		fGrafPort = thePort->fPort;
		#endif

		#if ZCONFIG(OS, Win32)
		if (!fHWND)
			{
			HWND parentHWND = static_cast<HWND>(iNPWindow->fPlatformWindow);
			if (parentHWND)
				{
				HWND theHWND = nil;
				if (ZUtil_Win::sUseWAPI())
					{
					theHWND = ::CreateWindowExW(
								0, // Extended attributes
								sWindowClassNameW, // window class name
								nil, // window caption
								WS_CHILD | WS_VISIBLE, // window style
								0, // initial x position
								0, // initial y position
								iNPWindow->fWidth, // initial x size
								iNPWindow->fHeight, // initial y size
								parentHWND, // Parent window
								nil,
								sHINSTANCE, // program instance handle
								this); // creation parameters
					}
				else
					{
					theHWND = ::CreateWindowExA(
								0, // Extended attributes
								sWindowClassNameA, // window class name
								nil, // window caption
								WS_CHILD | WS_VISIBLE, // window style
								0, // initial x position
								0, // initial y position
								iNPWindow->fWidth, // initial x size
								iNPWindow->fHeight, // initial y size
								parentHWND, // Parent window
								nil,
								sHINSTANCE, // program instance handle
								this); // creation parameters
					}
				ZAssertStop(kDebug_FakeWindow_NSPlugin, theHWND && fHWND == theHWND);
				}
			}
		#endif
		}
	else
		{
		#if ZCONFIG(OS, MacOS7) || ZCONFIG(OS, Carbon)
		fGrafPort = nil;
		#endif

		#if ZCONFIG(OS, Win32)
		ZAssertStop(kDebug_FakeWindow_NSPlugin, fHWND == nil);
		#endif
		}
	return ZNSPlugin::eError_None;
	}

// =================================================================================================

#if ZCONFIG(OS, MacOS7) || ZCONFIG(OS, Carbon)
int16 ZNSPlugin_FakeWindow::HandleEvent(const EventRecord& iEvent)
	{
	if (!fGrafPort)
		return true;

//	CheckUpdates theCheckUpdates(this);
	bool handledIt = true;
	switch (iEvent.what)
		{
		case nullEvent:
			{
			ZMessage theMessage;
			theMessage.SetString("what", "zoolib:MouseChange");

			Point windowWhere = iEvent.where;
			::GlobalToLocal(&windowWhere);
			theMessage.SetPoint("where", ZPoint(windowWhere) + fFrame.TopLeft());
			theMessage.SetInt64("when", bigtime_t(iEvent.when) * 16667LL);
		
			theMessage.SetBool("contained", fAperture.Contains(windowWhere));
		
			theMessage.SetBool("buttonLeft", 0 == (iEvent.modifiers & btnState));
		
			theMessage.SetBool("command", iEvent.modifiers & cmdKey);
			theMessage.SetBool("option", iEvent.modifiers & optionKey);
			theMessage.SetBool("shift", iEvent.modifiers & shiftKey);
			theMessage.SetBool("control", iEvent.modifiers & controlKey);
			theMessage.SetBool("capsLock", iEvent.modifiers & alphaLock);
			this->WindowMessage_Mouse(theMessage);

			this->WindowIdle();
			break;
			}
		case updateEvt:
			{
			ZDC_NativeQD theDC(nil);
			theDC.SetPatternOrigin(-this->GetWindowOrigin());
			this->DoUpdate(theDC);
			theDC.Flush();
			break;
			}
		case mouseDown:
		case mouseUp:
			{
			ZMessage theMessage;
			if (iEvent.what == mouseDown)
				theMessage.SetString("what", "zoolib:MouseDown");
			else
				theMessage.SetString("what", "zoolib:MouseUp");

			Point windowWhere = iEvent.where;
			::GlobalToLocal(&windowWhere);

			theMessage.SetPoint("where", ZPoint(windowWhere) + fFrame.TopLeft());
			theMessage.SetInt64("when", bigtime_t(iEvent.when) * 16667LL);
		
			theMessage.SetBool("contained", fAperture.Contains(windowWhere));
		
			theMessage.SetBool("buttonLeft", iEvent.what == mouseDown);
		
			theMessage.SetBool("command", iEvent.modifiers & cmdKey);
			theMessage.SetBool("option", iEvent.modifiers & optionKey);
			theMessage.SetBool("shift", iEvent.modifiers & shiftKey);
			theMessage.SetBool("control", iEvent.modifiers & controlKey);
			theMessage.SetBool("capsLock", iEvent.modifiers & alphaLock);
			this->WindowMessage_Mouse(theMessage);
			break;
			}
		case autoKey:
		case keyDown:
			{
			ZMessage theMessage;
			theMessage.SetString("what", "zoolib:KeyDown");
			theMessage.SetInt32("charCode", iEvent.message & charCodeMask);
			theMessage.SetInt32("keyCode", (iEvent.message & keyCodeMask) >> 8);
			theMessage.SetBool("shift", iEvent.modifiers & shiftKey);
			theMessage.SetBool("command", iEvent.modifiers & cmdKey);
			theMessage.SetBool("option", iEvent.modifiers & optionKey);
			theMessage.SetBool("control", iEvent.modifiers & controlKey);
			theMessage.SetBool("capsLock", iEvent.modifiers & alphaLock);
			this->WindowMessage_Key(theMessage);
			break;
			}
		case keyUp:
			{
			break;
			}
		case activateEvt:
			{
			break;
			}
		default:
			{
			handledIt = false;
			}
		}
	return handledIt;
	}
#endif // ZCONFIG(OS, MacOS7) || ZCONFIG(OS, Carbon)

// =================================================================================================

#if ZCONFIG(OS, Win32)
LRESULT ZNSPlugin_FakeWindow::sWindowProcW(HWND iHWND, UINT iMessage, WPARAM iWPARAM, LPARAM iLPARAM)
	{
	ZNSPlugin_FakeWindow* theFakeWindow;
	if (iMessage == WM_NCCREATE)
		{
		CREATESTRUCTW* theCREATESTRUCT = reinterpret_cast<CREATESTRUCTW*>(iLPARAM);
		theFakeWindow = reinterpret_cast<ZNSPlugin_FakeWindow*>(theCREATESTRUCT->lpCreateParams);
		theFakeWindow->fHWND = iHWND;
		// And set the user data
		::SetWindowLongW(iHWND, GWL_USERDATA, reinterpret_cast<LONG>(theFakeWindow));
		}
	else
		{
		theFakeWindow = reinterpret_cast<ZNSPlugin_FakeWindow*>(::GetWindowLongW(iHWND, GWL_USERDATA));
		}

	ZAssertStop(kDebug_FakeWindow_NSPlugin, theFakeWindow);
	return theFakeWindow->WindowProc(iHWND, iMessage, iWPARAM, iLPARAM, true);
	}

LRESULT ZNSPlugin_FakeWindow::sWindowProcA(HWND iHWND, UINT iMessage, WPARAM iWPARAM, LPARAM iLPARAM)
	{
	ZNSPlugin_FakeWindow* theFakeWindow;
	if (iMessage == WM_NCCREATE)
		{
		CREATESTRUCTA* theCREATESTRUCT = reinterpret_cast<CREATESTRUCTA*>(iLPARAM);
		theFakeWindow = reinterpret_cast<ZNSPlugin_FakeWindow*>(theCREATESTRUCT->lpCreateParams);
		theFakeWindow->fHWND = iHWND;
		// And set the user data
		::SetWindowLongA(iHWND, GWL_USERDATA, reinterpret_cast<LONG>(theFakeWindow));
		}
	else
		{
		theFakeWindow = reinterpret_cast<ZNSPlugin_FakeWindow*>(::GetWindowLongA(iHWND, GWL_USERDATA));
		}

	ZAssertStop(kDebug_FakeWindow_NSPlugin, theFakeWindow);
	return theFakeWindow->WindowProc(iHWND, iMessage, iWPARAM, iLPARAM, false);
	}

LRESULT ZNSPlugin_FakeWindow::WindowProc(HWND iHWND, UINT iMessage, WPARAM iWPARAM, LPARAM iLPARAM, bool iIsNT)
	{
	switch (iMessage)
		{
		case WM_NCDESTROY:
			{
			fHWND = nil;
			break;
			}
		case WM_KEYDOWN:
			{
			BYTE keyboardState[256];
			::GetKeyboardState(keyboardState);
			WORD translatedChar;
			int result = ::ToAscii(iWPARAM, HIWORD(iLPARAM), keyboardState, &translatedChar, 0);
			ZKeyboard::CharCode ourCharCode = ZKeyboard::ccUnknown;
			if (result == 0)
				{
				switch (iWPARAM)
					{
					case VK_CLEAR: ourCharCode = ZKeyboard::ccClear; break;
					case VK_ESCAPE: ourCharCode = ZKeyboard::ccEscape; break;
					case VK_PRIOR: ourCharCode = ZKeyboard::ccPageUp; break;
					case VK_NEXT: ourCharCode = ZKeyboard::ccPageDown; break;
					case VK_END: ourCharCode = ZKeyboard::ccEnd; break;
					case VK_HOME: ourCharCode = ZKeyboard::ccHome; break;
					case VK_LEFT: ourCharCode = ZKeyboard::ccLeft; break;
					case VK_UP: ourCharCode = ZKeyboard::ccUp; break;
					case VK_RIGHT: ourCharCode = ZKeyboard::ccRight; break;
					case VK_DOWN: ourCharCode = ZKeyboard::ccDown; break;
					case VK_DELETE: ourCharCode = ZKeyboard::ccFwdDelete; break;
					case VK_HELP: ourCharCode = ZKeyboard::ccHelp; break;
					}
				}
			else if (result == 1)
				{
				ourCharCode = translatedChar;
				}

			if (ourCharCode != ZKeyboard::ccUnknown)
				{
				ZMessage theMessage;
				theMessage.SetString("what", "zoolib:KeyDown");
				theMessage.SetInt64("when", bigtime_t(::GetMessageTime()) * 1000);
				theMessage.SetInt32("charCode", ourCharCode);
				// theMessage.SetInt32("keyCode", (inEvent.message & keyCodeMask) >> 8);
				theMessage.SetBool("shift", ::GetKeyState(VK_SHIFT) & 0x8000);
				// theMessage.SetBool("command", (inEvent.modifiers & cmdKey) == cmdKey);
				// theMessage.SetBool("option", (inEvent.modifiers & optionKey) == optionKey);
				theMessage.SetBool("control", ::GetKeyState(VK_CONTROL) & 0x8000);
				theMessage.SetBool("capsLock", ::GetKeyState(VK_CAPITAL) & 0x8000);
				this->WindowMessage_Key(theMessage);
				return 0;
				}
			break;
			}
		case WM_LBUTTONDOWN:
		case WM_MBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_MBUTTONUP:
		case WM_RBUTTONUP:
		case WM_MOUSEMOVE:
			{
			ZMessage theMessage;

			int buttonDownIndex = -1;
			if ((iMessage == WM_LBUTTONDOWN) && (0 == (iWPARAM & (MK_MBUTTON | MK_RBUTTON))))
				buttonDownIndex = 0;
			else if ((iMessage == WM_MBUTTONDOWN) && (0 == (iWPARAM & (MK_LBUTTON | MK_RBUTTON))))
				buttonDownIndex = 1;
			else if ((iMessage == WM_RBUTTONDOWN) && (0 == (iWPARAM & (MK_LBUTTON | MK_MBUTTON))))
				buttonDownIndex = 2;

			if (buttonDownIndex != -1)
				{
				theMessage.SetString("what", "zoolib:MouseDown");
				}
			else if ((iMessage == WM_LBUTTONUP || iMessage == WM_MBUTTONUP || iMessage == WM_RBUTTONUP)
					&& (0 == (iWPARAM & (MK_LBUTTON | MK_MBUTTON | MK_RBUTTON))))
				{
				theMessage.SetString("what", "zoolib:MouseUp");
				}
			else
				{
				theMessage.SetString("what", "zoolib:MouseChange");
				}

			ZPoint where(short(LOWORD(iLPARAM)), short(HIWORD(iLPARAM)));
			theMessage.SetPoint("where", where);
			theMessage.SetInt64("when", bigtime_t(::GetMessageTime()) * 1000);
		
			RECT clientRECT;
			::GetClientRect(fHWND, &clientRECT);
			ZRect clientRect(clientRECT);
			theMessage.SetBool("contained", clientRect.Contains(where));
		
			theMessage.SetBool("buttonLeft", (iWPARAM & MK_LBUTTON) != 0);
			theMessage.SetBool("buttonMiddle", (iWPARAM & MK_MBUTTON) != 0);
			theMessage.SetBool("buttonRight", (iWPARAM & MK_RBUTTON) != 0);
		
			theMessage.SetBool("command", false);
			theMessage.SetBool("option", false);
			theMessage.SetBool("shift", (iWPARAM & MK_SHIFT) != 0);
			theMessage.SetBool("control", (iWPARAM & MK_CONTROL) != 0);
			theMessage.SetBool("capsLock", ::GetKeyState(VK_CAPITAL) & 0x8000);

			this->WindowMessage_Mouse(theMessage);
			return 0;
			}
		case WM_PAINT:
			{
			PAINTSTRUCT thePS;
			HDC paintHDC = ::BeginPaint(fHWND, &thePS);
			ZDC_NativeGDI theDC(paintHDC);
			this->DoUpdate(theDC);
			theDC.Flush();
			return 0;
			}
		}

	if (iIsNT)
		return DefWindowProcW(iHWND, iMessage, iWPARAM, iLPARAM);
	else
		return DefWindowProcA(iHWND, iMessage, iWPARAM, iLPARAM);
	}

#endif // ZCONFIG(OS, Win32)

// =================================================================================================

ZMutexBase& ZNSPlugin_FakeWindow::GetWindowLock()
	{ return fMutex_Structure; }

ZPoint ZNSPlugin_FakeWindow::GetWindowOrigin()
	{
#if ZCONFIG(OS, MacOS7) || ZCONFIG(OS, Carbon)
	return fFrame.TopLeft();
#endif

#if ZCONFIG(OS, Win32)
	return ZPoint::sZero;
#endif
	}

ZPoint ZNSPlugin_FakeWindow::GetWindowSize()
	{
#if ZCONFIG(OS, MacOS7) || ZCONFIG(OS, Carbon)
	return fFrame.Size();
#endif

#if ZCONFIG(OS, Win32)
	if (fHWND)
		{
		RECT clientRECT;
		::GetClientRect(fHWND, &clientRECT);
		return ZPoint(clientRECT.right - clientRECT.left, clientRECT.bottom - clientRECT.top); 
		}
	return ZPoint(100, 100);
#endif
	}

ZDCRgn ZNSPlugin_FakeWindow::GetWindowVisibleContentRgn()
	{
#if ZCONFIG(OS, MacOS7) || ZCONFIG(OS, Carbon)
	return ZRect(fAperture + fFrame.TopLeft());
#endif

#if ZCONFIG(OS, Win32)
	if (fHWND)
		{
		RECT clientRECT;
		::GetClientRect(fHWND, &clientRECT);
		return ZRect(clientRECT);
		}
	return ZRect(100, 100);
#endif
	}

void ZNSPlugin_FakeWindow::InvalidateWindowRegion(const ZDCRgn& iWindowRegion)
	{
#if ZCONFIG(OS, MacOS7)
	ZDCRgn mutableRgn = iWindowRegion - fFrame.TopLeft();

	if (fGrafPort)
		{
		::SetGWorld(fGrafPort, nil);
		sNavigator_InvalidateRegion(fNPP, mutableRgn);
		}

#elif ZCONFIG(OS, Carbon)

	ZDCRgn mutableRgn = iWindowRegion - fFrame.TopLeft();

	if (fGrafPort)
		InvalWindowRgn(::GetWindowFromPort(fGrafPort), mutableRgn.GetRgnHandle());

#elif ZCONFIG(OS, Win32)
	if (fHWND)
		{
		ZDCRgn mutableRgn = iWindowRegion;
		::InvalidateRgn(fHWND, mutableRgn.GetHRGN(), true);
		}
#endif
	}

ZPoint ZNSPlugin_FakeWindow::WindowToGlobal(const ZPoint& iWindowPoint)
	{
	return ZPoint(0,0);
	}

void ZNSPlugin_FakeWindow::UpdateWindowNow()
	{
	}

ZRef<ZDCCanvas> ZNSPlugin_FakeWindow::GetWindowCanvas()
	{
#if ZCONFIG(OS, MacOS7) || ZCONFIG(OS, Carbon)

	if (fGrafPort)
		return ZDCCanvas_QD::sFindCanvasOrCreateNative(fGrafPort);

#elif ZCONFIG(OS, Win32)
//	if (fHWND)
//		return 
#endif

	return ZDCScratch::sGet().GetCanvas();
	}

void ZNSPlugin_FakeWindow::GetWindowNative(void* oNativeWindow)
	{
	}

void ZNSPlugin_FakeWindow::InvalidateWindowCursor()
	{
	}
