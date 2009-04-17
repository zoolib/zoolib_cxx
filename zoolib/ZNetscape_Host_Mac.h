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

// Our protocol
	void DoActivate(bool iActivate);
	void DoFocus(bool iFocused);
	void DoIdle();
	void DoDraw();

	void DoEvent(const EventRecord& iEvent);

	void SetPortAndBounds(CGrafPtr iGrafPtr,
		ZPoint iLocation, ZPoint iSize, const ZRect& iClip);

	void SetBounds(
		ZPoint iLocation, ZPoint iSize, const ZRect& iClip);

protected:
	#if defined(XP_MACOSX)
		NP_CGContext fNP_CGContext;
	#endif

	NP_Port fNP_Port;
	};

} // namespace ZNetscape

NAMESPACE_ZOOLIB_END

#endif // defined(XP_MAC) || defined(XP_MACOSX)

#endif // __ZNetscape_Host_Mac__
