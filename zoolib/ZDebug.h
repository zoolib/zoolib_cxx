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

#ifndef __ZDebug_h__
#define __ZDebug_h__ 1
#include "zconfig.h"

#include "zoolib/ZMACRO_FileFunctionLine.h"

#include <cstdarg> // For std::va_list
#include <cstddef> // For std::size_t

// =================================================================================================
// MARK: - ZDebug

namespace ZooLib {
namespace ZDebug {

struct Params_t
	{
	int fLevel;
	bool fStop;
	ZFileFunctionLine fFFL;
	const char* fConditionMessage;
	const char* fUserMessage;
	};

typedef void (*Function_t)(const Params_t& iParams, std::va_list iArgs);

extern void sInvoke(int iLevel, bool iStop,
	const ZFileFunctionLine& iFFL,
	const char* iConditionMessage, const char* iUserMessage, ...);

std::size_t sFormatStandardMessage(char* iBuf, int iBufSize, const Params_t& iParams);

#ifndef ZMACRO_Debug
	#define ZMACRO_Debug(level, stop, FFL, ...) \
		do { if (level <= ZCONFIG_Debug) \
			ZooLib::ZDebug::sInvoke(level, stop, FFL, 0, __VA_ARGS__); } while (0)
#endif

#ifndef ZMACRO_Assert
	#define ZMACRO_Assert(level, stop, condition, FFL, ...) \
		do { if (level <= ZCONFIG_Debug && !(condition)) \
			ZooLib::ZDebug::sInvoke(level, stop, FFL, #condition, __VA_ARGS__); } while (0)
#endif


#ifndef ZDebugLog
	#define ZDebugLog(level) \
		ZMACRO_Debug(level, false, ZMACRO_FileFunctionLine, 0)
#endif

#ifndef ZDebugLogf
	#define ZDebugLogf(level, ...) \
		ZMACRO_Debug(level, false, ZMACRO_FileFunctionLine, __VA_ARGS__)
#endif

#ifndef ZDebugStop
	#define ZDebugStop(level) \
		ZMACRO_Debug(level, true, ZMACRO_FileFunctionLine, 0)
#endif

#ifndef ZDebugStopf
	#define ZDebugStopf(level, ...) \
		ZMACRO_Debug(level, true, ZMACRO_FileFunctionLine, __VA_ARGS__)
#endif

#ifndef ZAssertLog
	#define ZAssertLog(level, condition) \
		ZMACRO_Assert(level, false, condition, ZMACRO_FileFunctionLine, 0)
#endif

#ifndef ZAssertLogf
	#define ZAssertLogf(level, condition, ...) \
		ZMACRO_Assert(level, false, condition, ZMACRO_FileFunctionLine, __VA_ARGS__)
#endif

#ifndef ZAssertStop
	#define ZAssertStop(level, condition) \
		ZMACRO_Assert(level, true, condition, ZMACRO_FileFunctionLine, 0)
#endif

#ifndef ZAssertStopf
	#define ZAssertStopf(level, condition, ...) \
		ZMACRO_Assert(level, true, condition, ZMACRO_FileFunctionLine, __VA_ARGS__)
#endif

// ZAssertCompile can be used to enforce a constraint at compile time, (for example that a
// struct obeys necessary alignment rules). It either drops out completely or generates an
// error, depending on whether the expression evaulates true or false.
template <bool> struct AssertCompile {};
template <> struct AssertCompile<true> { typedef bool IsValid; };

#define ZAssertCompile(a) typedef ZooLib::ZDebug::AssertCompile<(a)>::IsValid ZAssertCompileValid

// There are still quite a lot of places where plain old ZAssert is used.
#ifndef ZAssert
	#define ZAssert(a) ZAssertStop(1, a)
#endif

} // namespace ZDebug
} // namespace ZooLib

// =================================================================================================
// MARK: - ZUnimplemented

namespace ZooLib {

ZMACRO_NoReturn_Pre void ZUnimplemented() ZMACRO_NoReturn_Post;

} // namespace ZooLib

#endif // __ZDebug_h__
