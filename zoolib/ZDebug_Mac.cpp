/* -------------------------------------------------------------------------------------------------
Copyright (c) 2000 Andrew Green and Learning in Motion, Inc.
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

#include "ZDebug_Mac.h"

// =================================================================================================

#if ZCONFIG(OS, MacOS7) || ZCONFIG(OS, Carbon) // || ZCONFIG(OS, MacOSX)

#if !__MACH__
#	include <MetroNubUtils.h> // For AmIBeingMWDebugged
#endif

#if ZCONFIG(OS, MacOSX)
#	include <HIToolbox/Dialogs.h>
#	include <CarbonCore/Gestalt.h>
#	include <HIServices/Processes.h>
#else
#	include <Dialogs.h> // For StandardAlert
#	include <Gestalt.h>
#	include <Processes.h> // For ExitToShell
#endif

#include <cstdarg>
#include <cstdio>

#include "ZThreadTM.h" // For ZThreadTM_SetCurrentSolo

bool sDebug_InvokeLowLevelDebugger = false;

// ==================================================
// Forward declarations of helper functions
static bool sIsLowLevelDebuggerPresent();
static bool sAmIBeingMWDebugged();
static void sDoOther(const unsigned char* inMessage);

static void ZDebug_HandleActual_Mac(int inLevel, ZDebug_Action inAction, const char* inMessage)
	{
	Str255 thePString;
	const char* src = inMessage;

	unsigned char* dest = &thePString[1];
	unsigned char* destEnd = &thePString[256];

	while (*src && dest < destEnd)
		*dest++ = (unsigned char)(*src++);

	if (inAction == eDebug_ActionContinue)
		{
		*dest++ = ';';
		*dest++ = 'g';
		}

	thePString[0] = dest - &thePString[1];

	// Now, what to do? There are several variables to take account of. Do we have MacsBug installed? Is the source
	// debugger running? Do we want the app to be stopped? Is sDebug_InvokeLowLevelDebugger set? I put together a
	// decision table, which is in my (AG) notes dated 98-11-10.
	bool isMacsBugInstalled = ::sIsLowLevelDebuggerPresent();
	bool isMWDebugInstalled = ::sAmIBeingMWDebugged();
	bool stop = (inAction == eDebug_ActionStop);
	if (!isMacsBugInstalled && !isMWDebugInstalled && !sDebug_InvokeLowLevelDebugger && stop)
		{
		// Case: Dialog. Do a dialog box with the message, then exit to shell. (1 out of 16)
		// sDoOther will invoke ExitToShell
		::sDoOther(thePString);
		}
	else if (isMacsBugInstalled && isMWDebugInstalled && !sDebug_InvokeLowLevelDebugger)
		{
		// Case: SysBreak (display in the source level debugger) (2 out of 16)
		::DebugStr(thePString);
		}
	else if (!isMacsBugInstalled && sDebug_InvokeLowLevelDebugger && stop)
		{
		// Case: Houston, we have a problem. We have to stop execution, but we're running at a time when only the low level
		// debugger (MacsBug) would be safe. Just invoke the debugger, and cross our fingers.
		::Debugger();
		}
	else if (!isMacsBugInstalled && !stop && (sDebug_InvokeLowLevelDebugger || (!sDebug_InvokeLowLevelDebugger && !isMWDebugInstalled)))
		{
		// Case: Nothing. We don't have to do anything, the user does not have software installed that could display
		// a message, plus we weren't asked to stop, so just return. (3 out of 16)
		::DebugStr(thePString);	//## added this to get logging to work under OS X. -ec 02.02.15
		}
	else
		{
		// Case: DebugStr. This is every other case (8 out of the 16).
		if (!sDebug_InvokeLowLevelDebugger)
			::DebugStr(thePString);
		}
	}

ZDebug_HandleActual_t sDebug_HandleActual = ZDebug_HandleActual_Mac;

// ==================================================

static void sDoOther(const unsigned char* inMessage)
	{
	bool wasSoloed = ZThreadTM_SetCurrentSolo(true);

	bool gotAppearance10 = false;
	long response;
	if (::Gestalt(gestaltAppearanceAttr, &response) == noErr)
		{
		if (::Gestalt(gestaltAppearanceVersion, &response) == noErr)
			gotAppearance10 = response >= 0x0100;
		}

	if (gotAppearance10)
		{
		AlertStdAlertParamRec theRec;
		theRec.movable = false;
		theRec.helpButton = false;
		theRec.filterProc = nil;
		theRec.defaultText = "\pQuit";
		theRec.cancelText = nil;
		theRec.otherText = nil;
		theRec.defaultButton = kAlertStdAlertOKButton;
		theRec.cancelButton = 0;
		theRec.position = kWindowDefaultPosition;
		SInt16 hitItem;
		::StandardAlert(kAlertStopAlert, const_cast<unsigned char*>(inMessage), nil, &theRec, &hitItem);
		}
	::ExitToShell();
	}

// ==================================================
// sIsLowLevelDebuggerPresent was taken from Whisper 1.3, which was adapted from code
// in PowerPlant 3's UDebugUtils.cp which was adapted from an Apple snippet at:
// <http://devworld.apple.com/dev/techsupport/source/code/Snippets/Testing_and_Debugging/DebuggerPresence/debugger.c.html>

static bool sIsLowLevelDebuggerPresent()
	{
#if ZCONFIG(OS, MacOS7)
// MacsBug internals
	const char* MacJmpFlag = (char*)0xBFF;
	const int kDebuggerInstalled = 5;

	if (*MacJmpFlag & (1 << kDebuggerInstalled)) // check for MacsBug and TMON
		return true;
	else if (*(int16*)(*(int32*)0x38) == 0x487A) // check for The Debugger
		return true;
#endif // ZCONFIG(OS, MacOS7)

	return false;
	}

static bool sAmIBeingMWDebugged()
	{
#if __MACH__
	return false;
#else
	return ::AmIBeingMWDebugged();
#endif
	}

#endif // ZCONFIG(OS, MacOS7) || ZCONFIG(OS, Carbon) || ZCONFIG(OS, MacOSX)

// =================================================================================================
