#ifndef __ZNSPlugin_FakeWindow__
#define __ZNSPlugin_FakeWindow__ 1
#include "zconfig.h"

#include "ZNSPlugin.h"
#include "ZFakeWindow.h"

// =================================================================================================
#pragma mark -
#pragma mark * ZNSPluginMeister_FakeWindow

class ZNSPluginMeister_FakeWindow : public ZNSPluginMeister
	{
public:
	ZNSPluginMeister_FakeWindow();
	virtual ~ZNSPluginMeister_FakeWindow();
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZNSPlugin_FakeWindow

class ZNSPlugin_FakeWindow : public ZNSPluginInstance,
							public ZFakeWindow
	{
public:
	ZNSPlugin_FakeWindow(ZNSPlugin::NPP iNPP);
	virtual ~ZNSPlugin_FakeWindow();

// From ZNSPlugin
	virtual ZNSPlugin::EError SetWindow(ZNSPlugin::NPWindow* iNPWindow);

#if ZCONFIG(OS, MacOS7) || ZCONFIG(OS, Carbon)
	virtual int16 HandleEvent(const EventRecord& iEvent);
#endif // ZCONFIG(OS, MacOS7) || ZCONFIG(OS, Carbon)

#if ZCONFIG(OS, Win32)
	static LRESULT CALLBACK sWindowProcW(HWND inHWND, UINT inMessage, WPARAM inWPARAM, LPARAM inLPARAM);
	static LRESULT CALLBACK sWindowProcA(HWND iHWND, UINT iMessage, WPARAM iWPARAM, LPARAM iLPARAM);

	virtual LRESULT WindowProc(HWND iHWND, UINT iMessage, WPARAM iWPARAM, LPARAM iLPARAM, bool iIsNT);
#endif // ZCONFIG(OS, Win32)


// From ZFakeWindow
	virtual ZMutexBase& GetWindowLock();
	virtual ZPoint GetWindowOrigin();
	virtual ZPoint GetWindowSize();
	virtual ZDCRgn GetWindowVisibleContentRgn();
	virtual void InvalidateWindowRegion(const ZDCRgn& iWindowRegion);
	virtual ZPoint WindowToGlobal(const ZPoint& iWindowPoint);
	virtual void UpdateWindowNow();
	virtual ZRef<ZDCCanvas> GetWindowCanvas();
	virtual void GetWindowNative(void* oNativeWindow);
	virtual void InvalidateWindowCursor();

protected:
	ZMutex fMutex_Structure;

#if ZCONFIG(OS, MacOS7) || ZCONFIG(OS, Carbon)
	ZRect fFrame;
	ZRect fAperture;

	CGrafPtr fGrafPort;
#endif

#if ZCONFIG(OS, Win32)
	HWND fHWND;
#endif

	int fEntryCount;
	ZDCRgn fInvalidRgn;
	};

#endif // __ZNSPlugin_FakeWindow__

