/* -------------------------------------------------------------------------------------------------
Copyright (c) 2010 Andrew Green
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

#ifndef __ZNetscape_Host_Cocoa_h__
#define __ZNetscape_Host_Cocoa_h__ 1
#include "zconfig.h"

#include "zoolib/ZCONFIG_SPI.h"

#if defined(__OBJC__)
#if ZCONFIG_SPI_Enabled(Cocoa)

#include "zoolib/ZCallable.h"
#include "zoolib/netscape/ZNetscape_Host_Std.h"

#include <AppKit/NSView.h>

namespace ZooLib {
namespace ZNetscape {
class Host_Cocoa;
} // namespace ZNetscape
} // namespace ZooLib

// =================================================================================================
// MARK: - NSView_NetscapeHost

@interface NSView_NetscapeHost : NSView
	{
@public
	ZooLib::ZNetscape::Host_Cocoa* fHost;
	}

@end

// =================================================================================================
// MARK: - Host_Cocoa

namespace ZooLib {
namespace ZNetscape {

class Host_Cocoa : public Host_Std
	{
public:
	Host_Cocoa(ZRef<GuestFactory> iGuestFactory, NSView_NetscapeHost* iView);
	virtual ~Host_Cocoa();

// From Host via Host_Std
	virtual NPError Host_GetValue(NPP npp, NPNVariable variable, void* ret_value);
	virtual NPError Host_SetValue(NPP npp, NPPVariable variable, void* value);

	virtual void Host_InvalidateRect(NPP npp, NPRect* rect);
	virtual void PostCreateAndLoad();

	void Draw(const NSRect& iRect);
	void FlagsChanged(NSEvent* iEvent);
	void SendText(NSEvent* iEvent);
	bool SendKeyEvent(NSEvent* iEvent, NPCocoaEventType iType);
	bool SendMouseEvent(NSEvent* iEvent, NPCocoaEventType iType);
	void FocusChanged(bool hasFocus);
	void WindowFocusChanged(bool hasFocus);
	void Timer();
	void FrameChanged();

protected:
	void pDoSetWindow();
	NSView_NetscapeHost* fView;

	ZRef<ZCallable_Void> fCallable_Timer;

	NPWindow fNPWindow;

	NP_CGContext fNP_CGContext;
	NP_CGContext fNP_CGContext_Prior;

	float fLeft;
	float fTop;
	float fRight;
	float fBottom;
	};

} // namespace ZNetscape
} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(Cocoa)
#endif // defined(__OBJC__)
#endif // __ZNetscape_Host_Cocoa_h__
