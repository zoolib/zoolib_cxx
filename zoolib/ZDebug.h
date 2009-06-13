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

#ifndef __ZDebug__
#define __ZDebug__ 1
#include "zconfig.h"

enum ZDebug_Action { eDebug_ActionContinue, eDebug_ActionStop };

typedef void (*ZDebug_HandleInitial_t)(int inLevel, ZDebug_Action inAction, const char* inFilename,
	int inLine, const char* inAssertionMessage, const char* inUserMessage);

typedef void (*ZDebug_HandleActual_t)(int inLevel, ZDebug_Action inAction, const char* inMessage);

extern ZDebug_HandleInitial_t sDebug_HandleInitial;
extern ZDebug_HandleActual_t sDebug_HandleActual;

void ZDebug_DisplayMessageSimple(const char* message, ...);

const char* ZDebug_FormatMessage(const char* iMessage, ...);

inline const char* ZDebug_FormatMessage() { return nullptr; }

// The actual macros that get used in source code. The funkiness of these macros (should)
// generate minimal inline code, so that an assertion that's active at level 2 will
// generate *no code* at level 1 or 0. This does rely on the compiler optimizing away
// statements/expressions with no effect.
#define ZDebugPrintf(a, b) ((a)<=ZCONFIG_Debug ? ZDebug_DisplayMessageSimple b : ((void)0))

#define ZDebugLogf(a, b) \
	((a) <= ZCONFIG_Debug && sDebug_HandleInitial ? \
	sDebug_HandleInitial(a, eDebug_ActionContinue, __FILE__, __LINE__, 0, ZDebug_FormatMessage b) \
	: ((void)0))

#define ZDebugStopf(a, b) \
	((a) <= ZCONFIG_Debug && sDebug_HandleInitial ? \
	sDebug_HandleInitial(a, eDebug_ActionStop, __FILE__, __LINE__, 0, ZDebug_FormatMessage b) \
	: ((void)0))

#define ZDebugLog(a) \
	((a) <= ZCONFIG_Debug && sDebug_HandleInitial ? \
	sDebug_HandleInitial(a, eDebug_ActionContinue, __FILE__, __LINE__, 0, 0) : ((void)0))

#define ZDebugStop(a) \
	((a)<=ZCONFIG_Debug && sDebug_HandleInitial ? \
	sDebug_HandleInitial(a, eDebug_ActionStop, __FILE__, __LINE__, 0, 0): ((void)0))

#define ZAssertLogf(a, b, c) \
	((a) <= ZCONFIG_Debug && sDebug_HandleInitial && !(b) ? \
	sDebug_HandleInitial(a, eDebug_ActionContinue, __FILE__, __LINE__, #b, \
	ZDebug_FormatMessage c), false : true)

#define ZAssertStopf(a, b, c) \
	((a) <= ZCONFIG_Debug && sDebug_HandleInitial && !(b) ? \
	sDebug_HandleInitial(a, eDebug_ActionStop, __FILE__, __LINE__, #b, \
	ZDebug_FormatMessage c), false : true)

#define ZAssertLog(a, b) \
	((a) <= ZCONFIG_Debug && sDebug_HandleInitial && !(b) ? \
	sDebug_HandleInitial(a, eDebug_ActionContinue, __FILE__, __LINE__, #b, nullptr), false : true)

#define ZAssertStop(a, b) \
	((a)<=ZCONFIG_Debug && sDebug_HandleInitial && !(b) ? \
	sDebug_HandleInitial(a, eDebug_ActionStop, __FILE__, __LINE__, #b, nullptr), false : true)


// ZAssertCompile can be used to enforce a constraint at compile time, (for example that a
// struct obeys necessary alignment rules). It either drops out completely or generates an
// error, depending on whether the expression evaulates true or false.
template <bool> struct ZCompileTimeAssertion {};
template<> struct ZCompileTimeAssertion<true> { typedef bool IsValid; };

#define ZAssertCompile(a) typedef ZCompileTimeAssertion<(a)>::IsValid ZCompileTimeAssertionValid

// I'd like to formalize ZUnimplemented a little more sometime. Perhaps it should
// throw an exception in production code.
#define ZUnimplemented() ZDebugStopf(0, ("Unimplemented routine"))

// There are still quite a lot of places where plain old ZAssert is used.
#define ZAssert(a) ZAssertStop(1, a)

// =================================================================================================
// These defines could go into a private header, but I'd rather not add yet
// another file. Perhaps later.

#define ZDebug_Message_AssertionAndUser "Assertion failed: %s. %s, %s:%d"
#define ZDebug_Message_AssertionOnly "Assertion failed: %s, %s:%d"
#define ZDebug_Message_UserOnly "%s, %s:%d"
#define ZDebug_Message_None "%s:%d"

#endif // __ZDebug__
