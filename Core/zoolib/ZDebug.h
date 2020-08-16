// Copyright (c) 2000 Andrew Green and Learning in Motion, Inc.
// MIT License. http://www.zoolib.org

#ifndef __ZDebug_h__
#define __ZDebug_h__ 1
#include "zconfig.h"

#include "zoolib/ZMACRO_FileFunctionLine.h"

#include "zoolib/size_t.h" // For size_t

#include <cstdarg> // For std::va_list

#ifdef __clang_analyzer__
	#include <assert.h>
#endif

// =================================================================================================
#pragma mark - ZDebug

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

size_t sFormatStandardMessage(char* iBuf, int iBufSize, const Params_t& iParams);

#ifndef ZMACRO_Debug
	#ifdef __clang_analyzer__
		#define ZMACRO_Debug(level, stop, FFL, ...) \
			assert(not stop)
	#else
		#define ZMACRO_Debug(level, stop, FFL, ...) \
			do { if (level <= ZCONFIG_Debug) \
				ZooLib::ZDebug::sInvoke(level, stop, FFL, 0, __VA_ARGS__); } while (0)
	#endif
#endif

#ifndef ZMACRO_Assert
	#ifdef __clang_analyzer__
		#define ZMACRO_Assert(level, stop, condition, FFL, ...) \
			assert(condition || not stop)
	#else
		#define ZMACRO_Assert(level, stop, condition, FFL, ...) \
			do { if (level <= ZCONFIG_Debug && !(condition)) \
				ZooLib::ZDebug::sInvoke(level, stop, FFL, #condition, __VA_ARGS__); } while (0)
	#endif
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

#ifndef ZAssertCompile
	#define ZAssertCompile(a) \
		typedef ZooLib::ZDebug::AssertCompile<(a)>::IsValid ZAssertCompileValid
#endif

// There are still quite a lot of places where plain old ZAssert is used.
#ifndef ZAssert
	#define ZAssert(a) ZAssertStop(1, a)
#endif

#ifndef ZEnsure
	#define ZEnsure(a) ZAssertStop(0, a)
#endif

} // namespace ZDebug
} // namespace ZooLib

// =================================================================================================
#pragma mark - ZUnimplemented

namespace ZooLib {

ZMACRO_NoReturn_Pre void ZUnimplemented() ZMACRO_NoReturn_Post;

} // namespace ZooLib

#endif // __ZDebug_h__
