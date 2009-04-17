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

#include "zoolib/ZMemory.h"

NAMESPACE_ZOOLIB_BEGIN

namespace ZNetscape {

// =================================================================================================
#pragma mark -
#pragma mark * Host_Mac

Host_Mac::Host_Mac(ZRef<GuestFactory> iGuestFactory)
:	Host_Std(iGuestFactory)
	{
	ZBlockZero(&fNP_Port, sizeof(fNP_Port));
	}

Host_Mac::~Host_Mac()
	{
	}

static void sStuffEventRecord(EventRecord& oEventRecord)
	{
	oEventRecord.what = nullEvent;
	oEventRecord.message = 0;
	oEventRecord.when = ::TickCount();
	::GetGlobalMouse(&oEventRecord.where);
	oEventRecord.modifiers = 0;
	}

void Host_Mac::DoActivate(bool iActivate)
	{
	if (fNP_Port.port)
		{
		EventRecord theER;
		sStuffEventRecord(theER);

		theER.what = activateEvt;
		theER.message = (UInt32)::GetWindowFromPort(fNP_Port.port);
		if (iActivate)
			theER.modifiers = activeFlag;
		else
			theER.modifiers = 0;
		
		this->DoEvent(theER);
		}
	}

void Host_Mac::DoFocus(bool iFocused)
	{
	EventRecord theER;
	sStuffEventRecord(theER);

	if (iFocused)
		theER.what = NPEventType_GetFocusEvent;
	else
		theER.what = NPEventType_LoseFocusEvent;

	this->DoEvent(theER);
	}

void Host_Mac::DoIdle()
	{
	EventRecord theER;
	sStuffEventRecord(theER);

	this->DoEvent(theER);
	}

void Host_Mac::DoDraw()
	{
	if (fNP_Port.port)
		{
		EventRecord theER;
		sStuffEventRecord(theER);

		theER.what = updateEvt;
		theER.message = (UInt32)::GetWindowFromPort(fNP_Port.port);

		this->DoEvent(theER);
		}
	}

void Host_Mac::DoEvent(const EventRecord& iEvent)
	{
	// Hmm -- do we need to use the local?
	EventRecord localEvent = iEvent;
	this->Guest_HandleEvent(&localEvent);
	}

void Host_Mac::SetPortAndBounds(CGrafPtr iGrafPtr,
	ZPoint iLocation, ZPoint iSize, const ZRect& iClip)
	{
	fNP_Port.port = iGrafPtr;
	this->SetBounds(iLocation, iSize, iClip);
	}

void Host_Mac::SetBounds(
	ZPoint iLocation, ZPoint iSize, const ZRect& iClip)
	{
	fNP_Port.portx = -iLocation.h;
	fNP_Port.porty = -iLocation.v;
	fNPWindow.window = &fNP_Port;

	fNPWindow.type = NPWindowTypeDrawable;

	fNPWindow.x = iLocation.h;
	fNPWindow.y = iLocation.v;
	fNPWindow.width = iSize.h;
	fNPWindow.height = iSize.v;

	fNPWindow.clipRect.left = iClip.left;
	fNPWindow.clipRect.top = iClip.top;
	fNPWindow.clipRect.right = iClip.right;
	fNPWindow.clipRect.bottom = iClip.bottom;

	this->Guest_SetWindow(&fNPWindow);
	}

} // namespace ZNetscape

NAMESPACE_ZOOLIB_END

#endif // defined(XP_MAC) || defined(XP_MACOSX)
