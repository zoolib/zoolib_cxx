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

#include "ZDebug.h"
#include "ZCONFIG_SPI.h"

#include <cstdarg>
#include <cstdio>
#include <cstring> // For strrchr

#if ZCONFIG(Compiler, MSVC)
#	define vsnprintf _vsnprintf
#	define snprintf _snprintf
#else
//	using std::vsnprintf;
//	using std::snprintf;
#endif

static const size_t sStringBufferSize = 1024;

const char* ZDebug_FormatMessage(const char* inMessage, ...)
	{
	static char sDebugMessageString[sStringBufferSize];
	va_list args;
	va_start(args, inMessage);
	/*int count =*/ vsnprintf(sDebugMessageString, sStringBufferSize, inMessage, args);
	va_end(args);
	return sDebugMessageString;
	}

void ZDebug_DisplayMessageSimple(const char* inMessage, ...)
	{
	char buffer[sStringBufferSize];
	va_list args;
	va_start(args, inMessage);
	/*int count =*/ vsnprintf(buffer, sStringBufferSize, inMessage, args);
	va_end(args);
	fputs(buffer, stderr);
	}

static const char* sTruncateFileName(const char* inFilename)
	{
	if (const char* truncatedFileName = strrchr(inFilename, '/'))
		return truncatedFileName + 1;
	return inFilename;
	}

static void ZDebug_HandleInitial_Generic(int inLevel, ZDebug_Action inAction,
	const char* inFilename, int inLine, const char* inAssertionMessage, const char* inUserMessage)
	{
	if (sDebug_HandleActual)
		{
		static const size_t sStringBufferSize = 2048;
	
		char realDebugMessageString[sStringBufferSize];
	
		if (inAssertionMessage)
			{
			if (inUserMessage)
				{
				::snprintf(realDebugMessageString, sStringBufferSize,
					ZDebug_Message_AssertionAndUser"\n",
					inAssertionMessage,
					inUserMessage,
					sTruncateFileName(inFilename),
					inLine);
				}
			else
				{
				::snprintf(realDebugMessageString, sStringBufferSize,
					ZDebug_Message_AssertionOnly"\n",
					inAssertionMessage,
					sTruncateFileName(inFilename),
					inLine);
				}
			}
		else
			{
			if (inUserMessage)
				{
				::snprintf(realDebugMessageString, sStringBufferSize,
					ZDebug_Message_UserOnly"\n",
					inUserMessage,
					sTruncateFileName(inFilename),
					inLine);
				}
			else
				{
				::snprintf(realDebugMessageString, sStringBufferSize,
					ZDebug_Message_None"\n",
					sTruncateFileName(inFilename),
					inLine);
				}
			}
		sDebug_HandleActual(inLevel, inAction, realDebugMessageString);
		}
	}

ZDebug_HandleInitial_t sDebug_HandleInitial = ZDebug_HandleInitial_Generic;

// =================================================================================================

#if ZCONFIG_SPI_Enabled(POSIX)

#include <cstdlib> // For abort

static void ZDebug_HandleActual_POSIX(int inLevel, ZDebug_Action inAction, const char* inMessage)
	{
	::fputs(inMessage, stderr);

	if (inAction == eDebug_ActionStop)
		{
		// Force a segfault
		*reinterpret_cast<double*>(1) = 0;
//		abort();
		}
	}

ZDebug_HandleActual_t sDebug_HandleActual = ZDebug_HandleActual_POSIX;

#endif // ZCONFIG_SPI_Enabled(POSIX)

// =================================================================================================

#if ZCONFIG_SPI_Enabled(BeOS)

#include <kernel/OS.h>

static void ZDebug_HandleActual_Be(int inLevel, ZDebug_Action inAction, const char* inMessage)
	{
	if (inAction == eDebug_ActionStop)
		::debugger(inMessage);
	else
		::fputs(inMessage, stderr);
	}

ZDebug_HandleActual_t sDebug_HandleActual = ZDebug_HandleActual_Be;

#endif // ZCONFIG_SPI_Enabled(BeOS)

// =================================================================================================
