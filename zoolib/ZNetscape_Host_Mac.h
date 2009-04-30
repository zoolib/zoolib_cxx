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

#ifndef __ZNetscape_Host_Mac__
#define __ZNetscape_Host_Mac__ 1
#include "zconfig.h"

#include "zoolib/ZNetscape_Host_Std.h"

#if defined(XP_MAC) || defined(XP_MACOSX)

NAMESPACE_ZOOLIB_BEGIN

namespace ZNetscape {

// =================================================================================================
#pragma mark -
#pragma mark * Host_Mac

class Host_Mac : public Host_Std
	{
public:
	Host_Mac(ZRef<GuestFactory> iGuestFactory);
	virtual ~Host_Mac();

// From Host via Host_Std
	virtual NPError Host_GetValue(NPP npp, NPNVariable variable, void* ret_value);
	virtual NPError Host_SetValue(NPP npp, NPPVariable variable, void* value);

// Our protocol
	void DoActivate(bool iActivate);
	void DoFocus(bool iFocused);
	void DoIdle();
	void DoDraw(WindowRef iWindowRef);

	void DoEvent(const EventRecord& iEvent);

// Our protocol
	void UpdateWindowRef(WindowRef iWindowRef);

protected:
	#if defined(XP_MACOSX)
		NP_CGContext fNP_CGContext;
		bool fUseCoreGraphics;
	#endif

	NP_Port fNP_Port;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Host_Mac_EventLoop

class Host_Mac_EventLoop : public Host_Mac
	{
public:
	Host_Mac_EventLoop(ZooLib::ZRef<ZooLib::ZNetscape::GuestFactory> iGF);
	virtual ~Host_Mac_EventLoop();

protected:
	bool pDeliverEvent(EventRef iEventRef);

private:
	static EventLoopTimerUPP sEventLoopTimerUPP_Idle;
	static pascal void sEventLoopTimer_Idle(EventLoopTimerRef iTimer, void* iRefcon);
	void EventLoopTimer_Idle(EventLoopTimerRef iTimer);

    EventLoopTimerRef fEventLoopTimerRef_Idle;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Host_WindowRef

class Host_WindowRef : public Host_Mac_EventLoop
	{
public:
	Host_WindowRef(ZooLib::ZRef<ZooLib::ZNetscape::GuestFactory> iGF, WindowRef iWindowRef);
	virtual ~Host_WindowRef();

// From Host_Std
	virtual void Host_InvalidateRect(NPP npp, NPRect* rect);
	virtual void PostCreateAndLoad();

private:
	static EventHandlerUPP sEventHandlerUPP_Window;
	static pascal OSStatus sEventHandler_Window(
		EventHandlerCallRef iCallRef, EventRef iEventRef, void* iRefcon);
	OSStatus EventHandler_Window(EventHandlerCallRef iCallRef, EventRef iEventRef);

	WindowRef fWindowRef;

	EventTargetRef fEventTargetRef_Window;
	EventHandlerRef fEventHandlerRef_Window;
	};

} // namespace ZNetscape

NAMESPACE_ZOOLIB_END

#endif // defined(XP_MAC) || defined(XP_MACOSX)

#endif // __ZNetscape_Host_Mac__
