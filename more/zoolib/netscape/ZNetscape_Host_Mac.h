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

#ifndef __ZNetscape_Host_Mac_h__
#define __ZNetscape_Host_Mac_h__ 1
#include "zconfig.h"

#if ! ZCONFIG_Is64Bit
 
#include "zoolib/netscape/ZNetscape_Host_Std.h"

namespace ZooLib {
namespace ZNetscape {

// =================================================================================================
// MARK: - Host_Mac

#if defined(XP_MAC) || defined(XP_MACOSX)

class Host_Mac : public Host_Std
	{
public:
	Host_Mac(ZRef<GuestFactory> iGuestFactory, bool iAllowCG);
	virtual ~Host_Mac();

// From Host via Host_Std
	virtual NPError Host_GetValue(NPP npp, NPNVariable variable, void* ret_value);
	virtual NPError Host_SetValue(NPP npp, NPPVariable variable, void* value);

// Our protocol
	void DoActivate(bool iActivate);
	void DoFocus(bool iFocused);
	void DoIdle();

	void DoEvent(EventKind iWhat, uint32 iMessage);

	virtual void DoEvent(const EventRecord& iEvent);

	void DoSetWindow(const Rect& iWinFrame);
	void DoSetWindow(int iX, int iY, int iWidth, int iHeight);

	template <class Rect_p>
	Rect_p pApplyInsets(const Rect_p& iRect);

protected:
	bool pDeliverEvent(EventRef iEventRef);

	static EventLoopTimerUPP sEventLoopTimerUPP_Idle;
	static pascal void sEventLoopTimer_Idle(EventLoopTimerRef iTimer, void* iRefcon);
	void EventLoopTimer_Idle(EventLoopTimerRef iTimer);

	bool fAllowCG;
	EventLoopTimerRef fEventLoopTimerRef_Idle;

	#if defined(XP_MACOSX)
		NP_CGContext fNP_CGContext;
		NP_CGContext fNP_CGContext_Prior;
		bool fUseCoreGraphics;
	#endif

	#if !defined(NP_NO_QUICKDRAW)
		NP_Port fNP_Port;
	#endif

	float fLeft;
	float fTop;
	float fRight;
	float fBottom;

	NPWindow fNPWindow_Prior;
	};

#endif // defined(XP_MAC) || defined(XP_MACOSX)

// =================================================================================================
// MARK: - Host_WindowRef

#if defined(XP_MAC) || defined(XP_MACOSX)

class Host_WindowRef : public Host_Mac
	{
public:
	Host_WindowRef(
		ZRef<GuestFactory> iGF, bool iAllowCG, WindowRef iWindowRef);
	virtual ~Host_WindowRef();

// From Host_Std
	virtual void Host_InvalidateRect(NPP npp, NPRect* rect);
	virtual void PostCreateAndLoad();

// From Host_Mac
	using Host_Mac::DoEvent;
	virtual void DoEvent(const EventRecord& iEvent);

protected:
	WindowRef fWindowRef;

private:
	static EventHandlerUPP sEventHandlerUPP_Window;
	static pascal OSStatus sEventHandler_Window(
		EventHandlerCallRef iCallRef, EventRef iEventRef, void* iRefcon);
	OSStatus EventHandler_Window(EventHandlerCallRef iCallRef, EventRef iEventRef);

	EventTargetRef fEventTargetRef_Window;
	EventHandlerRef fEventHandlerRef_Window;
	};

#endif // defined(XP_MAC) || defined(XP_MACOSX)

// =================================================================================================
// MARK: - Host_HIViewRef

#if defined(XP_MACOSX)

class Host_HIViewRef : public Host_Mac
	{
public:
	Host_HIViewRef(
		ZRef<GuestFactory> iGF, bool iAllowCG, HIViewRef iHIViewRef);
	virtual ~Host_HIViewRef();

// From Host_Std
	virtual void Host_InvalidateRect(NPP npp, NPRect* rect);
	virtual void PostCreateAndLoad();

protected:
	HIViewRef fHIViewRef;

private:
	static EventHandlerUPP sEventHandlerUPP_View;
	static pascal OSStatus sEventHandler_View(
		EventHandlerCallRef iCallRef, EventRef iEventRef, void* iRefcon);
	OSStatus EventHandler_View(EventHandlerCallRef iCallRef, EventRef iEventRef);

	EventTargetRef fEventTargetRef_View;
	EventHandlerRef fEventHandlerRef_View;
	};

#endif // defined(XP_MACOSX)

} // namespace ZNetscape
} // namespace ZooLib

#endif // ! ZCONFIG_Is64Bit

#endif // __ZNetscape_Host_Mac_h__
